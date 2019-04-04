/***************************************************************************
  dlinidialog.h
  -------------
  Copyright © 2007-2011, Eco2s team, Antonio Forgione
  Copyright © 2011-2019, LI-COR Biosciences, Inc. All Rights Reserved.
  Author: Antonio Forgione

  This file is part of EddyPro®.

  NON-COMMERCIAL RESEARCH PURPOSES ONLY - EDDYPRO® is licensed for
  non-commercial academic and government research purposes only,
  as provided in the EDDYPRO® End User License Agreement.
  EDDYPRO® may only be used as provided in the End User License Agreement
  and may not be used or accessed for any commercial purposes.
  You may view a copy of the End User License Agreement in the file
  EULA_NON_COMMERCIAL.rtf.

  Commercial companies that are LI-COR flux system customers are
  encouraged to contact LI-COR directly for our commercial EDDYPRO®
  End User License Agreement.

  EDDYPRO® contains Open Source Components (as defined in the
  End User License Agreement). The licenses and/or notices for the
  Open Source Components can be found in the file LIBRARIES.txt.

  EddyPro® is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 ***************************************************************************/

#ifndef DLINIDIALOG_H
#define DLINIDIALOG_H

#include <QDialog>
#include <QFileDialog>

class QAbstractButton;
class QKeyEvent;
class QPushButton;
class QTabWidget;

struct ConfigState;
class DlInstrTab;
class DlProject;
class DlRawfileDescTab;
class DlSiteTab;

class DlIniDialog : public QDialog
{
    Q_OBJECT

public:
    DlIniDialog(QWidget *parent, DlProject *dlProject, ConfigState* config);

    inline QString iniFilename() { return filename_; }
    inline DlRawfileDescTab* rawFileDescTab() { return rawfile_tab; }
    inline DlProject *dlProject() { return dlProject_; }
    inline void setNewFlag(bool b) { newFlag_ = b; }
    bool requestMetadataReset();

signals:
    void metadataFileSaved(const QString &);
    void mdFileEditClearRequest();

public slots:
    bool openFile(const QString &fileName = QString(), bool embedded = true);
    void refresh();
    void reset();
    void defaults();
    void defaults_2();
    void saveAvailable();

private slots:
    void accept() override;
    void disableResetButton(bool ok);
    void saveAsButtonClicked();

private:
    void apply();
    bool saveFile(const QString& filename);
    void fileSaveAs();

    QTabWidget *tabwidget_;
    DlSiteTab *site_tab;
    DlInstrTab *instruments_tab;
    DlRawfileDescTab *rawfile_tab;

    QPushButton *saveAsButton;
    QPushButton *resetButton;

    QString filename_;
    bool newFlag_;

    DlProject *dlProject_;
    ConfigState *configState_;
};

#endif // DLINIDIALOG_H
