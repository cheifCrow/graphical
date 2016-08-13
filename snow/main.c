#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

typedef struct {
	int value;
	void* next;
	void* prev;
} llmember;

llmember* head = 0;
llmember* tail = 0;

void printll (llmember *index) {
	printf("%d\n", index->value);
	if(index->next) {
		printll((llmember*)(index->next));
	}
	return;
}

void genll (int num) {
	if(num) {
		llmember* tmp = (llmember*)calloc(1,sizeof(llmember));
		if(!head) {
			head = tmp;
		}
		if(!tail) {
			tail = tmp;
		}
		else {
			tail->next = tmp;
			tmp->prev = tail;
			tail = tmp;
		}
		tmp->value = rand();
		tmp->next = 0;
		num--;
		genll(num);

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

int main() {
	srand(time(NULL));

	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;

	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Initialization error %s\n", SDL_GetError());
		return 1;
	}
	else {
		window = SDL_CreateWindow("Snow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(!window) {
			printf("Error creating window %s\n", SDL_GetError());
		}
		else {
			surface = SDL_GetWindowSurface(window);
			SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
			SDL_UpdateWindowSurface(window);
			SDL_Delay(2000);
		}
	}

	genll(3);

	printf("Test of the Linked List System\n");

	printll(head);
	SDL_DestroyWindow(window);
	freell(head);
	SDL_Quit();

	return 0;
}
