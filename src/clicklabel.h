/***************************************************************************
  clicklabel.h
  ------------
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
****************************************************************************/

#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>
#include <QMetaType>
#include <QTime>

////////////////////////////////////////////////////////////////////////////////
/// \file src/clicklabel.h
/// \brief
/// \version
/// \date
/// \author      Antonio Forgione
/// \note
/// \sa
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QMouseEvent;

/// \class ClickLabel
/// \brief QLabel class with clicked signal, derived from QLabel
class ClickLabel : public QLabel
{
    Q_OBJECT

    // property system for question mark clickalabel in metadata editor tables
    Q_PROPERTY(HeaderData headerData READ headerData WRITE setHeaderData)
    Q_ENUMS(HeaderData)

public:
    ClickLabel(QWidget* parent = nullptr, Qt::WindowFlags flags = nullptr);
    ClickLabel(const QString& text, QWidget* parent = nullptr, Qt::WindowFlags flags = nullptr);
    ClickLabel(const ClickLabel& clabel);
    ClickLabel& operator=(const ClickLabel& clabel);

    enum HeaderData {
        NoHeader = 0,
        AnemSwVersion,
        AnemNAlign,
        AnemNOffset,
        AnemNSep,
        AnemESep,
        AnemVSep,
        IrgaModel = 10,
        IrgaLPathLength,
        IrgaTPathLength,
        VarIgnoreDesc = 20,
        VarNumericDesc,
        VarDesc,
        VarConv,
        VarMinValue,
        VarMaxValue,
        VarNomTLag,
        VarMinTLag,
        VarMaxTLag
    };

    HeaderData headerData() const { return headerData_; }
    void setHeaderData(HeaderData data);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QTime time;
    HeaderData headerData_;
};

Q_DECLARE_METATYPE(ClickLabel)

#endif // CLICKLABEL_H
