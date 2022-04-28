
/*
Read from controllers with pad_poll().
We also demonstrate sprite animation by cycling among
several different metasprites, which are defined using macros.
*/

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

///// METASPRITES

// define a 2x2 metasprite
#define DEF_METASPRITE_2x2(name,code,pal)\
const unsigned char name[]={\
        0,      0,      (code)+0,   pal, \
        0,      8,      (code)+1,   pal, \
        8,      0,      (code)+2,   pal, \
        8,      8,      (code)+3,   pal, \
        128};

// define a 2x2 metasprite, flipped horizontally
#define DEF_METASPRITE_2x2_FLIP(name,code,pal)\
const unsigned char name[]={\
        8,      0,      (code)+0,   (pal)|OAM_FLIP_H, \
        8,      8,      (code)+1,   (pal)|OAM_FLIP_H, \
        0,      0,      (code)+2,   (pal)|OAM_FLIP_H, \
        0,      8,      (code)+3,   (pal)|OAM_FLIP_H, \
        128};

DEF_METASPRITE_2x2(playerRStand, 0xd8, 0);
DEF_METASPRITE_2x2(playerRRun1, 0xdc, 0);
DEF_METASPRITE_2x2(playerRRun2, 0xe0, 0);
DEF_METASPRITE_2x2(playerRRun3, 0xe4, 0);

const unsigned char* const playerDirection[4] = {
  playerRStand, playerRRun1, playerRRun2, playerRRun3
};

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x00,			// screen color

  0x11,0x30,0x27,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x00,0x10,0x20,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,	// background palette 3

  0x2C,0x35,0x0f,0x00,	// sprite palette 0
  0x2F,0x30,0x1D,0x00,	// sprite palette 1
  0x0D,0x2D,0x3A,0x00,	// sprite palette 2
  0x0D,0x27,0x2A	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  oam_hide_rest(0);
  // set palette colors
  pal_all(PALETTE);
  // turn on PPU
  ppu_on_all();
}

// number of actors (4 h/w sprites each)
#define NUM_ACTORS 2

// actor x/y positions
byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
// actor x/y deltas per frame (signed)
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];

void movePlayer(char* oam_id_pointer) {
  char pad;
  byte direction = 0;
  char i = 0;
  
  // poll controller i (0-1)
  pad = pad_poll(i);
  
  // move player up/down
  if (pad&PAD_UP && actor_y[i]>0) {
    actor_dy[i]=-2; 
    direction = 1;
  }
  else if (pad&PAD_DOWN && actor_y[i]<212) actor_dy[i]=2;
  else actor_dy[i]=0;
  
  // move player left/right
  if (pad&PAD_LEFT && actor_x[i]>0) {
    actor_dx[i]=-2; 
    direction = 3;
  }
  else if (pad&PAD_RIGHT && actor_x[i]<240) {
    actor_dx[i]=2; 
    direction = 2;
  }
  else actor_dx[i]=0;
  
  // move and display player
  actor_x[i] += actor_dx[i];
  actor_y[i] += actor_dy[i];
  
  // increment actor index for other sprites
  *oam_id_pointer = oam_meta_spr(actor_x[i], actor_y[i], *oam_id_pointer, playerDirection[direction]);
}

void moveCharacters() {
  char oam_id = 0;	// sprite ID	// controller flags
  char* oam_id_pointer = &oam_id;
  
  movePlayer(oam_id_pointer);
  
  // hide rest of sprites
  // if we haven't wrapped oam_id around to 0
  if (oam_id!=0) oam_hide_rest(oam_id);
}

void initCharacters() {
  //player
  actor_x[0] = 128;
  actor_y[0] = 128;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
}

// main program
void main() {
  // print instructions
  //vram_adr(NTADR_A(2,2));
  //vram_write("\x1c\x1d\x1e\x1f to move metasprite", 24);

  setup_graphics();
  initCharacters();
  
  // loop forever
  while (1) {
    moveCharacters();
    ppu_wait_frame();
  }
}
