/***************************************************************************
  updatedialog.h
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

#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

class FtpManager;
class QLabel;
class QTimer;

class UpdateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit UpdateDialog(QWidget *parent = 0);
    ~UpdateDialog();

    void checkUpdate();
    bool hasNewVersion();

signals:

public slots:
    void close();
    void initialize();

private slots:
    void showDownloadPage();
    void ftpTimeout();

    void useFtpResults();
private:
    QPushButton *okButton;
    QPushButton *yesButton;
    QPushButton *noButton;
    QLabel *msgLabel;

    FtpManager* ftp;

    void getNewVersion(const QString& version);
    void noNewVersion();
    void noConnection();
    void connectionError();
    void downloadError();

    bool isNewVersionAvailable_;

    QTimer* ftpTimer_;
};

#endif // UPDATEDIALOG_H
