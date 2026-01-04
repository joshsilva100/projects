#include "declarations.h"

int main() {
	float A[512][512] __attribute__ ((aligned(16)));
	#pragma novector
	for (int i = 0; i < 512; i++) {
		#pragma novector
		for (int j = 0; j < 512; j++) {
			A[i][j] = 0.1/(i+j+1);
		}
	}

	unsigned long long start_c, end_c, diff_c;
	start_c = _rdtsc();

	t3(A);
	
	end_c=_rdtsc();
	diff_c = end_c - start_c;
	float giga_cycle = diff_c / 1000000000.0;
	float ret = 0.;
	#pragma novector
	for (int i = 0; i < 512; i++) {
		#pragma novector
		for (int j = 0; j < 512; j++) {
			ret += A[i][j];
		}
	}
	printf("t3 took %f giga cycles and the result is: %f\n", giga_cycle, ret);
}
