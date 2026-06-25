
#include <time.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

Uint32 mytimer(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;   /* wait this long till next call */
}

struct Item
{
    char id;
    char count;
    //int durability;
};

struct Projectile
{
    float x, y, dx, dy;
    int id;
    struct Item inventory;
};

//SET UP LISTS
struct Entity
{
    float x, y, dx, dy, xAccel, xSize, ySize, dxMod;
    int uuid, xDirection, hotbarSelect, health;
    struct Item inventory[9][4];
};

struct blockEntity
{
    int uuid;
    struct Item inventory[9][4];
};

    struct Blocks
    {
        int id;
        int lightLevel;
        int damageLevel;
    };
    //NEED seed system
    struct Chunks
    {
        struct Blocks block[16][16];
    }chunk[16][16];

void addItemToInventory(struct Entity *targetEntity, int itemId, int itemCount)
{
    int firstSpot = -1;
    //for(int i = 0;) //Loop through entity's inventory, find first same not full item stack or first available empty spot
}

int main(int argc, char *argv[])
{
    srand (time(NULL));

    const int WIDTH = 800, HEIGHT = 600;
    const float GRAVITY = 0.008;

    int spawnY = 64;
    int debugMode = 1; //0 no debug, 1 variables, 2 screen position/rendering
    int scene = 0; //0 title screen/controls, 1 game, 2 pause screen
    int blockSize = 25;
    int mouseX = 0;
    int mouseY = 0;
    int worldTime = 10;
    float zoomLevel = 1;
    float zoomPress = 1;
    bool inventoryPress = false, leftPress = false, rightPress = false, jumpPress = false;
    bool canMoveUp, canMoveDown, canMoveLeft, canMoveRight;

    struct
    {
        float zoomLevel;
    }camera;

    SDL_Event event;
    SDL_Surface *screen;
    SDL_Surface *blockImage[8];

    /* init SDL and open a window */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, SDL_ANYFORMAT);
    /* adding a timer for 20 ms; 1000 ms / 20 ms -> 50 fps */
    SDL_WM_SetCaption("Terraria", "Terraria");
    SDL_TimerID id = SDL_AddTimer(17, mytimer, NULL);


    //Load textures
    for(int i = 0; i < 3; i++)
    {
    }
    blockImage[1] = IMG_Load("1.bmp");
    blockImage[2] = IMG_Load("2.bmp");
    blockImage[3] = IMG_Load("3.bmp");
    blockImage[4] = IMG_Load("4.bmp");
    blockImage[5] = IMG_Load("5.bmp");
    blockImage[6] = IMG_Load("6.bmp");
    blockImage[7] = IMG_Load("7.bmp");

    struct Entity player[1];

    //Load world or create world file
    FILE *worldFile  = fopen("world.txt", "r+");
    if(!worldFile)
    {
        //world gen
        for(int p = 0; p < 1; p++)
        {
            player[p].x = 0;
            player[p].y = spawnY;
            player[p].dx = 0;
            player[p].dy = 0;
            player[p].xDirection = -1;
            player[p].xSize = 2;
            player[p].ySize = 2;
            player[p].dxMod = 1;
            player[p].hotbarSelect = 0;
            player[p].health = 20;
            for(int i = 0; i < 9; i++)
            {
                for(int j = 0; j < 4; j++)
                {
                    player[p].inventory[i][j].id = 0;
                    player[p].inventory[i][j].count = 0;
                    if(j == 0)
                    {
                        player[p].inventory[i][j].id = i+1;
                        player[p].inventory[i][j].count = 16;
                    }
                }
            }
        }

        int randomOffset = 0;
        for(int chunkIndexX = 0; chunkIndexX < 16; chunkIndexX++)
            for(int chunkIndexY = 0; chunkIndexY < 16; chunkIndexY++)
                for(int i = 0; i < 16; i++)
                {
                    randomOffset = 3*sin(i/2);
                    for(int j = 0; j < 16; j++)
                    {
                        if(j < 5 + randomOffset && chunkIndexY == 3)
                        {
                            chunk[chunkIndexX][chunkIndexY].block[i][j].id = 1;
                        }
                        else if (j == 5 + randomOffset && chunkIndexY == 3)
                        {
                            chunk[chunkIndexX][chunkIndexY].block[i][j].id = 2;
                        }
                        else if (chunkIndexY < 3)
                        {
                            chunk[chunkIndexX][chunkIndexY].block[i][j].id = 1;
                        }
                        else
                        {
                            chunk[chunkIndexX][chunkIndexY].block[i][j].id = 0;
                        }
                        chunk[chunkIndexX][chunkIndexY].block[i][j].lightLevel = 0;
                    }
                }
    }
    else
    {
        //Load file
    }

    bool gameLoop = true;
    while (gameLoop)
    {
        switch(scene)
        {
            //Title screen
            case 0:
                SDL_WaitEvent(&event);
                switch(event.type)
                {
                    case SDL_MOUSEBUTTONDOWN:
                        scene = 1;
                        break;
                    case SDL_USEREVENT:
                        stringColor(screen, 0, 0, "Press Mouse 1 to continue", 0xFFFFFFFF);
                        stringColor(screen, 0, 20, "WASD to move", 0xFFFFFFFF);
                        stringColor(screen, 0, 30, "Space to jump", 0xFFFFFFFF);
                        stringColor(screen, 0, 40, "Shift to crouch/reduce speed", 0xFFFFFFFF);
                        stringColor(screen, 0, 50, "CTRL to sprint", 0xFFFFFFFF);
                        stringColor(screen, 0, 60, "1-9 to scroll through hotbar", 0xFFFFFFFF);
                        stringColor(screen, 0, 70, "Mouse 1 to break blocks in game", 0xFFFFFFFF);
                        stringColor(screen, 0, 80, "Mouse 2 to place blocks from inventory/hotbar", 0xFFFFFFFF);
                        stringColor(screen, 0, 90, "-/= to zoom in and out", 0xFFFFFFFF);
                        stringColor(screen, 0, 100, "Esc to pause", 0xFFFFFFFF);
                        stringColor(screen, 0, 110, "F3 to cycle through the debug menu", 0xFFFFFFFF);
                        break;
                    case SDL_QUIT:
                        gameLoop = false;
                        break;
                }
                SDL_Flip(screen);
                break;
            //Game
            case 1:
                SDL_WaitEvent(&event);
                char bufferThing[32];
                switch (event.type)
                {
                    case SDL_MOUSEBUTTONDOWN:
                        if(!inventoryPress)
                        {
                            switch (event.button.button)
                            {
                                case SDL_BUTTON_LEFT:
                                    addItemToInventory(&player[0],
                                        chunk[(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) / 16]
                                             [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) / 16].block
                                             [(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) % 16]
                                             [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) % 16].id, 1);
                                    chunk[(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) / 16]
                                         [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) / 16].block
                                         [(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) % 16]
                                         [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) % 16].id = 0;
                                    break;
                                case SDL_BUTTON_RIGHT:
                                    if(chunk[(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) / 16]
                                            [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) / 16].block
                                            [(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) % 16]
                                            [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) % 16].id == 0 &&
                                            player[0].inventory[player[0].hotbarSelect][0].count > 0)
                                    {
                                         chunk[(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) / 16]
                                              [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) / 16].block
                                              [(int)((event.button.x - WIDTH/2)/(blockSize*zoomLevel) + player[0].x + player[0].xSize/2) % 16]
                                              [(int)((HEIGHT/2 - event.button.y)/(blockSize*zoomLevel) + player[0].y + player[0].ySize/2) % 16].id = player[0].inventory[player[0].hotbarSelect][0].id;
                                         player[0].inventory[player[0].hotbarSelect][0].count--;
                                         if(player[0].inventory[player[0].hotbarSelect][0].count == 0) player[0].inventory[player[0].hotbarSelect][0].id = 0;
                                    }
                                    break;
                            }
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        switch (event.button.button)
                        {
                            case SDL_BUTTON_LEFT:
                                break;
                            case SDL_BUTTON_RIGHT:
                                break;
                        }
                        break;
                    case SDL_MOUSEMOTION:
                        mouseX = event.button.x;
                        mouseY = HEIGHT-event.button.y;
                        break;
                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym)
                        {
                            case ' ':
                                jumpPress = 1;
                                break;
                            case 'a':
                                leftPress = 1;
                                break;
                            case 'd':
                                rightPress = 1;
                                break;
                            case '-':
                                zoomPress = 0.995;
                                break;
                            case '=':
                                zoomPress = 1.005;
                                break;
                            case 'e':
                                inventoryPress = (inventoryPress+1) % 2;
                                break;
                            case 'r':
                                player[0].y = spawnY;
                                break;
                            case 284: //F3
                                debugMode = (debugMode + 1) % 3;
                                break;
                            case 27: //Esc
                                scene = 2;
                                break;
                            case 304: //SHIFT
                                player[0].dxMod = 0.5;
                                break;
                            case 306: //CTRL
                                player[0].dxMod = 1.5;
                                break;
                            case 8: //Backspace
                                zoomLevel = 1;
                                break;
                            default:
                                if(event.key.keysym.sym > '0' && event.key.keysym.sym <= '9')
                                    player[0].hotbarSelect = event.key.keysym.sym-1-'0';
                                break;
                        }
                        sprintf(bufferThing, "Last key pressed: %d", event.key.keysym.sym);
                        break;
                    case SDL_KEYUP:
                        switch(event.key.keysym.sym)
                        {
                            case ' ':
                                jumpPress = 0;
                                break;
                            case 'a':
                                leftPress = 0;
                                break;
                            case 'd':
                                rightPress = 0;
                                break;
                            case 304: //S
                                player[0].dxMod = 1;
                                break;
                            case 306: //CTRL
                                player[0].dxMod = 1;
                                break;
                            case '-':
                            case '=':
                                zoomPress = 1;
                            default:
                                break;
                        }
                        break;
                    case SDL_USEREVENT:
                        //Physics

                        //SET UP RECURSION
                        //Y Movement
                        canMoveUp = true;
                        for(int i = 0; i < player[0].ySize+1; i++)
                        {
                            if(chunk[((int)(player[0].x + 0.1 + i*0.9) / 16)]
                                    [(int)((player[0].y + player[0].dy + player[0].ySize + 0.05) / 16)].
                               block[((int)(player[0].x + 0.1 + i*0.9) % 16)]
                                    [((int)(player[0].y + player[0].dy + player[0].ySize + 0.05) % 16)].id > 0)
                                canMoveUp = false;
                        }
                        if(!canMoveUp)
                        {
                            if(player[0].dy > 0) player[0].dy = 0;
                            player[0].y = round(player[0].y);
                        }

                        canMoveDown = true;
                        for(int i = 0; i < player[0].ySize+1; i++)
                        {
                            if(chunk[((int)(player[0].x + 0.1 + i*0.9) / 16)]
                                    [(int)((player[0].y + player[0].dy - 0.05) / 16)].
                               block[((int)(player[0].x + 0.1 + i*0.9) % 16)]
                                    [((int)(player[0].y + player[0].dy - 0.05) % 16)].id > 0)
                                canMoveDown = false;
                        }
                        if(!canMoveDown)
                        {
                            if(player[0].dy < -0.4) player[0].health += round(5*(player[0].dy));
                            player[0].dy = 0;
                            player[0].y = round(player[0].y);
                            if(jumpPress == 1 && canMoveUp) player[0].dy = 0.3;
                        }
                        else
                        {
                            player[0].dy -= GRAVITY;
                        }

                        if(canMoveUp || canMoveDown) player[0].y += player[0].dy;

                        //X Movement
                        canMoveLeft = true;
                        for(int i = 0; i < player[0].xSize+1; i++)
                        {
                            if(chunk[((int)(player[0].x - 0.05) / 16)]
                                    [(int)((player[0].y + i*0.9 + 0.1) / 16)].
                               block[((int)(player[0].x - 0.05) % 16)]
                                    [((int)(player[0].y + i*0.9 + 0.1) % 16)].id > 0 || player[0].x -0.05 < 0)
                                    canMoveLeft = false;
                        }
                        if(!canMoveLeft)
                        {
                            player[0].x = round(player[0].x);
                        }

                        canMoveRight = true;
                        for(int i = 0; i < player[0].xSize+1; i++)
                        {
                            if(chunk[((int)(player[0].x + 0.05 + player[0].xSize) / 16)]
                                    [(int)((player[0].y + i*0.9 + 0.1) / 16)].
                               block[((int)(player[0].x + 0.05 + player[0].xSize) % 16)]
                                    [((int)(player[0].y + i*0.9 + 0.1) % 16)].id > 0 || player[0].x +0.05 > 256)
                                    canMoveRight = false;
                        }
                        if(!canMoveRight)
                        {
                            player[0].x = round(player[0].x);
                        }

                        if(canMoveLeft && rightPress == 0 && leftPress == 1) player[0].dx = -0.1;
                        else if(canMoveRight && rightPress == 1 && leftPress == 0) player[0].dx = 0.1;
                        else player[0].dx = 0;
                        player[0].x += player[0].dx*player[0].dxMod;

                        if(player[0].health <= 0)
                        {
                            player[0].health = 0;
                            gameLoop = 0;
                            break;
                        }

                        //Camera Zoom
                        if((zoomLevel > 0.1 && zoomPress < 1) || (zoomLevel < 2 && zoomPress > 1)) zoomLevel *= zoomPress;
                        if(zoomLevel > 2) zoomLevel = 2;
                        if(zoomLevel < 0.1) zoomLevel = 0.1;

                        //Draw Sky
                        boxColor(screen, 0, 0, WIDTH, HEIGHT, 0x0094FF);

                        //Blocks
                        for(int i = 0; i <= WIDTH/(blockSize*zoomLevel) + 1; i++)
                        {
                            for(int j = 0; j <= HEIGHT/(blockSize*zoomLevel) + 1; j++)
                            {
                                int xCalc = i+(int)player[0].x-WIDTH/(2*blockSize*zoomLevel)+player[0].ySize/2;
                                int yCalc = j+(int)player[0].y-HEIGHT/(2*blockSize*zoomLevel)+player[0].ySize/2;
                                if(xCalc >= 0 && yCalc >= 0 && xCalc < 256 && yCalc < 256 && chunk[xCalc/16][yCalc/16].block[xCalc%16][yCalc%16].id != 0)
                                {
                                    if(chunk[xCalc/16][yCalc/16].block[xCalc%16][yCalc%16].lightLevel + worldTime > 0)
                                    {
                                        SDL_Rect dest_pos = { (i - fmod(player[0].x, 1.0))*zoomLevel*blockSize, HEIGHT-(j - fmod(player[0].y, 1.0) + player[0].ySize/2)*zoomLevel*blockSize, 0, 0 };
                                        SDL_BlitSurface(blockImage[chunk[xCalc/16][yCalc/16].block[xCalc%16][yCalc%16].id], NULL, screen, &dest_pos);
                                    }
                                    boxColor(screen, (i - fmod(player[0].x, 1.0))*zoomLevel*blockSize,
                                             HEIGHT -(j - fmod(player[0].y, 1.0))*zoomLevel*blockSize,
                                                     (i+1 - fmod(player[0].x, 1.0))*zoomLevel*blockSize,
                                             HEIGHT -(j+1 - fmod(player[0].y, 1.0))*zoomLevel*blockSize,
                                             0x000000FF-16*(chunk[xCalc/16][yCalc/16].block[xCalc%16][yCalc%16].lightLevel + worldTime));
                                }
                            }
                        }

                        //Player
                        boxColor (screen, (WIDTH-player[0].xSize*zoomLevel*blockSize)/2,
                                          (HEIGHT+player[0].ySize*zoomLevel*blockSize)/2,
                                          (WIDTH-player[0].xSize*zoomLevel*blockSize)/2+blockSize*player[0].xSize*zoomLevel,
                                          (HEIGHT+player[0].ySize*zoomLevel*blockSize)/2-blockSize*player[0].ySize*zoomLevel, 0x000000FF);
                        boxColor (screen, (WIDTH-player[0].xSize*zoomLevel*blockSize)/2+5*zoomLevel,
                                          (HEIGHT+player[0].ySize*zoomLevel*blockSize)/2-5*zoomLevel,
                                          (WIDTH-player[0].xSize*zoomLevel*blockSize)/2+blockSize*player[0].xSize*zoomLevel-5*zoomLevel,
                                          (HEIGHT+player[0].ySize*zoomLevel*blockSize)/2-blockSize*player[0].ySize*zoomLevel+5*zoomLevel, 0xFFFFFFFF);

                        //Hotbar and inventory
                        boxColor(screen, (WIDTH-9*50)/2+player[0].hotbarSelect*50, HEIGHT,
                                         (WIDTH-9*50)/2+(1+player[0].hotbarSelect)*50, HEIGHT - 50, 0x000000FF);
                        for(int j = 0; j < 1 + 3*inventoryPress; j++)
                        {
                            for(int i = 0; i < 9; i++)
                            {
                                boxColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT -j*50, (WIDTH-9*50)/2+i*50 + 50, HEIGHT -j*50 - 50, 0x88888888);
                                if(player[0].inventory[i][j].id != 0 && player[0].inventory[i][j].id < 8)
                                {
                                    SDL_Rect dest_pos = { (WIDTH-9*50)/2+i*50, HEIGHT -j*50 - 50, 0, 0 };
                                    SDL_BlitSurface(blockImage[player[0].inventory[i][j].id], NULL, screen, &dest_pos);
                                    char textBuffer[16];
                                    sprintf(textBuffer, "%d", player[0].inventory[i][j].count);
                                    stringColor(screen, (WIDTH-9*50)/2+i*50, HEIGHT -j*50 - 10, textBuffer, 0xFFFFFFFF);
                                }
                            }
                        }

                        //Health bar
                        stringColor(screen, WIDTH-160, 0, "Health:", 0xFFFFFFFF);
                        boxColor(screen, WIDTH-100, 0, WIDTH, 10, 0x333333FF);
                        boxColor(screen, WIDTH-100, 0, WIDTH-5*(20-player[0].health), 10, 0xFF3333FF);

                        //Screen debugging
                        if(debugMode != 0) {
                            //Screen position debugging
                            if(debugMode == 2) {
                                boxColor(screen, 0, 0, WIDTH/2, HEIGHT/2, 0xFF000088);
                                boxColor(screen, WIDTH/2, HEIGHT/2, WIDTH, HEIGHT, 0x0000FF88);
                            }
                            //F3 menu
                            char textBuffer[32];
                            sprintf(textBuffer, "X: %f, Y: %f, Chunk X: %d, Chunk Y: %d", player[0].x, player[0].y, (int)((player[0].x) / 16), (int)((player[0].y) / 16));
                            stringColor(screen, 0, 0, textBuffer, 0xFFFFFFFF);
                            sprintf(textBuffer, "Zoom Level: %f", zoomLevel);
                            stringColor(screen, 0, 10, textBuffer, 0xFFFFFFFF);
                            stringColor(screen, 0, 20, bufferThing, 0xFFFFFFFF);
                            sprintf(textBuffer, "Mouse X: %d, Mouse Y: %d", mouseX, mouseY);
                            stringColor(screen, 0, 30, textBuffer, 0xFFFFFFFF);
                            sprintf(textBuffer, "Speed X: %f, Speed Y: %f", player[0].dx, player[0].dy);
                            stringColor(screen, 0, 40, textBuffer, 0xFFFFFFFF);
                        }

                        break;
                    case SDL_QUIT:
                        gameLoop = false;
                        break;
                }
                SDL_Flip(screen);
                break;
            //Pause screen
            case 2:
                SDL_WaitEvent(&event);
                switch(event.type)
                {
                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym)
                        {
                            case 27:
                                scene = 1;
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        break;
                    case SDL_USEREVENT:
                        boxColor(screen, WIDTH/2-3, HEIGHT/2-3, WIDTH/2+164, HEIGHT/2+10, 0x000000FF);
                        stringColor(screen, WIDTH/2, HEIGHT/2, "Press Esc to unpause", 0xFFFFFFFF);
                        break;
                    case SDL_QUIT:
                        gameLoop = false;
                        break;
                }
                SDL_Flip(screen);
                break;
        }
    }
    fprintf(worldFile, "%f\n%f\n%f\n%f\n", player[0].x, player[0].y, player[0].dx, player[0].dy);

    /* delete mytimer */
    SDL_RemoveTimer(id);
    SDL_Quit();
    return 0;
}
