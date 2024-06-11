#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
  angleX = angleY = 0.5;
  alpha_v = 0;
  fov = 0;
  focusDeCamera = true;
  rotacio = 0;
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  carregaShaders();
  iniEscena ();
  iniCamera ();
}

void MyGLWidget::iniMaterialTronc ()
{
  // Donem valors al material del cub
  amb = glm::vec3(0.3,0.2,0.1);
  diff = glm::vec3(0.6,0.2,0.1);
  spec = glm::vec3(0.3,0.3,0.3);
  shin = 0;
}

void MyGLWidget::iniEscena ()
{
  creaBuffersPatricio();
  creaBuffersTerra();
  creaBuffersCub();
  creaBuffersEsfera();

  // Paràmetres de l'escena - arbitraris
  creaCapsaContenidora(glm::vec3(0, 0, 0), glm::vec3(30, 4, 20));

  std::cout << "Centre Escena: " << centreEsc.x << ", " << centreEsc.y << ", " << centreEsc.z << std::endl;
  std::cout << "Radi Escena: " << radiEsc << std::endl;

  enviaPosFocus();
}

void MyGLWidget::enviaPosFocus()
{
  if (focusDeCamera) { // Focus de Camera. No cal fer res. Inicialment el focus ja es troba a la camera.
    posFoc = glm::vec3(0,0,0);
  }
  else { // Focus Escena a (15, 10, 10). Cal Multiplicar per View.
    posFoc = View * glm::vec4(15, 10, 10, 1);
  }
  glUniform3fv (posfocusLoc, 1, &posFoc[0]);
}

void MyGLWidget::iniCamera ()
{
    /*  Codi Inicial
        angleY = 0.5;
        ra = float(width())/height();
        fov = float(M_PI/3.0);
        zn = 15;
        zf = 65;
    */

  float d = 2 * radiEsc;

  obs = vrp + (d * glm::vec3(0, 0, 1));
  vrp = centreEsc;

  up = glm::vec3(0, 1, 0);
  zn = d - radiEsc;
  zf = d + radiEsc;

  alpha_v = asin(radiEsc/d);
  fov = (float) 2 * alpha_v;

  raw = 1.0f;

//  if (ra < 1) {
//    raw = ra;
//    fov = 2 * atan(tan(alpha_v/ra));
//  }
//  else if (ra > 1) {
//      raw = ra;
//  }

  projectTransform ();
  viewTransform ();
}

void MyGLWidget::pintaArbre (glm::vec3 pos)
{
  // pintem l'arbre format pel tronc i la copa
  glBindVertexArray(VAO_Cub);
  modelTransformTronc (pos);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindVertexArray(VAO_Esf);
  modelTransformCopa (pos);
  glDrawArrays(GL_TRIANGLES, 0, esfera.faces().size()*3);
}

void MyGLWidget::paintGL ()
{
// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Pintem el terra
  glBindVertexArray (VAO_Terra);
  modelTransformTerra ();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Pintem el Patricio
  glBindVertexArray (VAO_Patr);
  modelTransformPatricio ();
  glDrawArrays(GL_TRIANGLES, 0, patr.faces().size()*3);

  // Pintem l'arbre
  pintaArbre (glm::vec3(10,0,10)); // Arbre 1
  pintaArbre (glm::vec3(20,0,5)); // Arbre 2
  pintaArbre (glm::vec3(25,0,15)); // Arbre 3

  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
/*#ifdef __APPLE__
  // Aquest codi és necessari únicament per a MACs amb pantalla retina.
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  w = vp[2];
  h = vp[3];
#endif*/
  ample = w;
  alt = h;
  ra = float(ample)/float(alt);
  projectTransform();
}

void MyGLWidget::modelTransformPatricio ()
{
  float alcada = 2;
  TG = glm::mat4(1.f);

  TG = glm::translate(TG, glm::vec3(10, 0, 10));
  TG = glm::rotate(TG, glm::radians(-rotacio), glm::vec3(0, 1, 0));
  TG = glm::translate(TG, glm::vec3(-10, 0, -10));

  TG = glm::translate(TG, glm::vec3(15, 0, 10));
  TG = glm::scale(TG, alcada * glm::vec3 (escala, escala, escala));
  TG = glm::translate(TG, -centreBasePat);

  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTronc (glm::vec3 pos)
{
  TG = glm::mat4(1.f);

  TG = glm::translate(TG, pos);
  TG = glm::scale(TG, glm::vec3 (1, 5, 1));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformCopa (glm::vec3 pos)
{
  TG = glm::mat4(1.f);

  TG = glm::translate(TG, pos + glm::vec3(0, 2, 0));
  TG = glm::scale(TG, glm::vec3 (2*escalaEsf, 2*escalaEsf, 2*escalaEsf));
  TG = glm::translate(TG, -centreBaseEsf);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTerra ()
{
  TG = glm::mat4(1.f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj;  // Matriu de projecció
  Proj = glm::perspective(fov, ra, zn, zf);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{

  View = glm::mat4(1.0f);

  View = glm::translate(View, glm::vec3(0.0, 0.0, -2 * radiEsc));
  View = glm::rotate(View, angleY, glm::vec3(1.0, 0.0, 0.0));
  View = glm::rotate(View, -angleX, glm::vec3(0.0, 1.0, 0.0));
  View = glm::translate(View, glm::vec3(-vrp));

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
  makeCurrent();
  switch (event->key()) {
    default: event->ignore(); break;

    case Qt::Key_F: {
        enviaPosFocus();
        focusDeCamera = !focusDeCamera;
    }
      case Qt::Key_Up: {
          rotacio += 360/32;
      }
  }
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
    angleX -= (e->x() - xClick) * (M_PI / 360);
    angleY += (e->y() - yClick) * (M_PI / 360);
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::creaCapsaContenidora(glm::vec3 pMin, glm::vec3 pMax)
{
    centreEsc = (pMax + pMin) / glm::vec3(2, 2, 2);
    radiEsc = sqrt(pow(pMax.x - pMin.x, 2) + pow(pMax.y - pMin.y, 2) + pow(pMax.z - pMin.z, 2)) / 2.f;
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, glm::vec3 &centreBase)
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = p.vertices()[0];
  miny = maxy = p.vertices()[1];
  minz = maxz = p.vertices()[2];
  for (unsigned int i = 3; i < p.vertices().size(); i+=3)
  {
    if (p.vertices()[i+0] < minx)
      minx = p.vertices()[i+0];
    if (p.vertices()[i+0] > maxx)
      maxx = p.vertices()[i+0];
    if (p.vertices()[i+1] < miny)
      miny = p.vertices()[i+1];
    if (p.vertices()[i+1] > maxy)
      maxy = p.vertices()[i+1];
    if (p.vertices()[i+2] < minz)
      minz = p.vertices()[i+2];
    if (p.vertices()[i+2] > maxz)
      maxz = p.vertices()[i+2];
  }
  escala = 1.0/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
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

  // Demanem identificadors per als uniforms dels shaders
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc = glGetUniformLocation (program->programId(), "proj");
  viewLoc = glGetUniformLocation (program->programId(), "view");
  posfocusLoc = glGetUniformLocation (program->programId(), "posFocus");
}

void MyGLWidget::creaBuffersPatricio ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  patr.load("./models/Patricio.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel (patr, escala, centreBasePat);

  // Creació del Vertex Array Object del Patricio
  glGenVertexArrays(1, &VAO_Patr);
  glBindVertexArray(VAO_Patr);

  // Creació dels buffers del model patr
  GLuint VBO_Patr[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Patr);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3*3, patr.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Patr[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*patr.faces().size()*3, patr.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersTerra ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posterra[] = {
    glm::vec3(0.0, 0.0, 20.0),
    glm::vec3(30.0, 0.0, 20.0),
    glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(30.0, 0.0, 0.0),
  };

  // VBO amb la normal de cada vèrtex
  glm::vec3 norm (0,1,0);
  glm::vec3 normterra[6] = {
    norm, norm, norm, norm
  };

  // Definim el material del terra
  glm::vec3 amb(0.1,0.15,0.1);
  glm::vec3 diff(0.3,0.5,0.3);
  glm::vec3 spec(1,1,1);
  float shin = 500;

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 matamb[] = {
    amb, amb, amb, amb
  };
  glm::vec3 matdiff[6] = {
    diff, diff, diff, diff
  };
  glm::vec3 matspec[6] = {
    spec, spec, spec, spec
  };
  float matshin[6] = {
    shin, shin, shin, shin
  };

// Creació del Vertex Array Object del terra
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[6];
  glGenBuffers(6, VBO_Terra);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posterra), posterra, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normterra), normterra, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matamb), matamb, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiff), matdiff, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspec), matspec, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshin), matshin, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersCub ()
{
  // Dades del cub
  // Vèrtexs del cub
  glm::vec3 vertexs[8] = {
       /* 0*/ glm::vec3( -0.25, 0.0, -0.25),  /* 1*/ glm::vec3( 0.25, 0.0, -0.25),
       /* 2*/ glm::vec3( -0.25, 0.5, -0.25),  /* 3*/ glm::vec3( 0.25, 0.5, -0.25),
       /* 4*/ glm::vec3( -0.25, 0.0, 0.25),  /* 5*/ glm::vec3( 0.25, 0.0, 0.25),
       /* 6*/ glm::vec3( -0.25, 0.5, 0.25),  /* 7*/ glm::vec3( 0.25, 0.5, 0.25)
  };

  // VBO amb la posició dels vèrtexs
  glm::vec3 poscub[36] = {  // 12 triangles
       vertexs[0], vertexs[2], vertexs[1],
       vertexs[1], vertexs[2], vertexs[3],
       vertexs[5], vertexs[1], vertexs[7],
       vertexs[1], vertexs[3], vertexs[7],
       vertexs[2], vertexs[6], vertexs[3],
       vertexs[3], vertexs[6], vertexs[7],
       vertexs[0], vertexs[4], vertexs[6],
       vertexs[0], vertexs[6], vertexs[2],
       vertexs[0], vertexs[1], vertexs[4],
       vertexs[1], vertexs[5], vertexs[4],
       vertexs[4], vertexs[5], vertexs[6],
       vertexs[5], vertexs[7], vertexs[6]
  };

  // VBO amb la normal de cada vèrtex
  glm::vec3 normals[6] = {
       /* 0*/ glm::vec3( 1.0, 0.0,  0.0),  /* 1*/ glm::vec3( -1.0, 0.0, 0.0),
       /* 2*/ glm::vec3( 0.0, 1.0,  0.0),  /* 3*/ glm::vec3( 0.0, -1.0, 0.0),
       /* 4*/ glm::vec3( 0.0, 0.0,  1.0),  /* 5*/ glm::vec3( 0.0, 0.0, -1.0)
  };
  glm::vec3 normcub[36] = {
       normals[5], normals[5], normals[5],
       normals[5], normals[5], normals[5],
       normals[0], normals[0], normals[0],
       normals[0], normals[0], normals[0],
       normals[2], normals[2], normals[2],
       normals[2], normals[2], normals[2],
       normals[1], normals[1], normals[1],
       normals[1], normals[1], normals[1],
       normals[3], normals[3], normals[3],
       normals[3], normals[3], normals[3],
       normals[4], normals[4], normals[4],
       normals[4], normals[4], normals[4]
  };

  // inicialitzem el material del cub
  iniMaterialTronc();

  // Fem que aquest material afecti a tots els vèrtexs per igual
  glm::vec3 matambcub[36] = {
    amb, amb, amb, amb, amb, amb,
    amb, amb, amb, amb, amb, amb,
    amb, amb, amb, amb, amb, amb,
    amb, amb, amb, amb, amb, amb,
    amb, amb, amb, amb, amb, amb,
    amb, amb, amb, amb, amb, amb
  };
  glm::vec3 matdiffcub[36] = {
    diff, diff, diff, diff, diff, diff,
    diff, diff, diff, diff, diff, diff,
    diff, diff, diff, diff, diff, diff,
    diff, diff, diff, diff, diff, diff,
    diff, diff, diff, diff, diff, diff,
    diff, diff, diff, diff, diff, diff
  };
  glm::vec3 matspeccub[36] = {
    spec, spec, spec, spec, spec, spec,
    spec, spec, spec, spec, spec, spec,
    spec, spec, spec, spec, spec, spec,
    spec, spec, spec, spec, spec, spec,
    spec, spec, spec, spec, spec, spec,
    spec, spec, spec, spec, spec, spec
  };
  float matshincub[36] = {
    shin, shin, shin, shin, shin, shin,
    shin, shin, shin, shin, shin, shin,
    shin, shin, shin, shin, shin, shin,
    shin, shin, shin, shin, shin, shin,
    shin, shin, shin, shin, shin, shin,
    shin, shin, shin, shin, shin, shin
  };

// Creació del Vertex Array Object del cub
  glGenVertexArrays(1, &VAO_Cub);
  glBindVertexArray(VAO_Cub);

  GLuint VBO_Cub[6];
  glGenBuffers(6, VBO_Cub);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(poscub), poscub, GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(normcub), normcub, GL_STATIC_DRAW);

  // Activem l'atribut normalLoc
  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matambcub), matambcub, GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matdiffcub), matdiffcub, GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matspeccub), matspeccub, GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(matshincub), matshincub, GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersEsfera ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  esfera.load("./models/Esfera.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModel (esfera, escalaEsf, centreBaseEsf);

  // Creació del Vertex Array Object
  glGenVertexArrays(1, &VAO_Esf);
  glBindVertexArray(VAO_Esf);

  // Creació dels buffers del model
  GLuint VBO_Esf[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Esf);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Esf[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*esfera.faces().size()*3*3, esfera.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Esf[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*esfera.faces().size()*3*3, esfera.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Esf[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*esfera.faces().size()*3*3, esfera.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Esf[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*esfera.faces().size()*3*3, esfera.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Esf[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*esfera.faces().size()*3*3, esfera.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Esf[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*esfera.faces().size()*3, esfera.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}


