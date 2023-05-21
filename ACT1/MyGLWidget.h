#define GLM_FORCE_RADIANS
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QKeyEvent>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
  Q_OBJECT

  public:
    MyGLWidget (QWidget *parent=0);
    ~MyGLWidget ();
    
  protected:
    // initializeGL - Aqui incluim les inicialitzacions del contexte grafic.
    virtual void initializeGL ();

    // paintGL - Mètode cridat cada cop que cal refrescar la finestra.
    // Tot el que es dibuixa es dibuixa aqui.
    virtual void paintGL ();
 
    // resize - Es cridat quan canvia la mida del widget
    virtual void resizeGL (int width, int height);  

    // keyPressEvent - Es cridat quan es prem una tecla
    virtual void keyPressEvent (QKeyEvent *event);

  private:
    void creaBuffersCapICos();
    void creaBuffersBrac();
    void creaBuffersCama();
    void creaBuffersCorona();
    
    void carregaShaders();

    void pintaCapICos();
    void pintaBrac();
    void pintaCama();
    void pintaCorona();
    void pintaNino();

    
    void modelTransformBracEsquerre();
    void modelTransformBracDret();
    void modelTransformCamaEsquerra();
    void modelTransformCamaDreta();
    void modelTransformCapICos();
    void modelTransformCorona();
    
    // program
    QOpenGLShaderProgram *program;
    // attribute locations
    GLuint vertexLoc, colorLoc;
    // uniform locations
    GLuint TGLoc;

    // VAOs
    GLuint VAOCapICos, VAOBrac, VAOCama, VAOCorona;

    // viewport
    GLint ample, alt;
	    
    // colors
    glm::vec3 colorCarn = glm::vec3( 1.0, 0.8, 0.6);
    glm::vec3 colorCos = glm::vec3( 0.0, 0.0, 0.5);
    glm::vec3 colorManiga = glm::vec3( 0.25, 0.25, 0.25);
    glm::vec3 colorPantalo = glm::vec3( 0.25, 0.25, 0.25);
    glm::vec3 colorSabata = glm::vec3( 0.0, 0.0, 0.0);
    glm::vec3 colorCorona = glm::vec3( 1.0, 1.0, 0.0);
    glm::vec3 colorCabell = glm::vec3( 0.0, 0.0, 0.0);
    glm::vec3 colorUlls = glm::vec3( 0.0, 0.0, 0.0);

    // Atributs nous Alumne: ------------------------------

    GLuint hooliganModeLoc;
    int hooliganMode = 0; // Indica l'estat al premer la tecla C:
                              // Estat 0: Braços baixats. Estat inicial.
                              // Estat 1: Braços baixats + Corona.
                              // Estat 2: Braços amunt + Corona + Samarreta
    float rotacio = 0.f;
};
