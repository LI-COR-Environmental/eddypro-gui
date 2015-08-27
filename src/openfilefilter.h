#ifndef OPENFILEFILTER_H
#define OPENFILEFILTER_H

#include <QObject>

class OpenFileFilter : public QObject
{
    Q_OBJECT

public:
    explicit OpenFileFilter(QObject *parent = 0);
    const QString& fileRequested() const;

protected:
    bool eventFilter(QObject* object, QEvent* event) Q_DECL_OVERRIDE;

private:
    QString file_requested_;
};

#endif // OPENFILEFILTER_H
