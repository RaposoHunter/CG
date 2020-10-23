#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>

using namespace std;

int ** generateMesh(int rows = 3, int cols = 3) {
    srand(time(NULL));
    int **mesh = (int **) malloc(sizeof(int *) * rows);

    for(int i = 0; i < rows; i++) {
        mesh[i] = (int *) malloc(sizeof(int) * cols);
        for(int j = 0; j < cols; j++) {
            mesh[i][j] = rand() % 256;
        }
    }

    return mesh;
}

int ** filterMesh(int ** mesh, int rows, int cols) {
    int **fmesh = (int **) malloc(sizeof(int *) * rows);
    int filter[3][3] = {
        {0, 2, 0},
        {2, 0, 2},
        {0, 2, 0}
    };

    for(int i = 0; i < rows; i++) {
        fmesh[i] = (int *) malloc(sizeof(int) * cols);
        for(int j = 0; j < cols; j++) {
            int num = 0;
            int den = 0;

            for(int r = -1; r < 2; r++) {
                for(int c = -1; c < 2; c++) {
                    if(i + r >= rows || i + r < 0 || j + c >= cols || j + c < 0) continue;

                    num += mesh[i + r][j + c] * filter[r + 1][c + 1];
                    den += filter[r + 1][c + 1];
                }
            }

            fmesh[i][j] = num/den;
        }
    }

    return fmesh;
}

int main() {
    int ** mesh;
    int ** fmesh;
    int rows = 10, cols = 10;

    mesh = generateMesh(rows, cols);

    cout << "Original Mesh:" << endl;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int meshNum = mesh[i][j];

            if(meshNum < 10) cout << meshNum << "   ";
            else if(meshNum < 100) cout << meshNum << "  ";
            else cout << meshNum << " ";
        }
        cout << endl;
    }

    fmesh = filterMesh(mesh, rows, cols);

    cout << endl << "Filtered Mesh:" << endl;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int meshNum = fmesh[i][j];

            if(meshNum < 10) cout << meshNum << "   ";
            else if(meshNum < 100) cout << meshNum << "  ";
            else cout << meshNum << " ";
        }
        cout << endl;
    }
}
