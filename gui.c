#include "gui.h"
#include <SDL2/SDL.h>
#define GUI_SCREEN_WIDTH  (640 *2)
#define GUI_SCREEN_HEIGHT  (400 * 2)
#define GUI_TEXTURE_WIDTH 320
#define GUI_TEXTURE_HEIGHT 100
SDL_Window* gui_window;
SDL_Surface* gui_surface;
SDL_Renderer* gui_renderer;
SDL_Texture* gui_texture;
void gui_init(){
	//TODO: tecnicamente anche sdl init non ha a che fare con la gpu, potrebbe avere senso spostarlo più avanti
	gui_window = SDL_CreateWindow( "CHIP-8",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			GUI_SCREEN_WIDTH,
			GUI_SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
	if (gui_window==NULL) {
		printf("SDL could not create window! SDL:Error: %s\n",SDL_GetError() );
		return ;
	}
	gui_surface = SDL_GetWindowSurface(gui_window);
	//SDL_FillRect( screen_surface, NULL, SDL_MapRGB(screen_surface->format, 0xFF, 0xff,0xff));
	//SDL_UpdateWindowSurface(window);
	gui_renderer = SDL_CreateRenderer(gui_window,-1, SDL_RENDERER_ACCELERATED);
	gui_texture = SDL_CreateTexture(gui_renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING, GUI_TEXTURE_WIDTH,GUI_TEXTURE_HEIGHT);
	return ;
}
void gui_close(){
	SDL_DestroyTexture(gui_texture);
	SDL_DestroyRenderer(gui_renderer);
	SDL_DestroyWindow(gui_window);
	gui_texture =NULL;
	gui_renderer =NULL;
	gui_window =NULL;
}
void gui_draw_memory(uint8_t* memory){
	SDL_SetRenderDrawColor(gui_renderer, 0x00,0x00,0x00,0xFF);
	SDL_RenderClear(gui_renderer);
	uint8_t* pixels;
	int pitch;
	//Se ho capito bene potrei anche usare una surface qui ma
	// la texture è più sdl2
	SDL_LockTexture(gui_texture,NULL, (void**)&pixels, &pitch);
	SDL_Rect dest = {.x=0, .y=0, .w=GUI_SCREEN_WIDTH,.h=GUI_SCREEN_HEIGHT};
	//TODO: Occhio che così non è del tutto corretto eh
	SDL_SetRenderDrawColor(gui_renderer, 0xFF,0xFF,0xFF,0xFF);
	unsigned int px = 0 ;
	unsigned int py = 0 ;
	for (unsigned int i = 0; i < 4000; ++i){
		
		for (unsigned int j = 0 ; j< 8; ++j){
			uint8_t val = (((memory[i] << j ) & 0x80) != 0 )?0xFF:0;
			unsigned int pos = (j+px) * 4 + py * pitch;
			pixels[pos] = val;
			pixels[++pos] = val;
			pixels[++pos] = val;
			pixels[++pos] = val;

		}
		py++;
		if (py % GUI_TEXTURE_HEIGHT == 0){
			py = 0;
			px+=8;
		}
	}
	SDL_UnlockTexture(gui_texture);
	SDL_RenderCopy(gui_renderer,gui_texture,NULL, &dest);
	SDL_RenderPresent(gui_renderer);
}
