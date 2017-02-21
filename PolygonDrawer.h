#pragma once
#include <vector>
#include "a4utils.h"

#define ImageW 400
#define ImageH 400

using namespace std;

class vertex;

class Pt
{
public:
	int x, y;

	Pt(void)
	{
		x = y = 0;
	}

	Pt(int nX, int nY)
	{
		x = nX;
		y = nY;
	}

	float F[3];
	float z;
};

void drawPolygonFlat(vector<vertex> verts);

void drawPolygonSmooth(vector<vertex> verts, int interpolation);