#include <stdbool.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include "gpu.h"




GPU* gpu_init(GPU* gpu){
	//TODO: tecnicamente anche sdl init non ha a che fare con la gpu, potrebbe avere senso spostarlo più avanti
	SDL_Window* window = SDL_CreateWindow( "CHIP-8",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			CHIP_SCREEN_WIDTH,
			CHIP_SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
	if (window==NULL) {
		printf("SDL could not create window! SDL:Error: %s\n",SDL_GetError() );
		return NULL;
	}
	SDL_Surface* screen_surface = SDL_GetWindowSurface(window);
	SDL_FillRect( screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xff,0xff));
	SDL_UpdateWindowSurface(window);
	gpu->window = window;
	gpu->screen_surface = screen_surface;
	gpu->renderer = SDL_CreateRenderer(gpu->window,-1, SDL_RENDERER_ACCELERATED);
	return gpu;
}
void gpu_loop(GPU* gpu){
	//TODO: ovviamente questo verrà spostato perché pollevents è qualcosa che non ha a che fare con la gpu
	            SDL_Event e;
		    bool quit = false;
		    gpu_draw(gpu);
		    while( quit == false ){ 
			    while( SDL_PollEvent( &e ) ){ 
				    if( e.type == SDL_QUIT ) quit = true; 
			    }
		    }
}


int gpu_close(GPU* gpu){
	SDL_DestroyWindow(gpu->window);
//TODO: Ricordarsi di spostare fuori
	SDL_Quit();
	return 0;

}

void gpu_draw_sprite(GPU* gpu, GPU_sprite* sprite) {
	uint8_t* ptr = sprite->ptr;
	//Weird but ok
	unsigned int startx = sprite->x % GPU_WIDTH;
	unsigned int starty = sprite->y %GPU_HEIGHT;
	for (unsigned int count = 0 ; count < sprite->height; count++){
		for (int i = 7; i>=0 ; i--){
			if ((starty + count) < GPU_HEIGHT &&(startx + 7-i )< GPU_WIDTH &&((*ptr) & (1<<i))!=0){
				unsigned int coord = (startx +7-i) + (starty + count)*GPU_WIDTH;
				gpu->mem[coord] = !gpu->mem[coord];
			}
		}

		//this should work, right?
		ptr++;
	}
	gpu_draw(gpu);
}
void gpu_draw(GPU* gpu){
	//tecnicamente posso evitare di ricolorare sempre, ma non so se ne valga la pena
	
	SDL_SetRenderDrawColor(gpu->renderer, 0x00,0x00,0x00,0xFF);
	SDL_RenderClear(gpu->renderer);

	SDL_Rect rect = {.x=0, .y=0, .w=10,.h=10};
	//TODO: Occhio che così non è del tutto corretto eh
	SDL_SetRenderDrawColor(gpu->renderer, 0xFF,0xFF,0xFF,0xFF);
	for (unsigned int i = 0 ; i < GPU_SIZE; i++){
		if (gpu->mem[i] == 1){
			rect.x = 10* (i % GPU_WIDTH);
			rect.y = 10* (i / GPU_WIDTH);
			SDL_RenderFillRect(gpu->renderer,&rect);
		}
	}
	SDL_RenderPresent(gpu->renderer);

}

void gpu_clear(GPU* gpu){
	//should I do this the hacky, efficient way or the fancy, less efficient one?
	memset(gpu->mem,0,sizeof(uint8_t)*GPU_SIZE); 
	SDL_SetRenderDrawColor(gpu->renderer, 0x00,0x00,0x00,0xFF);
	SDL_RenderClear(gpu->renderer);
	SDL_RenderPresent(gpu->renderer);
}
