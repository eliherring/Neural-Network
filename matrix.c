#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Square matrix only...
typedef struct {
    double *data;
    size_t size;
} Matrix;

Matrix mat_create(size_t size) {
    Matrix m;
    m.size = size;
    m.data = calloc(size * size, sizeof(double));
    return m;
}

void mat_free(Matrix *m) {
    free(m->data);
    m->data = NULL;
    m->size = 0;
}

void mat_copy(Matrix *dst, Matrix *src){
    memcpy(dst->data, src->data, src->size * src->size * sizeof(double));
}

void mat_add(Matrix *dst, const Matrix *a, const Matrix *b){
    size_t count = 0;
    for(size_t i = 0; i < a->size; i++){
        for(size_t j = 0; j < a->size; j++){
            dst->data[count] = a->data[count] + b->data[count];
            count +=1;
        }
    }
}

void mat_scalar(Matrix *m, double scalar){
    for(size_t i = 0; i < m->size * m->size; i++){
        m->data[i] = scalar * m->data[i];
    }
}



double mat_get_element(const Matrix *m, size_t i, size_t j) {
    return m->data[(i * m->size) + j];
}

size_t mat_get_index(const Matrix *m, size_t i, size_t j) {
    return (i * m->size) + j;
}

double mat_mult_sum_helper(const Matrix *a, const Matrix *b, size_t i, size_t j) {
    double sum = 0.0;
    for (size_t k = 0; k < a->size; k++) {
        sum += a->data[(i * a->size) + k] * b->data[(k * b->size) + j];
    }
    return sum;
}

void mat_multiply(Matrix *dst, const Matrix *a, const Matrix *b) {
    size_t count = 0;
    for (size_t i = 0; i < a->size; i++) {
        for (size_t j = 0; j < a->size; j++) {
            dst->data[count] = mat_mult_sum_helper(a, b, i, j);
            count++;
        }
    }
}

void mat_hadamard_product(Matrix *dst, const Matrix *a, const Matrix *b){
    for(size_t i = 0; i < a->size * a->size; i++){
        dst->data[i] = a->data[i] * b->data[i];
    }
}

void mat_transpose(Matrix *m){
    Matrix temp = mat_create(m->size);
    Matrix *p = &temp;
    size_t count = 0;

    for(size_t i = 0; i < m->size; i++){
        for(size_t j = 0; j < m->size; j++){
            p->data[count] = m->data[mat_get_index(m, j, i)];
            count += 1;
        }       
    }

    mat_copy(m, p);
    mat_free(p);
}

void mat_print(Matrix *m) {
    size_t count = 0;
    for (size_t i = 0; i < m->size; i++) {
        printf("|");
        for (size_t j = 0; j < m->size; j++) {
            printf(j + 1 < m->size ? "%.4f, " : "%.4f", m->data[count]);
            count++;
        }
        printf("|\n");
    }
}

int main(int argc, char *argv[]){
    if(argc > 1){
        int temp = atoi(argv[1]);
        size_t size = (size_t) temp;
        
        Matrix m1 = mat_create(size);
        Matrix *p1 = &m1;
        Matrix m2 = mat_create(size);
        Matrix *p2 = &m2;
        Matrix m3 = mat_create(size);
        Matrix *p3 = &m3;

        for(size_t i = 0; i < p1->size * p1->size; i++){
            p1->data[i] = (i + 1);
            p2->data[i] =  (p1->size * p1->size - i);
        }
        
        mat_add(p3, p1, p2);

        printf("Hello\n");
        
        mat_print(p1);
        printf("\n");
        mat_print(p2);
        printf("\n");
        mat_multiply(p3,p1,p2);
        mat_print(p3);
        printf("\n");
        mat_transpose(p3);
        mat_print(p3);
        printf("\n");

        mat_free(p1);
        mat_free(p2);
        mat_free(p3);

        
    }
}