#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    double *data;
    size_t rows;
    size_t cols;
    size_t size;
} Matrix;

Matrix mat_create(size_t rows, size_t cols){
    Matrix m;
    m.rows = rows;
    m.cols = cols;
    m.size = rows * cols;
    m.data = calloc(m.size, sizeof(double));
    return m;
}

void mat_free(Matrix *m){
    free(m->data);
    m->data = NULL;
    m->rows = 0;
    m->cols = 0;
}

void mat_copy(Matrix *dst, const Matrix *src){
    memcpy(dst->data, src->data, src->size * sizeof(double));
}

void mat_add(Matrix *dst, const Matrix *a, const Matrix *b){
    if(a->rows == b->rows && a->cols == b->cols){
        for(size_t i = 0; i < (a->size); i++){
            dst->data[i] = a->data[i] + b->data[i];
        }
    } else {
        printf("Cannot add matrices with mismatched dimensions.\n");
    }
}

void mat_hadamard(Matrix *dst, const Matrix *a, const Matrix *b){
    if(a->rows == b->rows && a->cols == b->cols){
        for(size_t i = 0; i < (a->size); i++){
            dst->data[i] = a->data[i] * b->data[i];
        }
    } else {
        printf("Cannot take the hadamard product of matrices with mismatched dimensions.\n");
    }
}

void mat_scalar(Matrix *m, double scalar){
    for(size_t i = 0; i < (m->size); i++){
        m->data[i] *= scalar;
    }
}

double mat_get_element(const Matrix *m, size_t i, size_t j){
    return m->data[(i * m->cols) + j];
}





void mat_print(Matrix *m) {
    size_t count = 0;
    for (size_t i = 0; i < m->rows; i++) {
        printf("|");
        for (size_t j = 0; j < m->cols; j++) {
            printf(j + 1 < m->cols ? "%.4f, " : "%.4f", m->data[count]);
            count++;
        }
        printf("|\n");
    }
    printf("\n");
}

int main(int argv, char *argc[]){
    Matrix m = mat_create(2,3);
    Matrix *p = &m;

    printf("Size: %d\n", (int)p->size);

    for(size_t i = 0; i < p->size; i++){
        p->data[i] = (i + 1);
    }


    
    mat_print(p);

    for(size_t i = 0; i < p->rows; i++){
        for(size_t j = 0; j < p->cols; j++){
            printf("%d : %f\n",(int)((i * p->cols) + j) , mat_get_element(p, i, j));
        }
    }
}