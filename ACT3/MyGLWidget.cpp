#include "MyGLWidget.h"
#include <glm/gtx/color_space.hpp>
#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
  apagarEsq = false;
  apagarDret = false;
  valActualRot = 0;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.0, 0.0, 0.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);

  carregaShaders();
  iniEscena ();
  iniCamera ();
}

void MyGLWidget::iniEscena ()
{
  creaBuffersEspelma();
  creaBuffersSnitch();
  creaBuffersBackground();
  creaBuffersFinestra();

  // Primera espelma activa
  indexEspelmaActiva=0;

  //------------------------------------------------
  // Posició de les espelmes
  EspelmaPos[0] = glm::vec3(-5.5, 5.0, -5.0);
  EspelmaPos[1] = glm::vec3(-0.5, 5.0, -5.0);
  // Angles incials snitchs
  AngleSnitch[0]=AngleSnitch[1]=0;
  //--------------------------------------------------
  centreEsc = glm::vec3 (0, 0, 0);

  fixaFocusPos = glm::vec4(0,6,5,1);

  radiEsc = 15;

  // Posicions inicials dels focus de cada Snitch:
  posFixaSnitchEsq =  glm::vec4(EspelmaPos[0] + glm::vec3(1.0, 2.6, 0.0), 1);
  //glUniform3fv(snitchFocusEsqLoc, 1, &posFixaSnitchEsq[0]);
  posFixaSnitchDret = glm::vec4(EspelmaPos[1] + glm::vec3(1.0, 2.6, 0.0), 1);
  //glUniform3fv(snitchFocusDretLoc, 1, &posFixaSnitchDret[0]);
}

void MyGLWidget::iniCamera ()
{
  ra = 1.0;
  angleY = 0.0;
  angleX = 0.2;

  projectTransform ();
  viewTransform ();
}

void MyGLWidget::iniLlum()
{
    glm::vec3 llumAmbient = glm::vec3(0.1, 0.1, 0.1);
    glUniform3fv(llumAmbientLoc, 1, &llumAmbient[0]);
    glm::vec3 colorFocus = glm::vec3(0.6, 0.6, 0.6);
    glUniform3fv(colorFocusLoc, 1, &colorFocus[0]);
    glm::vec3 colorFocusSnitchs = glm::vec3(0.5, 0.5, 0.0);
    glUniform3fv(colorFocusSnitchEsqLoc, 1, &colorFocusSnitchs[0]);
    glUniform3fv(colorFocusSnitchDretLoc, 1, &colorFocusSnitchs[0]);
}

void MyGLWidget::paintGL () 
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#endif


    projectTransform ();
    viewTransform ();
    // Esborrem el frame-buffer i el depth-buffer

    glClearColor(0.1f, 0.05f, 0.1f, 1.f);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_ONE, GL_ZERO);


    //----------------------------------------------------
    // Envieu la informació de les llums
    // ... {TODO} ...

    iniLlum();

    //--------------------------------------------------------
    // Activem el VAO per a pintar les espelmes
    glBindVertexArray (VAO_Espelma);
    // pintem les espelmes, cadascuna amb el seu transform
    for(int i=0;i<NUM_ESPELMES;i++){
        modelTransformEspelma(i);
        glDrawArrays(GL_TRIANGLES, 0, modelEspelma.faces().size()*3);
    }
    //--------------------------------------------------------
    // Activem el VAO per a pintar els snitchs
    glBindVertexArray (VAO_snitch);
    // pintem les espelmes, cadascuna amb el seu transform
    for(int i=0;i<NUM_ESPELMES;i++){
        modelTransformSnitch(i);
        glDrawArrays(GL_TRIANGLES, 0, modelSnitch.faces().size()*3);
    }
    //------------------------------------------------------------
    //Pintem terra
    glBindVertexArray (VAO_background);
    modelTransformBackground ();
    glDrawArrays(GL_TRIANGLES, 0, modelBackground.faces().size()*3);
    //--------------------------------------------------------
    // Pintem les finestres
    glUniform1i(opacitatLoc, 1); // Indiquem al fragshader quan s'ha de modficiar la component a d'opacitat.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Activem el mode de barreja
    glBindVertexArray (VAO_Finestra);

    modelTransformFinestres();
    glDrawArrays(GL_TRIANGLES, 0, modelFinestra.faces().size()*3);
    glUniform1i(opacitatLoc, 0); // Una vegada pintada la finestra amb la component a = 0.5,
                                 // la resta de models es continuaran pintant amb la component a = 1.
    //--------------------------------------------------------

    glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
  ample = w;
  alt = h;
  projectTransform ();
}


void MyGLWidget::modelTransformBackground ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);

  normalMatrix = glm::inverseTranspose(glm::mat3(View));
  glUniformMatrix3fv (normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
}
void MyGLWidget::modelTransformFinestres ()
{
  glm::mat4 TG(1.f);  // Matriu de transformació
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);

  normalMatrix = glm::inverseTranspose(glm::mat3(View * TG));
  glUniformMatrix3fv (normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
}

void MyGLWidget::modelTransformSnitch(int indexSnitch)
{
    /*
     * Dividim la matriu TG dels Snitchs en matriu de moviment i matriu de Escala&Posicionament
     * per poder actualitzar la posicio del focus cada vegada que es mou l'snitch i l'Espelma actual segons
     * l'indexSnitch actual.
    */

    glm::mat4 TG_Moviment(1.0f);
    // 5. Traslladem el pivot de la rotacio anterior a la posicio de la vela indicada per rotar sobre ella.
    TG_Moviment = glm::translate(TG_Moviment, EspelmaPos[indexSnitch]);
    // 4. Rotem AngleSnitch[indexSnitch] graus (valor que s'actualitza segons la tecla R i el Dial) sobre l'eix Y.
    TG_Moviment = glm::rotate(TG_Moviment, glm::radians(-float(AngleSnitch[indexSnitch])), glm::vec3(0, 1, 0));
    // 3. Posem l'Snitch a l'alcada de la flama de l'espelma i li agefim un radi de 1.
    TG_Moviment = glm::translate(TG_Moviment, glm::vec3(1, 2.6, 0));

    float alcada = 0.75;

    glm::mat4 TG_EscalaPos(1.0f);
    // 2. Escalem a la mida indicada multiplicant per el valor de l'alcada (escalat uniforme).
    TG_EscalaPos = glm::scale(TG_EscalaPos, alcada * glm::vec3(escalaSnitch, escalaSnitch, escalaSnitch));
    // 1. Col.loquem l'Snitch al centre de coordenades.
    TG_EscalaPos = glm::translate(TG_EscalaPos, -centreBaseSnitch);

    Snitch_TG = TG_Moviment * TG_EscalaPos;
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &Snitch_TG[0][0]);

    normalMatrix = glm::inverseTranspose(glm::mat3(View * Snitch_TG));
    glUniformMatrix3fv (normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

    glm::vec3 colorFocusSnitchs = glm::vec3(0.5, 0.5, 0.0);
    glm::vec3 colorApagat = glm::vec3(0, 0, 0);
    if (indexSnitch == 0) {
        // Actualitzem la posicio del focus del Snitch Esquerre enviant-la als shaders.
        glm::vec3 posFocusSnitchEsqAux = View * TG_Moviment * glm::vec4(0, 0, 0, 1);
        glUniform3fv(snitchFocusEsqLoc, 1, &posFocusSnitchEsqAux[0]);

        if (apagarEsq) { /* Si Apretem el boto de on/off modifiquem el color del focus de l'Snitch Esquerre.
                            I enviem el color de focus actual als shaders*/
            glUniform3fv(colorFocusSnitchEsqLoc, 1, &colorApagat[0]); // Focus de color negre = Apagat.
        }
        else {
            glUniform3fv(colorFocusSnitchEsqLoc, 1, &colorFocusSnitchs[0]); // Focus de color groc = Ences.
        }
    }
    else if (indexSnitch == 1) {
        // Actualitzem la posicio del focus del Snitch Dret enviant-la als shaders.
        glm::vec3 posFocusSnitchDretAux = View * TG_Moviment * glm::vec4(0, 0, 0, 1);
        glUniform3fv(snitchFocusDretLoc, 1, &posFocusSnitchDretAux[0]);

        if (apagarDret) { /* Si Apretem el boto de on/off modifiquem el color del focus de l'Snitch Dret.
                             I enviem el color de focus actual als shaders*/
            glUniform3fv(colorFocusSnitchDretLoc, 1, &colorApagat[0]);
        }
        else {
            glUniform3fv(colorFocusSnitchDretLoc, 1, &colorFocusSnitchs[0]);
        }
    }
}

void MyGLWidget::modelTransformEspelma(int indexEspelma)
{
  // Codi per a la TG necessària
  float alcada = 3;

  Espelma_TG = glm::mat4(1.0f);
  Espelma_TG = glm::translate(Espelma_TG, EspelmaPos[indexEspelma]);
  Espelma_TG = glm::scale(Espelma_TG, alcada*glm::vec3(escalaEspelma, escalaEspelma, escalaEspelma));
  Espelma_TG = glm::translate(Espelma_TG, -centreBaseEspelma);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &Espelma_TG[0][0]);

  normalMatrix = glm::inverseTranspose(glm::mat3(View * Espelma_TG));
  glUniformMatrix3fv (normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);
}

void MyGLWidget::projectTransform ()
{
  float fov, zn, zf;
  glm::mat4 Proj;  // Matriu de projecció
  
  fov = float(M_PI/3.0);
  zn = radiEsc*0.25;
  zf = 3*radiEsc;

  float rav =  width() /  (float)height() ;
  if(rav<ra){
      fov= 2*atan(tan(0.5*fov)/rav);
  }

  Proj = glm::perspective(fov, rav, zn, zf);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{
  // Matriu de posició i orientació
  View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -1.3*radiEsc));
  View = glm::rotate(View, angleX, glm::vec3(1, 0, 0));
  View = glm::rotate(View, -angleY, glm::vec3(0, 1, 0));
  View = glm::translate(View, -centreEsc-glm::vec3(0,2,-2));

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);

  normalMatrix = glm::inverseTranspose(glm::mat3(View * Snitch_TG));
  glUniformMatrix3fv (normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  glm::vec3 posFocus = View * glm::vec4(glm::vec3(fixaFocusPos), 1.0);
  glUniform3fv(posFocusLoc, 1, &posFocus[0]);

  glm::vec3 posFocusSnitchEsqAux = View * glm::vec4(posFixaSnitchEsq, 1);
  glUniform3fv(snitchFocusEsqLoc, 1, &posFocusSnitchEsqAux[0]);

  glm::vec3 posFocusSnitchDretAux = View * glm::vec4(posFixaSnitchDret, 1);
  glUniform3fv(snitchFocusDretLoc, 1, &posFocusSnitchDretAux[0]);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)  // Cal modificar aquesta funció...
{
    makeCurrent();
    switch (event->key()) {

        case Qt::Key_S: {
            EspelmaPos[indexEspelmaActiva] -= glm::vec3(0.f,0.2f,0.0f);
            break;
        }
        case Qt::Key_W: {
            EspelmaPos[indexEspelmaActiva] += glm::vec3(0.f,0.2f,0.0f);break;
        }
        case Qt::Key_A: {
            EspelmaPos[indexEspelmaActiva] -= glm::vec3(0.2f,0.0f,0.0f);break;
        }
        case Qt::Key_D: {
            EspelmaPos[indexEspelmaActiva] += glm::vec3(0.2f,0.0f,0.0f);break;
        }

        case Qt::Key_R:{
         AngleSnitch[indexEspelmaActiva]=(AngleSnitch[indexEspelmaActiva]+5)%360;
         emit enviaValActualRot(AngleSnitch[indexEspelmaActiva]); // Enviem l'angle actual al Dial.
         break;
        }

        case Qt::Key_T:{
            indexEspelmaActiva = (indexEspelmaActiva+1)%NUM_ESPELMES;
            // Enviem l'espelma activa als seus RadioButtons corresponents.
            if (indexEspelmaActiva == 0)
                emit enviaEspelmaActivaEsq();
            else if (indexEspelmaActiva == 1) {
                emit enviaEspelmaActivaDreta();
            }
            break;
        }

        case Qt::Key_H: {
            break;
        }

    default: event->ignore(); break;
  }
modelTransformSnitch(indexEspelmaActiva);
  update();
}

void MyGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void MyGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();
  if (DoingInteractive == ROTATE)
  {
    // Fem la rotació
    angleY += (e->x() - xClick) * M_PI / 180.0;    
    angleX += (e->y() - yClick) * M_PI / 180.0;
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::calculaCapsaModel (Model &m, float &escala, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = m.vertices()[0];
  miny = maxy = m.vertices()[1];
  minz = maxz = m.vertices()[2];
  for (unsigned int i = 3; i < m.vertices().size(); i+=3)
  {
    if (m.vertices()[i+0] < minx)
      minx = m.vertices()[i+0];
    if (m.vertices()[i+0] > maxx)
      maxx = m.vertices()[i+0];
    if (m.vertices()[i+1] < miny)
      miny = m.vertices()[i+1];
    if (m.vertices()[i+1] > maxy)
      maxy = m.vertices()[i+1];
    if (m.vertices()[i+2] < minz)
      minz = m.vertices()[i+2];
    if (m.vertices()[i+2] > maxz)
      maxz = m.vertices()[i+2];
  }
  escala = 1.0/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersBackground(){
    creaBuffersModel(modelBackground,"./models/escena_reduida.obj", &VAO_background, escalaBackground, centreBaseBackground);
}
void MyGLWidget::creaBuffersSnitch(){
  creaBuffersModel(modelSnitch,"./models/snitch.obj", &VAO_snitch, escalaSnitch, centreBaseSnitch);
}

void MyGLWidget::creaBuffersEspelma(){
  creaBuffersModel(modelEspelma,"./models/espelma.obj", &VAO_Espelma, escalaEspelma, centreBaseEspelma);
}

void MyGLWidget::creaBuffersFinestra(){
  creaBuffersModel(modelFinestra,"./models/finestres.obj", &VAO_Finestra, escalaFinestra, centreBaseFinestra);
}


void MyGLWidget::creaBuffersModel(Model &model, const char *fileName, GLuint *VAO,  float &escala, glm::vec3 &centreBase )
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  model.load(fileName);

  // Calculem la capsa contenidora del model
  calculaCapsaModel(model, escala, centreBase);
  
  // Creació del Vertex Array Object del model
  glGenVertexArrays(1, VAO);
  glBindVertexArray(*VAO);

  // Creació dels buffers del model
  GLuint VBO_Model[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Model);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3*3, model.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Model[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*model.faces().size()*3, model.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);

}



void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("./shaders/basicLlumShader.frag");
  vs.compileSourceFile("./shaders/basicLlumShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  // Obtenim identificador per a l'atribut “normal” del vertex shader
  normalLoc = glGetAttribLocation (program->programId(), "normal");
  // Obtenim identificador per a l'atribut “matamb” del vertex shader
  matambLoc = glGetAttribLocation (program->programId(), "matamb");
  // Obtenim identificador per a l'atribut “matdiff” del vertex shader
  matdiffLoc = glGetAttribLocation (program->programId(), "matdiff");
  // Obtenim identificador per a l'atribut “matspec” del vertex shader
  matspecLoc = glGetAttribLocation (program->programId(), "matspec");
  // Obtenim identificador per a l'atribut “matshin” del vertex shader
  matshinLoc = glGetAttribLocation (program->programId(), "matshin");

  // Demanem identificadors per als uniforms del vertex shader
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");

  // Bloc d'uniforms
  //----------------------------------------------
  // Manca afegir força variables uniform
  // ... {TODO} ...

  llumAmbientLoc = glGetUniformLocation (program->programId(), "llumAmbient");
  colorFocusLoc = glGetUniformLocation (program->programId(), "colorFocus");
  posFocusLoc = glGetUniformLocation (program->programId(), "posFocus");

  normalMatrixLoc = glGetUniformLocation (program->programId(), "NM");

  colorFocusSnitchEsqLoc = glGetUniformLocation (program->programId(), "colorFocusSnitchEsq");
  colorFocusSnitchDretLoc = glGetUniformLocation (program->programId(), "colorFocusSnitchDret");

  snitchFocusEsqLoc = glGetUniformLocation (program->programId(), "snitchFocusEsq");
  snitchFocusDretLoc = glGetUniformLocation (program->programId(), "snitchFocusDret");

  opacitatLoc = glGetUniformLocation (program->programId(), "opacitat");
}

void MyGLWidget::setEspelmaEsq()
{
    makeCurrent();
    indexEspelmaActiva = 0;
}

void MyGLWidget::setEspelmaDreta()
{
    makeCurrent();
    indexEspelmaActiva = 1;
}

void MyGLWidget::setEncendreApagar(bool b)
{
    makeCurrent();
    if (indexEspelmaActiva == 0) {
        apagarEsq = !b;
    }
    else if (indexEspelmaActiva == 1) {
        apagarDret = !b;
    }
    update();
}

void MyGLWidget::setValorRotacio(int val)
{
    makeCurrent();
    if (val > valActualRot) {
        AngleSnitch[indexEspelmaActiva]=(AngleSnitch[indexEspelmaActiva]+5)%360;
        if (AngleSnitch[indexEspelmaActiva] == 360)
            AngleSnitch[indexEspelmaActiva] = 0;
    }

    else if (val < valActualRot){
        AngleSnitch[indexEspelmaActiva]=(AngleSnitch[indexEspelmaActiva]-5)%360;
        if (AngleSnitch[indexEspelmaActiva] == 360)
            AngleSnitch[indexEspelmaActiva] = 0;
    }

    valActualRot = val;
    update();
}
