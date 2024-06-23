TEMPLATE    = app
QT         += opengl 

INCLUDEPATH +=  /usr/include/glm
INCLUDEPATH +=  /home/artuaragon/INDI2/Model

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h

SOURCES += main.cpp MyForm.cpp \
        MyGLWidget.cpp

SOURCES += /home/artuaragon/INDI2/Model/model.cpp
