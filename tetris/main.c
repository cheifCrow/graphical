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

#define LEFT 1
#define RIGHT 2

//Define the type used for the game grid
typedef struct blkslot {
    Uint8 state;
    Uint8 color;
    SDL_Rect block;
}blkslot;

//The board coordinates of the currently falling piece
int current_blocks[4];
//Game board 12 across 23 high top 2 rows invisible, sides and bottom are permanent blocks
blkslot board[276];

//SDL graphics
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window* win;
SDL_Renderer* rendr;
SDL_Event event;
SDL_Surface* surf;
SDL_Texture* tex;

//Keyboard handling
const Uint8 *keystates;

//Timers
SDL_TimerID drop_timer;
Uint32 drop_speed = 1000;

SDL_TimerID horiz_timer;
Uint32 horiz_speed = 125;
Uint8 move_allowed = 1;

SDL_TimerID fast_fall_timer;
Uint32 fast_fall_speed = 60;
Uint8 fast_allowed = 1;

Uint8 failure = 0;

//Necessary forward declarations
Uint32 queue_drop(Uint32 interval, void *param);

void cleanup() {
    SDL_DestroyRenderer(rendr);
    SDL_DestroyWindow(win);
}

//Initialize the grid, and define bottom and sides
//Define how grid is drawn graphically
void init_board() {
    memset(&board[0], 0, sizeof(board));
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

//Remove full lines
void clear_lines() {
    Uint8 i,j;
    for(i=1; i<21; i++) {
        Uint8 row_check = 1;
        for(j=1; j<11; j++) {
            row_check = row_check && board[(i*12)+j].state;
        }
        if(row_check) {
            Uint8 k;
            Uint16 index;
            for(k=i; k<21; k++) {
                for(j=1; j<11; j++) {
                    index = (k*12)+j;
                    board[index].state = board[index+12].state;
                    board[index].color = board[index+12].color;
                }
            }
            i--;
        }
    }
    
}

//Spawn a new game piece at the top of the board
void spawn_new() {
    clear_lines();
    Uint8 spawn_point = 221;
    Uint8 i;
    for(i=0;i<4;i++) {
        if(board[spawn_point+(-12*i)].state != EMPTY)
            failure = 1;

        current_blocks[i] = spawn_point+(-12*i);
        board[spawn_point+(-12*i)].color = YELLOW;
        board[spawn_point+(-12*i)].state = TMPBLOCK;
    }
    current_blocks[0] = spawn_point;
    board[spawn_point].color = YELLOW;
    board[spawn_point].state = TMPBLOCK;
}

//Attempt to drop a block, and if not freeze the block and spawn the next one
void drop_block() {
    Uint8 i;
    Uint8 stop_fall;
    Uint8 tmp[4];

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
                tmp[i] = board[current_blocks[i]].color;
                board[current_blocks[i]].state = EMPTY;
                board[current_blocks[i]].color = NONE;
            }
        }
        for(i=0;i<4;i++) {
            if(current_blocks[i] != -1) {
                board[current_blocks[i]-12].state = TMPBLOCK;
                board[current_blocks[i]-12].color = tmp[i];
                current_blocks[i] -= 12;
            }
        }
    }

    if(!failure)
        drop_timer = SDL_AddTimer(drop_speed, queue_drop, NULL);

}

void move_piece(Uint8 direction) {
    Uint8 i;
    Uint8 no_move;
    Uint8 tmp[4];
    int offset;

    switch (direction) {
        case (LEFT):
            offset = -1;
            break;
        case (RIGHT):
            offset = 1;
            break;
        default:
            return;
            break;
    }

    for(i=0; i<4; i++) {
        if(current_blocks[i] != -1) {
            if(board[current_blocks[i]].state == TMPBLOCK && 
                    (board[current_blocks[i]+offset].state == BLOCK || 
                     board[current_blocks[i]+offset].state == PERMBLOCK)) {
                
                no_move = 1;
            }
        }
    }
    if(!no_move) {
        for(i=0;i<4;i++) {
            if(current_blocks[i] != -1) {
                tmp[i] = board[current_blocks[i]].color;
                board[current_blocks[i]].color = NONE;
                board[current_blocks[i]].state = EMPTY;
            }
        }
        for(i=0;i<4;i++) {
            if(current_blocks[i] != -1) {
                board[current_blocks[i]+offset].state = TMPBLOCK;
                board[current_blocks[i]+offset].color = tmp[i];
                current_blocks[i] += offset;
            }
        }
    }
}

//When the drop timer expires, queue the drop block event
Uint32 queue_drop(Uint32 interval, void *param) {
    SDL_UserEvent e;
    e.type = SDL_USEREVENT;
    e.code = FUNCTION;
    e.data1 = &drop_block;

    event.type = SDL_USEREVENT;
    event.user = e;

    SDL_PushEvent(&event);
    return(0);
}

//Render the current grid to the window
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

//Allow horizontal movement
Uint32 allow_movement(Uint32 interval, void *param) {
    move_allowed = 1;
    return(0);
}

//Allow fast falling
Uint32 allow_fast(Uint32 interval, void *param) {
    fast_allowed = 1;
    return(0);
}

//What happens when you lose
void game_over() {
    failure = 0;
    init_board();
    spawn_new();
    drop_timer = SDL_AddTimer(drop_speed, queue_drop, NULL);
}

//Main game loop
void game() {
    //Event handling
    keystates = SDL_GetKeyboardState(NULL);
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                printf("Bye\n");
                return;
                break;
            case SDL_USEREVENT:
                switch (event.user.code) {
                    case FUNCTION: 
                        {
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

    //Key handling
    if (keystates[SDL_SCANCODE_DOWN] && fast_allowed) {
        fast_allowed = 0;
        SDL_RemoveTimer(drop_timer);
        fast_fall_timer = SDL_AddTimer(fast_fall_speed, allow_fast, NULL);
        drop_block();
    }
    else if (keystates[SDL_SCANCODE_LEFT] && !keystates[SDL_SCANCODE_RIGHT] && move_allowed) {
        move_allowed = 0;
        horiz_timer = SDL_AddTimer(horiz_speed, allow_movement, NULL);
        move_piece(LEFT);
    }
    else if (keystates[SDL_SCANCODE_RIGHT] && !keystates[SDL_SCANCODE_LEFT] && move_allowed) {
        move_allowed = 0;
        horiz_timer = SDL_AddTimer(horiz_speed, allow_movement, NULL);
        move_piece(RIGHT);
    }
    else if (keystates[SDL_SCANCODE_UP]) {
    }

    //Render Frame
    SDL_SetRenderDrawColor(rendr, 255,255,255,255);
    SDL_RenderClear(rendr);

    draw_board();

    SDL_RenderPresent(rendr);

    if(failure)
        game_over();

    //Limit frame rate to 60fps
    SDL_Delay(16);
    game();
}

int main() {
    //Initialization
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
    drop_timer = SDL_AddTimer(drop_speed, queue_drop, NULL);
    
    //Run the game
    game();

    cleanup();

    SDL_Quit();

    return 0;
}
