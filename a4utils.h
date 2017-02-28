#pragma once
#include <string>
#include <vector>
#include "PolygonDrawer.h"
using std::string;
using std::vector;

class Vector3 {
public:
	float x;
	float y;
	float z;

	Vector3() : x(0), y(0), z(0) {}

	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3 operator+ (Vector3 v);

	Vector3 operator- (Vector3 v);

	Vector3 operator/ (float n);

	std::string toString();
};

float magnitude(Vector3 v);

Vector3 operator* (const Vector3 & v, float n);

Vector3 operator* (float n, const Vector3 & v);

Vector3 operator+ (const Vector3 & v, float n);

Vector3 operator+ (float n, const Vector3 & v);

float dotProduct(Vector3 v1, Vector3 v2);

Vector3 crossProduct(Vector3 v1, Vector3 v2);

Vector3 normalize(Vector3 v);

class vertex {
public:
	
	vertex(float x, float y, float z) : x(x), y(y), z(z) {}
	float x;
	float y;
	float z;
	Vector3 normal;
	std::string toString();
	float F[3];
};

class face {
public:
	
	face(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
	int v1;
	int v2;
	int v3;
	Vector3 normal;
};

Vector3 normal(vector<vertex> v);

vertex interpretVertex(const string & line);

face interpretFace(const string & line);

float *  totalIllumination(Vector3 v);

