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
        printf("Cannot add matrices - dimension mismatch.\n");
    }
}

void mat_hadamard(Matrix *dst, const Matrix *a, const Matrix *b){
    if(a->rows == b->rows && a->cols == b->cols){
        for(size_t i = 0; i < (a->size); i++){
            dst->data[i] = a->data[i] * b->data[i];
        }
    } else {
        printf("Cannot take the hadamard product of matrices - dimension mismatch.\n");
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


size_t mat_get_index(const Matrix *m, size_t i, size_t j){
    return (i * m->cols) + j;
}

double mat_mult_sum_helper(const Matrix *a, const Matrix *b, size_t i, size_t j){
    double sum = 0.0;

    for(size_t k = 0; k < a->cols; k++){
        sum += (a->data[(a->cols * i) + k]) * (b->data[(b->cols * k) + j]);
    }

    return sum;
}

Matrix mat_multiply(Matrix *a, Matrix *b){

    //dimension check here.

    //needs to consider what the size of the dst matrix should be.
    //Probably should be allocated here,
    //Can be returned as a matrix object. 
    //Whatever calls this will have to be in charge of freeing it. 

    Matrix m = mat_create(a->rows, b->cols);
    Matrix *p = &m;

    if(a->cols == b->rows){
       size_t count = 0;
    
        for(size_t i = 0; i < a->rows; i++){
            for(size_t j = 0; j < b->cols; j++){
                p->data[count] = mat_mult_sum_helper(a, b, i, j);
                count++;
            }
        }
    } else {
        printf("Cannot multiply matrices - dimension mismatch."); 
    }

    return m;
}

void mat_transpose(Matrix *m){
    double *temp = malloc(m->size * sizeof(double));

    for(size_t i = 0; i < m->rows; i++){
        for(size_t j = 0; j < m->cols; j++){
            temp[(j * m->rows) + i] = m->data[(i * m->cols) + j];
        }
    }

    memcpy(m->data, temp, m->size * sizeof(double));
    free(temp);

    size_t tmp = m->rows;
    m->rows = m->cols;
    m->cols = tmp;
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

void test_mat(){
    Matrix m1 = mat_create(2,5);
    Matrix *p1 = &m1;

    Matrix m2 = mat_create(5,2);
    Matrix *p2 = &m2;


    for(size_t i = 0; i < p1->size; i++){
        p1->data[i] = (i + 1);
    }

    for(size_t i = 0; i < p2->size; i++){
        p2->data[i] = (10 - i);
    }


    
    mat_print(p1);    
    mat_print(p2);

    Matrix m3 = mat_multiply(p1,p2);
    Matrix *p3 = &m3;

    mat_print(p3);

    mat_transpose(p3);

    mat_print(p3);

    mat_free(p1);
    mat_free(p2);
    mat_free(p3);
}

int main(int argv, char *argc[]){
    test_mat();
}