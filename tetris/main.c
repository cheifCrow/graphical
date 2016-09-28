#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#ifdef __APPLE__
    #include <SDL2_image/SDL_image.h>
#elif
    #include <SDL2/SDL_image.h>
#endif
#include <time.h>

typedef struct blkslot {
    unsigned int occupied;
    unsigned int color;
}blkslot;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* win;
SDL_Renderer* rendr;
SDL_Event event;
SDL_Surface* surf;
SDL_Texture* tex;
const Uint8 *keystates;
blkslot board[276];

void cleanup() {
    SDL_DestroyRenderer(rendr);
    SDL_DestroyWindow(win);
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

    game();

    cleanup();

    SDL_Quit();

    return 0;
}
