#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <SDL.h>

SDL_Surface *blockImage[8];

void loadTextures()
{
	char file_buf[FILENAME_MAX];
    for (uint32_t file = 0; file < FILENAME_MAX; file++) {
        snprintf(file_buf, FILENAME_MAX, "%u.bmp", file);
        FILE *final = fopen(file_buf, "r");
        if (!final) {
            break;
        }
        fclose(final);
        blockImage[file] = IMG_Load(file_buf);
    }
}

#endif // TEXTURE_H_INCLUDED
