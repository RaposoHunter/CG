#include <GL/glut.h>
#include <GL/freeglut.h>
#include <math.h>

#if __WIN32 == 1
    #defined HAS_WINDOWS 1
    #include <windows.h>
#else
    #define HAS_WINDOWS 0
#endif

#include <stdlib.h>
#include <stdio.h>
#include <opencv2/core.hpp>

using namespace cv;

#define PI 3.14159265358979323846

// Limites do plano cartesiano
static int maxX =  10;
static int maxY =  10;
static int maxZ =  10;
static int minX = -10;
static int minY = -10;
static int minZ = -10;

// Variáveis que controlam fatores estéticos
static int draw =   0;
static int axis =   1;
static char *cmd = (char *) malloc(sizeof(char) * 10);

// Estrutura responsável por auxiliar na criação de pontos. Usado em CircunferenciaBresenham()
typedef struct ponto {
    int x;
    int y;
    struct ponto *nxtPt = NULL;
} Ponto;

Ponto * CriarPonto(int x, int y) {
    Ponto *pt = (Ponto *) malloc(sizeof(Ponto));
    pt->x = x;
    pt->y = y;
    pt->nxtPt = NULL;

    return pt;
}

// Renderiza uma string de caracteres na tela na posição (x,y,0) usando a fonte escolhida.
void showText(double x, double y, void* font, const char* str) {
    glRasterPos3d(x, y, 0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char *) str);
}

// Páginas 7 - 9 do PDF
void LinhaDeclive(int x1, int y1, int x2, int y2) {
    double m = (double)(y2 - y1)/(x2 - x1);
    double b = y1 - m * x1;
    double y;

    glBegin(GL_LINE_STRIP);
        glVertex2d(x1, y1); // Vertice inicial

        for(int x = x1 + 1; x <= x2; x++) {
            y = m * x + b;
            glVertex2d(x, round(y));
        }
    glEnd();

    glColor3d(1,1,1);
    showText(minX + .25, minY + .25, GLUT_BITMAP_HELVETICA_18, "Método do Declive");
}

// Páginas 10 - 13 do PDF
void LinhaDDA(int x1, int y1, int x2, int y2) {
    double y;
    double m = (double)(y2 - y1)/(x2 - x1);

    glBegin(GL_LINE_STRIP);
        glVertex2d(x1, y1); // Vértice inicial
        y = y1;

        for(int x = x1 + 1; x <= x2; x++) {
            y += m;
            glVertex2d(x, round(y));
        }
    glEnd();

    glColor3d(1,1,1);
    showText(minX + .25, minY + .25, GLUT_BITMAP_HELVETICA_18, "Simple DDA");
}

// Páginas 14 - 22
void LinhaBresenham(int x1, int y1, int x2, int y2) {
    int a = y2 - y1,
        b = x1 - x2,
        v = 2*a + b,
        x = x1,
        y = y1;

    glBegin(GL_LINE_STRIP); // Representação por Bresenham
        while(x <= x2) {
            glVertex2d(x, y);
            x++;
            if(v <= 0) {
                v += 2 * a;
            } else {
                v += 2 * (a + b);
                y++;
            }

        }
    glEnd();

    glColor3d(1,1,1);
    showText(minX + .25, minY + .25, GLUT_BITMAP_HELVETICA_18, "Algoritmo de Bresenham");
}

// Páginas 25 - 26 do PDF
void CircunferenciaPolinomial(int a, int b, int r) {
    double y;

    glBegin(GL_LINE_STRIP);
        for(int x = a - r; x <= a + r ; x++) {
            y = round(sqrt(r * r - (x - a) * (x - a)) + b);
            glVertex2d(x, y);
        }

        for(int x = a + r; x >= a - r ; x--) {
            y = round(-sqrt(r * r - (x - a) * (x - a)) + b);
            glVertex2d(x, y);
        }
    glEnd();

    glColor3d(1,1,1);
    showText(minX + .25, minY + .25, GLUT_BITMAP_HELVETICA_18, "Circunferencia Polinomial");
}

// Páginas 27 - 28 do PDF
void CircunferenciaTrigonometrica(int a, int b, int r) {
    double x, y;

    glBegin(GL_LINE_LOOP);
        for(double theta = 0; theta <= 360 ; theta += 1) {
            x = round(r * cos(theta * PI / 180)) + a;
            y = round(r * sin(theta * PI / 180)) + b;

            glVertex2d(x, y);
        }
    glEnd();

    glColor3d(1,1,1);
    showText(minX + .25, minY + .25, GLUT_BITMAP_HELVETICA_18, "Circunferencia Trigonometrica");
}

// Páginas 29 - 35 do PDF
void CircunferenciaBresenham(int a, int b, int r) {
    int d = 3 - 2*r; // Fator usado para aferir a posição do proximo ponto
    int y = r;
    int **pts;  // Ponteiro para guardar todos os pontos criados
    Ponto *pt = NULL, *cabeca = NULL;

    int x, counter = 0;
    int isSymm = 0;

    for(counter = 0, x = 0; x <= y; x+= 0) {
        if(d < 0)  {
            d += 4*x + 6;
        } else {
            d += 4*(x - y) + 10;
            y--;
        }

        x++;

        // Verifica se o ponto (x,y) ainda não criado é simetrico a um ponto (y,x) já criado
        if(counter > 0) {
            Ponto *tmp = cabeca;

            while(tmp != NULL) {
                if(x == tmp->y && y == tmp->x) {
                    isSymm = 1;
                    break;
                }

                tmp = tmp->nxtPt;
            }

            // Caso seja simétrico não é mais necessário adicionar pontos pois,
            // de acordo com o algoritmo de Bresenham, as simetrias serão lidadas depois
            if(isSymm) break;
        }

        // Cria um novo ponto que aponta para a cabeça (head) da
        // lista e, logo em seguida o torna a cabeça da lista
        pt = CriarPonto(x, y);
        pt->nxtPt = cabeca;
        cabeca = pt;

        // Incrementa o número de pontos já adicionados
        counter++;
    }

    // Inicia o ponteiro responsável por guardar todos os pontos da circunferencia
    // OBS.: Como são oito ponto de simetria para cada ponto criado, serão adicionados 8 * counter pontos
    pts = (int **) malloc(sizeof(int *) * counter * 8);

    // Inicia todos os ponteiros que guardarão as coordenadas dos pontos
    for(int i = 0; i < counter * 8; i++) {
        pts[i] = (int *) malloc(sizeof(int) * 2);
    }

    // Na lista encadeada simples é impossível correr por ela a partir do primeiro membro da lista.
    // Portanto, é necessário um for decremental que atribua cada ponto ao seu respectivo índice
    Ponto *tmp = cabeca;
    for(int i = counter - 1; i >= 0; i--) {
        pts[i][0] = tmp->x;
        pts[i][1] = tmp->y;

        tmp = tmp->nxtPt;
    }

    // Realiza metade das simetrias a 4 eixos apresentada na página 24 do PDF
    for(int i = 0; i < counter; i++) {
        int compensacao = 2 * counter;

        pts[compensacao - 1 - i][0] = pts[i][1];
        pts[compensacao - 1 - i][1] = pts[i][0];

        pts[compensacao + i][0] =  pts[i][1];
        pts[compensacao + i][1] = -pts[i][0];

        pts[2 * compensacao - 1 - i][0] =  pts[i][0];
        pts[2 * compensacao - 1 - i][1] = -pts[i][1];
    }

    // A outra metade dos pontos é decidida através da simetria no eixo y de maneira decremental
    for(int index = 4 * counter, i = 4 * counter - 1; i >= 0; i--) {
        pts[index][0] = -pts[i][0];
        pts[index][1] =  pts[i][1];

        index++;
    }

    // Renderiza todos os pontos na tela unindo-os de ponta-a-ponta
    glColor3d(1,0,0);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < counter * 8; i++) {
            int px = pts[i][0], py = pts[i][1];

            glVertex2d(px, py);
        }
    glEnd();

    // Desaloca os ponteiros usados para evitar o uso desnecessário de blocos de memória
    free(pt);
    free(cabeca);
    free(pts);

    glColor3d(1,1,1);
    showText(minX + .25, minY + .25, GLUT_BITMAP_HELVETICA_18, "Circunferencia Bresenham");
}

void drawIdealModels(int x1, int y1, int x2, int y2, int a, int b, int r) {
    if(draw == 0 || draw == 1 || draw == 2) {
        glBegin(GL_LINES);
            glVertex2d(x1, y1);
            glVertex2d(x2, y2);
        glEnd();
    } else {
        double x, y;

        glBegin(GL_LINE_LOOP);
            for(double theta = 0; theta <= PI * 2 ; theta += PI / 360) {
                x = r * cos(theta) + a;
                y = r * sin(theta) + b ;

                glVertex2d(x, y);
            }
        glEnd();
    }
}

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

static void resize(int width, int height){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(minX, maxX, minY, maxY, minZ, maxZ);
}

static void display(void) {
    const int time = glutGet(GLUT_ELAPSED_TIME) / 1000;
    glClear(GL_COLOR_BUFFER_BIT);
    if(axis) drawAxis();

    int x1 = -3, // x inicial das retas modelo
        x2 =  5, // x final das retas modelo
        y1 = -3, // y inicial das retas modelo
        y2 =  3, // y final das retas modelo
        a  =  0, // x do centro das circunferencias modelo
        b  =  0, // y do centro das circunferencias modelo
        r  =  5; // raio das circunferencias modelo

    glColor3d(1,0,0);

    if(draw == 0) LinhaDeclive(x1, y1, x2, y2);               // Representação Matricial de uma reta
    else if(draw == 1) LinhaDDA(x1, y1, x2, y2);                // Representação de uma reta por Simple DDA
    else if(draw == 2) LinhaBresenham(x1, y1, x2, y2);          // Representação de uma reta por Bresenham
    else if(draw == 3) CircunferenciaPolinomial(a, b, r);       // Representação de uma circunferencia pelo método polinomial
    else if(draw == 4) CircunferenciaTrigonometrica(a, b, r);   // Representação de uma circunferencia pelo método trigonométrico
    else CircunferenciaBresenham(a, b, r);                      // Representação de uma circunferencia pelo método de Bresenham

    if(time % 2 == 0) cmd[0] = 's';
    else cmd[0] = 'n';

    if(cmd[0] != 'n') {
        // Modelos Ideais
        glColor3d(1,1,0);
        drawIdealModels(x1, y1, x2, y2, a, b, r);
    }

    glFlush();
}


static void key(unsigned char key, int x, int y) {
    switch (key) {
        case 'q':
            exit(0);
            break;

        // Seleciona entre as 6 representações do programa
        case '0':
            draw = 0;
            break;
        case '1':
            draw = 1;
            break;
        case '2':
            draw = 2;
            break;
        case '3':
            draw = 3;
            break;
        case '4':
            draw = 4;
            break;
        case '5':
            draw = 5;
            break;

        // Liga ou Desliga a representação do plano cartesiano
        case 'a':
            if(axis) axis = 0;
            else axis = 1;
            break;
    }

    glutPostRedisplay();
}

static void idle(void) {
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    if(argc > 1) cmd = argv[1];

    int largura = 600,
        altura = 600;

    char windowName[25];

    glutInit(&argc, argv);
    glutInitWindowSize(largura, altura);
    
    if(HAS_WINDOWS) glutInitWindowPosition((GetSystemMetrics(SM_CXSCREEN) - largura)/2 , (GetSystemMetrics(SM_CYSCREEN) - altura)/2); // Inicia a tela centralizada
    else glutInitWindowPosition(10, 10);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    sprintf(windowName, "Scan Sweeping - %d:%d", maxX - minX, maxY - minY);

    glutCreateWindow(windowName);

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    glClearColor(0,0,0,0);

    glutMainLoop();

    return EXIT_SUCCESS;
}

/*if(i >= 0 && i < counter)               glColor3d(0.5,0.5,0.5);
            if(i >= counter && i < counter * 2)     glColor3d(0,0,1);
            if(i >= counter * 2 && i < counter * 3) glColor3d(0,1,0);
            if(i >= counter * 3 && i < counter * 4) glColor3d(0,1,1);
            if(i >= counter * 4 && i < counter * 5) glColor3d(1,0,0);
            if(i >= counter * 5 && i < counter * 6) glColor3d(1,0,1);
            if(i >= counter * 6 && i < counter * 7) glColor3d(1,1,0);
            if(i >= counter * 7 && i < counter * 8) glColor3d(1,1,1);*/
