#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
const double small = 1e-50;
const double e9 = 1e9;

void synchronize(int total_threads)
{
    /* Объект синхронизации типа mutex */
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    /* Объект синхронизации типа condvar */
    static pthread_cond_t condvar_in = PTHREAD_COND_INITIALIZER;
    /* Объект синхронизации типа condvar */
    static pthread_cond_t condvar_out = PTHREAD_COND_INITIALIZER;
    /* Число пришедших в функцию задач */
    static int threads_in = 0;
    /* Число ожидающих выхода из функции задач */
    static int threads_out = 0;

    /* "захватить" mutex для работы с переменными
       threads_in и threads_out */
    pthread_mutex_lock(&mutex);

    /* увеличить на 1 количество прибывших в
       эту функцию задач */
    threads_in++;

    /* проверяем количество прибывших задач */
    if (threads_in >= total_threads) {
        /* текущий поток пришел последним */
        /* устанавливаем начальное значение
           для threads_out */
        threads_out = 0;

        /* разрешаем остальным продолжать работу */
        pthread_cond_broadcast(&condvar_in);
    } else {
        /* есть еще не пришедшие потоки */

        /* ожидаем, пока в эту функцию не придут
           все потоки */
        while (threads_in < total_threads) {
            /* ожидаем разрешения продолжить работу:
               освободить mutex и ждать сигнала от
               condvar, затем "захватить" mutex опять */
            pthread_cond_wait(&condvar_in, &mutex);
        }
    }

    /* увеличить на 1 количество ожидающих выхода задач */
    threads_out++;

    /* проверяем количество прибывших задач */
    if (threads_out >= total_threads) {
        /* текущий поток пришел в очередь последним */
        /* устанавливаем начальное значение
           для threads_in */
        threads_in = 0;

        /* разрешаем остальным продолжать работу */
        pthread_cond_broadcast(&condvar_out);
    } else {
        /* в очереди ожидания еще есть потоки */

        /* ожидаем, пока в очередь ожидания не придет
           последний поток */
        while (threads_out < total_threads) {
            /* ожидаем разрешения продолжить работу:
               освободить mutex и ждать сигнала от
               condvar, затем "захватить" mutex опять */
            pthread_cond_wait(&condvar_out, &mutex);
        }
    }

    /* "освободить" mutex */
    pthread_mutex_unlock(&mutex);
}

double currentTimeNano()
{
    struct timespec t;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);
    return (double)(t.tv_sec * e9 + t.tv_nsec);
}

void raznost(int p, int q, int n, double *A, double z)
{
    // вычитание строк
    int j;
    int m;
    m = n + 1;

    for (j = 0; j < m; j++) {
        A[m * j + q] = A[m * j + q] - (A[m * j + p]) * z;
    }
}
void perest(int i, int k, int n, double *A)
{
    int j;
    int m;
    double w;
    m = n + 1;
    for (j = 0; j < m; j++) {
        w = A[m * i + j];
        A[m * i + j] = A[m * k + j];
        A[m * k + j] = w;
    }
}

int SolveSystem(int n, double *A, double *X, int my_rank, int Threadcount,
                double *time)
{
    int k;
    int j;
    int m = n + 1;
    int i;
    int p;
    // int j1;
    double b;
    double max;
    double q;
    double time1 = currentTimeNano();
    // double time00=0;
    // double time0;
    // synchronize(Threadcount);
    for (j = 0; j < m; j++) {
        A[m * (m - 1) + j] = (A[m * j + n]);
    }
    synchronize(Threadcount);
    for (k = 0; k < n; k++) {
        synchronize(Threadcount);
        // time0 = currentTimeNano();
        if (my_rank == 0) {
            max = fabs(A[m * k + k]);
            //нахожу строку с главным элементом и ставлю ее в начало
            p = k;
            for (i = k; i < n; i++) {
                if (A[m * k + i] > max) {
                    max = A[m * i + k];
                    p = i;
                }
            }
            for (j = 0; j < m; j++) {
                q = A[m * j + p];
                A[m * j + p] = A[m * j + k];
                A[m * j + k] = q;
            }
            //}
            // time00 = time00+currentTimeNano()-time0;
            // synchronize(Threadcount);
            b = A[m * k +
                  k]; //записываем в b коэффицент при первом символе строки
            if (fabs(b) < small) {

                return -4;
            }
            // synchronize(Threadcount);
            for (j = 0; j < m; j++) {

                A[m * j + k] = (A[m * j + k]) / (b);
            }
        }
        synchronize(Threadcount);

        j = my_rank;
        while (j < n) {

            if (j != k) {
                raznost(k, j, n, A, A[m * k + j]);
            }

            j = j + Threadcount;
        }

        synchronize(Threadcount);
    }
    // synchronize(Threadcount);
    synchronize(Threadcount);
    if (my_rank == 0) {
        for (i = 0; i < n; i++) {
            X[i] = A[m * (m - 1) + i]; //заполнение массива с ответами
        }
    }
    // synchronize(Threadcount);
    time1 = currentTimeNano() - time1;
    time[my_rank] = time1;
    return 0;
    // for (j1 = 0; j1 < n; j1++) {
    //    A[m * k + j1]= A[m * k + j1]+1;
    //    A[m * k + j1]= A[m * k + j1]-1;
    //}
}