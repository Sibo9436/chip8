//Dopo quasi un anno dall'ultima volta che ho scritto un programma in C
// Semplice emulatore/interprete del CHIP-8 per scaldare i muscoli prima di tornare sul progetto NES
// Da riscrivere poi almeno in Rust e in C++ (forse)

#include <stdio.h>
#include<memory.h>
#include "chip8.h"

int main(){

	printf("Ciao mondo\n");

	
	//tutto sto meraviglioso truschino verrà poi estratto no worries
	FILE*  fileptr;
	uint8_t *buffer;
	long filelen;
	//fileptr = fopen("ibm_logo.ch8","rb");
	//fileptr = fopen("test_opcode.ch8","rb");
	fileptr = fopen("test/test-suite.ch8","rb");
	fseek (fileptr,0, SEEK_END);
	filelen = ftell(fileptr);
	rewind(fileptr);
	printf("filesize is: %ld\n", filelen);

	buffer = (uint8_t*)malloc(filelen*sizeof(uint8_t));
	fread(buffer,filelen,1,fileptr);
	fclose(fileptr);
	//printf("FILE\n");
//	for (long i  = 0 ; i < filelen; i+=2)
//	{
//		printf("f[%x]=%x%x\n",i+0x200,buffer[i],buffer[i+1]);
//	}
	chip8_init();
	if (chip8_load(buffer,filelen) != 0){
		chip8_loop();
	}
	chip8_close();
	free(buffer);
	printf("\n");

	return 0;
}
