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
    double scale = 1.0 / sqrt((double)cols); //Xavier/Glorot initialization
    for(size_t i = 0; i < p->size; i++){
        p->data[i] = (((double)rand() / (double)RAND_MAX) * 2.0 - 1.0) * scale;
    }
    return m;
}

void mat_save_to_file(char *path, Matrix *m){
    FILE *file = fopen(path, "wb");
    if(file == NULL){
        printf("Could not open file for writing: %s\n", path);
        return;
    }

    fwrite(&m->rows, sizeof(size_t), 1, file);
    fwrite(&m->cols, sizeof(size_t), 1, file);
    fwrite(m->data, sizeof(double), m->size, file);

    fclose(file);
}

Matrix mat_create_from_file(char *path){
    FILE *file = fopen(path, "rb");
    Matrix m = {0};

    if(file == NULL){
        printf("Could not open file for reading: %s\n", path);
        return m;
    }

    size_t rows, cols;
    fread(&rows, sizeof(size_t), 1, file);
    fread(&cols, sizeof(size_t), 1, file);

    m = mat_create(rows, cols);
    fread(m.data, sizeof(double), m.size, file);

    fclose(file);
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

void mat_add_inplace(Matrix *dst, const Matrix *a, const Matrix *b){
    if(a->rows == b->rows && a->cols == b->cols){
        for(size_t i = 0; i < a->size; i++){
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

Matrix mat_transpose_new(const Matrix *m){
    Matrix t = mat_create(m->cols, m->rows);
    for(size_t i = 0; i < m->rows; i++){
        for(size_t j = 0; j < m->cols; j++){
            t.data[(j * m->rows) + i] = m->data[(i * m->cols) + j];
        }
    }
    return t;
}

void mat_sub_inplace(Matrix *dst, const Matrix *a, const Matrix *b){
    if(a->rows == b->rows && a->cols == b->cols){
        for(size_t i = 0; i < a->size; i++){
            dst->data[i] = a->data[i] - b->data[i];
        }
    } else {
        printf("Cannot subtract matrices - dimension mismatch.\n");
    }
}


//--  --  --  --  --  --  --  Functions  --  --  --  --  --  --  --//


double sigmoid(double x){
    return 1.0 / (1 + exp(-x));
}

double sigmoid_deriv(double a){
    //a is expected to be sigmoid(x)
    return a * (1.0 - a);
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

    n.z1 = mat_create(2,1);
    n.a1 = mat_create(2,1);
    n.z2 = mat_create(1,1);
    n.a2 = mat_create(1,1);

    return n;
}

void network_save(Network *n, const char *prefix){
    char path[256];

    snprintf(path, sizeof(path), "%s_W1.bin", prefix);
    mat_save_to_file(path, &n->W1);
    snprintf(path, sizeof(path), "%s_b1.bin", prefix);
    mat_save_to_file(path, &n->b1);
    snprintf(path, sizeof(path), "%s_W2.bin", prefix);
    mat_save_to_file(path, &n->W2);
    snprintf(path, sizeof(path), "%s_b2.bin", prefix);
    mat_save_to_file(path, &n->b2);
}

Network network_load(const char *prefix){
    Network n;
    char path[256];

    snprintf(path, sizeof(path), "%s_W1.bin", prefix);
    n.W1 = mat_create_from_file(path);
    snprintf(path, sizeof(path), "%s_b1.bin", prefix);
    n.b1 = mat_create_from_file(path);
    snprintf(path, sizeof(path), "%s_W2.bin", prefix);
    n.W2 = mat_create_from_file(path);
    snprintf(path, sizeof(path), "%s_b2.bin", prefix);
    n.b2 = mat_create_from_file(path);

    n.z1 = mat_create(n.b1.rows, 1);
    n.a1 = mat_create(n.b1.rows, 1);
    n.z2 = mat_create(n.b2.rows, 1);
    n.a2 = mat_create(n.b2.rows, 1);

    return n;
}

double forward(Network *n, Matrix *input){
    // z1 = W1 * input + b1
    Matrix temp = mat_multiply(&n->W1, input);
    mat_add_inplace(&n->z1, &temp, &n->b1);
    mat_free(&temp);

    // a1 = sigmoid(z1)
    mat_copy(&n->a1, &n->z1);
    mat_apply(&n->a1, sigmoid);

    // z2 = W2 * a1 + b2
    Matrix temp2 = mat_multiply(&n->W2, &n->a1);
    mat_add_inplace(&n->z2, &temp2, &n->b2);
    mat_free(&temp2);

    // a2 = sigmoid(z2)
    mat_copy(&n->a2, &n->z2);
    mat_apply(&n->a2, sigmoid);

    return n->a2.data[0];
}

void backward(Network *n, Matrix *input, double target, double lr){
    // ---- output layer ----
    double error = n->a2.data[0] - target;
    double d2 = error * n->a2.data[0] * (1.0 - n->a2.data[0]);

    Matrix delta2 = mat_create(1,1);
    delta2.data[0] = d2;

    Matrix a1_T = mat_transpose_new(&n->a1);        // 1x2
    Matrix dW2 = mat_multiply(&delta2, &a1_T);       // 1x1 * 1x2 = 1x2
    mat_free(&a1_T);

    Matrix dW2_scaled = mat_create(dW2.rows, dW2.cols);
    mat_copy(&dW2_scaled, &dW2);
    mat_scalar(&dW2_scaled, lr);
    mat_sub_inplace(&n->W2, &n->W2, &dW2_scaled);
    mat_free(&dW2);
    mat_free(&dW2_scaled);

    Matrix db2_scaled = mat_create(1,1);
    db2_scaled.data[0] = lr * delta2.data[0];
    mat_sub_inplace(&n->b2, &n->b2, &db2_scaled);
    mat_free(&db2_scaled);

    // ---- hidden layer ----
    Matrix W2_T = mat_transpose_new(&n->W2);         // 2x1
    Matrix backprop1 = mat_multiply(&W2_T, &delta2); // 2x1 * 1x1 = 2x1
    mat_free(&W2_T);

    Matrix sp1 = mat_create(n->a1.rows, n->a1.cols);
    mat_copy(&sp1, &n->a1);
    mat_apply(&sp1, sigmoid_deriv);

    Matrix delta1 = mat_create(n->a1.rows, 1);
    mat_hadamard(&delta1, &backprop1, &sp1);
    mat_free(&backprop1);
    mat_free(&sp1);
    mat_free(&delta2);

    Matrix input_T = mat_transpose_new(input);       // 1x2
    Matrix dW1 = mat_multiply(&delta1, &input_T);    // 2x1 * 1x2 = 2x2
    mat_free(&input_T);

    mat_scalar(&dW1, lr);
    mat_sub_inplace(&n->W1, &n->W1, &dW1);
    mat_free(&dW1);

    Matrix db1_scaled = mat_create(delta1.rows, delta1.cols);
    mat_copy(&db1_scaled, &delta1);
    mat_scalar(&db1_scaled, lr);
    mat_sub_inplace(&n->b1, &n->b1, &db1_scaled);
    mat_free(&db1_scaled);
    mat_free(&delta1);
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
    Matrix inputs[4];
    inputs[0] = mat_create(2,1);
    inputs[0].data[0] = 0; inputs[0].data[1] = 0;

    inputs[1] = mat_create(2,1);
        inputs[1].data[0] = 0; inputs[1].data[1] = 1;

    inputs[2] = mat_create(2,1);
    inputs[2].data[0] = 1; inputs[2].data[1] = 0;

    inputs[3] = mat_create(2,1);
    inputs[3].data[0] = 1; inputs[3].data[1] = 1;

    double targets[4] = {0, 1, 1, 0};

    Network n = create_network();

    printf("\n--- Before training ---\n");
    for(int i = 0; i < 4; i++){
        double pred = forward(&n, &inputs[i]);
        printf("Input (%.0f, %.0f) -> Predicted: %.4f, Target: %.0f\n",
            inputs[i].data[0], inputs[i].data[1], pred, targets[i]);
    }

    for(int epoch = 0; epoch < 1000000; epoch++){
        double total_loss = 0.0;

        for(int i = 0; i < 4; i++){
            double pred = forward(&n, &inputs[i]);
            backward(&n, &inputs[i], targets[i], 0.5);

            double err = pred - targets[i];
            total_loss += err * err;
        }

        if(epoch % 1000 == 0){
            //printf("Epoch %d: MSE = %.6f\n", epoch, total_loss / 4.0);
        }
    }

    printf("\n--- After training ---\n");
    for(int i = 0; i < 4; i++){
        double pred = forward(&n, &inputs[i]);
        printf("Input (%.0f, %.0f) -> Predicted: %.4f, Target: %.0f\n",
            inputs[i].data[0], inputs[i].data[1], pred, targets[i]);
    }

    

    network_save(&n, "xor2/xor");

}