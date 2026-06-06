#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


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

Matrix mat_create_random(size_t rows, size_t cols){
    Matrix m;
    m.rows = rows;
    m.cols = cols;
    m.size = rows * cols;
    m.data = calloc(m.size, sizeof(double));
    Matrix *p = &m;
    for(size_t i = 0; i < p->size; i++){
        p->data[i] = (double)rand() / ((double)RAND_MAX + 1.0);
    }
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

Matrix mat_add(const Matrix *a, const Matrix *b){
    Matrix m = mat_create(a->rows, a->cols);
    Matrix *p = &m;
    if(a->rows == b->rows && a->cols == b->cols){
        for(size_t i = 0; i < (a->size); i++){
            p->data[i] = a->data[i] + b->data[i];
        }
    } else {
        printf("Cannot add matrices - dimension mismatch.\n");
    }
    return m;
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

void mat_apply(Matrix *m, double (*fn)(double)){
    for(size_t i = 0; i < m->size; i++){
        m->data[i] = fn(m->data[i]);
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


//--  --  --  --  --  --  --  Functions  --  --  --  --  --  --  --//


double sigmoid(double x){
    return 1.0 / (1 + exp(-x));
}


//--  --  --  --  --  --  --  Neural Network  --  --  --  --  --  --  --//


//-- -- -- -- 2 -> 2 -> 1 -- -- -- --//
typedef struct {
    Matrix W1;
    Matrix b1;

    Matrix W2;
    Matrix b2;

    Matrix z1;
    Matrix a1;

    Matrix z2;
    Matrix a2;
} Network;

Network create_network(){
    Network n;

    n.W1 = mat_create_random(2,2);
    n.b1 = mat_create_random(2,1);

    n.W2 = mat_create_random(1,2);
    n.b2 = mat_create_random(1,1);

    return n;
}

//wow this looks absolutely garbage...
//Need to save these somewhere for later for backpropogation
//Might be worth using another external file to do so. 
//Reading and writing is slow though... might make the imbedded system idea wack. 
double forward(Network *n, Matrix *input){
    Matrix *W1 = &n->W1;
    Matrix temp = mat_multiply(W1, input);
    Matrix *b1 = &n->b1;
    n->z1 = mat_add(&temp, b1);
    mat_free(&temp);
    
    Matrix temp2 = n->z1;
    mat_apply(&temp2, sigmoid);
    n->a1 = temp2;

    Matrix *W2 = &n->W2;
    Matrix temp3 = mat_multiply(W2, &temp2);
    Matrix *b2 = &n->b2;
    n->z2 = mat_add(&temp3, b2);
    mat_free(&temp3);
    mat_free(&temp2);

    Matrix temp4 = n->z2;
    mat_apply(&temp4, sigmoid);
    n->a2 = temp4;
    mat_free(&temp4);

    //I think a2 is just a double atp... return it?
    return (double)n->a2.data[0];
}   


//--  --  --  --  --  --  --  Testing  --  --  --  --  --  --  --//


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

    Matrix m4 = mat_create_random(2,2);
    Matrix *p4 = &m4;
    mat_print(p4);

    mat_print(p3);

    mat_transpose(p3);

    mat_print(p3);

    mat_apply(p3, sigmoid);

    mat_print(p3);

    mat_free(p1);
    mat_free(p2);
    mat_free(p3);
}


//--  --  --  --  --  --  --  Main  --  --  --  --  --  --  --//


void main(int argc, char *argv[]){
    srand(time(NULL)); //update to time(NULL) later. 
    //test_vec();
    //test_mat();
    Network n = create_network();
    Matrix a = mat_create(2,1);
    a.data[0] = 0;
    a.data[1] = 0;

    Matrix b = mat_create(2,1);
    b.data[0] = 0;
    b.data[1] = 1;

    Matrix c = mat_create(2,1);
    c.data[0] = 1;
    c.data[1] = 0;

    Matrix d = mat_create(2,1);
    d.data[0] = 1;
    d.data[1] = 1;


    double value1 = forward(&n, &a);
    double value2 = forward(&n, &b);
    double value3 = forward(&n, &c);
    double value4 = forward(&n, &d);

    printf("Prediction: XOR(0,0) ~ %.4f\n", value1);
    printf("Prediction: XOR(0,0) ~ %.4f\n", value2);
    printf("Prediction: XOR(0,0) ~ %.4f\n", value3);
    printf("Prediction: XOR(0,0) ~ %.4f\n", value4);
}