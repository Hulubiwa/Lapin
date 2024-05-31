/* inclusion des fichiers d'en-tete freeglut */

#ifdef __APPLE__
#else
#include <GL/freeglut.h> /* Pour les autres systemes */
#endif
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <unistd.h>
#include <jpeglib.h>
#include <jerror.h>

#ifdef __WIN32
#pragma comment (lib, "jpeg.lib")
#endif

unsigned char image[256*256*3];

char presse;
int anglex,angley,x,y,xold,yold;
float zoomFactor=1.0;
float TranslationAnimParapluie=0.0;
float angleMinutes=1.0;
float angleHeures=1.0;
bool animParapluieEnCours=false;
float angleBrasAnimParapluie=30.0;
GLuint firstTexture;
GLuint secondTexture;
GLuint thirdTexture;

/* Prototype des fonctions */
///INTERFACE
void affichage();
void clavier(unsigned char touche,int x,int y);
void flechesClavier(int key, int x, int y);
void reshape(int x,int y);
void idle();
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);
void InitLumieres();
void idle();
void animationParapluie(int etape);

///TEXTURE
void loadJpegImage(char *fichier, GLuint& texture);

///GEOMETRIE
void ellipse(int paralleles, int meridiens, float a, float b, float c);
void demiEllipse(int paralleles, int meridiens, float a, float b, float c);
void FaceDessousDemiEllipse(float a, float c, int num_segments);
void EllipseCoupe(int paralleles, int meridiens, float a, float b, float c, float divp, float divm);
void Cylindre(int nbDiv, float nbtour, float hauteur, float rayon);
void Tore(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur);
void ToreTronque(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur);
void ConeTronque(int nbDiv, float rayon1, float rayon2, float hauteur);
void Cone(float a, float c, float h, int paralleles);

///TETE
void Nez();
void Bouche();
void Museau();
void HautDeTete();
void Tete();
void Oeil();
void joue();
void Oreille();
void Sourcil();
void Lunette();
void Moustache();

///CORPS
void Jambe();
void corps();
void Pantalon();
void montre();
void parapluie();

int main(int argc, char **argv) {
    /* initialisation de glut et creation de la fenetre */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Alice au pays des Merveilles");

    /* Initialisation d'OpenGL */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(2.0);
    glEnable(GL_DEPTH_TEST);

    //Génère des Id pour les textures
    glGenTextures(1, &firstTexture);
    glGenTextures(1, &secondTexture);
    glGenTextures(1, &thirdTexture);

     /* Chargement de la texture*/
      loadJpegImage("Image/NoirBlanc.jpg", firstTexture);
      loadJpegImage("Image/CarreBlanc.jpg", secondTexture);
      loadJpegImage("Image/CarreNoir.jpg", secondTexture);

      //préparation de la lumière
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    InitLumieres();

    // enregistrement des fonctions de rappel
    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mousemotion);
    glutSpecialFunc(flechesClavier);

    /* Entree dans la boucle principale glut */
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}

//////////////////////////////////////////////////////////////////
//                                                              //
//              G E S T I O N  I N T E R F A C E                //
//                                                              //
//////////////////////////////////////////////////////////////////

///AFFICHAGE
void affichage()
{
  /* effacement de l'image avec la couleur de fond */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glShadeModel(GL_SMOOTH);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glRotatef(angley,1.0,0.0,0.0);
  glRotatef(anglex,0.0,1.0,0.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-40,40,-40,40,40,-40);
  glScalef(zoomFactor,zoomFactor,1);
  glMatrixMode(GL_MODELVIEW);


    //Repère
    //axe x en rouge

   glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(1, 0,0.0);
    glEnd();
    //axe des y en vert
    glBegin(GL_LINES);
    	glColor3f(0.0,1.0,0.0);
    	glVertex3f(0, 0,0.0);
    	glVertex3f(0, 1,0.0);
    glEnd();
    //axe des z en bleu
    glBegin(GL_LINES);
    	glColor3f(0.0,0.0,1.0);
    	glVertex3f(0,0,0.0);
    	glVertex3f(0,0,1.0);
    glEnd();

    corps();
    parapluie();
    montre();
    glPushMatrix();
        glRotatef(200.0f,0.0,1.0,0.0);
        glTranslatef(0.0,9.0,0.0);
        Tete();
    glPopMatrix();


  //On echange les buffers
  glutSwapBuffers();
}

///CLAVIER
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
      break;
    case 'Z':
        zoomFactor += 0.2;
        glutPostRedisplay();
        break;
    case 'z':
        zoomFactor -= 0.2;
        glutPostRedisplay();
        break;
    case 'n':
        if(animParapluieEnCours==false){
            animParapluieEnCours=true;
            animationParapluie(0);
        }
        break;
    }

}

///FLECHES CLAVIER
void flechesClavier(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            angley+=5;
            break;
        case GLUT_KEY_DOWN:
            angley-=5;
            break;
        case GLUT_KEY_LEFT:
            anglex+=5;
            break;
        case GLUT_KEY_RIGHT:
            anglex-=5;
            break;
    }

    glutPostRedisplay();
}

///RESHAPE
void reshape(int x,int y)
{
  if (x<y)
    glViewport(0,(y-x)/2,x,x);
  else
    glViewport((x-y)/2,0,y,y);
}

///MOUSE
void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse=0; /* le booleen presse passe a 0 (faux) */
}

///MOUSEMOTION
void mousemotion(int x,int y)
  {
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
}

///ANIMATION PARAPLUIE
void animationParapluie(int etape){
    if(etape<5||(etape>=10&&etape<15)||(etape>=20&&etape<25)){
        TranslationAnimParapluie-=0.5;
        angleBrasAnimParapluie+=5;
        glutPostRedisplay();
        usleep(30000);
        glutTimerFunc(0,animationParapluie,etape+1);
    }
    else{
        if((etape>=5&&etape<10)||(etape>=15&&etape<20)||(etape>=25&&etape<30)){
            TranslationAnimParapluie+=0.5;
            angleBrasAnimParapluie-=5;
            glutPostRedisplay();
            usleep(50000);
            glutTimerFunc(0,animationParapluie,etape+1);
        }
        else{
        animParapluieEnCours=false;
        }
    }
}

///IDLE
void idle(){
    angleHeures+=5;
    angleMinutes+=9;
    glutPostRedisplay();
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

///LOADJPEGIMAGE
void loadJpegImage(char *fichier, GLuint& textureID){

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

  glBindTexture(GL_TEXTURE_2D, textureID);

  /* Parametrage du placage de textures */
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,
	       GL_RGB,GL_UNSIGNED_BYTE,image);
  glEnable(GL_TEXTURE_2D);
}

//////////////////////////////////////////////////////////////////
//                                                              //
//         F I N  G E S T I O N  I N T E R F A C E              //
//                                                              //
//////////////////////////////////////////////////////////////////






//////////////////////////////////////////////////////////////////
//                                                              //
//            F O R M E S  G E O M E T R I Q U E                //
//                                                              //
//////////////////////////////////////////////////////////////////

class Point{

    ///Cette classe est utilisée dans les fonctions qui applique une texture à un objet

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

///ELLIPSE
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

///DEMI ELLIPSE
void demiEllipse(int paralleles, int meridiens, float a, float b, float c){

    const float du=2*M_PI/meridiens;
    const float dv=M_PI/(paralleles-1);

    FaceDessousDemiEllipse(a,c,paralleles);

    for (int i = 0; i < meridiens/2; i++) {
        glBegin(GL_QUAD_STRIP);
            for (int j = 0; j < paralleles; j++) {
                float u = i * du;
                float v = j * dv +(-M_PI/2);

                float x = a * cos(u) * cos(v);
                float y = b * sin(u) * cos(v);
                float z = c * sin(v);

                float l = sqrt(x * x + y * y + z * z);
                float nx = x / l;
                float ny = y / l;
                float nz = z / l;

                glNormal3f(nx, ny, nz);
                glVertex3f(x, y, z);

                u = (i + 1) * du;
                x = a * cos(u) * cos(v);
                y = b * sin(u) * cos(v);
                z = c * sin(v);

                l = sqrt(x * x + y * y + z * z);
                nx = x / l;
                ny = y / l;
                nz = z / l;

                glNormal3f(nx, ny, nz);
                glVertex3f(x, y, z);

            }
        glEnd();
    }
  }

///FACE DESSOUS DEMI ELLIPSE
void FaceDessousDemiEllipse(float a, float c, int paralleles) {
    glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= paralleles; i++) {
            float theta = 2.0f * M_PI * i / paralleles;
            float x = a * cos(theta);
            float z = c * sin(theta);

            float l = sqrt(x * x + z * z);
            float nx = x / l;
            float nz = z / l;

            glNormal3f(nx, 0, nz);
            glVertex3f(x, 0.0,z);

            glNormal3f(0,-1,0);
            glVertex3f(0,0,0);
        }
    glEnd();
}

///ELLIPSE COUPE
void EllipseCoupe(int paralleles, int meridiens, float a, float b, float c, float divp, float divm){

    ///Cette fonction permet de construire une ellipse divisée en un nombre de part entré en paramètre
    ///Par exemple : avec cette fonction on peut générer un quart d'une ellipse.

    const float du=2*M_PI/meridiens;
    const float dv=M_PI/(paralleles-1);

    for (int i = 0; i < meridiens/divm; i++) {
        glBegin(GL_QUAD_STRIP);
            for (int j = 0; j < paralleles/divp; j++) {
                float u = i * du;
                float v = j * dv +(-M_PI/2);

                float x = a * cos(u) * cos(v);
                float y = b * sin(u) * cos(v);
                float z = c * sin(v);

                float l = sqrt(x * x + y * y + z * z);
                float nx = x / l;
                float ny = y / l;
                float nz = z / l;

                glNormal3f(nx, ny, nz);
                glVertex3f(x, y, z);

                u = (i + 1) * du;
                x = a * cos(u) * cos(v);
                y = b * sin(u) * cos(v);
                z = c * sin(v);

                l = sqrt(x * x + y * y + z * z);
                nx = x / l;
                ny = y / l;
                nz = z / l;

                glNormal3f(nx, ny, nz);
                glVertex3f(x, y, z);

            }
        glEnd();
    }
}

///CONE
void Cone(float a, float c, float h, int paralleles) {

    ///Cette fonction permet de créer un cone

    //Haut du cone
    glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= paralleles; i++) {
            float theta = 2.0f * M_PI * i / paralleles;
            float x = a * cos(theta);
            float z = c * sin(theta);

            float l = sqrt(x * x + z * z);
            float nx = x / l;
            float nz = z / l;

            glNormal3f(nx,0,nz);
            glVertex3f(x, 0.0, z);

            glNormal3f(0,1,0);
            glVertex3f(0, h, 0);
        }
    glEnd();


    //Face de dessous du cone
    glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= paralleles; i++) {
            float theta = 2.0f * M_PI * i / paralleles;
            float x = a * cos(theta);
            float z = c * sin(theta);

            float l = sqrt(x * x + z * z);
            float nx = x / l;
            float nz = z / l;

            glNormal3f(nx, 0, nz);
            glVertex3f(x, 0.0, z);

            glNormal3f(0,-1,0);
            glVertex3f(0, 0, 0);
        }
    glEnd();
}

///CYLINDRE
void Cylindre(int nbDiv, float nbtour, float hauteur, float rayon) {

    ///Cette fonction permet de conctruire un cylindre

        //Contour du cylindre
        glBegin(GL_QUAD_STRIP);
            for (int i = 0; i <= nbtour/nbDiv; i++) {
                float theta = 2.0f * M_PI * i / nbtour;
                float x = rayon * cos(theta);
                float z = rayon * sin(theta);

                 float l = sqrt(x * x + z * z);
                float nx = x / l;
                float nz = z / l;

                glNormal3f(nx,0,nz);
                glVertex3f(x, hauteur,z);

                glNormal3f(nx,0,nz);
                glVertex3f(x,0,z);
            }
        glEnd();

        //Face du dessus du cylindre
        glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i <= nbtour/nbDiv; i++) {
                float theta = 2.0f * M_PI * i / nbtour;
                float x = rayon * cos(theta);
                float z = rayon * sin(theta);

                float l = sqrt(x * x + z * z);
                float nx = x / l;
                float nz = z / l;

                glNormal3f(nx, 0, nz);
                glVertex3f(x, hauteur,z);

                glNormal3f(0,1,0);
                glVertex3f(0,hauteur,0);
            }
        glEnd();

        //Face du dessous du cylindre
        glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i <= nbtour/nbDiv; i++) {
                float theta = 2.0f * M_PI * i / nbtour;
                float x = rayon * cos(theta);
                float z = rayon * sin(theta);

                float l = sqrt(x * x + z * z);
                float nx = x / l;
                float nz = z / l;

                glNormal3f(nx,0,nz);
                glVertex3f(x, 0,z);

                glNormal3f(0,-1,0);
                glVertex3f(0,0,0);
            }
        glEnd();
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

        glBegin(GL_QUAD_STRIP);
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

                rotatedX = NextX+cos(rotationAngle)*cos(theta);
                rotatedZ = NextZ+epaisseur*sin(theta);

                NewX = rotatedX - NextX;
                Newz = rotatedZ - NextZ;

                float NX = NextX + cos(-NextBeta) * NewX - sin(-NextBeta) * Newz;
                float NZ = NextZ + sin(-NextBeta) * NewX + cos(-NextBeta) * Newz;

                float l = sqrt(X * X + rotatedY * rotatedY + Z * Z);
                float nx = X / l;
                float ny = rotatedY / l;
                float nz = Z / l;

                float l1 = sqrt(NX * NX + rotatedY * rotatedY + NZ * NZ);
                float nx1 = NX / l1;
                float ny1 = rotatedY / l1;
                float nz1 = NZ / l1;

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


            float l = sqrt(X * X + rotatedY * rotatedY + Z * Z);
            float nx = X / l;
            float ny = rotatedY / l;
            float nz = Z / l;

            float l1 = sqrt(NX * NX + rotatedY2 * rotatedY2 + NZ * NZ);
            float nx1 = NX / l1;
            float ny1 = rotatedY2 / l1;
            float nz1 = NZ / l1;

            glNormal3f(nx, ny, nz);
            glVertex3f(X, rotatedY, Z);

            glNormal3f(nx1, ny1, nz1);
            glVertex3f(NX, rotatedY2, NZ);
        }
        glEnd();
    }

}

///CONE TRONQUE
void ConeTronque(int nbDiv, float rayon1, float rayon2, float hauteur) {

    /** Cette fonction reprend le principe de la fonction cone et elle lui ajoute
    * la possibilité de définir les rayons des cercles qui compose le cone.
    * A la fin cette fonction génère une forme qui se rapproche plus d'un cylindre dont
    * on a paramétré les rayons de ses bases inférieure et supérieure.
    **/

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= nbDiv; i++) {
        float theta = 2.0f * M_PI * i / nbDiv;
        float x = rayon1 * cos(theta);
        float z = rayon1 * sin(theta);
        float u = rayon2 * cos(theta);
        float v = rayon2 * sin(theta);

        float l = sqrt(x * x + z * z);
        float nx = x / l;
        float nz = z / l;

        glNormal3f(nx,hauteur,nz);
        glVertex3f(x, hauteur,z);

        l = sqrt(u * u + v * v);
        nx = u / l;
        nz = v / l;

        glNormal3f(nx,0,nz);
        glVertex3f(u,0,v);
    }
    glEnd();

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= nbDiv; i++) {
        float theta = 2.0f * M_PI * i / nbDiv;
        float x = rayon1 * cos(theta);
        float z = rayon1 * sin(theta);

        float l = sqrt(x * x + z * z);
        float nx = x / l;
        float nz = z / l;

        glNormal3f(nx,0,nz);
        glVertex3f(x, hauteur,z);

        glNormal3f(0,1,0);
        glVertex3f(0,hauteur,0);
    }
    glEnd();
}

//////////////////////////////////////////////////////////////////
//                                                              //
//         F I N  F O R M E S  G E O M E T R I Q U E            //
//                                                              //
//////////////////////////////////////////////////////////////////







//////////////////////////////////////////////////////////////////
//                                                              //
//         C O M P O S A N T S  D U  L A P I N                  //
//                                                              //
//////////////////////////////////////////////////////////////////

///CORPS
void corps(){

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1.0,0.0,0.0, 1.0};
    GLfloat material_ambient[] = {1.0,0.0,0.0, 1.0};
    GLfloat material_specular[] = {1.0,0.0,0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);


    //veste rouge
    glPushMatrix();
        glColor3f(1.0f,0.0f,0.0f);
        ellipse(50,50,5,6,4.5);
    glPopMatrix();


    //bras droit
    glPushMatrix();
        glColor3f(1.0f,0.0f,0.0f);
        glRotatef(60.0f,0.0f,1.0f,0.0f);
        glTranslatef(-1.5,4,-6.5);
        ellipse(50,50,1.5,1.5,4);
    glPopMatrix();

    //bras gauche
    glPushMatrix();
        glColor3f(1.0f,0.0f,0.0f);
        glRotatef(angleBrasAnimParapluie,0.0,1.0,0.0);
        glTranslatef(3.5,4,-2.5);
        ellipse(50,50,1.5,1.5,4);
    glPopMatrix();

    GLfloat material_diffuse1[] = {0.5,0.5,0.5, 1.0};
    GLfloat material_ambient1[] = {0.5,0.5,0.5, 1.0};
    GLfloat material_specular1[] = {0.5,0.5,0.5, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);

    //pantalon gris
    glPushMatrix();
        glTranslatef(0,-1,0);
        glRotated(90,1,0,0);
        Pantalon();
    glPopMatrix();

    //jambe droite
    glPushMatrix();
        glTranslatef(-2.5,-2,0);
        glRotatef(180.0f,1.0f,0.0f,0.0f);
        Jambe();
    glPopMatrix();

    //jambe gauche
    glPushMatrix();
        glTranslatef(2.5,-2,0);
        glRotatef(180.0f,1.0f,0.0f,0.0f);
        Jambe();
    glPopMatrix();

    GLfloat material_diffuse2[] = {1.0,1.0,1.0, 1.0};
    GLfloat material_ambient2[] = {0.5,0.5,0.5, 1.0};
    GLfloat material_specular2[] = {0.0,0.0,0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular2);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse2);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient2);

    //queue
    glPushMatrix();
        glTranslatef(0,-2,6);
        glColor3f(1.0f,1.0f,1.0f);
        ellipse(50,50,2,2,2);
    glPopMatrix();

    //pied droit
    glPushMatrix();
        glTranslatef(-2.5,-7.5,-1);
        demiEllipse(50,50,2,1,3.5);
    glPopMatrix();

    //pied gauche
    glPushMatrix();
        glTranslatef(2.5,-7.5,-1);
        demiEllipse(50,50,2,1,3.5);
    glPopMatrix();

    //main droite
    glPushMatrix();
        glColor3f(1.0f,1.0f,1.0f);
        glTranslatef(-8.5,4,-4);
        ellipse(50,50,1.75,1.75,1.75);
    glPopMatrix();


    //main gauche
    glPushMatrix();
        glColor3f(1.0f,1.0f,1.0f);
        glTranslatef(0.5+TranslationAnimParapluie,4,-6);
        ellipse(50,50,1.75,1.75,1.75);
    glPopMatrix();
}

///PANTALON
void Pantalon(){

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};  // Couleur diffuse à 1,1,1 pour une texture en noir et blanc
    GLfloat material_ambient[] = {1.0, 1.0, 1.0, 1.0};  // Couleur ambiante à 1,1,1
    GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0}; // Couleur spéculaire à 1,1,1
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glColor3f(1,1,1);

    int nbM = 50;  // Nombre de méridiens
    int nbP = 50; // Nombre de parallèles
    int nbpoint = nbM * nbP;    //Nombre de points
    int nbface = nbM * (nbP-1); //Nombre de faces

    float a = 5.5;
    float b = 4.5;
    float c = 5.5;

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

            float x0 = pEllipse[fEllipse[i][j][0]].x;
            float x1 = pEllipse[fEllipse[i][j][1]].x;
            float x2 = pEllipse[fEllipse[i][j][2]].x;
            float x3 = pEllipse[fEllipse[i][j][3]].x;

            float y0 = pEllipse[fEllipse[i][j][0]].y;
            float y1 = pEllipse[fEllipse[i][j][1]].y;
            float y2 = pEllipse[fEllipse[i][j][2]].y;
            float y3 = pEllipse[fEllipse[i][j][3]].y;

            float z0 = pEllipse[fEllipse[i][j][0]].z;
            float z1 = pEllipse[fEllipse[i][j][1]].z;
            float z2 = pEllipse[fEllipse[i][j][2]].z;
            float z3 = pEllipse[fEllipse[i][j][3]].z;

            float l = sqrt(x0 * x0 + y0 * y0 + z0 * z0);
            float nx0 = x0 / l;
            float ny0 = y0 / l;
            float nz0 = z0 / l;

            l = sqrt(x1 * x1 + y1 * y1 + z1 * z1);
            float nx1 = x1 / l;
            float ny1 = y1 / l;
            float nz1 = z1 / l;

            l = sqrt(x2 * x2 + y2 * y2 + z2 * z2);
            float nx2 = x2 / l;
            float ny2 = y2 / l;
            float nz2 = z2 / l;

            l = sqrt(x3 * x3 + y3 * y3 + z3 * z3);
            float nx3 = x3 / l;
            float ny3 = y3 / l;
            float nz3 = z3 / l;

            glBegin(GL_POLYGON);
                glTexCoord2f(0,0);
                glNormal3f(nx0,ny0,nz0);
                glVertex3f(x0, y0, z0);

                glTexCoord2f(0,1);
                glNormal3f(nx1,ny1,nz1);
                glVertex3f(x1, y1, z1);

                glTexCoord2f(1,1);
                glNormal3f(nx2,ny2,nz2);
                glVertex3f(x2, y2, z2);

                glTexCoord2f(1,0);
                glNormal3f(nx3,ny3,nz3);
                glVertex3f(x3, y3, z3);
            glEnd();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, thirdTexture);
    for(int i=1; i<nbM; i+=2){
        for(int j=0; j<nbP-1; j++){
            float x0 = pEllipse[fEllipse[i][j][0]].x;
            float x1 = pEllipse[fEllipse[i][j][1]].x;
            float x2 = pEllipse[fEllipse[i][j][2]].x;
            float x3 = pEllipse[fEllipse[i][j][3]].x;

            float y0 = pEllipse[fEllipse[i][j][0]].y;
            float y1 = pEllipse[fEllipse[i][j][1]].y;
            float y2 = pEllipse[fEllipse[i][j][2]].y;
            float y3 = pEllipse[fEllipse[i][j][3]].y;

            float z0 = pEllipse[fEllipse[i][j][0]].z;
            float z1 = pEllipse[fEllipse[i][j][1]].z;
            float z2 = pEllipse[fEllipse[i][j][2]].z;
            float z3 = pEllipse[fEllipse[i][j][3]].z;

            float l = sqrt(x0 * x0 + y0 * y0 + z0 * z0);
            float nx0 = x0 / l;
            float ny0 = y0 / l;
            float nz0 = z0 / l;

            l = sqrt(x1 * x1 + y1 * y1 + z1 * z1);
            float nx1 = x1 / l;
            float ny1 = y1 / l;
            float nz1 = z1 / l;

            l = sqrt(x2 * x2 + y2 * y2 + z2 * z2);
            float nx2 = x2 / l;
            float ny2 = y2 / l;
            float nz2 = z2 / l;

            l = sqrt(x3 * x3 + y3 * y3 + z3 * z3);
            float nx3 = x3 / l;
            float ny3 = y3 / l;
            float nz3 = z3 / l;

            glBegin(GL_POLYGON);
                glTexCoord2f(0,0);
                glNormal3f(nx0,ny0,nz0);
                glVertex3f(x0, y0, z0);

                glTexCoord2f(0,1);
                glNormal3f(nx1,ny1,nz1);
                glVertex3f(x1, y1, z1);

                glTexCoord2f(1,1);
                glNormal3f(nx2,ny2,nz2);
                glVertex3f(x2, y2, z2);

                glTexCoord2f(1,0);
                glNormal3f(nx3,ny3,nz3);
                glVertex3f(x3, y3, z3);
            glEnd();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glColor3f(1,1,1);
}


///MONTRE
void montre(){

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1.0,215.0/255.0,0.0, 1.0};
    GLfloat material_ambient[] = {1.0,215.0/255.0,0.0, 1.0};
    GLfloat material_specular[] = {1.0,215.0/255.0,0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    //contour
    glPushMatrix();
        glColor3f(1.0,215.0/255.0,0.0);
        glTranslatef(-8.5,5.5,-9.5);
        glRotatef(90.0f,0.0f,0.0f,1.0f);
        Tore(50,50,5.5,5.5,1,0.6);
    glPopMatrix();

    //face de derrière
    glPushMatrix();
        glColor3f(1.0,215.0/255.0,0.0);
        glTranslatef(-9,5.5,-9.5);
        glRotatef(90.0f,0.0f,0.0f,-1.0f);
        Cylindre(1,50,0.5,5.5);
    glPopMatrix();


    GLfloat material_diffuse1[] = {1.0,1.0,1.0, 1.0};
    GLfloat material_ambient1[] = {1.0,1.0,1.0, 1.0};
    GLfloat material_specular1[] = {1.0,1.0,1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);

    //fond blanc
    glPushMatrix();
        glColor3f(1.0f,1.0f,1.0f);
        glTranslatef(-8.5,5.5,-9.5);
        glRotatef(90.0f,0.0f,0,-1.0f);
        Cylindre(1,50,0.1,4.9);
    glPopMatrix();

    GLfloat material_diffuse2[] = {1.0,140.0/255.0,0.0, 1.0};
    GLfloat material_ambient2[] = {1.0,140.0/255.0,0.0, 1.0};
    GLfloat material_specular2[] = {1.0,140.0/255.0,0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular2);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse2);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient2);

    //bouton
    glPushMatrix();
        glColor3f(1.0,140.0/255.0,0.0);
        glTranslatef(-8.5,11.5,-9.5);
        Cylindre(1,50,0.8,0.5);
    glPopMatrix();

    GLfloat material_diffuse3[] = {0.0,0.0,0.0, 1.0};
    GLfloat material_ambient3[] = {0.0,0.0,0.0, 1.0};
    GLfloat material_specular3[] = {0.0,0.0,0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular3);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse3);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient3);

    //sphère au niveau de la base des aiguilles
    glPushMatrix();
        glColor3f(0.0f,0.0f,0.0f);
        glTranslatef(-8,5.5,-9.5);
        ellipse(50,50,0.6,0.6,0.6);
        glutSolidSphere(0.6,50,50);
    glPopMatrix();

    //aiguille des minutes
    glPushMatrix();
        glTranslatef(-8,5.5,-9.5);
        glRotatef(angleMinutes,1.0,0.0,0.0);
        Cylindre(1,50,4.6,0.2);
    glPopMatrix();

    //aiguille des heures
    glPushMatrix();
        glTranslatef(-8,5.5,-9.5);
        glRotatef(angleHeures,1.0,0.0,0.0);
        Cylindre(1,50,2.3,0.2);
    glPopMatrix();
  }

///PARAPLUIE
void parapluie(){

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {0.0,100.0/255.0,0.0, 1.0};
    GLfloat material_ambient[] = {0.0,100.0/255.0,0.0, 1.0};
    GLfloat material_specular[] = {0.0,100.0/255.0,0.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    //parapluie
    glPushMatrix();
        glColor3f(0.0,100.0/255.0,0.0);
        glTranslatef(4.5+TranslationAnimParapluie,4,-6);
        glRotatef(90.0f,0.0f,0,1);
        Cone(1.2,1.2,10,50);
    glPopMatrix();

    GLfloat material_diffuse1[] = {160.0/255.0,69.0/255.0,19.0/255.0, 1.0};
    GLfloat material_ambient1[] = {160.0/255.0,69.0/255.0,19.0/255.0, 1.0};
    GLfloat material_specular1[] = {160.0/255.0,69.0/255.0,19.0/255.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);


    //sphère au bout du parapluie
    glPushMatrix();
        glColor3f(160.0/255.0,69.0/255.0,19.0/255.0);
        glTranslatef(-5+TranslationAnimParapluie,4,-6);
        ellipse(50,50,0.5,0.5,0.5);
    glPopMatrix();

    //manche
    glPushMatrix();
        glTranslatef(4.5+TranslationAnimParapluie,4,-6);
        glRotatef(-90,0,0,1);
        Cylindre(1,50,3,0.4);
    glPopMatrix();
}

///JAMBE
void Jambe() {

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1.0, 1.0, 1.0, 1.0};  // Couleur diffuse à 1,1,1 pour une texture en noir et blanc
    GLfloat material_ambient[] = {1.0, 1.0, 1.0, 1.0};  // Couleur ambiante à 1,1,1
    GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0}; // Couleur spéculaire à 1,1,1
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);


    ///Cette fonction construit une jambe avec la texture "NoirBlanc.jpg"

    //initialisation de la couleur du rendu
    glColor3f(1,1,1);

    glBindTexture(GL_TEXTURE_2D, firstTexture);
    Point pCylindre [20];
    int fCylindre [10][4];

    //initialisation des faces du cylindre
    for(int i=0; i<10; i++){
        fCylindre[i][0] = i;
        fCylindre[i][1] = i+10;
        fCylindre[i][2] = 10 + ((i+1)%10);
        fCylindre[i][3] = (i+1)%10;
    }

    int nbDiv = 10;
    float rayon = 1.5;
    float hauteur = 5;
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

    for(int i=0; i<10; i++){
    
    	float x0 = pCylindre[fCylindre[i][0]].x;
    	float x1 = pCylindre[fCylindre[i][1]].x;
    	float x2 = pCylindre[fCylindre[i][2]].x;
    	float x3 = pCylindre[fCylindre[i][3]].x;
    	
    	float y0 = pCylindre[fCylindre[i][0]].y;
    	float y1 = pCylindre[fCylindre[i][1]].y;
    	float y2 = pCylindre[fCylindre[i][2]].y;
    	float y3 = pCylindre[fCylindre[i][3]].y;
    	
    	float z0 = pCylindre[fCylindre[i][0]].z;
    	float z1 = pCylindre[fCylindre[i][1]].z;
    	float z2 = pCylindre[fCylindre[i][2]].z;
    	float z3 = pCylindre[fCylindre[i][3]].z;
    	
    	//calcul des normales et ajouts
    	float l = sqrt(x0 * x0 + y0 * y0 + z0 * z0);
    	float nx0 = x0 / l;
    	float ny0 = y0 / l;
   	float nz0 = z0 / l;

    	l = sqrt(x1 * x1 + y1 * y1 + z1 * z1);
    	float nx1 = x1 / l;
    	float ny1 = y1 / l;
    	float nz1 = z1 / l;

    	l = sqrt(x2 * x2 + y2 * y2 + z2 * z2);
    	float nx2 = x2 / l;
    	float ny2 = y2 / l;
    	float nz2 = z2 / l;

    	l = sqrt(x3 * x3 + y3 * y3 + z3 * z3);
    	float nx3 = x3 / l;
    	float ny3 = y3 / l;
    	float nz3 = z3 / l;
    	
        glBegin(GL_POLYGON);
        
	glTexCoord2f((float)(i/10.),0.0);
	glNormal3f(nx0,ny0,nz0);
	glVertex3f(x0,y0,z0);
	
	glTexCoord2f((float)(i/10.),1.0);
	glNormal3f(nx0,ny0,nz0);
	glVertex3f(x1,y1,z1);
	
	glTexCoord2f((float)((i+1)/10.),1.0);
	glNormal3f(nx0,ny0,nz0);
	glVertex3f(x2,y2,z2);
	
	glTexCoord2f((float)((i+1)/10.),0.0);
	glNormal3f(nx0,ny0,nz0);
	glVertex3f(x3,y3,z3);
	
        glEnd();

    }
    glBindTexture(GL_TEXTURE_2D,0);

    //initialisation de la couleur du rendu
    glColor3f(1,1,1);
}

///TETE
void Tete(){

    ///Cette fonction construit la tete du lapin

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1,1,1, 1.0};
    GLfloat material_ambient[] = {1,1,1, 1.0};
    GLfloat material_specular[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    /// Ajout le haut de la tête
    glPushMatrix();
        HautDeTete();
    glPopMatrix();
    ///FIN AJOUT

    ///Ajout du museau qui est composé du nez et de la bouche
    glPushMatrix();
        glTranslated(0, -3.5, 2.7);
        Museau();
    glPopMatrix();
    ///FIN AJOUT

    ///Ajout des yeux
    glPushMatrix();
        glTranslated(0.7, 1.5, 1.5);
        glRotated(30,0,1,0);
        glScaled(0.6,0.6,0.6);
        glRotated(-10,1,0,0);
        glRotated(10,0,0,1);
        Oeil();
    glPopMatrix();

    glPushMatrix();
        glTranslated(-0.7, 1.5, 1.5);
        glRotated(30,0,1,0);
        glScaled(0.6,0.6,0.6);
        glRotated(-10,1,0,0);
        glRotated(-20,0,0,1);
        Oeil();
    glPopMatrix();
     ///FIN AJOUT

    ///Ajout des joues
    glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(0,-1,-0.9);
        glRotated(-30,1,0,0);
        glRotated(90,0,1,0);
        joue();
    glPopMatrix();
     ///FIN AJOUT

    ///Ajout des oreilles
    glPushMatrix();
        glRotated(-15,0,0,1);
        glTranslated(0.5,7,0);
        Oreille();
    glPopMatrix();

    glPushMatrix();
        glRotated(15,0,0,1);
        glTranslated(-0.5,7,0);
        Oreille();
    glPopMatrix();
     ///FIN AJOUT

    ///Ajout des sourcils
    glPushMatrix();
        glColor3f(0.4,0.4,0.4);
        glTranslated(0.4, 3, 1);
        glRotated(-20,1,0,0);
        glRotated(5,0,0,1);
         glRotated(10,0,1,0);
        glScaled(0.40,0.40,0.40);
        Sourcil();
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.4,0.4,0.4);
        glTranslated(-0.4, 3, 1);
        glRotated(180,0,1,0);
        glRotated(20,1,0,0);
        glRotated(-5,0,0,1);
        glRotated(-10,0,1,0);
        glScaled(0.40,0.40,0.40);
        Sourcil();
    glPopMatrix();
     ///FIN AJOUT

    ///Ajout des lunettes
    glPushMatrix();
        glTranslated(0,0.7,2.7);
        glRotated(-90,1,0,0);
        glScaled(0.5,0.5,0.5);
        Lunette();
    glPopMatrix();
     ///FIN AJOUT
}

///HAUT DE TETE
void HautDeTete(){

    ///Cette fonction construit le haut de la tete du lapin

    glPushMatrix();
        glColor3f(1,1,1);
        glRotated(-10,1,0,0);
        ConeTronque(50, 1.4, 2.5, 3);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1,1,1);
        glRotated(-10,1,0,0);
        glRotated(180,0,0,1);
        ConeTronque(50, 1.4, 2.5, 3);
    glPopMatrix();
}

///LUNETTE
void Lunette(){

    ///Cette fonction construit les lunettes du lapin

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {38.0/255.0, 196.0/255.0, 236.0/255.0, 1.0};
    GLfloat material_ambient[] = {38.0/255.0, 196.0/255.0, 236.0/255.0, 1.0};
    GLfloat material_specular[] = {38.0/255.0, 196.0/255.0, 236.0/255.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    ///Moitié de Tore
    glPushMatrix();
        glColor3f(38.0/255.0, 196.0/255.0, 236.0/255.0);
        Tore(20,20,1.8,1,2,0.1);
    glPopMatrix();

    ///Lunette
    glPushMatrix();
        glColor3f(38.0/255.0, 196.0/255.0, 236.0/255.0);
        glTranslated(2.8,0,0);
        Tore(20,20,1,1,1,0.1);
    glPopMatrix();

    ///Lunette
    glPushMatrix();
        glColor3f(38.0/255.0, 196.0/255.0, 236.0/255.0);
        glTranslated(-2.8,0,0);
        Tore(20,20,1,1,1,0.1);
    glPopMatrix();

}

///SOURCIL
void Sourcil(){

    ///Cette fonction construit un sourcil

    glPushMatrix();
        glRotated(45,0,0,1);
        ellipse(50,50,1,0.2,0.2);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0.6,0,0);
        glRotated(25,0,0,1);
        ellipse(50,50,1,0.2,0.2);
    glPopMatrix();

    glPushMatrix();
        glTranslated(1.3,-0.3,0);
        glRotated(-25,0,0,1);
        ellipse(50,50,1,0.2,0.2);
    glPopMatrix();
}

///OREILLE
void Oreille(){

    ///Cette fonction construit une oreille

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1,1,1, 1.0};
    GLfloat material_ambient[] = {1,1,1, 1.0};
    GLfloat material_specular[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    const float meridiens = 50;
    const float paralleles = 50;
    float a = 1;
    float b = 2.5;
    float c = 1;
    float du=2*M_PI/meridiens;
    const float dv=M_PI/(paralleles-1);

    ///Création de la première demi ellipse de couleur blanche
    for (int i = 0; i < meridiens; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j < paralleles/2; j++) {

            float u = i * du;
            float v = j * dv +(-M_PI/2);

            if( i >= meridiens/2){
               b = 5;
            }

            float x = a * cos(u) * cos(v);
            float y = b * sin(u) * cos(v);
            float z = c * sin(v);

            float l = sqrt(x * x + y * y + z * z);
            float nx = x / l;
            float ny = y / l;
            float nz = z / l;

            glColor3f(1,1,1);
            glNormal3f(nx, ny, nz);
            glVertex3f(x, y, z);

            u = (i + 1) * du;
            x = a * cos(u) * cos(v);
            y = b * sin(u) * cos(v);
            z = c * sin(v);

            l = sqrt(x * x + y * y + z * z);
            nx = x / l;
            ny = y / l;
            nz = z / l;

            glColor3f(1,1,1);
            glNormal3f(nx, ny, nz);
            glVertex3f(x, y, z);

        }
        glEnd();
    }

    float a2 = 0.6;
    float b2 = 1.7;
    float c2 = 0.6;
    b=2.5;

    GLfloat material_diffuse1[] = {1,0.5,1, 1.0};
    GLfloat material_ambient1[] = {1,0.5,1, 1.0};
    GLfloat material_specular1[] = {1,0.5,1, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);

    ///Création de la seconde demi ellipse de couleur rose, cette ellipse ce situera à l'intérieure de la première
    for (int i = 0; i < meridiens; i++) {
    glBegin(GL_QUAD_STRIP);
        for (int j = 0; j < paralleles/2; j++) {


            float u = i * du;
            float v = j * dv +(-M_PI/2);

            if( i >= meridiens/2){
               b2 = 3.6;
            }

            float x = a2 * cos(u) * cos(v);
            float y = b2 * sin(u) * cos(v);
            float z = c2 * sin(v);

            float l = sqrt(x * x + y * y + z * z);
            float nx = x / l;
            float ny = y / l;
            float nz = z / l;

            glColor3f(1,0.5,1);
            glNormal3f(-nx, -ny, -nz);
            glVertex3f(x, y, z);

            u = (i + 1) * du;
            x = a2 * cos(u) * cos(v);
            y = b2 * sin(u) * cos(v);
            z = c2 * sin(v);

            l = sqrt(x * x + y * y + z * z);
            nx = x / l;
            ny = y / l;
            nz = z / l;

            glColor3f(1,0.5,1);
            glNormal3f(-nx, -ny, -nz);
            glVertex3f(x, y, z);

        }
        glEnd();
    }

    GLfloat points[4][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}};

    b2 = 3.6;
    b = 5;

    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    /// On referme l'espace présent entre les deux ellipses
    for (int j = 0; j < meridiens; j++) {


        float u = 2.0f * M_PI * j / meridiens;

        if( j >= meridiens/2){
           b2 = 1.7;
            b=2.5;
        }

        float x = a * cos(-u);
        float y = b * sin(-u);

        points[0][0] = x;
        points[0][1] = y;

        x = a2 * cos(-u);
        y = b2 * sin(-u);

        points[1][0] = x;
        points[1][1] = y;

        u = 2.0f * M_PI * (j+1) / meridiens;
        x = a * cos(-u);
        y = b * sin(-u);

        points[2][0] = x;
        points[2][1] = y;

        x = a2 * cos(-u);
        y = b2 * sin(-u);

        points[3][0] = x;
        points[3][1] = y;

        glBegin(GL_QUADS);
            glColor3f(1,1,1);
            glNormal3f(0,1,0);
            glVertex3fv(points[1]);

            glColor3f(1,1,1);
            glNormal3f(0,1,0);
            glVertex3fv(points[3]);

            glColor3f(1,1,1);
            glNormal3f(0,1,0);
            glVertex3fv(points[2]);

            glColor3f(1,1,1);
            glNormal3f(0,1,0);
            glVertex3fv(points[0]);
        glEnd();

    }
}

///MOUSTACHE
void Moustache(){

    ///Cette fonction construit une moustache

    glPushMatrix();
    ToreTronque(40,40,1,1.5,4,0.35);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.5,0,0);
     ToreTronque(40,40,1,3,6,0.5);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.5,0,0);
    glRotated(25,0,1,0);
     ToreTronque(40,40,1,3,7,0.4);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.5,0,0);
    glRotated(40,0,1,0);
     ToreTronque(40,40,1,3,7,0.4);
    glPopMatrix();
}

///JOUE
void joue(){

    ///Cette fonction construit les joues et le bas de la tete du lapin

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1,1,1, 1.0};
    GLfloat material_ambient[] = {1,1,1, 1.0};
    GLfloat material_specular[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
        EllipseCoupe(20,20,1.5,2,4,1,1);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,-2,0);
        glRotated(45,1,0,0);
        EllipseCoupe(20,20,1.5,2,4,1.75,1);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,-2,0.5);
        glRotated(45,0,0,1);
        EllipseCoupe(20,20,2,4,1.5,1,1);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,-2,-0.5);
        glRotated(45,0,0,1);
        EllipseCoupe(20,20,2,4,1.5,1,1);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,-2,0);
        glRotated(180,0,1,0);
        glRotated(45,1,0,0);
        EllipseCoupe(20,20,1.5,2,4,1.75,1);
    glPopMatrix();

    ///Ajout de la Moustache
    glPushMatrix();
        glTranslated(0,1,3.4);
        glScaled(0.65,0.65,0.65);
        glRotated(-90,0,0,1);
        Moustache();
    glPopMatrix();

    glPushMatrix();
        glTranslated(0,1,-3.4);
        glScaled(0.65,0.65,0.65);
        glRotated(90,0,0,1);
        glRotated(180,0,1,0);
        Moustache();
    glPopMatrix();
    ///Fin Ajout
}

///OEIL
void Oeil(){

    ///Cette fonction construit un oeil

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {1,0.5,0.96, 1.0};
    GLfloat material_ambient[] = {1,0.5,0.96, 1.0};
    GLfloat material_specular[] = {1,0.5,0.96, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);


    ///Globe oculaire
    glPushMatrix();
        glColor3f(1,0.5,0.96);
        ellipse(50,50,1,1.8,1);
    glPopMatrix();

    GLfloat material_diffuse1[] = {1,0,1, 1.0};
    GLfloat material_ambient1[] = {1,0,1, 1.0};
    GLfloat material_specular1[] = {1,0,1, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);

    ///Iris
    glPushMatrix();
        glColor3f(1,0,1);
        glScalef(0.5, 0.5, 0.5);
        glTranslated(0,0,1.4);
        ellipse(50,50,1,1.5,1);
    glPopMatrix();

}

///BOUCHE
void Bouche(){

    ///Cette fonction construit la bouche et le menton

    ///Bouche
    glPushMatrix();
        glColor3f(1,0.5,1);
        Cone(1,1,2,100);
        glRotatef(180.0f, 1, 0, 0.0f);
        EllipseCoupe(50, 50, 1,1,1,1,2);
    glPopMatrix();

    ///Menton
    glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(0,0,-2.5);
        glRotated(90,1,0,0);
        Cylindre(2,50,3,1.2);
    glPopMatrix();

}

///MUSEAU
void Museau(){

    ///Cette fonction construit le museau

    GLfloat shininess = 50.0;
    GLfloat material_diffuse[] = {0.95,0.95,0.95, 1.0};
    GLfloat material_ambient[] = {0.95,0.95,0.95, 1.0};
    GLfloat material_specular[] = {0.95,0.95,0.95, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glPushMatrix();
        glColor3f(0.95,0.95,0.95);
        glTranslated(-0.5,2,0);
        ellipse(20,20, 0.7,2.1,0.7);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.95,0.95,0.95);
        glTranslated(-0.5,3.2,0);
        ellipse(20,20,0.7,1,0.7);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.95,0.95,0.95);
        glTranslated(0.5,2,0);
        ellipse(20,20,0.7,2.1,0.7);
    glPopMatrix();

    glPushMatrix();
        glColor3f(0.95,0.95,0.95);
        glTranslated(0.5,3.2,0);
        ellipse(20,20,0.7,1,0.7);
    glPopMatrix();


    ///Nez
    glPushMatrix();
        glScalef(0.5, 0.5, 0.5);
        glTranslated(0,7.8,0.8);
        Nez();
    glPopMatrix();

    ///Bouche
    glPushMatrix();
        glScalef(0.5, 0.5, 0.5);
        //glRotated(180,0,1,0);
        glTranslated(0,0,-0.5);
        Bouche();
    glPopMatrix();

    GLfloat material_diffuse1[] = {1,1,1, 1.0};
    GLfloat material_ambient1[] = {1,1,1, 1.0};
    GLfloat material_specular1[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular1);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse1);
    glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient1);
    /// Grande partie du museau
    glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(-0.5,2,-1.5);
        ellipse(20,20,1.5,2.1,2);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(-0.5,3.2,-1.5);
        ellipse(20,20,1.5,1,2);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(0.5,2,-1.5);
        ellipse(20,20,1.5,2.1,2);
    glPopMatrix();

    glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(0.5,3.2,-1.5);
        ellipse(20,20,1.5,1,2);
    glPopMatrix();
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
              glNormal3f(0,0,1);
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

//////////////////////////////////////////////////////////////////
//                                                              //
//        F I N  C O M P O S A N T S  D U  L A P I N            //
//                                                              //
//////////////////////////////////////////////////////////////////
