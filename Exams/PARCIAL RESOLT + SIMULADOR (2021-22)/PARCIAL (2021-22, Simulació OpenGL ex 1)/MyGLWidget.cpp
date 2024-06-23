#include "MyGLWidget.h"

#include <iostream>

/** 
* @author: Arturo Aragón Hidalgo
*/
MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  scale = 1.0f;
  gir = 0.f;
  alcada = 0.f;
  ortho = false;
  angleTheta = anglePsi = 0;
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

  // Activem el Z-Buffer
  glEnable (GL_DEPTH_TEST);

  glClearColor(0.0, 0.0, 0.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersCub();
  creaBuffersTerra();
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

// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT);
  //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ini_camera();

  drawAxis();

  modelTransformTerra();
  pintaTerra();

  modelTransformCubA();
  pintaCub();

  modelTransformCubB();
  pintaCub();

  glBindVertexArray (0);
}

void MyGLWidget::ini_camera()
{
  centreEscena = glm::vec3(5, 2, 4);
  radiEscena = 7.0f;

  float d = 2 * radiEscena;

  OBS = VRP + (d * glm::vec3(1, 0, 0));
  VRP = centreEscena;
  UP = glm::vec3(0, 1, 0);

  znear = d - radiEscena;
  zfar = d + radiEscena;

  alpha_v = asin(radiEscena/d);
  FOV = (float) 2 * alpha_v;
  raw = 1.0f;

  if (rav < 1) {
    raw = rav;
    FOV = 2 * atan(tan(alpha_v/rav));
  }
  else if (rav > 1) {
      raw = rav;
  }

  left = bottom = -radiEscena;
  right = top = radiEscena;

  projectTransform();
  viewTransform();
}

void MyGLWidget::modelTransformCubA()
{
    float diagonal = 5;
    float c = diagonal/sqrt(2);

    glm::mat4 TG(1.0f);

    TG = glm::translate(TG, glm::vec3(3, 0, 2));
    TG = glm::rotate(TG, glm::radians(-20.f) , glm::vec3(1, 0, 0));
    TG = glm::scale(TG, glm::vec3(c, 1, 5));
    TG = glm::translate(TG, glm::vec3(0.5, 0.5, 0.5));

    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformCubB()
{
    float diagonal = 5;
    float c = diagonal/sqrt(2);

    glm::mat4 TG(1.0f);

    TG = glm::translate(TG, glm::vec3(3, 0, 2));
    TG = glm::rotate(TG, glm::radians(-20.f) , glm::vec3(1, 0, 0));
    TG = glm::translate(TG, glm::vec3(0.0, 1, 2.5));

    TG = glm::rotate(TG, glm::radians(-45.f) , glm::vec3(1, 0, 0));
    TG = glm::scale(TG, glm::vec3(c, c, c));
    TG = glm::translate(TG, glm::vec3(0.5, 0.5, 0.5));

    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTerra()
{
  // Matriu de transformació de model
  glm::mat4 transform (1.0f);
  transform = glm::scale(transform, glm::vec3(5.f, 1.f, 5.f));
  transform = glm::translate(transform, glm::vec3(1.f, 1.f, 1.f));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::drawAxis()
{
    // Pinta Y axis:
    glm::mat4 TG(1.0f);
    TG = glm::scale(TG, glm::vec3(0.005, 50, 0.005));
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);

    glBindVertexArray (VAO_Cub);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    // Pinta X axis:
    glm::mat4 TG2(1.0f);
    TG2 = glm::rotate(TG2, glm::radians(90.f), glm::vec3(0, 0, 1));
    TG2 = glm::scale(TG2, glm::vec3(0.005, 50, 0.005));
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG2[0][0]);

    glBindVertexArray (VAO_Cub);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

    // Pinta Z axis:
    glm::mat4 TG3(1.0f);
    TG3 = glm::rotate(TG3, glm::radians(90.f), glm::vec3(1, 0, 0));
    TG3 = glm::scale(TG3, glm::vec3(0.005, 50, 0.005));
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG3[0][0]);

    glBindVertexArray (VAO_Cub);
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void MyGLWidget::projectTransform()
{
    glm::mat4 Proj;
    if (ortho) {
        Proj = glm::ortho (left, right, bottom, top, znear, zfar);
    }
    else {
        // glm::perspective (FOV en radians, ra window, znear, zfar)
        Proj = glm::perspective(FOV, raw, znear, zfar);
    }

    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform()
{
    // glm::lookAt (`, VRP, UP)
    glm::mat4 View(1.0f);

    View = glm::translate(View, glm::vec3(0.0, 0.0, -2 * radiEscena));
    View = glm::rotate(View, angleTheta, glm::vec3(1.0, 0.0, 0.0));
    View = glm::rotate(View, -anglePsi, glm::vec3(0.0, 1.0, 0.0));
    View = glm::translate(View, glm::vec3(-VRP.x, -VRP.y, -VRP.z));

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::resizeGL (int w, int h) 
{
  ample = w;
  alt = h;
  rav = float(w) / float(h);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_S: { // escalar a més gran
      scale += 0.05;
      break;
    }
    case Qt::Key_D: { // escalar a més petit
      scale -= 0.05;
      break;
    }
    case Qt::Key_R: { // escalar a més petit
      gir += (float) M_PI/4;
      break;
    }
    case Qt::Key_O: { // escalar a més petit
      ortho = !ortho;
      break;
    }
    default: event->ignore(); break;
  }
  update();
}

void MyGLWidget::creaBuffersCub()
{
    glm::vec3 posicio[36] = {
        glm::vec3(-0.5f,-0.5f,-0.5f), // triangle 1 : begin
        glm::vec3(-0.5f,-0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f), // triangle 1 : end
        glm::vec3(0.5f, 0.5f,-0.5f), // triangle 2 : begin
        glm::vec3(-0.5f,-0.5f,-0.5f),
        glm::vec3(-0.5f, 0.5f,-0.5f), // triangle 2 : end
        glm::vec3(0.5f,-0.5f, 0.5f),
        glm::vec3(-0.5f,-0.5f,-0.5f),
        glm::vec3(0.5f,-0.5f,-0.5f),
        glm::vec3(0.5f, 0.5f,-0.5f),
        glm::vec3(0.5f,-0.5f,-0.5f),
        glm::vec3(-0.5f,-0.5f,-0.5f),
        glm::vec3(-0.5f,-0.5f,-0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f,-0.5f),
        glm::vec3(0.5f,-0.5f, 0.5f),
        glm::vec3(-0.5f,-0.5f, 0.5f),
        glm::vec3(-0.5f,-0.5f,-0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f,-0.5f, 0.5f),
        glm::vec3(0.5f,-0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f,-0.5f,-0.5f),
        glm::vec3(0.5f, 0.5f,-0.5f),
        glm::vec3(0.5f,-0.5f,-0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f,-0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f,-0.5f),
        glm::vec3(-0.5f, 0.5f,-0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f,-0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec3(0.5f,-0.5f, 0.5f)
    };

    glm::vec3 color[36] = {
        glm::vec3(0.583f,  0.771f,  0.014f),
        glm::vec3(0.609f,  0.115f,  0.436f),
        glm::vec3(0.327f,  0.483f,  0.844f),
        glm::vec3(0.822f,  0.569f,  0.201f),
        glm::vec3(0.435f,  0.602f,  0.223f),
        glm::vec3(0.310f,  0.747f,  0.185f),
        glm::vec3(0.597f,  0.770f,  0.761f),
        glm::vec3(0.559f,  0.436f,  0.730f),
        glm::vec3(0.359f,  0.583f,  0.152f),
        glm::vec3(0.483f,  0.596f,  0.789f),
        glm::vec3(0.559f,  0.861f,  0.639f),
        glm::vec3(0.195f,  0.548f,  0.859f),
        glm::vec3(0.014f,  0.184f,  0.576f),
        glm::vec3(0.771f,  0.328f,  0.970f),
        glm::vec3(0.406f,  0.615f,  0.116f),
        glm::vec3(0.676f,  0.977f,  0.133f),
        glm::vec3(0.971f,  0.572f,  0.833f),
        glm::vec3(0.140f,  0.616f,  0.489f),
        glm::vec3(0.997f,  0.513f,  0.064f),
        glm::vec3(0.945f,  0.719f,  0.592f),
        glm::vec3(0.543f,  0.021f,  0.978f),
        glm::vec3(0.279f,  0.317f,  0.505f),
        glm::vec3(0.167f,  0.620f,  0.077f),
        glm::vec3(0.347f,  0.857f,  0.137f),
        glm::vec3(0.055f,  0.953f,  0.042f),
        glm::vec3(0.714f,  0.505f,  0.345f),
        glm::vec3(0.783f,  0.290f,  0.734f),
        glm::vec3(0.722f,  0.645f,  0.174f),
        glm::vec3(0.302f,  0.455f,  0.848f),
        glm::vec3(0.225f,  0.587f,  0.040f),
        glm::vec3(0.517f,  0.713f,  0.338f),
        glm::vec3(0.053f,  0.959f,  0.120f),
        glm::vec3(0.393f,  0.621f,  0.362f),
        glm::vec3(0.673f,  0.211f,  0.457f),
        glm::vec3(0.820f,  0.883f,  0.371f),
        glm::vec3(0.982f,  0.099f,  0.879f)
    };

    // Creació del Vertex Array Object per pintar
    glGenVertexArrays(1, &VAO_Cub);
    glBindVertexArray(VAO_Cub);

    GLuint VBO_Cub[2];
    glGenBuffers(2, VBO_Cub);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posicio), posicio, GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cub[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);


    glBindVertexArray (0);
}

void MyGLWidget::pintaCub()
{
    // Activem el VAO per a pintar la caseta
    glBindVertexArray (VAO_Cub);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
}

void MyGLWidget::creaBuffersTerra()
{
    glm::vec3 posicio[6] = {
        glm::vec3(-1.f, -1.f, 1.f),
        glm::vec3(1.f, -1.f, 1.f),
        glm::vec3(1.f, -1.f, -1.f),

        glm::vec3(1.f, -1.f, -1.f),
        glm::vec3(-1.f, -1.f, -1.f),
        glm::vec3(-1.f, -1.f, 1.f)
    };

    glm::vec3 color[6] = {
        glm::vec3(0.2,0.2,0.2),
        glm::vec3(0.2,0.2,0.2),
        glm::vec3(0.2,0.2,0.2),
        glm::vec3(0.2,0.2,0.2),
        glm::vec3(0.2,0.2,0.2),
        glm::vec3(0.2,0.2,0.2)
    };

    // Creació del Vertex Array Object per pintar
    glGenVertexArrays(1, &VAO_Terra);
    glBindVertexArray(VAO_Terra);

    GLuint VBO_Terra[2];
    glGenBuffers(2, VBO_Terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posicio), posicio, GL_STATIC_DRAW);

    // Activem l'atribut vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);

    // Activem l'atribut colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    glBindVertexArray (0);
}

void MyGLWidget::pintaTerra()
{
    // Activem el VAO per a pintar la caseta
    glBindVertexArray (VAO_Terra);

    // pintem
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
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
  // Obtenim identificador per a l'atribut “color” del vertex shader
  colorLoc = glGetAttribLocation (program->programId(), "color");
  // Uniform locations
  transLoc = glGetUniformLocation(program->programId(), "TG");
  projLoc = glGetUniformLocation(program->programId(), "PM");
  viewLoc = glGetUniformLocation(program->programId(), "VM");
}

void MyGLWidget::mousePressEvent(QMouseEvent *e)
{
  xClick = e->x();
  yClick = e->y();

  if (e->button() & Qt::LeftButton &&
      !(e->modifiers() & (Qt::ShiftModifier | Qt::AltModifier | Qt::ControlModifier)))
  {
    DoingInteractive = ROTATE;
  }
  else if (e->button() == Qt::LeftButton && (e->modifiers() & (Qt::ShiftModifier)))
  {
    DoingInteractive = ZOOM;
    xClick = e->x();
    yClick = e->y();
  }
}

void MyGLWidget::mouseReleaseEvent(QMouseEvent *)
{
  DoingInteractive = NONE;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
  makeCurrent();

  if (DoingInteractive == ROTATE)
  {
    anglePsi -= (e->x() - xClick) * (M_PI / 360);
    angleTheta += (e->y() - yClick) * (M_PI / 360);

    xClick = e->x();
    yClick = e->y();
  }
  update();
}


