#include "ExamGLWidget.h"

#include <iostream>

ExamGLWidget::ExamGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
}

ExamGLWidget::~ExamGLWidget ()
{
  if (program != NULL)
    delete program;
}

void ExamGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();  

  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  carregaShaders();
  creaBuffersModels();
  creaBuffersCub();

  iniEscena ();
  iniCamera ();
}

void ExamGLWidget::iniEscena ()
{
  // Paràmetres de l'escena - arbitraris
  centreEsc = glm::vec3 (0, 0, 0);
  radiEsc = 3;
}

void ExamGLWidget::iniCamera ()
{
  angleY = 0.0;
  ra = float(width())/height();
  fov = float(M_PI/2.0);
  zn = 0.1;
  zf = 70;

  projectTransform ();
  viewTransform ();
}

void ExamGLWidget::pintaEscenari () 
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glBindVertexArray (VAO_models[MORTY]);
  modelTransformMorty();
  glDrawArrays(GL_TRIANGLES, 0, models[MORTY].faces().size()*3);

}

void ExamGLWidget::resizeGL (int w, int h) 
{
#ifdef __APPLE__
  // Aquest codi és necessari únicament per a MACs amb pantalla retina.
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif

  ra = float(ample)/float(alt);
  projectTransform();
}


void ExamGLWidget::modelTransformMorty()
{
  glm::mat4 TG(1.f);
  TG=glm::scale(TG, glm::vec3(3*escalaModels[MORTY]));
  TG = glm::translate(TG, -centreBaseModels[MORTY]);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void ExamGLWidget::modelTransformCoin()
{
  glm::mat4 TG(1.f);
  TG=glm::translate(TG, glm::vec3(0,0,6));
  TG = glm::scale(TG, glm::vec3(3*escalaModels[COIN]));
  TG = glm::translate(TG, -centreBaseModels[COIN]);  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


void ExamGLWidget::modelTransformFantasma()
{
  glm::mat4 TG(1.f);
  TG=glm::translate(TG, glm::vec3(10,0,5));
  TG = glm::scale(TG, glm::vec3(3*escalaModels[FANTASMA]));
  TG = glm::translate(TG, -centreBaseModels[FANTASMA]);  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void ExamGLWidget::modelTransformGroundBlock()
{
  /*
  glm::mat4 TG(1.f);
  TG = glm::scale(TG, glm::vec3(3,3,3));
  TG=glm::translate(TG, glm::vec3(0,0,4));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
*/
}

void ExamGLWidget::projectTransform ()
{
  glm::mat4 Proj;  // Matriu de projecció
  Proj = glm::perspective(fov, ra, zn, zf);

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void ExamGLWidget::viewTransform ()
{
  /*View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -2*radiEsc));
  View = glm::rotate(View, angleY, glm::vec3(0, 1, 0));
  View = glm::translate(View, -centreEsc);*/

  View=glm::lookAt(glm::vec3(25,15,25), glm::vec3(7,0,7), glm::vec3(0,1,0));

  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}


void ExamGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    default: event->ignore(); break;
  }
  update();
}

void ExamGLWidget::mousePressEvent (QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      ! (e->modifiers() & (Qt::ShiftModifier|Qt::AltModifier|Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
}

void ExamGLWidget::mouseReleaseEvent( QMouseEvent *)
{
  DoingInteractive = NONE;
}


void ExamGLWidget::calculaCapsaModel (Model &p, float &escala, float alcadaInicial, glm::vec3 &centreBase)
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
  escala = alcadaInicial/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void ExamGLWidget::iniMaterialCub ()
{
  // Donem valors al material del cub
  amb = glm::vec3(0.1,0.0,0.0);
  diff = glm::vec3(0.5,0.0,0.0);
  spec = glm::vec3(0.6,0.6,0.6);
  shin = 200;
}

void ExamGLWidget::creaBuffersCub ()
{
  // Dades del cub
  // Vèrtexs del cub
  glm::vec3 vertexs[8] = {
       /* 0*/ glm::vec3( 0.0, 0.0, 0.0),  /* 1*/ glm::vec3( 1.0, 0.0, 0.0),
       /* 2*/ glm::vec3( 0.0, 1.0, 0.0),  /* 3*/ glm::vec3( 1.0, 1.0, 0.0),
       /* 4*/ glm::vec3( 0.0, 0.0, 1.0),  /* 5*/ glm::vec3( 1.0, 0.0, 1.0),
       /* 6*/ glm::vec3( 0.0, 1.0, 1.0),  /* 7*/ glm::vec3( 1.0, 1.0, 1.0)
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
  iniMaterialCub();

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

void ExamGLWidget::creaBuffersModels ()
{
  // Creació de VAOs i VBOs per pintar els models
  glGenVertexArrays(NUM_MODELS, &VAO_models[0]);
  
  
  for (int i = 0; i < NUM_MODELS; i++)
  {	
      	  // Càrrega del models
  	  models[i].load("./models/"+objNames[i]);
  
	  // Calculem la capsa contenidora del model
	  calculaCapsaModel (models[i], escalaModels[i], alcadesInicials[i], centreBaseModels[i]);
  
	  glBindVertexArray(VAO_models[i]);

	  // Creació dels buffers del model fantasma
	  GLuint VBO[6];
	  // Buffer de posicions
	  glGenBuffers(6, VBO);
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_vertices(), GL_STATIC_DRAW);

	  // Activem l'atribut vertexLoc
	  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(vertexLoc);

	  // Buffer de normals
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_normals(), GL_STATIC_DRAW);

	  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(normalLoc);

	  // En lloc del color, ara passem tots els paràmetres dels materials
	  // Buffer de component ambient
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_matamb(), GL_STATIC_DRAW);

	  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matambLoc);

	  // Buffer de component difusa
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_matdiff(), GL_STATIC_DRAW);

	  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matdiffLoc);

	  // Buffer de component especular
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3, models[i].VBO_matspec(), GL_STATIC_DRAW);

	  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matspecLoc);

	  // Buffer de component shininness
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3, models[i].VBO_matshin(), GL_STATIC_DRAW);

	  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(matshinLoc);
  }
  
  glBindVertexArray (0);
}

void ExamGLWidget::carregaShaders() 
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
}


