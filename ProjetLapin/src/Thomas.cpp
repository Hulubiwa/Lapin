#include <GL/glut.h>
#include <iostream>
#include <cmath>

char presse;
int anglex, angley, x, y, xold, yold;
int zoom = 15;
const double pi = 3.14159265359;

void affichage();
void clavier(unsigned char touche, int x, int y);
void changeViewObject(int touche, int x, int y);
void reshape(int x, int y);
void idle();
void mouse(int bouton, int etat, int x, int y);
void mousemotion(int x, int y);

//Fonctions de geometrie
void repere();
void drawCylinder(double radius, double height, int numSlices);
void drawEllipsoid(double a, double b, double c, int numSlices, int numStacks);
void drawHalfEllipsoid(double a, double b, double c, int numSlices, int numStacks);
void drawSphere(double radius, int numSlices, int numStacks);
void drawHalfEllipse(double a, double b, int numSegments, bool isUpperHalf);
void drawEllipse(double a, double b, int numSegments);
void drawCustomCylinder(double radiusBottom, double centralRadius, double radiusTop, double height, int numSlices);
void drawCylinderWithTwoRadius(double radiusTop, double radiusBottom, double height, int numSlices);


//Fonctions de dessin
void drawFoots(double taillePied, double positionPiedGauche[], double positionPiedDroite[],  GLfloat couleurGrisPeau[]);
void drawJambes(double tailleJambe, double positionJambeGauche[], double positionJambeDroite[], GLfloat couleurBleuPantalon[]);
void drawHalfHipsNSexePart(double tailleBassin, double positionBassin[], GLfloat couleurBleuPantalon[]);

void drawSol();
void drawButtocks(double rayon, double position);
//void drawJambes(double x, double y, double z, double rayonHanche);
void drawBras();
void drawMain();
void drawHead();
void drawHipsNBreast(double radiusBottom, double radiusCenter, double radiusTop, double tailleHanche);
void drawBreast();

int main(int argc,char **argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - static_cast<int>(glutGet(GLUT_SCREEN_WIDTH) * 0.8)) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - static_cast<int>(glutGet(GLUT_SCREEN_HEIGHT) * 0.8)) / 2);
    glutInitWindowSize(static_cast<int>(glutGet(GLUT_SCREEN_WIDTH) * 0.8), static_cast<int>(glutGet(GLUT_SCREEN_HEIGHT) * 0.8));
    glutCreateWindow("Projet synthèse d'image - Moreau Thomas & Secula Bastien");

    glClearColor(0.07421875, 0.36328125, 0.75, 1.0);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(2.0);
    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);
    glutSpecialFunc(changeViewObject);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);

    glutMainLoop();
    return 0;
}


void affichage(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(-zoom, zoom, -zoom, zoom, -zoom, zoom);
    glRotatef(angley, 1.0, 0.0, 0.0);
    glRotatef(anglex, 0.0, 1.0, 0.0);

    repere();
    drawSol();
    //drawCylinder(1.0, 2.0, 20);
    //drawEllipsoid(2.0, 1.0, 3.0, 50, 50);
    //drawHalfEllipsoid(2.0, 1.0, 3.0, 50, 50);
    //drawSphere(2.0, 50, 50);
    //drawEllipse(2.0, 1.0, 100);
    //drawHalfEllipse inférieur
    //drawHalfEllipse superieur

    //drawEllipsoid(2.0, 2.0, 1.0, 50, 50);

    //Echelle de proportionnalité
    double tailleTete = 4;
    double tailleTeteProporReel = 4.5;
    double tailleOreille = (tailleTete*6)/tailleTeteProporReel;
    double tailleCou = (tailleTete*1)/tailleTeteProporReel;
    double tailleHautCorps = (tailleTete*1.5)/tailleTeteProporReel;
    double tailleBasCorps = (tailleTete*3)/tailleTeteProporReel;
    double tailleBras = (tailleTete*5.5)/tailleTeteProporReel;
    double tailleMain = (tailleTete*2)/tailleTeteProporReel;
    double tailleBassin = (tailleTete*2.5)/tailleTeteProporReel;
    double tailleJambe = (tailleTete*6)/tailleTeteProporReel;
    double taillePied = (tailleTete*1.5)/tailleTeteProporReel;

    double positionPiedGauche[] = {1.77777778, 0, 0}; //2cm avec produit en croix de l'echelle de proportionnalité
    double positionPiedDroite[] = {-1.77777778, 0, 0};
    double positionJambeGauche[] = {1.77777778, taillePied - 1.0/3 + tailleJambe/2 - taillePied/2, -taillePied};
    double positionJambeDroite[] = {-1.77777778, taillePied - 1.0/3 + tailleJambe/2 - taillePied/2, -taillePied};
    double positionBassin[] = {0, taillePied + tailleJambe + tailleBassin/2, 0};
    double positionBasCorps[] = {0, taillePied + tailleJambe + tailleBassin + tailleBasCorps/2, 0};
    double positionHautCorps[] = {0, taillePied + tailleJambe + tailleBassin + tailleBasCorps + tailleHautCorps/2, 0};
    double positionCou[] = {0, taillePied + tailleJambe + tailleBassin + tailleBasCorps + tailleHautCorps + tailleCou/2, 0};
    double positionBrasGauche[] = {1.77777778*2, taillePied + tailleJambe + tailleBassin + tailleBasCorps, 0};
    double positionBrasDroite[] = {-1.77777778*2, 0, 0};
    double positionMainGauche[] = {1.77777778*3, taillePied + tailleJambe + tailleBassin + tailleBasCorps/2, 0};
    double positionMainDroite[] = {-1.77777778*3, taillePied + tailleJambe + tailleBassin + tailleBasCorps/2, 0};
    double positionTete[] = {0, taillePied + tailleJambe + tailleBassin + tailleBasCorps + tailleHautCorps + tailleCou + tailleTete/2, 0};
    double positionOreilleGauche[] = {1.77777778, taillePied + tailleJambe + tailleBassin + tailleBasCorps + tailleHautCorps + tailleCou + tailleTete + tailleOreille/2, 0};
    double positionOreilleDroite[] = {-1.77777778, taillePied + tailleJambe + tailleBassin + tailleBasCorps + tailleHautCorps + tailleCou + tailleTete + tailleOreille/2, 0};

    GLfloat couleurNoir[] = {0.0, 0.0, 0.0};
    GLfloat couleurBlanche[] = {1.0, 1.0, 1.0};
    GLfloat couleurBleuPantalon[] = {0.128318584, 0.171875, 0.24609375};
    GLfloat couleurBleuHaut[] = {0.39453125, 0.578125, 0.82421875};
    GLfloat couleurGrisPeau[] = {0.5, 0.5, 0.5};


    /*
    double radiusCenter = 1.0;
    double radiusBottom = (5*radiusCenter)/3;
    double radiusTop = (2.5*radiusCenter)/3;
    double tailleHanche = 4.0;
    double rayonSphereFesseEtHautJambe = (radiusBottom + radiusBottom/2.0) / 2.0;
    */

    drawFoots(taillePied, positionPiedGauche, positionPiedDroite, couleurGrisPeau);
    drawJambes(tailleJambe, positionJambeGauche, positionJambeDroite, couleurBleuPantalon);
    //drawHalfHipsNSexePart(tailleBassin, positionBassin, couleurBleuPantalon);


    //drawHipsNBreast(radiusBottom, radiusCenter, radiusTop, tailleHanche);
    //drawButtocks(rayonSphereFesseEtHautJambe, -tailleHanche/2);
    //drawJambes(rayonSphereFesseEtHautJambe, -tailleHanche/2, -rayonSphereFesseEtHautJambe, radiusBottom);
    //drawBras();
    //drawMain();
    //drawHead();
    //drawBreast();
    //glTranslatef(0.0, -4.0, 0.0);
    //drawCylinderWithTwoRadius(1.0, 0.5, 4.0, 20);

    glutSwapBuffers();
}

void clavier(unsigned char touche, int x, int y){
    switch(touche){
        case 'p': /* affichage du carre plein */
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
            glutPostRedisplay();
            break;
        case 'f': /* affichage en mode fil de fer */
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            glutPostRedisplay();
            break;
        case 's' : /* Affichage en mode sommets seuls */
            glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
            glutPostRedisplay();
            break;
        case 'z': /* zoom in*/
            zoom -= 1;
            if(zoom < 0){
                zoom = 0;
            }
            glutPostRedisplay();
            break;
        case 'Z': /* zoom out*/
            zoom += 1;
            glutPostRedisplay();
            break;
        case 'd':
            glEnable(GL_DEPTH_TEST);
            glutPostRedisplay();
            break;
        case 'D':
            glDisable(GL_DEPTH_TEST);
            glutPostRedisplay();
            break;
        case 'a'://Les faces à l'envers s'affichent en fil de fer
            glPolygonMode(GL_FRONT,GL_FILL);
            glPolygonMode(GL_FRONT,GL_LINE);
            glutPostRedisplay();
            break;
        case 'q': /*la touche 'q' permet de quitter le programme */
            exit(0);
        case 27:
            exit(0);
    }
}

void changeViewObject(int touche, int x, int y){
    switch(touche){
        case GLUT_KEY_LEFT:
            anglex += 10;
            glutPostRedisplay();
            break;
        case GLUT_KEY_UP:
            angley += 10;
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:
            anglex -= 10;
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:
            angley -= 10;
            glutPostRedisplay();
            break;
    }
}

void reshape(int x, int y){
    if(x<y){
        glViewport(0,(y-x)/2,x,x);
    }else{
        glViewport((x-y)/2,0,y,y);
    }
}

void mouse(int button, int state,int x,int y){
    //Quand on presse les boutons
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        presse = 1;
        xold = x;
        yold = y;
    }

    //Quand on relache les boutons
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        presse = 0;
    }
}

void mousemotion(int x,int y){
    if(presse){
        anglex = anglex + (x - xold);
        angley = angley + (y - yold);
        glutPostRedisplay();
    }

    xold = x;
    yold = y;
}


/*
*
*
*
*
* Fonctions d'affciahe de nos objets
*
*
*
*
*/
void repere(){
    //Repère
    glPushMatrix();
    glBegin(GL_LINES); //axe x en rouge
        glColor3f(1.0, 0.0, 0.0);
    	glVertex3f(0.0, 0.0, 0.0);
    	glVertex3f(10.0, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINES); //axe des y en vert
    	glColor3f(0.0, 1.0, 0.0);
    	glVertex3f(0.0, 0.0, 0.0);
    	glVertex3f(0.0, 10.0, 0.0);
    glEnd();

    glBegin(GL_LINES); //axe des z en bleu
    	glColor3f(0.0, 0.0, 1.0);
    	glVertex3f(0.0, 0.0, 0.0);
    	glVertex3f(0.0, 0.0, 10.0);
    glEnd();
    glPopMatrix();
}

/*
*
*
*
*
* Fonctions de créations d'objets à partir de leurs équations paramètriques
*
*
*
*
*/

void drawCylinder(double radius, double height, int numSlices){
    const double angleStep = 2.0 * pi / numSlices;

    glPushMatrix();
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= numSlices; i++){
        double angle = i * angleStep;
        double x = radius * cos(angle);
        double z = radius * sin(angle);

        glVertex3d(x, -height / 2.0, z); // Bas du cylindre
        glVertex3d(x, height / 2.0, z); // Haut du cylindre
    }
    glEnd();

    // Draw the top and bottom faces
    glBegin(GL_TRIANGLE_FAN);
    glVertex3d(0.0, height / 2.0, 0.0); // Center of top face
    for(int i = 0; i <= numSlices; i++){
        double angle = i * angleStep;
        double x = radius * cos(angle);
        double z = radius * sin(angle);
        glVertex3d(x, height / 2.0, z);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex3d(0.0, -height / 2.0, 0.0); // Center of bottom face
    for (int i = 0; i <= numSlices; i++){
        double angle = i * angleStep;
        double x = radius * cos(angle);
        double z = radius * sin(angle);
        glVertex3d(x, -height / 2.0, z);
    }
    glEnd();
    glPopMatrix();
}

void drawEllipsoid(double a, double b, double c, int numSlices, int numStacks){
    const double phiStep = pi / numStacks;
    const double thetaStep = 2.0 * pi / numSlices;

    for(int i = 0; i < numStacks; i++){
        double phi1 = i * phiStep;
        double phi2 = (i + 1) * phiStep;

        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j <= numSlices; j++){
            double theta = j * thetaStep;
            double x1 = a * sin(phi1) * cos(theta);
            double y1 = b * sin(phi1) * sin(theta);
            double z1 = c * cos(phi1);

            double x2 = a * sin(phi2) * cos(theta);
            double y2 = b * sin(phi2) * sin(theta);
            double z2 = c * cos(phi2);

            glVertex3d(x1, y1, z1);
            glVertex3d(x2, y2, z2);
        }
        glEnd();
    }
}

void drawHalfEllipsoid(double a, double b, double c, int numSlices, int numStacks){
    const double phiStep = pi / (2 * numStacks);
    const double thetaStep = 2.0 * pi / numSlices;

    for(int i=0; i<numStacks; i++){
        double phi1 = i * phiStep;
        double phi2 = (i + 1) * phiStep;

        glPushMatrix();
        glBegin(GL_QUAD_STRIP);
        for(int j=0; j<=numSlices; j++){
            double theta = j * thetaStep;
            double x1 = a * sin(phi1) * cos(theta);
            double y1 = b * sin(phi1) * sin(theta);
            double z1 = c * cos(phi1);

            double x2 = a * sin(phi2) * cos(theta);
            double y2 = b * sin(phi2) * sin(theta);
            double z2 = c * cos(phi2);

            glVertex3d(x1, y1, z1);
            glVertex3d(x2, y2, z2);
        }
        glEnd();
        glPopMatrix();
    }
}

void drawSphere(double radius, int numSlices, int numStacks){
    const double phiStep = pi / numStacks;
    const double thetaStep = 2.0 * pi / numSlices;

    for(int i = 0; i < numStacks; i++){
        double phi1 = i * phiStep;
        double phi2 = (i + 1) * phiStep;

        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j <= numSlices; j++){
            double theta = j * thetaStep;
            double x1 = radius * sin(phi1) * cos(theta);
            double y1 = radius * sin(phi1) * sin(theta);
            double z1 = radius * cos(phi1);

            double x2 = radius * sin(phi2) * cos(theta);
            double y2 = radius * sin(phi2) * sin(theta);
            double z2 = radius * cos(phi2);

            glVertex3d(x1, y1, z1);
            glVertex3d(x2, y2, z2);
        }
        glEnd();
    }
}

void drawEllipse(double a, double b, int numSegments){
    const double angleStep = 2.0 * pi / numSegments;

    glPushMatrix();
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < numSegments; i++){
        double angle = i * angleStep;
        double x = a * cos(angle);
        double y = b * sin(angle);
        glVertex2d(x, y);
    }
    glEnd();
    glPopMatrix();
}

void drawHalfEllipse(double a, double b, int numSegments, bool isUpperHalf){
    const double angleStep = pi / numSegments;

    glPushMatrix();
    if(isUpperHalf){
        glBegin(GL_LINE_STRIP);
        for(int i = 0; i <= numSegments; i++){
            double angle = i * angleStep;
            double x = a * cos(angle);
            double y = b * sin(angle);
            glVertex2d(x, y);
        }
    }else{
        glBegin(GL_LINE_STRIP);
        for(int i = 0; i <= numSegments; i++){
            double angle = i * angleStep;
            double x = a * cos(angle);
            double y = -b * sin(angle);
            glVertex2d(x, y);
        }
    }

    glEnd();
    glPopMatrix();
}

void drawCustomCylinder(double radiusBottom, double centralRadius, double radiusTop, double height, int numSlices){
    const double angleStep = 2.0 * pi / numSlices;

    // Draw from the bottom to the middle disk
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSlices; i++) {
        double angle = i * angleStep;
        double xBottom = radiusBottom * cos(angle);
        double zBottom = radiusBottom * sin(angle);
        double xCentral1 = centralRadius * cos(angle);
        double zCentral1 = centralRadius * sin(angle);
        double xCentral2 = centralRadius * cos(angle + angleStep);
        double zCentral2 = centralRadius * sin(angle + angleStep);

        // Bottom disk
        glVertex3d(xBottom, -height / 2.0, zBottom);

        // Central disk (1st set)
        glVertex3d(xCentral1, -height / 4.0, zCentral1);
    }
    glEnd();

    // Draw from the middle (1st set) to the middle (2nd set) disk
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSlices; i++) {
        double angle = i * angleStep;
        double xCentral1 = centralRadius * cos(angle);
        double zCentral1 = centralRadius * sin(angle);
        double xCentral2 = centralRadius * cos(angle + angleStep);
        double zCentral2 = centralRadius * sin(angle + angleStep);

        // Central disk (1st set)
        glVertex3d(xCentral1, -height / 4.0, zCentral1);

        // Central disk (2nd set)
        glVertex3d(xCentral2, -height / 8.0, zCentral2);
    }
    glEnd();

    // Draw from the middle (2nd set) to the top disk
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= numSlices; i++) {
        double angle = i * angleStep;
        double xCentral2 = centralRadius * cos(angle);
        double zCentral2 = centralRadius * sin(angle);
        double xTop = radiusTop * cos(angle);
        double zTop = radiusTop * sin(angle);

        // Central disk (2nd set)
        glVertex3d(xCentral2, -height / 8.0, zCentral2);

        // Top disk
        glVertex3d(xTop, height / 2.0, zTop);
    }
    glEnd();
}

void drawCylinderWithTwoRadius(double radiusTop, double radiusBottom, double height, int numSlices){
    const double angleStep = 2.0 * pi / numSlices;

    glBegin(GL_QUAD_STRIP);
    for (int i=0; i<=numSlices; i++){
        double angle = i * angleStep;
        double xTop = radiusTop * cos(angle);
        double zTop = radiusTop * sin(angle);
        double xBottom = radiusBottom * cos(angle);
        double zBottom = radiusBottom * sin(angle);

        // Top disk
        glVertex3d(xTop, height / 2.0, zTop);

        // Bottom disk
        glVertex3d(xBottom, -height / 2.0, zBottom);
    }
    glEnd();
}

/*
*
*
*
*
* Fonctions de dessins de certaines partie du corps
*
*
*
*
*/

void drawSol(){
    bool isBlack = true;
    const int numSquares = 20;

    glPushMatrix();
    glTranslated(-numSquares/2, 0, -numSquares/2);

    GLfloat couleurNoir[] = {0.0, 0.0, 0.0};
    GLfloat couleurBlanche[] = {1.0, 1.0, 1.0};
    for(int i=0; i<numSquares; i++){
        for(int j=0; j<numSquares; j++){
            if(isBlack){
                glColor3f(couleurNoir[0], couleurNoir[1], couleurNoir[2]); // Noir
            }else{
                glColor3f(couleurBlanche[0], couleurBlanche[1], couleurBlanche[2]); // Blanc
            }

            glBegin(GL_QUADS);
            glVertex3d(i, 0, j);
            glVertex3d(i + 1, 0, j);
            glVertex3d(i + 1, 0, j + 1);
            glVertex3d(i, 0, j + 1);
            glEnd();

            isBlack = !isBlack;
        }
        isBlack = !isBlack;
    }

    glPopMatrix();
}

void drawFoots(double taillePied, double positionPiedGauche[], double positionPiedDroite[],  GLfloat couleurGrisPeau[]){
    glColor3f(couleurGrisPeau[0], couleurGrisPeau[1], couleurGrisPeau[2]);

    //Pied gauche
    glPushMatrix();

    glTranslatef(positionPiedGauche[0], positionPiedGauche[1], positionPiedGauche[2]);
    glRotatef(-90, 1.0, 0.0, 0.0);
    drawHalfEllipsoid(1.0, 2.0, 1.0, 20, 20);

    glPopMatrix();

    //Pied droite
    glPushMatrix();

    glTranslatef(positionPiedDroite[0], positionPiedDroite[1], positionPiedDroite[2]);
    glRotatef(-90, 1.0, 0.0, 0.0);
    drawHalfEllipsoid(1.0, 2.0, 1.0, 20, 20);

    glPopMatrix();
}

void drawJambes(double tailleJambe, double positionJambeGauche[], double positionJambeDroite[], GLfloat couleurBleuPantalon[]){
    glColor3f(couleurBleuPantalon[0], couleurBleuPantalon[1], couleurBleuPantalon[2]);

    //Jambe gauche
    glPushMatrix();

    glTranslatef(positionJambeGauche[0], positionJambeGauche[1], positionJambeGauche[2]);
    drawCylinderWithTwoRadius(0.75, 0.33333333, tailleJambe, 20);

    glPopMatrix();

    //Jambe droite
    glPushMatrix();

    glTranslatef(positionJambeDroite[0], positionJambeDroite[1], positionJambeDroite[2]);
    drawCylinderWithTwoRadius(0.75, 0.33333333, tailleJambe, 20);

    glPopMatrix();
}

void drawHalfHipsNSexePart(double tailleBassin, double positionBassin[], GLfloat couleurBleuPantalon[]){
    glColor3f(couleurBleuPantalon[0], couleurBleuPantalon[1], couleurBleuPantalon[2]);

    glPushMatrix();

    glTranslatef(positionBassin[0], positionBassin[1], positionBassin[2]);
    double variable = 5.0/3;
    drawCustomCylinder(variable+0.4, variable+0.5, variable, tailleBassin, 20);

    glPopMatrix();
}


































void drawButtocks(double radiusSphereFesse, double position){
    //Faut que je mette un cylindre entre les deux fesses et que j'ajoute 2 sphères
    glPushMatrix();
    glColor3f(0.128318584, 0.171875, 0.24609375);
    //Fesse gauche
    glTranslatef(radiusSphereFesse, position-radiusSphereFesse, -radiusSphereFesse);
    drawSphere(radiusSphereFesse, 50, 50);

    //Fesse droite
    glTranslatef(-2*radiusSphereFesse, 0.0, 0.0);
    drawSphere(radiusSphereFesse, 50, 50);

    glPopMatrix();
}

/*
void drawJambes(double x, double y, double z, double rayonHanche){
    glPushMatrix();
    glColor3f(0.128318584, 0.171875, 0.24609375);

    //Fesses
    glTranslatef(0.0, y-1, 0.0);
    drawCustomCylinder(rayonHanche+0.4, rayonHanche+0.5, rayonHanche, 2, 20);

    glPopMatrix();

    glPushMatrix();
    double tailleJambe = 4.0;
    //Jambe gauche
    glTranslatef(x-0.5, y-tailleJambe/2.0, 0);
    glRotatef(5, 0.0, 0.0, 1.0);
    drawCylinderWithTwoRadius((x+x/2)/2, x/2, tailleJambe, 20);

    glPopMatrix();

    glPushMatrix();
    //Jambe droite
    glTranslatef(-x+0.5, y-tailleJambe/2.0, 0.0);
    glRotatef(-5, 0.0, 0.0, 1.0);
    drawCylinderWithTwoRadius((x+x/2)/2, x/2, tailleJambe, 20);

    glPopMatrix();
}
*/

void drawBras(){
    //Bras gauche
    glPushMatrix();
    glColor3f(0.39453125, 0.578125, 0.82421875);

    glTranslatef(2.8, 1.5, 0.0);
    //glRotatef(90, 0.0, 0.0, 1.0);
    glRotatef(78.75, 0.0, 0.0, 1.0);
    drawCylinderWithTwoRadius(0.5, 0.25, 4.0, 20);
    glPopMatrix();

    //Bras droite
    glPushMatrix();
    glTranslatef(-2.8, 1.5, 0.0);
    glRotatef(-78.75, 0.0, 0.0, 1.0);
    drawCylinderWithTwoRadius(0.5, 0.25, 4.0, 20);

    glPopMatrix();

    //Espace près du cou
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);

    glTranslatef(0.0,  2.0, 0.0);
    drawSphere(1.0, 20, 20);

    glPopMatrix();
}

void drawMain(){
    glPushMatrix();

    glColor3f(0.5, 0.5, 0.5);
    //Main gauche
    glTranslatef(5.0, 1.15, 0.0);
    drawSphere(0.5, 20, 20);
    //Main droite
    glTranslatef(-10.0, 0.0, 0.0);
    drawSphere(0.5, 20, 20);

    glPopMatrix();
}

void drawHead(){
    glPushMatrix();
    glColor3f(0.5, 0.5, 0.5);

    glTranslatef(0.0, 4.0, 0.0);
    drawSphere(2.0, 20, 20);

    glPopMatrix();
}

void drawHipsNBreast(double radiusBottom, double radiusCenter, double radiusTop, double tailleHanche){
    glPushMatrix();
    glColor3f(0.39453125, 0.578125, 0.82421875);

    drawCustomCylinder(radiusBottom, radiusCenter, radiusTop, tailleHanche, 20); //Hanche

    //glTranslatef(0.0, 0.0, 5.0);
    //drawCylinderWithTwoRadius(radiusCenter, radiusTop, tailleHanche/2, 20);

    glPopMatrix();
}

void drawBreast(){
    glPushMatrix();
    glColor3f(0.39453125, 0.578125, 0.82421875);

    //Sein gauche
    //glColor3f(0.0, 0.0, 0.0);
    glTranslatef(0.5, 1.0, 1.0-0.25);
    drawSphere(0.5, 20, 20);

    //Sein droite
    glTranslatef(-1.0, 0.0, 0.0);
    drawSphere(0.5, 20, 20);

    glPopMatrix();
}
