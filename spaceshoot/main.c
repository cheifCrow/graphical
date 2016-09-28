#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 800;

SDL_Window* win;
SDL_Renderer* renderer;
SDL_Event event;
SDL_Surface* surface;
SDL_Texture* tex;
const Uint8 *keystates;


void gameLoop() {
    SDL_Rect dst = {320,725,64,64};
    while(1) {
        keystates = SDL_GetKeyboardState(NULL);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    printf("Aborting\n");
                    return;
                    break;
                default:
                    break;
            }
        }

        if(keystates[SDL_SCANCODE_UP] && !keystates[SDL_SCANCODE_DOWN]){
            dst.y -= 5;
            if (dst.y < 0) 
                dst.y = 0;
        }
        if(keystates[SDL_SCANCODE_DOWN] && !keystates[SDL_SCANCODE_UP]){
            dst.y += 5;
            if (dst.y > SCREEN_HEIGHT-64)
                dst.y = SCREEN_HEIGHT-64;
        }
        if(keystates[SDL_SCANCODE_LEFT] && !keystates[SDL_SCANCODE_RIGHT]){
            dst.x -= 5;
            if (dst.x < 0)
                dst.x = 0;
        }
        if(keystates[SDL_SCANCODE_RIGHT] && !keystates[SDL_SCANCODE_LEFT]){
            dst.x += 5;
            if (dst.x > SCREEN_WIDTH-64)
                dst.x = SCREEN_WIDTH-64;
        }

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

}


int main() {
    srand(time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Initialization error %s\n", SDL_GetError());
        return 1;
    }
    else {
        win = SDL_CreateWindow("Space Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(!win) {
            printf("Error creating window %s\n", SDL_GetError());
            return 1;
        }
        else {
            renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
            if(!renderer) {
                printf("Error initializing renderer %s\n", SDL_GetError());
                return 1;
            }
        }
    }

    surface = IMG_Load("ship.png");
    tex = SDL_CreateTextureFromSurface(renderer, surface);

    gameLoop();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();

    return 0;
}
