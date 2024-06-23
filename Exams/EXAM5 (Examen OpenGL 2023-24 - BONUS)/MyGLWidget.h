#include "ExamGLWidget.h"

/**
 * @author Arturo Aragón Hidalgo - https://github.com/artHub-j/indi-opengl-full-course
 */
class MyGLWidget:public ExamGLWidget
{
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0);
    ~MyGLWidget();

  protected:
    virtual void paintGL ();
    virtual void resizeGL (int width, int height);
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void mouseMoveEvent (QMouseEvent *event);

    virtual void iniEscena ();
    virtual void iniCamera ();
    virtual void projectTransform ();
    virtual void viewTransform ();
    
    virtual void modelTransformMorty();
    virtual void modelTransformFantasma();
    virtual void modelTransformCoin (glm::vec3);

    bool girant=true;

    int laberint[20][15]={
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
         1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1,
         1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1,
         1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1,
         1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1,
         1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1,
         1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1,
         1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1,
         1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1,
         1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1,
         1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
         1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1,
         1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1,
         1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1,
         1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1,
         1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1,
         1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1,
         1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1,
         1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
         1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };



  public slots:
    void rotateCoins();

  private:
    int printOglError(const char file[], int line, const char func[]);

    void updateFantasmaPos(glm::vec3 dirM);
    
    glm::vec3 posMorty;
    float angleMorty;
    glm::vec3 dirMorty;
    glm::vec2 posMortyLab;
    
    glm::vec3 posFantasma;
    float angleCoins, angleFantasma;
    glm::vec2 posFantasmaLab;
    glm::vec3 dirFantasma;

    int numfiles=15;
    int numcolumnes=20;

    void carregaShaders();

    //------------ Nous Atributs, Funcions i Slots/Signals

    std::vector<std::pair<glm::vec3, bool>> posicionsDisponibles;
    std::vector<glm::vec3> posCoinsFinals;
    GLuint posFocusLoc;
    glm::vec4 posFocus;
    glm::vec3 obs, vrp, up;
    bool primeraPersona;
    bool focusEscena;
    int puntuacio;

    void creaCapsaContenidora(glm::vec3, glm::vec3);
    void modelTransformParet(glm::vec3);
    void modelTransformTerra(glm::vec3);
    void enviaPosFocus();
    void updatePuntuacioMonedes();

  public slots:
    void atura();
    void engega();
    void setCamPrimeraPersona();
    void setCamEscena();
    void setFocusCamera();
    void setFocusMorty();
    void reset();

  signals:
    void enviaEstatCamera(bool);
    void enviaEstatFocus(bool);
    void updatePuntuacio(int);
    void updateVictoria(QString);
};
