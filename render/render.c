/* render.c
 * Triangle Rendering
 *
 * Purpose: To render triangles from an input .ply file, outputs to a PPM file
 *
 * Assumptions: User inputs a .ply that plots triangles, so faces always have 3 points. 
 * 		          User knows that camera defaults to <1,0,0>, their input only rotates the camera
 *
 * The program accepts one command line argument, including the name of the file and angles (Ex. 'file.ply 90 -45 20')
 */ 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <math.h> 
#include "render.h"

/* Find max vector
 * max: Given vector to hold maximum x,y,z values 
 * V: A structure holding all the given input vertices 
 * vertices: The amount of vertices in the input file
 */
void v_max(float max[3], vector *V, int vertices) 
{ 
	//Assign min variable to hold temporarily
	float max_A = -FLOAT_MAX; 
	float max_B = -FLOAT_MAX; 
	float max_C = -FLOAT_MAX; 
	
	//Go through all vertices, find max of each element
	for (int i = 0; i < vertices; i++) 
	{  
		if (max_A < V[i].a) max_A = V[i].a; 
		if (max_B < V[i].b) max_B = V[i].b; 
		if (max_C < V[i].c) max_C = V[i].c;
	} 
	
	//Found max, assign into given array
	max[0] = max_A; 
	max[1] = max_B; 
	max[2] = max_C;
} 

/* Find min vector 
 * min: Given vector to hold minimum x,y,z values 
 * V: A structure holding all the given input vertices 
 * vertices: The amount of vertices in the input file
 */
void v_min(float min[3], vector *V, int vertices)
{ 
	//Assign max variable to hold temporarily
	float min_A = FLOAT_MAX; 
	float min_B = FLOAT_MAX; 
	float min_C = FLOAT_MAX; 
	
	//Go through all vertices, find min of each element
	for (int i = 0; i < vertices; i++) 
	{  
		if (min_A > V[i].a) min_A = V[i].a; 
		if (min_B > V[i].b) min_B = V[i].b; 
		if (min_C > V[i].c) min_C = V[i].c;
	} 
	
	//Found min, assign into given array
	min[0] = min_A; 
	min[1] = min_B; 
	min[2] = min_C;
} 

/* Find center vector (min + max)/2 
 * center: Given vector to hold center x,y,z values 
 * max: Given vector to hold maximum x,y,z values 
 * min: Given vector to hold minimum x,y,z values
 */
void v_center(float center[3], float max[3], float min[3]) 
{
	center[0] = (max[0] + min[0])/2; 
	center[1] = (max[1] + min[1])/2; 
	center[2] = (max[2] + min[2])/2; 
} 

/* Find E scalar of bounding box, largest component of <max-min> or largest extent of the three axes
 * max: Given vertex to hold maximum x,y,z values 
 * min: Given vertex to hold minimum x,y,z values
 */
float find_e(float max[3], float min[3]) 
{ 
	//Assign temporary value for e
	float e = -FLOAT_MAX; 
	
	//Grab extents of each axis
	float temp_X = max[0] - min[0]; 
	float temp_Y = max[1] - min[1]; 
	float temp_Z = max[2] - min[2]; 

	//Find largest extent 
	e = temp_X; 
	if (e < temp_Y) e = temp_Y; 
	if (e < temp_Z) e = temp_Z; 

	return e;
}

/* Create Rotation matrix by multiplying a 3x3 matrix by another 3x3 matrix. Output is a 3x3 matrix
 * R: Our output matrix, stores the product of the two input matrices 
 * R1: One of the input matrices. First one R1*R2 
 * R2: One of the input matrices. Second one R1*R2
 */
void create_rotate(float R[3][3], float R1[3][3], float R2[3][3]) 
{ 
	//Go through entire 3x3 matrix. Go through each row at a time
	for (int i = 0; i < 3; i++) 
	{
		//Set to zero to allow for incrementation
		R[i][0] = 0; 
		R[i][1] = 0;
		R[i][2] = 0;

		//Go through each column in the row, adding all the multiplications between the two matrices
		for(int j = 0; j < 3; j++) 
		{ 
			R[i][0] += R1[i][j] * R2[j][0]; 
			R[i][1] += R1[i][j] * R2[j][1];  
			R[i][2] += R1[i][j] * R2[j][2];
		}  
	} 
}

/* Apply rotation by multiplying a 3x3 matrix by a 3x1 matrix. Output is a 3x1 matrix that is rotated around all axes (XYZ)
 * product: Our output matrix, stores the product of the two input matrices
 * R: Input 3x3 matrix. First matrix in the multiplication order
 * V: Input 3x1 matrix. Second matrix in the multiplication order
 */
void v_rotate(float product[3], float R[3][3], float V[3]) 
{ 
	//Go through entire 3x3 matrix. Go through each row at a time
	for(int i = 0; i < 3; i++) 
	{ 
		//Set to zero to allow for incrementation
		product[i] = 0; 

		//Go through each column in the row, adding all the multiplications between the two matrices
		for(int j = 0; j < 3; j++) 
		{ 
			product[i] += R[i][j] * V[j];
		}  
	}
} 

/* Rotate the camera & up vectors by the given degrees 
 * cam: Provided camera vector that will be rotated. Default <1, 0, 0>
 * up: Provided up vector that will be rotated. Default <0, 0, 1> 
 * X: Degrees that camera & up vectors will be rotated about the X axis, given by user input
 * Y: Degrees that camera & up vectors will be rotated about the Y axis, given by user input
 * Z: Degrees that camera & up vectors will be rotated about the Z axis, given by user input
 */
void rotate(float cam[3], float up[3], float X, float Y, float Z) 
{ 
	//Convert degree input into radians for math.h trig functions
	float radX = X * (M_PI / 180); 
	float radY = Y * (M_PI / 180);
	float radZ = Z * (M_PI / 180); 

	//Set up rotation matrix for X, Y, Z
	float Rx[3][3] = {{1, 0, 0}, {0, cos(radX), -sin(radX)}, {0, sin(radX), cos(radX)}}; 
	float Ry[3][3] = {{cos(radY), 0, sin(radY)}, {0, 1, 0}, {-sin(radY), 0, cos(radY)}};
	float Rz[3][3] = {{cos(radZ), -sin(radZ), 0}, {sin(radZ), cos(radZ), 0}, {0, 0, 1}}; 
	
	//Create matrices to store products & old vectors for purposes of matrix multiplication
	float R[3][3], temp_R[3][3], temp_cam[3], temp_up[3]; 
	for (int i = 0; i < 3; i++) //Create temporary arrays to hold old cam/up vectors
	{
		temp_cam[i] = cam[i]; 
		temp_up[i] = up[i]; 
	}
	
	//Find rotation matrix Rz*(Ry*Rx)
	create_rotate(temp_R, Ry, Rx); //Ry * Rx
	create_rotate(R, Rz, temp_R); //Rz * (Ry * Rx) 
	
	//Rotate camera & up vectors around the X-axis, Y-axis, Z-axis
	v_rotate(cam, R, temp_cam); 
	v_rotate(up, R, temp_up); 
} 

/* Add two 3x1 matrices together
 * sum: Our output matrix, stores the sum of the two input matrices
 * v1: Input 3x1 matrix. First matrix in the addition order
 * v2: Input 3x1 matrix. Second matrix in the addition order
 */
void v_add(float sum[3], float v1[3], float v2[3]) 
{ 
	//Go through matrix, add corresponding rows together
	for (int i = 0; i < 3; i++) 
	{ 
		sum[i] = v1[i] + v2[i];
	}
} 

/* Subtract two 3x1 matrices together
 * diff: Our output matrix, stores the difference of the two input matrices
 * v1: Input 3x1 matrix. First matrix in the subtraction order
 * v2: Input 3x1 matrix. Second matrix in the subtraction order
 */
void v_sub(float diff[3], float v1[3], float v2[3]) 
{ 
	//Go through matrix, subtract corresponding rows together
	for (int i = 0; i < 3; i++) 
	{ 
		diff[i] = v1[i] - v2[i];
	}
} 

/* Multiply a 3x1 matrix by a scalar 
 * product: Output 3x1 matrix, result of the scalar being applied to the matrix
 * v: Input 3x1 matrix 
 * scal: Scalar that matrix will be multiplied by
 */
void v_scal(float product[3], float v[3], float scal) 
{ 
	//Go through matrix, multiply each row by scalar
	for (int i = 0; i < 3; i++) 
	{ 
		product[i] = v[i] * scal;
	}	 
} 

/* Cross Multiply two 3x1 matrices using Algebraic Definition
 * product: Result of the cross multiplication (3x1 matrix)
 * v1: Input 3x1 matrix 
 * v2: Input 3x1 matrix
 */
void v_cross_product(float product[3], float v1[3], float v2[3]) 
{ 
	//Apply cross product formula 
	product[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]); //v1[y]*v2[z] - v1[z]*v2[y]
	product[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]); //v1[z]*v2[x] - v1[x]*v2[z]
	product[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]); //v1[x]*v2[y] - v1[y]*v2[x]
} 

/* Dot Multiply two 3x1 matrices
 * v1: Input 3x1 matrix 
 * v2: Input 3x1 matrix 
 * Return product: Result of the dot multiplication (Scalar)
 */
float v_dot_product(float v1[3], float v2[3]) 
{ 
	return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]); 
} 

/* Find vertices for a point on the triangle, depending on the vector index & vector table
 * v: Output 3x1 matrix, will be assigned vertices depending on found vector 
 * table: Table that stores all vertices from .ply file 
 * index: Specific vector that the face from .ply file wants, to assign vertice for triangle edge
 */
void find_vector(float v[3], vector *table, int index) 
{ 
	v[0] = table[index].a; 
	v[1] = table[index].b;
	v[2] = table[index].c;
} 

/* Print out vector for debugging purposes 
 * vector: Vector you want printed out 
 */
void v_print (float vector[3]) 
{
	for (int i = 0; i < 3; i++) 
	{	
		printf("%f ", vector[i]); 
	} 
	printf("\n"); 
}

/* Handle user input & begin Triangle Rendering */
int main(int argc, char *argv[]) 
{
	FILE *fpt; 
	char *filename, *newfilename, *ext, *newExt, header[10], *newHeader; 
	unsigned char pixel[ROWS][COLS];
	int r, c, i, close_tri, vertices, faces; 
	float camera[3], up[3], X, Y, Z, max[3], min[3], center[3]; 
	float E, a, left[3], right[3], top[3], bottom[3], top_left[3], diff[3]; 
	float diff1[3], sum[3], image[3], ABC[3], v0[3], v1[3], v2[3], D, n, d;
	float zBuffer, intersect[3], dot1, dot2, dot3, prod1[3], prod2[3];
	
	//Make sure user enters correct # of arguments
	if (argc != 5) 
	{
		printf("Program use is ./render 'filename' degree1 degree2 degree3\n"); 
		printf("Degrees are for the camera rotation\n");
		exit(0);	
	} 
	
	//Grab user input for filename
	filename = argv[1]; 
	
	/* Parse through .ply file, grab vertices & faces */
	fpt = fopen(filename, "rb"); 
	
	//Check if we can find file given by user, otherwise exit to prevent segfault
	if (fpt == NULL) 
	{ 
		printf("Invalid open, make sure that file is located in the same folder as executable\n"); 
		exit(0);
	} 
	
	//Go through header
	fscanf(fpt, "%s ", header); 
	//Make sure user gives valid file type by reading 1st line in header
	if (strcmp("ply", header) != 0) 
	{ 
		printf("File must be a .ply file, run program again with correct file type\n"); 
		fclose(fpt); 
		exit(0); 
	} 
	fscanf(fpt, "%*[^\n]\n");
	fscanf(fpt, "element vertex %d ", &vertices); //Grab # vertices
	fscanf(fpt, "%*[^\n]\n"); 
	fscanf(fpt, "%*[^\n]\n");
	fscanf(fpt, "%*[^\n]\n"); 
	fscanf(fpt, "element face %d ", &faces); //Grab # faces, or # triangles
	fscanf(fpt, "%*[^\n]\n"); 
	fscanf(fpt, "%*[^\n]\n"); 
	
	//Create vertex & face structs to hold in vertices & faces from .ply file
	vector V[vertices]; 
	face triangle[faces]; 
	
	//Collect vertices 
	for (i = 0; i < vertices; i++) 
	{ 
		fscanf(fpt, "%f %f %f ", &V[i].a, &V[i].b, &V[i].c);
	} 
        
        //Collect faces (or triangles)
        for (i = 0; i < faces; i++)
	{
        	fscanf(fpt, "3 %d %d %d ", &triangle[i].v0, &triangle[i].v1, &triangle[i].v2);
        } 

	fclose(fpt); //Done with parsing 
	
	/* Calculate the bounding box on the vertices */ 
	
	v_max(max, V, vertices); 
	v_min(min, V, vertices); 
	v_center(center, max, min); 
	E = find_e(max, min); 
	
	/* Calculate camera position and orientation */  

	//Grab rotations from user and default camera, convert input into floats
	X = atof(argv[2]); 
	Y = atof(argv[3]);
	Z = atof(argv[4]); 
	
	//Default camera onto X axis (<1,0,0>) 
	camera[0] = 1;
	camera[1] = 0;
	camera[2] = 0; 
	
	//Default up onto Z axis (<0,0,1>)
	up[0] = 0;
	up[1] = 0;
	up[2] = 1; 
	
	//Rotate camera & up vector
	rotate(camera, up, X, Y, Z); 
	
	//Move and scale the camera vector (<camera> = 1.5E<camera> + <center>) 
	v_scal(camera, camera, 1.5 * E); //<camera> = 1.5E*<camera>
	v_add(camera, camera, center); //<camera> = 1.5e*<camera> + center 
	
	/* Determine the 3D coordinates bounding the image */ 	
	
	//Find first left
	v_sub(diff, center, camera); //<diff> = <center> - <camera> 
	v_cross_product(left, up, diff); //<left> = <up> x <center-camera> 
	
	//Find a = ||<left>||
	a = sqrtf((left[0] * left[0]) + (left[1] * left[1]) + (left[2] * left[2])); 
	
	//Find final left
	v_scal(left, left, E/(2*a)); //<left> = E/2a<left> 
	v_add(left, left, center); //<left> = E/2a<left> + center  

	//Find right
	v_cross_product(right, diff, up); //right = <center-camera> x <up> 
	v_scal(right, right, E/(2*a)); //<right> = E/2a<right> 
	v_add(right, right, center); //<right> = E/2a<right> + center 
	
	//Find top 
	v_scal(top, up, E/2); //<top> = E/2<up> 
	v_add(top, top, center); //<top> = E/2<up> + <center> 
	
	//Find bottom 
	v_scal(bottom, up, (-E)/2); //<bottom> = -E/2<up> 
	v_add(bottom, bottom, center); //<bottom> = -E/2<up> + <center> 

	//Find topleft 
	v_scal(top_left, up, E/2); //<topleft> = E/2<up> 
	v_add(top_left, top_left, left); //<topleft> = E/2<up> + <left> 

	/* Determine each pixel r,c in the image */ 
	printf("Rendering...\n");
	for(r = 0; r < ROWS; r++) 
	{
		//Go through each column in the row
		for(c = 0; c < COLS; c++) 
		{
			//Calculate vector coordinates for the image pixel
			v_sub(diff, bottom, top); //<diff> = <bottom - top> 
			v_scal(diff, diff, (float)r/(ROWS-1)); //r/(ROWS-1)<bottom - top> 
			v_sub(diff1, right, left); //<diff1> = <right-left>  
			v_scal(diff1, diff1, (float)c/(COLS-1)); //<diff1> = c/(COLS-1)<right-left> 
			v_add(sum, diff1, diff); //<sum> = c/(COLS-1)<right-left> + r/(ROWS-1)<bottom - top>
			v_add(image, top_left, sum); //<image> = <topleft> + c/(COLS-1)<right-left> + r/(ROWS-1)<bottom - top>  
			
			//Find the plane equation that contains the triangle 
			zBuffer = FAR; //Default the zBuffer to far away for each pixel before checking triangles 
			close_tri = -1; //Default index to impossible value, meant to distinguish if any triangle is found or not
			for (i = 0; i < faces; i++) 
			{ 
				//Find v0, v1, v2 for this triangle 
				find_vector(v0, V, triangle[i].v0); 
				find_vector(v1, V, triangle[i].v1);
				find_vector(v2, V, triangle[i].v2); 

				//Find ABC
				v_sub(diff, v1, v0); //<diff> = <v1-v0> 
				v_sub(diff1, v2, v0); //<diff1> = <v2-v0> 
				v_cross_product(ABC, diff, diff1); //<A,B,C> = <v1-v0> x <v2-v0> 
				
				//Find D 
				v_scal(prod1, ABC, -1); //<prod1> = -<A,B,C>
				D =  v_dot_product(prod1, v0); //D = -<A,B,C> * <v0>
				
				//Find distance along the image pixel ray to the triangle
				n = v_dot_product(prod1, camera) - D; //n = -<A,B,C> * camera - D
				v_sub(diff, image, camera); //<diff> = <image - camera>
				d = v_dot_product(ABC, diff); //d = <ABC> * <image - camera> 
				
				//If ray is parallel to triangle, d near zero, skip
				if(fabs(d) > 1e-6) 
				{ 
					//Find 3D coords <intersect> of ray & plane
					v_scal(diff, diff, n/d); //<diff> = n/d<image-camera> 
					v_add(intersect, camera, diff); //<intersect> = <camera> + n/d<image-camera> 
					
					/* Determine if intersection point lies within triangle, if any dot product is less than 0 (outside!), stop early */
					
					//Dot1
					v_sub(diff, v2, v0); //<diff> = <v2-v0> 
					v_sub(diff1, v1, v0); //<diff1> = <v1-v0> 
					v_cross_product(prod1, diff, diff1); //<prod1> = <v2-v0> x <v1-v0> 
					v_sub(diff, intersect, v0); //<diff> = <intersect-v0> 
					v_cross_product(prod2, diff, diff1); //<prod2> = <intersect-v0> x <v1-v0>
					dot1 = v_dot_product(prod1, prod2); //dot1 = 〈v2 − v0〉 × 〈v1 − v0〉 · 〈intersect − v0〉 × 〈v1 − v0〉 
					
					//Dot2 if previous passes
					if(dot1>=0) 
					{ 
						v_sub(diff, v0, v1); //<diff> = <v0-v1> 
						v_sub(diff1, v2, v1); //<diff1> = <v2-v1> 
						v_cross_product(prod1, diff, diff1); //<prod1> = <v0-v1> x <v2-v1> 
						v_sub(diff, intersect, v1); //<diff> = <intersect-v1> 
						v_cross_product(prod2, diff, diff1); //<prod2> = <intersect-v1> x <v2-v1>
						dot2 = v_dot_product(prod1, prod2); //dot2 = 〈v0 − v1〉 × 〈v2 − v1〉 · 〈intersect − v1〉 × 〈v2 − v1〉
					}
					
					//Dot3 if previous two pass
					if(dot1>=0 && dot2>=0) 
					{ 
						v_sub(diff, v1, v2); //<diff> = <v1-v2> 
						v_sub(diff1, v0, v2); //<diff1> = <v0-v2> 
						v_cross_product(prod1, diff, diff1); //<prod1> = <v1-v2> x <v0-v2> 
						v_sub(diff, intersect, v2); //<diff> = <intersect-v2> 
						v_cross_product(prod2, diff, diff1); //<prod2> = <intersect-v2> x <v0-v2>
						dot3 = v_dot_product(prod1, prod2); //dot3 = 〈v1 − v2〉 × 〈v0 − v2〉 · 〈intersect − v2〉 × 〈v0 − v2〉
					}

					//Proved all dot products, triangle is "seen" 
					if(dot1>=0 && dot2>=0 && dot3>=0) 
					{ 
						//Check if this triangle is closer than other triangles. If so, set this triangle to be closest 
						if(n/d < zBuffer) 
						{
							close_tri = i; //Save triangle index
							zBuffer = n/d; //Save the distance
						}
					}
				} 
			} 

			//Set pixel color depending on triangle found
			if (close_tri < 0) pixel[r][c] = BLACK; //Triangle not found, set to background color
			else pixel[r][c] = 155 + (close_tri%100); //Triangle found, set to greyscale value varied by triangle index 
			//printf("%d", pixel[r][c]);
		}
	}

	/* Handle new file name */
	newExt = ".ppm";
	ext = strrchr(filename, '.'); //Find location of file extension in filename
	if (ext != NULL) *ext = '\0'; //Get rid of old extension, if it exists
	newfilename = (char *)malloc(strlen(newExt) + strlen(filename) + 1); //Allocate enough space to store newfilename, char is 1 byte & include null terminator
	strcpy(newfilename, filename);
	strcat(newfilename, newExt); //Create a new file name to preserve the old file 
	
	/* Write pixel values to new .ppm file */ 
	newHeader = "P5 256 256 255\n";
	fpt = fopen(newfilename, "wb"); 
	fwrite(newHeader, sizeof(char), strlen(newHeader), fpt);
	fwrite(&pixel[0][0], sizeof(unsigned char), ROWS * COLS, fpt);
	fclose(fpt); 
	printf("Outputting to %s\n", newfilename);
	
	//Free any allocated memory and exit
	free(newfilename);
	return 0;
}
