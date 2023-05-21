#ifndef CUSTOMRADIOBUTTON_H
#define CUSTOMRADIOBUTTON_H

#include <QWidget>
#include <QRadioButton>
#include "MyGLWidget.h"

class CustomRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    explicit CustomRadioButton(QWidget *parent = nullptr);

signals:

};

#endif // CUSTOMRADIOBUTTON_H
