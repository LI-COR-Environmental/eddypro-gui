/***************************************************************************
  welcomepage.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
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

#include "welcomepage.h"

#include <QCheckBox>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QGridLayout>
#include <QListWidget>
#include <QPushButton>
#include <QToolButton>
#include <QUrl>

#include "clicklabel.h"
#include "dbghelper.h"
#include "defs.h"
#include "ecproject.h"
#include "smartfluxbar.h"
#include "widget_utils.h"

WelcomePage::WelcomePage(QWidget *parent, EcProject *ecProject, ConfigState* configState) :
    QWidget(parent),
    ecProject_(ecProject),
    configState_(configState)
{
    appLogoLabel = new ClickLabel;
    appLogoLabel->setProperty("applogoLabel", true);
    appLogoLabel->setProperty("applogoSmallLabel", false);

    connect(appLogoLabel, &ClickLabel::clicked,
            this, &WidgetUtils::openAppWebsite);

    feedbackLabel = new ClickLabel;
    feedbackLabel->setProperty("feedbackLabel", true);
    connect(feedbackLabel, &ClickLabel::clicked,
            this, &WelcomePage::openForumFeedback);

    newButton = new QToolButton;
    newButton->setText(tr("New Project"));
    newButton->setObjectName(QStringLiteral("newButton"));
    newButton->setIcon(QPixmap(QStringLiteral(":/icons/new-big")));
    newButton->setIconSize(QSize(94, 103));
    newButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    openButton = new QToolButton;
    openButton->setText(tr("Open Project"));
    openButton->setObjectName(QStringLiteral("openButton"));
    openButton->setIcon(QPixmap(QStringLiteral(":/icons/open-big")));
    openButton->setIconSize(QSize(94, 103));
    openButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    auto mainButtonsLayout = new QHBoxLayout;
    mainButtonsLayout->addStretch(12);
    mainButtonsLayout->addWidget(newButton);
    mainButtonsLayout->addStretch(1);
    mainButtonsLayout->addWidget(openButton);
    mainButtonsLayout->addStretch(12);
    mainButtonsLayout->setContentsMargins(0, 0, 0, 0);
    mainButtonsLayout->setMargin(0);

    auto recentListTitle = new QLabel(tr("Recent Projects"));
    recentListTitle->setProperty("openRecentGroupTitle", true);

    recentListWidget = new QListWidget;
    recentListWidget->setProperty("helpListItem", true);
    recentListWidget->setMinimumWidth(730);
    recentListWidget->setMinimumHeight(240);
    recentListWidget->setAlternatingRowColors(true);
    recentListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    recentListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    updateRecentList();

    QString smartfluxTooltip = QStringLiteral("Use EddyPro in "
                               "'SMARTFlux configuration' mode. <br>"
                               "This is an interface to customize "
                               "advanced processing options and create "
                               "the .smartflux configuration package, "
                               "which you will upload to the SMARTFlux "
                               "device for fully corrected, fully "
                               "customized fluxes on-the-fly out of "
                               "your LI-COR eddy-covariance system.");

    smartfluxModeCheckbox_ = new QCheckBox;
    smartfluxModeCheckbox_->setToolTip(smartfluxTooltip);

    auto title = new ClickLabel;
    auto pixmap = QPixmap(QStringLiteral(":/icons/smartflux-grey"));
#if defined(Q_OS_MAC)
    pixmap.setDevicePixelRatio(2.0);
#endif
    title->setPixmap(pixmap);
    title->setProperty("smartfluxLogoGrey", true);
    title->setToolTip(smartfluxTooltip);

    auto subTitle = new ClickLabel;
    subTitle->setObjectName(QStringLiteral("smartfluxSubtitle"));
    subTitle->setText(tr("Configuration "));
    subTitle->setToolTip(smartfluxTooltip);

    connect(title, &ClickLabel::clicked,
            smartfluxModeCheckbox_, &QCheckBox::toggle);
    connect(subTitle, &ClickLabel::clicked,
            smartfluxModeCheckbox_, &QCheckBox::toggle);

    createQuestionMark();

    auto smartfluxLabelContainerLayout = new QHBoxLayout;
    smartfluxLabelContainerLayout->addSpacing(100);
    smartfluxLabelContainerLayout->addWidget(smartfluxModeCheckbox_);
    smartfluxLabelContainerLayout->addSpacing(5);
    smartfluxLabelContainerLayout->addWidget(title, 0, Qt::AlignBottom);
    smartfluxLabelContainerLayout->addSpacing(5);
    smartfluxLabelContainerLayout->addWidget(subTitle);
    smartfluxLabelContainerLayout->addWidget(questionMark_1);
    smartfluxLabelContainerLayout->addStretch();
    smartfluxLabelContainerLayout->setContentsMargins(0, 0, 0, 0);
    smartfluxLabelContainerLayout->setMargin(0);

    auto projectsLayout = new QGridLayout;
    projectsLayout->addLayout(smartfluxLabelContainerLayout, 0, 1, 1, 1, Qt::AlignBottom);
    projectsLayout->addLayout(mainButtonsLayout, 1, 1, Qt::AlignCenter);
    projectsLayout->addWidget(recentListTitle, 2, 0, 1, -1, Qt::AlignLeft);
    projectsLayout->addWidget(recentListWidget, 3, 0, 1, -1, Qt::AlignCenter);
    projectsLayout->setRowStretch(4, 1);
    projectsLayout->setRowMinimumHeight(0, 40);

    auto projectsWidget = new QWidget;
    projectsWidget->setMinimumWidth(600);
    projectsWidget->setLayout(projectsLayout);

    auto newsTitle = new QLabel(tr("What's New at LI-COR"));
    newsTitle->setProperty("groupTitle3", true);

    auto newsWidget = new QWidget;

    newsListWidget = new QListWidget;
    newsListWidget->setProperty("helpListItem", true);
    newsListWidget->setMinimumWidth(730);
    newsListWidget->setMinimumHeight(420);
    newsListWidget->setAlternatingRowColors(true);
    newsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    newsListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto rssItem_1 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("LI-COR Launches Redesigned Light Sensors Optimized for Meteorological Measurements"), newsListWidget);
    rssItem_1->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/light")));

    auto rssItem_2 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("Coming Soon! FluxSuite™ Software – Provides real-time results, status information, and alerts from your EC site"), newsListWidget);
    rssItem_2->setData(Qt::UserRole, QString(QStringLiteral("http://www.fluxsuite.com")));

    auto rssItem_3 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("New! SoilFluxPro™ Software – Expanded processing for soil gas flux data"), newsListWidget);
    rssItem_3->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/soilfluxpro")));

    auto rssItem_4 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/wrench")), tr("Design Your Own Eddy Covariance System"), newsListWidget);
    rssItem_4->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/products/eddy_covariance/system_build.html")));

    auto rssItem_5 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("Eddy Covariance Case Studies and Applications"), newsListWidget);
    rssItem_5->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/products/eddy_covariance/applications.html")));

    auto rssItem_6 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/wrench")), tr("Small add-on provides big benefits for eddy covariance systems - The SMARTFlux™ System"), newsListWidget);
    rssItem_6->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/SMARTFlux/")));

    auto rssItem_7 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/grad")), tr("View Upcoming Eddy Covariance Training Courses"), newsListWidget);
    rssItem_7->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/products/eddy_covariance/training.html")));

    auto rssItem_8 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("Mapping CO2 Concentrations and Fluxes with the LI-8100A"), newsListWidget);
    rssItem_8->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/applications/mapping.html")));

    newsListWidget->addItem(rssItem_1);
    newsListWidget->addItem(rssItem_2);
    newsListWidget->addItem(rssItem_3);
    newsListWidget->addItem(rssItem_4);
    newsListWidget->addItem(rssItem_5);
    newsListWidget->addItem(rssItem_6);
    newsListWidget->addItem(rssItem_7);
    newsListWidget->addItem(rssItem_8);

    auto newsLayout = new QGridLayout;
    newsLayout->addWidget(newsTitle, 0, 0);
    newsLayout->addWidget(newsListWidget, 1, 0);
    newsWidget->setLayout(newsLayout);
    newsWidget->setMinimumWidth(600);

    auto helpWidget = new QWidget;

    auto helpTitle = new QLabel(tr("Help resources"));
    helpTitle->setProperty("groupTitle3", true);

    auto item_1 = new QListWidgetItem(tr("%1 Help").arg(Defs::APP_NAME));
    item_1->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/EddyPro_Home.html")));
    auto item_2 = new QListWidgetItem(tr("Getting started"));
    item_2->setData(Qt::UserRole, QString(QStringLiteral("https://boxenterprise.net/s/qmhucid6g0hdvd3d13tk")));
    auto item_3 = new QListWidgetItem(tr("Video tutorials"));
    item_3->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Video_Library.html")));
    auto item_4 = new QListWidgetItem(tr("Printable manual (PDF version)"));
    item_4->setData(Qt::UserRole, QString(QStringLiteral("https://boxenterprise.net/s/1ium2zmwm6hl36yz9bu4")));
    auto item_5 = new QListWidgetItem(tr("Download sample data files"));
    item_5->setData(Qt::UserRole, Defs::EP_SAMPLE_DATA_FILES);

    helpListWidget = new QListWidget;
    helpListWidget->setProperty("helpListItem", true);
    helpListWidget->setAlternatingRowColors(true);
    helpListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    helpListWidget->addItem(item_1);
    helpListWidget->addItem(item_2);
    helpListWidget->addItem(item_3);
    helpListWidget->addItem(item_4);
    helpListWidget->addItem(item_5);

    auto supportTitle = new QLabel(tr("Support resources"));
    supportTitle->setProperty("groupTitle3", true);

    auto item_6 = new QListWidgetItem(tr("Forum"));
    item_6->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/forum/?forum=eddypro")));
    auto item_7 = new QListWidgetItem(tr("Eddy Covariance Glossary"));
    item_7->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Glossary.html")));
    auto item_8 = new QListWidgetItem(tr("LI-COR Newsline (EddyPro related articles and technical tips)"));
    item_8->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/newsline/tag/eddypro/")));
    auto item_9 = new QListWidgetItem(tr("Features request"));
    item_9->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/forum/?topic=feature-requests")));
    auto item_10 = new QListWidgetItem(tr("Check for updates"));
    item_10->setData(Qt::UserRole, QString(QStringLiteral("check_updates")));

    supportListWidget = new QListWidget;
    supportListWidget->setProperty("helpListItem", true);
    supportListWidget->setAlternatingRowColors(true);
    supportListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    supportListWidget->addItem(item_6);
    supportListWidget->addItem(item_7);
    supportListWidget->addItem(item_8);
    supportListWidget->addItem(item_9);
    supportListWidget->addItem(item_10);

    auto helpLayout = new QGridLayout;
    helpLayout->addWidget(helpTitle, 0, 0);
    helpLayout->addWidget(helpListWidget, 1, 0);
    helpLayout->addWidget(supportTitle, 0, 1);
    helpLayout->addWidget(supportListWidget, 1, 1);
    helpLayout->setRowMinimumHeight(0, 30);
    helpLayout->setColumnStretch(0, 1);
    helpLayout->setColumnStretch(1, 1);

    helpWidget->setLayout(helpLayout);
    helpWidget->setMinimumWidth(600);

    auto welcomeTab = new QTabWidget;
    welcomeTab->addTab(projectsWidget, tr("Manage Projects"));
    welcomeTab->addTab(newsWidget, tr("News"));
    welcomeTab->addTab(helpWidget, tr("Help and Support"));

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);

    // TODO: verify if necessary: it seems not in windows
//    smartfluxBar_->setMinimumHeight(35);

    smartfluxBarPlaceholder_ = new QWidget;
#if defined(Q_OS_WIN)
    smartfluxBarPlaceholder_->setMinimumHeight(35);
#elif defined(Q_OS_MAC)
    smartfluxBarPlaceholder_->setMinimumHeight(50);
#endif

    mainLayout_ = new QGridLayout(this);
    mainLayout_->addWidget(smartfluxBar_, 0, 0, 1, -1);
    mainLayout_->addWidget(smartfluxBarPlaceholder_, 0, 0, 1, -1, Qt::AlignHCenter);
    mainLayout_->addWidget(appLogoLabel, 1, 0, 1, -1, Qt::AlignHCenter);
    mainLayout_->addWidget(feedbackLabel, 2, 0, 1, 1, Qt::AlignHCenter);
    mainLayout_->addWidget(welcomeTab, 3, 0, 1, -1, Qt::AlignHCenter);
    mainLayout_->setRowStretch(0, 0);
    mainLayout_->setRowStretch(1, 1);
    mainLayout_->setRowStretch(2, 1);
    mainLayout_->setRowStretch(3, 5);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setRowMinimumHeight(0, 35);

    setLayout(mainLayout_);

    connect(recentListWidget, &QListWidget::itemClicked,
            this, &WelcomePage::recentOpenRequested, Qt::QueuedConnection);

    connect(recentListWidget, &QListWidget::itemDoubleClicked,
            this, &WelcomePage::doNothing);

    connect(newsListWidget, &QListWidget::itemClicked,
            this, &WelcomePage::newsItemRequested);

    connect(helpListWidget, &QListWidget::itemClicked,
            this, &WelcomePage::helpItemRequested);

    connect(supportListWidget, &QListWidget::itemClicked,
            this, &WelcomePage::supportItemRequested);

    connect(parent, SIGNAL(recentUpdated()),
            this, SLOT(updateRecentList()));

    connect(openButton, &QToolButton::clicked,
            this, &WelcomePage::openProjectRequested);
    connect(newButton, &QToolButton::clicked,
            this, &WelcomePage::newProjectRequest);

    // routing signal to parent
    connect(smartfluxBar_, SIGNAL(showSmartfluxBarRequest(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveSilentlyRequest()),
            parent, SIGNAL(saveSilentlyRequest()));

    connect(smartfluxModeCheckbox_, SIGNAL(toggled(bool)),
            parent, SIGNAL(showSmartfluxBarRequest(bool)));

    connect(smartfluxBar_, SIGNAL(saveRequest()),
            parent, SIGNAL(saveRequest()));
}

WelcomePage::~WelcomePage()
{
}

void WelcomePage::openProjectRequested()
{
    DEBUG_FUNC_NAME
    emit openProjectRequest(QString());
}

void WelcomePage::recentOpenRequested(QListWidgetItem* item)
{
    DEBUG_FUNC_NAME
    emit openProjectRequest(item->text());
}

// NOTE: hack to avoid side effects opening a recent project with a double click
void WelcomePage::doNothing(QListWidgetItem* item)
{
    Q_UNUSED(item)
    DEBUG_FUNC_NAME

#ifdef QT_DEBUG
//    auto msgBox = new QMessageBox;
//    msgBox->setWindowTitle(QStringLiteral("Troppi click, Ger :-)"));
//    msgBox->setText(QStringLiteral("Detected Double Open"));
//    msgBox->show();
#endif
    qDebug() << "Detected Double Click";
}

void WelcomePage::newsItemRequested(QListWidgetItem* item)
{
    QDesktopServices::openUrl(QUrl(item->data(Qt::UserRole).toString()));
}

void WelcomePage::helpItemRequested(QListWidgetItem* item)
{
    if (item->text() == tr("Download sample data files"))
    {
        QDesktopServices::openUrl(QUrl(item->data(Qt::UserRole).toString()));
    }
    else
    {
        WidgetUtils::showHelp(QUrl(item->data(Qt::UserRole).toString()));
    }
}

void WelcomePage::supportItemRequested(QListWidgetItem* item)
{
    QDesktopServices::openUrl(QUrl(item->data(Qt::UserRole).toString()));
    if (item->data(Qt::UserRole).toString() == QLatin1String("check_updates"))
    {
        emit checkUpdatesRequest();
    }
}

void WelcomePage::updateRecentList()
{
    DEBUG_FUNC_NAME

    recentListWidget->clear();

    foreach (const QString& recentfile, configState_->general.recentfiles)
    {
        if (QFile::exists(recentfile))
        {
            auto item = new QListWidgetItem(QDir::toNativeSeparators(recentfile), recentListWidget);
            Q_UNUSED(item)
        }
    }
}

void WelcomePage::updateWelcomePage(bool small)
{
    QList<WidgetUtils::PropertyList> appLogoProp;

    if (small)
    {
        appLogoProp << WidgetUtils::PropertyList("applogoLabel", false)
                    << WidgetUtils::PropertyList("applogoSmallLabel", true);
    }
    else
    {
        appLogoProp << WidgetUtils::PropertyList("applogoLabel", true)
                    << WidgetUtils::PropertyList("applogoSmallLabel", false);
    }

    WidgetUtils::updatePropertyListAndStyle(appLogoLabel, appLogoProp);
}

void WelcomePage::openForumFeedback()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://www.licor.com/env/forum/?forum=eddypro")));
}

void WelcomePage::updateSmartfluxBar()
{
    DEBUG_FUNC_NAME
    qDebug() << configState_->project.smartfluxMode;
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
}

void WelcomePage::updateSmartfluxCheckBox()
{
    DEBUG_FUNC_NAME
    smartfluxModeCheckbox_->blockSignals(true);
    smartfluxModeCheckbox_->setChecked(configState_->project.smartfluxMode);
    smartfluxModeCheckbox_->blockSignals(false);
}

// NOTE: not used
void WelcomePage::updateMainLayout(bool on)
{
    if (on)
    {
        mainLayout_->removeWidget(smartfluxBarPlaceholder_);
        smartfluxBarPlaceholder_->setVisible(false);
        mainLayout_->addWidget(smartfluxBar_, 0, 0, 1, -1);
        smartfluxBar_->setVisible(true);
    }
    else
    {
        mainLayout_->removeWidget(smartfluxBar_);
        smartfluxBar_->setVisible(false);
        mainLayout_->addWidget(smartfluxBarPlaceholder_, 0, 0, 1, -1);
        smartfluxBarPlaceholder_->setVisible(true);
    }
}

void WelcomePage::createQuestionMark()
{
    questionMark_1 = new QPushButton;
    questionMark_1->setObjectName(QStringLiteral("questionMarkImg"));
    questionMark_1->setProperty("smartfluxQuestionMark", true);

    connect(questionMark_1, &QPushButton::clicked,
            this, &WelcomePage::onlineHelpTrigger_1);
}

void WelcomePage::onlineHelpTrigger_1()
{
    WidgetUtils::showHelp(QUrl(QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/smartfluxSettings.html")));
}
