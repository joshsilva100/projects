/* display16.cpp
 * Joshua Silva 
 * jysilva 
 * ECE468 Spring 2025 
 * Lab1 
 * 
 * Purpose: To take a .ppm file that is either 8 bit or 24 bit, and convert it to display to a 16 bit display
 * 
 * Assumptions: User knows that the program is meant for .ppm files with P5/P6 headers. User has a 16 bit display
 * 
 * The program accepts one command line arguments, including the name of the file
 */ 

#include <windows.h> 
#include <stdio.h> 
#include <stdlib.h>  
#include <string.h> 

LRESULT CALLBACK EventProcessor (HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{ return(DefWindowProc(hWnd,uMsg,wParam,lParam)); }

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,int nCmdShow)
{
WNDCLASS		wc;
HWND			WindowHandle;
int			ROWS,COLS, max, i, bytes, red, green, blue; 
int j = 0;
unsigned char	*displaydata, *filedata; 
FILE *fpt;//, *fptr; 
char *filename, header[80]; 
BITMAPINFO		*bm_info;
HDC			hDC;

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)EventProcessor;
wc.cbClsExtra=wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=NULL; 
wc.lpszMenuName=NULL;
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszClassName=L"Image Window Class";
if (RegisterClass(&wc) == 0)
  exit(0);

/* My work */ 

//Make sure user enters correct # of arguments 
if (__argc != 2) //Trying to humor Windows by using some of their new variables, they don't make it easy to get argc
{ 
	printf("Program use is ./lab1 'filename'\n");
	exit(0);
} 

//Open the file, and grab the relevant data within
filename = lpCmdLine; //Grab the argument, lpCmdLine is the same as argv[1] or __argv
fpt = fopen(filename, "rb"); //Open the file 
if (fpt == NULL) 
{ 
	printf("Invalid open, make sure that file is located in the same folder as executable\n");
	exit(0);
}
fscanf(fpt, "%s %d %d %d ", header, &COLS, &ROWS, &max); //Grab the header
bytes = (strcmp(header, "P6") == 0)?3:1; //Determine how many bytes per pixel
filedata = (unsigned char *)calloc(bytes*ROWS*COLS, sizeof(unsigned char)); //Allocate memory for the filedata 
fread(filedata, 1, bytes*ROWS*COLS, fpt); //Store file data into 'filedata'
fclose(fpt); //Close the file

displaydata = (unsigned char *)calloc(2*ROWS*COLS, sizeof(unsigned char)); //Allocate memory for the displaydata

//Convert the data, either 8 or 24 bit, to display to our 16-bit monitor 
if (bytes == 1) 
{ 
	displaydata = filedata; //Greyscale image conversion is easy, set equal to filedata & let colormap handle the rest
} 
else if (bytes == 3) 
{  
	for (i = 0; i < (3*ROWS*COLS); i += 3, j += 2) 
	{ 
		// X555 Method, each color gets 5 bits and lower byte has a don't care
		red = (filedata[i]>>3); 
		green = (filedata[i+1]>>3); 
		blue = (filedata[i+2]>>3); 
		//Now handle placing into both bytes of displaydata 
		displaydata[j+1] = (red<<2) | (green>>3); 
		displaydata[j] = (green<<5) | (blue);
	} 
	//fptr = fopen("hello.txt", "w");
	//fprintf(fptr, "Filedata[0] is %d\nFiledata[1] is %d\nFiledata[2] is %d\ndisplaydata[0] is %d\ndisplaydata[1] is %d", filedata[0], filedata[1], filedata[2], displaydata[0], displaydata[1]); 
	//fclose(fptr);
} 
else 
{
	printf("Sorry, the program is intended for converting .ppm files with header P5 or P6.\n");
	exit(0);
}

/* Display code */
WindowHandle=CreateWindow(L"Image Window Class",L"ECE468 Lab1",
						  WS_OVERLAPPEDWINDOW,
						  10,10,COLS,ROWS,
						  NULL,NULL,hInstance,NULL);
if (WindowHandle == NULL)
  {
  MessageBox(NULL,L"No window",L"Try again",MB_OK | MB_APPLMODAL);
  exit(0);
  }
ShowWindow (WindowHandle, SW_SHOWNORMAL);

bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
hDC=GetDC(WindowHandle);

/* ... set up bmiHeader field of bm_info ... */ 
bm_info->bmiHeader.biBitCount = (bytes==1)?8:16;  
bm_info->bmiHeader.biHeight = -1 * ROWS; 
bm_info->bmiHeader.biWidth = COLS; 
bm_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);//ROWS*COLS*bytes;  
bm_info->bmiHeader.biPlanes = 1; 
bm_info->bmiHeader.biCompression = BI_RGB; 
bm_info->bmiHeader.biSizeImage = 0;//bytes * ROWS * COLS;

for (i=0; i<256; i++)	/* colormap */
  {
  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
  bm_info->bmiColors[i].rgbReserved=0;
  } 
SetDIBitsToDevice(hDC,0,0,COLS,ROWS,0,0,
			  0, /* first scan line */
			  ROWS, /* number of scan lines, change to rows for handling retina, bridge is a square so doesn't matter either way */
			  displaydata,bm_info,DIB_RGB_COLORS); 
ReleaseDC(WindowHandle,hDC);
free(bm_info); 
MessageBox(NULL,L"Press OK to continue",L"",MB_OK | MB_APPLMODAL); 

//Free the data and leave
free(filedata); 
if (bytes==3) free(displaydata); //Freeing the filedata for greyscale has the same effect as freeing both file/displaydata
exit(0); 
}
