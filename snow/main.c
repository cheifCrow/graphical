#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

typedef struct {
	int down,horiz,r,g,b;
	void* next;
	void* prev;
	SDL_Rect flake;
} llmember;

llmember* head = 0;
llmember* tail = 0;

SDL_Window* window = NULL;
SDL_Surface* surface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Event event;

void printll (llmember *index) {
	printf("x %d y %d w %d h %d\n", index->flake.x, index->flake.y,
			index->flake.w, index->flake.h);
	if(index->next) {
		printll((llmember*)(index->next));
	}
	return;
}

void genll (int num) {
	if(num) {
		llmember* tmp = (llmember*)calloc(1,sizeof(llmember));
		
		//The head object functions as a safety
		//It wont be removed by garbage cleanup so it protects against a seg fault
		if(!head && !tail) {
			head = tmp;
			tail = tmp;
			genll(num);
		}
		else {
			tail->next = tmp;
			tmp->prev = tail;
			tail = tmp;
			tmp->flake.x = rand() % SCREEN_WIDTH;
			tmp->flake.y = (rand() % SCREEN_HEIGHT)*-1;
			tmp->flake.w = (rand() % 4) + 2;
			tmp->flake.h = tmp->flake.w;
			tmp->r = rand() % 256;
			tmp->g = rand() % 256;
			tmp->b = rand() % 256;
			tmp->down = (rand() %4) + 2;
			tmp->horiz = (rand() % 2)+1;
			if(rand() % 2) {
				tmp->horiz = tmp->horiz*-1;
			}
			num--;
			genll(num);
		}

	}
	else {
		return;
	}
}

void freell(llmember* index) {
	if((llmember*)(index->next)) {
		freell(index->next);
	}
	free(index);
	return;
}

void delete_llmember(llmember* index) {
	if(index == head) {
		head = index->next;
		((llmember*)(index->next))->prev = 0;
	}
	else if(index == tail) {
		tail = index->prev;
		((llmember*)(index->prev))->next = 0;
	}
	else {
		((llmember*)(index->next))->prev = index->prev;
		((llmember*)(index->prev))->next = index->next;
	}

	free(index);
	return;

}

void physics_snow(llmember* index) {
	index->flake.y += index->down;
	index->flake.x += index->horiz;
	llmember* tmp = index->next;

	if(index->flake.y > SCREEN_HEIGHT) {
		delete_llmember(index);
	}
	else if(index->flake.x < (0-index->flake.w)) {
		delete_llmember(index);
	}
	else if(index->flake.x > SCREEN_WIDTH) {
		delete_llmember(index);
	}

	if(tmp) {
		physics_snow(tmp);
	}
	else {
		return;
	}

}

void draw_snow(llmember* index) {
	SDL_SetRenderDrawColor (renderer, index->r, index->g, index->b, 255);
	SDL_RenderFillRect (renderer, &(index->flake));
	if((llmember*)(index->next)) {
		draw_snow(index->next);
	}
	else {
		return;
	}
}

void graphicsLoop() {
	genll(100);
	int t, t2;
	while(1) {
		t = SDL_GetTicks();
		while (SDL_PollEvent(&event)){
			switch (event.type){
				case SDL_QUIT:
					printf("Aborting\n");
					freell(head);
					return;
					break;
					
				default:
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		physics_snow(head);
		draw_snow(head);

		SDL_RenderPresent(renderer);

		genll(5);

		t2 = SDL_GetTicks();
		t = 16-(t2-t);
		if(t < 0) {
			printf("Warning your computer cannot sustain a stable framerate\n");
		}
		else {
			SDL_Delay(t);
		}		
	}
}

int main() {
	srand(time(NULL));
	
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Initialization error %s\n", SDL_GetError());
		return 1;
	}
	else {
		window = SDL_CreateWindow("Snow", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(!window) {
			printf("Error creating window %s\n", SDL_GetError());
			return 1;
		}
		else {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if(!renderer) {
				printf("Error initializing renderer %s\n", SDL_GetError());
				return 1;
			}

		}
	}

	graphicsLoop();

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
