#include "a4utils.h"
#include "ScanConvert.h"
#include "PolygonDrawer.h"

vertex interpretVertex(const string & line) {
	string * arr = new string[3];
	arr[0] = "";
	arr[1] = "";
	arr[2] = "";
	int current = 0;
	for (int i = 2; i < line.length(); i++) {
		if (line[i] == ' ') {
			current++;
		}
		else if (current < 3) arr[current] += line[i];
	}
	size_t sz;
	float x = std::stof(arr[0], &sz);
	float y = std::stof(arr[1], &sz);
	float z = std::stof(arr[2], &sz);
	return vertex(x, y, z);
}

face interpretFace(const string & line) {
	string * arr = new string[3];
	arr[0] = "";
	arr[1] = "";
	arr[2] = "";
	int current = 0;
	for (int i = 2; i < line.length(); i++) {
		if (line[i] == ' ') {
			current++;
		}
		else if (current < 3) arr[current] += line[i];
	}
	size_t sz;
	int v1 = std::stoi(arr[0]);
	int v2 = std::stoi(arr[1]);
	int v3 = std::stoi(arr[2]);
	face rvalue(v1 - 1, v2 - 1, v3 - 1);
	return rvalue;
}

float magnitude(Vector3 v) {
	return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

Vector3 Vector3::operator+ (Vector3 v) {
	return Vector3(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector3 Vector3::operator- (Vector3 v) {
	return Vector3(this->x - v.x, this->y - v.y, this->z - v.z);
}

Vector3 Vector3::operator/ (float n) {
	return Vector3(this->x / n, this->y / n, this->z / n);
}

Vector3 operator* (const Vector3 & v, float n) {
	return Vector3(v.x*n, v.y*n, v.z*n);
}

Vector3 operator* (float n, const Vector3 & v) {
	return Vector3(v.x*n, v.y*n, v.z*n);
}

Vector3 operator+ (const Vector3 & v, float n) {
	return Vector3(v.x + n, v.y + n, v.z + n);
}

Vector3 operator+ (float n, const Vector3 & v) {
	return Vector3(v.x + n, v.y + n, v.z + n);
}

std::string Vector3::toString() {
	return '[' + std::to_string(x) + ',' + std::to_string(y) + ',' + std::to_string(z) + ']';
}

std::string vertex::toString() {
	return '[' + std::to_string(x) + ',' + std::to_string(y) + ',' + std::to_string(z) + ']';
}

float dotProduct(Vector3 v1, Vector3 v2) {
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector3 crossProduct(Vector3 v1, Vector3 v2) {
	float a = v1.x,
		b = v1.y,
		c = v1.z,
		d = v2.x,
		e = v2.y,
		f = v2.z;
	return Vector3(-c*e + b*f, c*d - a*f, -b*d + a*e);
}

Vector3 normalize(Vector3 v) {
	return v / magnitude(v);
}

Vector3 A(0.5, 0.5, 0.5);
Vector3 E = normalize(Vector3(0, 0, -1));
Vector3 L = normalize(Vector3(1, 1, -1));
Vector3 C(1, 1, 1);
Vector3 ka(0.1, 0, 0);
Vector3 kd(0.7, 0, 0);
Vector3 ks(0.5, 0.5, 0.5);
float n = 5; //specular exponent

float *  totalIllumination(Vector3 v) {
	float n = 5;
	float LN = dotProduct(L, v);
	Vector3 R = (2 * LN*v) - L; //approximately "normal"
	float RE = dotProduct(R, E);
	float REn = pow(RE, n);
	float Ir, Ig, Ib;
	if (LN <= 0) {
		Ir = ka.x*A.x;
		Ig = ka.y*A.y;
		Ib = ka.z*A.z;
	}
	else {
		if (REn < 0) REn = 0;
		Ir = ka.x*A.x + C.x*(kd.x*(LN)+ks.x*(REn));
		Ig = ka.y*A.y + C.y*(kd.y*(LN)+ks.y*(REn));
		Ib = ka.z*A.z + C.z*(kd.z*(LN)+ks.z*(REn));
	}
	return new float[3]{ Ir, Ig, Ib };
}

Vector3 normal(vector<vertex> v) {
	Vector3 v3v1 = Vector3(v[2].x, v[2].y, v[2].z) - Vector3(v[0].x, v[0].y, v[0].z);
	Vector3 v2v1 = Vector3(v[1].x, v[1].y, v[1].z) - Vector3(v[0].x, v[0].y, v[0].z);
	return normalize(crossProduct(v3v1, v2v1));
}