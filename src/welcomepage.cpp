/***************************************************************************
  welcomepage.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2018, LI-COR Biosciences
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

    toviLabel = new ClickLabel;
    toviLabel->setProperty("toviLabel", true);
    toviLabel->setProperty("toviWideLabel", false);
    connect(toviLabel, &ClickLabel::clicked,
            this, &WelcomePage::openToviHomepage);

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
#if defined(Q_OS_MACOS)
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

    auto rssItem_1 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/ep")), tr("Introducing Tovi") + Defs::TRADEMARK_SYMBOL
                                                                                 + tr(" – Powerful software for processing EddyPro outputs"), newsListWidget);
    rssItem_1->setData(Qt::UserRole, QStringLiteral("https://tovi.io/?utm_source=EddyPro%20Software&utm_medium=Tovi%20Ads&utm_campaign=EP_Tovi_ads"));

    auto rssItem_2 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("New Trace Gas Analyzers – high performance in an entirely new, portable design"), newsListWidget);
    rssItem_2->setData(Qt::UserRole, QStringLiteral("https://www.licor.com/env/products/trace_gas/"));

    auto rssItem_3 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("Simplified Biomet data collection with the LI-COR-designed Data Acquisition System"), newsListWidget);
    rssItem_3->setData(Qt::UserRole, QStringLiteral("https://www.licor.com/env/products/eddy_covariance/daqm.html"));

    auto rssItem_4 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("New LI-7500DS CO2/H2O Analyzer features a lower price and runs on less power"), newsListWidget);
    rssItem_4->setData(Qt::UserRole, QStringLiteral("https://www.licor.com/env/products/eddy_covariance/LI-7500DS.html"));

    auto rssItem_5 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("LI-6800 Portable Photosynthesis System – Fast response creates new research possibilities"), newsListWidget);
    rssItem_5->setData(Qt::UserRole, QStringLiteral("https://www.licor.com/env/products/photosynthesis/LI-6800/"));

    auto rssItem_6 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/grad")), tr("Learn how to make accurate soil gas flux measurements in a free online training course"), newsListWidget);
    rssItem_6->setData(Qt::UserRole, QStringLiteral("https://accounts.skilljar.com/accounts/signup/?next=%2Fauth%2Fendpoint%2Flogin%2Fresult%3Fd%3D2hffjl7lbimea%26next%3D%252F&d=2hffjl7lbimea&t=zhzzn654u9er"));

    auto rssItem_7 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/grad")), tr("See upcoming Eddy Covariance training courses"), newsListWidget);
    rssItem_7->setData(Qt::UserRole, QStringLiteral("https://www.licor.com/env/products/eddy_covariance/resources.html#training"));

    newsListWidget->addItem(rssItem_1);
    newsListWidget->addItem(rssItem_2);
    newsListWidget->addItem(rssItem_3);
    newsListWidget->addItem(rssItem_4);
    newsListWidget->addItem(rssItem_5);
    newsListWidget->addItem(rssItem_6);
    newsListWidget->addItem(rssItem_7);

    auto newsLayout = new QGridLayout;
    newsLayout->addWidget(newsTitle, 0, 0);
    newsLayout->addWidget(newsListWidget, 1, 0);
    newsWidget->setLayout(newsLayout);
    newsWidget->setMinimumWidth(600);

    auto helpWidget = new QWidget;

    auto helpTitle = new QLabel(tr("Help resources"));
    helpTitle->setProperty("groupTitle3", true);

    auto item_1 = new QListWidgetItem(tr("%1 Help").arg(Defs::APP_NAME));
    item_1->setData(Qt::UserRole, QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/EddyPro_Home.html"));
    auto item_2 = new QListWidgetItem(tr("Getting started (PDF)"));
    item_2->setData(Qt::UserRole, QStringLiteral("https://boxenterprise.net/s/qmhucid6g0hdvd3d13tk"));
    auto item_3 = new QListWidgetItem(tr("Video tutorials"));
    item_3->setData(Qt::UserRole, QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Video_Library.html"));
    auto item_4 = new QListWidgetItem(tr("Printable manual (PDF)"));
    item_4->setData(Qt::UserRole, QStringLiteral("https://boxenterprise.net/s/1ium2zmwm6hl36yz9bu4"));
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
    item_6->setData(Qt::UserRole, QStringLiteral("http://www.licor.com/env/forum/?forum=eddypro"));
    auto item_7 = new QListWidgetItem(tr("Eddy Covariance Glossary"));
    item_7->setData(Qt::UserRole, QStringLiteral("http://www.licor.com/env/help/eddypro/topics_eddypro/Glossary.html"));
    auto item_8 = new QListWidgetItem(tr("LI-COR Newsline (EddyPro related articles and technical tips)"));
    item_8->setData(Qt::UserRole, QStringLiteral("http://www.licor.com/env/newsline/tag/eddypro/"));
    auto item_9 = new QListWidgetItem(tr("Features request"));
    item_9->setData(Qt::UserRole, QStringLiteral("http://www.licor.com/env/forum/?topic=feature-requests"));
    auto item_10 = new QListWidgetItem(tr("Check for updates"));
    item_10->setData(Qt::UserRole, QStringLiteral("check_updates"));

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
//
    auto welcomeTab = new QTabWidget;
    welcomeTab->addTab(projectsWidget, tr("Manage Projects"));
    welcomeTab->addTab(newsWidget, tr("News"));
    welcomeTab->addTab(helpWidget, tr("Help and Support"));

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);

    smartfluxBarPlaceholder_ = new QWidget;
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    smartfluxBarPlaceholder_->setMinimumHeight(35);
#elif defined(Q_OS_MACOS)
    smartfluxBarPlaceholder_->setMinimumHeight(50);
#endif

    mainLayout_ = new QGridLayout(this);
    mainLayout_->addWidget(smartfluxBar_, 0, 0, 1, -1);
    mainLayout_->addWidget(smartfluxBarPlaceholder_, 0, 0, 1, -1, Qt::AlignHCenter);
    mainLayout_->addWidget(appLogoLabel, 1, 0, 1, -1, Qt::AlignHCenter | Qt::AlignVCenter);
    mainLayout_->addWidget(toviLabel, 2, 0, 1, 1, Qt::AlignHCenter);
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
    emit openProjectRequest(QString());
}

void WelcomePage::recentOpenRequested(QListWidgetItem* item)
{
    emit openProjectRequest(item->text());
}

// NOTE: hack to avoid side effects opening a recent project with a double click
void WelcomePage::doNothing(QListWidgetItem* item)
{
    Q_UNUSED(item)

#ifdef QT_DEBUG
//    auto msgBox = new QMessageBox;
//    msgBox->setWindowTitle(QStringLiteral("Troppi click, Ger :-)"));
//    msgBox->setText(QStringLiteral("Detected Double Open"));
//    msgBox->show();
#endif
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
    recentListWidget->clear();

    for (const auto &recentfile : configState_->general.recentfiles)
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
    QList<WidgetUtils::PropertyList> toviAdProp;

    if (small)
    {
        appLogoProp << WidgetUtils::PropertyList("applogoLabel", false)
                    << WidgetUtils::PropertyList("applogoSmallLabel", true);
        toviAdProp << WidgetUtils::PropertyList("toviLabel", false)
                   << WidgetUtils::PropertyList("toviWideLabel", true);
    }
    else
    {
        appLogoProp << WidgetUtils::PropertyList("applogoLabel", true)
                    << WidgetUtils::PropertyList("applogoSmallLabel", false);
        toviAdProp << WidgetUtils::PropertyList("toviLabel", true)
                   << WidgetUtils::PropertyList("toviWideLabel", false);
    }

    WidgetUtils::updatePropertyListAndStyle(appLogoLabel, appLogoProp);
    WidgetUtils::updatePropertyListAndStyle(toviLabel, toviAdProp);
}

void WelcomePage::openToviHomepage()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://tovi.io/?utm_source=EddyPro%20Software&utm_medium=Tovi%20Ads&utm_campaign=EP_Tovi_ads")));
}

void WelcomePage::updateSmartfluxBar()
{
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
}

void WelcomePage::updateSmartfluxCheckBox()
{
    smartfluxModeCheckbox_->blockSignals(true);
    smartfluxModeCheckbox_->setChecked(configState_->project.smartfluxMode);
    smartfluxModeCheckbox_->blockSignals(false);
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
