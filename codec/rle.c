/* rle.c
 * Joshua Silva
 * jysilva 
 * ECE468 Spring 2025 
 * RLE
 *
 * Purpose: To compress/decompress a file, specifically golfcore.ppm, using Run-Length-Encoding (RLE)
 *
 * Assumptions: User knows that code uses RLE method
 *
 * The program accepts one command line argument, including the name of the file
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

//Use RLE encoding to compress a file
int main(int argc, char *argv[])
{
	//Declarations to handle compression/decompression & accessing file
	int i, j;
	long int filesize, currsize, newsize;
	unsigned char *indata, *outdata, runCount, A, B;	
	FILE *fpt;
        char *command, *filename, *newfilename, *extension, *newExtension;

	//Make sure user enters correct # of arguments
	if (argc != 3) 
	{
		printf("Program use is ./lab2 command 'filename'\n"); 
		printf("Use command -c for compression\n"); 
		printf("Use command -u for decompression\n");
		exit(0);	
	} 

	//Open the file
	command = argv[1]; //Grab the command
	filename = argv[2]; //Grab the file name
	fpt = fopen(filename, "rb"); 
	
	//Check if we can find file given by user, otherwise exit to prevent segfault
	if (fpt == NULL) 
	{ 
		printf("Invalid open, make sure that file is located in the same folder as executable\n"); 
		return 0;
	} 
	
	//Find size of file, read the contents & allocate memory for them, then close the file
	fseek(fpt, 0, SEEK_END); //Go to the end of the file, no offset!
	filesize = (ftell(fpt)==0)? 0: ftell(fpt)+1; //Set size of file by our current position in the file, account for empty file
	indata = (unsigned char *)malloc(filesize); 
	fseek(fpt, 0, SEEK_SET); //Go back to the beginning of the file to read the data
	fread(indata, 1, filesize, fpt); //Store data into the buffer
	fclose(fpt); 
	newsize = 0; 
	currsize = 0;

	//Compress the file
	if (strcmp("-c", command) == 0) 
	{  
		newExtension = ".rle";
		outdata = (unsigned char *)malloc(2*filesize); //Allocate memory for the outdata, account for worst case negative compression
		j = 0;
		//Read Value A
		runCount = 1; 
		A = indata[0]; 
	        printf("Compressing %s...\n", filename);	

		//Go through entire file and compress
		for (i = 1; i < filesize; i++) 
		{  
			B = indata[i]; //Read Value B 
			
			//No End of File
			if (i != (filesize - 1)) 
			{ 
				//B = indata[i+1]; //Read Value B
				if (A == B) //A == B?
				{ 
				       if(runCount == 255) //runCount at max? 
				       { 
					       //Write and finish
					       outdata[j] = runCount; 
					       outdata[j+1] = A; 
					       A = B; //Let A = B 
					       runCount = 1;
					       j+=2;
				       }	       
				       else 
				       {
					      runCount++; //Keep going
				       }
				}	
				else // A != B 
				{ 
					outdata[j] = runCount; 
					outdata[j+1] = A; 
					A = B;
					runCount = 1; 
					j+=2;
				}
			}

			//Yes End of File
			else 
			{
				outdata[j] = runCount; 
				outdata[j+1] = A; 
				newsize = j + 2; 	
			} 		
		}
	} 
	//Decompress the file
	else if (strcmp("-u", command) == 0) 
	{ 
		newExtension = ".u";
		printf("Decompressing %s...\n", filename);
		outdata = (unsigned char *)malloc(255*(filesize/2)); //Allocate memory for the outdata, account for worst case
		//Go through each pair in compressed file, write out 45 into 5555
		for (i = 0; i < filesize; i+=2) 
		{ 
			//printf("%d %d\n", indata[i], indata[i+1]);
			for (j = 0; j < indata[i]; j++) 
			{ 
				outdata[currsize+j] = indata[i+1];
				newsize++;	
			}  
			currsize = newsize;
		} 
	} 
	//User gave invalid command
	else 
	{ 
		printf("Invalid command, exiting program\n");
		printf("Use command -c for compression\n");
                printf("Use command -u for decompression\n");
		free(indata); 
		return 0;
	}

	//Write to a new compressed/decompressed file 
	extension = strrchr(filename, '.'); //Find location of file extension in filename
	if (extension != NULL) *extension = '\0'; //Get rid of old extension, if it exists 
	newfilename = (char *)malloc(strlen(newExtension) + strlen(filename) + 1); //Allocate enough space to store newfilename, char is 1 byte & include null terminator 
	strcpy(newfilename, filename); 
	strcat(newfilename, newExtension); //Create a new file name to preserve the old file 
	fpt = fopen(newfilename, "wb");
	fwrite(outdata, 1, newsize, fpt); //Write binary data to the file 
	printf("New file saved to %s\n", newfilename);
	fclose(fpt); 

	//Free the data and leave 
	free(newfilename);
	free(indata); 	
	free(outdata); 
	return 0;
}
