#include <iostream>
#include <iomanip>
#include <vector>

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

void callback(int event, int x, int y, int flags, void * userdata) {
    system("CLS");
    cout << "(x,y): (" <<  setw(3) << x << ", " << setw(3) << y << ")";
}

Mat rotateImgPart(Mat img, double x, double y, double width, double height, double angle, int mode = RAD) {
    Mat rotatedImg = img.clone();

    Matrix3d R, T1, T2;
    Vector3d p[4], pc, f[4];

    p[0] << x, y, 1;
    p[1] << x + width, y, 1;
    p[2] << x + width, y + height, 1;
    p[3] << x, y + height, 1;
    pc = (p[0] + p[2])/2;

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

    for(int i = 0; i < 4; i++) {
        f[i] = p[i].transpose() * T1 * R * T2;
    }

    vector<vector<vector<int>>> v;
    vector<vector<int>> tmp1;
    vector<int> tmp2;

    for(int k = 0; k < 3; k++) {
        tmp2.push_back(0);
    }

    for(int j = 0; j < width + 1; j++) {
        tmp1.push_back(tmp2);
    }

    for(int i = 0; i < height + 1; i++) {
        v.push_back(tmp1);
    }

    Vector3d a, d;
    Vec3b pixels[(int) ((width + 1) * (height + 1))];

    int c = 0;
    for(int i = 0; i < height + 1; i++) {
        for(int j = 0; j < width + 1; j++) {
            Vec3b pixel = rotatedImg.at<Vec3b>(y + i, x + j); // BGR
            pixels[c++] = pixel;

            v[i][j][0] = (int) pixel[0]; // B
            v[i][j][1] = (int) pixel[1]; // G
            v[i][j][2] = (int) pixel[2]; // R
            //cout << rotatedImg.at<Vec3b>(y + i, x + j) << endl;
            rotatedImg.at<Vec3b>(y + i, x + j) = {0,0,0};


        }
    }

    c = 0;
    /*for(int i = 0; i < height + 1; i++) {
        for(int j = 0; j < width + 1; j++) {
            a << x + j, y + i, 1;
            d = a.transpose() * T1 * R * T2;
            rotatedImg.at<Vec3b>(d[1], d[0]) = 0;
        }
    }*/

    for(int i = 0; i < height + 1; i++) {
        for(int j = 0; j < width + 1; j++) {
            a << x + j, y + i, 1;
            d = a.transpose() * T1 * R * T2;
            //cout << d.transpose() << endl;
            //int nX = cvRound((double) d[0]), nY = cvRound((double) d[1]);
            rotatedImg.at<Vec3b>(d[1], d[0]) = pixels[c++];
            //cout << a.transpose() << endl;
        }
    }

    for(int i = 0; i < 4; i++) {
        line(rotatedImg, Point(f[i][0], f[i][1]), Point(f[(i + 1) % 4][0], f[(i + 1) % 4][1]), Scalar(0,255,0));
    }

    return rotatedImg;
}

int main() {
    Mat img = imread("./img_2.png");
    Mat r[5];

    r[0] = rotateImgPart(img, 400, 400, 100, 100, 90, DEG);
    r[1] = rotateImgPart(img, 100, 100, 50, 50, 180, DEG);
    r[2] = rotateImgPart(img, 200, 200, 150, 150, 270, DEG);
    r[3] = rotateImgPart(img, img.cols / 2, img.rows / 2, 75, 75, 30, DEG);
    r[4] = rotateImgPart(img, 150, 150, 300, 300, 60, DEG);

    Vector3d p[4], pc;

    /*p[0] << 200, 200, 1;
    p[1] << 300, 200, 1;
    p[2] << 300, 300, 1;
    p[3] << 200, 300, 1;
    pc = (p[0] + p[2]) / 2;*/

    /*for(int i = 0; i < 4; i++) {
        line(img, Point(p[i][0], p[i][1]), Point(p[(i + 1) % 4][0], p[(i + 1) % 4][1]), Scalar(255,255,255));
    }*/

    imshow("Makako", img);
    imshow("Makako_0", r[0]);
    imshow("Makako_1", r[1]);
    imshow("Makako_2", r[2]);
    imshow("Makako_3", r[3]);
    imshow("Makako_4", r[4]);

    setMouseCallback("Makako", callback);
    waitKey(0);

    /*cout << img.at<Vec3b>(250, 250);

    int c = 0;

    cout << "[\n";

    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 4; j++) {
            cout << "[";
            for(int k = 0; k < 7; k++) {
                cout << v[i][j][k] << " ";
            }
            cout << "]";
        }
        cout << "\n";
    }

    cout << "]";*/
}
