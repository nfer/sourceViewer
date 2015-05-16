
#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include "stable.h"

class PushButton : public QPushButton
{
    Q_OBJECT

public:
    PushButton(const QString & text, QWidget *parent,
        const char * method = NULL, const QObject * receiver = NULL);

private:
    void connectClickedSignal(QWidget *parent, const char * method, const QObject * receiver);

};

#endif
