#ifndef FUCNTION_H
#define FUCNTION_H
double f1(int t,int n,int i,int j);
int vvod(int t,int n,int m, double *A,char name[20]);
int SolveSystem(int n, double *a, double *x, int my_rank,
                 int Threadcount, double *time);
void synchronize(int total_threads);
//void Substraction(int p, int q, int n, double *A, double z);
void raznost(int p, int q, int n, double *A, double z);  
void *Thread(void *Ptr);               
#endif /*FUCNTION_H*/