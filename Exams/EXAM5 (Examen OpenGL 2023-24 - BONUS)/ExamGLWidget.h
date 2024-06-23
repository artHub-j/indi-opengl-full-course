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

// definim el numero total de models a carregar
#define NUM_MODELS 3

/**
 * @author Arturo Aragón Hidalgo - https://github.com/artHub-j/indi-opengl-full-course
 */
class ExamGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core 
{
  Q_OBJECT

  public:
    ExamGLWidget (QWidget *parent=0);
    ~ExamGLWidget ();

  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ( );
    // resizeGL - És cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);
    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);
    // mouse{Press/Release/Move}Event - Són cridades quan es realitza l'event 
    // corresponent de ratolí
    virtual void mousePressEvent (QMouseEvent *event);
    virtual void mouseReleaseEvent (QMouseEvent *event);

    // model transforms
    virtual void modelTransformMorty ();
    virtual void modelTransformCoin ();
    virtual void modelTransformFantasma ();
    virtual void modelTransformGroundBlock ();


    virtual void iniEscena ();
    virtual void iniCamera ();
    
    virtual void projectTransform ();
    virtual void viewTransform ();

    virtual void carregaShaders ();

    void creaBuffersCub();
    void iniMaterialCub();
        // Definim els paràmetres del material del cub
    glm::vec3 amb, diff, spec;
    float shin;

    // creaBuffersModels - Aquí carreguem els fitxers obj i fem la inicialització dels 
    // diferents VAOS i VBOs
    void creaBuffersModels ();    
    // calculaCapsaModel - Calcula la capsa contenidora d'un Model p retornant el centre 
    // de la seva base a CentreBase, i el factor d'escala necessari per a que la seva alçada
    // sigui alcadaInicial.
    void calculaCapsaModel (Model &p, float &escala, float alcadaInicial, 
                            glm::vec3 &CentreBase);

    void pintaEscenari ();


    // enum models - els models estan en un array de VAOs (VAO_models), aquest enum és 
    // per fer més llegible el codi. 
    typedef enum { COIN = 0, FANTASMA = 1, MORTY = 2} ModelType;
    	
    std::string objNames[NUM_MODELS] = {"Coin.obj", "Fantasma.obj", "Morty.obj"};
    	
    float alcadesInicials[NUM_MODELS] = {1, 1, 1};

    // VAO names
    GLuint VAO_models[NUM_MODELS];
    GLuint VAO_Cub;

    // Models, capses contenidores i escales
    Model models[NUM_MODELS];
    glm::vec3 centreBaseModels[NUM_MODELS];        
    float escalaModels[NUM_MODELS];
    
    // Program
    QOpenGLShaderProgram *program;
    // Viewport
    GLint ample, alt;
    // uniform locations
    GLuint transLoc, projLoc, viewLoc;
    // attribute locations
    GLuint vertexLoc, normalLoc, matambLoc, matdiffLoc, matspecLoc, matshinLoc;

    // Matriu de posició i orientació
    glm::mat4 View;

    glm::vec3 centreEsc;
    float radiEsc, ra, fov, zn, zf, d, angleini;

    float angleX, angleY;

    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;
	     
    // timer
    QTimer timerCoins;

};

