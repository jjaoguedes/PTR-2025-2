// inc/matrix.h
#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Matrix {
    size_t rows;
    size_t cols;
    double *data; // row-major: data[i*cols + j]
} Matrix;

typedef enum {
    MAT_OK = 0,
    MAT_ERR_ALLOC,
    MAT_ERR_DIM,
    MAT_ERR_NOT_SQUARE,
    MAT_ERR_SINGULAR,
    MAT_ERR_NULL
} MatrixStatus;

// --- criação / destruição ---
Matrix* mat_create(size_t rows, size_t cols);
Matrix* mat_zeros(size_t rows, size_t cols);
Matrix* mat_identity(size_t n);
Matrix* mat_from_array(size_t rows, size_t cols, const double *arr);
Matrix* mat_clone(const Matrix *A);
void    mat_free(Matrix **A);

// --- utilidades ---
void    mat_print(const Matrix *A, const char *name);
bool    mat_equals(const Matrix *A, const Matrix *B, double eps);

// --- operações matriz-matriz ---
Matrix* mat_add(const Matrix *A, const Matrix *B, MatrixStatus *status);
Matrix* mat_sub(const Matrix *A, const Matrix *B, MatrixStatus *status);
Matrix* mat_mul(const Matrix *A, const Matrix *B, MatrixStatus *status);

// --- operações com escalar ---
Matrix* mat_add_scalar(const Matrix *A, double s, MatrixStatus *status);
Matrix* mat_sub_scalar(const Matrix *A, double s, MatrixStatus *status);
Matrix* mat_scale(const Matrix *A, double s, MatrixStatus *status);

// --- transposta ---
Matrix* mat_transpose(const Matrix *A, MatrixStatus *status);

// --- determinante / inversa (apenas quadradas) ---
double  mat_determinant(const Matrix *A, MatrixStatus *status);
Matrix* mat_inverse(const Matrix *A, MatrixStatus *status);

#ifdef __cplusplus
}
#endif
#endif // MATRIX_H
