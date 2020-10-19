#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

string LaplacianFilter(Mat img) {
    // De acordo com o site: https://homepages.inf.ed.ac.uk/rbf/HIPR2/log.htm
    // "O filtro de Laplace realça regiões com mudanças rápidas de intensidade e, portanto, é comummente usado para detecção de bordas"

    // Declarando as variáveis que serão usadas
    Mat gray, draw, draw2;

    // Convertendo a imagem original para escala de cinza
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Aplicando o filtro de LaPlace na imagem em tons de cinza
    Laplacian(gray, draw, CV_16S, 3);
    convertScaleAbs(draw, draw2);
    // OBS.: Não é possível mostrar "draw" diretamente, não sei o porquê
    //       Apenas depois de salvar a imagem em algum lugar é possível lê-la depois

    // Destinos das imagens com e sem conversão de escala absoluta
    string path1 = "./foreverPlayerLaplace.jpg"; //... sem conversão
    string path2 = "./foreverPlayerLaplaceConverted.jpg"; //... com conversão

    // Salvando ambas as imagens es seus respectivos destinos
    imwrite(path1, draw);
    imwrite(path2, draw2);

    // Retornando o primeiro destino para ser lido e mostrado depois pelo programa
    return path1;
}

Mat SobelFilter(Mat img) {
    // O filtro de sobel tem uma funcionalidade similar ao filtro de laplace

    // Declaração e atribuição da imagem em tons de cinza a partir de "img"
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // Declaração e aplicação do filtro sobel na imagem em tons de cinza
    Mat sobelx;
    Sobel(gray, sobelx, CV_32F, 1, 0);

    // Parâmetros chave para conversão do filtro sobel
    double minVal, maxVal;
    minMaxLoc(sobelx, &minVal, &maxVal);
    cout << "minVal: " << minVal << "\nmaxval: " <<  maxVal << "\n";

    // Declaração e atribuição da imagem final a partir do filtro sobel
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
