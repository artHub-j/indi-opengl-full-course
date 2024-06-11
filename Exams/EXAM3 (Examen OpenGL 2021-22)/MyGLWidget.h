#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "model.h"

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
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);
    virtual void mouseMoveEvent (QMouseEvent *event);
    
    virtual void pintaArbre (glm::vec3 pos);
    virtual void iniMaterialTronc ();
    virtual void modelTransformTerra ();
    virtual void modelTransformPatricio ();
    virtual void modelTransformTronc (glm::vec3 pos);
    virtual void modelTransformCopa (glm::vec3 pos);
    virtual void enviaPosFocus ();
    virtual void iniEscena ();
    virtual void iniCamera ();
    virtual void projectTransform ();
    virtual void viewTransform ();

    void creaBuffersPatricio ();
    void creaBuffersTerra ();
    void creaBuffersCub ();
    void creaBuffersEsfera ();
    void carregaShaders ();
    void calculaCapsaModel (Model &p, float &escala, glm::vec3 &centreBase);

    // VAO names
    GLuint VAO_Patr, VAO_Cub, VAO_Terra, VAO_Esf;
    // Program
    QOpenGLShaderProgram *program;
    // Viewport
    GLint ample, alt;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc, posfocusLoc;
    // attribute locations
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc;

    // Matriu de posició i orientació
    glm::mat4 View;
    // Matriu de transformació del model
    glm::mat4 TG;

    // models
    Model patr, esfera;
    // paràmetres calculats a partir de la capsa contenidora dels models
    glm::vec3 centreBasePat, centreBaseEsf;
    float escala, escalaEsf;

    glm::vec3 centreEsc;
    float radiEsc, ra, raw, fov, zn, zf;

    // Definim els paràmetres del material del terra
    glm::vec3 amb, diff, spec;
    float shin;

    glm::vec3 posFoc;
    float angleX, angleY;

    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;


    glm::vec3 vrp, obs, up;

    void creaCapsaContenidora(glm::vec3, glm::vec3);

    float alpha_v;

    bool focusDeCamera;

    float rotacio;
};

