// src/matrix.c
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static inline size_t IDX(size_t i, size_t j, size_t cols) { return i*cols + j; }

Matrix* mat_create(size_t rows, size_t cols) {
    Matrix *A = (Matrix*)malloc(sizeof(Matrix));
    if (!A) return NULL;
    A->rows = rows;
    A->cols = cols;
    A->data = (double*)calloc(rows*cols, sizeof(double));
    if (!A->data) { free(A); return NULL; }
    return A;
}

Matrix* mat_zeros(size_t r, size_t c) { return mat_create(r, c); }

Matrix* mat_identity(size_t n) {
    Matrix *I = mat_create(n, n);
    if (!I) return NULL;
    for (size_t i = 0; i < n; ++i) I->data[IDX(i,i,n)] = 1.0;
    return I;
}

Matrix* mat_from_array(size_t r, size_t c, const double *arr) {
    Matrix *A = mat_create(r, c);
    if (!A) return NULL;
    for (size_t i = 0; i < r*c; ++i) A->data[i] = arr[i];
    return A;
}

Matrix* mat_clone(const Matrix *A) {
    if (!A) return NULL;
    Matrix *B = mat_create(A->rows, A->cols);
    if (!B) return NULL;
    for (size_t i = 0; i < A->rows*A->cols; ++i) B->data[i] = A->data[i];
    return B;
}

void mat_free(Matrix **A) {
    if (A && *A) {
        free((*A)->data);
        free(*A);
        *A = NULL;
    }
}

void mat_print(const Matrix *A, const char *name) {
    if (name) printf("%s =\n", name);
    if (!A) { printf("(null)\n"); return; }
    for (size_t i = 0; i < A->rows; ++i) {
        for (size_t j = 0; j < A->cols; ++j) {
            printf("%10.6f ", A->data[IDX(i,j,A->cols)]);
        }
        printf("\n");
    }
}

bool mat_equals(const Matrix *A, const Matrix *B, double eps) {
    if (!A || !B || A->rows != B->rows || A->cols != B->cols) return false;
    size_t n = A->rows * A->cols;
    for (size_t k = 0; k < n; ++k) {
        if (fabs(A->data[k] - B->data[k]) > eps) return false;
    }
    return true;
}

// --- verificação de dimensões ---
static bool same_shape(const Matrix *A, const Matrix *B) {
    return A && B && A->rows == B->rows && A->cols == B->cols;
}
static bool mult_compat(const Matrix *A, const Matrix *B) {
    return A && B && A->cols == B->rows;
}
static bool is_square(const Matrix *A) {
    return A && A->rows == A->cols;
}

// --- operações matriz-matriz ---
Matrix* mat_add(const Matrix *A, const Matrix *B, MatrixStatus *status) {
    if (!same_shape(A,B)) { if(status) *status = MAT_ERR_DIM; return NULL; }
    Matrix *C = mat_create(A->rows, A->cols);
    if (!C) { if(status) *status = MAT_ERR_ALLOC; return NULL; }
    size_t n = A->rows * A->cols;
    for (size_t k = 0; k < n; ++k) C->data[k] = A->data[k] + B->data[k];
    if(status) *status = MAT_OK;
    return C;
}

Matrix* mat_sub(const Matrix *A, const Matrix *B, MatrixStatus *status) {
    if (!same_shape(A,B)) { if(status) *status = MAT_ERR_DIM; return NULL; }
    Matrix *C = mat_create(A->rows, A->cols);
    if (!C) { if(status) *status = MAT_ERR_ALLOC; return NULL; }
    size_t n = A->rows * A->cols;
    for (size_t k = 0; k < n; ++k) C->data[k] = A->data[k] - B->data[k];
    if(status) *status = MAT_OK;
    return C;
}

Matrix* mat_mul(const Matrix *A, const Matrix *B, MatrixStatus *status) {
    if (!mult_compat(A,B)) { if(status) *status = MAT_ERR_DIM; return NULL; }
    Matrix *C = mat_create(A->rows, B->cols);
    if (!C) { if(status) *status = MAT_ERR_ALLOC; return NULL; }
    for (size_t i = 0; i < A->rows; ++i) {
        for (size_t k = 0; k < A->cols; ++k) {
            double aik = A->data[IDX(i,k,A->cols)];
            for (size_t j = 0; j < B->cols; ++j) {
                C->data[IDX(i,j,B->cols)] += aik * B->data[IDX(k,j,B->cols)];
            }
        }
    }
    if(status) *status = MAT_OK;
    return C;
}

// --- operações com escalar ---
Matrix* mat_add_scalar(const Matrix *A, double s, MatrixStatus *status) {
    if (!A) { if(status) *status = MAT_ERR_NULL; return NULL; }
    Matrix *C = mat_clone(A);
    if (!C) { if(status) *status = MAT_ERR_ALLOC; return NULL; }
    size_t n = A->rows * A->cols;
    for (size_t k = 0; k < n; ++k) C->data[k] += s;
    if(status) *status = MAT_OK;
    return C;
}
Matrix* mat_sub_scalar(const Matrix *A, double s, MatrixStatus *status) {
    return mat_add_scalar(A, -s, status);
}
Matrix* mat_scale(const Matrix *A, double s, MatrixStatus *status) {
    if (!A) { if(status) *status = MAT_ERR_NULL; return NULL; }
    Matrix *C = mat_clone(A);
    if (!C) { if(status) *status = MAT_ERR_ALLOC; return NULL; }
    size_t n = A->rows * A->cols;
    for (size_t k = 0; k < n; ++k) C->data[k] *= s;
    if(status) *status = MAT_OK;
    return C;
}

// --- transposta ---
Matrix* mat_transpose(const Matrix *A, MatrixStatus *status) {
    if (!A) { if(status) *status = MAT_ERR_NULL; return NULL; }
    Matrix *T = mat_create(A->cols, A->rows);
    if (!T) { if(status) *status = MAT_ERR_ALLOC; return NULL; }
    for (size_t i = 0; i < A->rows; ++i)
        for (size_t j = 0; j < A->cols; ++j)
            T->data[IDX(j,i,T->cols)] = A->data[IDX(i,j,A->cols)];
    if(status) *status = MAT_OK;
    return T;
}

// --- determinante via eliminação gaussiana com pivoteamento parcial ---
double mat_determinant(const Matrix *A, MatrixStatus *status) {
    if (!is_square(A)) { if(status) *status = MAT_ERR_NOT_SQUARE; return NAN; }
    size_t n = A->rows;
    Matrix *M = mat_clone(A);
    if (!M) { if(status) *status = MAT_ERR_ALLOC; return NAN; }

    double det = 1.0;
    int sign = 1;

    for (size_t k = 0; k < n; ++k) {
        // pivoteamento parcial
        size_t piv = k;
        double maxv = fabs(M->data[IDX(k,k,n)]);
        for (size_t i = k+1; i < n; ++i) {
            double v = fabs(M->data[IDX(i,k,n)]);
            if (v > maxv) { maxv = v; piv = i; }
        }
        if (fabs(maxv) < 1e-12) {
            mat_free(&M);
            if(status) *status = MAT_ERR_SINGULAR;
            return 0.0;
        }
        if (piv != k) {
            // troca de linhas
            for (size_t j = 0; j < n; ++j) {
                double tmp = M->data[IDX(k,j,n)];
                M->data[IDX(k,j,n)] = M->data[IDX(piv,j,n)];
                M->data[IDX(piv,j,n)] = tmp;
            }
            sign = -sign;
        }
        // eliminação
        for (size_t i = k+1; i < n; ++i) {
            double factor = M->data[IDX(i,k,n)] / M->data[IDX(k,k,n)];
            M->data[IDX(i,k,n)] = 0.0;
            for (size_t j = k+1; j < n; ++j) {
                M->data[IDX(i,j,n)] -= factor * M->data[IDX(k,j,n)];
            }
        }
    }
    // determinante = produto da diagonal * sign
    for (size_t i = 0; i < n; ++i) det *= M->data[IDX(i,i,n)];
    det *= sign;

    mat_free(&M);
    if(status) *status = MAT_OK;
    return det;
}

// --- inversa via Gauss-Jordan com pivoteamento parcial ---
Matrix* mat_inverse(const Matrix *A, MatrixStatus *status) {
    if (!is_square(A)) { if(status) *status = MAT_ERR_NOT_SQUARE; return NULL; }
    size_t n = A->rows;

    // matriz aumentada [A | I]
    Matrix *Aug = mat_create(n, 2*n);
    if (!Aug) { if(status) *status = MAT_ERR_ALLOC; return NULL; }
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j)
            Aug->data[IDX(i,j,2*n)] = A->data[IDX(i,j,n)];
        Aug->data[IDX(i,n+i,2*n)] = 1.0;
    }

    // eliminação Gauss-Jordan
    for (size_t k = 0; k < n; ++k) {
        // pivoteamento parcial
        size_t piv = k;
        double maxv = fabs(Aug->data[IDX(k,k,2*n)]);
        for (size_t i = k+1; i < n; ++i) {
            double v = fabs(Aug->data[IDX(i,k,2*n)]);
            if (v > maxv) { maxv = v; piv = i; }
        }
        if (fabs(maxv) < 1e-12) {
            mat_free(&Aug);
            if(status) *status = MAT_ERR_SINGULAR;
            return NULL;
        }
        if (piv != k) {
            for (size_t j = 0; j < 2*n; ++j) {
                double tmp = Aug->data[IDX(k,j,2*n)];
                Aug->data[IDX(k,j,2*n)] = Aug->data[IDX(piv,j,2*n)];
                Aug->data[IDX(piv,j,2*n)] = tmp;
            }
        }
        // normaliza linha pivô
        double pivv = Aug->data[IDX(k,k,2*n)];
        for (size_t j = 0; j < 2*n; ++j) Aug->data[IDX(k,j,2*n)] /= pivv;

        // zera as outras linhas na coluna k
        for (size_t i = 0; i < n; ++i) {
            if (i == k) continue;
            double factor = Aug->data[IDX(i,k,2*n)];
            if (fabs(factor) > 1e-18) {
                for (size_t j = 0; j < 2*n; ++j)
                    Aug->data[IDX(i,j,2*n)] -= factor * Aug->data[IDX(k,j,2*n)];
            }
        }
    }

    // extrai a inversa (lado direito)
    Matrix *Inv = mat_create(n, n);
    if (!Inv) { mat_free(&Aug); if(status) *status = MAT_ERR_ALLOC; return NULL; }
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            Inv->data[IDX(i,j,n)] = Aug->data[IDX(i,n+j,2*n)];

    mat_free(&Aug);
    if(status) *status = MAT_OK;
    return Inv;
}
