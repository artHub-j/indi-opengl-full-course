// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

/**
 * @author Arturo Aragón Hidalgo - https://github.com/artHub-j/indi-opengl-full-course
 */
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

MyGLWidget::MyGLWidget(QWidget *parent) : ExamGLWidget(parent) {
    connect(&timerCoins,SIGNAL(timeout()),this,SLOT(rotateCoins()));
    timerCoins.start(80);
    srand (time(NULL));

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 15; ++j) {
            if (laberint[i][j] == 0) posicionsDisponibles.emplace_back(glm::vec3(j, 0, i), false);
        }
    }

    int num_max_monedes = 10;
    int compt = 0;
    while (compt < num_max_monedes) {
        int random = std::rand() % posicionsDisponibles.size();
        if (posicionsDisponibles[random].second == false) {
            //std::cout << "PosCoins: " << (posicionsDisponibles[random].first).x << ", " << (posicionsDisponibles[random].first).z << std::endl;
            posCoinsFinals.push_back(posicionsDisponibles[random].first);
            posicionsDisponibles[random].second = true;
            ++compt;
        }
    }

    angleX = angleY = M_PI/4;
    focusEscena = true;
    primeraPersona = false;
    puntuacio = 0;
}

MyGLWidget::~MyGLWidget() {
}

void MyGLWidget::iniEscena ()
{
    angleMorty = 0.0;
    posMorty = glm::vec3(1.5,0,1.5);
    posMortyLab = glm::vec2(1,1);
    dirMorty = glm::vec3(0,0,1);
  
    angleFantasma = 0.0;
    posFantasma = glm::vec3(8.5,0,10.5);
    posFantasmaLab = glm::vec2(9,7);
    dirFantasma = glm::vec3(-1,0,0);

    //ExamGLWidget::iniEscena();

    creaCapsaContenidora(glm::vec3(15, 1.5, 20), glm::vec3(0, 0, 0));
}


void MyGLWidget::iniCamera ()
{
    //ExamGLWidget::iniCamera();
    angleY = angleX = M_PI/4;

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
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // MORTY
    glBindVertexArray (VAO_models[MORTY]);
    modelTransformMorty();
    glDrawArrays(GL_TRIANGLES, 0, models[MORTY].faces().size()*3);

    // COIN
    for (unsigned int i = 0; i < posCoinsFinals.size(); ++i) {
        glBindVertexArray (VAO_models[COIN]);
        modelTransformCoin(posCoinsFinals[i]);
        glDrawArrays(GL_TRIANGLES, 0, models[COIN].faces().size()*3);
    }

    // FANTASMA
    glBindVertexArray (VAO_models[FANTASMA]);
    modelTransformFantasma();
    glDrawArrays(GL_TRIANGLES, 0, models[FANTASMA].faces().size()*3);

    // LABERINT
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 15; j++) {
            if (laberint[i][j] == 1) {
                glBindVertexArray(VAO_Cub);
                modelTransformParet(glm::vec3(j, 0, i));
            } else if (laberint[i][j] == 0) {
                glBindVertexArray(VAO_Cub);
                modelTransformTerra(glm::vec3(j, 0, i));
            }
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    glBindVertexArray(0);
}

void MyGLWidget::resizeGL (int w, int h)
{
    ample=w;
    alt=h;
    ra = (float)ample/(float)alt;
}

void MyGLWidget::viewTransform ()
{
    //ExamGLWidget::viewTransform();
    View = glm::mat4(1.0f);

    if (primeraPersona) {
      /* Per al mode en primera persona fem servir el metode lookAt amb els seguents parametres:
       *        - obs (posicio de la camera): Com que volem que la camera es mogui segons la posicio d'en Morty,
       *                                      cal que els seus valors X i Z siguin els mateixos que els d'ell. L'altura
       *                                      de la camera (Y) ha de ser 1.5 (alcada del cap d'en Morty).
       *
       *        - vrp (cap a on mira la camera): Com que la camera sempre ha de mirar endevant perpendicularment
       *                                         a en Morty. Calculem els valors X i Z amb sin i cos del seu angle actual
       *                                         respectivament. Si l'angle de Morty es 45 graus -> la camera
       *                                         ha d'apuntar a (sin(45), 0, cos(45)) == (0.7, 1.5, 0.7). Que es la direccio cap a
       *                                         on es dibuixaria la hipotenusa resultant, a una altura de 1.5.
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
        glm::vec3 obs_n = glm::vec3(posMorty.x, 1.3, posMorty.z);
        glm::vec3 vrp_n = glm::vec3(sin(glm::radians(angleMorty)), 0, cos(glm::radians(angleMorty)));
        glm::vec3 up_n = glm::vec3(0, 1, 0);
        View = glm::lookAt(obs_n, obs_n + vrp_n, up_n);
    }
    else {
        View = glm::translate(View, glm::vec3(0.0, 0.0, -2 * radiEsc));
        View = glm::rotate(View, angleY, glm::vec3(1.0, 0.0, 0.0));
        View = glm::rotate(View, -angleX, glm::vec3(0.0, 1.0, 0.0));
        View = glm::translate(View, glm::vec3(-vrp));
    }
    glUniformMatrix4fv (viewLoc, 1, GL_FALSE, &View[0][0]);
}

void MyGLWidget::projectTransform ()
{
    //ExamGLWidget::projectTransform();

    glm::mat4 Proj;  // Matriu de projecció

    if (primeraPersona) {
        //Proj = glm::perspective((float)60.0f, ra, 0.25, zf);
        float zn_n = 0.25;
        Proj = glm::perspective(glm::radians(60.0f), ra, zn_n, zf);
    }
    else {
        Proj = glm::perspective(fov, ra, zn, zf);
    }

    glUniformMatrix4fv (projLoc, 1, GL_FALSE, &Proj[0][0]);
}

void MyGLWidget::modelTransformMorty()
{
    //ExamGLWidget::modelTransformMorty();
    glm::mat4 TG(1.f);

    TG = glm::translate(TG, posMorty);
    TG = glm::rotate(TG, glm::radians((float)angleMorty), glm::vec3(0, 1, 0));

    TG = glm::scale(TG, glm::vec3(1.5 * escalaModels[MORTY]));
    TG = glm::translate(TG, -centreBaseModels[MORTY]);

    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::modelTransformFantasma()
{
    //ExamGLWidget::modelTransformFantasma();
    glm::mat4 TG(1.f);

    TG = glm::translate(TG, posFantasma);

    TG = glm::scale(TG, glm::vec3(0.6*escalaModels[FANTASMA]));
    TG = glm::translate(TG, -centreBaseModels[FANTASMA]);
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}


void MyGLWidget::modelTransformCoin(glm::vec3 pos)
{
    //ExamGLWidget::modelTransformCoin();

    glm::mat4 TG(1.f);

    TG = glm::translate(TG, pos);
    TG = glm::translate(TG, glm::vec3(0.5, 0, 0.5));

    TG = glm::rotate(TG, float(glm::radians(angleCoins)),glm::vec3(0, 1, 0));

    TG = glm::scale(TG, glm::vec3(0.5*escalaModels[COIN]));
    TG = glm::translate(TG, -centreBaseModels[COIN]);
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformParet(glm::vec3 pos)
{
    glm::mat4 TG(1.f);
    TG = glm::translate(TG, pos);
    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformTerra(glm::vec3 pos)
{
    glm::mat4 TG(1.f);
    TG = glm::translate(TG, pos);
    TG = glm::scale(TG, glm::vec3(1, 0.1, 1));

    glUniformMatrix4fv (transLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
    makeCurrent();

    switch (event->key()) {
    case Qt::Key_Left: {
        angleMorty += 90;
        dirMorty = glm::vec3(glm::sin(glm::radians(angleMorty)), 0, glm::cos(glm::radians(angleMorty)));
        updateFantasmaPos(dirMorty);
        viewTransform();
        //enviaPosFocus();
        break;
    }
    case Qt::Key_Right: {
        angleMorty -= 90;
        dirMorty = glm::vec3(glm::sin(glm::radians(angleMorty)), 0, glm::cos(glm::radians(angleMorty)));
        updateFantasmaPos(dirMorty);
        //enviaPosFocus();
        viewTransform();
        break;
    }
    case Qt::Key_Up: {
        dirMorty = glm::normalize(dirMorty);

        int fila = posMortyLab.x + (int)dirMorty.z;
        int columna = posMortyLab.y + (int)dirMorty.x;

        if (fila >= 0 && fila < 20 && columna >= 0 && columna < 15 && laberint[fila][columna] != 1) {
            posMorty += dirMorty;
            posMortyLab.x = fila;
            posMortyLab.y = columna;
        }
        updateFantasmaPos(dirMorty);
        updatePuntuacioMonedes();
        viewTransform();

        //std::cout << "Pos Morty: " << posMorty.x << ", " << posMorty.z << std::endl;
        //std::cout << "Pos Morty Lab: " << posMortyLab.x << ", " << posMortyLab.y << std::endl;

        break;
    }
    case Qt::Key_C: { // // Canvi Camera Escena <=> Camera 1a Persona
        primeraPersona = !primeraPersona;
        emit enviaEstatCamera(primeraPersona);
        projectTransform();
        viewTransform();
        break;
    }
    case Qt::Key_L: { // Canvi de Focus
        focusEscena = !focusEscena;
        enviaPosFocus();
        emit enviaEstatFocus(focusEscena);
        break;
    }
    case Qt::Key_R: { // Reset
        angleX = angleY = M_PI/4;
        focusEscena = true;
        primeraPersona = false;

        angleMorty = 0.0;
        posMorty = glm::vec3(1.5,0,1.5);
        posMortyLab = glm::vec2(1,1);
        dirMorty = glm::vec3(0,0,1);

        angleFantasma = 0.0;
        posFantasma = glm::vec3(8.5,0,10.5);
        posFantasmaLab = glm::vec2(9,7);
        dirFantasma = glm::vec3(-1,0,0);

        puntuacio = 0;

        // Buidar posCoinsFinals per recalcular noves posicions.
        while (not posCoinsFinals.empty()) {
            posCoinsFinals.pop_back();
        }

        // Recalcular Noves posicions Aleatories pels Coins
        int num_max_monedes = 10;
        int compt = 0;
        while (compt < num_max_monedes) {
            int random = std::rand() % posicionsDisponibles.size();
            if (posicionsDisponibles[random].second == false) {
                posCoinsFinals.push_back(posicionsDisponibles[random].first);
                posicionsDisponibles[random].second = true;
                ++compt;
            }
        }

        projectTransform();
        viewTransform();
        enviaPosFocus();
        break;
    }
    default:
        ExamGLWidget::keyPressEvent(event);
        break;
    }

    update();
}


void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{
    makeCurrent();
    if (DoingInteractive == ROTATE && !primeraPersona)
    {
        // Fem la rotació
        angleX -= (e->x() - xClick) * (M_PI / 360);
        angleY += (e->y() - yClick) * M_PI / 360;
        viewTransform ();
    }

    xClick = e->x();
    yClick = e->y();

    update ();
}


void MyGLWidget::updateFantasmaPos(glm::vec3 dirM)
{

    if ((int)dirM.z==1)  angleFantasma = M_PI;
    if ((int)dirM.z==-1)  angleFantasma = 0;
    if ((int)dirM.x==1)  angleFantasma = -M_PI/2.0f;
    if ((int)dirM.x==-1)  angleFantasma = M_PI/2.0f;

    dirFantasma=glm::vec3(glm::sin(angleFantasma),0,glm::cos(angleFantasma));
    posFantasma+=dirFantasma;

    if (posFantasma.x>15 || posFantasma.x<0 || posFantasma.z>15 || posFantasma.z<0)
        posFantasma-=dirFantasma;
}


void MyGLWidget::rotateCoins()
{
    if (girant) {
        makeCurrent();
        angleCoins += 20;
        update();
    }
}


void MyGLWidget::carregaShaders()
{
    ExamGLWidget::carregaShaders();

    posFocusLoc = glGetUniformLocation (program->programId(), "posFoc");
}

// --------------------------- Nous Metodes Alumne ------------------------

void MyGLWidget::creaCapsaContenidora(glm::vec3 pMin, glm::vec3 pMax)
{
    centreEsc = (pMax + pMin) / glm::vec3(2, 2, 2);
    radiEsc = sqrt(pow(pMax.x - pMin.x, 2) + pow(pMax.y - pMin.y, 2) + pow(pMax.z - pMin.z, 2)) / 2.f;
}

void MyGLWidget::enviaPosFocus()
{
    if (!focusEscena) {
        posFocus = View * glm::vec4(posMorty.x, 1, posMorty.z, 1);
    }
    else {
        posFocus = glm::vec4(0, 0, 0, 1);
    }

    glUniform3fv(posFocusLoc, 1, &posFocus[0]);
}

void MyGLWidget::atura() {
    girant = false;
    timerCoins.stop();
}

void MyGLWidget::engega() {
    girant = true;
    timerCoins.start(80);
}

void MyGLWidget::setCamPrimeraPersona()
{
    makeCurrent();
    if (!primeraPersona)
        primeraPersona = true;
    projectTransform();
    viewTransform();
    update();
}

void MyGLWidget::setCamEscena()
{
    makeCurrent();
    if (primeraPersona)
        primeraPersona = false;
    projectTransform();
    viewTransform();
    update();
}

void MyGLWidget::setFocusMorty()
{
    makeCurrent();
    if (focusEscena)
        focusEscena = false;
    enviaPosFocus();
    update();
}

void MyGLWidget::setFocusCamera()
{
    makeCurrent();
    if (!focusEscena)
        focusEscena = true;
    enviaPosFocus();
    update();
}

void MyGLWidget::reset()
{
    makeCurrent();
    angleX = angleY = M_PI/4;
    focusEscena = true;
    primeraPersona = false;

    angleMorty = 0.0;
    posMorty = glm::vec3(1.5,0,1.5);
    posMortyLab = glm::vec2(1,1);
    dirMorty = glm::vec3(0,0,1);

    angleFantasma = 0.0;
    posFantasma = glm::vec3(8.5,0,10.5);
    posFantasmaLab = glm::vec2(9,7);
    dirFantasma = glm::vec3(-1,0,0);

    puntuacio = 0;

    // Buidar posCoinsFinals per recalcular noves posicions.
    while (not posCoinsFinals.empty()) {
        posCoinsFinals.pop_back();
    }

    // Recalcular Noves posicions Aleatories pels Coins
    int num_max_monedes = 10;
    int compt = 0;
    while (compt < num_max_monedes) {
        int random = std::rand() % posicionsDisponibles.size();
        if (posicionsDisponibles[random].second == false) {
            posCoinsFinals.push_back(posicionsDisponibles[random].first);
            posicionsDisponibles[random].second = true;
            ++compt;
        }
    }

    projectTransform();
    viewTransform();
    enviaPosFocus();

    update();
}

void MyGLWidget::updatePuntuacioMonedes()
{
    for (auto it = posCoinsFinals.begin(); it != posCoinsFinals.end(); ++it) {
        if (posMortyLab.x == (*it).z and posMortyLab.y == (*it).x) {
            posCoinsFinals.erase(it);
            puntuacio++;
            emit updatePuntuacio(puntuacio);

            if (puntuacio == 10)
                emit updateVictoria("Has Guanyat");

            break;
        }
    }
}
