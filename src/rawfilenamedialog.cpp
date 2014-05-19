/***************************************************************************
  rawfilenamedialog.cpp
  -------------------
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
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QButtonGroup>
#include <QRadioButton>
#include <QFrame>
#include <QScrollArea>
#include <QDirIterator>

#include "dbghelper.h"
#include "alia.h"
#include "ecproject.h"
#include "rawfilenamedialog.h"

RawFilenameDialog::RawFilenameDialog(QWidget *parent, EcProject *ecProject) :
    QDialog(parent),
    ecProject_(ecProject)
{
    setWindowTitle(tr("Raw File Name Format"));
    Qt::WindowFlags winFflags = windowFlags();
    winFflags &= ~Qt::WindowContextHelpButtonHint;
    setWindowFlags(winFflags);
    setWindowModality(Qt::WindowModal);

    title = new QLabel(tr("Raw file name format"));
    title->setProperty("groupTitle", true);

    desc = new QLabel;

    radioGroupBoxLayout = new QVBoxLayout;
    radioGroupBox = new QFrame;
    radioGroupBox->setProperty("scrollContainerWidget", true);
    radioGroupBox->setLayout(radioGroupBoxLayout);

    QScrollArea* radioScrollArea = new QScrollArea;
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

    QGridLayout *dialogLayout = new QGridLayout(this);
    dialogLayout->addWidget(title, 0, 0);
    dialogLayout->addWidget(desc, 1, 0);
    dialogLayout->addWidget(radioScrollArea, 2, 0);
    dialogLayout->addWidget(rawFilenameFormatEdit, 3, 0);
    dialogLayout->addWidget(okButton, 4, 0, 1, 1, Qt::AlignCenter);
    dialogLayout->setVerticalSpacing(10);
    dialogLayout->setContentsMargins(30, 30, 30, 30);
    setLayout(dialogLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(extRadioGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(updateFormatEdit(int)));
    connect(rawFilenameFormatEdit, SIGNAL(textEdited(QString)),
            this, SLOT(updateFileList(QString)));
}

void RawFilenameDialog::close()
{
    emit updateFileFormatRequest(rawFilenameFormatEdit->text());
    if (isVisible())
        hide();
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
    reset();

    QString baseDescText = tr("<p>Your entry in this field will describe "
                     "how the timestamp is encoded in the raw file name.<br />"
                     "In the field below, replace the:"
                     "<ul><li>Year with characters yy or yyyy</li>"
                     "<li>Month with mm</li>"
                     "<li>Day with dd (day of month) or ddd (day of year) (case sensitive)</li>"
                     "<li>Hour and minute with HH and MM</li></ul>"
                     "<u>Example</u><br />"
                     "<table><tr><td>File name:</td><td>2011-09-27_1030_mysite.raw</td></tr>"
                     "<tr><td>File name format:</td><td>yyyy-mm-dd_HHMM_mysite.raw</td></tr>"
                     "</table></p><hr />");

    fileList_ = getRawFileTypeAvailable();

    if (fileList_.count() > 1)
    {
        desc->setText(baseDescText +
                      tr("<p>In the 'Raw data directory' we found file names with the "
                         "following extensions:</p>"));

        createRadioButtons(fileList_);
    }
    else if (fileList_.count() == 1)
    {
        desc->setText(baseDescText +
                      tr("\nIn the 'Raw data directory' we found file names of the "
                         "following type:"));
        createRadioButtons(fileList_);
    }
    else
    {
        desc->setText(baseDescText +
                      tr("\nIn the 'Raw data directory' we found no files."));
    }

    if (!fileList_.isEmpty())
    {
        qDebug() << fileList_.count() << fileList_;
        QFileInfo fileInfo(fileList_.at(0));
        rawFilenameFormatEdit->setText(fileInfo.fileName());
        rawFilenameFormatEdit->selectAll();
    }
    okButton->setEnabled(Alia::isGoodRawFileNameFormat(rawFilenameFormatEdit->text()));
}

QStringList RawFilenameDialog::getRawFileTypeAvailable()
{
    DEBUG_FUNC_NAME
    QStringList list;
    if (!ecProject_->screenDataPath().isEmpty())
    {
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
        QString currExt;
        QStringList prevExtList;
        QFileInfo currFileInfo;
        while (dirIterator.hasNext())
        {
            dirIterator.next();
            currFileInfo = dirIterator.fileInfo();
            currFile = currFileInfo.absoluteFilePath();
            currExt = currFileInfo.suffix();

            if (!prevExtList.contains(currExt))
            {
                list.append(currFile);
                prevExtList.append(currExt);
            }
        }
    }
    return list;
}

void RawFilenameDialog::createRadioButtons(const QStringList& list)
{
    QRadioButton* button;
    int i = 0;
    foreach (const QString& file, list)
    {
        qDebug() << QFileInfo(file).suffix();
        button = new QRadioButton(QFileInfo(file).suffix());
        extRadioGroup->addButton(button, i);
        radioGroupBoxLayout->addWidget(button);
        ++i;
    }
    extRadioGroup->buttons().first()->setChecked(true);
    radioGroupBox->setVisible(true);
}

void RawFilenameDialog::updateFormatEdit(int id)
{
    QFileInfo fileInfo(fileList_.at(id));

    rawFilenameFormatEdit->setText(fileInfo.fileName());
    rawFilenameFormatEdit->selectAll();
}

void RawFilenameDialog::updateFileList(const QString& file)
{
    DEBUG_FUNC_NAME
    qDebug() << extRadioGroup->buttons().count()
             << extRadioGroup->checkedId()
             << file;
    if (extRadioGroup->buttons().count())
        fileList_.replace(extRadioGroup->checkedId(), file);

    okButton->setEnabled(Alia::isGoodRawFileNameFormat(rawFilenameFormatEdit->text()));
}
