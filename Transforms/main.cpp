#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdio>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#define RAD 0
#define DEG 1
#define PI 3.14159265358979323846

#include <Eigen/Dense>

using namespace Eigen;
using namespace std;
using namespace cv;

Mat rotateImgPart(Mat img, double x, double y, double width, double height, double angle, double scaleX = 1.0, double scaleY = 1.0, int mode = RAD) {
    bool applyCorrection = (int) angle % 90 != 0;

    Mat rotatedImg(img.cols, img.rows, CV_8UC3, Scalar(100,100,100));
    Mat scaledImg = rotatedImg.clone();

    Matrix3d R, T1, T2, S;
    Vector3d p[4], pc;

    p[0] << x, y, 1; // Canto superior esquerdo a imagem
    p[1] << x + width, y + height, 1; // Canto inferior direito da imagem
    pc = (p[0] + p[1])/2;

    if(mode == DEG) {
        angle *= PI/180;
    }

    T1 <<      1,      0, 0,
               0,      1, 0,
          -pc[0], -pc[1], 1;

    T2 <<     1,     0, 0,
              0,     1, 0,
          pc[0], pc[1], 1;

    R << cos(angle) , sin(angle), 0,
         -sin(angle), cos(angle), 0,
                   0,          0, 1;

    S << scaleX,      0, 0,
              0, scaleY, 0,
              0,      0, 1;

    Vector3d a, d;
    Vec3b **pixels = (Vec3b **) malloc(sizeof(Vec3b *) * (int) (height + 1));

    // Guarda os valores BGR dos pixels originais
    for(int i = 0; i < height + 1; i++) {
        pixels[i] = (Vec3b *) malloc(sizeof(Vec3b) * (int) (width + 1));

        for(int j = 0; j < width + 1; j++) {
            Vec3b pixel = img.at<Vec3b>(y + i, x + j); // BGR
            img.at<Vec3b>(y + i, x + j) = {0, 0, 0}; // Adiciona um fundo preto à parte da imagem que será extraida
            pixels[i][j] = pixel;
        }
    }

    if(scaleX != 1.0 || scaleY != 1.0) {
        // Altera a escala da imagem
        for(int i = 0; i < height + 1; i++) {
            for(int j = 0; j < width + 1; j++) {
                // Realiza as transformações ponto a ponto
                a << x + j, y + i, 1;
                d = a.transpose() * T1 * S * T2;

                if(d[0] >= 0 && d[0] < scaledImg.cols && d[1] >= 0 && d[1] < scaledImg.rows) {
                    for(int sy = 0; sy < scaleY; sy++) {
                        for(int sx = 0; sx < scaleX; sx++) {
                            if(d[0] + sx >= 0 && d[0] + sx < scaledImg.cols && d[1] + sy >= 0 && d[1] + sy < scaledImg.rows) scaledImg.at<Vec3b>(Point2d(d[0] + sx, d[1] + sy)) = pixels[i][j];
                        }
                    }
                }
            }
        }

        free(pixels); // Libera o espaço de memória usado anteriormente

        p[2] = p[0].transpose() * T1 * S * T2; // Calcula a posição do canto superior do recorte escalonado da imagem

        // Atualização dos valores
        x = p[2][0];
        y = p[2][1];
        width *= scaleX;
        height *= scaleY;

        pixels = (Vec3b **) malloc(sizeof(Vec3b *) * (int) (height + 1));

        // Guarda os valores BGR dos pixels que foram escalados
        for(int i = 0; i < height + 1; i++) {
            pixels[i] = (Vec3b *) malloc(sizeof(Vec3b) * (int) (width + 1));

            for(int j = 0; j < width + 1; j++) {
                Vec3b pixel = scaledImg.at<Vec3b>(y + i, x + j); // BGR
                pixels[i][j] = pixel;
            }
        }
    }

    // Rotaciona o recorte da imagem
    for(int i = 0; i < height + 1; i++) {
        for(int j = 0; j < width + 1; j++) {
            a << x + j, y + i, 1;
            d = a.transpose() * T1 * R * T2;

            if(d[0] >= 0 && d[0] < rotatedImg.cols && d[1] >= 0 && d[1] < rotatedImg.rows) {
                rotatedImg.at<Vec3b>(Point2d(d[0], d[1])) = pixels[i][j];

                if(applyCorrection) {
                    rotatedImg.at<Vec3b>(Point2d(d[0], d[1] - 1)) = pixels[i][j];
                    rotatedImg.at<Vec3b>(Point2d(d[0], d[1] + 1)) = pixels[i][j];
                    rotatedImg.at<Vec3b>(Point2d(d[0] - 1, d[1])) = pixels[i][j];
                    rotatedImg.at<Vec3b>(Point2d(d[0] + 1, d[1])) = pixels[i][j];
                }
            }
        }
    }

    return rotatedImg;
}

int main() {
    Mat img = imread("./img_2.png");
    Mat res;

    res = rotateImgPart(img, img.cols / 4, img.rows / 4, 200, 200, 27, 2, 1, DEG);

    cout << "Para encerrar o programa aperte qualquer tecla enquanto na tela de uma das imagens.\n";

    imshow("Original", img);
    imshow("Transformed Part", res);

    waitKey(0);
}
