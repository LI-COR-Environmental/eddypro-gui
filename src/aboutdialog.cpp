/***************************************************************************
  aboutdialog.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2016, LI-COR Biosciences
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

#include "aboutdialog.h"

#include <QDebug>
#include <QLabel>
#include <QFile>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

#include "defs.h"
#include "widget_utils.h"

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
#if defined(Q_OS_WIN)
    resize(530, 620);
    setMinimumSize(530, 620);
#elif defined(Q_OS_MAC)
    resize(660, 690);
    setMinimumSize(660, 690);
#endif

    setWindowModality(Qt::WindowModal);
    QString titleText = tr("About %1").arg(Defs::APP_NAME);
    setWindowTitle(titleText);
    WidgetUtils::removeContextHelpButton(this);

    auto introduction = new QLabel;
    introduction->setText(
        tr("<h2>%1<sup>&reg;</sup> v%2-%3</h2>"
           "<h6>Built on %4 at %5<br />With %7<br /></h6>"
           ).arg(Defs::APP_NAME,
            Defs::APP_VERSION_STR,
            Defs::APP_STAGE_STR,
            QStringLiteral(__DATE__),
            QStringLiteral(__TIME__))
        #if defined(Q_OS_WIN)
            .arg(Defs::WIN_COMPILER)
        #elif defined(Q_OS_MAC)
            .arg(Defs::MAC_COMPILER)
        #endif
        );
    auto icon = new QLabel;
    auto app_logo_2x = QPixmap(QStringLiteral(":/icons/app-logo-about"));
#if defined(Q_OS_MAC)
    app_logo_2x.setDevicePixelRatio(2.0);
#endif
    icon->setPixmap(app_logo_2x);

    // About information
    auto infoWidget = new QWidget;
    auto infoLabel = new QLabel;
    infoLabel-> setText(
        tr("<br />%1 is an open source software application that is developed, "
           "maintained, supported by LI-COR Biosciences. It originates from "
           "ECO2S, the Eddy COvariance COmmunity Software project, which was "
           "developed as part of the IMECC-EU research project.</p>"
           "<p>We gratefully acknowledge the IMECC consortium, the ECO2S "
           "development team, the University of Tuscia (Italy) and scientists "
           "around the world who assisted with development and testing of the "
           "original version of this software."
           "<p>Copyright &copy; 2011-%2 LI-COR Inc.</p>"
           "<div>Contact LI-COR Inc.:</div><br />"
           "<div style=\"text-indent: 20px;\">4647 Superior Street</div>"
           "<div style=\"text-indent: 20px;\">P.O. Box 4000</div>"
           "<div style=\"text-indent: 20px;\">Lincoln, Nebraska, 68504, USA</div><br />"
           "<div style=\"text-indent: 20px;\">Phone: 1-402-467-3576</div>"
           "<div style=\"text-indent: 20px;\">Toll Free: 800-447-3576</div>"
           "<div style=\"text-indent: 20px;\">Fax: 1-402-467-2819</div>"
           "<div style=\"text-indent: 20px;\">Email: <a href=\"mailto:envsupport@licor.com?subject=EddyPro %3\">envsupport@licor.com</a></div>"
           "<div style=\"text-indent: 20px;\">Website: <a href=\"http://www.licor.com\">http://www.licor.com</a></div>"
           ).arg(Defs::APP_NAME, Defs::CURRENT_COPYRIGHT_YEAR, Defs::APP_VERSION_STR)
        );
    infoLabel->setOpenExternalLinks(true);
    infoLabel->setWordWrap(true);

    auto infoLayout = new QVBoxLayout;
    infoLayout->addWidget(infoLabel);
    infoLayout->addStretch();
    infoWidget->setLayout(infoLayout);

    // Thanks
    auto thanksWidget = new QWidget;
    auto thanksLabel = new QLabel;
    thanksLabel->setText(
        tr("<br />We would like to thank the whole "
            "Eddy Covariance community, the authors, testers, the users and the "
            "following people (and the missing ones), in no special "
            "order, for their collaboration and source code contribution "
            "to create this free software." ));
    thanksLabel->setWordWrap(true);

    auto thanksEdit = new QTextEdit;
    thanksEdit->setText(
        tr("<h4>Original Authors</h4>"
           "<ul type=\"square\">"
           "<li>Gerardo Fratini (gerardo.fratini@licor.com): processing engines designer and developer</li>"
           "<li>Antonio Forgione (antonio.forgione@licor.com): GUI designer and developer</li>"
           "<li>Dario Papale (darpap@unitus.it): project manager and coordinator</li>"
           "</ul>"

           "<h4>Others contributors</h4>"
           "<ul type=\"square\">"
           "<li>Carlo Trotta: code harmonization and documentation</li>"
           "<li>Natascha Kljun: code for footprint estimation, Kljun et al. (2004, BLM)</li>"
           "<li>Taro Nakai: code for angle of attack correction, Nakai et al. (2006, AFM)</li>"
           "<li>Andreas Ibrom: supervision during implementation of a spectral correction procedure, Ibrom et al. (2007, AFM)</li>"
           "<li>Stephen Chan: Revision, refinement and testing of implementation of Massman 2000/2001 spectral correction.</li>"
           "</ul>"

           "<h4>Software validation (intercomparison)</h4>"
           "<ul type=\"square\">"
           "<li>Juha-Pekka Tuovinen</li>"
           "<li>Andreas Ibrom</li>"
           "<li>Ivan Mammarella</li>"
           "<li>Robert Clement</li>"
           "<li>Meelis Molder</li>"
           "<li>Olaf Kolle</li>"
           "<li>Corinna Rebmann</li>"
           "<li>Matthias Mauder</li>"
           "<li>Jan Elbers</li>"
           "</ul>"

           "<h4>User testing and bug notifications</h4>"
           "<ul type=\"square\">"
           "<li>Tarek El-Madany</li>"
           "<li>Sergiy Medinets</li>"
           "<li>Beniamino Gioli</li>"
           "<li>Nicola Arriga</li>"
           "<li>Luca Belelli</li>"
           "<li>Michal Heliasz</li>"
           "<li>Bernard Heinesch</li>"
           "<li>Arnaud Carrara</li>"
           "<li>Patrik Vestin</li>"
           "<li>Matthias Barthel</li>"
           "<li>Karoline Wischnewski</li>"
           "<li>Matthew Wilkinson</li>"
           "<li>Simone Sabbatini</li>"
           "</ul>"

           "<h4>Software discussions</h4>"
           "<ul type=\"square\">"
           "<li>Ian Elbers</li>"
           "<li>George Burba</li>"
           "<li>Christian Wille</li>"
           "</ul>"

           "<h4>Libraries</h4>"
           "<ul type=\"square\">"
           "<li>Arjan van Dijk: libdate module</li>"
           "<li>Michael Baudin, Arjen Markus: m_logging module</li>"
           "<li>University of Chicago: m_levenberg_marquardt from the MINPACK package</li>"
           "<li>netlib.org: FFT routines from the SLATEC Common Mathematical Library</li>"
           "<li>The Qt Company: Qt framework</li>"
           "<li>Boost::math</li>"
           "<li>Trenton Schulz (Trolltech AS): Fader widget</li>"
           "<li>Morgan Leborgne: QProgressIndicator widget</li>"
           "<li>Witold Wysota: Debug helper class</li>"
           "<li>Sergey A. Tachenov: QuaZIP</li>"
           "<li>Mark Summerfield: classes from the book 'Advanced Qt Programming'</li>"
           "</ul>"

           "<h4>Tools</h4>"
           "<ul type=\"square\">"
           "<li>GFortran compiler</li>"
           "<li>MinGW compiler and GDB debugger</li>"
           "<li>Clang compiler</li>"
           "<li>The Qt Company: Qt Creator IDE</li>"
           "<li>Code::Blocks IDE</li>"
           "<li>\n</li>"
           "</ul>"));
    thanksEdit->setReadOnly(true);

    auto thanksLayout = new QVBoxLayout;
    thanksLayout->addWidget(thanksLabel);
    thanksLayout->addWidget(thanksEdit);
    thanksWidget->setLayout(thanksLayout);

    // License
    auto licenseWidget = new QWidget;
    auto licenseLabel = new QLabel;
    licenseLabel->setText(
        tr("<br />The %1 software application is Copyright &copy; 2011-%2 "
           "LI-COR Inc.\n\n"
           "You may use, distribute and copy the %1 programs suite under "
           "the terms of the GNU General Public License version 3, "
           "which is displayed below. If you would like to obtain "
           "a copy of the source package please contact LI-COR "
           "Biosciences at "
           "<a href=\"mailto:envsupport@licor.com?subject=%1 %3&body="
           "Please, send me a copy of the source package."
           "\">envsupport@licor.com</a>."
        ).arg(Defs::APP_NAME, Defs::CURRENT_COPYRIGHT_YEAR, Defs::APP_VERSION_STR));
    licenseLabel->setWordWrap(true);
    licenseLabel->setOpenExternalLinks(true);

    auto licenseEdit = new QTextEdit;
    QFile licenseFile(QStringLiteral(":/docs/license"));
    qDebug() << licenseFile.open(QIODevice::ReadOnly | QIODevice::Text);
    licenseEdit->setText(QLatin1String(licenseFile.readAll()));
    licenseEdit->setReadOnly(true);
    licenseFile.close();

    auto licenseLayout = new QVBoxLayout;
    licenseLayout->addWidget(licenseLabel);
    licenseLayout->addWidget(licenseEdit);
    licenseWidget->setLayout(licenseLayout);

    // Changelog
    auto changelogWidget = new QWidget;
    auto changelogLabel = new QLabel;
    changelogLabel->setText(
        tr("<br />Software updates include bug fixes, usability "
           "improvements\n\n and feature enhancements. These are summarized "
           "in the change log below."));
    changelogLabel->setWordWrap(true);
    changelogLabel->setOpenExternalLinks(true);

    auto changelogEdit = new QTextEdit;
    QFile changelogFile(QStringLiteral(":/docs/changelog"));
    qDebug() << changelogFile.open(QIODevice::ReadOnly | QIODevice::Text);
    changelogEdit->setText(QLatin1String(changelogFile.readAll()));
    changelogEdit->setReadOnly(true);
    changelogFile.close();

    auto changelogLayout = new QVBoxLayout;
    changelogLayout->addWidget(changelogLabel);
    changelogLayout->addWidget(changelogEdit);
    changelogWidget->setLayout(changelogLayout);

    // Dialog Tabs
    auto tab = new QTabWidget;
    tab->addTab(infoWidget, tr("About"));
    tab->addTab(thanksWidget, tr("Acknowledgments"));
    tab->addTab(licenseWidget, tr("License"));
    tab->addTab(changelogWidget, tr("Changes"));

    auto okButton = WidgetUtils::createCommonButton(this, tr("Ok"));

    auto dialogLayout = new QVBoxLayout(this);
    dialogLayout->addWidget(icon, 0, Qt::AlignCenter);
    dialogLayout->addWidget(introduction, 0, Qt::AlignCenter);
    dialogLayout->addWidget(tab);
    dialogLayout->addWidget(okButton, 0, Qt::AlignCenter);
    dialogLayout->setContentsMargins(30, 30, 30, 30);
    setLayout(dialogLayout);

    connect(okButton, &QPushButton::clicked,
            [=](){ if (this->isVisible()) hide(); });
}

AboutDialog::~AboutDialog()
{
    qDebug() << Q_FUNC_INFO;
}
