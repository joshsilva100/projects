#include "declarations.h"
#include "math.h"

void t5(float* restrict A, float* restrict B, float* restrict C, float* restrict D, float* restrict E) {
        float* temp = (float*) memalign(16, LEN5*sizeof(float)); 

        for (int nl = 0; nl < ntimes; nl++) {
                for (int i = 1; i < LEN5; i++) {
                        temp[i] = D[i-1]; 
                        A[i] = temp[i] + (float)sqrt(C[i]);
                }          

                for (int i = 1; i < LEN5; i++) {
                        D[i] = B[i] + (float)sqrt(E[i]);
                } 

                A[0]++;
        }
}


