#include <immintrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>

#define ntimes 100000

void t1(float *A, float *B);

void t2(float* restrict A, float* restrict B);

void t3(float A[512][512]);

#define LEN4 256
void t4(float** M1, float** M2, float** M3);

#define LEN5 1024
void t5(float* restrict A, float* restrict B, float* restrict C, float* restrict D, float* restrict E);

#define LEN6 1024
void t6(float* A, float* D);

#define LEN7 32000
void t7init();

void t8_runner(float* restrict A, float* B);
void t8(float* restrict A, float* B, int k);

#define LEN9 1024
void t9(float* A, float* B);
