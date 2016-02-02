/****************************************************************************
**
** Copyright (C) 2006 Trolltech AS. All rights reserved.
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** This file may be used under the terms of the GNU General Public License
** version 2.0 as published by the Free Software Foundation or under the
** terms of the Qt Commercial License Agreement. The respective license
** texts for these are provided with the open source and commercial
** editions of Qt.
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef FADERWIDGET_H
#define FADERWIDGET_H

#include <QWidget>
#include <QTimeLine>

////////////////////////////////////////////////////////////////////////////////
/// \file src/faderwidget.h
/// \brief FaderWidget class
/// \version     1.0
/// \date        2006-2007
/// \author      Trenton Schulz, Trolltech AS
/// \note        Modified by Antonio Forgione, 2011-2016
/// \sa http://labs.qt.nokia.com/2007/08/21/fade-effects-a-blast-from-the-past/
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

class QTimeLine;
/// \class FaderWidget
/// \brief Class that provides fading effect
class FaderWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QBrush fadeBrush READ fadeBrush WRITE setFadeBrush)
    Q_PROPERTY(int fadeDuration READ fadeDuration WRITE setFadeDuration)

public:
    explicit FaderWidget(QWidget *parent);

    QBrush fadeBrush() const { return startBrush; }
    void setFadeBrush(const QBrush &newColor) { startBrush = newColor; }

    int fadeDuration() const { return timeLine->duration(); }
    void setFadeDuration(int milliseconds) { timeLine->setDuration(milliseconds); }

    void start();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    QTimeLine *timeLine;
    QBrush startBrush;
};

#endif
