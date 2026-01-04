#include "declarations.h"

void t1(float* restrict A, float* restrict B) {
        __builtin_assume_aligned(A, 16);
        __builtin_assume_aligned(B, 16);

        for (int nl = 0; nl < 1000000; nl++){
                for (int i = 0; i < 1024; i+=2) {
                  A[i+1] = (A[i] + B[i]) / (A[i] + B[i] + 1.);
                }
                B[0]++;
        }
}
