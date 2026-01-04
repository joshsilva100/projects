#include "declarations.h"

int main(){
	float* A = (float*) memalign(16, LEN6*sizeof(float));
	float* D = (float*) memalign(16, LEN6*sizeof(float));
		
	for (int i = 0; i < LEN6; i++) {
		A[i] = (float)(i)/(float)LEN6;
		D[i] = (float)(i+3)/(float)LEN6;
	}

	unsigned long long start_c, end_c, diff_c;
	start_c = _rdtsc();

	t6(A,D);

	end_c =_rdtsc(); 
	diff_c = end_c - start_c;
	float giga_cycle = diff_c / 1000000000.0;
	
	float ttt = (float)0.;
	for (int i = 0; i < LEN6; i++)
		ttt += A[i];
	printf("t6 took\t %.2f and the result is %f\t", giga_cycle, ttt);
}
