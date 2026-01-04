#include "declarations.h"

void t6(float* restrict A, float* restrict D) {
        //#pragma novector
        float* temp = (float*) memalign(16, LEN5*sizeof(float));
        for (int nl = 0; nl < ntimes; nl++) {
                for (int i = 0; i < LEN6-1; i++) {
                        temp[i] = D[i];
                        A[i] = temp[i] + (float)1.0;
                        //D[i+1] = A[i] + (float) 2.0;
                }

                for (int i = 0; i < LEN6-1; i++) {
                       D[i+1] = A[i] + (float) 2.0;
                }
        }
}
