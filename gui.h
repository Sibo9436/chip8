#ifndef CHIP8_GUI_H
#define CHIP8_GUI_H
#include<stdint.h>


void gui_init();
void gui_close();
void gui_draw_memory(uint8_t* memory);
#endif //CHIP8_GUI_H
