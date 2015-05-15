
#ifndef ACTION_H
#define ACTION_H

#include "stable.h"

class Action : public QAction
{
    Q_OBJECT

public:
    Action(const QString & text, const QString & statusTip,
        QWidget *parent, const char * method = NULL, const QObject * receiver = NULL);

    Action(const QString & text, const QString & statusTip, const QIcon & icon,
        QWidget *parent, const char * method = NULL, const QObject * receiver = NULL);

    Action(const QString & text, const QString & statusTip,QList<QKeySequence> keyList,
        QWidget *parent, const char * method = NULL, const QObject * receiver = NULL);

    Action(const QString & text, const QString & statusTip,QKeySequence::StandardKey key,
        QWidget *parent, const char * method = NULL, const QObject * receiver = NULL);

    Action(const QString & text, const QString & statusTip,
        QList<QKeySequence> keyList, const QIcon & icon,
        QWidget *parent, const char * method = NULL, const QObject * receiver = NULL);

    Action(const QString & text, const QString & statusTip,
        QKeySequence::StandardKey key, const QIcon & icon,
        QWidget *parent, const char * method = NULL, const QObject * receiver = NULL);

private:
    void connectTriggeredSignal(QWidget *parent, const char * method, const QObject * receiver);

};

#endif
