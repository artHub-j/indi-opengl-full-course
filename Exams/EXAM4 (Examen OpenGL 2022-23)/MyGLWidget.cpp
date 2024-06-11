#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  connect(&timer, SIGNAL(timeout()), this, SLOT(girarRoda())); // per al moviment de la roda
  DoingInteractive = NONE;
  camera_nova = false;
  posFantasmaZ = 0;
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
  glEnable(GL_CULL_FACE);

  carregaShaders();
  iniEscena ();
  iniCamera ();
}

void MyGLWidget::iniMaterialCub ()
{
  // Donem valors al material del cub
  amb = glm::vec3(0.0,0.0,0.8);
  diff = glm::vec3(0.0,0.0,0.8);
  spec = glm::vec3(1.0,1.0,1);
  shin = 100;
}

void MyGLWidget::creaCapsaContenidoraEscena(glm::vec3 pMin, glm::vec3 pMax)
{
    centreEsc = (pMax + pMin) / glm::vec3(2, 2, 2);
    radiEsc = sqrt(pow(pMax.x - pMin.x, 2) + pow(pMax.y - pMin.y, 2) + pow(pMax.z - pMin.z, 2)) / 2.f;
}

void MyGLWidget::enviaPosFocus()
{
  posFocus = glm::vec4(0, 0, 0, 0); //Focus de Camera.
  glUniform4fv(posFocusLoc, 1, &posFocus[0]);
}

void MyGLWidget::iniEscena ()
{
  creaBuffersFantasma();
  creaBuffersCub();

  // Creem la capsa contenidora de l'escena:
    // Circumferencia amb centre a (0, 0, 0).
    // radi interior cercle + altura maxima cub (x i y) = 20 + 3
    // amplada escena (z) = 6
    // Punt Minim: (-23, -23, -3), Punt Maxim: (23, 23, 3)

  creaCapsaContenidoraEscena(glm::vec3(-23, -23, -3), glm::vec3(23, 23, 3));
  enviaPosFocus();

  // Paràmetres moviment roda
  angleRoda = 0;
  velocitatRoda = 0.25;
  
  girant = true;
  timer.start(16);
}

void MyGLWidget::iniCamera ()
{
//  angleY = 0.0;
//  ra = float(width())/height();
//  fov = float(M_PI/3.0);
//  zn = 15;
//  zf = 65;

    angleY = 1;
    angleX = 0;
    //radiEsc = 23;
    //centreEsc = glm::vec3(0, 0, 0);
    float d = 2 * radiEsc;

    obs = vrp + (d * glm::vec3(0, 0, 1));
    vrp = centreEsc;

    up = glm::vec3(0, 1, 0);
    zn = d - radiEsc;
    zf = d + radiEsc;

    float alpha_v = asin(radiEsc/d);
    fov = (float) 2 * alpha_v;

    ra = 1.0;

  projectTransform ();
  viewTransform ();
}

void MyGLWidget::paintGL ()
{
// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  // Esborrem el frame-buffer i el depth-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Roda - La roda té 72 cubs, un cub cada 5 graus
  glBindVertexArray (VAO_Cub);
  for (int i = 0; i < 360; i += 5)
  {
    modelTransformCub (alcades[0][i/5],float(i), 20.0f, -2.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  glBindVertexArray (VAO_Cub);
  for (int i = 0; i < 360; i += 5)
  {
    modelTransformCub (alcades[1][i/5],float(i),20.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  glBindVertexArray (VAO_Cub);
  for (int i = 0; i < 360; i += 5)
  {
    modelTransformCub (alcades[2][i/5],float(i),20.0f, 2.0f);
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }
  
  // Fantasma
  glBindVertexArray (VAO_Fantasma);
  modelTransformFantasma ();
  glDrawArrays(GL_TRIANGLES, 0, fantasma.faces().size()*3);

  glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h) 
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

void MyGLWidget::modelTransformFantasma ()
{
  glm::mat4 TG(1.f);

  TG = glm::translate(TG, glm::vec3(0, 21, posFantasmaZ));

  TG = glm::rotate(TG, glm::radians(90.0f), glm::vec3(0, 1, 0));
  TG = glm::scale(TG, glm::vec3 (escalaFantasma, escalaFantasma, escalaFantasma)); // Fa 10 alcada
  TG = glm::translate(TG, -centreBaseFantasma);
  
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformCub (float alcada, float angle, float radi, float z)
{
  glm::mat4 TG = glm::mat4(1.f);  // Matriu de transformació
  TG = glm::rotate(TG, float(glm::radians(angle + angleRoda)),glm::vec3(0.f, 0.f, 1.f));
  TG = glm::translate(TG, glm::vec3(0,radi,z));
  TG = glm::scale(TG, glm::vec3(2, glm::max(0.1f,alcada)*0.5, 1.8));
  TG = glm::translate(TG, -glm::vec3(0.5,0,0.5));
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj;  // Matriu de projecció

  if (camera_nova) {
      //Proj = glm::perspective((float)60.0f, ra, 0.25, zf);
      float zn_n = 0.25;
      Proj = glm::perspective(glm::radians(60.0f), ra, zn_n, zf);
  }
  else {
      Proj = glm::perspective(fov, ra, zn, zf);
  }

  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{
//  View = glm::translate(glm::mat4(1.f), glm::vec3(0, 0, -2*radiEsc));
//  View = glm::rotate(View, angleY, glm::vec3(0, 1, 0));
//  View = glm::translate(View, -centreEsc);

//  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);


    View = glm::mat4(1.0f);

    if (camera_nova) {
        glm::vec3 obs_n = glm::vec3(-5, 25, 0);
        glm::vec3 vrp_n = glm::vec3(0, 23, 0);
        up = glm::vec3(0, 1, 0);
        View = glm::lookAt(obs_n, vrp_n, up);
    }
    else {
        View = glm::translate(View, glm::vec3(0.0, 0.0, -2 * radiEsc));
        View = glm::rotate(View, angleY, glm::vec3(1.0, 0.0, 0.0));
        View = glm::rotate(View, -angleX, glm::vec3(0.0, 1.0, 0.0));
        View = glm::translate(View, glm::vec3(-vrp));
    }
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::girarRoda()
{
  if (girant)
  {
    makeCurrent();
    angleRoda += velocitatRoda;
    update();
  }  
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Left: {
      // ...
      if (posFantasmaZ > -2)
        posFantasmaZ -= 2;
      break;
    }
    case Qt::Key_Right: {
      // ...
      if (posFantasmaZ < 2)
        posFantasmaZ += 2;
      break;
    }
    case Qt::Key_C: {
      // ...
      camera_nova = !camera_nova;
      projectTransform();
      viewTransform();
      emit enviaEstatCamera(camera_nova);
      break;
    }
    case Qt::Key_R: {
      // ...
      camera_nova = false;
      posFantasmaZ = 0;
      angleY = 1;
      angleX = 0;
      makeCurrent();
      girant = true;
      update();
      projectTransform();
      viewTransform();

      break;
    }
    default: {
      event->ignore(); 
      break;
    }
  }

  if (not girant)
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
    //angleY += (e->x() - xClick) * M_PI / ample;
      angleX -= (e->x() - xClick) * (M_PI / 360);
      angleY += (e->y() - yClick) * (M_PI / 360);
    viewTransform ();
  }

  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::calculaCapsaModelFantasma ()
{
  // Càlcul capsa contenidora i valors transformacions inicials
  float minx, miny, minz, maxx, maxy, maxz;
  minx = maxx = fantasma.vertices()[0];
  miny = maxy = fantasma.vertices()[1];
  minz = maxz = fantasma.vertices()[2];
  for (unsigned int i = 3; i < fantasma.vertices().size(); i+=3)
  {
    if (fantasma.vertices()[i+0] < minx)
      minx = fantasma.vertices()[i+0];
    if (fantasma.vertices()[i+0] > maxx)
      maxx = fantasma.vertices()[i+0];
    if (fantasma.vertices()[i+1] < miny)
      miny = fantasma.vertices()[i+1];
    if (fantasma.vertices()[i+1] > maxy)
      maxy = fantasma.vertices()[i+1];
    if (fantasma.vertices()[i+2] < minz)
      minz = fantasma.vertices()[i+2];
    if (fantasma.vertices()[i+2] > maxz)
      maxz = fantasma.vertices()[i+2];
  }
  escalaFantasma = 2.0/(maxx-minx);
  
  centreBaseFantasma[0] = (minx+maxx)/2.0; 
  centreBaseFantasma[1] = miny; 
  centreBaseFantasma[2] = (minz+maxz)/2.0;
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

  posFocusLoc = glGetUniformLocation (program->programId(), "posFocus");
}

void MyGLWidget::creaBuffersFantasma ()
{
  // Carreguem el model de l'OBJ - Atenció! Abans de crear els buffers!
  fantasma.load("./models/Fantasma.obj");

  // Calculem la capsa contenidora del model
  calculaCapsaModelFantasma ();
  
  // Creació del Vertex Array Object del Fantasma
  glGenVertexArrays(1, &VAO_Fantasma);
  glBindVertexArray(VAO_Fantasma);

  // Creació dels buffers del model fantasma
  GLuint VBO_Fantasma[6];
  // Buffer de posicions
  glGenBuffers(6, VBO_Fantasma);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Fantasma[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fantasma.faces().size()*3*3, fantasma.VBO_vertices(), GL_STATIC_DRAW);

  // Activem l'atribut vertexLoc
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // Buffer de normals
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Fantasma[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fantasma.faces().size()*3*3, fantasma.VBO_normals(), GL_STATIC_DRAW);

  glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(normalLoc);

  // En lloc del color, ara passem tots els paràmetres dels materials
  // Buffer de component ambient
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Fantasma[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fantasma.faces().size()*3*3, fantasma.VBO_matamb(), GL_STATIC_DRAW);

  glVertexAttribPointer(matambLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matambLoc);

  // Buffer de component difusa
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Fantasma[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fantasma.faces().size()*3*3, fantasma.VBO_matdiff(), GL_STATIC_DRAW);

  glVertexAttribPointer(matdiffLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matdiffLoc);

  // Buffer de component especular
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Fantasma[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fantasma.faces().size()*3*3, fantasma.VBO_matspec(), GL_STATIC_DRAW);

  glVertexAttribPointer(matspecLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matspecLoc);

  // Buffer de component shininness
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Fantasma[5]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*fantasma.faces().size()*3, fantasma.VBO_matshin(), GL_STATIC_DRAW);

  glVertexAttribPointer(matshinLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(matshinLoc);

  glBindVertexArray(0);
}

void MyGLWidget::creaBuffersCub ()
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

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

void MyGLWidget::atura() {
  girant = false;
  timer.stop();
}

void MyGLWidget::engega() {
  girant = true;
  timer.start(16);
}

void MyGLWidget::setCamera()
{
    makeCurrent();
    camera_nova = !camera_nova;
    projectTransform();
    viewTransform();
    update();
}
