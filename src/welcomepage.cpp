/***************************************************************************
  welcomepage.cpp
  -------------------
  Copyright (C) 2007-2011, Eco2s team, Antonio Forgione
  Copyright (C) 2011-2014, LI-COR Biosciences
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

#include <QDebug>
#include <QGridLayout>
#include <QListWidget>
#include <QUrl>
#include <QDir>
#include <QDesktopServices>
#include <QToolButton>
#include <QCheckBox>
#include <QPushButton>

#include "defs.h"
#include "alia.h"
#include "smartfluxbar.h"
#include "dbghelper.h"
#include "clicklabel.h"
#include "ecproject.h"
#include "welcomepage.h"

WelcomePage::WelcomePage(QWidget *parent, EcProject *ecProject, ConfigState* configState) :
    QWidget(parent),
    ecProject_(ecProject),
    configState_(configState)
{
    appLogoLabel = new ClickLabel;
    appLogoLabel->setProperty("applogoLabel", true);
    appLogoLabel->setProperty("applogoSmallLabel", false);

    connect(appLogoLabel, SIGNAL(clicked()), this, SLOT(openWebSite()));

    feedbackLabel = new ClickLabel;
    feedbackLabel->setProperty("feedbackLabel", true);
    connect(feedbackLabel, SIGNAL(clicked()), this, SLOT(openForumFeedback()));

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
    openButton->setIconSize(QSize(110, 105));
    openButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(12);
    buttonsLayout->addWidget(newButton);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(openButton);
    buttonsLayout->addStretch(12);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setMargin(0);

    QLabel *recentListTitle = new QLabel(tr("Recent Projects"));
    recentListTitle->setProperty("groupTitle3", true);

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

    ClickLabel* title = new ClickLabel;
    title->setPixmap(QPixmap(QStringLiteral(":/icons/smartflux-grey")));
    title->setProperty("smartfluxLogoGrey", true);
    title->setToolTip(smartfluxTooltip);

    ClickLabel* subTitle = new ClickLabel;
    subTitle->setObjectName(QStringLiteral("smartfluxSubtitle"));
    subTitle->setText(tr("Configuration "));
    subTitle->setToolTip(smartfluxTooltip);

    connect(title, SIGNAL(clicked()), smartfluxModeCheckbox_, SLOT(toggle()));
    connect(subTitle, SIGNAL(clicked()), smartfluxModeCheckbox_, SLOT(toggle()));

    createQuestionMark();

    QHBoxLayout* smartfluxLabelContainerLayout = new QHBoxLayout;
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

    QGridLayout* projectsLayout = new QGridLayout;
    projectsLayout->addLayout(smartfluxLabelContainerLayout, 0, 1, 1, 1, Qt::AlignBottom);
    projectsLayout->addLayout(buttonsLayout, 1, 1, Qt::AlignCenter);
    projectsLayout->addWidget(recentListTitle, 2, 0, 1, -1, Qt::AlignLeft);
    projectsLayout->addWidget(recentListWidget, 3, 0, 1, -1, Qt::AlignCenter);
    projectsLayout->setRowStretch(4, 1);
    projectsLayout->setRowMinimumHeight(0, 40);

    QWidget* projectsWidget = new QWidget;
    projectsWidget->setMinimumWidth(600);
    projectsWidget->setLayout(projectsLayout);

    QLabel *newsTitle = new QLabel(tr("What's New at LI-COR"));
    newsTitle->setProperty("groupTitle3", true);

    QWidget* newsWidget = new QWidget;

    newsListWidget = new QListWidget;
    newsListWidget->setProperty("helpListItem", true);
    newsListWidget->setMinimumWidth(730);
    newsListWidget->setMinimumHeight(420);
    newsListWidget->setAlternatingRowColors(true);
    newsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    newsListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QListWidgetItem* rssItem_1 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/wrench")), tr("Design Your Own Eddy Covariance System"), newsListWidget);
    rssItem_1->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/products/eddy_covariance/system_build.html")));

    QListWidgetItem* rssItem_2 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("Case Studies and Applications"), newsListWidget);
    rssItem_2->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/products/eddy_covariance/applications.html")));

    QListWidgetItem* rssItem_3 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/video")), tr("Overview of Biological and Meteorological Sensors in Eddy Covariance Flux Research"), newsListWidget);
    rssItem_3->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/webinars/index.html#8-15-12")));

    QListWidgetItem* rssItem_4 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/video")), tr("Using Biomet Sensors in LI-COR Eddy Covariance Systems"), newsListWidget);
    rssItem_4->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/webinars/index.html#8-29-12")));

    QListWidgetItem* rssItem_5 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/video")), tr("Biomet Data Processing and Advanced Features of EddyPro 4"), newsListWidget);
    rssItem_5->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/webinars/index.html#9-5-12")));

    QListWidgetItem* rssItem_6 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/8100-app")), tr("Wireless Application Available for LI-8100A"), newsListWidget);
    rssItem_6->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/newsline/2012/06/new-wireless-application-available-for-li-8100a/")));

    QListWidgetItem* rssItem_7 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/grad")), tr("New Eddy Covariance Training course available"), newsListWidget);
    rssItem_7->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/products/eddy_covariance/training.html")));

    QListWidgetItem* rssItem_8 = new QListWidgetItem(QIcon(QStringLiteral(":/icons/article")), tr("Mapping CO2 Concentrations and Fluxes with the LI-8100A"), newsListWidget);
    rssItem_8->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/applications/mapping.html")));

    newsListWidget->addItem(rssItem_1);
    newsListWidget->addItem(rssItem_2);
    newsListWidget->addItem(rssItem_3);
    newsListWidget->addItem(rssItem_4);
    newsListWidget->addItem(rssItem_5);
    newsListWidget->addItem(rssItem_6);
    newsListWidget->addItem(rssItem_7);
    newsListWidget->addItem(rssItem_8);

    QGridLayout* newsLayout = new QGridLayout;
    newsLayout->addWidget(newsTitle, 0, 0);
    newsLayout->addWidget(newsListWidget, 1, 0);
    newsWidget->setLayout(newsLayout);
    newsWidget->setMinimumWidth(600);

    QWidget* helpWidget = new QWidget;

    QLabel *helpTitle = new QLabel(tr("Help resources"));
    helpTitle->setProperty("groupTitle3", true);

    QListWidgetItem* item_1 = new QListWidgetItem(tr("%1 Help").arg(Defs::APP_NAME));
    item_1->setData(Qt::UserRole, QString(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#EddyPro_Home.htm")));
    QListWidgetItem* item_2 = new QListWidgetItem(tr("Getting started"));
    item_2->setData(Qt::UserRole, QString(QStringLiteral("ftp://ftp.licor.com/perm/env/EddyPro/Manual/EddyPro5_Getting_Started.pdf")));
    QListWidgetItem* item_3 = new QListWidgetItem(tr("Video tutorials"));
    item_3->setData(Qt::UserRole, QString(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#Video_Library.htm")));
    QListWidgetItem* item_4 = new QListWidgetItem(tr("Printable manual (PDF version)"));
    item_4->setData(Qt::UserRole, QString(QStringLiteral("ftp://ftp.licor.com/perm/env/EddyPro/Manual/EddyPro5_User_Guide.pdf")));
    QListWidgetItem* item_5 = new QListWidgetItem(tr("Download sample data files"));
    item_5->setData(Qt::UserRole, QString(QStringLiteral("ftp://ftp.licor.com/perm/env/EddyPro/Data/")));

    helpListWidget = new QListWidget;
    helpListWidget->setProperty("helpListItem", true);
    helpListWidget->setAlternatingRowColors(true);
    helpListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    helpListWidget->addItem(item_1);
    helpListWidget->addItem(item_2);
    helpListWidget->addItem(item_3);
    helpListWidget->addItem(item_4);
    helpListWidget->addItem(item_5);

    QLabel *supportTitle = new QLabel(tr("Support resources"));
    supportTitle->setProperty("groupTitle3", true);

    QListWidgetItem* item_6 = new QListWidgetItem(tr("Forum"));
    item_6->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/forum/forum.php?id=8")));
    QListWidgetItem* item_7 = new QListWidgetItem(tr("Eddy Covariance Glossary"));
    item_7->setData(Qt::UserRole, QString(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#skinName=Glossary")));
    QListWidgetItem* item_8 = new QListWidgetItem(tr("LI-COR Newsline (EddyPro related articles and technical tips)"));
    item_8->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/newsline/tag/eddypro/")));
    QListWidgetItem* item_9 = new QListWidgetItem(tr("Features request"));
    item_9->setData(Qt::UserRole, QString(QStringLiteral("http://www.licor.com/env/forum/topic.php?id=15")));
    QListWidgetItem* item_10 = new QListWidgetItem(tr("Check for updates"));
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

    QGridLayout* helpLayout = new QGridLayout;
    helpLayout->addWidget(helpTitle, 0, 0);
    helpLayout->addWidget(helpListWidget, 1, 0);
    helpLayout->addWidget(supportTitle, 0, 1);
    helpLayout->addWidget(supportListWidget, 1, 1);
    helpLayout->setRowMinimumHeight(0, 30);
    helpLayout->setColumnStretch(0, 1);
    helpLayout->setColumnStretch(1, 1);

    helpWidget->setLayout(helpLayout);
    helpWidget->setMinimumWidth(600);

    QTabWidget* welcomeTab = new QTabWidget;
    welcomeTab->addTab(projectsWidget, tr("Manage Projects"));
    welcomeTab->addTab(newsWidget, tr("News"));
    welcomeTab->addTab(helpWidget, tr("Help and Support"));

    smartfluxBar_ = new SmartFluxBar(ecProject_, configState_);
    smartfluxBar_->setVisible(false);
    smartfluxBar_->setToolTip(tr("EddyPro is in SMARTFlux configuration mode (Ctrl+F to exit)"));
    smartfluxBar_->setMinimumHeight(35);

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

    connect(recentListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(recentOpenRequested(QListWidgetItem*)), Qt::QueuedConnection);

    connect(recentListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(doNothing(QListWidgetItem*)));

    connect(newsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(newsItemRequested(QListWidgetItem*)));

    connect(helpListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(helpItemRequested(QListWidgetItem*)));

    connect(supportListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(supportItemRequested(QListWidgetItem*)));

    connect(parent, SIGNAL(recentUpdated()),
            this, SLOT(updateRecentList()));

    connect(openButton, SIGNAL(clicked()), this, SLOT(openProjectRequested()));
    connect(newButton, SIGNAL(clicked()), this, SIGNAL(newProjectRequest()));

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
    QMessageBox* msgBox = new QMessageBox();
    msgBox->setWindowTitle(QStringLiteral("Troppi click, Ger :-)"));
    msgBox->setText(QStringLiteral("Detected Double Open"));
    msgBox->show();
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
        Alia::showHelp(QUrl(item->data(Qt::UserRole).toString()));
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
            QListWidgetItem* item = new QListWidgetItem(QDir::toNativeSeparators(recentfile), recentListWidget);
            Q_UNUSED(item)
        }
    }
}

void WelcomePage::updateWelcomePage(bool small)
{
    QList<Alia::PropertyList> appLogoProp;

    if (small)
    {
        appLogoProp << Alia::PropertyList("applogoLabel", false)
                    << Alia::PropertyList("applogoSmallLabel", true);
    }
    else
    {
        appLogoProp << Alia::PropertyList("applogoLabel", true)
                    << Alia::PropertyList("applogoSmallLabel", false);
    }

    Alia::updatePropertyListAndStyle(appLogoLabel, appLogoProp);
}

void WelcomePage::openWebSite()
{
    Alia::openWebSite();
}

void WelcomePage::openForumFeedback()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://www.licor.com/env/forum/forum.php?id=8")));
}

void WelcomePage::updateSmartfluxBar()
{
    DEBUG_FUNC_NAME
    qDebug() << configState_->project.smartfluxMode;
    smartfluxBar_->setVisible(configState_->project.smartfluxMode);
}

// NOTE: to complete
void WelcomePage::updateSmartfluxCheckBox()
{
    DEBUG_FUNC_NAME
    smartfluxModeCheckbox_->blockSignals(true);
    smartfluxModeCheckbox_->setChecked(configState_->project.smartfluxMode);
    smartfluxModeCheckbox_->blockSignals(false);
}

//  NOTE: not used
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

    connect(questionMark_1, SIGNAL(clicked()),
            this, SLOT(onlineHelpTrigger_1()));
}

void WelcomePage::onlineHelpTrigger_1()
{
    Alia::showHelp(QUrl(QStringLiteral("http://envsupport.licor.com/help/EddyPro5/index.htm#smartfluxSettings.htm")));
}
