/* lzw.c
 * LZW
 *
 * Purpose: To compress/decompress a file, specifically golfcore.ppm, using Lempel-Ziv-Welch (LZW)
 *
 * Assumptions: User knows that code uses LZW method
 *
 * The program accepts one command line argument, including the name of the file
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <assert.h>

#define MAXCODE 65535 
#define MAXPATTERN 100

/* Structure to handle dictionary. Holding codes, patterns, and pattern length  
 * The alphabet is 1 byte [0, 255], will add more patterns later
 */
typedef struct {
    unsigned char data[MAXCODE][MAXPATTERN]; //Create 2D array. Think of as simple table with rows & columns
    int patternLength[MAXCODE]; //Store pattern length 
    unsigned short int numCode; //Number of current codes  
} Dictionary; //For your sanity

//Function declarations 
void init_dict(Dictionary *dict);
unsigned short int find_code(Dictionary *dict, unsigned char A[MAXPATTERN]); 
int d_find_code(Dictionary *dict, unsigned short int code); 
void add_dict(Dictionary *dict, unsigned char A[MAXPATTERN], int pLength); 
void empty(unsigned char A[MAXPATTERN]); 
void compress(FILE *fpt, char *filename, Dictionary *dict); 
void decompress(FILE *fpt, char *filename, Dictionary *dict); 

//Initialize the dictionary with 0-255 to represent one byte values 
void init_dict(Dictionary *dict) 
{ 
	assert (dict != NULL); 

	//Set everything to empty in dictionary 
	for (int i = 0; i < MAXCODE; i++) 
	{ 
		empty(dict->data[i]);
		dict->patternLength[i] = 0;	
	}
	//Initalize 0-255 & keep track of size 
	dict->numCode = 255;
	for (int i = 0; i < 256; i++) 
	{ 
		dict->data[i][0] = i; //Code and Pattern. 1 byte for now at [0] 
		dict->patternLength[i] = 1; //Pattern Length 
	} 
	
	/*for (int i = 0; i < 256; i++) 
	{ 
		printf("%d Size is %d\n", dict->data[i][0], dict->patternLength[i]); 
	}*/
	
} 

//Find if the code is in the dictionary, and return it back. Return 65535(not possible for index) if no code found
unsigned short int find_code(Dictionary *dict, unsigned char A[MAXPATTERN]) 
{ 
	unsigned short int i = 0;
        unsigned short int code;  	
        int foundCode;
	//printf("Number of codes is %u, find this pattern with length %d\n", (dict->numCode), patternLength);	

	//P+C in dict?
        while ((i <= (dict->numCode)) && (foundCode != 0))
        {
		//Go through entire pattern and check if anything doesn't match
                foundCode = memcmp(dict->data[i], A, MAXPATTERN); //Returns 0 if match
		if (foundCode == 0) code = i;  		
		i++; 
	}
	if (foundCode != 0) code = MAXCODE; //If not found, set code to max possible value
	//printf("	We found code at %d\n", code);
	return code; //Return if code was found or not  
} 

//Find if the code is in the dictionary, and return it back. Return an impossible code if none found
int d_find_code(Dictionary *dict, unsigned short int code)
{
        unsigned short int i = 0;
        int foundCode = 0;
        //printf("Number of codes is %u\n", (dict->numCode));

        //Code in dict?
        while ((i <= (dict->numCode)) && (foundCode == 0))
        {
		//Code is in dictionary 
		if (code == i) 
		{
		       foundCode = 1;	
		}
		i++;
        }
        	
        return foundCode; //Return 0 for not found, 1 for found
}

//Add new code to the dictionary 
void add_dict(Dictionary *dict, unsigned char A[MAXPATTERN], int pLength) 
{ 
	assert(pLength <= MAXPATTERN); //Hard to keep track of all lengths, double check that no impossible patterns are here
	//printf("Adding new code with pattern length %d\n", pLength);
	unsigned short int temp_numCode; 
	if((dict->numCode) < (MAXCODE-2)) 
	{
		(dict->numCode)++; //Indicate that we're adding a new code 
	        temp_numCode = (dict->numCode); //Store new code we're inserting to
		(dict->patternLength[temp_numCode]) = pLength;
		//printf("New pattern length is %d", dict->patternLength[temp_numCode]);

		//Insert all bytes for pattern into the new code
		memcpy(dict->data[temp_numCode], A, MAXPATTERN); 
	}
} 

//Empty out 100 byte pattern
void empty(unsigned char A[MAXPATTERN]) 
{ 
	for (int i = 0; i < MAXPATTERN; i++) 
	{
	       A[i] = '\0';	
	}
}  

//Write to new file to output pattern. No more output buffer
void output_code(char *filename, unsigned short int code)
{
        assert(code < MAXCODE); //Make sure possible code
	FILE *fpt; 
	//printf("Outputting code %d\n", code);

        //Write to a new compressed/decompressed file
        fpt = fopen(filename, "ab");
        fwrite(&code, sizeof(code), 1, fpt); //Write binary data to the file
        fclose(fpt);
}

//Write to new file to output pattern. No more output buffer
void output_pattern(char *filename, unsigned char P[MAXPATTERN], int patternLength) 
{
	assert(patternLength <= MAXPATTERN); //Make sure possible pattern
	FILE *fpt; 

	/*for (int i = 0; i < patternLength; i++) 
	{ 
		printf("Outputting pattern %d\n", P[i]); 
	}*/

	//Write to a new compressed/decompressed file
        fpt = fopen(filename, "ab");
        fwrite(P, 1, patternLength, fpt); //Write binary data to the file
        fclose(fpt); 
}

//Compress the patterns from the uncompress filed. Take in 1 byte patterns, shoot out 2 byte codes 
void compress(FILE *fpt, char *filename, Dictionary *dict) 
{ 
        //Declarations to handle compression & accessing file
	int P_Length, PC_Length;
        long int i, filesize;
        unsigned short int code;
        unsigned char *indata, P[MAXPATTERN], PC[MAXPATTERN], C;
        char *newfilename, *extension, *newExtension; 

	fseek(fpt, 0, SEEK_END); //Go to the end of the file, no offset!
        filesize = ftell(fpt); //Set size of file by our current position in the file
        indata = (unsigned char *)malloc(filesize); 
        fseek(fpt, 0, SEEK_SET); //Go back to the beginning of the file to read the data
        fread(indata, 1, filesize, fpt); //Store our 1 byte patterns into input buffer
        fclose(fpt);

	 //Handle new file name
         printf("Compressing %s...\n", filename);
         newExtension = ".lzw";
         extension = strrchr(filename, '.'); //Find location of file extension in filename
         if (extension != NULL) *extension = '\0'; //Get rid of old extension, if it exists
         newfilename = (char *)malloc(strlen(newExtension) + strlen(filename) + 1); //Allocate enough space to store newfilename, char is 1 byte & include null terminator
         strcpy(newfilename, filename);
         strcat(newfilename, newExtension); //Create a new file name to preserve the old file

         //Start LZW Compression
         empty(P); //Let P(preview) = empty
         P_Length = 0; //Keep track of our preview's length.

         //Go through entire file and compress
         for (i = 0; i < filesize; i++)
         {
		 empty(PC); //Make sure P+C is empty before each use
                 PC_Length = 0;
                 C = indata[i]; //Read C(current)
                 //printf("C = %d\n", C);

                 //P+C. Don't go over max length. Our Pattern (100)
                 if (P_Length < MAXPATTERN)
                 {
			 memcpy(PC, P, MAXPATTERN); //Add P first
                         PC_Length = P_Length; //Now we know length of P
                         PC[PC_Length] = C; //Add in C
                         //printf("C is %d\n", PC[PC_Length]);
                         PC_Length++; //Assign new length (Account for C's length)
		 }

                 //P+C in dict?
                 //printf("Grab code for P+C\n");
                 code = find_code(dict, PC);
                 //printf("Code is %d\n", code);
		 
		 //No, not in dictionary
                 if (code == MAXCODE)
                 {
			 //printf("Could not find code\n");
                         //printf("Grab code for P\n");
                         code = find_code(dict, P); //Grab code for P
                         output_code(newfilename, code);//Output code for P
                         if (PC_Length < MAXPATTERN) add_dict(dict, PC, PC_Length); //Add P+C to dictionary
                         empty(P); //Empty out old pattern
                         P[0] = C; //P = C
                         P_Length = 1; //Set new length for P
		 } 
		 
		 //Yes, in dictionary
                 else
                 {
			 memcpy(P, PC, MAXPATTERN); //Let P = P+C
                         P_Length = PC_Length;
		 }
		 
		 //More data?
                 if(i == (filesize - 1))
		 {
			 //printf("Filesize is %ld, i is %d\n", filesize, i);
			 //printf("End of file\n");
			 code = find_code(dict, P); //Grab code for P
			 output_code(newfilename, code);//No more data, output code for P
		 }
	 } 

	 //Let user know what their new file saved to
         printf("New file saved to %s\n", newfilename); 

	 //Free the data & leave
	 free(newfilename); 
	 free(indata);
} 

//Decompress the codes from the compressed file. Take in 2 byte code, shoot out 1 byte pattern
void decompress(FILE *fpt, char *filename, Dictionary *dict) 
{ 
	//Declarations to handle decompression & accessing file
        int C_Length, P_Length, XZ_Length;
        long int i, filesize;
        unsigned short int *indata, P, C;
        unsigned char c_pattern[MAXPATTERN], p_pattern[MAXPATTERN], X[MAXPATTERN], XZ[MAXPATTERN], Z;
        char *newfilename, *extension, *newExtension;

	fseek(fpt, 0, SEEK_END); //Go to the end of the file, no offset!
        filesize = ftell(fpt); //Set size of file by our current position in the file
        indata = (unsigned short int *)malloc(filesize);	
        fseek(fpt, 0, SEEK_SET); //Go back to the beginning of the file to read the data
        fread(indata, 2, (filesize/2), fpt); //Store our 2 byte codes into input buffer
        fclose(fpt); 

	//Handle new file name
	printf("Decompressing %s...\n", filename);
        newExtension = ".u";
        extension = strrchr(filename, '.'); //Find location of file extension in filename
        if (extension != NULL) *extension = '\0'; //Get rid of old extension, if it exists
        newfilename = (char *)malloc(strlen(newExtension) + strlen(filename) + 1); //Allocate enough space to store newfilename, char is 1 byte & include null terminator
        strcpy(newfilename, filename);
        strcat(newfilename, newExtension); //Create a new file name to preserve the old file 
        //printf("Amount of codes %ld\n", (filesize/2));

	//Begin Decompression 
	/*for(i = 0; i<(filesize/2);i++) 
	{
	       printf("Inside compressed file %d\n", indata[i]);	
	}*/	
	C = 0; 
	P = 0;
	C_Length = 0;
        P_Length = 0;
        XZ_Length = 0;

	//Read C(current)
        C = indata[0]; 
        //printf("Initial C code is %d\n", C);	
	if(d_find_code(dict,C)) 
	{
		memcpy(c_pattern, dict->data[C], MAXPATTERN); 
		C_Length = dict->patternLength[C];	
	}
        output_pattern(newfilename, c_pattern, C_Length); //Output pattern for C
	
	//Go through each pair in compressed file
        for (i = 1; i < (filesize/2); i++)
        {
		//printf("Going through %ld, is is %ld\n", filesize, i);
		//Empty out all temporary patterns
		empty(c_pattern); 
		empty(p_pattern);
		empty(X); 
		empty(XZ);	

		//Let P(preview) = C
	        P = C; 
		//printf("P code is %d\n", P); 
		if(d_find_code(dict, P)) 
		{ 
			memcpy(p_pattern, dict->data[P], MAXPATTERN); //Copy over pattern from given code 
			P_Length = dict->patternLength[P];		
		}

		C = indata[i]; //Read C(current)  
		//printf("C code is %d\n", C); 
		//Is C in dictionary?
		if(d_find_code(dict, C)) 
		{  
			memcpy(c_pattern, dict->data[C], MAXPATTERN); //Copy over pattern from given code 
			C_Length = dict->patternLength[C]; //Grab pattern length
		}
		
		//Not in dictionary
                if ((d_find_code(dict,C)) == 0)
		{
			memcpy(X, p_pattern, MAXPATTERN); //Let X = pattern for P
			Z = p_pattern[0]; //Let Z = 1st char of pattern for P
			memcpy(XZ, X, MAXPATTERN);
			XZ_Length = P_Length;
			XZ[XZ_Length] = Z; //Get X+Z
			XZ_Length++; //Account for new length with additional Z
			output_pattern(newfilename, XZ, XZ_Length); //Output X+Z
			if (XZ_Length < MAXPATTERN) add_dict(dict, XZ, XZ_Length); //Add X+Z to dictionary
		}
		
		//Yes, in dictionary
		else
		{
			output_pattern(newfilename, c_pattern, C_Length); //Output pattern for C 
			memcpy(X, p_pattern, MAXPATTERN); //Let X = pattern for P
			Z = c_pattern[0]; //Let Z = 1st char of pattern for C
			memcpy(XZ, X, MAXPATTERN);
			XZ_Length = P_Length;
			XZ[XZ_Length] = Z; //Get X+Z
			XZ_Length++; //Account for new length with additional Z
			if (XZ_Length < MAXPATTERN) add_dict(dict, XZ, XZ_Length); //Add X+Z to dictionary
		}
		
		//End of file...DONE
	}

	//Let user know what their new file saved to
        printf("New file saved to %s\n", newfilename);

	//Free the data & leave
        free(newfilename);
        free(indata);
}

//Use LZW algorithm to compress/decompress a file
int main(int argc, char *argv[])
{
	//Declarations to handle compression/decompression & accessing file 	
	Dictionary *dict;
	FILE *fpt;
        char *command, *filename;

	//Make sure user enters correct # of arguments
	if (argc != 3) 
	{
		printf("Program use is ./lzw command 'filename'\n"); 
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
	
	//Initialize dictionary with 0-255 for one byte patterns 
	dict = (Dictionary *)malloc(sizeof(Dictionary)); //Allocate memory for our new dictionary
	init_dict(dict); //Initialize our dictionary

	//Compress the file
	if (strcmp("-c", command) == 0) 
	{  
	        compress(fpt, filename, dict); //Let function handle compression due to differences in input data from decompression
	} 
	
	//Decompress the file
	else if (strcmp("-u", command) == 0) 
	{ 
		decompress(fpt, filename, dict); //Let function handle decompression due to differences in input data from compression 
	} 
	
	//User gave invalid command
	else 
	{ 
		printf("Invalid command, exiting program\n");
		printf("Use command -c for compression\n");
                printf("Use command -u for decompression\n"); 
		return 0;
	} 

	//Free the data and leave 	
	free(dict); 
	return 0;
}
