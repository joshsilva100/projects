/* rma.c
 * Rate Monotonic Analysis
 *
 * Purpose: To test the schedulability of a group of tasks using RMA
 *
 * Assumptions: User understands how RMA works. User will have to change arrays of task values to test different scenarios
 *
 * The program only uses an executable to run. Will run tasks from RMA instructions
 */ 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 

/* Begin RMA */
int main(int argc, char *argv[]) 
{
	/* Declare tasks, leave 0 in the array empty for simplicity in SUMMATION calculation */ 
	int i, j, k, l, num_L; //Handle incrementation in loops (Select each index in task arrays) 
	int n = 8; //Number of tasks 
	double run[9] = {0.00, 1.30, 4.70, 9.00, 23.00, 38.30, 10.00, 3.00, 2.00}; //Runtime in ms
	double period[9] = {0.00, 10.56, 40.96, 61.44, 100.00, 165.00, 285.00, 350.00, 700.00}; //Period in ms
	double block[9] = {0.00, 3.30, 3.30, 9.30, 9.20, 5.20, 5.20, 2.00, 0.00}; //Max Blocking in ms
	double overhead = 0.153; //Overhead = 153 us per task or 0.153 ms per task 
	double sum, lTk; //Keep track of summation & l*Tk
	
	//Make sure user enters correct # of arguments
	if (argc != 1) 
	{
		printf("Program use is ./rma\n"); 
		exit(0);	
	} 
	
	
	/* Begin formula for RMA. Go through each task */
	
	//For i = 1...n
	for (i = 1; i <= n; i++) 
	{
		//For k = 1...i
		for(k = 1; k <= i; k++) 
		{ 
			num_L = (int) floor(period[i]/period[k]); 
			//printf("num_L is %d, floor is %lf\n", num_L, floor(period[i]/period[k]));
			
			//For l = 1...floor(Ti/Tk)
			for (l = 1; l <= num_L; l++) 
			{ 
				sum = 0; //Reset sum for new run 
				lTk = l*period[k]; 
				
				//Summation j= 1...i-1
				for (j = 1; j <= (i - 1); j++) 
				{ 
					//Summation of (Rj + O)*ceiling(lTk/Tj)
					sum += (run[j] + overhead) * ceil(lTk / period[j]);    
				} 
				sum += run[i] + block[i]; //Summation is done, add in Ri + Bi 
				//printf("Sum is %lf, lTk is %lf\n", sum, lTk);
				
				if(sum <= lTk) printf("(k, l) = %i, %i\n", k, l); //printf("i=%i (k=%i l=%i) <= lTk = %lf.....SCHEDULABLE\n", i, k, l, lTk); 
				//else printf("i=%i (k=%i l=%i) <= lTk = %lf.....NOT SCHEDULABLE\n", i, k, l, lTk);
			} 
		}  
	}
	
	return 0;
}
