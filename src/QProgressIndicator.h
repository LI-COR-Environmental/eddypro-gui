/***************************************************************************
  QProgressIndicator.h
  -------------------
  QProgressIndicator class
  -------------------
  Copyright (C) 2009 Morgan Leborgne

  from http://qt-apps.org/content/show.php/QProgressIndicator?content=115762
  License: LGPL

  Modified by Antonio Forgione, 2011
****************************************************************************/

#ifndef QPROGRESSINDICATOR_H
#define QPROGRESSINDICATOR_H

#include <QColor>
#include <QWidget>

////////////////////////////////////////////////////////////////////////////////
/// \file src/qprogressindicator.h
/// \brief QProgressIndicator class
/// \version     1.0.2
/// \date        2009-2010
/// \author      Morgan Leborgne
/// \note        Modified for EddyPro by Antonio Forgione
/// \sa http://qt-apps.org/content/show.php/QProgressIndicator?content=115762
/// \bug
/// \deprecated
/// \test
/// \todo
////////////////////////////////////////////////////////////////////////////////

/*!
    \class QProgressIndicator
    \brief The QProgressIndicator class lets an application display a progress indicator to show that a lengthy task is under way.

    Progress indicators are indeterminate and do nothing more than spin to show that the application is busy.
    \sa QProgressBar
*/
class QProgressIndicator : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int delay READ animationDelay WRITE setAnimationDelay)
    Q_PROPERTY(bool displayedWhenStopped READ isDisplayedWhenStopped WRITE setDisplayedWhenStopped)
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    explicit QProgressIndicator(QWidget* parent = 0);

    /*! Returns the delay between animation steps.
        \return The number of milliseconds between animation steps. By default, the animation delay is set to 40 milliseconds.
        \sa setAnimationDelay
     */
    int animationDelay() const { return m_delay; }

    /*! Returns a Boolean value indicating whether the component is currently animated.
        \return Animation state.
        \sa startAnimation stopAnimation
     */
    bool isAnimated () const;

    /*! Returns a Boolean value indicating whether the receiver shows itself even when it is not animating.
        \return Return true if the progress indicator shows itself even when it is not animating. By default, it returns false.
        \sa setDisplayedWhenStopped
     */
    bool isDisplayedWhenStopped() const;

    /*! Returns the color of the component.
        \sa setColor
      */
    const QColor & color() const { return m_color; }

    virtual QSize sizeHint() const;
    int heightForWidth(int w) const;
public slots:
    /*! Starts the spin animation.
        \sa stopAnimation isAnimated
     */
    void startAnimation();

    /*! Stops the spin animation.
        \sa startAnimation isAnimated
     */
    void stopAnimation();

    /*! Sets the delay between animation steps.
        Setting the \a delay to a value larger than 40 slows the animation, while setting the \a delay to a smaller value speeds it up.
        \param delay The delay, in milliseconds.
        \sa animationDelay
     */
    void setAnimationDelay(int delay);

    /*! Sets whether the component hides itself when it is not animating.
       \param state The animation state. Set false to hide the progress indicator when it is not animating; otherwise true.
       \sa isDisplayedWhenStopped
     */
    void setDisplayedWhenStopped(bool state);

    /*! Sets the color of the components to the given color.
        \sa color
     */
    void setColor(const QColor & color);
protected:
    virtual void timerEvent(QTimerEvent * event);
    virtual void paintEvent(QPaintEvent * event);
private:
    int m_angle;
    int m_timerId;
    int m_delay;
    bool m_displayedWhenStopped;
    QColor m_color;
};

#endif // QPROGRESSINDICATOR_H
