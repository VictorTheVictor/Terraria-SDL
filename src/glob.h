#ifndef GLOB_H_INCLUDED
#define GLOB_H_INCLUDED

#define SKY_COLOR 0x0094FF

#define WIDTH 800
#define HEIGHT 600

#define HALF_WIDTH (WIDTH/2)
#define HALF_HEIGHT (HEIGHT/2)

#define REAL_X (i+x*16)
#define REAL_Y (j+y*16)

#define CHUNK_POS(position) ((int)(position/16))

#define WORLD_SIZE 255
#define WORLD_CENTER 127

int debugValue;
int selBlockX, selBlockY;

char *intToString(int value)
{
    static char textBuffer[32];
    sprintf(textBuffer, "%d", value);
    return textBuffer;
}

#endif // GLOB_H_INCLUDED
