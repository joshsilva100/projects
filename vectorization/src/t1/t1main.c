#include "declarations.h"

int main() {
	float* A = (float*) _mm_malloc(1024*sizeof(float), 16);
	float* B = (float*) _mm_malloc(1024*sizeof(float), 16);
	for (int i = 0; i < 1024; i++){
		A[i] = 1. / (i+1);
		B[i] = 2. / (i+1);
	}

	unsigned long long start_c, end_c, diff_c;
	start_c = _rdtsc();

	t1(A,B);

	end_c =_rdtsc();
	diff_c = end_c - start_c;
	float giga_cycle = diff_c / 1000000000.0;
	float ret = 0;
	for (int i = 0; i < 1024; i++) {
		ret += A[i];
	}
	printf("t1 took %f giga cycles and the result is: %f\n", giga_cycle, ret);
}
