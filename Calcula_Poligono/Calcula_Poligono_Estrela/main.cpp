#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define PI 3.14159265358979323846

static unsigned int n;
static double **pts = NULL; // Vetor contendo os pontos

static int maxX =  10;
static int maxY =  10;
static int maxZ =  10;
static int minX = -10;
static int minY = -10;
static int minZ = -10;

void drawAxis(void) {
    // Incrementos de 1 unidade
    glColor3d(.5,.5,.5);
    glBegin(GL_LINES);
        // Paralelos ao eixo x
        for(int i = minY + 1; i < maxY; i++) {
            glVertex3d(minX, i, 0);
            glVertex3d(maxX, i, 0);
        }

        // Paralelos ao eixo y
        for(int i = minX + 1; i < maxX; i++) {
            glVertex3d(i, minY, 0);
            glVertex3d(i, maxY, 0);
        }
    glEnd();

    // Eixo das abscissas e das ordenadas
    glColor3d(1,1,1);
    glBegin(GL_LINES);
        glVertex3d(minX, 0, 0);
        glVertex3d(maxX, 0, 0);
        glVertex3d(0, minY, 0);
        glVertex3d(0, maxY, 0);
    glEnd();

    // Coordenadas
    for(double i = minX; i <= maxX; i += 2) {
        void *font;
        char cd[4];
        double dec = i - (int) i;

        if(dec == 0) {
            font = GLUT_BITMAP_HELVETICA_18;
            sprintf(cd, "%1.0f", i);
        }

        // X
        glRasterPos3d(i, 0, 0);
        glutBitmapString(font, (unsigned char *) cd);

        // Y
        glRasterPos3d(0, i, 0);
        glutBitmapString(font, (unsigned char *) cd);
    }
}

static void resize(int width, int height) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(minX, maxX, minY, maxY, minZ, maxZ);
}

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    drawAxis();

    // Descomentar caso queira visualizar o poligono regular
    /*glColor3d(1,0,0);
    glBegin(GL_LINE_LOOP); // Desenha o poligono
        for(int i = 1; i < n + 1; i++) {
            glVertex3dv(pts[i]);
        }
    glEnd();*/

    glColor3d(1,1,0);
    glBegin(GL_LINES); // Desenha pares de vertices amarelos
        for(int i = 1; i < n - 1; i++) {
            glVertex3dv(pts[i]);
            for(int j = 0; j < n - 3; j++) {
                int index = (i + 2 + j) % n;
                if (index == 0) index = n;

                glVertex3dv(pts[index]);

                if(j != n - 4) glVertex3dv(pts[i]); // Não é necessário criar o vertice ao fim do loop
            }
        }
    glEnd();

    glFlush();
}


static void key(unsigned char key, int x, int y) {
    switch (key) {
        case 27 :
        case 'q':
            exit(0);
            break;
    }

    glutPostRedisplay();
}

static void idle(void) {
    glutPostRedisplay();
}

// Algoritmo apresentado em aula
double ** plotRegularPolygon(unsigned int sideNum, unsigned int sideLength, double *centerPoint, double* startPoint, double **pts) {
    double theta, ctheta, stheta, sa, ca, r, aux;
    double x0 = centerPoint[0],
           y0 = centerPoint[1],
           z0 = centerPoint[2],
           xi = startPoint [0],
           yi = startPoint [1],
           zi = startPoint [2];

    pts = (double **) malloc(sizeof(double *) * (n + 1));

    if(pts == NULL) {
        printf("Erro de alocacao de memoria\n");
        exit(-1);
    }

    for(int i = 0; i < n + 1; i++) {
        pts[i] = (double *) malloc(sizeof(double) * 3);

        if(pts[i] == NULL) {
            printf("Erro de alocacao de memoria\n");
            exit(-1);
        }
    }

    theta = 2 * PI/sideNum;
    ctheta = cos(theta);
    stheta = sin(theta);
    r = sideLength / (2 * sin(PI/sideNum));
    sa = sin((yi - y0) / r);
    ca = sin((xi - x0) / r);

    pts[0][0] = xi;
    pts[0][1] = yi;
    pts[0][2] = zi;

    for(int i = 1; i < n + 1; i++) {
        pts[i][0] = x0 + r * ca;
        pts[i][1] = y0 + r * sa;
        pts[i][2] = z0;

        aux = sa * ctheta + ca * stheta;
        ca = ca * ctheta - sa * stheta;
        sa = aux;

        printf("Pontos %d: (%lf, %lf, %1.0lf)\n", i, pts[i][0], pts[i][1], pts[i][2]);
    }

    return pts;
}

int main(int argc, char *argv[]) {
    unsigned int l; // Número de lados

    double center[3]; // Vetor contendo as coordenadas do centro do polígono
    double start[3]; // Vetor contendo as coordenadas iniciais do polígono

    do {
        printf("Digite o numero de lados......: ");
        scanf("%u", &n);
    } while (n < 5);

    printf("Digite o comprimento dos lados: ");
    scanf("%u", &l);

    printf("Digite o ponto central (X Y Z): ");
    scanf("%lf %lf %lf", &center[0], &center[1], &center[2]);

    printf("Digite o ponto inicial (X Y Z): ");
    scanf("%lf %lf %lf", &start[0], &start[1], &start[2]);

    pts = plotRegularPolygon(n, l, center, start, pts);

    int totalWidth   = GetSystemMetrics(SM_CXSCREEN);
    int totalHeight  = GetSystemMetrics(SM_CYSCREEN);
    int windowWidth  = totalWidth / 3;
    int windowHeight = windowWidth;
    int windowPosX   = (totalWidth  - windowWidth )/2;
    int windowPosY   = (totalHeight - windowHeight)/2;
    char windowName[40];

    sprintf(windowName, "Poligono Regular | %d lados", n);

    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutCreateWindow(windowName);

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(0,0,0,0);

    glutMainLoop();

    return 0;
}
