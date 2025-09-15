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

	/* Constructors */
	Vector() : a(0.0), b(0.0), c(0.0) {} 
	Vector(float _a, float _b, float _c) : a(_a), b(_b), c(_c) {} 

	/* Member Functions Declarations */ 

	// Add two 3x1 matrices together. Overload + operator
	Vector operator+(const Vector& other) const 
	{
		//Go through matrix, add corresponding rows together
		return Vector(a + other.a, b + other.b, c + other.c);
	}

	// Subtract two 3x1 matrices together
	Vector operator-(const Vector& other) const
	{
		//Go through matrix, subtract corresponding rows together
		return Vector(a - other.a, b - other.b, c - other.c);
	}

	// Multiply a 3x1 matrix by a scalar
	Vector operator*(float scal) const
	{
		//Go through matrix, multiply each row by scalar 
		return Vector(a * scal, b * scal, c * scal);
	}

	/* Cross Multiply two 3x1 matrices using Algebraic Definition
	 * product: Result of the cross multiplication (3x1 matrix)
	 * v1: Input 3x1 matrix
	 * v2: Input 3x1 matrix
	 */
	Vector cross(const Vector& other) const
	{
		//Apply cross product formula 
		Vector product;
		product.a = (b * other.c) - (c * other.b); //v1[y]*v2[z] - v1[z]*v2[y]
		product.b = (c * other.a) - (a * other.c); //v1[z]*v2[x] - v1[x]*v2[z]
		product.c = (a * other.b) - (b * other.a); //v1[x]*v2[y] - v1[y]*v2[x] 
		return product;
	} 

	// Print out vector for debugging purposes
	void print() const
	{
		std::cout << a << ", " << b << ", " << c << std::endl;
	}

};

//Class holds the faces of a 3x1 matrix. Meant to store triangle points
class Face { 
public:
	int v0, v1, v2; 

	//Constructors
	Face() : v0(0), v1(0), v2(0) {}
	Face(int _v0, int _v1, int _v2) : v0(_v0), v1(_v1), v2(_v2) {} 

	void print() const
	{
		std::cout << v0 << ", " << v1 << ", " << v2 << std::endl;
	}
};

/* Function Declarations */ 
float find_e(Vector max, Vector min); //Find E scalar
void create_rotate(float R[3][3], float R1[3][3], float R2[3][3]); //Create the 3x3 rotation matrix for XYZ plane
void v_rotate(Vector product, float R[3][3], Vector V); //Apply rotation onto given 3x1 vector
void rotate(Vector cam, Vector up, float X, float Y, float Z); //Handle rotation of camera & up vector
float v_dot_product(Vector v1, Vector v2); //Dot Product two 3x1 vectors
Vector find_vector(const std::vector<Vector>& table, int index); //Find vertices for given point on triangle
#pragma once
