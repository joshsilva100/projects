#include "declarations.h"

int main(){
	float* A = (float*) _mm_malloc(1024*sizeof(float), 16);
	float* B = (float*) _mm_malloc(1024*sizeof(float), 16);
	for (int i = 0; i < 1024; i++){
		A[i] = 1. / (i+1);
		B[i] = 2. / (i+1);
	}

        t8_runner(A, B);        
}
