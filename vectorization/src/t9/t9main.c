#include "declarations.h"

int main() {
	float* A = (float*) memalign(16, LEN9*sizeof(float));
	float* B = (float*) memalign(16, LEN9*sizeof(float));	
	for (int i = 0; i < LEN9; i++) {
		B[i] = (float)(i)/(float)LEN9;
	}

	unsigned long long start_c, end_c, diff_c;
	start_c = _rdtsc();

	t9(A,B);

	end_c = _rdtsc(); 
	diff_c = end_c - start_c;
	float giga_cycle = diff_c / 1000000000.0;
	
	float ttt = (float)0.;
	for (int i = 0; i < LEN9; i++)
		ttt += A[i];
	printf("t9 took\t %.2f and the result is %f\n", giga_cycle, ttt);
}
