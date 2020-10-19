#include <iostream>
#include <cstdlib> // Responsável pelas funções de alocação de memória malloc()
#include <iomanip>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

int maior; // Guarda o valor do maior componente RGB. Também será usada pelo callback para mudar a origem do gr�fico, bem como os valores dos eixos x,y

void callback(int event, int x, int y, int flags, void *userdata) {
    system("CLS"); // Limpa o console a cada nova execução do callback

    Mat img = *(static_cast<Mat *>(userdata));
    Vec3b bgr = img.at<Vec3b>(y, x);

    // Modifica os valores iniciais de x e y para se adequar a um plano cartesiano
    x /= img.cols/256;                          // Eixo X = [0, 255]
    y = (y - img.rows) * -1 * maior/img.rows;   // Eixo Y = [0, maior]

    // Mostra a posição atual (x,y) e se existem valores RGB no ponto. Caso existam, diz quantos e de qual componente.
    if(bgr[2] > 0 && bgr[1] == 0 && bgr[0] == 0) cout << "x,y: (" << setw(3) << x << ", " << setw(4) << y << ") => Existem [" << setw(4) << y << "] pixels com valor [" << setw(3) << x << "] no componente vermelho";
    else if(bgr[1] > 0 && bgr[2] == 0 && bgr[0] == 0) cout << "x,y: (" << setw(3) << x << ", " << setw(4) << y << ") => Existem [" << setw(4) << y << "] pixels com valor [" << setw(3) << x << "] no componente verde";
    else if(bgr[0] > 0 && bgr[2] == 0 && bgr[1] == 0) cout << "x,y: (" << setw(3) << x << ", " << setw(4) << y << ") => Existem [" << setw(4) << y << "] pixels com valor [" << setw(3) << x << "] no componente azul";
    else cout << "x,y: (" << setw(3) << x << ", " << setw(4) << y << ")";
}

int ** calcularHistograma(Mat img) {
    // Cria um histograma que quantifica as ocorrências de um valor específico de cada componente RGB

    int ** histograma = (int **) malloc(sizeof(int *) * 3);

    for(int i = 0; i < 3; i++) {
        histograma[i] = (int *) malloc(sizeof(int) * 256);
        for(int j = 0; j < 256; j++) {
            histograma[i][j] = 0; // Inicializa a matriz com todos os valores zerados;
        }
    }

    for(int r = 0; r < img.rows; r++) {
        for(int c = 0; c < img.cols; c++) {
            Vec3b p = img.at<Vec3b>(r, c); // p é um array de 3 índices que possui os valores BGR do pixel na posição (c, r) da imagem;

            histograma[0][(int) p[2]]++; // Incrementa a quantidade de valores vermelhos p[2]
            histograma[1][(int) p[1]]++; // Incrementa a quantidade de valores verdes p[1]
            histograma[2][(int) p[0]]++; // Incrementa a quantidade de valores azuis p[0]
        }
    }

    return histograma;
}

Mat plotarHistograma( int ** histograma, Mat hGraph, bool adaptarEscala = true) {
    // A função pode retornar dois gráficos com escalas diferentes

    int altura = hGraph.rows,
        largura = hGraph.cols;

    if(adaptarEscala) {
        maior = 0;

        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 256; j++) {
                if(histograma[i][j] > maior) maior = histograma[i][j]; // Decide qual o maior valor em Y
            }
        }
    } else {
        maior = altura; // Fará com que a escala vertical não seja alterada
    }

    int modificador_largura = cvRound((double) largura/256); // Permite que o gráfico ocupe todo o eixo horizontal independente do tamanho da janela
    double modificador_altura = (double) altura/maior;       // Aumenta, ou não, a escala verical do gráfico, permitindo ver todos os seu valores em y;

    for(int i = 1; i < 256; i++) {
        double  x1 = modificador_largura * (i-1),                          // x inicial
                x2 = modificador_largura * i,                              // x final
                r_y1 = altura - histograma[0][i-1] * modificador_altura,   // y inicial do componente R
                r_y2 = altura - histograma[0][i] * modificador_altura,     // y final do componente R
                g_y1 = altura - histograma[1][i-1] * modificador_altura,   // y inicial do componente G
                g_y2 = altura - histograma[1][i] * modificador_altura,     // y final do componente G
                b_y1 = altura - histograma[2][i-1] * modificador_altura,   // y inicial do componente B
                b_y2 = altura - histograma[2][i] * modificador_altura;     // y final do componente B

        // Cria um ponto inicial e um ponto final que serão ligados por uma linha, cada um com sua respectiva cor
        Point redPoint1(x1, r_y1), redPoint2(x2, r_y2);
        Point greenPoint1(x1, g_y1), greenPoint2(x2, g_y2);
        Point bluePoint1(x1, b_y1), bluePoint2(x2, b_y2);

        // Liga os pontos criados acima com suas respectivas cores
        line(hGraph, redPoint1, redPoint2, Scalar(255, 0, 0), 2);
        line(hGraph, greenPoint1, greenPoint2, Scalar(0, 255, 0), 2);
        line(hGraph, bluePoint1, bluePoint2, Scalar(0, 0, 255), 2);
    }

    return hGraph;
}

// @overload de plotarHistograma
Mat plotarHistograma( int ** histograma, int altura, int largura, int cvTipo, bool adaptarEscala = true) {
    // Cria um recipiente vazio de dimensões altura x largura com fundo preto

    Mat hGraph(altura, largura, cvTipo, Scalar(0, 0, 0));

    if(adaptarEscala) {
        maior = 0;

        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 256; j++) {
                if(histograma[i][j] > maior) maior = histograma[i][j];
            }
        }
    } else {
        maior = altura;
    }

    int modificador_largura = cvRound((double) largura/256);
    double modificador_altura = (double) altura/maior;

    for(int i = 1; i < 256; i++) {
        double  x1 = modificador_largura * (i-1),
                x2 = modificador_largura * i,
                r_y1 = altura - histograma[0][i-1] * modificador_altura,
                r_y2 = altura - histograma[0][i] * modificador_altura,
                g_y1 = altura - histograma[1][i-1] * modificador_altura,
                g_y2 = altura - histograma[1][i] * modificador_altura,
                b_y1 = altura - histograma[2][i-1] * modificador_altura,
                b_y2 = altura - histograma[2][i] * modificador_altura;

        Point redPoint1(x1, r_y1), redPoint2(x2, r_y2);
        Point greenPoint1(x1, g_y1), greenPoint2(x2, g_y2);
        Point bluePoint1(x1, b_y1), bluePoint2(x2, b_y2);

        line(hGraph, redPoint1, redPoint2, Scalar(255, 0, 0), 2);
        line(hGraph, greenPoint1, greenPoint2, Scalar(0, 255, 0), 2);
        line(hGraph, bluePoint1, bluePoint2, Scalar(0, 0, 255), 2);
    }

    return hGraph;
}

int main() {
    Mat img = imread("./imagem.png");
    int ** h = calcularHistograma(img); // h é uma matriz bidimensional de inteiros

    // Imagem que será usada para gravar o histograma
    Mat graph = imread("./graph.png");

    // Insere o histograma na imagem
    graph = plotarHistograma(h, graph);

    //Caso queira uma janela vazia para guardar o histograma
    /*
        int altura = 400,
            largura = 512;

        Mat graph = plotarHistograma(h, altura, largura, CV_8UC3);
    */

    // Nome personalizado do histograma e sua escala
    string graphName = "Histograma - " + to_string(maior) + ":256";

    // Mostra as imagens
    imshow("Original", img);
    imshow(graphName, graph);

    // Define a função de callback para a imagem
    setMouseCallback(graphName, callback, &graph);

    waitKey(0);
}
