#include "declarations.h"

#pragma auto_inline(off)
void t2(float* restrict A, float* restrict B) {
        //__assume_aligned(A, 16);
        //__assume_aligned(B, 16);

        for (int nl = 0; nl < 10000000; nl++) {
                for (int i = 0; i < 1280 - 5; i+=5) {
                        A[i] = B[i] * A[i];
                        A[i + 1] = B[i] * A[i + 1];
                        A[i + 2] = B[i] * A[i + 2];
                        A[i + 3] = B[i] * A[i + 3];
                        A[i + 4] = B[i] * A[i + 4];
                }
                A[0]++;
        }
}
