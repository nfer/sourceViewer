
#include "Action.h"

Action::Action(const QString & text, const QString & statusTip,
        QWidget *parent, const char * method, const QObject * receiver) :
    QAction(text, parent)
{
    connectTriggeredSignal(parent, method, receiver);
    setStatusTip(statusTip);
}

Action::Action(const QString & text, const QString & statusTip, const QIcon & icon,
        QWidget *parent, const char * method, const QObject * receiver) :
    QAction(icon, text, parent)
{
    connectTriggeredSignal(parent, method, receiver);
    setStatusTip(statusTip);
}

Action::Action(const QString & text, const QString & statusTip, QList<QKeySequence> keyList,
        QWidget *parent, const char * method, const QObject * receiver) :
    QAction(text, parent)
{
    connectTriggeredSignal(parent, method, receiver);
    setStatusTip(statusTip);
    setShortcuts(keyList);
}

Action::Action(const QString & text, const QString & statusTip, QKeySequence::StandardKey key,
        QWidget *parent, const char * method, const QObject * receiver) :
    QAction(text, parent)
{
    connectTriggeredSignal(parent, method, receiver);
    setStatusTip(statusTip);
    setShortcuts(key);
}

Action::Action(const QString & text, const QString & statusTip,
        QList<QKeySequence> keyList, const QIcon & icon,
        QWidget *parent, const char * method, const QObject * receiver) :
    QAction(icon, text, parent)
{
    connectTriggeredSignal(parent, method, receiver);
    setStatusTip(statusTip);
    setShortcuts(keyList);
}

Action::Action(const QString & text, const QString & statusTip,
        QKeySequence::StandardKey key, const QIcon & icon,
        QWidget *parent, const char * method, const QObject * receiver) :
    QAction(icon, text, parent)
{
    connectTriggeredSignal(parent, method, receiver);
    setStatusTip(statusTip);
    setShortcuts(key);
}

void Action::connectTriggeredSignal(QWidget *parent, const char * method, const QObject * receiver)
{
    if (method != NULL){
        if (receiver != NULL)
            connect(this, SIGNAL(triggered()), receiver, method);
        else
            connect(this, SIGNAL(triggered()), parent, method);
    }
}
