#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int const dvad = 20;
double f1(int t, int n, int i, int j)
{
    double e;
    double r;
    e = i;
    r = j;
    if (t == 1) {
        if (i > j) {
            return (n - i);
        }
        if (i < j + 1) {
            return (n - j);
        }
    }
    if (t == 2) {
        if (i > j) {
            return (i + 1);
        }
        if (i < j + 1) {
            return (j + 1);
        }
    }
    if (t == 3) {
        return (abs(i - j));
    }
    if (t == 4) {
        return (1 / (e + r + 1));
    }
    return 0;
}

int vvod(int t, int n, int m, double *A, char name[dvad])
{
    int s1 = 0;
    int i;
    int j;
    int q = 0;
    double s = 0;
    double u;

    FILE *fp;

    if (t == 0) {

        fp = fopen(name, "rt");
        if (fp == NULL) {
            return -3;
        }
        fseek(fp, 0, SEEK_END);
        long pos = ftell(fp);
        if (pos > 0) {
            rewind(fp);
        } else {
            fclose(fp);
            return -3;
        }
        if (pos < n * n) {
            fclose(fp);
            return -3;
        }
    }

    if (t == 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                s1 = s1 + 1;
                if (!fscanf(fp, "%lf", &u)) {
                    fclose(fp);
                    q = -3;
                } else {
                    A[m * i + j] = u;
                }
                if (q == -3) {
                    return q;
                }
            }
        }
    }

    if (t == 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                s1 = s1 + 1;
                if (!fscanf(fp, "%lf", &u)) {
                    fclose(fp);
                    return -4;
                }
            }
        }
    }

    if (t != 0) {
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                A[m * i + j] = f1(t, n, i, j);
                if ((j % 2) == 0) {
                    s = s + A[m * i + j];
                }
            }
            A[m * i + n] = s;
            s = 0;
        }
    }
    if (t == 0) {
        fclose(fp);
    }
    return 0;
}