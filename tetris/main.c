#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#ifdef __APPLE__
    #include <SDL2_image/SDL_image.h>
#elif
    #include <SDL2/SDL_image.h>
#endif
#include <time.h>

#define EMPTY 0
#define PERMBLOCK 1
#define BLOCK 2
#define TMPBLOCK 3

#define NONE 0
#define BLACK 1
#define RED 2
#define BLUE 3
#define GREEN 4
#define ORANGE 5
#define YELLOW 6

typedef struct blkslot {
    unsigned int state;
    unsigned int color;
    SDL_Rect block;
}blkslot;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* win;
SDL_Renderer* rendr;
SDL_Event event;
SDL_Surface* surf;
SDL_Texture* tex;
const Uint8 *keystates;

//12 across 23 high
//2 rows invisible at top, one row is bottom
blkslot board[276];

void cleanup() {
    SDL_DestroyRenderer(rendr);
    SDL_DestroyWindow(win);
}

void initboard() {
    int xoffset = 100;
    int yoffset = 440;
    int i;
    
    for (i = 0; i<252; i++) {
        board[i].block.x = xoffset + 21*(i%12);
        board[i].block.y = yoffset - 21*(i/12);
        board[i].block.w = 20;
        board[i].block.h = 20;
    }

    for (i = 0; i<23; i++) {
        int left = i*12;
        int right = left+11;

        board[left].state = PERMBLOCK;
        board[left].color = BLACK;

        board[right].state = PERMBLOCK;
        board[right].color = BLACK;

    }
    
    for (i = 1; i<11; i++) {
        board[i].state = PERMBLOCK;
        board[i].color = BLACK;

    }
}

void drawboard() {
    int i;
    for (i = 0; i<276; i++) {
        if (board[i].state != 10) {
            switch(board[i].color) {
                case (BLACK):
                    SDL_SetRenderDrawColor(rendr, 0,0,0,255);
                    break;
                default:
                    SDL_SetRenderDrawColor(rendr, 255,0,0,255);
                    break;
            }
            SDL_RenderFillRect(rendr, &board[i].block);
        }
    }
}

void game() {
    keystates = SDL_GetKeyboardState(NULL);

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                printf("Bye\n");
                return;
                break;
            default:
                break;
        }
    }

    SDL_SetRenderDrawColor(rendr, 255,255,255,255);
    SDL_RenderClear(rendr);

    drawboard();

    SDL_RenderPresent(rendr);

    SDL_Delay(16);
    game();
}

int main() {
    srand(time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Initialization error %s\n", SDL_GetError());
        return 1;
    }
    else {
        win = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(!win) {
            printf("Error creating window %s\n", SDL_GetError());
            return 1;
        }
        else {
            rendr = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
            if(!rendr) {
                printf("Error initializing renderer %s\n", SDL_GetError());
                return 1;
            }
        }
    }

    initboard();
    game();

    cleanup();

    SDL_Quit();

    return 0;
}
