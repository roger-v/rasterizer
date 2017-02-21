#include "PolygonDrawer.h"
#include "ScanConvert.h"
#include <algorithm>
#include <math.h>
#include "a4utils.h"
#include <iostream>
using std::cout;
using std::endl;

#define PHONG 3
#define GOURAUD 2
#define FLAT 1

float zBuffer[ImageW][ImageH];
bool zBufferSetToZeroes = false;
void print(float * f);

class Edge
{
public:
	float slopeRecip;
	float maxY;
	float currentX;
	float currentF[3];
	float fIncr[3];
	float currentZ;
	float zIncr;

	bool operator < (const Edge &e)
	{
		if (currentX == e.currentX)
		{
			return slopeRecip < e.slopeRecip;
		}
		else
		{
			return currentX < e.currentX;
		}
	}
};

vector<vector<Edge> > activeEdgeTable;
vector<Edge> activeEdgeList;

void buildActiveEdgeTableSmooth(vector<Pt> &points)
{
	int i;

	activeEdgeTable.clear();

	// add rows equal to height of image to active edge table
	for (i = 0; i < ImageH; i++)
	{
		vector<Edge> row;

		activeEdgeTable.push_back(row);
	}

	for (i = 0; i < points.size(); i++)
	{
		Edge e;
		int next = (i + 1) % points.size();

		// ignore horizontal lines
		if (points[i].y == points[next].y)
		{
			continue;
		}

		e.maxY = max(points[i].y, points[next].y);
		e.slopeRecip = (points[i].x - points[next].x) / (float)(points[i].y - points[next].y);
		e.zIncr = (points[i].z - points[next].z) / (float)(points[i].y - points[next].y);


		e.fIncr[0] = (points[i].F[0] - points[next].F[0]) / (float)(points[i].y - points[next].y);
		e.fIncr[1] = (points[i].F[1] - points[next].F[1]) / (float)(points[i].y - points[next].y);
		e.fIncr[2] = (points[i].F[2] - points[next].F[2]) / (float)(points[i].y - points[next].y);

		//one of the points does not get included 
		//in here...

		if (points[i].y == e.maxY) //otherwise
		{
			e.currentX = points[next].x;
			e.currentZ = points[next].z;
			e.currentF[0] = points[next].F[0];
			e.currentF[1] = points[next].F[1];
			e.currentF[2] = points[next].F[2];

			activeEdgeTable[points[next].y].push_back(e);
		}
		else //yi = min(yi, yi+1)
		{
			e.currentX = points[i].x;
			e.currentZ = points[i].z;
			e.currentF[0] = points[i].F[0];
			e.currentF[1] = points[i].F[1];
			e.currentF[2] = points[i].F[2];
			activeEdgeTable[points[i].y].push_back(e);
		}


	}
}


void buildActiveEdgeTable(vector<Pt> &points)
{
	int i;

	activeEdgeTable.clear();

	// add rows equal to height of image to active edge table
	for (i = 0; i < ImageH; i++)
	{
		vector<Edge> row;

		activeEdgeTable.push_back(row);
	}

	for (i = 0; i < points.size(); i++)
	{
		Edge e;
		int next = (i + 1) % points.size();

		// ignore horizontal lines
		if (points[i].y == points[next].y)
		{
			continue;
		}

		e.maxY = max(points[i].y, points[next].y);
		e.slopeRecip = (points[i].x - points[next].x) / (float)(points[i].y - points[next].y);

		if (points[i].y == e.maxY) //otherwise
		{
			e.currentX = points[next].x;
			activeEdgeTable[points[next].y].push_back(e);
		}
		else //yi = min(yi, yi+1)
		{
			e.currentX = points[i].x;
			activeEdgeTable[points[i].y].push_back(e);
		}


	}
}


void drawPolygonFlat(vector<vertex> verts) {
	Vector3 N = normal(verts);
	float * color = totalIllumination(N);
	vector<Pt> points = { worldToScreen(verts[0], 0), worldToScreen(verts[1], 0), worldToScreen(verts[2], 0) };

	int x, y, i;

	activeEdgeList.clear();
	buildActiveEdgeTable(points);


	for (y = 0; y < ImageH; y++)
	{

		// add edges into active Edge List
		for (i = 0; i < activeEdgeTable[y].size(); i++)
		{
			activeEdgeList.push_back(activeEdgeTable[y][i]);
		}

		// delete edges from active Edge List
		for (i = 0; i < activeEdgeList.size(); i++)
		{
			if (activeEdgeList[i].maxY <= y)
			{
				activeEdgeList.erase(activeEdgeList.begin() + i);
				i--;
			}
		}



		// sort according to x value... a little expensive since not always necessary
		sort(activeEdgeList.begin(), activeEdgeList.end());

		// draw scan line
		for (i = 0; i < activeEdgeList.size(); i += 2)
		{

			for (x = (int)ceil(activeEdgeList[i].currentX); x < activeEdgeList[i + 1].currentX; x++)
			{
				setFramebuffer(x, y, color[0], color[1], color[2]);
			}

		}

		// update edges in active edge list
		for (i = 0; i < activeEdgeList.size(); i++)
		{
			activeEdgeList[i].currentX += activeEdgeList[i].slopeRecip;

		}
	}
}

// assumes all vertices are within window!!!
void drawPolygonSmooth(vector<vertex> verts, int interpolation)
{
	if (!zBufferSetToZeroes) {
		memset(zBuffer, 100, sizeof(zBuffer));
		zBufferSetToZeroes = true;
	}
	Vector3 N = normal(verts);
	float * color = totalIllumination(N);
	vector<Pt> points = { worldToScreen(verts[0], interpolation), worldToScreen(verts[1], interpolation), worldToScreen(verts[2], interpolation) };

	int x, y, i;

	activeEdgeList.clear();
	buildActiveEdgeTableSmooth(points);


	for (y = 0; y < ImageH; y++)
	{

		// add edges into active Edge List
		for (i = 0; i < activeEdgeTable[y].size(); i++)
		{
			activeEdgeList.push_back(activeEdgeTable[y][i]);
		}

		// delete edges from active Edge List
		for (i = 0; i < activeEdgeList.size(); i++)
		{
			if (activeEdgeList[i].maxY <= y)
			{
				activeEdgeList.erase(activeEdgeList.begin() + i);
				i--;
			}
		}



		// sort according to x value... a little expensive since not always necessary
		sort(activeEdgeList.begin(), activeEdgeList.end());

		// draw scan line
		for (i = 0; i < activeEdgeList.size(); i += 2)
		{
			float dF[3];
			dF[0] = (activeEdgeList[i + 1].currentF[0] - activeEdgeList[i].currentF[0]) / (activeEdgeList[i + 1].currentX - activeEdgeList[i].currentX);
			dF[1] = (activeEdgeList[i + 1].currentF[1] - activeEdgeList[i].currentF[1]) / (activeEdgeList[i + 1].currentX - activeEdgeList[i].currentX);
			dF[2] = (activeEdgeList[i + 1].currentF[2] - activeEdgeList[i].currentF[2]) / (activeEdgeList[i + 1].currentX - activeEdgeList[i].currentX);

			float dZ = (activeEdgeList[i + 1].currentZ - activeEdgeList[i].currentZ) / (activeEdgeList[i + 1].currentX - activeEdgeList[i].currentX);

			float cX = activeEdgeList[i].currentX;
			//			float * cF = activeEdgeList[i].currentF;
			float cF[3];
			float cZ = activeEdgeList[i].currentZ;

			float fX = 0;

			int count = 0;

			for (x = (int)ceil(activeEdgeList[i].currentX); x < activeEdgeList[i + 1].currentX; x++)
			{
				if (count == 0) {


					//----
					cF[0] = activeEdgeList[i].currentF[0];
					cF[1] = activeEdgeList[i].currentF[1];
					cF[2] = activeEdgeList[i].currentF[2];
					//----
					cZ = activeEdgeList[i].currentZ;

					fX = ceil(cX) - cX;

					if (fX == 0) {
						cF[0] += dF[0];
						cF[1] += dF[1];
						cF[2] += dF[2];
						cZ += dZ;
					}
					else {
						cF[0] += fX*dF[0];
						cF[1] += fX*dF[1];
						cF[2] += fX*dF[2];
						cZ += fX*dZ;
					}


					if (cZ <= zBuffer[x][y]) {
						if (interpolation == PHONG) {
							color = totalIllumination(Vector3(cF[0], cF[1], cF[2]));
							setFramebuffer(x, y, color[0], color[1], color[2]);
						}
						else if (interpolation == GOURAUD) { // GOURAUD
							color = cF;
							setFramebuffer(x, y, color[0], color[1], color[2]);
						}
						zBuffer[x][y] = cZ;
					}

				}
				else {
					cF[0] += dF[0];
					cF[1] += dF[1];
					cF[2] += dF[2];
					cZ += dZ;
					if (cZ <= zBuffer[x][y]) {
						if (interpolation == PHONG) {
							color = totalIllumination(Vector3(cF[0], cF[1], cF[2]));
							setFramebuffer(x, y, color[0], color[1], color[2]);
						}
						else {
							color = cF;
							setFramebuffer(x, y, color[0], color[1], color[2]);
						}
						zBuffer[x][y] = cZ;
					}
				}
				count += 1;
			}

		}

		// update edges in active edge list
		for (i = 0; i < activeEdgeList.size(); i++)
		{
			activeEdgeList[i].currentX += activeEdgeList[i].slopeRecip;
			activeEdgeList[i].currentZ += activeEdgeList[i].zIncr;
			activeEdgeList[i].currentF[0] += activeEdgeList[i].fIncr[0];
			activeEdgeList[i].currentF[1] += activeEdgeList[i].fIncr[1];
			activeEdgeList[i].currentF[2] += activeEdgeList[i].fIncr[2];


		}
	}
}




void print(float * f) {
	cout << f[0] << ", " << f[1] << ", " << f[2] << endl;
}