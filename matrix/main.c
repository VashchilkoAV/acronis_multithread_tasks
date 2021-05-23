#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define D_MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define D_MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef struct thread_args {
    int M;
    int N;
    int K;
    const double * A;
    const double * B;
    double * C;
} thread_args;

void printMatrix(int numColumns, int numRows, double * matrix) {
    int i, j;
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numColumns; j++) {
            printf("%lf ", matrix[i * numColumns + j]);
        }
        printf("\n");
    }
}

void fillMatrix(int numColumns, int numRows, double * matrix) {
    int i, j;
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numColumns; j++) {
            matrix[i * numColumns + j] = rand();
        }
    }
}

int runThreads(int numThreads, double * matrix1, double * matrix2, double * result, int matrix1NumRows, int matrix1NumColumns, int matrix2NumRows, int matrix2NumColumns) {
    return 1;
}

int main(int argc, char** argv) {
    int matrix1NumRows = 0, matrix1NumColumns = 0, matrix2NumRows = 0, matrix2NumColumns = 0, num_threads = 1, i = 0;
    if (argc < 6) {
        printf("Not enough parameters provided\n");
        return 1;
    }
    num_threads = atoi(argv[1]);
    matrix1NumRows = atoi(argv[2]);
    matrix1NumColumns = atoi(argv[3]);
    matrix2NumRows = atoi(argv[4]);
    matrix2NumColumns = atoi(argv[5]);

    if (matrix1NumColumns != matrix2NumRows) {
        printf("Cannot multiply matrices: unappropriated operand size\n");
        return 1;
    }

    double * result = (double*)malloc(matrix1NumRows*matrix2NumColumns*sizeof(double));
    double * matrix1 = (double*)malloc(matrix1NumRows*matrix1NumColumns*sizeof(double));
    double * matrix2 = (double*)malloc(matrix2NumRows*matrix2NumColumns*sizeof(double));

    fillMatrix(matrix1NumColumns, matrix1NumRows, matrix1);
    printMatrix(matrix1NumColumns, matrix1NumRows, matrix1);

    free(result);
    free(matrix1);
    free(matrix2);
    return 0;
}
