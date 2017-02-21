#pragma once
#include "a4utils.h"


#define ImageW 400
#define ImageH 400

struct color {
	float r, g, b;		// Color (R,G,B values)
};

// Clears framebuffer to black
void clearFramebuffer();

// Sets pixel x,y to the color RGB
void setFramebuffer(int x, int y, float R, float G, float B);

Pt worldToScreen(vertex v1, int interp);

Vector3 faceNormal(face & f);

