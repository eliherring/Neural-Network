#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void test_vec(){
    printf("--------------vec test--------------\n");
    Vector v1 = vec_create(4);
    Vector *p1 = &v1;
    Vector v2 = vec_create(4);
    Vector *p2 = &v2;
    Vector dst = vec_create(4);
    Vector *pdst = &dst;

    for(size_t i = 0; i < p1->size; i++){
        p1->data[i] = i + 1;
        p2->data[i] = (i + 1) * 2;
    }

    vec_add(pdst,p1,p2);

    vec_print(p1);
    vec_print(p2);
    vec_print(pdst);
    vec_scale(pdst,2);
    vec_print(pdst);
    vec_copy(pdst,p2);
    vec_print(pdst);
    printf("%f \n", vec_dot(p1,p2));


    vec_free(p1);
    vec_free(p2);
    vec_free(pdst);
    printf("------------vec test over------------\n");
}




void main(int arvc, char *argv[]){
    test_vec();
}