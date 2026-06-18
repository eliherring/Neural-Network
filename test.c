#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>



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

int digit_helper(int n){
    return floor(log10(llabs(n))) + 1;
}

int number_flip_helper(int n){
    int out = 0;
    int digits = digit_helper(n);
    for(int i = 0; i < digits; i++){
        //Some mod junk...
        out += ((int)((n % (int)(pow(10,(digits - i)))) * (pow(10, i))) / (int)(pow(10, digits - 1))) * (pow(10,i));
    }
    return out;
}

Matrix mat_create_from_file(char *path){
    FILE *file = fopen(path, "r");

    if(file == NULL){
        printf("File was null :(\n");
        //exit / break
    }
    
    
    size_t rows;
    size_t cols;

    int count = 0;
    int c;

    
    int i = 0;
    while((c = fgetc(file)) != 10){
        c = c - 48;
        count += c * pow(10, i);
        i += 1;
    }

    count = number_flip_helper(count);
    rows = (size_t) count;

    count = 0;
    i = 0;
    while((c = fgetc(file)) != 10){
        c = c - 48;
        count += c * pow(10, i);
        i += 1;
    }

    count = number_flip_helper(count);
    cols = (size_t) count;

    Matrix m = mat_create(rows,cols);
    int j = 0;

    //probably a better way to do this!
    while((c = fgetc(file)) != EOF){
        //does this add any value? 
        if(c == '\n' || c == '\r'){
            continue;
        }
        if(c == 'E'){
            break;
        }

        
        if(c == '-'){
            //is a negative number
            double value = 0.0;
            int i = 0;
            
            while((c = fgetc(file)) != 124 && c != EOF){
                if(c != 46){
                    value += (c - 48) * pow(10,i);
                    i++;
                }
            }
            int n = digit_helper((int)value);
            value = number_flip_helper((int) value);
            value = (value) / (pow(10,n - 1));
            value *= -1;
            m.data[j] = value;
            j ++;
        } else {
            //not a negative number.
            double value = 0.0;
            int i = 0;
            
            while((c = fgetc(file)) != 124 && c != EOF){
                if(c != 46){
                    value += (c - 48) * pow(10,i);
                    i++;
                }
            }
            int n = digit_helper((int)value);
            value = number_flip_helper((int) value);
            value = (value) / (pow(10,n));
            m.data[j] = value;
            j ++;
        }
    }
    return m;
}

void mat_save_to_file(char *path, Matrix *m){
    FILE *file = fopen(path, "w");

    if(file == NULL){
        printf("File was null :(\n");
        //exit / break
    }

    size_t cols = m->cols;



    fprintf(file, "%d\n", (int)m->rows);
    fprintf(file, "%d\n", (int)cols);

    for(size_t i = 0; i < m->size; i++){ 
        if(i % cols == cols-1){
            fprintf(file, "%.4f|\n",m->data[i]);
        } else {
            fprintf(file, "%.4f|",m->data[i]);
        }
    }
    fprintf(file, "E");
    fclose(file);
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
            if(m->data[count] < 0){
                //negative
                printf(j + 1 < m->cols ? "%.4f, " : "%.4f", m->data[count]);
            } else {
                //positive
                printf(j + 1 < m->cols ? " %.4f, " : " %.4f", m->data[count]);
            }
            
            count++;
        }
        printf("|\n");
    }
    printf("\n");
}

void main(int argc, char *argv[]){
    Matrix m = mat_create_random(5,5);
    printf("Before\n");
    mat_print(&m);
    char *p = "Matrix.txt";
    mat_save_to_file(p,&m);
    mat_free(&m);
    Matrix a = mat_create_from_file(p);
    printf("After\n");
    mat_print(&a);
}
