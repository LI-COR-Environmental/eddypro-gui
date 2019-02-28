/***************************************************************************
  ancillaryfiletest.cpp
  -------------------
  Copyright (C) 2014-2018, LI-COR Biosciences
  Author: Antonio Forgione

  This file is part of EddyPro (R).

  EddyPro (R) is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  EddyPro (R) is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with EddyPro (R). If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "ancillaryfiletest.h"

#include <QDateTime>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QTextBrowser>
#include <QPushButton>
#include <QSaveFile>
#include <QVBoxLayout>

#include <algorithm>

#include "stringutils.h"
#include "container_helpers.h"
#include "widget_utils.h"

const auto helpPage = QStringLiteral("http://www.licor.com/env/help/eddypro/topics/assessment-tests.html");

AncillaryFileTest::AncillaryFileTest(FileType type,
                                     QWidget *parent) :
    QDialog(parent),
    type_(type)
{
    setVisible(false);

    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Assessment file test results"));
    WidgetUtils::removeContextHelpButton(this);

    testResults_ = new QTextBrowser(this);
    testResults_->setReadOnly(true);
    testResults_->setMinimumWidth(800);

    // neccesary to avoid following the question mark link as a document link
    // the connection with QTextBrowser::anchorClicked will provide the
    // expected behavior
    testResults_->setOpenLinks(false);

    auto cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    cancelButton->setDefault(true);
    cancelButton->setProperty("commonButton", true);

    auto continueButton = new QPushButton(tr("Continue"));
    continueButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    continueButton->setDefault(true);
    continueButton->setProperty("commonButton", true);

    auto saveButton = new QPushButton(tr("Save to file"));
    saveButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    saveButton->setDefault(true);
    saveButton->setProperty("commonButton", true);

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(continueButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);
    buttonBox->addButton(saveButton, QDialogButtonBox::ActionRole);

    auto dialogLayout = new QVBoxLayout(this);
    dialogLayout->addWidget(testResults_);
    dialogLayout->addWidget(buttonBox, 0, Qt::AlignCenter);
    setLayout(dialogLayout);

    connect(cancelButton, &QPushButton::clicked,
            [=](){ this->close(); this->setResult(QDialog::Rejected); });
    connect(continueButton, &QPushButton::clicked,
            [=](){ this->close(); this->setResult(QDialog::Accepted); });
    connect(saveButton, &QPushButton::clicked, [=](){ this->saveResults(); });

    connect(testResults_, &QTextBrowser::anchorClicked,
            [=](const QUrl& link){ WidgetUtils::showHelp(link); });
}

void AncillaryFileTest::refresh(const QString &file)
{
    name_ = file;
    actualLines_.clear();
    testResults_->clear();
    setVisible(false);
}

QString AncillaryFileTest::formatPassFail(bool test_result)
{
    return (test_result ? QStringLiteral("<font color=\"#0066FF\">pass</font>")
                        : QStringLiteral("<font color=\"#FF3300\">fail</font>"));
}

bool AncillaryFileTest::makeTest()
{
    auto result = testFile();
    if (!result)
    {
        setVisible(true);
        testResults_->moveCursor(QTextCursor::End);
    }
    return result;
}

bool AncillaryFileTest::testFile()
{
    bool parseResult = false;
    bool formalResult = false;
    bool scientificResult = false;

    const auto parseErrorStr_1 =
            tr("<b>The formatting and content of the selected file "
               "could not be assessed due to missing "
               "template files. Please, re-install the software.</b>");

    const auto parseErrorStr_2 =
            tr("<b>Unable to open the selected file or the file "
               "is empty. Please, select another file.</b>");

    // if not already read the template file
    if (templateLines_.isEmpty())
    {
        // test presence of the template file and read it
        parseResult
            = parseFile(testFileMap_.value(type_).filepath, &templateLines_);
        if (!parseResult)
        {
            testResults_->append(parseErrorStr_1);
            return false;
        }
    }

    parseResult = parseFile(name_, &actualLines_);
    if (!parseResult)
    {
        testResults_->append(parseErrorStr_2);
        return false;
    }

    testResults_->append(QLatin1String("<b>FORMAT test</b>"));
    formalResult =
            (this->*testFileMap_.value(type_).formalTest)(templateLines_,
                                                          actualLines_);
    const auto formalErrorStr =
            tr("<b>FORMAT test <font color=\"#FF3300\">failed</font>.</b><br />");
    const auto formalSuccessStr =
            tr("<b>FORMAT test <font color=\"#0066FF\">passed</font>.</b><br />");
    const auto finalErrorStr =
            tr("<b>The selected file does not match the expected "
               "formatting or scientific content. "
               "<p>If you would like to upload a different file or choose an alternate method, please click <i>Cancel</i>. "
               "If you click <i>Continue</i>, EddyPro will probably not use the file and will resort to the default method.</p>"
               "<p>More information about the testing performed "
               "can be found in the help.</b>&nbsp;"
               "<a href=\"%1\"><img src=\"qrc:/icons/qm-enabled\"></img></a>").arg(helpPage);

    if (!formalResult)
    {
        testResults_->append(formalErrorStr);
        testResults_->append(finalErrorStr);
    }
    else
    {
        testResults_->append(formalSuccessStr);
        testResults_->insertHtml(QStringLiteral("<br>"));

        testResults_->append(tr("<b>SCIENTIFIC test</b>"));
        scientificResult =
                (this->*testFileMap_.value(type_).scientificTest)(actualLines_);
        const auto scientificErrorStr =
                tr("<b>SCIENTIFIC test <font color=\"#FF3300\">failed</font>.</b><br />");

        if (!scientificResult)
        {
            testResults_->append(scientificErrorStr);
            testResults_->append(finalErrorStr);
        }
    }

    return (formalResult && scientificResult);
}

bool AncillaryFileTest::parseFile(const QString& filename, LineList *lines)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "parseFile error: file open" << filename;
        return false;
    }

    QTextStream in(&file);
    QString line;

    line = in.readLine();
    if (line.isNull()) { return false; }

    const auto space = QLatin1Char(' ');
    *lines << line.split(space, QString::SkipEmptyParts);
    while (!line.isNull())
    {
        line = in.readLine();
        *lines << line.split(space, QString::SkipEmptyParts);
    }

    file.close();
    return true;
}

bool AncillaryFileTest::testSpectraF(const LineList& templateList, const LineList& actualList)
{
    // test total number of rows
    auto rowCountTest = (actualList.size() == templateList.size());
    testResults_->append(QLatin1String("Number of rows [")
                  + QString::number(actualList.size())
                  + QStringLiteral("]: ")
                  + formatPassFail(rowCountTest));
    if (!rowCountTest) { return false; }

    // other tests
    QList<bool> test;
    auto last_test = [&](){ return test.value(test.size() - 1); };

    // test header, rows 1-7
    test << ContainerHelper::rangeEqual(templateList, actualList, 0, 7);
    testResults_->append(QLatin1String("Header, rows 1-7: ")
                                  + formatPassFail(last_test()));

    // test water vapour TFP labels, rows 8-16
    for (auto i = 7; i < 16; ++i)
    {
        test << (StringUtils::subStringList(templateList.value(i), 0, 6)
                == StringUtils::subStringList(actualList.value(i), 0, 6));
        testResults_->append(QLatin1String("<u>H<sub>2</sub>O</u> TFP label, row ")
                                      + QString::number(i + 1)
                                      + QStringLiteral(": ")
                                      + formatPassFail(last_test()));
    }

    // test header rows 17-18
    test << ContainerHelper::rangeEqual(templateList, actualList, 16, 18);
    testResults_->append(QLatin1String("Header rows 17-18: ")
                                  + formatPassFail(last_test()));

    // test CO2 TFP labels, rows 19-30
    for (auto i = 18; i < 30; ++i)
    {
        test << (StringUtils::subStringList(templateList.value(i), 0, 2)
                == StringUtils::subStringList(actualList.value(i), 0, 2));

        testResults_->append(QLatin1String("<u>CO<sub>2</sub></u> TFP label, row ")
                                      + QString::number(i + 1)
                                      + QStringLiteral(": ")
                                      + formatPassFail(last_test()));
    }

    // test header row 31-32
    test << ContainerHelper::rangeEqual(templateList, actualList, 30, 32);
    testResults_->append(QLatin1String("Header rows 31-32: ")
                                  + formatPassFail(last_test()));

    // test CH4 TFP labels, rows 33-44
    for (auto i = 32; i < 44; ++i)
    {
        test << (StringUtils::subStringList(templateList.value(i), 0, 2)
                == StringUtils::subStringList(actualList.value(i), 0, 2));

        testResults_->append(QLatin1String("<u>CH<sub>4</sub></u> TFP label, row ")
                                      + QString::number(i + 1)
                                      + QStringLiteral(": ")
                                      + formatPassFail(last_test()));
    }

    // test header row 45-46
    test << ContainerHelper::rangeEqual(templateList, actualList, 44, 46);
    testResults_->append(QLatin1String("Header rows 45-46: ")
                                  + formatPassFail(last_test()));

    // test other gas TFP labels, rows 19-30
    for (auto i = 46; i < 58; ++i)
    {
        test << (StringUtils::subStringList(templateList.value(i), 0, 2)
                == StringUtils::subStringList(actualList.value(i), 0, 2));

        testResults_->append(QLatin1String("<u>Other gas</u> TFP label, row ")
                                      + QString::number(i + 1)
                                      + QStringLiteral(": ")
                                      + formatPassFail(last_test()));
    }

    // test header, rows 59-62
    test << ContainerHelper::rangeEqual(templateList, actualList, 58, 62);
    testResults_->append(QLatin1String("Header, rows 59-62: ")
                                  + formatPassFail(last_test()));

    // test header, rows 64-69
    test << ContainerHelper::rangeEqual(templateList, actualList, 63, 69);
    testResults_->append(QLatin1String("Header, rows 64-69: ")
                                  + formatPassFail(last_test()));

    // test high pass parameters labels, rows 70-71
    for (auto i = 69; i < 71; ++i)
    {
        test << (StringUtils::subStringList(templateList.value(i), 0, 2)
                == StringUtils::subStringList(actualList.value(i), 0, 2));

        testResults_->append(QLatin1String("HP correction parameters label, row ")
                                      + QString::number(i + 1)
                                      + QStringLiteral(": ")
                                      + formatPassFail(last_test()));
    }

    auto res = true;
    for (auto i = 0; i < test.size(); ++i)
    {
        res &= test.value(i);
    }

    return res;
}

bool AncillaryFileTest::testSpectraS(const LineList &actualList)
{
    // get numbers
    QVector<double> FnH2o;
    QVector<double> fcH2o;
    QVector<double> numerosity;
    for (auto i = 7; i < 16; ++i)
    {
        FnH2o << StringUtils::subStringList(actualList.value(i), 6, 7).value(0).toDouble();
        fcH2o << StringUtils::subStringList(actualList.value(i), 7, 8).value(0).toDouble();
        numerosity << StringUtils::subStringList(actualList.value(i), 8, 9).value(0).toDouble();
    }
    QVector<double> FnCo2;
    QVector<double> fcCo2;
    for (auto i = 18; i < 30; ++i)
    {
        FnCo2 << StringUtils::subStringList(actualList.value(i), 2, 3).value(0).toDouble();
        fcCo2 << StringUtils::subStringList(actualList.value(i), 3, 4).value(0).toDouble();
    }
    QVector<double> FnCh4;
    QVector<double> fcCh4;
    for (auto i = 32; i < 44; ++i)
    {
        FnCh4 << StringUtils::subStringList(actualList.value(i), 2, 3).value(0).toDouble();
        fcCh4 << StringUtils::subStringList(actualList.value(i), 3, 4).value(0).toDouble();
    }
    QVector<double> fitParameters;
    for (auto i = 0; i < 3; ++i)
    {
        fitParameters << actualList.value(62).value(i).toDouble();
    }
    QVector<double> modelParameters;
    modelParameters << actualList.value(69).value(2).toDouble();
    modelParameters << actualList.value(69).value(3).toDouble();
    modelParameters << actualList.value(70).value(2).toDouble();
    modelParameters << actualList.value(70).value(3).toDouble();

    // test criteria
    QList<bool> test;
    auto last_test_index = [&](){ return (test.size() - 1); };
    auto last_test = [&](){ return test.value(test.size() - 1); };

    // test a.1
    test << std::any_of(fcH2o.begin(), fcH2o.end(),
                        [](double d){ return (d >= 0.001 && d <= 10.0); });
    auto a1_label = QStringLiteral("<u>H<sub>2</sub>O</u> Column 'fc' "
                                   "shall have at least 1 value in the range [0.001; 10.0]: ");
    testResults_->append(a1_label + formatPassFail(last_test()));

    // test a.2
    test << std::any_of(fcH2o.begin(), fcH2o.end(),
                        [](double d){ return !qFuzzyCompare(d, -9999.0); });
    auto a2_label = QStringLiteral("<u>H<sub>2</sub>O</u> Column 'fc' "
                                   "shall not have all values set to -9999: ");
    testResults_->append(a2_label + formatPassFail(last_test()));

    // test a.3
    test << std::any_of(numerosity.begin(), numerosity.end(),
                        [](int i){ return (i > 0); });
    auto a3_label = QStringLiteral("<u>H<sub>2</sub>O</u> Column 'numerosity'' "
                                   "shall have at least 1 value > 0: ");
    testResults_->append(a3_label + formatPassFail(last_test()));

    // test a.4
    test << true;
    for (auto i = 0; i < 10; ++i)
    {
        if (fcH2o.value(i) >= 0.001 && fcH2o.value(i) <= 10.0)
        {
            if (FnH2o.value(i) < 0.01 || FnH2o.value(i) > 10.0)
            {
                test.replace(last_test_index(), false);
                break;
            }
        }
    }
    auto a4_label = QStringLiteral("<u>H<sub>2</sub>O</u> Column 'Fn' shall be "
                                   "in the range [0.01; 10.0] for good values of column 'fc': ");
    testResults_->append(a4_label + formatPassFail(last_test()));

    // test b.1
    test << std::all_of(fitParameters.begin(), fitParameters.end(),
                [](double d){ return !qFuzzyCompare(d, -9999.0); });
    auto b1_label = QStringLiteral("<u>H<sub>2</sub>O</u> All spectral corrections RH/fc "
                                      "exponential fit parameters shall be != -9999.0: ");
    testResults_->append(b1_label + formatPassFail(last_test()));

    // test c.2
    test << std::all_of(fcCo2.begin(), fcCo2.end(),
                        [](double d){ return (d >= 0.001 && d <= 10.0); });
    auto c2_label = QStringLiteral("<u>CO<sub>2</sub></u> All column 'fc' values "
                                   "shall be in the range [0.001; 10.0]: ");
    testResults_->append(c2_label + formatPassFail(last_test()));

    // test c.3
//    if (last_test())
//    {
//        test << std::all_of(FnCo2.begin(), FnCo2.end(),
//                            [](double d){ return (d >= 0.01 && d <= 10.0); });
//    }

//    test << std::equal(fcCo2.begin(), fcCo2.end(), FnCo2.begin(),
//                       [](double d1, double d2){});

    test << true;
    for (auto i = 0; i < 12; ++i)
    {
        if (fcCo2.value(i) >= 0.001 && fcCo2.value(i) <= 10.0)
        {
            if (FnCo2.value(i) < 0.01 || FnCo2.value(i) > 10.0)
            {
                test.replace(last_test_index(), false);
                break;
            }
        }
    }
    auto c3_label = QStringLiteral("<u>CO<sub>2</sub></u> All column 'Fn' shall "
                                   "be in the range [0.01; 10.0] for good values of column 'fc': ");
    testResults_->append(c3_label + formatPassFail(last_test()));

    // test d.2
    test << std::all_of(fcCh4.begin(), fcCh4.end(),
                        [](double d){ return (d >= 0.001 && d <= 10); });
    auto d2_label = QStringLiteral("<u>CH<sub>4</sub></u> All column 'fc' values "
                                   "shall be in the range [0.001; 10.0]: ");
    testResults_->append(d2_label + formatPassFail(last_test()));

    // test d.3
    test << true;
    for (auto i = 0; i < 12; ++i)
    {
        if (fcCh4.value(i) >= 0.001 && fcCh4.value(i) <= 10.0)
        {
            if (FnCh4.value(i) < 0.01 || FnCh4.value(i) > 10.0)
            {
                test.replace(last_test_index(), false);
                break;
            }
        }
    }
    auto d3_label = QStringLiteral("<u>CH<sub>4</sub></u> All column 'Fn' "
                                   "shall be in the range [0.01; 10.0] for good values of column 'fc': ");
    testResults_->append(d3_label + formatPassFail(last_test()));

    // test e.1
    test << std::all_of(modelParameters.begin(), modelParameters.end(),
                        [](double d){ return (d >= 0.0 && d <= 1.0); });
    auto e1_label = QStringLiteral("<u>H<sub>2</sub>O or CO<sub>2</sub> or CH<sub>4</sub></u> "
                                   "All high-pass correction factor model parameters "
                                   "shall be within the range [0; 1]: ");
    testResults_->append(e1_label + formatPassFail(last_test()));

    auto res = true;
    for (auto i = 0; i < test.size(); ++i)
    {
        res &= test.value(i);
    }

    return res;
}

bool AncillaryFileTest::testPlanarFitF(const LineList &templateList, const LineList &actualList)
{
    // preliminary test, number of rows
    auto rowCountTest = (actualList.size() > 2);
    testResults_->append(QLatin1String("Number of rows [")
                                 + QString::number(actualList.size())
                                 + QStringLiteral("]: ")
                                 + formatPassFail(rowCountTest));
    if (!rowCountTest) { return false; }

    // other tests
    QList<bool> test;
    auto last_test_index = [&](){ return (test.size() - 1); };
    auto last_test = [&](){ return test.value(test.size() - 1); };

    // test a, header rows 1-7
    for (auto i = 0; i < 6; ++i)
    {
        test << (StringUtils::subStringList(templateList.value(i), 0, 1)
                == StringUtils::subStringList(actualList.value(i), 0, 1));
        testResults_->append(QLatin1String("Header, row ")
                             + QString::number(i + 1)
                             + QStringLiteral(": ")
                             + formatPassFail(last_test()));
    }

    // test a, header rows 8-10
    test << ContainerHelper::rangeEqual(templateList, actualList, 7, 10);
    testResults_->append(QLatin1String("Header, rows 8-10: ")
                         + formatPassFail(last_test()));

    // wind sectors > 0
    auto windSectorsStr = StringUtils::subStringList(actualList.value(1), 1, 2).value(0);
    auto windSectors = windSectorsStr.toInt();
    test << (windSectors > 0);
    testResults_->append(QLatin1String("Wind sectors [")
                         + QString::number(windSectors)
                         + QStringLiteral("]: ")
                         + formatPassFail(last_test()));
    if (!last_test()) { return false; }

    // test e, total number of rows (depending from wind sectors)
    rowCountTest = ((5 * windSectors + 13) == actualList.size());
    testResults_->append(QLatin1String("Total number of rows [")
                         + QString::number(actualList.size())
                         + QStringLiteral("]: ")
                         + formatPassFail(rowCountTest));
    if (!rowCountTest) { return false; }

    // test b, rows 11-14 formal test
    test << true;
    for (auto i = 0; i < windSectors; ++i)
    {
        // column 1
        if (actualList.value(10 + i).value(0).toInt() != i + 1)
        {
            test.replace(last_test_index(), false);
            break;
        }

        // columns 3-5
        auto conversionToDouble = false;
        for (auto j = 3; j < 6; ++j)
        {
            actualList.value(10 + i).value(j).toDouble(&conversionToDouble);

            if (!conversionToDouble)
            {
                test.replace(last_test_index(), false);
                break;
            }
        }
    }
    testResults_->append(QLatin1String("Wind sectors coefficients formal structure: ")
                         + formatPassFail(last_test()));

    // test c, header rows (11-12 + windSectors)
    test << std::equal(templateList.begin() + 10 + 4, templateList.begin() + 10 + 6,
                       actualList.begin() + 10 + windSectors);
    testResults_->append(QLatin1String("Header, rows ")
                         + QString::number(11 + windSectors)
                         + QStringLiteral("-")
                         + QString::number(12 + windSectors)
                         + QStringLiteral(": ")
                         + formatPassFail(last_test()));

    // test d1
    test << true;
    for (auto i = 0; i < windSectors; ++i)
    {
        if (StringUtils::subStringList(templateList.value(16), 0, 2)
            != StringUtils::subStringList(actualList.value(12 + windSectors + 4 * i), 0, 2))
        {
            test.replace(last_test_index(), false);
            break;
        }
        if (StringUtils::subStringList(templateList.value(16), 3, 4)
            != StringUtils::subStringList(actualList.value(12 + windSectors + 4 * i), 3, 4))
        {
            test.replace(last_test_index(), false);
            break;
        }
        if (StringUtils::subStringList(templateList.value(16), 7, 9)
            != StringUtils::subStringList(actualList.value(12 + windSectors + 4 * i), 7, 9))
        {
            test.replace(last_test_index(), false);
            break;
        }
    }
    testResults_->append(QLatin1String("Rotation matrices formal structure 1: ")
                         + formatPassFail(last_test()));

    // test d2
    test << true;
    for (auto i = 0; i < windSectors; ++i)
    {
        // columns 1-3
        auto conversionToDouble = false;
        for (auto j = 0; j < 3; ++j)
        {
            for (auto k = 0; k < 3; ++k)
            {
                actualList.value(13 + j + windSectors + 4 * i).value(k).toDouble(&conversionToDouble);

                if (!conversionToDouble)
                {
                    test.replace(last_test_index(), false);
                    break;
                }
            }
        }
    }
    testResults_->append(QLatin1String("Rotation matrices formal structure 2: ")
                         + formatPassFail(last_test()));

    // test d3
    test << true;
    for (auto i = 0; i < windSectors; ++i)
    {
        if (actualList.value(14 + windSectors + 4 * i).value(0).toDouble() != 0.0)
        {
            test.replace(last_test_index(), false);
            break;
        }
    }
    testResults_->append(QLatin1String("Rotation matrices formal structure 3: ")
                         + formatPassFail(last_test()));

    auto res = true;
    for (auto i = 0; i < test.size(); ++i)
    {
        res &= test.value(i);
    }

    return res;
}

bool AncillaryFileTest::testPlanarFitS(const LineList &actualList)
{
    auto windSectorsStr = StringUtils::subStringList(actualList.value(1), 1, 2).value(0);
    auto windSectors = windSectorsStr.toInt();

    // QGenericMatrix
    QVector<QVector<double>> fitParameters(windSectors);
    for (auto i = 0; i < windSectors; ++i)
    {
        fitParameters[i].resize(3);
        fitParameters[i][0] = actualList.value(10 + i).value(3).toDouble();
        fitParameters[i][1] = actualList.value(10 + i).value(4).toDouble();
        fitParameters[i][2] = actualList.value(10 + i).value(5).toDouble();
    }

    // QMatrix3x3
    QList<QVector<QVector<double>>> rotMatrices;
    rotMatrices.reserve(windSectors);
    for (int i = 0; i < windSectors; ++i)
    {
        QVector<QVector<double>> matrix(3);
        for (int j = 0; j < 3; ++j)
        {
            matrix[j].resize(3);
            matrix[j][0] = actualList.value(13 + j + windSectors + 4 * i).value(0).toDouble();
            matrix[j][1] = actualList.value(13 + j + windSectors + 4 * i).value(1).toDouble();
            matrix[j][2] = actualList.value(13 + j + windSectors + 4 * i).value(2).toDouble();
        }
        rotMatrices << matrix;
    }

    auto test_full = false;
    QList<bool> test_detail;
    test_detail << false << true << false;

    for (auto i = 0; i < windSectors; ++i)
    {
        // init test results
        test_detail.replace(0, false);
        test_detail.replace(1, true);
        test_detail.replace(2, false);

        // test a
//        if (std::all_of(fitParameters[i].begin(), fitParameters[i].end()),
//                            [](double d){ return (d != -9999.0); })
        if (!qFuzzyCompare(fitParameters[i][0], -9999.0)
            && !qFuzzyCompare(fitParameters[i][1], -9999.0)
            && !qFuzzyCompare(fitParameters[i][2], -9999.0))
        {
            test_full = true;
            test_detail.replace(0, true);

            // test b.1
            for (auto j = 0; j < 3; ++j)
            {
                for (auto k = 0; k < 3; ++k)
                {
                    if (qFuzzyCompare(rotMatrices.value(i)[j][k], -9999.0))
                    {
                        test_full = false;
                        test_detail.replace(1, false);

                        // not leaving the outermost for loop
                        // is not efficient, but acceptable
                        break;
                    }
                }
            }
//            if (!std::all_of(rotMatrices.value(i).begin(), rotMatrices.value(i).end(),
//                                [](double d){ return (d != -9999.0); }))
//            {
//                test.replace(last_test_index(), false);
//                break;
//            }

            // test b.2
            for (auto j = 0; j < 3; ++j)
            {
                for (auto k = 0; k < 3; ++k)
                {
                    if (rotMatrices.value(i)[j][k] != 0.0)
                    {
                        test_full = test_full && true;
                        test_detail.replace(2, true);
                        // not leaving the outermost for loop
                        // is not efficient, but acceptable
                        break;
                    }
                }
            }
//            if (!std::any_of(rotMatrices.value(i).begin(), rotMatrices.value(i).end(),
//                                [](double d){ return (d != 0.0); }))
//            {
//                test.replace(last_test_index(), false);
//                break;
//            }
        }

        // print results
        auto wind_sector_test = std::all_of(test_detail.begin(), test_detail.end(),
                                            [](bool res){ return (res); });
        testResults_->append(QLatin1String("<u>Wind sector ")
                             + QString::number(i + 1)
                             + QStringLiteral("</u>: ")
                             + formatPassFail(wind_sector_test));
        if (!wind_sector_test)
        {
            if (!test_detail.value(0))
            {
                testResults_->append(QLatin1String("At least one wind sector "
                                     "should have all three coefficients "
                                     "!= -9999.0: ") + formatPassFail(false));
            }
            else
            {
                if (!test_detail.value(1))
                {
                    testResults_->append(QLatin1String("A wind sector having valid coefficients "
                                         "shall have all rotations values "
                                         "!= -9999.0") + formatPassFail(false));
                }
                if (!test_detail.value(2))
                {
                    testResults_->append(QLatin1String("A wind sector having valid coefficients "
                                         "shall have at least one rotation value "
                                         "!= 0.0: ") + formatPassFail(false));
                }
            }
        }
        else
        {
            break;
        }
    }

    return test_full;
}

bool AncillaryFileTest::testTimeLagF(const LineList &templateList, const LineList &actualList)
{
    // preliminary test, number of rows
    auto rowCountTest = (actualList.size() > 2);
    testResults_->append(QLatin1String("Number of rows [")
                                 + QString::number(actualList.size())
                                 + QStringLiteral("]: ")
                                 + formatPassFail(rowCountTest));
    if (!rowCountTest) { return false; }

    QList<bool> test;
    auto last_test = [&](){ return test.value(test.size() - 1); };

    // test a
    for (auto i = 0; i < 5; ++i)
    {
        test << (templateList.value(i).value(0) == actualList.value(i).value(0));
        testResults_->append(QLatin1String("Header, row ")
                             + QString::number(i + 1)
                             + QStringLiteral(": ")
                             + formatPassFail(last_test()));
    }

    // test b
    auto gasCount = 0;
    timelagValues.resize(3);
    while (actualList.value(5 + 5 * gasCount).value(0).contains(QStringLiteral("Number_of_timelags_used_for_"))
           && actualList.value(6 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(0) == QLatin1String("Median")
           && actualList.value(6 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(2) == QLatin1String("timelag")
           && actualList.value(6 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(3) == QLatin1String("[s]:")
           && actualList.value(7 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(0) == QLatin1String("Mimimum")
           && actualList.value(7 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(2) == QLatin1String("timelag")
           && actualList.value(7 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(3) == QLatin1String("[s]:")
           && actualList.value(8 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(0) == QLatin1String("Maximum")
           && actualList.value(8 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(2) == QLatin1String("timelag")
           && actualList.value(8 + 5 * gasCount).value(0)
                        .split(QStringLiteral("_")).value(3) == QLatin1String("[s]:"))
    {
        ++gasCount;

        // collect values
        timelagValues[0].resize(gasCount);
        timelagValues[1].resize(gasCount);
        timelagValues[2].resize(gasCount);
        timelagValues[0][gasCount - 1] = actualList.value(6 + 5 * (gasCount - 1)).value(1).toDouble();
        timelagValues[1][gasCount - 1] = actualList.value(7 + 5 * (gasCount - 1)).value(1).toDouble();
        timelagValues[2][gasCount - 1] = actualList.value(8 + 5 * (gasCount - 1)).value(1).toDouble();
    }

    // test c1
    // compare 3 lines of RH headers
    if (std::equal(templateList.begin() + 15, templateList.begin() + 18,
                   actualList.begin() + (5 + 5 * gasCount)))
    {
        test << true;
        testResults_->append(QLatin1String("Header of RH sorted H<sub>2</sub>O classes (3 rows): ")
                             + formatPassFail(last_test()));

        // test c1' (moved from scientific to formal)
        auto rhClassCount = 0;

        while (!actualList.value(8 + 5 * gasCount + rhClassCount).isEmpty())
        {
            ++rhClassCount;
            auto actualRhlClassIndex = actualList.value(8 + 5 * gasCount + rhClassCount - 1).value(0).toInt();
            test << (rhClassCount == actualRhlClassIndex);
            testResults_->append(QLatin1String("Consistent RH index [")
                                 + QString::number(actualRhlClassIndex)
                                 + QStringLiteral("]: ")
                                 + formatPassFail(last_test()));
        }

        // test c2
        if (rhClassCount <= 20)
        {
            test << (actualList.value(8 + 5 * gasCount).value(1) == QLatin1String("0")
                     && actualList.value(8 + 5 * gasCount + rhClassCount - 1).value(3) == QLatin1String("100%"));

            // collect values
            h2oTimelagValues.resize(4);
            h2oTimelagValues[0].resize(rhClassCount);
            h2oTimelagValues[1].resize(rhClassCount);
            h2oTimelagValues[2].resize(rhClassCount);
            h2oTimelagValues[3].resize(rhClassCount);
            for (auto i = 0; i < rhClassCount; ++i)
            {
                h2oTimelagValues[0][i] = actualList.value(8 + 5 * gasCount + i).value(4).toDouble();
                h2oTimelagValues[1][i] = actualList.value(8 + 5 * gasCount + i).value(5).toDouble();
                h2oTimelagValues[2][i] = actualList.value(8 + 5 * gasCount + i).value(6).toDouble();
                h2oTimelagValues[3][i] = actualList.value(8 + 5 * gasCount + i).value(7).toDouble();
            }

            testResults_->append(QStringLiteral("Consistent RH ranges: ") + formatPassFail(last_test()));
        }
        else
        {
            test << false;
            testResults_->append(QLatin1String("RH classes <= 20: ") + formatPassFail(last_test()));
        }
    }
    else
    {
        auto rhIsEmpty = actualList.value(5 + 5 * gasCount).isEmpty()
                         && actualList.value(6 + 5 * gasCount).isEmpty()
                         && actualList.value(7 + 5 * gasCount).isEmpty();

        // with no gases and no rh classes
        if (!gasCount && rhIsEmpty)
        {
            test << false;
            testResults_->append(QLatin1String("Number of gases [0] and header of "
                                                "RH sorted H<sub>2</sub>O classes (3 rows): ")
                          + formatPassFail(last_test()));
        }
        // with no gases and > 20 rh classes
        else if (!gasCount && !rhIsEmpty)
        {
            test << false;
            testResults_->append(QLatin1String("Header of RH sorted H<sub>2</sub>O classes (3 rows): ")
                          + formatPassFail(last_test()));
        }
        // with gases and > 20 rh classes
        else if (gasCount && !rhIsEmpty)
        {
            test << false;
            testResults_->append(QLatin1String("Header of gases or RH sorted H<sub>2</sub>O classes (3 rows): ")
                          + formatPassFail(last_test()));
        }
        // with gases and no rh classes
        else if (gasCount && rhIsEmpty)
        {
            test << true;
            testResults_->append(QStringLiteral("Number of gases [0]: ")
                          + formatPassFail(last_test()));
        }
    }

    auto res = true;
    for (auto i = 0; i < test.size(); ++i)
    {
        res &= test.value(i);
    }
    return res;
}

bool AncillaryFileTest::testTimeLagS(const LineList &actualList)
{
    Q_UNUSED(actualList);

    QList<bool> test;
    auto last_test_index = [&](){ return (test.size() - 1); };
    auto last_test = [&](){ return test.value(test.size() - 1); };
    test << true;

    // test a
    auto gasCount = timelagValues[0].size();
    if (gasCount > 0)
    {
        for (auto j = 0; j < gasCount; ++j)
        {
            if (!((timelagValues[0][j] >= timelagValues[1][j])
                && timelagValues[0][j] <= timelagValues[2][j]))
            {
                test.replace(last_test_index(), false);
            }
        }
        testResults_->append(QLatin1String("Gas time-lag median values inside the "
                             "[minimum; maximum] range: ")
                             + formatPassFail(last_test()));

        // test b
        test << true;
        for (auto i = 0; i < 3; ++i)
        {
            for (auto j = 0; j < gasCount; ++j)
            {
                if (timelagValues[i][j] > 60.0)
                {
                    test.replace(last_test_index(), false);
                    goto end_loop;
                }
            }
        }
        end_loop:
        testResults_->append(QLatin1String("Time-lag values not larger than 60 seconds: ")
                             + formatPassFail(last_test()));
    }

    // if there are RH classes
    if (!h2oTimelagValues.isEmpty())
    {
        // test c.2
        test << true;
        auto rhClassCount = h2oTimelagValues[0].size();
        for (auto i = 0; i < rhClassCount; ++i)
        {
            if (!((h2oTimelagValues[0][i] >= h2oTimelagValues[1][i])
                  && (h2oTimelagValues[0][i] <= h2oTimelagValues[2][i])))
            {
                test.replace(last_test_index(), false);
                break;
            }
        }
        testResults_->append(QStringLiteral("H<sub>2</sub>O RH-sorted median values inside the "
                             "[minimum; maximum] range: ")
                             + formatPassFail(last_test()));

        // test c.3
        test << false;
        auto classNumCount = 0;
        for (auto i = 0; i < rhClassCount; ++i)
        {
            if (h2oTimelagValues[3][i] > 30)
            {
                ++classNumCount;
            }
            if (classNumCount >= 3)
            {
                test.replace(last_test_index(), true);
                break;
            }
        }
        testResults_->append(QStringLiteral("At least 3 H<sub>2</sub>O classes with numerosity > 30: ")
                             + formatPassFail(last_test()));
    }

    auto res = true;
    for (auto i = 0; i < test.size(); ++i)
    {
        res &= test.value(i);
    }
    return res;
}

QString AncillaryFileTest::typeToString(FileType type)
{
    switch (type)
    {
    case FileType::Spectra:
        return QStringLiteral("spectral-assessment-file-check");
    case FileType::PlanarFit:
        return QStringLiteral("planar-fit-assessment-file-check");
    case FileType::TimeLag:
        return QStringLiteral("time-lag-assessment-file-check");
    }
    return QString();
}

// TODO: Use sheet on Mac with getSaveFileName
void AncillaryFileTest::saveResults()
{
    auto timestamp = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-ddThhmmss"));
    auto filenameHint =
            WidgetUtils::getSearchPathHint()
            + QStringLiteral("/")
            + typeToString(type_)
            + QStringLiteral("-")
            + timestamp
            + Defs::TEMPLATE_FILE_EXT;
    auto filename = QFileDialog::getSaveFileName(this,
                                         tr("Save the test results as..."),
                                         filenameHint,
                                         tr("%1 assessment file check results (*.txt);;All files (*)").arg(Defs::APP_NAME));

    if (!filename.isEmpty())
    {
        QSaveFile file(filename);
        file.open(QIODevice::WriteOnly | QIODevice::Text);

        QTextStream out(&file);

        // add header
        out << tr("%1 check of assessment file %2 against %3")
               .arg(Defs::APP_NAME, name_, testFileMap_.value(type_).filepath);
        out << "\n\n";

        // get text
        auto text = testResults_->toPlainText();

        // remove question mark
        text.chop(1);

        // write text
        out << text;

        // add online help address
        out << "\nSee ";
        out << helpPage;
        out << ".\n";

        // flush data to file
        file.commit();
    }
}
