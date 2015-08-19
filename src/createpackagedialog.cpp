/***************************************************************************
  createpackagedialog.cpp
  -------------------
  Copyright (C) 2013-2014, LI-COR Biosciences
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
 ***************************************************************************/

#include "createpackagedialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include <QwwButtonLineEdit/QwwButtonLineEdit>
#include <QwwClearLineEdit/QwwClearLineEdit>

#include "clicklabel.h"
#include "configstate.h"
#include "dbghelper.h"
#include "ecproject.h"
#include "fileutils.h"
#include "globalsettings.h"
#include "widget_utils.h"

CreatePackageDialog::CreatePackageDialog(EcProject *ecProject,
                                         ConfigState* config,
                                         QWidget *parent) :
    QDialog(parent),
    ecProject_(ecProject),
    configState_(config)
{
    setWindowModality(Qt::WindowModal);
    setWindowTitle(tr("SmartFLux Package Creation"));
    WidgetUtils::removeContextHelpButton(this);

    auto groupTitle = new QLabel;
    groupTitle->setText(tr("Choose a name and a location for "
                           "your .smartflux bundle. <br>This file "
                           "will need to be uploaded to the "
                           "SMARTFlux device, <br>so choose a file "
                           "path that is easy to remember.<br>"
                           "The .smartflux extension will "
                           "be automatically appended if needed."));

    auto hrLabel = new QLabel;
    hrLabel->setObjectName(QStringLiteral("hrLabel"));

    filenameLabel = new ClickLabel(tr("Package name :"));
    filenameLabel->setToolTip(tr("<b>Package name:</b> Enter the Package name. Note that characters that result in file names that are unacceptable to the commonest operating systems (this includes | \\ / : ; ? * ' \" < > CR LF TAB SPACE and other non readable characters) are not permitted."));

    // prevent filesystem's illegal characters and whitespace insertion:
    // exclude the first 33 (from 0 to 32) ASCII chars, including
    // '\0'(NUL),'\a'(BEL),'\b'(BS),'\t'(TAB),'\n'(LF),'\v'(VT),'\f'(FF),'\r'(CR) and ' '(SPACE)
    // plus the following:
    // '|', '\', '/', ':', ';', '?', '*', '"', ''', '`', '<', '>'
    QString filenameRegexp = QStringLiteral("[^\\000-\\040|\\\\/:;\\?\\*\"'`<>]+");

    filenameEdit = new QwwClearLineEdit;
    filenameEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    filenameEdit->setToolTip(filenameLabel->toolTip());
    filenameEdit->setMaxLength(200);
    filenameEdit->setRegExp(filenameRegexp);
#if defined(Q_OS_WIN)
    filenameEdit->setMaximumWidth(400);
#elif defined(Q_OS_MAC)
    filenameEdit->setMaximumWidth(395);
#endif

    outpathLabel = new ClickLabel(tr("Save package to :"), this);
    outpathLabel->setToolTip(tr("<b>Save package to:</b> Specify where the package file will be stored. Click the <i>Browse...</i> button and navigate to the desired directory."));
    outpathEdit = new QwwButtonLineEdit;
    outpathEdit->setIcon(QIcon(QStringLiteral(":/icons/clear-line")));
    outpathEdit->setButtonVisible(false);
    outpathEdit->setButtonPosition(QwwButtonLineEdit::RightInside);
    outpathEdit->installEventFilter(const_cast<CreatePackageDialog*>(this));
    outpathEdit->setReadOnly(true);
    outpathEdit->setProperty("asButtonLineEdit", true);
    outpathEdit->setMinimumWidth(400);
    outpathBrowse = new QPushButton(tr("Browse..."));
    outpathBrowse->setProperty("loadButton", true);
    outpathBrowse->setToolTip(outpathLabel->toolTip());

    auto outpathContainerLayout = new QHBoxLayout;
    outpathContainerLayout->addWidget(outpathEdit);
    outpathContainerLayout->addWidget(outpathBrowse);
    outpathContainerLayout->setStretch(2, 1);
    outpathContainerLayout->setContentsMargins(0, 0, 0, 0);
    outpathContainerLayout->setSpacing(0);
    auto outpathContainer = new QWidget;
    outpathContainer->setLayout(outpathContainerLayout);

    createButton = new QPushButton(tr("Create"));
//    createButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    createButton->setDefault(true);
    createButton->setAutoDefault(true);
    createButton->setProperty("commonButton", true);

    cancelButton = new QPushButton(tr("Cancel"));
//    cancelButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    cancelButton->setProperty("commonButton", true);

    auto propertiesLayout = new QGridLayout;
    propertiesLayout->addWidget(filenameLabel, 0, 0);
    propertiesLayout->addWidget(filenameEdit, 0, 1);
    propertiesLayout->addWidget(outpathLabel, 1, 0);
    propertiesLayout->addWidget(outpathContainer, 1, 1);
    propertiesLayout->setVerticalSpacing(3);
    propertiesLayout->setRowMinimumHeight(1, 10);
    propertiesLayout->setContentsMargins(3, 3, 3, 3);

    auto propertiesFrame = new QWidget;
    propertiesFrame->setLayout(propertiesLayout);

    auto buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(createButton);
    buttonsLayout->addSpacing(20);
    buttonsLayout->addWidget(cancelButton);

    auto mainLayout = new QGridLayout(this);
    mainLayout->addWidget(groupTitle, 0, 0, Qt::AlignCenter);
    mainLayout->addWidget(hrLabel, 1, 0);
    mainLayout->addWidget(propertiesFrame, 2, 0);
    mainLayout->addLayout(buttonsLayout, 3, 0);
    mainLayout->setVerticalSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setRowMinimumHeight(3, 40);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);

    connect(filenameLabel, &ClickLabel::clicked,
            this, &CreatePackageDialog::onFilenameLabelClicked);
    connect(filenameEdit, &QwwClearLineEdit::textChanged,
            this, &CreatePackageDialog::updateFilename);
    connect(filenameEdit, &QwwClearLineEdit::buttonClicked,
            this, &CreatePackageDialog::clearFilenameEdit);

    connect(outpathEdit, &QwwButtonLineEdit::buttonClicked,
            this, &CreatePackageDialog::clearOutpathEdit);
    connect(outpathEdit, &QwwButtonLineEdit::textChanged,
            this, &CreatePackageDialog::updateOutpath);
    connect(outpathBrowse, &QPushButton::clicked,
            this, &CreatePackageDialog::outpathBrowse_clicked);

    connect(createButton, &QPushButton::clicked,
            this, &CreatePackageDialog::createPackageRequest);

    connect(cancelButton, &QPushButton::clicked,
            this, &CreatePackageDialog::close);

    refresh();
}

void CreatePackageDialog::refresh()
{
    QString lastFilename = GlobalSettings::getAppPersistentSettings(
                                            Defs::CONFGROUP_PROJECT,
                                            Defs::CONF_PROJ_SMARTFLUX_FILENAME,
                                            QString()).toString();

    QString lastOutpath = GlobalSettings::getAppPersistentSettings(
                                            Defs::CONFGROUP_PROJECT,
                                            Defs::CONF_PROJ_SMARTFLUX_FILEPATH,
                                            QString()).toString();

    filenameEdit->setText(lastFilename);
    outpathEdit->setText(lastOutpath);

    refreshButtonStatus();
}

void CreatePackageDialog::showResult(bool ok, const QString& pkgname)
{
    DEBUG_FUNC_NAME
    if (ok)
    {
        WidgetUtils::information(this,
                      tr("SMARTFlux package creation"),
                      tr("<p>SMARTFlux package <b>'%1'</b> "
                         "created successfully.</p>").arg(pkgname),
                      tr("<p>Now upload it to your SMARTFlux "
                         "device <br>with the GHG software "
                         "<br>and enjoy on-the-fly fluxes, "
                         "corrected according to your settings!</p>"));
    }
    else
    {
        WidgetUtils::information(this,
                      tr("SMARTFlux package creation"),
                      tr("<p>An error occurred during the package creation."),
                      tr("<p>Please double check your settings and try again. "
                         "Contact LI-COR support if you are unable "
                         "to resolve the issue.</p>"));
    }
}

void CreatePackageDialog::refreshButtonStatus()
{
    createButton->setEnabled(!filenameEdit->text().isEmpty()
                         && !outpathEdit->text().isEmpty());
}

bool CreatePackageDialog::eventFilter(QObject *o, QEvent *e)
{
//    if (o == this)
//    {
//        bool previousHidden = isHidden();

////        if (e->type() == QEvent::WindowStateChange)
////        {
////            toggleConsoleOutputAct->setChecked(previousVisible);
////            return true;
////        }

//        if ( e->type() == QEvent::ActivationChange
//            || e->type() == QEvent::ApplicationActivate
//            || e->type() == QEvent::ApplicationStateChange
//            || e->type() == QEvent::Show
//             || e->type() == QEvent::WindowActivate
//            || e->type() == QEvent::WindowStateChange)
//        {
//            qDebug() << "create filter" << e->type();
//            if (previousHidden)
//                hide();

////            activateWindow();
//            return true;
//        }
//    }

    return QDialog::eventFilter(o, e);
}

void CreatePackageDialog::close()
{
    DEBUG_FUNC_NAME

    if (isVisible())
    {
        hide();
    }
    qDebug() << "isVisible()" << isVisible();
}

void CreatePackageDialog::onFilenameLabelClicked()
{
    filenameEdit->setFocus();
    filenameEdit->selectAll();
}

void CreatePackageDialog::clearFilenameEdit()
{
    filenameEdit->clear();
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_PROJECT,
                                             Defs::CONF_PROJ_SMARTFLUX_FILENAME,
                                             QString());
    refreshButtonStatus();
}

void CreatePackageDialog::updateFilename(const QString& fn)
{
    if (fn.isEmpty()) { return; }

    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_PROJECT,
                                             Defs::CONF_PROJ_SMARTFLUX_FILENAME,
                                             fn);
    refreshButtonStatus();
}

void CreatePackageDialog::clearOutpathEdit()
{
    outpathEdit->clear();
    WidgetUtils::updateLineEditToolip(outpathEdit);
    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_PROJECT,
                                             Defs::CONF_PROJ_SMARTFLUX_FILEPATH,
                                             QString());
    refreshButtonStatus();
}

void CreatePackageDialog::updateOutpath(const QString& fp)
{
    DEBUG_FUNC_NAME

    if (fp.isEmpty()) { return; }

    GlobalSettings::setAppPersistentSettings(Defs::CONFGROUP_PROJECT,
                                             Defs::CONF_PROJ_SMARTFLUX_FILEPATH,
                                             fp);
    refreshButtonStatus();
    outpathEdit->setButtonVisible(outpathEdit->isEnabled() && !outpathEdit->text().isEmpty());
    WidgetUtils::updateLineEditToolip(outpathEdit);
}

void CreatePackageDialog::outpathBrowse_clicked()
{
    DEBUG_FUNC_NAME
    QString searchPath = QDir::homePath();
    if (!configState_->window.last_data_path.isEmpty()
        && FileUtils::existsPath(configState_->window.last_data_path))
    {
        searchPath = configState_->window.last_data_path;
    }

    QString dir = QFileDialog::getExistingDirectory(this,
                        tr("Select the Output Directory"),
                        searchPath);

    if (dir.isEmpty()) { return; }

    QDir outDir(dir);
    QString canonicalOutDir = outDir.canonicalPath();
    outpathEdit->setText(QDir::toNativeSeparators(canonicalOutDir));

    configState_->window.last_data_path = canonicalOutDir;
    GlobalSettings::updateLastDatapath(canonicalOutDir);
}
