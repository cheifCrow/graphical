SOURCE = linkedlist.c main.c
OBJ = linkedlist.o main.o
CFLAGS = -Wall
LIBS = -lSDL2 -lSDL2_image

%.o: %.c
	gcc $(CFLAGS) -c  $< 

shooter: $(OBJ)
	gcc $(CFLAGS) $^ $(LIBS) -o $@
