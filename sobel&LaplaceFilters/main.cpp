#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

string LaplacianFilter(Mat img) {
    // De acordo com o site: https://homepages.inf.ed.ac.uk/rbf/HIPR2/log.htm
    // "O filtro de Laplace real�a regi�es com mudan�as r�pidas de intensidade e, portanto, � comummente usado para detec��o de bordas"

    // Declarando as vari�veis que ser�o usadas
    Mat gray, draw, draw2;

    // Convertendo a imagem original para escala de cinza
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Aplicando o filtro de LaPlace na imagem em tons de cinza
    Laplacian(gray, draw, CV_16S, 3);
    convertScaleAbs(draw, draw2);
    // OBS.: N�o � poss�vel mostrar "draw" diretamente, n�o sei o porqu�
    //       Apenas depois de salvar a imagem em algum lugar � poss�vel l�-la depois

    // Destinos das imagens com e sem convers�o de escala absoluta
    string path1 = "./foreverPlayerLaplace.jpg"; //... sem convers�o
    string path2 = "./foreverPlayerLaplaceConverted.jpg"; //... com convers�o

    // Salvando ambas as imagens es seus respectivos destinos
    imwrite(path1, draw);
    imwrite(path2, draw2);

    // Retornando o primeiro destino para ser lido e mostrado depois pelo programa
    return path1;
}

Mat SobelFilter(Mat img) {
    // O filtro de sobel tem uma funcionalidade similar ao filtro de laplace

    // Declara��o e atribui��o da imagem em tons de cinza a partir de "img"
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Declara��o e aplica��o do filtro sobel na imagem em tons de cinza
    Mat sobelx;
    Sobel(gray, sobelx, CV_32F, 1, 0);

    // Par�metros chave para convers�o do filtro sobel
    double minVal, maxVal;
    minMaxLoc(sobelx, &minVal, &maxVal);
    cout << "minVal: " << minVal << "\nmaxval: " <<  maxVal << "\n";

    // Declara��o e atribui��o da imagem final a partir do filtro sobel
    Mat draw;
    double alpha = 255.0 / (maxVal - minVal), beta = -minVal * 255.0 / (maxVal - minVal);
    sobelx.convertTo(draw, CV_8U, alpha, beta);

    // Salvando a imagem final no destino especificado
    string path = "./foreverPlayerSobeled.jpg";
    imwrite(path, draw);

    // Retornando a imagem final para o programa principal
    return draw;
}

int main()
{
    Mat img, laplaced, sobeled;

    img = imread("./forever.jpg");
    imshow("Original Forever", img);

    laplaced = imread(LaplacianFilter(img));
    sobeled = SobelFilter(img);

    imshow("LaPlaced Forever", laplaced);
    imshow("Sobeled Forever", sobeled);

    waitKey(0);
}
