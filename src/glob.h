#ifndef GLOB_H_INCLUDED
#define GLOB_H_INCLUDED

#include <cstdio>
#include <SDL3/SDL.h>

SDL_Renderer *screen;

#define SKY_COLOR 0x0094FF

#define WIDTH 800
#define HEIGHT 600

//TODO: CHUNK RENDER LOD SYSTEM BASED ON RESOLUTION

#define HALF_WIDTH (WIDTH/2)
#define HALF_HEIGHT (HEIGHT/2)

#define REAL_X (i+x*16)
#define REAL_Y (j+y*16)

#define CHUNK_POS(position) ((int)(position/16))

#define WORLD_SIZE 255
#define WORLD_CENTER 127

#define STRUCT_OP(type,op) \
		type operator op(const type& p) const { return type(x op p.x, y op p.y); }

#define STRUCT_ASSIGN_OP(type,op) \
		type& operator op##=(const type& p) { x op##= p.x; y op##= p.y; return *this; }

#define STRUCT_OP_ALL(type,op) \
		STRUCT_OP(type,op) \
		STRUCT_ASSIGN_OP(type,op)

struct Position
{
	float x = 0, y = 0;

	Position() = default;

	Position(float _x, float _y) : x(_x), y(_y) {}
	Position(float _a) : Position(_a, _a) {}

	STRUCT_OP_ALL(Position,+);
	STRUCT_OP_ALL(Position,-);
	STRUCT_OP_ALL(Position,*);
	STRUCT_OP_ALL(Position,/);
};

struct Point
{
	int x = 0, y = 0;

	Point() = default;

	Point(int _x, int _y) : x(_x), y(_y) {}
	Point(int _a) : Point(_a, _a) {}

	STRUCT_OP_ALL(Point,+);
	STRUCT_OP_ALL(Point,-);
	STRUCT_OP_ALL(Point,*);
	STRUCT_OP_ALL(Point,/);
};

int debugValue;
int selBlockX, selBlockY;

char *intToString(int value)
{
	static char textBuffer[32];
	sprintf(textBuffer, "%d", value);
	return textBuffer;
}

#endif // GLOB_H_INCLUDED
