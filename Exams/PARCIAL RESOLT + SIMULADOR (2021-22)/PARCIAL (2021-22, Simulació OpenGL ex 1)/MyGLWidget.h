#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "./Model/model.h"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    // mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event
    // corresponent de ratolí
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

  private:
    void carregaShaders();
    void creaBuffersCub(), modelTransformCubA(), modelTransformCubB(), pintaCub();
    void creaBuffersTerra(), modelTransformTerra(), pintaTerra();
    void projectTransform(), viewTransform();

    void ini_camera(), drawAxis();

    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // VAO i VBO names
    GLuint VAO_Cub, VAO_Terra;
    // Program
    QOpenGLShaderProgram *program;
    // Viewport
    GLint ample, alt;
    // Internal vars
    float scale, gir, alcada;
    glm::vec3 pos;

    Model m;

    glm::vec3 OBS, VRP, UP;
    float FOV, raw, znear, zfar;

    glm::vec3 centreEscena, centreBasePatricio;
    float radiEscena, alpha_v, rav, escalaPatricio;
    bool ortho;
    float left, right, bottom, top;

    // Mouse interaction
    typedef enum
    {
      NONE,
      ROTATE,
      ZOOM
    } InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;

    float anglePsi, angleTheta;
};

