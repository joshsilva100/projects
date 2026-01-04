#include "declarations.h"

#pragma auto_inline(off)
void t3(float A[512][512]) { 
	float temp1[512][512] __attribute__ ((aligned(16))); 
	float temp2[512][512] __attribute__ ((aligned(16)));
	for (int nl = 0; nl < 1000; nl++) {
 		for (int i = 1; i < 512; i++) {
			for (int j = 1; j < 512; j++) {
				temp1[i][j] = A[i-1][j]; 
				temp2[i][j] = A[i][j-1];
				A[i][j] = temp1[i][j] + temp2[i][j];
	    		}
	    	}
		A[0][0]++;
	}
}
