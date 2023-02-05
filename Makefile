CC = gcc 
CC_FLAGS = -Wall -std="c99"
SDLFLAGS = -lSDL2 `sdl2-config --cflags --libs`
.PHONY = clean
# questa cosa si può migliorare di peso con tipo %.o:%.c
OBJS = chip8.o gpu.o gui.o

gpu.o : gpu.c
	$(CC) $(CC_FLAGS)  gpu.c  -c -o gpu.o $(SDLFLAGS)
chip8.o : chip8.c
	$(CC) $(CC_FLAGS)  chip8.c -c -o chip8.o $(SDLFLAGS)
gui.o : gui.c
	$(CC) $(CC_FLAGS)  gui.c -c -o gui.o $(SDLFLAGS)
build: $(OBJS)
	$(CC) $(CC_FLAGS)  main.c $(OBJS) -o bin/chip8 $(SDLFLAGS)

run: build
	bin/chip8
clean: 
	rm *.o bin/chip8
