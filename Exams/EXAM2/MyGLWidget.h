#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
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

  private:
    void creaBuffersTerra ();
    void creaBuffersCub ();
    void creaBuffersCub2 ();
    void creaBuffersCotxe ();
    void carregaShaders ();
    void projectTransform ();
    void viewTransform ();
    void modelTransformPorta ();
    void modelTransformTerra ();
    void modelTransformParet1 ();
    void modelTransformParet2 ();
    void modelTransformParet3 ();
    void modelTransformCotxe ();
    void calculaCapsaModel (Model &p, float &escala, glm::vec3 &centreBase);
    void iniEscena ();
    void iniCamera ();

    // VAO names
    GLuint VAO_Cub, VAO_Cub2, VAO_Terra, VAO_Cotxe;
    // Program
    QOpenGLShaderProgram *program;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // attribute locations
    GLuint vertexLoc, colorLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc;

    GLint ample, alt;
    
    // model
    Model cotxe;
    // paràmetres calculats a partir de la capsa contenidora del model
    glm::vec3 centreBaseCotx;
    float escalaCotx;

    glm::vec3 centreEsc, obs, vrp, up, posFocus;
    float radiEsc, ra, fov, zn, zf, FOVini;

    float angleX, angleY;

    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;

    void creaCapsaContenidoraEscena(glm::vec3, glm::vec3);
    void enviaPosFocus();
    bool focusEscena, portaOberta;

    glm::mat4 View, TG_Moviment;

    GLuint posFocusLoc;
    float movCotxeX, oberturaPorta, d;

  public slots: // Interficie -> MyGLWidget

    void setOberturaPorta(int val);
    void setMovimentCotxe(int val);

  signals: // MyGLWidget -> Actualitza Interficie

    void enviaOberturaPorta(int val);
};

