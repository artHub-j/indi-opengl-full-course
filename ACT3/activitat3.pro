TEMPLATE    = app
QT         += opengl

INCLUDEPATH +=  /usr/include/glm
INCLUDEPATH += ./Model

FORMS += MyForm.ui

HEADERS += MyForm.h MyGLWidget.h

SOURCES += main.cpp  MyForm.cpp \
        MyGLWidget.cpp ./Model/model.cpp

DISTFILES += \
    models/MINITUNNEL.mtl \
    models/f-16.mtl \
    shaders/basicLlumShader.frag \
    shaders/basicLlumShader.vert
