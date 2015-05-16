
#include "PushButton.h"

PushButton::PushButton(const QString & text, QWidget *parent,
    const char * method, const QObject * receiver) :
    QPushButton(text)
{
    connectClickedSignal(parent, method, receiver);
}

void PushButton::connectClickedSignal(QWidget *parent, const char * method, const QObject * receiver)
{
    if (method != NULL){
        if (receiver != NULL)
            connect(this, SIGNAL(clicked()), receiver, method);
        else
            connect(this, SIGNAL(clicked()), parent, method);
    }
}
