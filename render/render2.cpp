/* render2.c
 * Triangle Rendering
 *
 * Purpose: To render triangles from an input .ply file, outputs to a PPM file. Now in C++!
 *
 * Assumptions: User inputs a .ply that plots triangles, so faces always have 3 points.
 * 		User knows that camera defaults to <1,0,0>, their input only rotates the camera
 *
 * The program accepts one command line argument, including the name of the file and rotation angles (Ex. 'file.ply 90 -45 20')
 */

#include <iostream> 
#include <cstdlib>
#include <vector>
#include <fstream> 
#include <sstream>
#include <string>
#include <cmath> 
#include "render2.h" 

/* Find max vector
 * V: A structure holding all the given input vertices
 * vertices: The amount of vertices in the input file
 */
Vector v_max(const std::vector<Vector>& V, int vertices)
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
	return Vector(max_A, max_B, max_C);
}

/* Find min vector
 * V: A structure holding all the given input vertices
 * vertices: The amount of vertices in the input file
 */
Vector v_min(const std::vector<Vector>& V, int vertices)
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
	return Vector(min_A, min_B, min_C);
}

/* Find center vector (min + max)/2
 * max: Given vector to hold maximum x,y,z values
 * min: Given vector to hold minimum x,y,z values
 */
Vector v_center(Vector max, Vector min)
{
	return Vector((max.a + min.a) / 2, (max.b + min.b) / 2, (max.c + min.c) / 2);
}

/* Find E scalar of bounding box, largest component of <max-min> or largest extent of the three axes
 * max: Given vertex to hold maximum x,y,z values
 * min: Given vertex to hold minimum x,y,z values
 */
float find_e(Vector max, Vector min)
{
	//Assign temporary value for e
	float e = -FLOAT_MAX;

	//Grab extents of each axis
	float temp_X = max.a - min.a;
	float temp_Y = max.b - min.b;
	float temp_Z = max.c - min.c;

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
		for (int j = 0; j < 3; j++)
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
 * Note: Wish I kept this in original C implementation, but doesn't hurt to practice
 */
void v_rotate(Vector product, float R[3][3], Vector V)
{ 
	//Go through entire 3x3 matrix. Go through each row at a time
	product.a = (R[0][0] * V.a) + (R[0][1] * V.b) + (R[0][2] * V.c);
	product.b = (R[1][0] * V.a) + (R[1][1] * V.b) + (R[1][2] * V.c);
	product.c = (R[2][0] * V.a) + (R[2][1] * V.b) + (R[2][2] * V.c);
}

/* Rotate the camera & up vectors by the given degrees
 * cam: Provided camera vector that will be rotated. Default <1, 0, 0>
 * up: Provided up vector that will be rotated. Default <0, 0, 1>
 * X: Degrees that camera & up vectors will be rotated about the X axis, given by user input
 * Y: Degrees that camera & up vectors will be rotated about the Y axis, given by user input
 * Z: Degrees that camera & up vectors will be rotated about the Z axis, given by user input
 */
void rotate(Vector cam, Vector up, float X, float Y, float Z)
{
	//Convert degree input into radians for cmath trig functions 
	float radX = X * (PI / 180.0);
	float radY = Y * (PI / 180.0);
	float radZ = Z * (PI / 180.0);

	//Set up rotation matrix for X, Y, Z
	float Rx[3][3] = { {1, 0, 0}, {0, cos(radX), -sin(radX)}, {0, sin(radX), cos(radX)} };
	float Ry[3][3] = { {cos(radY), 0, sin(radY)}, {0, 1, 0}, {-sin(radY), 0, cos(radY)} };
	float Rz[3][3] = { {cos(radZ), -sin(radZ), 0}, {sin(radZ), cos(radZ), 0}, {0, 0, 1} };

	//Create matrices to store products & old vectors for purposes of matrix multiplication
	float R[3][3], temp_R[3][3]; 

	//Create temporary arrays to hold old cam/up vectors
	Vector temp_cam = Vector(cam.a, cam.b, cam.c);  
	Vector temp_up = Vector(up.a, up.b, up.c);


	//Find rotation matrix Rz*(Ry*Rx)
	create_rotate(temp_R, Ry, Rx); //Ry * Rx
	create_rotate(R, Rz, temp_R); //Rz * (Ry * Rx) 

	//Rotate camera & up vectors around the X-axis, Y-axis, Z-axis
	v_rotate(cam, R, temp_cam);
	v_rotate(up, R, temp_up);
}

/* Dot Multiply two 3x1 matrices
 * v1: Input 3x1 matrix
 * v2: Input 3x1 matrix
 * Return product: Result of the dot multiplication (Scalar)
 */
float v_dot_product(Vector v1, Vector v2)
{
	return (v1.a * v2.a) + (v1.b * v2.b) + (v1.c * v2.c);
}

/* Find vertices for a point on the triangle, depending on the vector index & vector table
 * v: Output 3x1 matrix, will be assigned vertices depending on found vector
 * table: Table that stores all vertices from .ply file
 * index: Specific vector that the face from .ply file wants, to assign vertice for triangle edge
 */
Vector find_vector(const std::vector<Vector>& table, int index)
{
	return Vector(table[index].a, table[index].b, table[index].c);
}

/* Handle user input & begin Triangle Rendering */
int main(int argc, char* argv[])
{
	unsigned char pixel[ROWS][COLS];
	int close_tri, vertices, faces; 
	Vector camera, up, max, min, center, left, right, top, bottom; 
	Vector top_left, diff, diff1, sum, image, ABC, v0, v1, v2; 
	Vector intersect, prod1, prod2;
	float X, Y, Z, E, a, D, n, d;
	float zBuffer, dot1, dot2, dot3;

	//Make sure user enters correct # of arguments
	if (argc != 5)
	{
		std::cout << "Program use is . / render 'filename' degree1 degree2 degree3" << std::endl;
		std::cout << "Degrees are for the camera rotation" << std::endl;
		return 1;
	}

	//Grab user input for filename
	std:: string filename = argv[1];

	/* Parse through .ply file, grab vertices & faces */
	std::ifstream infile(filename, std::ios::binary);

	//Check if we can find file given by user, otherwise exit to prevent segfault
	if (!infile.is_open())
	{
		std::cout << "Invalid open, make sure that file is located in the same folder as executable" << std::endl;
		return 1;
	}

	//Go through header 
	std::string header;
	std::getline (infile, header); 
	header.erase(std::remove_if(header.begin(), header.end(), ::isspace), header.end());
	//Make sure user gives valid file type by reading 1st line in header
	if (header != "ply")
	{
		std::cout << "File must be a .ply file, run program again with correct file type" << std::endl; 
		std::cout << header << std::endl;
		infile.close();
		return 1;
	} 
	std::string line; 
	std::getline(infile, line); 
	std::getline(infile, line); 
    vertices = std::stoi(line.substr(15)); //Grab # vertices after 'element vertex ' from previously read line
	std::getline(infile, line);
	std::getline(infile, line);
	std::getline(infile, line); 
	std::getline(infile, line); 
	faces = std::stoi(line.substr(13)); //Grab # faces, or # triangles after 'element faces ' from previously read line
	std::getline(infile, line);
	std::getline(infile, line);

	//Create vertex & face structs to hold in vertices & faces from .ply file
	std::vector <Vector> V(vertices);
	std::vector <Face> triangle(faces); 

	//Collect vertices 
	for (int i = 0; i < vertices; i++)
	{
		infile >> V[i].a >> V[i].b >> V[i].c;
	}

	int temp; //Stores 3 in front of each triangle

	//Collect faces (or triangles)
	for (int i = 0; i < faces; i++)
	{
		infile >> temp >> triangle[i].v0 >> triangle[i].v1 >> triangle[i].v2;
	}

	infile.close(); //Done with parsing 

	/* Calculate the bounding box on the vertices */

	max = v_max(V, vertices); 
	min = v_min(V, vertices); 
	center = v_center(max, min); 
	E = find_e(max, min); 

	/* Calculate camera position and orientation */

	//Grab rotations from user and default camera, convert input into floats
	X = atof(argv[2]);
	Y = atof(argv[3]);
	Z = atof(argv[4]);

	//Default camera onto X axis (<1,0,0>) 
	camera = Vector(1.0, 0.0, 0.0);

	//Default up onto Z axis (<0,0,1>) 
	up = Vector(0.0, 0.0, 1.0);

	//Rotate camera & up vector
	rotate(camera, up, X, Y, Z);

	//Move and scale the camera vector (<camera> = 1.5E<camera> + <center>) 
	camera = camera * (1.5 * E); //<camera> = 1.5E*<camera> 
	camera = camera + center; //<camera> = 1.5e*<camera> + center 

	/* Determine the 3D coordinates bounding the image */

	//Find first left
	diff = center - camera;//<diff> = <center> - <camera> 
	left = up.cross(diff); //<left> = <up> x <center-camera> 

	//Find a = ||<left>||
	a = sqrtf((left.a * left.a) + (left.b * left.b) + (left.c * left.c));

	//Find final left
	left = left * (E / (2 * a)); //<left> = E/2a<left> 
	left = left + center;//<left> = E/2a<left> + center  

	//Find right
	right = diff.cross(up); //right = <center-camera> x <up> 
	right = right * (E / (2 * a)); //<right> = E/2a<right> 
	right = right + center;//<right> = E/2a<right> + center 

	//Find top 
	top = up * (E / 2); //<top> = E/2<up> 
	top = top + center;//<top> = E/2<up> + <center> 

	//Find bottom 
	bottom = up * ((-E) / 2); //<bottom> = -E/2<up> 
	bottom = bottom + center;//<bottom> = -E/2<up> + <center> 

	//Find topleft 
	top_left = up * (E / 2); //<topleft> = E/2<up> 
	top_left = top_left + left;//<topleft> = E/2<up> + <left> 

	/* Determine each pixel r,c in the image */
	std::cout << "Rendering..." << std::endl;
	for (int r = 0; r < ROWS; r++)
	{
		//Go through each column in the row
		for (int c = 0; c < COLS; c++)
		{
			//Calculate vector coordinates for the image pixel
		    diff = bottom - top; //<diff> = <bottom - top> 
			diff = diff * ((float)r / (ROWS - 1)); //[r/(ROWS-1)]<bottom - top> 
			diff1 = right - left; //<diff1> = <right-left>  
			diff1 = diff1 * ((float)c / (COLS - 1)); //<diff1> = c/(COLS-1)<right-left> 
			sum = diff1 + diff;//<sum> = c/(COLS-1)<right-left> + r/(ROWS-1)<bottom - top>
			image = top_left + sum;//<image> = <topleft> + c/(COLS-1)<right-left> + r/(ROWS-1)<bottom - top>  

			//Find the plane equation that contains the triangle 
			zBuffer = FAR; //Default the zBuffer to far away for each pixel before checking triangles 
			close_tri = -1; //Default index to impossible value, meant to distinguish if any triangle is found or not
			for (int i = 0; i < faces; i++)
			{
				//Find v0, v1, v2 for this triangle 
				v0 = find_vector(V, triangle[i].v0);
				v1 = find_vector(V, triangle[i].v1);
				v2 = find_vector(V, triangle[i].v2); 

				//Find ABC
				diff = v1 - v0; //<diff> = <v1-v0> 
				diff1 = v2 - v0; //<diff1> = <v2-v0> 
				ABC = diff.cross(diff1); //<A,B,C> = <v1-v0> x <v2-v0> 

				//Find D 
				prod1 = ABC * -1; //<prod1> = -<A,B,C>
				D = v_dot_product(prod1, v0); //D = -<A,B,C> * <v0>

				//Find distance along the image pixel ray to the triangle
				n = v_dot_product(prod1, camera) - D; //n = -<A,B,C> * camera - D
				diff = image - camera; //<diff> = <image - camera>
				d = v_dot_product(ABC, diff); //d = <ABC> * <image - camera> 

				//If ray is parallel to triangle, d near zero, skip
				if (fabs(d) > 1e-6)
				{
					//Find 3D coords <intersect> of ray & plane
					diff = diff * (n/d); //<diff> = n/d<image-camera> 
					intersect = camera + diff;//<intersect> = <camera> + n/d<image-camera> 

					/* Determine if intersection point lies within triangle, if any dot product is less than 0 (outside!), stop early */

					//Dot1
					diff = v2 - v0; //<diff> = <v2-v0> 
					diff1 = v1 - v0; //<diff1> = <v1-v0> 
					prod1 = diff.cross(diff1); //<prod1> = <v2-v0> x <v1-v0> 
					diff = intersect - v0; //<diff> = <intersect-v0> 
					prod2 = diff.cross(diff1); //<prod2> = <intersect-v0> x <v1-v0>
					dot1 = v_dot_product(prod1, prod2); //dot1 = 〈v2 − v0〉 × 〈v1 − v0〉 · 〈intersect − v0〉 × 〈v1 − v0〉 

					//Dot2 if previous passes
					if (dot1 >= 0)
					{
						diff = v0 - v1; //<diff> = <v0-v1> 
						diff1 = v2 - v1; //<diff1> = <v2-v1> 
						prod1 = diff.cross(diff1); //<prod1> = <v0-v1> x <v2-v1> 
						diff = intersect - v1; //<diff> = <intersect-v1> 
						prod2 = diff.cross(diff1); //<prod2> = <intersect-v1> x <v2-v1>
						dot2 = v_dot_product(prod1, prod2); //dot2 = 〈v0 − v1〉 × 〈v2 − v1〉 · 〈intersect − v1〉 × 〈v2 − v1〉
					}

					//Dot3 if previous two pass
					if (dot1 >= 0 && dot2 >= 0)
					{
						diff = v1 - v2; //<diff> = <v1-v2> 
						diff1 = v0 - v2; //<diff1> = <v0-v2> 
						prod1 = diff.cross(diff1); //<prod1> = <v1-v2> x <v0-v2> 
						diff = intersect - v2; //<diff> = <intersect-v2> 
						prod2 = diff.cross(diff1); //<prod2> = <intersect-v2> x <v0-v2>
						dot3 = v_dot_product(prod1, prod2); //dot3 = 〈v1 − v2〉 × 〈v0 − v2〉 · 〈intersect − v2〉 × 〈v0 − v2〉
					}

					//Proved all dot products, triangle is "seen" 
					if (dot1 >= 0 && dot2 >= 0 && dot3 >= 0)
					{
						//Check if this triangle is closer than other triangles. If so, set this triangle to be closest 
						if (n / d < zBuffer)
						{
							close_tri = i; //Save triangle index
							zBuffer = n / d; //Save the distance
						}
					}
				}
			}

			//Set pixel color depending on triangle found
			if (close_tri < 0) pixel[r][c] = BLACK; //Triangle not found, set to background color
			else pixel[r][c] = 155 + (close_tri % 100); //Triangle found, set to greyscale value varied by triangle index 
			//printf("%d", pixel[r][c]);
		}
	}

	/* Handle new file name */
	std::string newExt = ".ppm";
	size_t ext_pos = filename.find_last_of('.'); //Find location of file extension in filename
	if (ext_pos != std::string::npos) filename = filename.substr(0, ext_pos); //Get rid of old extension, if it exists 
	std::string newfilename = filename + newExt; //New file should be the same name, with new extension

	/* Write pixel values to new .ppm file */
	std::string newHeader = "P5 256 256 255\n"; 
	std::ofstream outfile(newfilename, std::ios::binary);
	outfile.write(newHeader.c_str(), newHeader.size());
	outfile.write(reinterpret_cast<char*>(&pixel[0][0]), ROWS * COLS * sizeof(unsigned char)); //Expects char, need to convert from unsigned to char
	outfile.close(); 
	std::cout << "Outputting to " << newfilename << std::endl;

	//Exit
	return 0;
}