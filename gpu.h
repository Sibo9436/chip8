#ifndef GPU_H
#define GPU_H
#include <SDL2/SDL.h>

#define CHIP_SCREEN_WIDTH  640
#define CHIP_SCREEN_HEIGHT  320
#define GPU_SPRITE_SIZE 

//se non ho frainteso il funzionamento del chip8, la gpu è da considerarsi con una memoria a parte

// Tecnicamente parlando potrebbe essere una practice migliore quella di separare la gpu da uno 
// "schermo" in modo che lo schermo possa essere tecnicamente implementato da una libreria qualsiasi
// ma dato che il c manca di interfacce (credo) e non ho intenzione di usare altre librerie (almeno
// non nella versione in c comunque per ora me ne frego, in Rust senza dubbio sarà un'aggiunta importante
// Le interfacce si possono in un certo modo implementare con dei define ben utilizzati credo
//
#define GPU_WIDTH  64
#define GPU_HEIGHT  32
#define GPU_SIZE 2048

//static  int GPU_MEM = 2048;

typedef struct gpu{
	SDL_Window* window;
	SDL_Surface* screen_surface;
	SDL_Renderer* renderer;
	char  mem[GPU_SIZE];
}GPU;

typedef struct sprite{
	unsigned int x;
	unsigned int y;
	unsigned int height;
	//Living dangerously, this has to be a pointer to the cpu memory
	char* ptr;
}GPU_sprite;

GPU* gpu_init();
void gpu_loop(GPU* gpu);

int gpu_close(GPU* gpu);

void gpu_draw(GPU* gpu);

//tutti gli sprite hanno le stesse dimensioni quindi mi basta avere la loro posizione in memoria
//It is a bit risky ma credo funzioni senza problemi
//Il concetto è quello di usare i bit dello sprite per invertire quelli della gpu
//ora 
//io per ora la gpu uso un byte per punto (più per semplicità che per altro)
//immagino che a livello di efficienza usare un formato più compatto sarebbe più intelligente
//ma sono solo 2kb per ora posso comodamnente sbattermene
void gpu_draw_sprite(GPU* gpu, GPU_sprite* sprite);
void gpu_clear(GPU* gpu);

#endif // GPU_H
