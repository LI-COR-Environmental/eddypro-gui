/***************************************************************************
 *   Copyright (C) 2008-2010 by Matthias Pospiech & Matthias Nagl          *
 *   mnagl@uni-goettingen.de                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef QSCIENCESPINBOX_H
#define QSCIENCESPINBOX_H

#include <QDoubleSpinBox>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QVariant>
#include <QDebug>
#include <QString>

/*!
 * \author Matthias Pospiech (http://www.matthiaspospiech.de/blog/2009/01/03/qt-spinbox-widget-with-scientific-notation/)
 * \author modified by Matthias Nagl <mnagl@uni-goettingen.de>
 * \ingroup pacmaster
 * \brief Enhanced QDoubleSpinbox featuring exponential representation
 */
class QScienceSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
  public:
    explicit QScienceSpinBox(QWidget * parent = 0);

    int decimals() const;
    void setDecimals(int value);

    QString textFromValue(double value) const;
    double valueFromText(const QString & text) const;

  private:
    int dispDecimals;
    QChar delimiter, thousand;
    QDoubleValidator * v;

  private:
    void initLocalValues(QWidget *parent);
    bool isIntermediateValue(const QString &str) const;
    QVariant validateAndInterpret(QString &input, int &pos, QValidator::State &state) const;
    QValidator::State validate(QString &text, int &pos) const;
    void fixup(QString &input) const;
    QString stripped(const QString &t, int *pos) const;
    double round(double value) const;
    void stepBy(int steps);

  public slots:
    void stepDown();
    void stepUp();
};

#endif
