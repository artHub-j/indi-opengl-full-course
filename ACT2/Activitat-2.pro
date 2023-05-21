TEMPLATE    = app
QT         += opengl  \
    widgets
CONFIG += c++11

INCLUDEPATH +=  /usr/include/glm  \
                ./Model 

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h \
    customradiobutton.h

SOURCES += main.cpp MyForm.cpp \
           MyGLWidget.cpp  \
           ./Model/model.cpp \
           customradiobutton.cpp
