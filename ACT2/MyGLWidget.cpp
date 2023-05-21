#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
  xClick = yClick = 0;
  DoingInteractive = NONE;
  factorAngleX = factorAngleY = 0;
  angleRick = anglePortal = angleMortyNou = 0;
  primeraPersona = false;
  pintaPortal = true;
  apuntantPortal = true;
  portalDisponible = true;
  posMortyNou = glm::vec3(0, 0, 0);
  pintarMortyNou = false;
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
  glEnable(GL_DEPTH_TEST);
  
  glClearColor(0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffersModels();
  creaBuffersTerra();
  iniEscena();
  iniCamera();
}

void MyGLWidget::iniEscena()
{
  radiEscena = sqrt(80);
  centreEscena = glm::vec3(0,0,0);
}

void MyGLWidget::iniCamera()
{
    float d = 2 * radiEscena;

    obs = vrp + (d * glm::vec3(0, 0, 1));
    vrp = centreEscena;
    up = glm::vec3(0, 1, 0);

    znear = d - radiEscena;
    zfar = d + radiEscena;

    float alpha_v = asin(radiEscena/d);
    fov = (float) 2 * alpha_v;

    raw = 1.0f;

    if (ra < 1) {
      raw = ra;
      fov = 2 * atan(tan(alpha_v/ra));
    }
    else if (ra > 1) {
        raw = ra;
    }

    projectTransform();
    viewTransform();
}

void MyGLWidget::paintGL ()
{
  iniCamera();
  // descomentar per canviar paràmetres
  // glViewport (0, 0, ample, alt);

  // Esborrem el frame-buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Rick
  glBindVertexArray (VAO_models[RICK]);
  RickTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[RICK].faces().size()*3);

  // Morty
  glBindVertexArray (VAO_models[MORTY]);
  MortyTransform();
  glDrawArrays(GL_TRIANGLES, 0, models[MORTY].faces().size()*3);

  //std::cout << angleRick + 180;
  if (pintaPortal and MortyPortalTest(glm::radians(anglePortal + 180) , posMorty)) {
      posMortyNou = posMorty;
      angleMortyNou = angleMorty;
      pintarMortyNou = true;
      pintaPortal = false;
  }

  if (pintarMortyNou) {
      pintaMortyNou();
  }

  glBindVertexArray (VAO_models[PORTAL]);
  PortalTransform();

  // Portal
  if (pintaPortal) {
      glDrawArrays(GL_TRIANGLES, 0, models[PORTAL].faces().size()*3);
  }
  
  // Terra
  glBindVertexArray (VAO_Terra);
  identTransform();
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindVertexArray (0);
}

void MyGLWidget::RickTransform ()
{
  glm::mat4 TG(1.0f);  

  /* 4. Traslladem el Rick a la posicio indicada:
   *        - Posicio actual de la base: (0, 0, 0)
   *        - Posico desitjada: (-2.5, 0, 0) -> traslladem (-2.5f, 0, 0).
   */
  TG = glm::translate(TG, glm::vec3(-2.5f, 0, 0));

  /* 3. Aprofitant que tenim en Rick a l'origen de coordenades, apliquem una rotacio
   *    per rotar-lo 45 o -45 graus segons les tecles Q i E.
  */
  TG = glm::rotate(TG, glm::radians(angleRick), glm::vec3(0, 1, 0));

  /* 2. Inicialment en Rick mira cap a les Z positives. Per tant,
   *    rotem 180 graus sobre l'eix Y per a que miri cap a les z negatives. */
  TG = glm::rotate(TG, glm::radians(180.0f), glm::vec3(0, 1, 0));

  /* 1. Col.loquem la base del Rick (pivot sobre el que farem la rotacio seguent)
   *    a la posicio (0, 0, 0). */
  TG = glm::translate(TG, glm::vec3(1, 0, 0));

  TG = glm::translate(TG, glm::vec3(-1, 0, 0));
  TG = glm::scale(TG, glm::vec3(escalaModels[RICK]));
  TG = glm::translate(TG, -centreBaseModels[RICK]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::MortyTransform ()
{
  glm::mat4 TG(1.0f);

  /* 3. Cal aplicar una altra translacio de posMorty unitats ja que al punt "1."
   *    s'ha restat per poder col.locar-lo a l'origen de coordenades.
  */
  TG = glm::translate(TG, posMorty);

  /* 2. Rotem en Morty sobre si mateix (sobre l'eix Y) angleMorty graus (variable que augmenta o disminueix
   *    45 graus quan es prem la tecla <- o -> respectivament).
  */
  TG = glm::rotate(TG, glm::radians(angleMorty), glm::vec3(0, 1, 0));

  /* 1. Traslladem el morty al punt (0, 0, 0), per despres aplicarli una rotacio sobre si mateix.
   *    Ja que totes les rotacions, es fan sempre sobre l'origen de coordenades.
  */
  TG = glm::translate(TG, -posMorty);

  TG = glm::translate(TG, posMorty);
  TG = glm::scale(TG, glm::vec3(escalaModels[MORTY]));
  TG = glm::translate(TG, -centreBaseModels[MORTY]);
  TGMorty = TG;
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::MortyNouTransform()
{
  glm::mat4 TG(1.0f);

  TG = glm::translate(TG, posMortyNou);
  TG = glm::rotate(TG, glm::radians(angleMortyNou), glm::vec3(0, 1, 0));
  TG = glm::scale(TG, glm::vec3(escalaModels[MORTY]));
  TG = glm::translate(TG, -centreBaseModels[MORTY]);

  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::pintaMortyNou()
{
    glBindVertexArray (VAO_models[MORTY]);
    MortyNouTransform();
    glUniform1i(colorNouLoc, 1); // Com que volem que nomes es pintin de vermell els vertex del Morti Nou amb la seva respectiva TG,
                                 // Enviem un true al fragment shader.
    glDrawArrays(GL_TRIANGLES, 0, models[MORTY].faces().size()*3);
    glUniform1i(colorNouLoc, 0); // Una vegada pintat, enviem un false per no multiplicar la resta de vertex amb els valors del color vermell.
}

void MyGLWidget::PortalTransform ()
{
  glm::mat4 TG(1.0f);

  /* 4. Canviem el punt de rotacio (pivot), que sempre es l'origen de coordenades (0, 0, 0)
   *    al punt on es troba el Rick (-2.5f, 0, 0) per tal de rotar el portal mantenint una
   *    distancia de 3 unitats respecte en Rick (radi de la rotacio = 3, distancia que hem aplicat al punt anterior "3.").
  */
  TG = glm::translate(TG, glm::vec3(-2.5f, 0, 0));
  TG = glm::rotate(TG, glm::radians(anglePortal), glm::vec3(0, 1, 0));
  TG = glm::translate(TG, glm::vec3(2.5f, 0, 0));

  /* 3. Traslladem el Portal a la posicio indicada:
   *      - Posicio actual: (0, 0, 0)
   *      - Posicio desitjada:
   *                - "Davant en Rick" (-2.5, 0, 0): traslladem (-2.5f, 0, 0).
   *                - "a una distància de 3": traslladem (0, 0, -3).
  */
  TG = glm::translate(TG, glm::vec3(-2.5f, 0, -3.0f));

  /* 2. Escalem l'amplada del portal a la mesura indicada:
   *      - Amplada actual: EscalaModels[PORTAL].
   *      - Amplada desitjada: "4 cops més estret", amb amplada ens referim a l'eix X -> EscalaModels[PORTAL]/4 sobre X.
  */
  TG = glm::scale(TG, glm::vec3(escalaModels[PORTAL]/4, 1, 1));

  /* 1. Col.loquem la base del Portal a la posicio (0, 0, 0). */
  TG = glm::translate(TG, glm::vec3(0, 0, 3));

  TG = glm::translate(TG, glm::vec3(0,0,-3));
  TG = glm::scale(TG, glm::vec3(escalaModels[PORTAL]));
  TG = glm::translate(TG, -centreBaseModels[PORTAL]);
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::identTransform ()
{
  glm::mat4 TG(1.0f);
  glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::projectTransform ()
{
  glm::mat4 Proj(1.0f);

  if (primeraPersona) {
      Proj = glm::perspective (fov, ra, 0.25f, zfar); // En mode Primera Persona volem que el seu znear sigui 0.25.
  }
  else {
      Proj = glm::perspective (fov, ra, znear, zfar);
  }
  glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::viewTransform ()
{
  // Matriu de posició i orientació de l'observador
  glm::mat4 View(1.0f);

  if (primeraPersona) {
      /* Per al mode en primera persona fem servir el metode lookAt amb els seguents parametres:
       *        - obs (posicio de la camera): Com que volem que la camera es mogui segons la posicio d'en Morty,
       *                                      cal que els seus valors X i Z siguin els mateixos que els d'ell. L'altura
       *                                      de la camera (Y) ha de ser 0.8.
       *
       *        - vrp (cap a on mira la camera): Com que la camera sempre ha de mirar endevant perpendicularment
       *                                         a en Morty. Calculem els valors X i Z amb sin i cos del seu angle actual
       *                                         respectivament. Si l'angle de Morty es 45 graus -> la camera
       *                                         ha d'apuntar a (sin(45), 0, cos(45)) == (0.7, 0.8, 0.7). Que es la direccio cap a
       *                                         on es dibuixaria la hipotenusa resultant, a una altura de 0.8.
       *
       *                                             /|
       *                                            / | sin(45)
       *                                           /  |
       *                                          /45)|
       *                                          -----
       *                                          cos(45)
       *
       *  Finalment li sumem obs a vrp ja que al avancar en morty, el vrp s'ha d'allunyar.
      */
      obs = glm::vec3(posMorty.x, 0.8, posMorty.z);
      vrp = glm::vec3(sin(glm::radians(angleMorty)), 0, cos(glm::radians(angleMorty)));
      up = glm::vec3(0, 1, 0);
      View = glm::lookAt(obs, obs + vrp, up);
  }
  else {

      /* 5. Traslladem la camera a una distancia de -2 * radiEscena partint del centre de l'escena. */
      View = glm::translate(View, glm::vec3(0.0, 0.0, -2 * radiEscena));

      /* 4. Aprofitant que el centre de l'escena es troba a (0, 0, 0), rotem respecte X
       *    per poder girar l'escena d'adalt a baix i viceversa segons la variable factorAngleY que s'actualitza
       *    amb el moviment del ratoli.
      */
      View = glm::rotate(View, factorAngleY, glm::vec3(1.0, 0.0, 0.0));

      /* 3. Com volem tenir inicialment l'escena amb una inclinació vertical inicial de 45
       *    graus, apliquem una rotacio de 45 graus sobre l'eix x.
      */
      View = glm::rotate(View, glm::radians(45.0f), glm::vec3(1.0, 0.0, 0.0));

      /* 2. Aprofitant que el centre de l'escena es troba a (0, 0, 0), rotem respecte Y
       *    per poder girar l'escena d'esquerra a dreta i viceversa segons la variable factorAngleX que s'actualitza
       *    amb el moviment del ratoli.
       *    Com volem girar els objectes en sentit contrari, neguem l'angle donat. Ja que el sentit de
       *    la rotacio a openGL ve donada segons la regla de la ma dreta (direccio cap on apunten els dits).
      */
      View = glm::rotate(View, -factorAngleX, glm::vec3(0.0, 1.0, 0.0));

      /* 1. Inicialment camera es troba a (0, 0, 0) apuntant cap a z negatives.
       *    S'aplica una translacio cap al centre de l'escena (vrp). En aquest cas com el centre es
       *    troba a l'origen de coordenades no es fa res.
      */
      View = glm::translate(View, glm::vec3(-vrp));
  }
  glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}


void MyGLWidget::resizeGL (int w, int h) 
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif
  ra = float(ample)/float(alt);
  factorAngleY = M_PI / ample;
  factorAngleX = M_PI / alt;
  projectTransform();
}

bool MyGLWidget::MortyPortalTest(float angleRick, glm::vec3 posMorty)
{
  glm::vec3 posPortal = glm::vec3(sin(angleRick),0.0,cos(angleRick))*glm::vec3(3) + glm::vec3(-2.5,0,0);
  // std::cout << "posPortal: (" <<posPortal.x << ", " << posPortal.z << ")" << std::endl;
  // std::cout << "PosMorty: (" << posMorty.x << ", " << posMorty.z << ")" << std::endl;
  return (glm::distance(posMorty,posPortal) <= 0.2);
}


void MyGLWidget::keyPressEvent(QKeyEvent* event) 
{
  makeCurrent();
  switch (event->key()) {
    case Qt::Key_Up: { 
        /* Per al moviment tipus tanc calculem la direccio de moviment segons els sin i cos del seu angle actual aprofitant que fem la
         * seva rotacio sobre si mateix. L'ecplicacio es similiar a la del moviment de camera en primera persona. */
        posMorty = posMorty + glm::vec3(sin(glm::radians(angleMorty)), 0, cos(glm::radians(angleMorty))) * 0.1f;
      break;
    }
    case Qt::Key_Down: {
        posMorty = posMorty - glm::vec3(sin(glm::radians(angleMorty)), 0, cos(glm::radians(angleMorty))) * 0.1f;
      break;
    }
    case Qt::Key_Q: {
        angleRick += 45.0f;
        if (angleRick >= 360.0f or angleRick <= -360.0f)
            angleRick = 0;
        anterior = angleRick;
      break;
    }
    case Qt::Key_E: {
        angleRick -= 45.0f;
        if (angleRick >= 360.0f or angleRick <= -360.0f)
            angleRick = 0;
        anterior = angleRick;
      break;
    }
    case Qt::Key_P: {
      pintaPortal = !pintaPortal;
      if (anglePortal != anterior) {
          portalDisponible = true;
          pintaPortal = true;
          anglePortal = angleRick;
      }
      portalDisponible = (pintaPortal == true);
      break;
    }
    case Qt::Key_Left: {
        angleMorty += 45.0f;
      break;
    }
    case Qt::Key_Right: {
        angleMorty -= 45.0f;
      break;
    }
    case Qt::Key_C: {
        primeraPersona = !primeraPersona;
      break;
    }
    case Qt::Key_R: {
        xClick = yClick = 0;
        DoingInteractive = NONE;
        factorAngleX = factorAngleY = 0;
        angleRick = anglePortal = angleMortyNou = 0;
        primeraPersona = false;
        pintaPortal = true;
        apuntantPortal = true;
        portalDisponible = true;
        posMortyNou = glm::vec3(0, 0, 0);
        pintarMortyNou = false;
        posMorty = glm::vec3(1,0,0);
        angleMorty = 0;
      break;
  }
    default: event->ignore(); break;
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
  if (DoingInteractive == ROTATE and !primeraPersona) {
    factorAngleX -= (e->x() - xClick) * (M_PI / 360);
    factorAngleY += (e->y() - yClick) * (M_PI / 360);
  }
  xClick = e->x();
  yClick = e->y();

  update ();
}

void MyGLWidget::creaBuffersTerra ()
{
  // VBO amb la posició dels vèrtexs
  glm::vec3 posTerra[4] = {
        glm::vec3(-7.5, 0.0, -5.0),
        glm::vec3(-7.5, 0.0,  5.0),
        glm::vec3( 7.5, 0.0, -5.0),
        glm::vec3( 7.5, 0.0,  5.0)
  }; 

  glm::vec3 c(0.8, 0.7, 1.0);
  glm::vec3 colTerra[4] = { c, c, c, c };

  // VAO
  glGenVertexArrays(1, &VAO_Terra);
  glBindVertexArray(VAO_Terra);

  GLuint VBO_Terra[2];
  glGenBuffers(2, VBO_Terra);

  // geometria
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(posTerra), posTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(vertexLoc);

  // color
  glBindBuffer(GL_ARRAY_BUFFER, VBO_Terra[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colTerra), colTerra, GL_STATIC_DRAW);
  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(colorLoc);
  
  glBindVertexArray (0);
}

void MyGLWidget::calculaCapsaModel (Model &p, float &escala, float alcadaDesitjada, glm::vec3 &centreBase)
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
  escala = alcadaDesitjada/(maxy-miny);
  centreBase[0] = (minx+maxx)/2.0; centreBase[1] = miny; centreBase[2] = (minz+maxz)/2.0;
}

void MyGLWidget::creaBuffersModels ()
{
  // Càrrega dels models
  models[RICK].load("./models/Rick.obj");
  models[MORTY].load("./models/Morty.obj");
  models[PORTAL].load("./models/portal.obj");

  // Creació de VAOs i VBOs per pintar els models
  glGenVertexArrays(3, &VAO_models[0]);
  
  float alcadaDesitjada[3] = {2,1,3};
  
  for (int i = 0; i < 3; i++)
  {	
	  // Calculem la capsa contenidora del model
	  calculaCapsaModel (models[i], escalaModels[i], alcadaDesitjada[i], centreBaseModels[i]);
  
	  glBindVertexArray(VAO_models[i]);

	  GLuint VBO[2];
	  glGenBuffers(2, VBO);

	  // geometria
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_vertices(), GL_STATIC_DRAW);
	  glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(vertexLoc);

	  // color
	  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*models[i].faces().size()*3*3,
		   models[i].VBO_matdiff(), GL_STATIC_DRAW);
	  glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	  glEnableVertexAttribArray(colorLoc);  
  }
  
  glBindVertexArray (0);
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

  // Identificador per als  atributs
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc = glGetAttribLocation (program->programId(), "color");

  // Identificadors dels uniform locations
  transLoc = glGetUniformLocation (program->programId(), "TG");
  projLoc  = glGetUniformLocation (program->programId(), "Proj");
  viewLoc  = glGetUniformLocation (program->programId(), "View");

  colorNouLoc = glGetUniformLocation(program->programId(), "colorNou"); // uniform "boolea" per indicar quan multiplicar vertex per vermell.
}

void MyGLWidget::changeCamera(bool b)
{
    primeraPersona = b;
    viewTransform();
    primeraPersona = !primeraPersona;
}
