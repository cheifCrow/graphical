SOURCE = main.c
OBJ = main.o
CFLAGS = -Wall
CC = gcc

OS = $(shell uname)
ifeq ($(OS),Darwin)
	LIBS = -framework SDL2
else
	LIBS = -lSDL2
endif

%.o: %.c
	$(CC) $(CFLAGS) -c  $< 

snow: $(OBJ)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f $(OBJ) snow
