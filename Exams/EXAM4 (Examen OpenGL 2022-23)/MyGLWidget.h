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
    
    virtual void modelTransformCub (float alcada, float angle, float radi, float z);
    virtual void modelTransformFantasma ();
    virtual void iniMaterialCub ();
    virtual void iniEscena ();
    virtual void iniCamera ();
    virtual void projectTransform ();
    virtual void viewTransform ();

    
  public slots:
    void girarRoda();
    void atura();
    void engega();

  private:
    void creaBuffersFantasma ();
    void creaBuffersCub ();
    void carregaShaders ();

    void calculaCapsaModelFantasma ();

    // VAO names
    GLuint VAO_Fantasma, VAO_Cub;
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

    // models
    Model fantasma;
    // paràmetres calculats a partir de la capsa contenidora dels models
    glm::vec3 centreBaseFantasma;
    float escalaFantasma;

    glm::vec3 centreEsc;
    float radiEsc, ra, fov, zn, zf;

    // Definim els paràmetres del material del cub
    glm::vec3 amb, diff, spec;
    float shin;

    float angleX, angleY;

    typedef  enum {NONE, ROTATE} InteractiveAction;
    InteractiveAction DoingInteractive;
    int xClick, yClick;

    // configuració alçades cubs per rodes
    int alcades[3][72] = {
{0,0,0,4,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,0,0,0,2,4,4,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,3,3,0,0,3,0,0,0,0,0,0,4,4,4,0,0,0,0,0,4,0},
		{0,0,3,0,0,0,0,0,4,5,5,0,0,0,3,3,4,0,0,0,0,4,3,5,0,0,0,0,0,0,0,0,0,2,3,4,0,0,6,4,5,0,0,0,0,0,0,6,5,4,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,4,4,0,0,0,0},
		{0,0,0,0,0,5,0,0,0,0,0,0,0,5,0,0,0,0,0,4,4,4,0,0,4,0,0,2,3,4,0,0,0,0,0,0,0,5,0,0,4,4,4,0,4,4,4,5,0,0,0,0,4,5,5,0,6,5,0,0,5,5,5,0,4,0,0,0,5,5,0,0}
	     };
	     
    // per moure la roda     
    QTimer timer;
    float angleRoda;
    float velocitatRoda;
    bool girant;

    int printOglError(const char file[], int line, const char func[]);




    void creaCapsaContenidoraEscena(glm::vec3 pMin, glm::vec3 pMax);
    void enviaPosFocus();
    GLuint posFocusLoc;
    glm::vec3 obs, vrp, up;
    glm::vec4 posFocus;
    bool camera_nova;
    float posFantasmaZ;

public slots:
    void setCamera();
signals:
    void enviaEstatCamera(bool);
};

