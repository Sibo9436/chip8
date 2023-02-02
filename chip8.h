#ifndef CHIP8_H
#define CHIP8_H

#define CHIP_TRUE 1
#define CHIP_FALSE 0
#define CHIP_MEM_SIZE 4096
#include <SDL.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include"gpu.h"

typedef struct chip8{
	GPU gpu;
	char mem[CHIP_MEM_SIZE];
	uint16_t  pc;//program counter
	uint16_t i; //Index register
	//something needs to be done with the delay and sound timers :)
	uint8_t v[16]; //variable registers
	//C'è uno stack usato esclusivamente per ritornare dalle subroutines
	//Devo decidere se implementarlo fisso o variabile e se magari può avere senso metterlo nello heap;
	//La specifica che ho guardato indica "almeno 16 numeri da 16bit"
	uint16_t stack[16];
	uint8_t stackptr;
	uint8_t delay_timer;
	uint8_t sound_timer;
	
	
}CHIP8;
CHIP8 chip;

//TODO: A questo punto potrebbe essere semplicemente più intelligente restituire un oggetto err(come in go)
//IDEA: tecnicamente si potrebbe anche pensare di avere solo un "singleton" CHIP8
//e quindi lasciare il controllo della memoria a questo file 
//main a quel punto chiamrebbe solo le funzioni di inizializzazione, loop eccetera
//un po' come funziona sdl in parte almeno credo
//e a questo punto le funzioni non sarebbero più costrette a restituire null
//ma solo dei codici di errore


int chip8_init();
int chip8_load(char*, long);
void chip8_loop();
void chip8_close();



#endif //CHIP8_H
