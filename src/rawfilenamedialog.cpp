/***************************************************************************
  rawfilenamedialog.cpp
  -------------------
  Copyright (C) 2011-2015, LI-COR Biosciences
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

#include "rawfilenamedialog.h"

#include <QButtonGroup>
#include <QDebug>
#include <QDialogButtonBox>
#include <QDirIterator>
#include <QLabel>
#include <QLineEdit>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>

#include "dbghelper.h"
#include "ecproject.h"
#include "widget_utils.h"

RawFilenameDialog::RawFilenameDialog(QWidget *parent,
                                     EcProject *ecProject,
                                     QStringList* suffixList,
                                     QStringList* rawFileList) :
    QDialog(parent),
    ecProject_(ecProject),
    ghgSuffixList_(suffixList),
    rawFileList_(rawFileList)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("Raw File Name Format"));
    WidgetUtils::removeContextHelpButton(this);

    title = new QLabel(tr("Raw file name format"));
    title->setProperty("groupTitle", true);

    desc = new QLabel;

    radioGroupBoxLayout = new QVBoxLayout;
    radioGroupBox = new QFrame;
    radioGroupBox->setProperty("scrollContainerWidget", true);
    radioGroupBox->setLayout(radioGroupBoxLayout);

    auto radioScrollArea = new QScrollArea;
    radioScrollArea->setWidget(radioGroupBox);
    radioScrollArea->setWidgetResizable(true);
    radioScrollArea->setMinimumWidth(radioGroupBox->sizeHint().width());

    extRadioGroup = new QButtonGroup(this);

    rawFilenameFormatEdit = new QLineEdit;

    okButton = new QPushButton(tr("&Ok"));
    okButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    okButton->setDefault(true);
    okButton->setProperty("commonButton", true);
    okButton->setEnabled(false);

    auto cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    cancelButton->setDefault(true);
    cancelButton->setProperty("commonButton", true);

    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(okButton, QDialogButtonBox::AcceptRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

    auto dialogLayout = new QGridLayout(this);
    dialogLayout->addWidget(title, 0, 0);
    dialogLayout->addWidget(desc, 1, 0);
    dialogLayout->addWidget(radioScrollArea, 2, 0);
    dialogLayout->addWidget(rawFilenameFormatEdit, 3, 0);
    dialogLayout->addWidget(buttonBox, 4, 0, 1, 1, Qt::AlignCenter);
    dialogLayout->setVerticalSpacing(10);
    dialogLayout->setContentsMargins(30, 30, 30, 30);
    setLayout(dialogLayout);

    connect(okButton, &QPushButton::clicked,
            this, &RawFilenameDialog::accept);
    connect(cancelButton, &QPushButton::clicked,
            this, &RawFilenameDialog::reject);
    connect(extRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateFormatEdit(int)));
    connect(rawFilenameFormatEdit, &QLineEdit::textEdited,
            this, &RawFilenameDialog::updateFileList);
}

void RawFilenameDialog::accept()
{
    emit updateFileFormatRequest(rawFilenameFormatEdit->text());
    if (isVisible())
    {
        hide();
    }
}

void RawFilenameDialog::reset()
{
    fileList_.clear();

    radioGroupBox->setVisible(false);
    rawFilenameFormatEdit->clear();

    // clean the radiogroup
    foreach (QAbstractButton* button, extRadioGroup->buttons())
    {
        extRadioGroup->removeButton(button);
    }

    // remove the objects
    // the first object is the layout...
    QObjectList buttonList = radioGroupBox->children();
    qDeleteAll(buttonList.begin() + 1, buttonList.end());
}

void RawFilenameDialog::refresh()
{
    // clean the dialog
    reset();

    populateDialog();

    // recover existing prototype if possible
    auto existingPrototype = ecProject_->generalFilePrototype();
    if (!existingPrototype.isEmpty())
    {
        rawFilenameFormatEdit->setText(existingPrototype);
    }

    // select corresponding radio button if possible
    auto buttonsNumber = extRadioGroup->buttons().count();
    qDebug() << "buttons #" << buttonsNumber;
    if (buttonsNumber)
    {
        auto selectionIndex = 0;
        foreach (const auto btn, extRadioGroup->buttons())
        {
            // stop if found
            // 1. GHG case
            if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
            {
                if (rawFilenameFormatEdit->text().contains(btn->text()))
                {
                    break;
                }
            }
            // 2. non GHG case
            else
            {
                if (rawFilenameFormatEdit->text().section(QLatin1Char('.'), -1).contains(btn->text()))
                {
                    break;
                }
            }
            ++selectionIndex;
        }
        qDebug() << "last selectionIndex" << selectionIndex;
        // process all button
        if (selectionIndex == extRadioGroup->buttons().length())
        {
            extRadioGroup->buttons().last()->toggle();
        }
        // standard buttons
        else
        {
            extRadioGroup->buttons().at(selectionIndex)->toggle();
        }
    }

    // update the ok button
    updateOkButton();
}

void RawFilenameDialog::populateDialog()
{
    DEBUG_FUNC_NAME

    QString baseDescText
            = tr("<p>Your entry in this field will describe "
                 "how the timestamp is encoded in the raw file name.<br />"
                 "In the field below, replace the:"
                 "<ul><li>Year with characters yy or yyyy</li>"
                 "<li>Month with mm</li>"
                 "<li>Day with dd (day of month) or ddd (day of year) (case sensitive)</li>"
                 "<li>Hour and minute with HH and MM</li></ul></p>"
                 "<p>You can also specify a wildcard matching any single character "
                 "using the question mark (?).</p>"
                 "<u>Example</u><br />"
                 "<table><tr><td>File name: </td><td><tt>2015-05-27_1030_mysite-12.raw</tt></td></tr>"
                 "<tr><td>File name format: </td><td><tt>yyyy-mm-dd_HHMM_mysite-??.raw</tt></td></tr>"
                 "</table></p><hr />");

    // GHG case
    if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
    {
        qDebug() << "ghgSuffixList_" << *ghgSuffixList_ << ghgSuffixList_;
        qDebug() << "ghgSuffixList_ count" << ghgSuffixList_->count();

        if (ghgSuffixList_->count() > 1)
        {
            desc->setText(baseDescText +
                          tr("<p>In the 'Raw data directory' we found GHG file names with the "
                             "following suffixes:</p>"));

            createGhgSuffixRadioButtons();
        }
        else if (ghgSuffixList_->count() == 1)
        {
            desc->setText(baseDescText +
                          tr("\nIn the 'Raw data directory' we found GHG file names with the "
                             "following suffix:"));
            createGhgSuffixRadioButtons();
        }
        else
        {
            desc->setText(baseDescText +
                          tr("\nIn the 'Raw data directory' we found no GHG files."));
        }

        // update prototype line edit
        updateFormatEdit(0);
    }
    // non GHG cases
    else
    {
        fileList_ = getRawFileTypesAvailable();
        qDebug() << "fileList_" << fileList_;

        // filter fileList by selected extension for known cases
        // notably SLT and TOB1
        if (ecProject_->generalFileType() == Defs::RawFileType::SLT1
            || ecProject_->generalFileType() == Defs::RawFileType::SLT2)
        {
            removeFileExtensionFromList(Defs::TOB1_NATIVE_DATA_FILE_EXT);
        }
        else if (ecProject_->generalFileType() == Defs::RawFileType::TOB1)
        {
            removeFileExtensionFromList(Defs::SLT_NATIVE_DATA_FILE_EXT);
        }
        else if (ecProject_->generalFileType() == Defs::RawFileType::ASCII
                 || ecProject_->generalFileType() == Defs::RawFileType::BIN)
        {
            removeFileExtensionFromList(Defs::SLT_NATIVE_DATA_FILE_EXT);
            removeFileExtensionFromList(Defs::TOB1_NATIVE_DATA_FILE_EXT);
        }
        // in all the cases remove GHG
        removeFileExtensionFromList(Defs::GHG_NATIVE_DATA_FILE_EXT);

        qDebug() << "filtered fileList_" << fileList_;

        if (fileList_.count() > 1)
        {
            desc->setText(baseDescText +
                          tr("<p>In the 'Raw data directory' we found file names with the "
                             "following extensions:</p>"));

            createFileExtensionRadioButtons(fileList_);
        }
        else if (fileList_.count() == 1)
        {
            desc->setText(baseDescText +
                          tr("\nIn the 'Raw data directory' we found file names of the "
                             "following type:"));
            createFileExtensionRadioButtons(fileList_);
        }
        else
        {
            desc->setText(baseDescText +
                          tr("\nIn the 'Raw data directory' we found no files."));
        }

        // update prototype line edit
        updateFormatEdit(0);
    }
}

void RawFilenameDialog::removeFileExtensionFromList(const QString& ext)
{
    foreach (auto type, fileList_)
    {
        if (QFileInfo(type).suffix().contains(ext))
        {
            fileList_.removeOne(type);
        }
    }
}

QStringList RawFilenameDialog::getRawFileTypesAvailable()
{
    DEBUG_FUNC_NAME

    // initial test
    if (ecProject_->screenDataPath().isEmpty())
    {
        qDebug() << "no raw data dir set";
        return QStringList();
    }

    // set the flag according to the current raw data directory flag
    QDirIterator::IteratorFlags recursionFlag = QDirIterator::Subdirectories;
    if (!ecProject_->screenRecurse())
    {
        recursionFlag = QDirIterator::NoIteratorFlags;
    }

    QDirIterator dirIterator(ecProject_->screenDataPath(),
                     QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                     recursionFlag);

    QString currFile;
    QStringList currentFileList;
    QString currExt;
    QStringList currentExtensionList;
    QFileInfo currFileInfo;
    while (dirIterator.hasNext())
    {
        dirIterator.next();

        currFileInfo = dirIterator.fileInfo();
        currFile = currFileInfo.absoluteFilePath();
        currExt = currFileInfo.suffix();

        if (!currentExtensionList.contains(currExt))
        {
            currentFileList.append(currFile);
            currentExtensionList.append(currExt);
        }
    }
    return currentFileList;
}

void RawFilenameDialog::createGhgSuffixRadioButtons()
{
    auto firstSuffixLenght = ghgSuffixList_->first().length();
    qDebug() << "firstSuffixLenght" << firstSuffixLenght << ghgSuffixList_->first();

    auto i = 0;
    auto equalLengthSuffix = true;
    QRadioButton* button;
    foreach (const auto& suffix, *ghgSuffixList_)
    {
        button = new QRadioButton(suffix);
        extRadioGroup->addButton(button, i);
        radioGroupBoxLayout->addWidget(button);
        ++i;
        if (equalLengthSuffix)
        {
            equalLengthSuffix = (suffix.length() == firstSuffixLenght);
            qDebug() << "suffix.length()" << suffix << suffix.length();
        }
    }

    // add button in case of equal length suffixes
    if (equalLengthSuffix && ghgSuffixList_->size() > 1)
    {
        button = new QRadioButton(tr("Process all"));
        extRadioGroup->addButton(button, i);
        radioGroupBoxLayout->addWidget(button);
    }

    extRadioGroup->buttons().first()->setChecked(true);
    radioGroupBox->setVisible(true);
}

void RawFilenameDialog::createFileExtensionRadioButtons(const QStringList& list)
{
    auto i = 0;
    foreach (const QString& file, list)
    {
        qDebug() << QFileInfo(file).suffix();
        auto button = new QRadioButton(QFileInfo(file).suffix());
        extRadioGroup->addButton(button, i);
        radioGroupBoxLayout->addWidget(button);
        ++i;
    }
    extRadioGroup->buttons().first()->setChecked(true);
    radioGroupBox->setVisible(true);
}

// update prototype line edit
void RawFilenameDialog::updateFormatEdit(int id)
{
    if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
    {
        if (ghgSuffixList_->isEmpty())
        {
            return;
        }

        // standard buttons
        if (id < ghgSuffixList_->length())
        {
            rawFilenameFormatEdit->setText(Defs::GHG_TIMESTAMP_FORMAT + ghgSuffixList_->at(id));
            rawFilenameFormatEdit->selectAll();
        }
        // 'catch all' button ('Process all')
        else
        {
            QString filledSuffix;
            filledSuffix.fill(QLatin1Char('?'), ghgSuffixList_->first().length() - 4);
            rawFilenameFormatEdit->setText(Defs::GHG_TIMESTAMP_FORMAT
                                           + filledSuffix
                                           + QLatin1Char('.')
                                           + Defs::GHG_NATIVE_DATA_FILE_EXT);
            rawFilenameFormatEdit->selectAll();
        }
    }
    else
    {
        if (fileList_.isEmpty())
        {
            return;
        }

        QFileInfo fileInfo(fileList_.at(id));

        rawFilenameFormatEdit->setText(fileInfo.fileName());
        rawFilenameFormatEdit->selectAll();
    }
    updateOkButton();
}

// update the current list when the user edit the prototype
// to keep in sync the changes and to avoid losing them
// switching between different radio buttons
// of course if the list changes, any synchronism would
// be lost
// in the GHG case the list is created and updated by
// the parent class, no effort is made to keep it in sync
// locally but local changes are not preserved
void RawFilenameDialog::updateFileList(const QString& file)
{
    DEBUG_FUNC_NAME

    if (ecProject_->generalFileType() == Defs::RawFileType::GHG)
    {
        if (ghgSuffixList_->isEmpty())
        {
            return;
        }
    }
    else
    {
        if (fileList_.isEmpty())
        {
            return;
        }

        qDebug() << extRadioGroup->buttons().count()
                 << extRadioGroup->checkedId()
                 << file;
        if (extRadioGroup->buttons().count())
        {
            fileList_.replace(extRadioGroup->checkedId(), file);
        }
    }
    updateOkButton();
}

void RawFilenameDialog::updateOkButton()
{
    okButton->setEnabled(ecProject_->isGoodRawFilePrototype(rawFilenameFormatEdit->text()));
}
