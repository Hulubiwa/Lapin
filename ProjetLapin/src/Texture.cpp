#include <cstdio>
#include <cstdlib>
/****************************************************************/
/* works on mac using freeglut and xquartz                      */
/* you can install freeglut using homebrew                      */
/* xquartz can be downloaded at http://xquartz.macosforge.org   */
/****************************************************************/
#include <GL/freeglut.h>
#include <jpeglib.h>
#include <jerror.h>
#include<math.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __WIN32
#pragma comment (lib, "jpeg.lib")
#endif

unsigned char image[256*256*3];
char presse;
int anglex=30,angley=20,x,y,xold,yold;

// Position de la source lumineuse
GLfloat light_position[] = { -3.0, 1.0, -3.0, 0.0 };

void affichage();
void clavier(unsigned char touche,int x,int y);
void souris(int boutton, int etat,int x,int y);
void sourismouv(int x,int y);
void redim(int l,int h);
void loadJpegImage(char *fichier);
void Jambe();
void Jambe2();
void InitLumieres();
void Nez();
void Pantalon();
void Tore(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur);
void ToreTronque(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur);
void ellipse(int paralleles, int meridiens, float a, float b, float c);
void ellipse(int paralleles, int meridiens, float a, float b, float c);
GLuint firstTexture;
GLuint secondTexture;
GLuint thirdTexture;


class Point{
    public :
	//coordonnées x, y et z du point
	double x;
	double y;
	double z;
	// couleur r, v et b du point
	float r;
	float g;
	float b;
};

int main(int argc,char **argv)

{


  /* initialisation de glut et creation
     de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(200,200);
  glutInitWindowSize(800, 600);
  glutCreateWindow("cube");

  /* Initialisation d'OpenGL */
  glClearColor(0.0,0.0,0.0,0.0);
  glColor3f(1.0,1.0,1.0);
  glPointSize(2.0);
  glEnable(GL_DEPTH_TEST);



  //Génère des Id pour les textures
    glGenTextures(1, &firstTexture);
    glGenTextures(1, &secondTexture);
    glGenTextures(1, &thirdTexture);

  /* Chargement de la texture*/
      /*loadJpegImage("Image/NoirBlanc.jpg");
      glBindTexture(GL_TEXTURE_2D, firstTexture);


      loadJpegImage("Image/CarreBlanc.jpg");
      glBindTexture(GL_TEXTURE_2D, secondTexture);

      loadJpegImage("Image/CarreNoir.jpg");
      glBindTexture(GL_TEXTURE_2D, thirdTexture);

      /* Parametrage du placage de textures*/
      /*glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
      glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,
               GL_RGB,GL_UNSIGNED_BYTE,image);
      glEnable(GL_TEXTURE_2D);*/

      //préparation de la lumière
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    InitLumieres();

  /* Mise en place des fonctions de rappel*/
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutMouseFunc(souris);
  glutMotionFunc(sourismouv);
  glutReshapeFunc(redim);

  /* Entr�e dans la boucle principale glut */
  glutMainLoop();
  return 0;
}



void affichage()
{
    // Effacement de l'image avec la couleur de fond
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(angley,1.0,0.0,0.0);
    glRotatef(anglex,0.0,1.0,0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int k = 2;
    glOrtho(-k,k,-k,k,k,-k);
    glMatrixMode(GL_MODELVIEW);
    Tore(10,10,1,1,1,0.5);
    glFlush();

    glutSwapBuffers();
}




///INIT LUMIERE
void InitLumieres(){

    glEnable(GL_DEPTH_TEST); // Activation du test de profondeur
    glEnable(GL_LIGHTING);   // Activation de l'éclairage
    glEnable(GL_LIGHT0);     // Activation de la première source de lumière

    GLfloat light_position[] = {0, 0, -30.0}; // Position de la lumière (directionnelle)
    GLfloat light_direction[] = {0.0, 0.0, -1.0};
    GLfloat light_ambient[] = {0.2, 0.2, 0.2, 1.0}; // Couleur de la lumière ambiante
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  // Couleur de la lumière (blanche)
    GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0}; // Couleur de la lumière spéculaire (blanche)
    GLfloat spot_cutoff = 180.0; // Angle de diffusion (en degrés)
    GLfloat spot_exponent = 2.0; // Exposant de la lumière

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // Définition de la position
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);    // Définition de la couleur
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);

    glEnable(GL_LIGHT1); // Activation de la deuxième source de lumière (ambiante)
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient); // Définition de la couleur ambiante
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);  // Définition de la couleur spéculaire
}

void ellipse(int paralleles, int meridiens, float a, float b, float c) {
    const float du = 2 * M_PI / meridiens;
    const float dv = M_PI / (paralleles - 1);

    for (int i = 0; i < meridiens; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= paralleles; j++) {
            float u = i * du;
            float v = j * dv + (-M_PI / 2);

            float x = a * cos(u) * cos(v);
            float y = b * sin(u) * cos(v);
            float z = c * sin(v);

            float l = sqrt(x * x + y * y + z * z);
            float nx = x / l;
            float ny = y / l;
            float nz = z / l;

            glNormal3f(nx, ny, nz);
            // Vous pouvez configurer ici les propriétés matérielles (glMaterialfv) si nécessaire
            glVertex3f(x, y, z);

            float u1 = (i + 1) * du;
            float x1 = a * cos(u1) * cos(v);
            float y1 = b * sin(u1) * cos(v);
            float z1 = c * sin(v);

            float l1 = sqrt(x1 * x1 + y1 * y1 + z1 * z1);
            float nx1 = x1 / l1;
            float ny1 = y1 / l1;
            float nz1 = z1 / l1;

            glNormal3f(nx1, ny1, nz1);
            // Configurer les propriétés matérielles ici
            glVertex3f(x1, y1, z1);
        }
        glEnd();
    }
}


///NEZ
void Nez(){

    ///Cette fonction construit le nez

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0, 1.0};
    GLfloat material_ambient[] = {158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0, 1.0};
    GLfloat material_specular[] = {158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    Point pNez [9] = {
        {1.4, -0.4, 0, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {1, 0.5, 0, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {-1, 0.5, 0, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {-1.4, -0.4, 0, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {-0.4, -1.4, 0, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {0.4, -1.4, 0, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {0, -1, 0.5, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {1, 0, 0.5, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0},
        {-1, 0, 0.5, 158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0}
    };

    int fNez [4][4] = {
        {6,5,0,7},
        {8,7,1,2},
        {6,8,3,4},
        {6,7,8,6}
    };

    int i,j;
    for (i=0;i<4;i++)
    {
      glBegin(GL_POLYGON);
          for (j=0;j<4;j++){
              glColor3f(pNez[fNez[i][j]].r,pNez[fNez[i][j]].g,pNez[fNez[i][j]].b);
              glVertex3f(pNez[fNez[i][j]].x,pNez[fNez[i][j]].y,pNez[fNez[i][j]].z);
          }
      glEnd();
    }

    glPushMatrix();
        glTranslatef(0.0, -1.0, 0.0);
        ellipse(50,50,0.5,0.5,0.5);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(1, 0, 0.0);
        ellipse(50,50,0.5,0.5,0.5);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-1, 0, 0.0);
        ellipse(50,50,0.5,0.5,0.5);
    glPopMatrix();

}

///TORE
void Tore(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur){

    ///Cette fonction permet de construire un tore

    for (int i = 0; i < nbParallele/division; i++) {
        float theta2 = 2.0f * M_PI * i / nbParallele;
        float x =  a*cos(theta2);
        float z =  c*sin(theta2);
        float NextTheta = 2.0f * M_PI * (i+1) / nbParallele;
        float NextX = a*cos(NextTheta);
        float NextZ = c*sin(NextTheta);

        glBegin(GL_TRIANGLE_STRIP);
            for (int j = 0; j <= nbRotationMeridien; j++) {
                float theta = 2.0f * M_PI * j / nbRotationMeridien;
                float rotationAngle = 90* M_PI / 180.0f; /// Angle de rotation en radians (90 degrés)
                float beta = M_PI - (M_PI/2) - theta2;
                float NextBeta = M_PI - (M_PI/2) - NextTheta;

                /// Appliquer la rotation autour de l'axe z
                float rotatedX = x+cos(rotationAngle)*cos(theta);
                float rotatedY = epaisseur*sin(rotationAngle)*cos(theta);
                float rotatedZ = z+epaisseur*sin(theta);

                float NewX = rotatedX - x;
                float Newz = rotatedZ - z;

                float X = x + cos(-beta) * NewX - sin(-beta) * Newz;
                float Z = z + sin(-beta) * NewX + cos(-beta) * Newz;

                float l = sqrt(X * X + rotatedY * rotatedY + Z * Z);
                float nx = X / l;
                float ny = rotatedY / l;
                float nz = Z / l;

                rotatedX = NextX+cos(rotationAngle)*cos(theta);
                rotatedZ = NextZ+epaisseur*sin(theta);

                NewX = rotatedX - NextX;
                Newz = rotatedZ - NextZ;

                float NX = NextX + cos(-NextBeta) * NewX - sin(-NextBeta) * Newz;
                float NZ = NextZ + sin(-NextBeta) * NewX + cos(-NextBeta) * Newz;

                float x1 = NX;
                float y1 = rotatedY;
                float z1 = NZ;

                float l1 = sqrt(x1 * x1 + y1 * y1 + z1 * z1);
                float nx1 = x1 / l1;
                float ny1 = y1 / l1;
                float nz1 = z1 / l1;

                glNormal3f(nx, ny, nz);
                glVertex3f(X, rotatedY, Z);

                glNormal3f(nx1, ny1, nz1);
                glVertex3f(NX, rotatedY, NZ);
            }
        glEnd();
    }
}

///TORE TRONQUE
void ToreTronque(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur){

    /**Cette fonction nous permet de construire un tore tronqué. Cette fonction fonction nous sert dans notre projet à
    * créer des poils. Lors de la création du tore, on réduit au fur et à mesure son épaisseur. A la fin de cette réduction
    * l'épaisseur doit valoir 0 à fin de la construction du tore.
    **/

    for (int i = 0; i < nbParallele/division; i++) {
        float theta2 = 2.0f * M_PI * i / nbParallele;
        float x =  a*cos(theta2);
        float z =  c*sin(theta2);
        float NextTheta = 2.0f * M_PI * (i+1) / nbParallele;
        float NextX = a*cos(NextTheta);
        float NextZ = c*sin(NextTheta);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= nbRotationMeridien; j++) {
            float theta = 2.0f * M_PI * j / nbRotationMeridien;
            float rotationAngle = 90* M_PI / 180.0f; /// Angle de rotation en radians (90 degrés)
            float beta = M_PI - (M_PI/2) - theta2;

            float NextBeta = M_PI - (M_PI/2) - NextTheta;
            float ep = (epaisseur -((epaisseur /(nbParallele/division))*i));
            float Nextep = (epaisseur -((epaisseur /(nbParallele/division))*(i+1)));

            /// Appliquer la rotation autour de l'axe z
            float rotatedX = x+cos(rotationAngle)*cos(theta);
            float rotatedY = ep*sin(rotationAngle)*cos(theta);
            float rotatedZ = z+ep*sin(theta);

            float NewX = rotatedX - x;
            float Newz = rotatedZ - z;

            float X = x + cos(-beta) * NewX - sin(-beta) * Newz;
            float Z = z + sin(-beta) * NewX + cos(-beta) * Newz;

            rotatedX = NextX+cos(rotationAngle)*cos(theta);
            float rotatedY2 = Nextep*sin(rotationAngle)*cos(theta);
            rotatedZ = NextZ+Nextep*sin(theta);

            NewX = rotatedX - NextX;
            Newz = rotatedZ - NextZ;

            float NX = NextX + cos(-NextBeta) * NewX - sin(-NextBeta) * Newz;
            float NZ = NextZ + sin(-NextBeta) * NewX + cos(-NextBeta) * Newz;


            glVertex3f(X, rotatedY, Z);
            glVertex3f(NX, rotatedY2, NZ);
        }
        glEnd();
    }

}

void Pantalon(){

    glColor3f(1,1,1);

    int nbM = 20;  // Nombre de méridiens
    int nbP = 20; // Nombre de parallèles

    float a = 1;
    float b = 1;
    float c = 1;

    const float du=2*M_PI/nbM;
    const float dv=M_PI/(nbP-1);

    Point pEllipse[nbM*nbP];
    int fEllipse[nbM][nbP-1][4];

    ///Construction du tableau de points
    for(int i=0; i<nbM; i++){
        for(int j=0; j<nbP; j++){

            float u = i * du;
            float v = j * dv +(-M_PI/2);

            float X = a * cos(u) * cos(v);
            float Y = b * sin(u) * cos(v);
            float Z = c * sin(v);

            pEllipse[i+(j*nbM)].x = X;
            pEllipse[i+(j*nbM)].y = Y;
            pEllipse[i+(j*nbM)].z = Z;
        }
    }

    ///Construction du tableau de faces
    for(int i=0; i<nbM; i++){
        for(int j=0; j<nbP-1; j++){

            fEllipse[i][j][0] = i + (j*nbM);
            fEllipse[i][j][1] = ((i+1)%(nbM-1)) + (j*nbM);
            fEllipse[i][j][2] = ((i+1)%(nbM-1)) + ((j+1)*nbM);
            fEllipse[i][j][3] = i + ((j+1)*nbM);

        }
    }

    glBindTexture(GL_TEXTURE_2D, secondTexture);
    for(int i=0; i<nbM-1; i+=2){
        for(int j=0; j<nbP-1; j++){
            glBegin(GL_POLYGON);
            glTexCoord2f(0,0);
            glVertex3f(pEllipse[fEllipse[i][j][0]].x, pEllipse[fEllipse[i][j][0]].y, pEllipse[fEllipse[i][j][0]].z);
            glTexCoord2f(0,1);
            glVertex3f(pEllipse[fEllipse[i][j][1]].x, pEllipse[fEllipse[i][j][1]].y, pEllipse[fEllipse[i][j][1]].z);
            glTexCoord2f(1,1);
            glVertex3f(pEllipse[fEllipse[i][j][2]].x, pEllipse[fEllipse[i][j][2]].y, pEllipse[fEllipse[i][j][2]].z);
            glTexCoord2f(1,0);
            glVertex3f(pEllipse[fEllipse[i][j][3]].x, pEllipse[fEllipse[i][j][3]].y, pEllipse[fEllipse[i][j][3]].z);
            glEnd();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, thirdTexture);
    for(int i=1; i<nbM; i+=2){
        for(int j=0; j<nbP-1; j++){
            glBegin(GL_POLYGON);
            glTexCoord2f(0,0);
            glVertex3f(pEllipse[fEllipse[i][j][0]].x, pEllipse[fEllipse[i][j][0]].y, pEllipse[fEllipse[i][j][0]].z);
            glTexCoord2f(0,1);
            glVertex3f(pEllipse[fEllipse[i][j][1]].x, pEllipse[fEllipse[i][j][1]].y, pEllipse[fEllipse[i][j][1]].z);
            glTexCoord2f(1,1);
            glVertex3f(pEllipse[fEllipse[i][j][2]].x, pEllipse[fEllipse[i][j][2]].y, pEllipse[fEllipse[i][j][2]].z);
            glTexCoord2f(1,0);
            glVertex3f(pEllipse[fEllipse[i][j][3]].x, pEllipse[fEllipse[i][j][3]].y, pEllipse[fEllipse[i][j][3]].z);
            glEnd();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glColor3f(1,1,1);
}

void Jambe2(){
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, secondTexture);
    Point pCylindre [12];
    int fCylindre [6][4] = {
    {0,6,7,1},
    {1,7,8,2},
    {2,8,9,3},
    {3,9,10,4},
    {4,10,11,5},
    {5,11,6,0}
    };

     int nbDiv = 6;
   float rayon = 1;
   float hauteur = 1;
        for (int i = 0; i < nbDiv; i++) {
            float theta = 2.0f * M_PI * i / nbDiv;
            float x = rayon * cos(theta);
            float z = rayon * sin(theta);
            pCylindre[i].x=x;
            pCylindre[i].y=0;
            pCylindre[i].z=z;

            pCylindre[i+nbDiv].x=x;
            pCylindre[i+nbDiv].y=hauteur;
            pCylindre[i+nbDiv].z=z;
    }


    for(int i=0; i<6; i++){
        glBegin(GL_POLYGON);
        glTexCoord2f((float)(i/6.),0.0);
        glVertex3f(pCylindre[fCylindre[i][0]].x,pCylindre[fCylindre[i][0]].y,pCylindre[fCylindre[i][0]].z);
        glTexCoord2f((float)(i/6.),1.0);
        glVertex3f(pCylindre[fCylindre[i][1]].x,pCylindre[fCylindre[i][1]].y,pCylindre[fCylindre[i][1]].z);
        glTexCoord2f((float)((i+1)/6.),1.0);
        glVertex3f(pCylindre[fCylindre[i][2]].x,pCylindre[fCylindre[i][2]].y,pCylindre[fCylindre[i][2]].z);
        glTexCoord2f((float)((i+1)/6.),0.0);
        glVertex3f(pCylindre[fCylindre[i][3]].x,pCylindre[fCylindre[i][3]].y,pCylindre[fCylindre[i][3]].z);
        glEnd();

    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Jambe(){
    glBindTexture(GL_TEXTURE_2D, firstTexture);
    Point pCylindre [12];
    int fCylindre [6][4] = {
    {0,6,7,1},
    {1,7,8,2},
    {2,8,9,3},
    {3,9,10,4},
    {4,10,11,5},
    {5,11,6,0}
    };

     int nbDiv = 6;
   float rayon = 1;
   float hauteur = 1;
        for (int i = 0; i < nbDiv; i++) {
            float theta = 2.0f * M_PI * i / nbDiv;
            float x = rayon * cos(theta);
            float z = rayon * sin(theta);
            pCylindre[i].x=x;
            pCylindre[i].y=0;
            pCylindre[i].z=z;

            pCylindre[i+nbDiv].x=x;
            pCylindre[i+nbDiv].y=hauteur;
            pCylindre[i+nbDiv].z=z;
    }


    for(int i=0; i<6; i++){
        glBegin(GL_POLYGON);
        glTexCoord2f((float)(i/6.),0.0);
        glVertex3f(pCylindre[fCylindre[i][0]].x,pCylindre[fCylindre[i][0]].y,pCylindre[fCylindre[i][0]].z);
        glTexCoord2f((float)(i/6.),1.0);
        glVertex3f(pCylindre[fCylindre[i][1]].x,pCylindre[fCylindre[i][1]].y,pCylindre[fCylindre[i][1]].z);
        glTexCoord2f((float)((i+1)/6.),1.0);
        glVertex3f(pCylindre[fCylindre[i][2]].x,pCylindre[fCylindre[i][2]].y,pCylindre[fCylindre[i][2]].z);
        glTexCoord2f((float)((i+1)/6.),0.0);
        glVertex3f(pCylindre[fCylindre[i][3]].x,pCylindre[fCylindre[i][3]].y,pCylindre[fCylindre[i][3]].z);
        glEnd();

    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
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
    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);
    }
}

void souris(int bouton, int etat,int x,int y)
{
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_DOWN)
  {
    presse = 1;
    xold = x;
    yold=y;
  }
  if (bouton == GLUT_LEFT_BUTTON && etat == GLUT_UP)
    presse=0;
}

void sourismouv(int x,int y)
  {
    if (presse)
    {
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay();
    }

    xold=x;
    yold=y;
  }

void redim(int l,int h)
{
  if (l<h)
    glViewport(0,(h-l)/2,l,l);
  else
    glViewport((l-h)/2,0,h,h);
}


///LOADJPEGIMAGE
void loadJpegImage(char *fichier){

    ///Cette fonction permet de Charger une texture et de lui affectée un identifiant

  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
    #ifdef __WIN32
      if (fopen_s(&file,fichier,"rb") != 0)
        {
          fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
          exit(1);
        }
    #elif __GNUC__
      if ((file = fopen(fichier,"rb")) == 0)
        {
          fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
          exit(1);
        }
    #endif
      jpeg_stdio_src(&cinfo, file);
      jpeg_read_header(&cinfo, TRUE);


  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*256*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);


}
