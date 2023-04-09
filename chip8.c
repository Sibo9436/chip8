#include "chip8.h"
#include "gui.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MASK_NNN(x) (x & 0x0FFF)
#define MASK_NN(x) (x & 0x00FF)
#define MASK_N(x) (x & 0x000F)
#define MASK_X(x) ((x & 0x0F00) >> 8)
#define MASK_Y(x) ((x & 0x00F0) >> 4)

// thanks stackoverflow

#ifdef DEBUG
#define CHIP8_DEBUG 1
#else
#define CHIP8_DEBUG 0
#endif
#define debug_println(line)                                                    \
  do {                                                                         \
    if (CHIP8_DEBUG)                                                           \
      fprintf(stderr, line);                                                   \
  } while (0);
#ifdef DEBUG
#define debug_printf(fmt, ...)                                                 \
  do {                                                                         \
    if (CHIP8_DEBUG)                                                           \
      fprintf(stderr, fmt, __VA_ARGS__);                                       \
  } while (0)
#else
#define debug_printf(fmt, ...) ;
#endif

CHIP8 chip;
uint8_t font_array[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

enum CHIP_KEYBOARD_STATE { CHIP_KEY_UP, CHIP_KEY_DOWN, CHIP_KEY_RELEASED };
// mi sono reso conto di poterlo fare moooolto più semplicemente di così
// visto che il chip8 supporta un solo tasto premuto per volta
// mi basta tenere una variabile con lo stato e una con il valore del tasto
enum CHIP_KEYBOARD_STATE keyboard_state[17] = {0};

uint8_t CHIP8_key = 0;
enum CHIP_KEYBOARD_STATE CHIP8_key_state = CHIP_KEY_UP;
// TODO: da mettere da qualche parte pls

size_t chip8_get_key_index(SDL_Scancode code) {
  switch (code) {
  case SDL_SCANCODE_1:
    return 1;
  case SDL_SCANCODE_2:
    return 2;
  case SDL_SCANCODE_3:
    return 3;
  case SDL_SCANCODE_4:
    return 0xC;
  case SDL_SCANCODE_Q:
    return 0x4;
  case SDL_SCANCODE_W:
    return 5;
  case SDL_SCANCODE_E:
    return 6;
  case SDL_SCANCODE_R:
    return 0xD;
  case SDL_SCANCODE_A:
    return 7;
  case SDL_SCANCODE_S:
    return 8;
  case SDL_SCANCODE_D:
    return 9;
  case SDL_SCANCODE_F:
    return 0xE;
  case SDL_SCANCODE_Z:
    return 0xA;
  case SDL_SCANCODE_X:
    return 0;
  case SDL_SCANCODE_C:
    return 0xB;
  case SDL_SCANCODE_V:
    return 0xF;
  default:
    return 16;
  }
  // FIXME: remember to fix
}
int chip8_init() {
  srand(time(NULL));
  memset(chip.mem, 0, sizeof(uint8_t) * CHIP_MEM_SIZE);
  memset(chip.stack, 0, sizeof(uint8_t) * 6);
  chip.stackptr = 0;
  memset(chip.v, 0, sizeof(uint8_t) * 16);
  chip.pc = 0x200;

  printf("copying font memory\n");
  memcpy(&(chip.mem[0x050]), font_array, sizeof(uint8_t) * 80);

  printf("clearing gpu memory\n");
  memset(chip.gpu.mem, 0, sizeof(uint8_t) * GPU_SIZE);
  //	for (unsigned int i = 0; i<160; i++){
  //		printf("mem[%d] -> %u\n", i, chip.mem[i]);
  //	}
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not be initialized! SDL:Error: %s\n", SDL_GetError());
    // Extremely dangerous living
    return CHIP_FALSE;
  }
  GPU *ptr = gpu_init(&(chip.gpu));
  if (ptr == NULL) {
    return CHIP_FALSE;
  }
  // gui_init();
  return CHIP_TRUE;
}

// There's other ways I guess
// who cares
uint16_t chip8_fetch() {
  uint8_t first_byte = chip.mem[chip.pc++];
  uint8_t second_byte = chip.mem[chip.pc++];
  // numbers are stored big endiannly (lol)
  return ((uint16_t)first_byte << 8) + (uint16_t)second_byte;
}

// Ispirandomi a sdl chip_intruction sarà una union di tutte le istruzioni
// possibili in c++ o rust qui verrà il momento della porgrammazione a
// oggetti/composizione di interfacce :) Oppure potrei semplicemente fare girare
// qui le istruzioni hmmm
void chip8_dump_state() {
  // dumps cpu state to stdout (no memory)
  printf("PC: %d\n", chip.pc);
  printf("I:[\%d]", chip.i);
  printf(" v[");
  for (size_t i = 0; i < 16; i++) {
    printf("%u ", chip.v[i]);
  }
  printf("]\n");
}
void chip8_decode(uint16_t op) {
  debug_printf("Op: %x\n", op);
  switch (op & 0xF000) {
  case 0x0000: {
    if (op == 0x00E0) {
      gpu_clear(&chip.gpu);
    } else if (op == 0x00EE) {
      chip.pc = chip.stack[--chip.stackptr];
    }
    break;
  }
  case 0x1000: {
    chip.pc = MASK_NNN(op);
    break;
  }
  case 0x2000: // subroutine
  {
    // TODO:Dovrei veramente fare un check per gli stackoverflow!
    chip.stack[chip.stackptr++] = chip.pc;
    chip.pc = MASK_NNN(op);
    break;
  }
  case 0x3000: {
    // Skip if vx == NN
    chip.pc += (chip.v[MASK_X(op)] == MASK_NN(op)) * 2;
    break;
  }
  case 0x4000: {
    // Skip if vx != NN
    chip.pc += (chip.v[MASK_X(op)] != MASK_NN(op)) * 2;
    break;
  }
  case 0x5000: {
    // Skip if vx == vy
    chip.pc += (chip.v[MASK_X(op)] == chip.v[MASK_Y(op)]) * 2;
    break;
  }
  case 0x6000: // setta registro x a valore NN
  {
    debug_printf("op:%x, setting v[%u]=%u\n", op, (uint8_t)MASK_X(op),
                 MASK_NN(op));
    chip.v[MASK_X(op)] = (uint8_t)MASK_NN(op);
    break;
  }
  case 0x7000: // add !!Does not set the carry flag!!
  {
    debug_printf("op:%x, adding v[%u]+v[%u]\n", op, (uint8_t)MASK_X(op),
                 MASK_NN(op));
    chip.v[MASK_X(op)] += (uint8_t)MASK_NN(op);
    break;
  }
  case 0x8000: {
    switch (MASK_N(op)) {
    case 0x0:
      chip.v[MASK_X(op)] = chip.v[MASK_Y(op)];
      break;
    case 0x1:
      chip.v[MASK_X(op)] |= chip.v[MASK_Y(op)];
      break;
    case 0x2:
      chip.v[MASK_X(op)] &= chip.v[MASK_Y(op)];
      break;
    case 0x3:
      chip.v[MASK_X(op)] ^= chip.v[MASK_Y(op)];
      break;
    case 0x4: {
      uint16_t flag;
      if (chip.v[MASK_X(op)] > 255 - chip.v[MASK_Y(op)]) {
        flag = 1;
      } else {
        flag = 0;
      }
      chip.v[MASK_X(op)] += chip.v[MASK_Y(op)];
      chip.v[0xF] = flag;
      break;
    }
    case 0x5: {
      uint16_t flag;
      if (chip.v[MASK_X(op)] > chip.v[MASK_Y(op)]) {
        flag = 1;
      } else {
        flag = 0;
      }
      chip.v[MASK_X(op)] -= chip.v[MASK_Y(op)];
      chip.v[0xF] = flag;
      break;
    }
    case 0x6: {
      // Occhio che in base al tipo di interprete questa operazione cambia -.-
      // in versioni più vecchie copiava vy in vx prima di fare
      // noi inizieremo con questa versione, poi vedremo
      // scherzone, proviamo a passare alla nuova

      uint8_t x = MASK_X(op);
      // chip.v[x] = chip.v[MASK_Y(op)];
      uint16_t flag = ((chip.v[x] & 0x8) != 0);
      chip.v[x] >>= 1;
      chip.v[0xF] = flag;
      break;
    }
    case 0x7: {
      uint16_t flag;
      if (chip.v[MASK_Y(op)] > chip.v[MASK_X(op)]) {
        flag = 1;
      } else {
        flag = 0;
      }
      chip.v[MASK_X(op)] = chip.v[MASK_Y(op)] - chip.v[MASK_X(op)];
      chip.v[0xF] = flag;
      break;
    }
    case 0xE: {
      // Occhio che in base al tipo di interprete questa operazione cambia -.-
      // in versioni più vecchie copiava vy in vx prima di fare
      // noi inizieremo con questa versione, poi vedremo
      uint8_t x = MASK_X(op);
      // chip.v[x] = chip.v[MASK_Y(op)];
      uint16_t flag = ((chip.v[x] & 0x8) != 0);
      chip.v[x] <<= 1;
      chip.v[0xF] = flag;
      break;
    }
    default:
      printf("Useless op:%x\n", op);
      break;
    }
    break;
  }

  case 0x9000: {
    // Skip if vx != vy
    chip.pc += (chip.v[MASK_X(op)] != chip.v[MASK_Y(op)]) * 2;
    break;
  }
  case 0xA000: {
    debug_printf("Setting i reg to %d\n", MASK_NNN(op));
    chip.i = MASK_NNN(op);
    break;
  }
  case 0xB000: {
    // ambigua, nell'originale saltava a nnn+v0, dopo invece funzionava come
    // bxnn ovvero xnn+vx,strano implementiamo la versione nuova for now
    chip.pc = chip.v[MASK_X(op)] + MASK_NNN(op);
    break;
  }
  case 0xC000: {
    int randint = rand() % 0xFF;
    uint16_t rint = (uint16_t)randint;
    chip.v[MASK_X(op)] = rint & MASK_NN(op);
    break;
  }
  case 0xD000: {
    // Logicamente ha più senso non creare ogni volta lo sprite :)
    debug_printf("draw op %x x:%u, y:%u\n", op, MASK_X(op), MASK_Y(op));
    // for (unsigned int i = 0; i< 16 ; i++)printf("v[%u]=%u\n",i,chip.v[i]);
    GPU_sprite sprite = {.x = (unsigned int)chip.v[MASK_X(op)],
                         .y = (unsigned int)chip.v[MASK_Y(op)],
                         .height = (unsigned int)MASK_N(op),
                         .ptr = &chip.mem[chip.i]};
    bool hit = gpu_draw_sprite(&chip.gpu, &sprite);
    chip.v[0xF] = (uint16_t)hit;
    break;
  }
  case 0xE000: {
    if (MASK_NN(op) == 0x9E) {
      chip.pc += 2 * (keyboard_state[chip.v[MASK_X(op)]] == CHIP_KEY_DOWN);
    } else if (MASK_NN(op) == 0xA1) {
      chip.pc += 2 * (keyboard_state[chip.v[MASK_X(op)]] != CHIP_KEY_DOWN);
    }

    break;
  }
  case 0xF000: {
    // TODO:Implementare quelle relative al timer :)
    switch (MASK_NN(op)) {
    case 0x07: {
      chip.v[MASK_X(op)] = chip.delay_timer;
      break;
    }
    case 0x15:
      chip.delay_timer = chip.v[MASK_X(op)];
      break;
    case 0x18:
      chip.sound_timer = chip.v[MASK_X(op)];
    case 0x1E:
      // L'originale non teneva conto dell'overflow, ma noi si
      if (chip.i > 0xFF - chip.v[MASK_X(op)])
        chip.v[0xF] = 1;
      chip.i += chip.v[MASK_X(op)];
      break;
    case 0x0A:
      if (CHIP8_key_state == CHIP_KEY_RELEASED) {
        chip.v[MASK_X(op)] = CHIP8_key;
      } else {
        chip.pc -= 2;
      }
      break;
    case 0x29: // Font uint8_tacter
      chip.i = (5 * (uint16_t)chip.v[MASK_X(op)]) + 0x0050;
      break;
    case 0x33: {
      uint8_t number = chip.v[MASK_X(op)];
      chip.mem[chip.i + 2] = number % 10;
      number /= 10;
      chip.mem[chip.i + 1] = number % 10;
      number /= 10;
      chip.mem[chip.i] = number % 10;
      debug_printf("Number %u is %u%u%u\n", chip.v[MASK_X(op)],
                   chip.mem[chip.i], chip.mem[chip.i + 1],
                   chip.mem[chip.i + 2]);

      break;
    }
      // per entrambi si usa il comportamento "moderno" in cui non incremento i
    case 0x55: {
      // si ringrazia la libreria standard
      memcpy(&chip.mem[chip.i], chip.v, sizeof(uint8_t) * (MASK_X(op) + 1));
      break;
    }
    case 0x65: {
      memcpy(chip.v, &chip.mem[chip.i], sizeof(uint8_t) * (MASK_X(op) + 1));
      break;
    }
    }
    break;
  }

  default:
    debug_printf("UNRECOGNIZED OPCODE:%u\n", op);
    break;
  }
  if (CHIP8_DEBUG)
    chip8_dump_state();
}

void chip8_loop() {
  SDL_Event e;
  bool quit = false;
  uint32_t tick60hz = SDL_GetTicks();
  while (quit == false) {
    uint64_t start_ticks = SDL_GetPerformanceCounter();
    bool stepped = true;
    // Non so quanto possa convenirmi fare una roba del genere
    if (CHIP8_key_state == CHIP_KEY_RELEASED)
      CHIP8_key_state = CHIP_KEY_UP;
    for (size_t i = 0; i < 16; i++) {
      if (keyboard_state[i] == CHIP_KEY_RELEASED)
        keyboard_state[i] = CHIP_KEY_UP;
    }
    while (SDL_PollEvent(&e)) {
      // TODO: l'idea è di inserire qui lo switch per gli input
      // nella speranza che sdl mi permetta anche di creare un evento di timeout
      if (e.type == SDL_QUIT)
        quit = true;
      switch (e.type) {
      case SDL_QUIT:
        quit = true;
        break;
      case SDL_KEYUP: {
        if (e.key.keysym.scancode == SDL_SCANCODE_P) {
          stepped = true;
        }
        // printf("Released key: %s\n", SDL_GetKeyName(e.key.keysym.sym));
        // devo estrarre sta roba perché sta diventando mezza illeggibile
        uint16_t idx = chip8_get_key_index(e.key.keysym.scancode);
        if (idx < 16) {

          keyboard_state[idx] = CHIP_KEY_RELEASED;
          CHIP8_key = (uint8_t)idx;
          CHIP8_key_state = CHIP_KEY_RELEASED;
        }
        break;
      }
      case SDL_KEYDOWN: {
        // printf("Pressed key: %s\n", SDL_GetKeyName(e.key.keysym.sym));
        uint16_t idx = chip8_get_key_index(e.key.keysym.scancode);
        if (idx < 16) {

          keyboard_state[idx] = CHIP_KEY_DOWN;
          CHIP8_key = (uint8_t)idx;
          CHIP8_key_state = CHIP_KEY_DOWN;
        }
        break;
      }
      }
    }

    if (stepped) {
      if (SDL_GetTicks() - tick60hz >= 16) {
        if (chip.delay_timer > 0)
          chip.delay_timer -= 1;
        if (chip.sound_timer > 0)
          chip.sound_timer -= 1;
      }
      uint16_t op = chip8_fetch();

      chip8_decode(op);
      // gui_draw_memory(chip.mem);
      uint64_t end_ticks = SDL_GetPerformanceCounter();
      float elapsedMS = 1000.0f * (end_ticks - start_ticks) /
                        (float)SDL_GetPerformanceFrequency();
      // printf("Elapsed S: %f\n", (end_ticks -
      // start_ticks)/(float)SDL_GetPerformanceCounter()); printf("Elapsed ms:
      // %f\n",elapsedMS); printf("Current FPS: %f\n" ,1000.0f/elapsedMS);
      int delay = (int)(16.666f - elapsedMS);
      // printf("Delay: %d\n", delay);

      // SDL_Delay(floor(16.666f - elapsedMS));
      // SDL_Delay((delay>0)?delay:0);
      SDL_Delay(1);
    }
  }
}

void chip8_close() {
  gpu_close(&(chip.gpu));
  // gui_close();
}

int chip8_load(uint8_t *buffer, long filelen) {
  if (filelen > CHIP_MEM_SIZE - 0x200) {
    fprintf(stderr, "FILE TOO BIG!");
    return 0;
  }
  memcpy(&chip.mem[0x0200], buffer, sizeof(uint8_t) * filelen);
  return 1;
}
