SOURCE = main.c
OBJ = main.o
CFLAGS = -Wall
CC = gcc

OS = $(shell uname)
ifeq ($(OS),Darwin)
	LIBS = -framework SDL2 -framework SDL2_image
else
    LIBS = -lSDL2 -lSDL2_image
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $<

tetris: $(OBJ)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(OBJ) tetris
