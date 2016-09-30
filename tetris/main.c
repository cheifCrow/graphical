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

#define FUNCTION 1

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
SDL_TimerID drop_timer;
Uint32 drop_time = 1000;
int current_blocks[4];
Uint32 movement_time = 50;

//12 across 23 high
//2 rows invisible at top, one row is bottom
blkslot board[276];

void cleanup() {
    SDL_DestroyRenderer(rendr);
    SDL_DestroyWindow(win);
}

void init_board() {
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

void spawn_new() {
    int spawn_point = 245;
    current_blocks[0] = spawn_point;
    board[spawn_point].color = YELLOW;
    board[spawn_point].state = TMPBLOCK;
}


void drop_block() {
    printf("Tick\n");
    Uint8 i;
    Uint8 stop_fall;

    for(i=0;i<4;i++) {
        if(current_blocks[i] != -1) {
            if (board[current_blocks[i]].state == TMPBLOCK) {
                if(board[current_blocks[i]-12].state == BLOCK || board[current_blocks[i]-12].state == PERMBLOCK) {
                    stop_fall = 1;
                }
            }
        }
    }

    if(stop_fall) {
        for(i=0;i<4;i++) {
            if (board[current_blocks[i]].state == TMPBLOCK) {
                board[current_blocks[i]].state = BLOCK;
                current_blocks[i] = -1;
            }
        }
        spawn_new();
    }
    else {
        for(i=0;i<4;i++) {
            if(current_blocks[i] != -1) {
                board[current_blocks[i]-12].state = board[current_blocks[i]].state;
                board[current_blocks[i]-12].color = board[current_blocks[i]].color;
                board[current_blocks[i]].state = EMPTY;
                board[current_blocks[i]].color = NONE;
                current_blocks[i] = current_blocks[i]-12;
            }
        }
    }

}

Uint32 queue_drop(Uint32 interval, void *param) {
    SDL_UserEvent e;
    e.type = SDL_USEREVENT;
    e.code = FUNCTION;
    e.data1 = &drop_block;

    event.type = SDL_USEREVENT;
    event.user = e;

    SDL_PushEvent(&event);
    return(drop_time);
}

void draw_board() {
    int i;
    for (i = 0; i<276; i++) {
        if (board[i].state != 10) {
            switch(board[i].color) {
                case (BLACK):
                    SDL_SetRenderDrawColor(rendr, 0,0,0,255);
                    break;
                case (YELLOW):
                    SDL_SetRenderDrawColor(rendr, 255,255,0,255);
                    break;
                default:
                    SDL_SetRenderDrawColor(rendr, 255,255,255,255);
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
            case SDL_USEREVENT:
                switch (event.user.code) {
                    case FUNCTION: {
                        void (*f)() = event.user.data1;
                        f();
                        break;
                    }
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    SDL_SetRenderDrawColor(rendr, 255,255,255,255);
    SDL_RenderClear(rendr);

    draw_board();

    SDL_RenderPresent(rendr);

    SDL_Delay(16);
    game();
}

int main() {
    srand(time(NULL));
    int i;
    for(i=0;i<4;i++) {
        current_blocks[i] = -1;
    }

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

    init_board();
    spawn_new();
    drop_timer = SDL_AddTimer(drop_time, queue_drop, NULL);
    game();

    cleanup();

    SDL_Quit();

    return 0;
}
