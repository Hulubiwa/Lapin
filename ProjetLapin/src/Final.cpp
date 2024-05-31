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

//pour les textures
unsigned char image[256*256*3];
GLuint firstTexture;
GLuint secondTexture;
GLuint thirdTexture;

//pour la souris
char presse;

//pour la caméra
int anglex,angley,x,y,xold,yold;
float zoom=1.0;

//pour les animations
float TranslationAnimParapluie=0.0;
float angleMinutes=1.0;
float angleHeures=1.0;
bool animParapluieEnCours=false;
float angleBrasAnimParapluie=30.0;


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
void Cylindre(int nbFaces, float hauteur, float rayon);
void Tore(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur);
void ToreTronque(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur);
void ConeTronque(int nbDiv, float rayon1, float rayon2, float hauteur);
void Cone(float rayon, float h, int paralleles);

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

    /* Chargement de la texture */
    loadJpegImage("Image/NoirBlanc.jpg", firstTexture);
    loadJpegImage("Image/CarreBlanc.jpg", secondTexture);
    loadJpegImage("Image/CarreNoir.jpg", secondTexture);


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
  glScalef(zoom,zoom,1);
  glMatrixMode(GL_MODELVIEW);

   corps();
    parapluie();
    montre();
    glPushMatrix();
        glRotatef(200.0f,0.0,1.0,0.0);
        glTranslatef(0.0,9.0,0.0);
        Tete();
    glPopMatrix();


    InitLumieres();
    glutPostRedisplay();

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
        zoom += 0.2;
        glutPostRedisplay();
        break;
    case 'z':
        zoom -= 0.2;
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

    //activation de l'éclairage
    glEnable(GL_LIGHTING);

    //LUMIÈRE 0
    GLfloat position0[] = {0, 5, -35}; // position
    glLightfv(GL_LIGHT0,GL_POSITION,position0);
    GLfloat amb0[]={0.6,0.6,0.6,1.0}; //lumière ambiante
    glLightfv(GL_LIGHT0,GL_AMBIENT,amb0);
    GLfloat dif0[]={1.0,1.0,1.0,1.0}; //lumière diffuse
    glLightfv(GL_LIGHT0,GL_DIFFUSE,dif0);
    GLfloat spe0[]={1,1,1,1};
    glLightfv(GL_LIGHT0,GL_SPECULAR,spe0);
    glEnable(GL_LIGHT0); //activation de cette lumière


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
void ellipse(int paralleles, int meridiens, float a, float b, float c){

    const float du=2*M_PI/meridiens;
    const float dv=M_PI/(paralleles-1);

     for (int i = 0; i < meridiens; i++) {
            for (int j = 0; j < paralleles; j++) {

                glBegin(GL_POLYGON);

                //calcul des angles
                float u = i * du;
                float v = j * dv +(-M_PI/2);

                //calcul des coordonnées
                float x = a * cos(u) * cos(v);
                float y = b * sin(u) * cos(v);
                float z = c * sin(v);

                //idem pour les autres points de la face pour pouvoir la tracer
                float u1 = (i + 1) * du;
                float x1 = a * cos(u1) * cos(v);
                float y1 = b * sin(u1) * cos(v);
                float z1 = c * sin(v);

                float v1 = (j + 1) * dv +(-M_PI/2);
                float x2 = a * cos(u1) * cos(v1);
                float y2 = b * sin(u1) * cos(v1);
                float z2 = c * sin(v1);

                float x3 = a * cos(u) * cos(v1);
                float y3 = b * sin(u) * cos(v1);
                float z3 = c * sin(v1);

                //Calcul des vecteurs pour le calcul de la normale
                float vx=x-x3;
                float vy=y-y3;
                float vz=z-z3;

                float ux=x2-x3;
                float uy=y2-y3;
                float uz=z2-z3;

                //calcul de la normale
                float nx=uz*vy-uy*vz;
                float ny=ux*vz-uz*vx;
                float nz=uy*vx-ux*vy;
                float l = sqrt(nx * nx + ny * ny + nz * nz);


                //ajout des points avec la normale
                glNormal3f(nx/l,ny/l,nz/l);
                glVertex3f(x, y, z);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glVertex3f(x3, y3, z3);

                glEnd();

            }
        }
  }

///DEMI ELLIPSE
void demiEllipse(int paralleles, int meridiens, float a, float b, float c){

    const float du=2*M_PI/meridiens;
    const float dv=M_PI/(paralleles-1);

    FaceDessousDemiEllipse(a,c,paralleles);

    for (int i = 0; i < meridiens/2; i++) {
            for (int j = 0; j < paralleles; j++) {

                glBegin(GL_POLYGON);

                //calcul des angles
                float u = i * du;
                float v = j * dv +(-M_PI/2);

                //calcul des coordonnées
                float x = a * cos(u) * cos(v);
                float y = b * sin(u) * cos(v);
                float z = c * sin(v);

                //idem pour les autres points de la face pour pouvoir la tracer
                float u1 = (i + 1) * du;
                float x1 = a * cos(u1) * cos(v);
                float y1 = b * sin(u1) * cos(v);
                float z1 = c * sin(v);

                float v1 = (j + 1) * dv +(-M_PI/2);
                float x2 = a * cos(u1) * cos(v1);
                float y2 = b * sin(u1) * cos(v1);
                float z2 = c * sin(v1);

                float x3 = a * cos(u) * cos(v1);
                float y3 = b * sin(u) * cos(v1);
                float z3 = c * sin(v1);

                //Calcul des vecteurs pour le calcul de la normale
                float vx=x-x3;
                float vy=y-y3;
                float vz=z-z3;

                float ux=x2-x3;
                float uy=y2-y3;
                float uz=z2-z3;

                //calcul de la normale
                float nx=uz*vy-uy*vz;
                float ny=ux*vz-uz*vx;
                float nz=uy*vx-ux*vy;
                float l = sqrt(nx * nx + ny * ny + nz * nz);


                //ajout des points avec la normale
                glNormal3f(nx/l,ny/l,nz/l);
                glVertex3f(x, y, z);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glVertex3f(x3, y3, z3);

                glEnd();

            }
    }
  }

///FACE DESSOUS DEMI ELLIPSE
void FaceDessousDemiEllipse(float a, float c, int paralleles) {
    glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= paralleles; i++){

            //calcul de l'angle et des coordonnées
            float theta = 2.0f * M_PI * i / paralleles;
            float x = a * cos(theta);
            float z = c * sin(theta);

            //ajout du premier point et de sa normale
            glNormal3f(0,-1,0);
            glVertex3f(x,0.0,z);

            //ajout du deuxième point et de sa normale
            glVertex3f(0,0,0);
        }
    glEnd();
}

///ELLIPSE COUPE
void EllipseCoupe(int paralleles, int meridiens, float a, float b, float c, float divp, float divm){

    //fonction pour récupérer une partie d'une ellipe

    const float du=2*M_PI/meridiens;
    const float dv=M_PI/(paralleles-1);

    for (int i = 0; i < meridiens/divm; i++) {
            for (int j = 0; j < paralleles/divp; j++) {

                 glBegin(GL_POLYGON);

                //calcul des angles
                float u = i * du;
                float v = j * dv +(-M_PI/2);

                //calcul des coordonnées
                float x = a * cos(u) * cos(v);
                float y = b * sin(u) * cos(v);
                float z = c * sin(v);

                //idem pour les autres points de la face pour pouvoir la tracer
                float u1 = (i + 1) * du;
                float x1 = a * cos(u1) * cos(v);
                float y1 = b * sin(u1) * cos(v);
                float z1 = c * sin(v);

                float v1 = (j + 1) * dv +(-M_PI/2);
                float x2 = a * cos(u1) * cos(v1);
                float y2 = b * sin(u1) * cos(v1);
                float z2 = c * sin(v1);

                float x3 = a * cos(u) * cos(v1);
                float y3 = b * sin(u) * cos(v1);
                float z3 = c * sin(v1);

                //Calcul des vecteurs pour le calcul de la normale
                float vx=x-x3;
                float vy=y-y3;
                float vz=z-z3;

                float ux=x2-x3;
                float uy=y2-y3;
                float uz=z2-z3;

                //calcul de la normale
                float nx=uz*vy-uy*vz;
                float ny=ux*vz-uz*vx;
                float nz=uy*vx-ux*vy;
                float l = sqrt(nx * nx + ny * ny + nz * nz);


                //ajout des points avec la normale
                glNormal3f(nx/l,ny/l,nz/l);
                glVertex3f(x, y, z);
                glVertex3f(x1, y1, z1);
                glVertex3f(x2, y2, z2);
                glVertex3f(x3, y3, z3);

                glEnd();

            }
    }
}

///CONE
void Cone(float rayon, float h, int paralleles) {

    //contour
    glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= paralleles; i++) {

            //calcul de l'angle et des coordonnées
            float theta = 2.0f * M_PI * i / paralleles;
            float x = rayon * cos(theta);
            float z = rayon * sin(theta);

            //on a besoin du point suivant pour calculer la normale
            float theta2 = 2.0f * M_PI * (i + 1) / paralleles;
            float x1 = rayon * cos(theta2);
            float z1 = rayon * sin(theta2);

                //Calcul des vecteurs pour le calcul de la normale
                float vx=-x;
                float vy=h;
                float vz=-z;

                float ux=x1-x;
                float uy=0;
                float uz=z1-z;

                //calcul de la normale
                float nx=uz*vy-uy*vz;
                float ny=ux*vz-uz*vx;
                float nz=uy*vx-ux*vy;
                float l = sqrt(nx * nx + ny * ny + nz * nz);


                //ajout des points avec la normale
                glNormal3f(nx/l,ny/l,nz/l);
                glVertex3f(x, 0.0, z);
                glVertex3f(0, h, 0);
        }
    glEnd();

    //face du dessous
    glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= paralleles; i++) {

            float theta = 2.0f * M_PI * i / paralleles;
            float x = rayon * cos(theta);
            float z = rayon * sin(theta);

            glNormal3f(0,-1,0);
            glVertex3f(x, 0.0, z);
            glVertex3f(0, 0, 0);
        }
    glEnd();
}

///CONE TRONQUE
void ConeTronque(int nbDiv, float rayon1, float rayon2, float hauteur) {

    //cette fonction permet de faire un cône sans l'extrémité pointue


    //contour
    for (int i = 0; i <= nbDiv; i++) {

        glBegin(GL_POLYGON);

        //calcul de l'angle et des coordonnées
        float theta = 2.0f * M_PI * i / nbDiv;
        float x = rayon1 * cos(theta);
        float z = rayon1 * sin(theta);

        //on fait pareil pour les autres points de la face
        float x1 = rayon2 * cos(theta);
        float z1 = rayon2 * sin(theta);

        float theta2 = 2.0f * M_PI * (i+1) / nbDiv;
        float x2 = rayon1 * cos(theta2);
        float z2 = rayon1 * sin(theta2);

        float x3 = rayon2 * cos(theta2);
        float z3 = rayon2 * sin(theta2);

        //Calcul des vecteurs pour le calcul de la normale
        float vx=x-x1;
        float vy=hauteur;
        float vz=z-z1;

        float ux=x3-x1;
        float uy=0;
        float uz=z3-z1;

        //calcul de la normale
        float nx=uz*vy-uy*vz;
        float ny=ux*vz-uz*vx;
        float nz=uy*vx-ux*vy;
        float l = sqrt(nx * nx + ny * ny + nz * nz);


        //ajout des points avec la normale
        glNormal3f(nx/l,ny/l,nz/l);
        glVertex3f(x1,0,z1);
        glVertex3f(x, hauteur,z);
        glVertex3f(x2, hauteur,z2);
        glVertex3f(x3,0,z3);

        glEnd();
    }

    //face du dessus
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= nbDiv; i++){


        float theta = 2.0f * M_PI * i / nbDiv;
        float x = rayon1 * cos(theta);
        float z = rayon1 * sin(theta);

        glNormal3f(0,1,0);
        glVertex3f(x, hauteur,z);
        glVertex3f(0,hauteur,0);
    }
    glEnd();

    //face du dessous
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= nbDiv; i++) {
        float theta = 2.0f * M_PI * i / nbDiv;
        float x = rayon2 * cos(theta);
        float z = rayon2 * sin(theta);

        glNormal3f(0,-1,0);
        glVertex3f(x, 0,z);
        glVertex3f(0,0,0);
    }
    glEnd();

}

///CYLINDRE
void Cylindre(int nbFaces, float hauteur, float rayon) {

	//contour
            for (int i = 0; i <= nbFaces; i++) {

                glBegin(GL_POLYGON);

                //calcul des coordonnées et de l'angle
                float theta = 2.0f * M_PI * i / nbFaces;
                float x = rayon * cos(theta);
                float z = rayon * sin(theta);

                //idem pour les autre points de la face
                float theta2 = 2.0f * M_PI * (i+1) / nbFaces;
                float x1 = rayon * cos(theta2);
                float z1 = rayon * sin(theta2);

                //Calcul des vecteurs pour le calcul de la normale
                float vx=0;
                float vy=hauteur;
                float vz=0;

                float ux=x1-x;
                float uy=0;
                float uz=z1-z;

                //calcul de la normale
                float nx=uz*vy-uy*vz;
                float ny=ux*vz-uz*vx;
                float nz=uy*vx-ux*vy;
                float l = sqrt(nx * nx + ny * ny + nz * nz);


                //ajout des points avec la normale
                glNormal3f(nx/l,ny/l,nz/l);
                glVertex3f(x, hauteur,z);
                glVertex3f(x,0,z);
                glVertex3f(x1,0,z1);
                glVertex3f(x1, hauteur,z1);

                glEnd();
            }

	//face du haut
        glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i <= nbFaces; i++) {

                float theta = 2.0f * M_PI * i / nbFaces;
                float x = rayon * cos(theta);
                float z = rayon * sin(theta);

                glNormal3f(0,1,0);
                glVertex3f(x, hauteur,z);
                glVertex3f(0,hauteur,0);
            }
        glEnd();

	//face du dessous
        glBegin(GL_TRIANGLE_STRIP);
            for (int i = 0; i <= nbFaces; i++) {

                float theta = 2.0f * M_PI * i / nbFaces;
                float x = rayon * cos(theta);
                float z = rayon * sin(theta);

                glNormal3f(0,-1,0);
                glVertex3f(x, 0,z);
                glVertex3f(0,0,0);
            }
        glEnd();
}

///TORE
void Tore(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur){

    ///Cette fonction permet de construire un tore

    for (int i = 0; i < nbParallele/division; i++) {
        /// Parcours du cercle du tore

        /**
        *   Notre Tore est composé d'un enchainement de cylindre
        *   Le principe de saconstruction est de prendre les bases de chaque
        *   cylindres et de les construire un à un.
        **/

        ///Centre de la Première base
        float Theta = 2.0f * M_PI * i / nbParallele;
        float Centrex =  a*cos(Theta);
        float Centrez =  c*sin(Theta);

        ///Centre de la Seconde base
        float NextTheta = 2.0f * M_PI * (i+1) / nbParallele;
        float CentreNextX = a*cos(NextTheta);
        float CentreNextZ = c*sin(NextTheta);


            for (int j = 0; j <= nbRotationMeridien; j++) {

                glBegin(GL_POLYGON);
                float theta = 2.0f * M_PI * j / nbRotationMeridien;
                float theta2 = 2.0f * M_PI * ( j+1 ) / nbRotationMeridien;
                float rotationAngle = 90* M_PI / 180.0f; /// Angle de rotation en radians (90 degrés)
                float beta = (M_PI/2) - Theta;
                float NextBeta = (M_PI/2) - NextTheta;

                // On place le point sur la première base du cylindre
                // Puis on effectue une rotation de 90° de la base
                float PositionX = Centrex+cos(rotationAngle)*cos(theta);
                float PositionY = epaisseur*sin(rotationAngle)*cos(theta);
                float PositionZ = Centrez+epaisseur*sin(theta);

                //Calcul des coordonnées du vecteur reliant le Centre du Cercle et la position du point
                float NewX = PositionX - Centrex;
                float Newz = PositionZ - Centrez;

                //On effectue une rotation de la base afin qu'elle soit aligné à l'axe qui relie le centre du tore et le centre de la base
                float x0 = Centrex + cos(-beta) * NewX - sin(-beta) * Newz;
                float z0 = Centrez + sin(-beta) * NewX + cos(-beta) * Newz;

                PositionX = CentreNextX+cos(rotationAngle)*cos(theta);
                PositionZ = CentreNextZ+epaisseur*sin(theta);

                //Calcul des coordonnées du vecteur reliant le Centre du Cercle et la position du point
                NewX = PositionX - CentreNextX;
                Newz = PositionZ - CentreNextZ;

                //On effectue une rotation de la base afin qu'elle soit aligné à l'axe qui relie le centre du tore et le centre de la base
                float x1 = CentreNextX + cos(-NextBeta) * NewX - sin(-NextBeta) * Newz;
                float z1 = CentreNextZ + sin(-NextBeta) * NewX + cos(-NextBeta) * Newz;

                /**
                *   On applique le même principe avec cette fois (j+1) pour avoir les deux autres point de la face
                **/

                // On place le point sur la première base du cylindre
                // Puis on effectue une rotation de 90° de la base
                PositionX = Centrex+cos(rotationAngle)*cos(theta2);
                float PositionY2 = epaisseur*sin(rotationAngle)*cos(theta2);
                PositionZ = Centrez+epaisseur*sin(theta2);

                //Calcul des coordonnées du vecteur reliant le Centre du Cercle et la position du point
                NewX = PositionX - Centrex;
                Newz = PositionZ - Centrez;

                //On effectue une rotation de la base afin qu'elle soit aligné à l'axe qui relie le centre du tore et le centre de la base
                float x2 = Centrex + cos(-beta) * NewX - sin(-beta) * Newz;
                float z2 = Centrez + sin(-beta) * NewX + cos(-beta) * Newz;

                PositionX = CentreNextX+cos(rotationAngle)*cos(theta2);
                PositionZ = CentreNextZ+epaisseur*sin(theta2);

                //Calcul des coordonnées du vecteur reliant le Centre du Cercle et la position du point
                NewX = PositionX - CentreNextX;
                Newz = PositionZ - CentreNextZ;

                //On effectue une rotation de la base afin qu'elle soit aligné à l'axe qui relie le centre du tore et le centre de la base
                float x3 = CentreNextX + cos(-NextBeta) * NewX - sin(-NextBeta) * Newz;
                float z3 = CentreNextZ + sin(-NextBeta) * NewX + cos(-NextBeta) * Newz;

                //Calcul des vecteurs pour le calcul de la normale
                float vx=x0-x3;
                float vy=PositionY-PositionY2;
                float vz=z0-z3;

                float ux=x2-x3;
                float uy=PositionY2-PositionY2;
                float uz=z2-z3;

                //calcul de la normale
                float nx=uy*vz-uz*vy;
                float ny=uz*vx-ux*vz;
                float nz=ux*vy-uy*vx;
                float l = sqrt(nx * nx + ny * ny + nz * nz);


                //Ajout des vecteurs normales
                glNormal3f(nx/l, ny/l, nz/l);
                glVertex3f(x0, PositionY, z0);
                glVertex3f(x1, PositionY, z1);
                glVertex3f(x3, PositionY2, z3);
                glVertex3f(x2, PositionY2, z2);

                glEnd();

            }

    }
}

///TORE TRONQUE
void ToreTronque(float nbRotationMeridien, float nbParallele, float a, float c, float division, float epaisseur){

    /**Cette fonction nous permet de construire un tore tronqué. Cette fonction fonction nous sert dans notre projet à
    * créer des poils. Lors de la création du tore, on réduit au fur et à mesure son épaisseur. A la fin de cette réduction
    * l'épaisseur doit valoir 0 à fin de la construction du tore.
    **/

    for (int i = 0; i < nbParallele/division; i++) {

        float Theta = 2.0f * M_PI * i / nbParallele;
        float x =  a*cos(Theta);
        float z =  c*sin(Theta);
        float NextTheta = 2.0f * M_PI * (i+1) / nbParallele;
        float NextX = a*cos(NextTheta);
        float NextZ = c*sin(NextTheta);


        for (int j = 0; j <= nbRotationMeridien; j++) {
            glBegin(GL_POLYGON);

            float theta = 2.0f * M_PI * j / nbRotationMeridien;
            float theta2 = 2.0f * M_PI * (j+1) / nbRotationMeridien;
            float rotationAngle = 90* M_PI / 180.0f; /// Angle de rotation en radians (90 degrés)
            float beta = M_PI - (M_PI/2) - Theta;

            float NextBeta = M_PI - (M_PI/2) - NextTheta;
            float ep = (epaisseur -((epaisseur /(nbParallele/division))*i));
            float Nextep = (epaisseur -((epaisseur /(nbParallele/division))*(i+1)));

            /// Appliquer la rotation autour de l'axe z
            float PositionX = x+cos(rotationAngle)*cos(theta);
            float PositionY = ep*sin(rotationAngle)*cos(theta);
            float PositionZ = z+ep*sin(theta);

            float NewX = PositionX - x;
            float Newz = PositionZ - z;

            float x0 = x + cos(-beta) * NewX - sin(-beta) * Newz;
            float z0 = z + sin(-beta) * NewX + cos(-beta) * Newz;

            PositionX = NextX+cos(rotationAngle)*cos(theta);
            float PositionY1 = Nextep*sin(rotationAngle)*cos(theta);
            PositionZ = NextZ+Nextep*sin(theta);

            NewX = PositionX - NextX;
            Newz = PositionZ - NextZ;

            float x1 = NextX + cos(-NextBeta) * NewX - sin(-NextBeta) * Newz;
            float z1 = NextZ + sin(-NextBeta) * NewX + cos(-NextBeta) * Newz;

            /// Appliquer la rotation autour de l'axe z
            PositionX = x+cos(rotationAngle)*cos(theta2);
            float PositionY2 = ep*sin(rotationAngle)*cos(theta2);
            PositionZ = z+ep*sin(theta2);

            NewX = PositionX - x;
            Newz = PositionZ - z;

            float x2 = x + cos(-beta) * NewX - sin(-beta) * Newz;
            float z2 = z + sin(-beta) * NewX + cos(-beta) * Newz;

            PositionX = NextX+cos(rotationAngle)*cos(theta2);
            float PositionY3 = Nextep*sin(rotationAngle)*cos(theta2);
            PositionZ = NextZ+Nextep*sin(theta2);

            NewX = PositionX - NextX;
            Newz = PositionZ - NextZ;

            float x3 = NextX + cos(-NextBeta) * NewX - sin(-NextBeta) * Newz;
            float z3 = NextZ + sin(-NextBeta) * NewX + cos(-NextBeta) * Newz;


            //Calcul des vecteurs pour le calcul de la normale
            float vx=x0-x3;
            float vy=PositionY-PositionY3;
            float vz=z0-z3;

            float ux=x2-x3;
            float uy=PositionY2-PositionY3;
            float uz=z2-z3;

            //calcul de la normale
            float nx=uy*vz-uz*vy;
            float ny=uz*vx-ux*vz;
            float nz=ux*vy-uy*vx;
            float l = sqrt(nx * nx + ny * ny + nz * nz);


            //Ajout des vecteurs normales
            glNormal3f(nx/l, ny/l, nz/l);
            glVertex3f(x0, PositionY, z0);
            glVertex3f(x1, PositionY1, z1);
            glVertex3f(x3, PositionY3, z3);
            glVertex3f(x2, PositionY2, z2);

            glEnd();

        }
    }

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
    GLfloat d[] = {1.0,0.0,0.0, 1.0};
    GLfloat a[] = {1.0,0.0,0.0, 1.0};
    GLfloat s[] = {1.0,0.0,0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

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

    GLfloat d1[] = {0.5,0.5,0.5, 1.0};
    GLfloat a1[] = {0.5,0.5,0.5, 1.0};
    GLfloat s1[] = {0.5,0.5,0.5, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a1);

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

    GLfloat d2[] = {1.0,1.0,1.0, 1.0};
    GLfloat a2[] = {0.5,0.5,0.5, 1.0};
    GLfloat s2[] = {0.0,0.0,0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a2);

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

            glBegin(GL_POLYGON);

            //premier point
            float x=pEllipse[fEllipse[i][j][0]].x;
            float y=pEllipse[fEllipse[i][j][0]].y;
            float z=pEllipse[fEllipse[i][j][0]].z;


            //deuxième point
            float x1=pEllipse[fEllipse[i][j][1]].x;
            float y1=pEllipse[fEllipse[i][j][1]].y;
            float z1=pEllipse[fEllipse[i][j][1]].z;

            //troisième point
            float x2=pEllipse[fEllipse[i][j][2]].x;
            float y2=pEllipse[fEllipse[i][j][2]].y;
            float z2=pEllipse[fEllipse[i][j][2]].z;


            //quatrième point
            float x3=pEllipse[fEllipse[i][j][3]].x;
            float y3=pEllipse[fEllipse[i][j][3]].y;
            float z3=pEllipse[fEllipse[i][j][3]].z;

            //Calcul des vecteurs pour le calcul de la normale
            float vx=x-x3;
            float vy=y-y3;
            float vz=z-z3;

            float ux=x2-x3;
            float uy=y2-y3;
            float uz=z2-z3;

            //calcul de la normale
            float nx=uz*vy-uy*vz;
            float ny=ux*vz-uz*vx;
            float nz=uy*vx-ux*vy;


            //ajout des points avec la normale
            float l = sqrt(nx * nx + ny * ny + nz * nz);
            glNormal3f(nx/l,ny/l,nz/l);


            glTexCoord2f(0,0);
            glVertex3f(x, y, z);

            glTexCoord2f(0,1);
            glVertex3f(x1, y1, z1);

            glTexCoord2f(1,1);
            glVertex3f(x2, y2, z2);

            glTexCoord2f(1,0);
            glVertex3f(x3, y3, z3);

            glEnd();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindTexture(GL_TEXTURE_2D, thirdTexture);
    for(int i=1; i<nbM; i+=2){
        for(int j=0; j<nbP-1; j++){

           glBegin(GL_POLYGON);

            //premier point
            float x=pEllipse[fEllipse[i][j][0]].x;
            float y=pEllipse[fEllipse[i][j][0]].y;
            float z=pEllipse[fEllipse[i][j][0]].z;


            //deuxième point
            float x1=pEllipse[fEllipse[i][j][1]].x;
            float y1=pEllipse[fEllipse[i][j][1]].y;
            float z1=pEllipse[fEllipse[i][j][1]].z;

            //troisième point
            float x2=pEllipse[fEllipse[i][j][2]].x;
            float y2=pEllipse[fEllipse[i][j][2]].y;
            float z2=pEllipse[fEllipse[i][j][2]].z;


            //quatrième point
            float x3=pEllipse[fEllipse[i][j][3]].x;
            float y3=pEllipse[fEllipse[i][j][3]].y;
            float z3=pEllipse[fEllipse[i][j][3]].z;

            //Calcul des vecteurs pour le calcul de la normale
            float vx=x-x3;
            float vy=y-y3;
            float vz=z-z3;

            float ux=x2-x3;
            float uy=y2-y3;
            float uz=z2-z3;

            //calcul de la normale
            float nx=uz*vy-uy*vz;
            float ny=ux*vz-uz*vx;
            float nz=uy*vx-ux*vy;


            //ajout des points avec la normale
            float l = sqrt(nx * nx + ny * ny + nz * nz);
            glNormal3f(nx/l,ny/l,nz/l);


            glTexCoord2f(0,0);
            glVertex3f(x, y, z);

            glTexCoord2f(0,1);
            glVertex3f(x1, y1, z1);

            glTexCoord2f(1,1);
            glVertex3f(x2, y2, z2);

            glTexCoord2f(1,0);
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
    GLfloat d[] = {1.0,215.0/255.0,0.0, 1.0};
    GLfloat a[] = {1.0,215.0/255.0,0.0, 1.0};
    GLfloat s[] = {1.0,215.0/255.0,0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

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
        Cylindre(50,0.5,5.5);
    glPopMatrix();


    GLfloat d1[] = {1.0,1.0,1.0, 1.0};
    GLfloat a1[] = {1.0,1.0,1.0, 1.0};
    GLfloat s1[] = {1.0,1.0,1.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a1);

    //fond blanc
    glPushMatrix();
        glColor3f(1.0f,1.0f,1.0f);
        glTranslatef(-8.5,5.5,-9.5);
        glRotatef(90.0f,0.0f,0,-1.0f);
        Cylindre(50,0.1,4.9);
    glPopMatrix();

    GLfloat d2[] = {1.0,140.0/255.0,0.0, 1.0};
    GLfloat a2[] = {1.0,140.0/255.0,0.0, 1.0};
    GLfloat s2[] = {1.0,140.0/255.0,0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d2);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a2);

    //bouton
    glPushMatrix();
        glColor3f(1.0,140.0/255.0,0.0);
        glTranslatef(-8.5,11.5,-9.5);
        Cylindre(50,0.8,0.5);
    glPopMatrix();

    GLfloat d3[] = {0.0,0.0,0.0, 1.0};
    GLfloat a3[] = {0.0,0.0,0.0, 1.0};
    GLfloat s3[] = {0.0,0.0,0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s3);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d3);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a3);

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
        Cylindre(50,4.6,0.2);
    glPopMatrix();

    //aiguille des heures
    glPushMatrix();
        glTranslatef(-8,5.5,-9.5);
        glRotatef(angleHeures,1.0,0.0,0.0);
        Cylindre(50,2.3,0.2);
    glPopMatrix();
  }

///PARAPLUIE
void parapluie(){

    GLfloat shininess = 50.0;
    GLfloat d[] = {0.0,100.0/255.0,0.0, 1.0};
    GLfloat a[] = {0.0,100.0/255.0,0.0, 1.0};
    GLfloat s[] = {0.0,100.0/255.0,0.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    //parapluie
    glPushMatrix();
        glColor3f(0.0,100.0/255.0,0.0);
        glTranslatef(4.5+TranslationAnimParapluie,4,-6);
        glRotatef(90.0f,0.0f,0,1);
        Cone(1.2,10,50);
    glPopMatrix();

    GLfloat d1[] = {160.0/255.0,69.0/255.0,19.0/255.0, 1.0};
    GLfloat a1[] = {160.0/255.0,69.0/255.0,19.0/255.0, 1.0};
    GLfloat s1[] = {160.0/255.0,69.0/255.0,19.0/255.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a1);


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
        Cylindre(50,3,0.4);
    glPopMatrix();
}

///JAMBE
void Jambe() {

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

        glBegin(GL_POLYGON);

        //premier point
        float x=pCylindre[fCylindre[i][0]].x;
        float y=pCylindre[fCylindre[i][0]].y;
        float z=pCylindre[fCylindre[i][0]].z;
        float l=sqrt(x*x+y*y+z*z);
        float nx=x/l;
        float ny=y/l;
        float nz=z/l;
        glTexCoord2f((float)(i/10.),0.0);
        glNormal3f(nx, ny, nz);
        glVertex3f(x, y, z);

        //deuxième point
        float x1=pCylindre[fCylindre[i][1]].x;
        float y1=pCylindre[fCylindre[i][1]].y;
        float z1=pCylindre[fCylindre[i][1]].z;
        float l1=sqrt(x1*x1+y1*y1+z1*z1);
        float nx1=x1/l1;
        float ny1=y1/l1;
        float nz1=z1/l1;
        glTexCoord2f((float)(i/10.),1.0);
        glNormal3f(nx1, ny1, nz1);
        glVertex3f(x1, y1, z1);

        //troisième point
        float x2=pCylindre[fCylindre[i][2]].x;
        float y2=pCylindre[fCylindre[i][2]].y;
        float z2=pCylindre[fCylindre[i][2]].z;
        float l2=sqrt(x2*x2+y2*y2+z2*z2);
        float nx2=x2/l2;
        float ny2=y2/l2;
        float nz2=z2/l2;
        glTexCoord2f((float)((i+1)/10.),1.0);
        glNormal3f(nx2, ny2, nz2);
        glVertex3f(x2, y2, z2);

        //quatrième point
        float x3=pCylindre[fCylindre[i][3]].x;
        float y3=pCylindre[fCylindre[i][3]].y;
        float z3=pCylindre[fCylindre[i][3]].z;
        float l3=sqrt(x3*x3+y3*y3+z3*z3);
        float nx3=x3/l3;
        float ny3=y3/l3;
        float nz3=z3/l3;
        glTexCoord2f((float)((i+1)/10.),0.0);
        glNormal3f(nx3, ny3, nz3);
        glVertex3f(x3, y3, z3);

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
    GLfloat d[] = {1,1,1, 1.0};
    GLfloat a[] = {1,1,1, 1.0};
    GLfloat s[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

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
    GLfloat d[] = {38.0/255.0, 196.0/255.0, 236.0/255.0, 1.0};
    GLfloat a[] = {38.0/255.0, 196.0/255.0, 236.0/255.0, 1.0};
    GLfloat s[] = {38.0/255.0, 196.0/255.0, 236.0/255.0, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

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
    GLfloat d[] = {1,1,1, 1.0};
    GLfloat am[] = {1,1,1, 1.0};
    GLfloat s[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, am);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    const float meridiens = 50;
    const float paralleles = 50;
    float a = 1;
    float b = 2.5;
    float c = 1;
    float du=2*M_PI/meridiens;
    const float dv=M_PI/(paralleles-1);

    ///Création de la première demi ellipse de couleur blanche
    for (int i = 0; i < meridiens; i++) {
        for (int j = 0; j < paralleles/2; j++) {

            glBegin(GL_POLYGON);

            if( i >= meridiens/2){
               b = 5;
            }

            //calcul de l'angle
            float u = i * du;
            float v = j * dv +(-M_PI/2);

            //calcul des coordonnées
            float x = a * cos(u) * cos(v);
            float y = b * sin(u) * cos(v);
            float z = c * sin(v);

            //idem pour les autres points de la face pour pouvoir la tracer
            float u1 = (i + 1) * du;
            float x1 = a * cos(u1) * cos(v);
            float y1 = b * sin(u1) * cos(v);
            float z1 = c * sin(v);

            float v1 = (j + 1) * dv +(-M_PI/2);
            float x2 = a * cos(u1) * cos(v1);
            float y2 = b * sin(u1) * cos(v1);
            float z2 = c * sin(v1);

            float x3 = a * cos(u) * cos(v1);
            float y3 = b * sin(u) * cos(v1);
            float z3 = c * sin(v1);

            //Calcul des vecteurs pour le calcul de la normale
            //Vecteur V
            float vx=x-x3;
            float vy=y-y3;
            float vz=z-z3;

            //Vecteur U
            float ux=x2-x3;
            float uy=y2-y3;
            float uz=z2-z3;

            //calcul de la normale V^U (Produit Vectoriel)
            float nx=uz*vy-uy*vz;
            float ny=ux*vz-uz*vx;
            float nz=uy*vx-ux*vy;
            float l = sqrt(nx * nx + ny * ny + nz * nz);


            //ajout des points avec la normale
            glColor3f(1,1,1);
            glNormal3f(nx/l,ny/l,nz/l);
            glVertex3f(x, y, z);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);

            glEnd();

        }
    }

    float a2 = 0.6;
    float b2 = 1.7;
    float c2 = 0.6;
    b=2.5;

    GLfloat d1[] = {1,0.5,1, 1.0};
    GLfloat a1[] = {1,0.5,1, 1.0};
    GLfloat s1[] = {1,0.5,1, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a1);

    ///Création de la seconde demi ellipse de couleur rose, cette ellipse ce situera à l'intérieure de la première
    for (int i = 0; i < meridiens; i++) {

        for (int j = 0; j < paralleles/2; j++) {

            glBegin(GL_POLYGON);

            if( i >= meridiens/2){
               b2 = 3.6;
            }

            //calcul de l'angle
            float u = i * du;
            float v = j * dv +(-M_PI/2);

            //calcul des coordonnées
            float x = a2 * cos(u) * cos(v);
            float y = b2 * sin(u) * cos(v);
            float z = c2 * sin(v);

            //idem pour les autres points de la face pour pouvoir la tracer
            float u1 = (i + 1) * du;
            float x1 = a2 * cos(u1) * cos(v);
            float y1 = b2 * sin(u1) * cos(v);
            float z1 = c2 * sin(v);

            float v1 = (j + 1) * dv +(-M_PI/2);
            float x2 = a2 * cos(u1) * cos(v1);
            float y2 = b2 * sin(u1) * cos(v1);
            float z2 = c2 * sin(v1);

            float x3 = a2 * cos(u) * cos(v1);
            float y3 = b2 * sin(u) * cos(v1);
            float z3 = c2 * sin(v1);

            //Calcul des vecteurs pour le calcul de la normale
            //Vecteur V
            float vx=x-x3;
            float vy=y-y3;
            float vz=z-z3;

            //Vecteur U
            float ux=x2-x3;
            float uy=y2-y3;
            float uz=z2-z3;

            //calcul de la normale U^V (Produit Vectoriel)
            float nx=uy*vz-uz*vy;
            float ny=uz*vx-ux*vz;
            float nz=ux*vy-uy*vx;
            float l = sqrt(nx * nx + ny * ny + nz * nz);


            //ajout des points avec la normale
            glColor3f(1,0.5,1);
            glNormal3f(nx/l,ny/l,nz/l);
            glVertex3f(x, y, z);
            glVertex3f(x1, y1, z1);
            glVertex3f(x2, y2, z2);
            glVertex3f(x3, y3, z3);

            glEnd();

        }
    }

    GLfloat points[4][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}};

    b2 = 3.6;
    b = 5;

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, am);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

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
            glNormal3f(0,0,1);
            glVertex3fv(points[1]);
            glVertex3fv(points[3]);
            glVertex3fv(points[2]);
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
    GLfloat d[] = {1,1,1, 1.0};
    GLfloat a[] = {1,1,1, 1.0};
    GLfloat s[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

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
    GLfloat d[] = {1,0.5,0.96, 1.0};
    GLfloat a[] = {1,0.5,0.96, 1.0};
    GLfloat s[] = {1,0.5,0.96, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);


    ///Globe oculaire
    glPushMatrix();
        glColor3f(1,0.5,0.96);
        ellipse(50,50,1,1.8,1);
    glPopMatrix();

    GLfloat d1[] = {1,0,1, 1.0};
    GLfloat a1[] = {1,0,1, 1.0};
    GLfloat s1[] = {1,0,1, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a1);

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
        Cone(1,2,100);
        glRotatef(180.0f, 1, 0, 0.0f);
        EllipseCoupe(50, 50, 1,1,1,1,2);
    glPopMatrix();

    ///Menton
    glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(0,0,-2.5);
        glRotated(90,1,0,0);
        Cylindre(50,3,1.2);
    glPopMatrix();

}

///MUSEAU
void Museau(){

    ///Cette fonction construit le museau

    GLfloat shininess = 50.0;
    GLfloat d[] = {0.95,0.95,0.95, 1.0};
    GLfloat a[] = {0.95,0.95,0.95, 1.0};
    GLfloat s[] = {0.95,0.95,0.95, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

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

    GLfloat d1[] = {1,1,1, 1.0};
    GLfloat a1[] = {1,1,1, 1.0};
    GLfloat s1[] = {1,1,1, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, s1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, d1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, a1);
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
    GLfloat d[] = {158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0, 1.0};
    GLfloat a[] = {158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0, 1.0};
    GLfloat s[] = {158.0 / 255.0, 14.0 / 255.0, 64.0 / 255.0, 1.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, s);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
    glMaterialfv(GL_FRONT, GL_AMBIENT, a);
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

//////////////////////////////////////////////////////////////////
//                                                              //
//        F I N  C O M P O S A N T S  D U  L A P I N            //
//                                                              //
//////////////////////////////////////////////////////////////////
