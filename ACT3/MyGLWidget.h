#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include <QMouseEvent>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glm/gtc/matrix_inverse.hpp"

#include "model.h"


#define NUM_ESPELMES 2

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
    //--------------------------------------------
    // Inicialitzacions
    void iniEscena ();
    void iniCamera ();
    //--------------------------------------------
    // Creació de buffers
    void creaBuffersModel(Model &model, const char *fileName, GLuint *VAO, float &escala, glm::vec3 &centreBase);
    void creaBuffersEspelma();
    void creaBuffersSnitch();
    void creaBuffersBackground();
    void creaBuffersFinestra();
    //--------------------------------------------
    void carregaShaders ();
    //--------------------------------------------
    void projectTransform ();
    void viewTransform ();
    //--------------------------------------------
    // Matrius de transformació de cada objecte
    void modelTransformBackground();
    void modelTransformFinestres();
    void modelTransformEspelma(int i);
    void modelTransformSnitch(int i);
    //--------------------------------------------
    void calculaCapsaModel (Model &m, float &escala, glm::vec3 &centreBase);


    //--------------------------------------------------------------
    //          GLOBALS D'ESCENA
    glm::vec3 centreEsc;
    float radiEsc, ra;
    GLint ample, alt;

    //--------------------------------------------------------------
    //          SHADERS
    //--------------------------------------------------------------
    QOpenGLShaderProgram *program;
    //--------------------------------------------------------------
    // uniform locations    
    GLuint transLoc, projLoc, viewLoc;
    //--------------------------------------------------------------
    // attribute locations
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc;

    //--------------------------------------------------------------
    // VAO names
    GLuint VAO_Espelma, VAO_snitch, VAO_background, VAO_Finestra;
    // model
    Model modelEspelma, modelSnitch, modelBackground, modelFinestra;
    // paràmetres calculats a partir de la capsa contenidora del model
    glm::vec3 centreBaseEspelma, centreBaseSnitch, centreBaseBackground, centreBaseFinestra;
    float escalaEspelma, escalaSnitch,escalaBackground,escalaFinestra;
    //--------------------------------------------------------------
    // Posicions de les espelmes
    glm::vec3 EspelmaPos[NUM_ESPELMES], focusBasePos;
    // Angles dels snitchs
    int AngleSnitch[NUM_ESPELMES];
    // Índex de l'espelma activa
    int indexEspelmaActiva;
    // Posició del focus fix
    glm::vec4 fixaFocusPos;

    //--------------------------------------------------------------
    // MATRIUS DE TRANSFORMACIÓ
    glm::mat4 View, Espelma_TG, Snitch_TG;

    //--------------------------------------------------------------
    // INTERACCIÓ
    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
    float angleX, angleY;


     //--------------------------------

    GLuint llumAmbientLoc, colorFocusLoc, posFocusLoc, normalMatrixLoc;
    void iniLlum();
    glm::mat3 normalMatrix;
    glm::vec3 posFocusSnitchs;
    GLuint snitchFocusEsqLoc, snitchFocusDretLoc, colorFocusSnitchEsqLoc, colorFocusSnitchDretLoc, opacitatLoc;

    glm::vec3 posFixaSnitchEsq, posFixaSnitchDret;

    int valActualRot;

    bool apagarEsq, apagarDret;

  public slots: // info de interficie -> MyGLWidget.
    void setEspelmaEsq();
    void setEspelmaDreta();
    void setEncendreApagar(bool);
    void setValorRotacio(int val);

  signals: // info de MyGLWidget -> interficie.
    void enviaEspelmaActivaEsq();
    void enviaEspelmaActivaDreta();
    void enviaValActualRot(int angle);
};

