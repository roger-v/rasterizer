#include <Windows.h>
#include <GL/glut.h>
#include <math.h>
#include "ScanConvert.h"
#include "PolygonDrawer.h"
#include "a4utils.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

#define PHONG 3
#define GOURAUD 2
#define FLAT 1
int interpolation = GOURAUD;

float framebuffer[ImageH][ImageW][3];

vector<vertex> vertices;
vector<face> faces;

void drawit(void)
{
	glDrawPixels(ImageW, ImageH, GL_RGB, GL_FLOAT, framebuffer);
	glFlush();
}

void clearFramebuffer()
{
	int i, j;

	for (i = 0; i<ImageH; i++) {
		for (j = 0; j<ImageW; j++) {
			framebuffer[i][j][0] = 0.0;
			framebuffer[i][j][1] = 0.0;
			framebuffer[i][j][2] = 0.0;
		}
	}
}

void setFramebuffer(int x, int y, float R, float G, float B)
{
	y = ImageH - 1 - y;
	if (R <= 1.0)
		if (R >= 0.0)
			framebuffer[y][x][0] = R;
		else
			framebuffer[y][x][0] = 0.0;
	else
		framebuffer[y][x][0] = 1.0;
	if (G <= 1.0)
		if (G >= 0.0)
			framebuffer[y][x][1] = G;
		else
			framebuffer[y][x][1] = 0.0;
	else
		framebuffer[y][x][1] = 1.0;
	if (B <= 1.0)
		if (B >= 0.0)
			framebuffer[y][x][2] = B;
		else
			framebuffer[y][x][2] = 0.0;
	else
		framebuffer[y][x][2] = 1.0;
}

void display(void)
{
	drawit();
}

void init(void)
{
	clearFramebuffer();
	for (int i = 0; i < faces.size(); i++) {
		face f = faces[i];
		float * color = totalIllumination(f.normal);
		if (f.normal.z < 0) {
			if (interpolation == GOURAUD) {
				vector<vertex> verts = { vertices[f.v1], vertices[f.v2], vertices[f.v3] };
				drawPolygonSmooth(verts, GOURAUD);

			}
			else if (interpolation == PHONG) {
				vector<vertex> verts = { vertices[f.v1], vertices[f.v2], vertices[f.v3] };
				drawPolygonSmooth(verts, PHONG);

			}
			else {
				vector<vertex> verts = { vertices[f.v1], vertices[f.v2], vertices[f.v3] };
				drawPolygonFlat(verts);
			}
		}
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		interpolation = FLAT;
		break;
	case '2':
		interpolation = GOURAUD;
		break;
	case '3':
		interpolation = PHONG;
		break;
	default:
		break;
	}
	init();
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	if (!argv[1]) {
		cout << "no argument detected, using cow!" << endl;
	}
	else cout << "file is:\n" << argv[1] << endl;
	string cow = "cow.obj";
	ifstream inFile;
	if (argv[1]) inFile.open(argv[1]);
	else inFile.open(cow);
	if (!inFile.is_open()) {
		cout << "The file did not open.\n";
		return 0;
	}
	string line;

	while (getline(inFile, line))
	{
		if (line[0] != '#') {
			if (line[0] == 'v') {
				vertex v = interpretVertex(line);
				vertices.push_back(v);
			}
			else if (line[0] == 'f') {
				face f = interpretFace(line);
				Vector3 faceNorm = faceNormal(f);
				vertices[f.v1].normal = vertices[f.v1].normal + f.normal;
				vertices[f.v2].normal = vertices[f.v2].normal + f.normal;
				vertices[f.v3].normal = vertices[f.v3].normal + f.normal;
				faces.push_back(f);
			}
		}
	}

	for (int i = 0; i < vertices.size(); i++) {
		vertices[i].normal = normalize(vertices[i].normal);
	}

	inFile.close();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(ImageW, ImageH);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Roger Villanueva - Homework 4");
	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);

	glutMainLoop();
	return 0;
}

Vector3 faceNormal(face & f) {
	Vector3 v3v1 = Vector3(vertices[f.v3].x, vertices[f.v3].y, vertices[f.v3].z) - Vector3(vertices[f.v1].x, vertices[f.v1].y, vertices[f.v1].z);
	Vector3 v2v1 = Vector3(vertices[f.v2].x, vertices[f.v2].y, vertices[f.v2].z) - Vector3(vertices[f.v1].x, vertices[f.v1].y, vertices[f.v1].z);
	f.normal = normalize(crossProduct(v3v1, v2v1));
	return f.normal;
}

Pt worldToScreen(vertex v1, int interp) {
	float x = v1.x;
	float y = v1.y;
	Vector3 v(x, y, 1);
	Vector3 m0((ImageW - 1) / 2, 0, (ImageW - 1) / 2);
	Vector3 m1(0, (1 - ImageH) / 2, (ImageH - 1) / 2);
	Vector3 m2(0, 0, 1);
	Vector3 outPut(dotProduct(v, m0), dotProduct(v, m1), dotProduct(v, m2));
	Pt p(outPut.x, outPut.y);
	if (interp == PHONG) { //interpolate normals
		p.F[0] = v1.normal.x;
		p.F[1] = v1.normal.y;
		p.F[2] = v1.normal.z;
	}
	else { //interpolate colors for gouraud
		float * colors = totalIllumination(v1.normal);
		p.F[0] = colors[0];
		p.F[1] = colors[1];
		p.F[2] = colors[2];
	}
	p.z = v1.z;
	return p;
}