SOURCE = main.c
OBJ = main.o
CFLAGS = -Wall
LIBS = -lSDL2

%.o: %.c
	gcc $(CFLAGS) -c  $< 

snow: $(OBJ)
	gcc $(CFLAGS) $^ $(LIBS) -o $@
