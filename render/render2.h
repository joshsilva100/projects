/* render2.h
 * Triangle Rendering Library
 *
 * Purpose: To assist program in rendering triangles from an input .ply file, outputs to a PPM file
 *
 * Assumptions: User knows that render.c needs this library, and understands datatypes & macros
 */

 /* Define macros */
#define COLS 256 
#define ROWS 256 
#define FLOAT_MAX 3.402823466e+38F 
#define BLACK 0 
#define FAR 999999 
#define PI 3.14159265358979323846

/* Declare Classes */

//Class holds the vertices of a 3x1 matrix. Works for vertices with floats
class Vector{ 
public:
	float a, b, c; 

	//Constructors
	Vector() : a(0.0), b(0.0), c(0.0) {} 
	Vector(float _a, float _b, float _c) : a(_a), b(_b), c(_c) {}
};

//Class holds the faces of a 3x1 matrix. Meant to store triangle points
class Face { 
public:
	int v0, v1, v2; 

	//Constructors
	Face() : v0(0), v1(0), v2(0) {}
	Face(int _v0, int _v1, int _v2) : v0(_v0), v1(_v1), v2(_v2) {}
};

/* Function Declarations */
void v_max(float max[3], const std::vector<Vector>& V, int vertices); //Find max vertices for XYZ plane
void v_min(float min[3], const std::vector<Vector>& V, int vertices); //Find min vertices for XYZ plane
void v_center(float center[3], float max[3], float min[3]); //Find center vertices for XYZ plane
float find_e(float max[3], float min[3]); //Find E scalar
void create_rotate(float R[3][3], float R1[3][3], float R2[3][3]); //Create the 3x3 rotation matrix for XYZ plane
void v_rotate(float product[3], float R[3][3], float V[3]); //Apply rotation onto given 3x1 vector
void rotate(float cam[3], float up[3], float X, float Y, float Z); //Handle rotation of camera & up vector
void v_add(float sum[3], float v1[3], float v2[3]); //Add two 3x1 vectors together
void v_sub(float diff[3], float v1[3], float v2[3]); //Subtract two 3x1 vectors together
void v_scal(float product[3], float v[3], float scal); //Apply scalar to 3x1 vector
void v_cross_product(float product[3], float v1[3], float v2[3]); //Cross Product two 3x1 vectors
float v_dot_product(float v1[3], float v2[3]); //Dot Product two 3x1 vectors
void find_vector(float v[3], const std::vector<Vector>& table, int index); //Find vertices for given point on triangle
void v_print(float vector[3]); //Print out 3x1 vector#pragma once
