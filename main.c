#include "function.h"
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
const double q = 0.00001;
const double e92 = 1e9;
const double e93 = 1e+9;
const int big = 10000;
const int dvadz = 20;
const int c6 = 6;
const int c5 = 5;
const int c4 = 4;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double currentTimeNano1() //функция для подсчета времени
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)(t.tv_sec * e92 + t.tv_nsec);
}

void vyvod(int t, int n, int m, double *A) //вывод матрицы
{
    int i;
    int j;
    int n1;
    int m1;
    if (t < n) {
        n1 = t;
    } else {
        n1 = n;
    }
    if (t < m) {
        m1 = t;
    } else {
        m1 = m;
    }
    for (i = 0; i < n1; i++) {
        for (j = 0; j < m1; j++) {
            printf("%.3e", A[m * i + j]);
            printf(" ");
        }
        printf("%.3e", A[m * i + n]);
        printf("\n");
        printf(" ");
    }
}

double error(int n, double *X) //норма погрешности
{
    int i;
    int f = 1;
    double s = 0;
    for (i = 0; i < n; i++) {
        s = s + fabs(X[i] - f);
        if (f == 1) {
            f = 0;
        } else {
            f = 1;
        }
    }
    return sqrt(s);
}
double norma(int n, int m, double *A, double *X) //норма невязки
{
    int i;
    int j;
    double max1;
    double max2;
    double s;
    s = 0; //в эту переменную суммирую произведение строки на столбец
    max1 = 0; //максимум для матрицы Ax-b
    max2 = 0; //максимум для матрицы b
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            s = s + (fabs(A[m * i + j])) * (fabs(X[j]));
        }
        s = fabs(s - fabs(A[m * i + n])); //вычитаю элемент матрицы b
        if (s > max1) {
            max1 = s;
        } //проверяю на максимум
        s = 0; //зануляю s для следующего суммирования
    }
    for (i = 0; i < n; i++) {
        if (fabs(A[m * i + n]) > max2) {
            max2 = fabs(A[m * i + n]);
        }
    }
    return max1 / max2; //нахожу итоговую норму
}
typedef struct {
    int n;
    double *a;
    double *b;
    double *x;
    double *time;
    int my_rank;
    int Threadcount;
    int flag;
} ARGS;

void *Solution(void *p_arg) //функция, работающая в задаче с номером my_rank
{
    ARGS *arg = (ARGS *)p_arg;
    if (SolveSystem(arg->n, arg->a, arg->x, arg->my_rank, arg->Threadcount,
                    arg->time) == -4) {
        arg->flag = -4;
    };
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t *threads;
    int kk;
    int j;
    int i;
    int n;
    int m;
    int t;
    int k;
    int v;
    // double y=0;
    char name[dvadz];
    double r;
    double er;
    ARGS *args;

    if (argc < c5) {

        return -1;
    }
    if (argc > c6) {

        return -1;
    }
    for (i = 1; i < c5; i++) {
        for (j = 0; j < 1; j++) {
            if (isdigit(argv[i][j]) == 0) {

                return -1;
            }
        }
    }

    if (argv[1] == NULL) {
        return -1;
    }
    if (argv[2] == NULL) {
        return -1;
    }
    if (argv[3] == NULL) {
        return -1;
    }
    if (argv[4] == NULL) {
        return -1;
    }

    kk = (atoi(argv[1]));
    n = (atoi(argv[2]));
    t = (atoi(argv[3]));
    k = (atoi(argv[4]));

    if (k == 0) {
        if (argv[c5] == NULL) {
            return -1;
        }
        strcpy(name, argv[c5]);
    } else {
        strcpy(name, "xxx");
    }
    if (kk > 4) {
        return -1;
    }
    if (kk == 0) {
        return -1;
    }
    if (n < 1) {
        return -1;
    }
    if (n > big) {
        return -2;
    }

    if (t > n) {
        return -1;
    }
    if (t < 1) {
        return -1;
    }
    if (k > c4) {
        return -1;
    }
    m = n + 1;
    double *A;
    double *B;
    A = (double *)malloc(m * m * sizeof(double));
    B = (double *)malloc(m * m * sizeof(double));
    threads = (pthread_t *)malloc(kk * sizeof(pthread_t));
    args = (ARGS *)malloc(kk * sizeof(ARGS));

    double time[kk];
    double X[n]; //матрица для ответа
    for (i = 0; i < kk; i++) {
        args[i].n = n;
        args[i].a = A;
        args[i].x = X;
        args[i].my_rank = i;
        args[i].Threadcount = kk;
        args[i].flag = 0;
        args[i].time = time;
    }

    for (i = 0; i < n; i++) {
        X[i] = 0;
    } //зануляем матрицу для ответа

    v = vvod(k, n, m, A, name);
    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            B[m * i + j] = A[m * i + j];
        }
    }

    if (v != 0) {
        free(threads);
        free(args);
        free(A);
        free(B);
        return v;
    }

    vyvod(t, n, m, A); //вывод исходной матрицы

    double start1 = currentTimeNano1();
    for (i = 0; i < kk; i++) {
        pthread_create(threads + i, 0, Solution, args + i);
    }

    for (i = 0; i < kk; i++) {
        pthread_join(threads[i], 0);
        if (args[i].flag == -4) {
            printf("\ndet=0\n ");
            free(threads);
            free(args);
            free(A);
            free(B);
            return -4;
        }
    }
    double finish1 = currentTimeNano1();

    for (i = 0; i < kk; i++) {
        printf("Time of thread %d", i);
        printf(":");
        printf(" %lf", time[i] / e93);
        printf("\n");
    }

    printf("Time: %lf\n", (finish1 - start1) / e93); //вывод времени в секундах
    printf("Solution: ");

    for (i = 0; i < t; i++) {
        printf("%.3e", X[i]);
        printf(" ");
    }
    printf("\n");
    r = norma(n, m, B, X); //подсчет нормы
    printf("Residual: %.3e\t", r); //вывод нормы
    printf("\n");
    er = error(n, X);
    if (k != 0) {
        printf("Error: %.3lf\t", er);
    }
    printf("\n");
    free(threads);
    free(args);
    free(A);
    free(B);
    return 0;
}