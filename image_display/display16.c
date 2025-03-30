/* display16.c
 * Joshua Silva
 * jysilva 
 * ECE468 Spring 2025 
 * Lab1 
 *
 * Purpose: To take a .ppm file that is either 8 bit or 24 bit, and convert it to display to a 16 bit display
 *
 * Assumptions: User knows that program is meant for .ppm files with P5/P6 headers. User has a 16 bit display.
 *
 * The program accepts one command line argument, including the name of the file
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//Take in an 8 or 24 bit image, convert it to 16 bits, then display it to monitor
int main(int argc, char *argv[])
{
	//Declarations to handle monitor display & accessing file
	Display *Monitor;
	Window ImageWindow;
	GC ImageGC;
	XWindowAttributes Atts;
	XImage *Picture;
	int ROWS,COLS, max, i, bytes, red, green, blue; 
	int j = 0; 
	unsigned char *displaydata, *filedata;	
	FILE *fpt;
        char *filename;
        char header[80];

	/* My work */

	//Make sure user enters correct # of arguments
	if (argc != 2) 
	{
		printf("Program use is ./lab1 'filename'\n"); 
		exit(0);	
	} 

	//Open the file, and grab the relevant data within
	filename = argv[1]; //Grab the file name, intended to handle 'retina.ppm' and 'bridge.ppm'
	fpt = fopen(filename, "rb"); //Open the file, and store it to the file pointer 
	if (fpt == NULL) 
	{ 
		printf("Invalid open, make sure that file is located in the same folder as executable\n"); 
		exit(0);
	} 
	fscanf(fpt, "%s %d %d %d ", header, &COLS, &ROWS, &max); //Grab header info, remember to leave empty space at the end
	bytes = (strcmp(header,"P6") == 0)?3:1; //Check if the incoming file is RGB or not, needed for allocation & conversion method
	filedata = (unsigned char *)calloc(bytes*ROWS*COLS, sizeof(unsigned char)); //Allocate memory for the filedata	
	fread(filedata, 1, bytes*ROWS*COLS, fpt); //Store data into our buffer, 'filedata,' from our current file
	fclose(fpt); //Close access to the file

	displaydata = (unsigned char *)calloc(2*ROWS*COLS, sizeof(unsigned char)); //Allocate memory for the display data
	
	//Convert the data, either 8 or 24 bit, to display to our 16-bit monitor 
	if (bytes == 1) 
	{ 
		//Convert from 8 bit to 16 bit 
		//i = r*COLS+C; 
		//displaydata = (unsigned char *)calloc(2*ROWS*COLS, sizeof(unsigned char)); //Allocate memory for the display data, twice as large as 8-bit image  
		for (i = 0; i < (ROWS*COLS); i++, j+=2) 
		{
			//Take one byte from file 
			red = (filedata[i]>>3); 
			green = (filedata[i]>>2); 
			blue = red; //Since it's from the same byte, red and blue are identical 
			//Handle placing into both bytes of displaydata
			displaydata[j+1] = (red<<3) | (green>>3); //Take in all of red, and first half of green 
		        displaydata[j] = (green<<5) | (blue); //Take in the last half of green, and all of blue	
		} 	
		//printf("The filedata is %d, displaydata[0] is %d & display[1] is %d\n", filedata[500], displaydata[1000], displaydata[1001]); 
	} 
	else if (bytes == 3)
	{ 
		//Convert from 24 bit for 16 bit 
		//i = (r*COLS+c)*3 + RGB(0 for R, 1 for G, 2 for B); 
		//displaydata = (unsigned char *)calloc((2*ROWS*COLS), sizeof(unsigned char)); //Allocate memory for the display data, two/thirds as large as 24-bit image
		for (i = 0; i < (3*ROWS*COLS); i += 3, j+= 2) 
		{
			//Different approach due to 3 bytes from file, assign each byte to corresponding color 
			red = (filedata[i]>>3); 
			green = (filedata[i+1]>>2); 
			blue = (filedata[i+2]>>3); 
			//Now handle placing into both bytes of displaydata
			displaydata[j+1] = (red<<3) | (green>>3); //Take all of red in, and first half of green
			displaydata[j] = (green<<5) | (blue); //Take last half of green, and all of blue
		}
		//printf("The filedata[0] is %d, filedata[1] is %d, filedata[2] is %d, displaydata[0] is %d & display[1] is %d\n", filedata[0], filedata[1], filedata[2], displaydata[0], displaydata[1]);
	} 
	else 
	{
		printf("Sorry, program is intended for converting .ppm files with header P5 or P6.\n");
		exit(0);	
	}

	/* Given Work, handles the calls for monitor display  */

	//Connects to the monitor
	Monitor=XOpenDisplay(NULL);
	if (Monitor == NULL)
	  {
	  printf("Unable to open graphics display\n");
	  exit(0);
	  }

	ImageWindow=XCreateSimpleWindow(Monitor,RootWindow(Monitor,0),
		50,10,		// x,y on screen 
		COLS,ROWS,	// width, height 
		2, 		// border width 
		BlackPixel(Monitor,0),
		WhitePixel(Monitor,0));

	ImageGC=XCreateGC(Monitor,ImageWindow,0,NULL);

	XMapWindow(Monitor,ImageWindow);
	XFlush(Monitor); //Empty out the transfer to flush the streams. Make sure it's there
	while(1)
	  {
	  XGetWindowAttributes(Monitor,ImageWindow,&Atts);
	  if (Atts.map_state == IsViewable /*2*/)
	    break;
	  }

	Picture=XCreateImage(Monitor,DefaultVisual(Monitor,0),
			DefaultDepth(Monitor,0),
			ZPixmap,	// format 
			0,		// offset 
			displaydata,// the data 
			COLS,ROWS,	// size of the image data 
			16,		// pixel quantum (8, 16 or 32) 
			0);		// bytes per line (0 causes compute) 

	//Display memory
	XPutImage(Monitor,ImageWindow,ImageGC,Picture,
			0,0,0,0,	// src x,y and dest x,y offsets 
			COLS,ROWS);	// size of the image data 

	XFlush(Monitor); //Empty out the transfer to flush the streams. Make sure it's there
	sleep(5);
	XCloseDisplay(Monitor); 
	
	//Free the data and leave 
	free(ImageGC); //No more memory leaks :D
	free(Picture);
	free(filedata); 	
	free(displaydata); 
	exit(0);
}
