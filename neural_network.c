#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//--  --  --  --  --  --  --  Vectors  --  --  --  --  --  --  --//

typedef struct {
    double *data;
    size_t size;
} Vector;

Vector vec_create(size_t size) {
    Vector v;
    v.size = size;
    v.data = calloc(size, sizeof(double));
    return v;
}

void vec_free(Vector *v) {
    free(v->data);
    v->data = NULL;
    v->size = 0;
}

void vec_add(Vector *dst, const Vector *a, const Vector *b) {
    for(size_t i = 0; i < a->size; i++){
        dst->data[i] = a->data[i] + b->data[i];
    }
}

double vec_dot(const Vector *a, const Vector *b) {
    double sum = 0.0;
    for(size_t i = 0; i < a->size; i++){
        sum += a->data[i] * b->data[i];
    }
    return sum;
}

void vec_scale(Vector *v, double scalar){
    for(size_t i = 0; i < v->size; i++){
        v->data[i] *= scalar;
    }
}

void vec_apply(Vector *v, double (*fn)(double)){
    for(size_t i = 0; i < v->size; i++){
        v->data[i] = fn(v->data[i]);
    }
}

void vec_copy(Vector *dst, Vector *src){
    memcpy(dst->data, src->data, src->size * sizeof(double));
}

void vec_print(const Vector *v){
    printf("[");
    for(size_t i = 0; i < v->size; i++){
        printf(i + 1 < v->size ? "%.4f, " : "%.4f", v->data[i]);
    }
    printf("]\n");
}

//--  --  --  --  --  --  --  Matricies  --  --  --  --  --  --  --//

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



void main(int argc, char *argv[]){
    test_vec();
    if(argc > 1){
        test_mat(atoi(argv[1]));
    }
}