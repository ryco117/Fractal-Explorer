#pragma once
#include <cmath>

#include "ObjectsBase.h"

GLfloat cubeVertices[] = {
	 0.5,  0.5,  0.5, 1, // 0
	-0.5,  0.5,  0.5, 1, // 1
	-0.5, -0.5,  0.5, 1, // 2
	 0.5, -0.5,  0.5, 1, // 3
	 0.5, -0.5, -0.5, 1, // 4
	-0.5, -0.5, -0.5, 1, // 5
	-0.5,  0.5, -0.5, 1, // 6
	 0.5,  0.5, -0.5, 1  // 7
};
unsigned int cubeIndices[] = {
	0, 1, 3, 2,                 // Front face
	7, 4, 6, 5,                 // Back face
	6, 5, 1, 2,                 // Left face
	7, 0, 4, 3,                 // Right face
	7, 6, 0, 1,                 // Top face
	3, 2, 4, 5,                 // Bottom face
};


class CubeObj: public Object3D
{
public:
	CubeObj(): Object3D(24, 24, GL_TRIANGLE_STRIP)
	{
		for(int i = 0; i < 8; i++)
		{
			vertices[i] = Vertices[i];
			indices[i] = C_Indices[i];
		}
		for(int i = 8; i < 24; i++)
			indices[i] = C_Indices[i];
	}
};

GLfloat lineVertices[] = {
	0, 0, 0, 1,// 0
	1, 1, 1, 1 // 1
};

class LineObj: public Object3D
{
public:
	LineObj(): Object3D(2, 2, GL_LINES)
	{
		vertices[0] = lineVertices[0];
		vertices[1] = lineVertices[1];
		indices[0] = 0;
		indices[1] = 1;
	}
};

GLfloat planeVertices[] = {
	 1,  1, 0, 1, // top right
	-1,  1, 0, 1, // top left
	-1, -1, 0, 1, // bottom left
	 1, -1, 0, 1, // bottom right
};

unsigned int planeIndices[4] = {
	2, 3, 1, 0,
};

class PlaneObj: public Object3D
{
public:
	PlaneObj(): Object3D(4, 4, GL_TRIANGLE_STRIP)
	{
		for(int i = 0; i < 4; i++)
		{
			vertices[i] = P_Vertices[i];
			indices[i] = P_Indices[i];
		}
	}
};

class SphereObj: public Object3D
{
public:
	SphereObj(unsigned int n, float radius): Object3D()
	{
		mode = 4;

		if(n == 0)
			n = 1;

		verts = 4 * n;
		for(int k = 1; k < n; k++)
			verts += 8 * (n-k);
		verts += 2;
		inds = ((verts * 2) - 4) * 3;

		vertices = new Vertex3fColour[verts];
		indices = new GLuint[inds];
		mapcoords = new float[verts*2];

		vertices[0].Pos = float3(0, 1, 0);
		mapcoords[0] = 0;
		mapcoords[1] = 1;
		Scale = float3(radius);

		const float pi = 3.14159265359;
		unsigned int MI = 1;	//MasterIndex for vertices
		for(unsigned int iY = 1; iY < n + 1; iY++)		//Fills in top half
		{
			float ThetaY = (float(iY) / float(n)) * 0.5 * pi;
			for(unsigned int iX = 0; iX < (iY * 4); iX++)
			{
				float ThetaX = (float)iX/float(iY * 4) * 2 * pi;
				vertices[MI].Pos = float3(cosf(ThetaX) * sinf(ThetaY), cosf(ThetaY), sinf(ThetaX) * sinf(ThetaY));
				MI++;
			}
		}
		for(; MI < verts; MI++)		//Fills in bottom half
		{
			vertices[MI].Pos = vertices[(verts - 1) - MI].Pos;
			vertices[MI].Pos.y *= -1;
			vertices[MI].Pos.z *= -1;
		}

		MI = 0;
		for(; MI < verts; MI++)
		{
			//mapcoords[MI*2] = 0.5 + (atan2(vertices[MI].Pos.x, vertices[MI].Pos.z) / (2 * pi));
			//mapcoords[MI*2+1] = 0.5 - (asinf(vertices[MI].Pos.y) / pi);
			mapcoords[MI*2] = 0.1;
			mapcoords[MI*2+1] = 0.1;
		}

		//First row
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

		indices[6] = 0;
		indices[7] = 3;
		indices[8] = 4;

		indices[9] = 0;
		indices[10] = 1;
		indices[11] = 4;
		MI = 12;

		unsigned int Corns = 3;
		for(unsigned int iY = 1; iY < n; iY++)		//Fills in top half
		{
			for(unsigned int iX = 0; iX < (iY * 4); iX++)
			{
				unsigned int self = SphereFindPoint(iY-1) + iX;			//Find our fucking self....

				float ratio = float(iX) / float(iY * 4);
				//			0/4			  1/4			   2/4			   3/4
				if(ratio == 0 || ratio == 0.25 || ratio == 0.5 || ratio == 0.75)		//are we at a corner? (0-3 == 4 corners)
				{
					Corns++;

					unsigned int* points = new unsigned int[4];
					if(iX == 0)
						points[0] = SphereFindPoint(iY+1) - 1;					//final point in their row
					else
						points[0] = self + Corns - 1;

					points[1] = self + Corns;
					points[2] = self + Corns + 1;
					if(self == SphereFindPoint(iY) - 1)
						points[3] = SphereFindPoint(iY-1);
					else
						points[3] = self + 1;

					for(int i = 0; i < 3; i++)
					{
						indices[MI] = self;
						MI++;
						indices[MI] = points[i];
						MI++;
						indices[MI] = points[i+1];
						MI++;
					}
					delete[] points;
				}
				else
				{
					unsigned int* points = new unsigned int[3];
					points[0] = self + Corns;
					points[1] = self + Corns + 1;
					if(self == SphereFindPoint(iY) - 1)
						points[2] = SphereFindPoint(iY-1);
					else
						points[2] = self + 1;

					for(int i = 0; i < 2; i++)
					{
						indices[MI] = self;
						MI++;
						indices[MI] = points[i];
						MI++;
						indices[MI] = points[i+1];
						MI++;
					}
					delete[] points;
				}
			}
		}
		for(; MI < inds; MI++)		//Fills in bottom half
		{
			indices[MI] = indices[(inds - 1) - MI];
			indices[MI] = (verts-1) - indices[MI];
		}
	}

private:
	unsigned int SphereFindPoint(unsigned int n)
	{
		unsigned int tn = 4 * (n - 1);		//no +4 here so...
		return ((tn + 8) * n / 2) + 1;		//+8 here
	}
};
#endif
