#include "declarations.h"

void t9(float* restrict A, float* restrict B) {
	//#pragma novector	
	for (int nl = 0; nl < ntimes; nl++) {
		for (int i = 1; i < LEN9-1; i++) {
			 A[i] = A[i-1] + B[i];
		}
	}
}
