#include "declarations.h"

#pragma auto_inline(off)
void t8(float* restrict A, float* restrict B, int k)
{
	for (int nl = 0; nl < 10000000; nl++){
		for (int i = 0; i < 1024; i++) {
			A[i*k] += B[i];
		}
		B[0]++;
	}
}

// DO NOT MODIFY THIS FUNCTION
void t8_runner(float* restrict A, float* B) {
	unsigned long long start_c, end_c, diff_c;
	start_c = _rdtsc();

	t8(A, B, 1);

	end_c =_rdtsc();
	diff_c = end_c - start_c;
	float giga_cycle = diff_c / 1000000000.0;
	float ret = 0;
	#pragma novector
	for (int i = 0; i < 1024; i++)
		ret += A[i];
	printf("t8.1 took %f giga cycles and the result is: %f\n", giga_cycle, ret);
	
	start_c = _rdtsc();
	
	t8(A, B, 2);
	
	end_c =_rdtsc();
	diff_c = end_c - start_c;
	giga_cycle = diff_c / 1000000000.0;
	ret = 0;
	#pragma novector
	for (int i = 0; i < 1024; i++)
		ret += A[i];
	printf("t8.2 took %f giga cycles and the result is: %f\n", giga_cycle, ret);
	
	
	
}
