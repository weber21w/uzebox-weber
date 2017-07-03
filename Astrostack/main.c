/*
 *  Astro Stacker
 *  Lee Weber 2017
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/


/*A note about the networking, it is likely difficult to follow without understanding how all the parts of the game work together. The way it is, is what I consider the highest performance
possible;otherwise I would have done it differently. So basically the goal here, is to get our data to them as soon as it is calculated, but also not mess up if we miss vsync. We are trying
to keep 2 separate simulations synchronized, so it becomes critical that in no place in the code does our player ever directly set the state of the other player. In theory this is all that
is needed to consider, however it is very hard to predict what network latency and loss will do, perhaps starving for data 1 moment and receiving an immense amount later. It seems there
could very well be some situations where UART data would be lost, that wont come up during testing. So I go the extra complicated step of *trying* to have the game automatically synchronize
if that ever happens. Past that, it is just dealing with parsing serial data in plain text to get to the data that becomes any sort of complication. Ask any questions on the forums, I mean 
for this to be a (scary by necessity) example of how do Uzenet stuff. Some games are easier than others...*/







//////////////////////////////OTHER GAMES TO POSSIBLY TRY!!!!!!!!!!!!
/////////////////////Animal Yokochou: Doki Doki Shinkyuu 
/////////////////////Monster Slider
////////////////////Star Sweep...weird game
////////////////////Gururin...interesting
////////////////////Chain Reaction, Magical Drop 3
////////////////////Block Out ..3d tetris!!
////////////////////Pnickies...cool, Puzzli 2..good graphic, Puzzled..interesting spin on Tetris, large development investment?

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>

//external data
#include "data/tiles.inc"
#include "data/sprites.inc"
#include "data/maps.inc"
#include "data/patches.inc"
#include "data/music.inc"

extern void TriggerCommon(Track* track,u8 patch,u8 volume,u8 note);
extern unsigned char ram_tiles[];

void DrawBigChar(uint8_t x, uint8_t y, uint8_t c);
void DrawBigString(uint8_t x, uint8_t y, const char *str);
void DrawWell(uint8_t t);
void ResetSprites();
void SetSprite(int16_t x, int16_t y, uint8_t t, uint8_t f);

void ShowCredits();
void Intro();
void TitleScreen();
void VsyncHook();
void FillScreenPattern();
void DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t t);
void DrawBlock(int8_t x, int8_t y, uint8_t b);
void FillVram(uint8_t t);
void DrawPreview(uint8_t i);
void DrawScored(uint8_t i);
void UndrawScored(uint8_t i);

#define AS_MASTER_VOLUME 180

#define MAX_PLAYERS	2

#define WELL_WIDTH	6
#define WELL_HEIGHT	(12+1)
#define WELL_SIZE	(WELL_WIDTH*WELL_HEIGHT)

#define BLOCK_EMPTY	0
#define BLOCK_RED	1
#define BLOCK_GREEN	2
#define BLOCK_BLUE	3
#define BLOCK_GARBAGE	4
#define BLOCK_BOMB	5
#define BLOCK_STAR	6//also 7 and 8 are animation frames
#define BLOCK_SKULL	9

#define HALFSTEP	32
#define SCORE		64

#define STATE_DISABLED			0//don't run logic for this player
#define STATE_AWAIT_READY		1//waiting for user to be ready
#define STATE_COUNT_DOWN		2//counting down before play
#define STATE_AWAIT_LOCK		4//waiting for pieces to lock, no player control
#define STATE_AWAIT_SCORE		5//scoring is happening, no player control until it is done
#define STATE_AWAIT_DUST		6//waiting for stars to finish dropping from the top
#define STATE_WON_GAME			9//game over, this player won. wait for the other player to finish losing, then wait for input to start a new game
#define STATE_LOST_GAME			10//game over, this player lost. do the skull well animation, sound effect, music, etc. then wait for input to start a new game
#define STATE_SPAWN_BLOCKS		11//put the preview blocks into the controllable blocks, then generate new preview blocks based on the player's LFSR(both players will always get the same block order)
#define STATE_MOVING			12//move the blocks around until they drop, then go to STATE_SET_BLOCKS
#define STATE_SCORING			13//find and mark all scores, go to STATE_SCORE_FLASHING if there were scores, otherwise go to STATE_CHAIN_END(if chain) or go to STATE_GARBAGE(if no chain)
#define STATE_SCORE_FLASHING	14//flash all scored blocks in the well for a time then clear them, if there was a bomb(s) go to STATE_BOMB, otherwise go to STATE_GRAVITY
#define STATE_GRAVITY			15//drop any block that can, once per tick. When nothing dropped, goto STATE_SCORING(which if a score happened, will keep coming back here until it doesn't)
#define STATE_CHAIN_END			16//drops stars into the well depending on the chain count. Then go to STATE_GRAVITY(which could lead to another score)
#define STATE_ADDED_CHAIN_END	17//the last star drop failed, end the chain count to break out of the loop. Go to STATE_GRAVITY
#define STATE_BOMB				18//explode all bombs in the well, once the state is over go to STATE_GRAVITY
#define STATE_SET_BLOCKS		19//set the player's controlled blocks into the well, then go to STATE_GRAVITY
#define STATE_GARBAGE			20//keep track of garbage we have sent, and add any garbage received from the opponent. Then go to STATE_SPAWN_BLOCKS.

#define SUBSTATE_PAUSE_MENU_BACK		1//cursor is on "BACK"
#define SUBSTATE_PAUSE_MENU_QUIT		2//cursor is on "QUIT"

#define INPUT_NULL			0
#define INPUT_DOWN			1
#define INPUT_LEFT			2
#define INPUT_RIGHT			4
#define INPUT_ROTATE_RIGHT	8//clockwise
#define INPUT_ROTATE_LEFT	16//counterclockwise
#define INPUT_START			32


#define SCORE_STATE_LENGTH	45
#define BOMB_STATE_LENGTH	45

#define MAX_PARTICLES	6

#define PARTICLE_STAR	0
#define PARTICLE_NORMAL_TIME	45

void db(){
	TriggerFx(0,255,1);
}
//network states

#define NET_GET_PAD
#define NET_SEND_LFSR
#define NET_GET_LFSR
 
typedef struct{
	uint16_t lfsr, rolling_lfsr;
	uint8_t state;
	uint8_t sub_state;
	uint8_t state_count;
	uint8_t x,y,r;//position of current piece
	uint8_t blocks[2];
	uint8_t next[2];
	uint8_t chain;
	uint8_t iteration_stars;
	uint16_t total_stars;
	uint16_t total_garbage_processed;//garbage we have handled so far
	uint16_t total_garbage_rx;//total garbage received from opponent
	uint16_t total_garbage_tx;//total garbaged sent to opponent
	uint8_t piece_count;//used to add garbage every couple drops
	uint8_t added_chain;//whether or not this chain has been extended by falling stars
////	uint8_t input;//either game pad, demo data, or network data to be applied this tick
	uint8_t well[WELL_WIDTH*WELL_HEIGHT];
	uint8_t input_source;
	uint16_t checksum;//checksum of state, for network consistency verification
	uint8_t padState;
	uint8_t oldPadState;
	uint8_t latest_input_tick;//this is designed to roll over
	uint8_t network_state;
	uint8_t net_last_sync_spot;
	uint16_t tick;
}player_t;

player_t p[MAX_PLAYERS];

#define MAX_AIS		1

typedef struct{
	uint8_t state;
	uint8_t iteration;
	uint8_t score[WELL_WIDTH*4];
	uint8_t pawn;//this is which player the AI is controlling
	uint8_t well[WELL_SIZE];
	uint8_t target_rotation;
	uint8_t target_x;
	uint8_t wait;
	uint8_t padState;
}ai_t;

ai_t ai[MAX_AIS];


#define AI_WAIT				1
#define AI_CHOOSE_MOVE		2
#define AI_IMPLEMENT_MOVE	4
#define AI_CALCULATE_MOVE	8
#define AI_WAIT_NEXT_MOVE	16
#define AI_WAIT_ROUND_START	32

uint8_t global_frame = 0;
typedef struct{
	uint8_t x,y;
	uint8_t time,type;
	
}particle_t;

particle_t particles[MAX_PARTICLES];

//void AddParticle{uint8_t x, uint8_t y, uint8_t style, uint8_t life){
void AddParticle(uint8_t x, uint8_t y, uint8_t style, uint8_t life){
	uint8_t low = 0;
	uint8_t mag = 255;
	for(uint8_t i=1;i<MAX_PARTICLES;i++){
		if(particles[i].time < mag){//found a particle that has been around longer/closer to death
			mag = particles[i].time;
			low = i;
		}
	}
	particles[low].x = x;
	particles[low].y = y;
	particles[low].time = life;
	particles[low].type = style;
}

void ProcessParticles(){
	uint8_t scount = 0;
	for(uint8_t i=0;i<MAX_SPRITES;i++)//hide all sprites
		sprites[i].x = TILE_WIDTH*SCREEN_TILES_H;
	
	for(uint8_t i=0;i<MAX_PARTICLES;i++){
		if(!particles[i].type)
			continue;
		if(!particles[i].time){
			particles[i].type = 0;//turn off
			continue;
		}else{
			particles[i].time--;
			if((global_frame & 3) == 3)
				particles[i].y--;
		}
		sprites[scount].x = particles[i].x;
		sprites[scount].y = particles[i].y;
		//if(particles[i].type < 16)
			sprites[scount].tileIndex = particles[i].type;
		sprites[scount++].flags = 0;
		
	}
}

uint8_t sprite_count;




void VsyncHook();

void AS_TriggerFx(uint8_t patch);


void ResetSprites();
void SetSprite(int16_t x, int16_t y, uint8_t t, uint8_t f);

void ShowCredits();
void Intro();
void TitleScreen();
void LevelIntro();
uint8_t PauseMenu();
void ProcessAI();




void FillVram(uint8_t t){
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = t;
}

void AS_TriggerFx(uint8_t patch){//use the 5th channel exclusively to allow music(uses 1-4) to be uninterrupted
TriggerFx(patch,255,1);return;
	Track* track=&tracks[4];
	tracks[4].flags|=TRACK_FLAGS_PRIORITY;
	track->patchCommandStreamPos = NULL;
	TriggerCommon(track,patch,255,80);
	track->flags|=TRACK_FLAGS_PLAYING;

}



void ResetSprites(){

	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = SCREEN_TILES_H*8;
	sprite_count = 0;

}






void SetSprite(int16_t x, int16_t y, uint8_t t, uint8_t f){

	if(x < 0 || y < 0 || x >= (SCREEN_TILES_H*TILE_WIDTH) || y >= (SCREEN_TILES_V*TILE_WIDTH))
		return;

	if(sprite_count == MAX_SPRITES)
		return;//dbf();
	sprites[sprite_count].x = x;
	sprites[sprite_count].y = y;
	sprites[sprite_count].tileIndex = t;
	sprites[sprite_count++].flags = f;
}





const char uzebox_string[] PROGMEM = "UZEBOX";

void Intro(){
#ifdef FASTDEBUG
return;
#endif

	for(uint8_t i=0;i<6;i++){//for each letter
		for(uint8_t j=0;j<12;j++){//slide down towards center
			for(uint8_t k=0;k<12;k++){//blank out trail
				SetTile(9+(i*2),k,BLACK_TILE);
				SetTile(10+(i*2),k,BLACK_TILE);
			}
			DrawBigChar(9+(i*2),j,pgm_read_byte(&uzebox_string[i]));
			WaitVsync(1);
		}
	}

	FadeIn(9,true);
	WaitVsync(60);
	FadeOut(1,true);
	FillVram(BLACK_TILE+RAM_TILES_COUNT);
	DrawBigString(2,4 ,PSTR("SOURCE CODE"));
	DrawBigString(2,6 ,PSTR("   GPL 3"));
	DrawBigString(2,10,PSTR("SFX GFX MUSIC"));
	DrawBigString(2,12,PSTR("  CC SA 4"));
	DrawBigString(2,20,PSTR(" LEE WEBER"));
	DrawBigString(2,22,PSTR("   2017"));
	FadeIn(2,true);
	WaitVsync(180);
	FadeOut(2,true);
	FadeIn(3,false);
	
}

uint8_t UzenetConnect(){
	WaitVsync(60);
	return 0;
}

uint8_t UzenetMenu(){
	FadeIn(1,false);
	FillVram(BLACK_TILE+RAM_TILES_COUNT);
	DrawBigString(9,2,PSTR("UZENET"));
	DrawBigString(5,6,PSTR("CONNECTING"));
	uint8_t r = UzenetConnect();
	r = 1;
	if(r){
		for(uint8_t i=0;i<SCREEN_TILES_H;i++){
			SetTile(i,6,BLACK_TILE);
			SetTile(i,7,BLACK_TILE);
		}
		DrawBigString(8,6,PSTR("ERROR"));
		DrawBigChar(20,6,'0'+r);
		DrawBigString(2,13,	PSTR(" RUN ESP8266"));
		DrawBigString(2,15,	PSTR("SETUP UTILITY"));
		while(1){
			WaitVsync(1);
			if(p[0].padState & BTN_START && !(p[0].oldPadState & BTN_START))
				return r;
		}
	}else{
		DrawBigString(5,6,PSTR("CONNECTED["));
		uint8_t cursorpos = 1;
		uint8_t playerpos = 0;
		uint8_t totalplayers = 5;
		
		while(1){
			WaitVsync(1);
		
			if((p[0].padState & BTN_UP) && !(p[0].oldPadState & BTN_UP))
				cursorpos--;
			else if((p[0].padState & BTN_DOWN) && !(p[0].oldPadState & BTN_DOWN))
				cursorpos++;
			if(cursorpos == 4)
				cursorpos = 1;
			else if(cursorpos < 1)
				cursorpos = 3;
		
			for(uint8_t i=14;i<SCREEN_TILES_V;i++){
				SetTile(1,i,BLACK_TILE);
				SetTile(2,i,BLACK_TILE);
			}
			DrawBlock(1,12+(cursorpos*2),BLOCK_RED);

			
			DrawBigString(1,10,PSTR("CHOOSE PLAYER"));
			DrawBigString(3,14,PSTR("D3THADD3R"));
			DrawBigString(3,16,PSTR("EASY CPU"));
			DrawBigString(3,18,PSTR("GOOD CPU"));
			DrawBigString(3,20,PSTR("ARTCFOX"));
			if(totalplayers > playerpos+4)
				DrawBigString(3,22,PSTR("\\MORE^"));
		}
	}
	while(1);
	return 0;
}

const uint8_t chain_star_count[] PROGMEM = {
0,2,4,5,6,12,12,12,12,12,12,12,12,12,12,12,0,0,
};

const uint8_t title_block_coords[] PROGMEM = {
2,1,//'A'
0,3,
4,3,
0,5,
2,5,
4,5,
0,7,
4,7,

9,1,//'S'
7,2,
8,4,
10,6,
8,7,

12,1,//'T'	
14,1,
13,3,
13,5,
13,7,

17,1,//'R'
19,1,
17,3,
20,3,
17,5,
19,5,
17,7,
20,7,

24,1,//'O'
26,1,
23,3,
27,3,
23,5,
27,5,
24,7,
26,7,


2,11,//'S'
0,12,
1,14,
3,16,
1,17,

5,11,//'T'	
7,11,
6,13,
6,15,
6,17,

12,11,//'A'
10,13,
14,13,
10,15,
12,15,
14,15,
10,17,
14,17,

18,11,//'C'
20,11,
17,13,
17,15,
18,17,
20,17,

23,11,//'K'
23,13,
26,13,
23,15,
25,15,
23,17,
26,17,

};

const uint8_t title_star_effect_pos[] PROGMEM = {
6,4,//A
14,7,//R
20,7,//O

14,13,//A

	
};

void TitleScreen(){
TITLE_SCREEN_TOP:

	ResetSprites();
	for(uint8_t y=0;y<10;y++){//TODO JUST ADD OFFSETS TO CONST DATA
		for(uint8_t x=0;x<22;x++){
			uint8_t t = pgm_read_byte(&title_map[(y*22)+x+2]);
			t += sizeof(rom_tiles)/64;
			SetTile(x+4,y+3,t);
		}
	}
	
	for(uint8_t i=0;i<sizeof(&title_star_effect_pos)/2;i+=2){//set up for the ram tile effect
		uint8_t x = pgm_read_byte(&title_star_effect_pos[i+0]);
		uint8_t y = pgm_read_byte(&title_star_effect_pos[i+1]);
		uint8_t off = (y*VRAM_TILES_H)+x;
		vram[off++] = 0;
		vram[off++] = 1;
		off += VRAM_TILES_H-2;
		vram[off++] = 2;
		vram[off++] = 3;
	}
/*
	FillVram(BLACK_TILE+RAM_TILES_COUNT);
	for(uint8_t i=0;i<sizeof(title_block_coords);i+=2)
		DrawBlock(pgm_read_byte(&title_block_coords[i]),pgm_read_byte(&title_block_coords[i+1]),BLOCK_STAR);
*/
	uint8_t cursorpos = 1;
	uint8_t stars_x[3] = {2,5,7};
	uint8_t stars_y[3] = {2,3,6};
	
StartSong(Song1);

	while(1){
		for(uint8_t i=0;i<64*3;i++)
			ram_tiles[i] = 0;
		for(uint8_t i=0;i<3;i++){
			if(stars_x[i])
				stars_x[i]--;
			else
				stars_x[i] = 7;
			if(stars_y[i])
				stars_y[i]--;
			else
				stars_y[i] = 7;
			ram_tiles[(stars_y[i]*8)+stars_y[i]] = 255;
		}
		DrawBigString(9,20,PSTR("SINGLE"));
		DrawBigString(9,22,PSTR("VERSUS"));
		DrawBigString(9,24,PSTR("UZENET"));
		for(uint8_t i=20;i<SCREEN_TILES_V;i++){
			SetTile(7,i,BLACK_TILE);
			SetTile(8,i,BLACK_TILE);
		}

		if((p[0].padState & BTN_UP) && !(p[0].oldPadState & BTN_UP))
			cursorpos--;
		else if((p[0].padState & BTN_DOWN) && !(p[0].oldPadState & BTN_DOWN))
			cursorpos++;
		if(cursorpos == 4)
			cursorpos = 1;
		else if(cursorpos < 1)
			cursorpos = 3;

		DrawBlock(7,18+(cursorpos*2),BLOCK_BLUE);
//p[0].padState = BTN_START;
		if((p[0].padState & BTN_START) && !(p[0].oldPadState & BTN_START)){
			if(cursorpos == 2 && (DetectControllers() & 0b00001100) != 0b00000100){//cannot play 2 player game without a second controller
				AS_TriggerFx(1);
			}else{
				FadeOut(2,true);
				if(cursorpos == 1){//single
					return;
				}else if(cursorpos == 2){//vs
					return;
				}else{//uzenet
					if(UzenetMenu())
						return;
					goto TITLE_SCREEN_TOP;
				}
			}
		}
		WaitVsync(1);
	}
}




void VsyncHook(){

//	master_input_clock++;//this is designed to roll over
//	p[0].oldPadState = p[0].padState;
//	p[0].padState = ReadJoypad(0);
//	p[0].latest_input_tick = master_input_clock;

	uint16_t buttons = 0;
	for(uint8_t i=0;i<MAX_PLAYERS;i++){

		if(!p[i].network_state){
			buttons = ReadJoypad(i);
		}else{//check the data stream for input from the remote side
			buttons = ai[0].padState;//HACK FOR AI!!!!!!!!
		}
		p[i].oldPadState = p[i].padState;
		p[i].padState = (buttons & 0xFF);
		
		if((buttons & BTN_SL) || (buttons & BTN_Y))
			p[i].padState |= BTN_Y;//this fits within 8 bits, BTN_SL does not
		if((buttons & BTN_SR) || (buttons & BTN_B))
			p[i].padState |= BTN_B;//this fits within 8 bits, BTN_SR does not
	}
	
}




uint8_t PauseMenu(){

	return 0;

}

#define FIRST_BORDER_TILE 36

void FillScreenPattern(){//return;
	uint8_t x = 0;
	uint8_t y = 0;
	uint8_t t = 0;
	uint8_t drew = 0;

	do{
		drew = 0;
		t = (FIRST_BORDER_TILE+8)-1;
		for(uint8_t i=y;i<y+4;i++){
			for(uint8_t j=x;j<x+2;j++){
				t++;
				if(j >= SCREEN_TILES_H || i >= SCREEN_TILES_V)
					continue;
				drew = 1;
				SetTile(j,i,t);
			}
		}
		x += 2;
		if(x >= SCREEN_TILES_H){
			x = 0;
			y += 4;
		}
	}while(drew);
}

void DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t t){
	for(uint8_t i=y;i<y+h;i++)
		for(uint8_t j=x;j<x+w;j++)
			SetTile(j,i,t);
}
void DrawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
	w--;h--;
	DrawRectangle(x,y,w,h,FIRST_BORDER_TILE);

	for(uint8_t i=1;i<w;i++){//draw horizontal lines
		if(x+i >= SCREEN_TILES_H)
			break;

		if(y+0 < SCREEN_TILES_V )
			SetTile(x+i,y+0,FIRST_BORDER_TILE+7);
		if(y+h < SCREEN_TILES_V )
			SetTile(x+i,y+h,FIRST_BORDER_TILE+7);

	}

	for(uint8_t i=1;i<h;i++){//draw vertical lines
		if(y+i >= SCREEN_TILES_V)
			break;

		if(x+0 < SCREEN_TILES_H )
			SetTile(x+0,y+i,FIRST_BORDER_TILE+5);
		if(x+w < SCREEN_TILES_H )
			SetTile(x+w,y+i,FIRST_BORDER_TILE+6);

	}

	
	if(x+0 < SCREEN_TILES_H && y+0 < SCREEN_TILES_V)//top left corner
		SetTile(x+0,y+0,FIRST_BORDER_TILE+1);
	if(x+w < SCREEN_TILES_H && y+0 < SCREEN_TILES_V)//top right corner
		SetTile(x+w,y+0,FIRST_BORDER_TILE+2);
	if(x+0 < SCREEN_TILES_H && y+h < SCREEN_TILES_V)//bottom left corner
		SetTile(x+0,y+h,FIRST_BORDER_TILE+3);
	if(x+w < SCREEN_TILES_H && y+h < SCREEN_TILES_V)//bottom right corner
		SetTile(x+w,y+h,FIRST_BORDER_TILE+4);


}

void DrawClearBlockGrid(uint8_t x, uint8_t y, uint8_t i){
	if(y == 0)
		return;
	//TODO NO NEED FOR BOUNDS CHECK???
		uint8_t b = p[i].well[(y*WELL_WIDTH)+x];
		x <<= 1;//convert to vram coords
		x += (i?15:2);//add border offset
		y<<= 1;//convert to vram coords
		if(b & HALFSTEP)
			y++;
		uint16_t voff = (y*VRAM_TILES_H)+x;
		vram[voff++]	= BLACK_TILE+RAM_TILES_COUNT;
		vram[voff]		= BLACK_TILE+RAM_TILES_COUNT;
		voff += VRAM_TILES_H-1;
		vram[voff++]	= BLACK_TILE+RAM_TILES_COUNT;
		vram[voff]		= BLACK_TILE+RAM_TILES_COUNT;
	
}

void DrawBlockGrid(uint8_t x, uint8_t y, uint8_t i){
	if(y == 0)
		return;
	//TODO NO NEED FOR BOUNDS CHECK??
	uint8_t b = p[i].well[(y*WELL_WIDTH)+x];
	x <<= 1;
	x += (i?15:2);
	y <<= 1;
	if(b & HALFSTEP)
		y++;
	
	b &= 0x0F;//strip flags
	
	uint16_t voff = (y*VRAM_TILES_H)+x;

	if(b){
		b--;
		uint8_t toff = (b<<2)+RAM_TILES_COUNT;
		vram[voff++]	= toff++;
		vram[voff]		= toff++;
		voff += VRAM_TILES_H-1;
		vram[voff++]	= toff++;
		vram[voff]		= toff;
	}else{
		vram[voff++]	= BLACK_TILE+RAM_TILES_COUNT;
		vram[voff]		= BLACK_TILE+RAM_TILES_COUNT;
		voff += VRAM_TILES_H-1;
		vram[voff++]	= BLACK_TILE+RAM_TILES_COUNT;
		vram[voff]		= BLACK_TILE+RAM_TILES_COUNT;
	}
}


void DrawBlock(int8_t x, int8_t y, uint8_t b){

	if(b & HALFSTEP)
		y++;

	if(x >= SCREEN_TILES_H || y >= SCREEN_TILES_V || y < 1)
		return;
	uint16_t voff = (y*VRAM_TILES_H)+x;

//	if(voff < VRAM_TILES_H)//the top and bottom are both inside the top two rows we don't want to draw over
	//	return;
	
	b &= 0x0F;//strip flags
	
	if(b){
		b--;
		uint8_t toff = (b<<2)+RAM_TILES_COUNT;

		if(voff >= VRAM_TILES_H*2){//don't draw piece preview over the border
			vram[voff++] = toff++;
			vram[voff] = toff++;
			voff += VRAM_TILES_H-1;
		}else{
			toff += 2;
			voff += VRAM_TILES_H;
		}
		vram[voff++] = toff++;
		vram[voff] = toff;
	}else{
		if(voff >= VRAM_TILES_H*2){
			vram[voff++] = BLACK_TILE+RAM_TILES_COUNT;
			vram[voff] = BLACK_TILE+RAM_TILES_COUNT;
			voff += VRAM_TILES_H-1;
		}else
			voff += VRAM_TILES_H;
		
		vram[voff++] = BLACK_TILE+RAM_TILES_COUNT;
		vram[voff] = BLACK_TILE+RAM_TILES_COUNT;
	}
}


/*
#define STATE_DISABLED	0
#define STATE_AWAIT_READY		1//waiting for user to be ready
#define STATE_COUNT_DOWN		2//counting down before play
#define STATE_AWAIT_LOCK		4//waiting for pieces to lock, no player control
#define STATE_AWAIT_SCORE		5//scoring is happening, no player control until it is done
#define STATE_AWAIT_DUST		6//waiting for stars to finish dropping from the top
#define STATE_AWAIT_LOSS		7//waiting for skulls to come up in center
#define STATE_AWAIT_WIN			8//waiting for opponent to finish losing
#define STATE_WON_GAME			9
#define STATE_LOST_GAME			10
#define STATE_MOVING			11
*/





uint8_t ProcessGravity(uint8_t i){
//return 0;
	uint8_t moved = 0;
	uint8_t off;

	for(uint8_t x=0;x<WELL_WIDTH;x++){
		off = (WELL_SIZE-WELL_WIDTH)+x;//start at the lowest left block
		for(uint8_t y=WELL_HEIGHT-1;y>0;y--){
			uint8_t t = p[i].well[off];//what piece is here?
			if(!t){//empty, something above could fall here
				t = p[i].well[(off-WELL_WIDTH)];//what is above?
				if(t){//something!
					moved++;

					DrawClearBlockGrid(x,y-1,i);//blank out the old position
					if(t & HALFSTEP){//was half way there already, put it in the open spot
						p[i].well[off] = (p[i].well[off-WELL_WIDTH]&(0x0F|SCORE));//drop the HALFSTEP, since it is taking a new spot. Keep the score in case it is a falling star(which needs to disappear after)
						p[i].well[off-WELL_WIDTH] = 0;
						DrawBlockGrid(x,y+0,i);//draw it in the new position
						
					}else{//this block still has to go through the half step phase before it enters this location
						p[i].well[off-WELL_WIDTH] |= HALFSTEP;
						DrawBlockGrid(x,y-1,i);//draw it in the new position
					}
				//	break;//only drop 1 block per column, per tick. Looks a little nicer, and makes worst case scenarios cheaper.
				}//else an empty space above an empty space, which doesn't mean there isn't something further up
			}
			off -= WELL_WIDTH;
		}
	}
	
	/*for(uint8_t x=0;x<WELL_WIDTH;x++){//get any extra stars that might be in the invisible row
		if(p[i].well[x] && !p[i].well[x+WELL_WIDTH]){
			p[i].well[x+WELL_WIDTH] = p[i].well[x];
			p[i].well[x] = 0;
			moved++;
		}
	}*/
	return moved;
}

const uint8_t spawn_pile[] PROGMEM = {//statistical analysis of 100 pieces(SNES version) indicates it really is 25% for each of red,green,blue,star. This table might hurt the randomness instead of help?
BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,BLOCK_STAR,BLOCK_GREEN,BLOCK_BLUE,BLOCK_RED,
BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_BLUE,BLOCK_GREEN,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_STAR,BLOCK_BLUE,BLOCK_STAR,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,
BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,
BLOCK_BLUE,BLOCK_GREEN,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_STAR,BLOCK_BLUE,BLOCK_STAR,
BLOCK_BLUE,BLOCK_GREEN,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_BLUE,BLOCK_STAR,BLOCK_STAR,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_STAR,BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR,
};



void SpawnNewPiece(uint8_t i){
	p[i].x = 2;
	p[i].y = 0;
	p[i].blocks[0] = p[i].next[0];
	p[i].blocks[1] = p[i].next[1];
	GetPrngNumber(p[i].lfsr);//set our existing value from last random usage
	p[i].next[0] = pgm_read_byte(&spawn_pile[GetPrngNumber(0)%sizeof(spawn_pile)]);
	p[i].lfsr = GetPrngNumber(0);//get a new value, this method makes the generated pieces the same for both player regardless of how long the game goes(no other use of random)
	p[i].next[1] = pgm_read_byte(&spawn_pile[p[i].lfsr%sizeof(spawn_pile)]);
	p[i].r = 0;
	DrawPreview(i);
}

uint8_t ProcessScoring(uint8_t i){

	uint8_t off = WELL_WIDTH;//first row is not visible
	uint8_t left,right,t;
	uint8_t leftpos=0,rightpos=0;
	uint8_t numscores = 0;
	uint8_t icon_x = 255;
	uint8_t icon_y = 255;

	//scan horizontally
	for(uint8_t y=1;y<WELL_HEIGHT;y++){
		left = right = 255;//clear the markers for a new row
		for(uint8_t x=0;x<WELL_WIDTH;x++){
			t = p[i].well[off++]&0x0F;//drop any flags
			if(t || right != 255){
				if(left == 255){//we must find a left score marker first
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is a score marker
						left = t;//store it
						leftpos = x;
					}
				}else if(right == 255){//once we have a left, we must find a right
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is  not a star or anything else, this can make or break the score
						if(t != left){//we found a different color marker, so everything to the left is not a score. start a new scan from here
							left = t;//store the new left marker for the new search
							leftpos = x;
						}else{
							right = t;
							rightpos = x;
							if(x == WELL_WIDTH-1)//need to short circuit, otherwise this score wont process. Would need redundant code without the goto
								goto SCORING_HORIZONTAL_SHORT_CIRCUIT;
						}
					}
				}else{//we have a left and a right so a score did happen, we just need to see how far it goes(more of the same color on the right or side of well?)
SCORING_HORIZONTAL_SHORT_CIRCUIT:
				//db();
					if(icon_x == 255){//first score we found? set the position for the particle chain counter
						icon_x = leftpos+1;
						icon_y = y;
					}
					if((x == WELL_WIDTH-1) || (t != right && t <= BLOCK_BLUE)){//the score has ended, found the edge of the well or a different color marker or empty spot
						
						if(t == right)//must have found the well edge, don't leave this one behind(not trivial to understand why this is needed..)
							rightpos = x;

						leftpos += (y*WELL_WIDTH);
						rightpos += (y*WELL_WIDTH);
						
						while(leftpos <= rightpos){
							p[i].well[leftpos++] |= SCORE;//mark everything from the left to rightmost marker(there could be more than 1 right marker, but we store the last found)
							numscores++;
						//db();
						}
						left = right = 255;//clear the markers so we can score more than once per line
					}else if(t == right){//we found another right marker. the score is NOT over, there might be another matching marker to the right of this
						rightpos = x;
					}
					//else we found a star or something, which neither means the score is over, nor means it isn't
				}
			}else{//empty
				left = right = 255;
			}
		}
	}


	uint8_t up,down;
	uint8_t uppos=0,downpos=0;
	off = WELL_WIDTH;
	//scan vertically
	for(uint8_t x=0;x<WELL_WIDTH;x++){
		up = down = 255;//clear the markers for a new column
		off = WELL_WIDTH+x;
		for(uint8_t y=1;y<WELL_HEIGHT;y++){
			t = p[i].well[off]&0x0F;//drop any flags
			off += WELL_WIDTH;
			if(t || down != 255){
				if(up == 255){//we must find a top score marker first
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is a score marker
						up = t;//store it
						uppos = y;
					}
				}else if(down == 255){//once we have a top, we must find a bottom
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is  not a star or anything else, this can make or break the score
						if(t != up){//we found a different color marker, so everything to the top is not a score. start a new scan from here
							up = t;//store the new top marker for the new search
							uppos = y;
						}else{
							down = t;
							downpos = y;
							if(y == WELL_HEIGHT-1)//need to short circuit, otherwise this score wont process. Would need redundant code without the goto
								goto SCORING_VERTICAL_SHORT_CIRCUIT;
						}
					}
				}else{//we have an up and a down so a score did happen, we just need to see how far it goes(more of the same color on the bottom or edge of well?)
SCORING_VERTICAL_SHORT_CIRCUIT:
				//db();
					if(icon_y == 255){//first score we found? set the position for the particle chain counter
						icon_y = uppos+1;
						icon_x = x;
					}
					if((y == WELL_HEIGHT-1) || (t != down && t <= BLOCK_BLUE)){//the score has ended, found the edge of the well or a different color marker or empty spot
						
						if(t == down)//must have found the well edge, don't leave this one behind(not trivial to understand why this is needed..)
							downpos = y;

						//THIS IS THE BROKEN PART??!?
						uppos *= WELL_WIDTH;//uppos += (y*WELL_WIDTH);
						downpos *= WELL_WIDTH;//downpos += (y*WELL_WIDTH);
						uppos += x;
						downpos += x;
						
						while(uppos <= downpos){
							p[i].well[uppos] |= SCORE;//mark everything from the top to bottom marker(there could be more than 1 bottom marker, but we store the last found)
							numscores++;
							uppos += WELL_WIDTH;
						//db();
						}
						up = down = 255;//clear the markers so we can score more than once per line
					}else if(t == down){//we found another bottom marker. the score is NOT over, there might be another matching marker below this
						downpos = y;
					}
					//else we found a star or something, which neither means the score is over, nor means it isn't
				}
			}else{//empty
				up = down = 255;
			}
		}
	}
	
	
	uint8_t pt = PARTICLE_STAR;
	if(numscores){//a score happened, throw a particle
		if(true){//p[i].chain > 1){//do the chain counter icon
			pt = p[i].chain;
			
		}//else first score, draw a flying star instead of the chain counter
		icon_x <<= 1;//convert to tile coords
		icon_y <<= 1;//convert to tile coords
		icon_x += (i?15:2);//add border offset
		icon_x <<= 3;//convert to sprite coords
		icon_y <<= 3;//convert to sprite coords
		AddParticle(icon_x,icon_y,pt,PARTICLE_NORMAL_TIME);
	}
	
	return numscores;
}


uint8_t ClearScoredBlocks(uint8_t i){

	uint8_t bombs_found = 0;
	uint8_t vx = (i?15:2);
	uint8_t vy = 2;
	
	for(uint8_t j=WELL_WIDTH;j<WELL_SIZE;j++){//first row is invisible
		uint8_t t = p[i].well[j];
		if(t & SCORE){
			if((t&0x0F) == BLOCK_BOMB)
				bombs_found++;
			
			if((t&0x0F) == BLOCK_GARBAGE){//special case, this turns into a star
				p[i].well[j] = BLOCK_STAR;
				DrawBlock(vx,vy,BLOCK_STAR);//draw the star
			}else{
				p[i].well[j] = 0;
				DrawBlock(vx,vy,0);
			}
		}
		vx += 2;
		if(vx >= (WELL_WIDTH<<1)+(i?15:2)){
			vx = (i?15:2);
			vy += 2;
		}
	}
	return bombs_found;
}


uint8_t RotatePiece(uint8_t i, uint8_t inp){
	if(p[i].padState == BTN_B){
		
	}
	return 0;
}


const int8_t orbital_vpos_lookup[] PROGMEM = {//this is for drawing positions
	0,-2,	2,0,	0,2,	-2,0,
};

const int8_t orbital_grid_lookup[] PROGMEM = {//this is for grid logic
-WELL_WIDTH,	1,	WELL_WIDTH,	-1,
};

void UartSend_uint16_t(uint16_t t){
	
}

void UartSend_uint8_t(uint8_t t){
	
}

#define PACKET_AWAIT_TYPE	0
#define PACKET_AWAIT_IPD	1
#define PACKET_GET_LENGTH	2
#define PACKET_READ_PAYLOAD	3
#define PACKET_ERROR		255
void SendPadUpdate(){/*
	//first send our tick, so that the other side can make sure they did not lose data(probably not on the TCP side, but perhaps UART?)
	NetSend_uint16_t(p[0].tick);//send our current tick. we are always player 0 in our own mind, so are they
	//make sure the other side always knows what our LFSR is, to make synchronizing them easy across games.
	UartSend_uint16_t(p[0].lfsr);//the remote side can detect desyncs, if the lfsr does not match what it expected
	
	UartSend_uint8_t(p[0].padState);//this is compressed to 1 byte because we do not use all the buttons in this game

	if(++net_last_sync_spot == WELL_SIZE)//every tick, we send the other side an authorative state about 1 grid space of our well. Over the course of about a second, any desyncs will be fixed.
		net_last_sync_spot = WELL_WIDTH;
	UartSend_uint8_t(p[0].net_last_sync_spot);
	UartSend_uint8_t(p[0].well[net_last_sync_spot);
*/
}
/*
void SendFullSync(){//something bad happened, and the other side is requesting an exact replica of our state before play can continue
	
}

uint8_t packet_digits[4];
uint8_t packet_digits_pos = 0;
void RecvUpdate(){
	//this NEEDS TO BE FAST, and also needs to eat every single byte sent to get it out of the UART buffer. This takes priority over everything else, even game logic, otherwise we WILL have poor network play!!
	while(UartUnreadcount() && packet_parsing_state != PACKET_ERROR){//run through all bytes we have received from the ESP8266 and update the state machine
		
		if(packet_parsing_state == PACKET_AWAIT_TYPE){//we are awaiting the start of a message type from the ESP8266. This is either "+IPD=.." or something we can't handle ie. an error
			if(UartReadChar() == '+')//hopefully this is "+IPD..", we will call it an error if it contains anything else(probably a disconnect)
				packet_parsing_state = PACKET_AWAIT_IPD;
			else
				packet_parsing_state = PACKET_ERROR;
			continue;
		}
		
		if(packet_parsing_state == PACKET_AWAIT_IPD){
			char c = UartReadChar();
			if(c != 'I' && c != 'P' && c != 'D' && c != '=')
				packet_parsing_state = PACKET_ERROR;
			else{
				packet_parsing_state = PACKET_GET_LENGTH;
				packet_digits_pos = 0;
			}
			continue;
		}
		
		if(packet_parsing_state == PACKET_GET_LENGTH){
			char c = UartReadChar();
			if(c == ':'){//next byte is our data
				//figure out what length it is
			}else
				packet_digits[packet_digit_pos++] = c-'0';
			continue;
		}
		
		if(packet_parsing_state == PACKET_READ_PAYLOAD){
			continue;
		}
	}
}
*/
void SynchronizeLFSR(){
	uint16_t rolling_lfsr = 0b1010101011100010;
	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		for(uint8_t j=0;j<32;j++){
			GetPrngNumber(p[i].lfsr);
			p[i].lfsr = GetPrngNumber(0);
			rolling_lfsr += p[i].lfsr;
			if(rolling_lfsr & 0b0000010000000000)
				rolling_lfsr = ((rolling_lfsr&0b1111111100000000)>>8)|((rolling_lfsr&0b0000000011111111)<<8);
			else if(rolling_lfsr & 0b0000000010001000){
				for(uint8_t k=0;k<(rolling_lfsr&0b11111);k++)
					rolling_lfsr *= (rolling_lfsr+rolling_lfsr);
			}
		}
	}
	for(uint8_t i=0;i<MAX_PLAYERS;i++)//make sure all players get the same order of blocks throughout the match
		p[i].lfsr = rolling_lfsr;
}

const uint8_t garbage_pile[] PROGMEM = {
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,

BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR+0,BLOCK_RED,BLOCK_BOMB,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,
BLOCK_GREEN,BLOCK_BLUE,BLOCK_RED,BLOCK_STAR+1,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR+2,
BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR+0,BLOCK_RED,BLOCK_STAR+0,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,
BLOCK_GREEN,BLOCK_BLUE,BLOCK_RED,BLOCK_STAR+1,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,
BLOCK_RED,BLOCK_GREEN,BLOCK_BLUE,BLOCK_STAR+2,
BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
BLOCK_BLUE,BLOCK_GREEN,BLOCK_RED,
};
void AddGarbageLine(uint8_t i){
	uint8_t off = WELL_WIDTH;
	GetPrngNumber(p[i].rolling_lfsr);
	
	for(uint8_t x=0;x<WELL_WIDTH;x++){//move all blocks up one space, and redraw if there is anything there
		off = WELL_WIDTH+x;
		for(uint8_t y=1;y<WELL_HEIGHT-1;y++){
			if(p[i].well[off+WELL_WIDTH]){
				p[i].well[off] = p[i].well[off+WELL_WIDTH];
				DrawBlockGrid(x,y,i);
			}
			off += WELL_WIDTH;
		}
	}
	
	off = WELL_SIZE-WELL_WIDTH;
	for(uint8_t x=0;x<WELL_WIDTH;x++){
		
		uint8_t first_up = 255;
		uint8_t first_left = 255;
		for(uint8_t y=WELL_HEIGHT-1;y>0;y--){
			uint8_t t = p[i].well[(y*WELL_WIDTH)+x];
			if(t >= BLOCK_RED && t <= BLOCK_BLUE){
				first_up = t;
				break;
			}
		}
		
		if(x){
			for(int8_t xb=x-1;xb>=0;xb--){//scan left looking for the first score marker
				uint8_t t = p[i].well[(WELL_SIZE-WELL_WIDTH)+xb];
				if(t >= BLOCK_RED && t <= BLOCK_BLUE){
					first_left = t;
					break;
				}
			}
		}
		
		uint8_t r;
		do{//find a random block, that will not cause a score in either direction
			r = (GetPrngNumber(0)%sizeof(garbage_pile));
			r = pgm_read_byte(&garbage_pile[r]);
		}while(r == first_up || r == first_left);
		
		p[i].well[off++] = r;
		DrawBlockGrid(x,WELL_HEIGHT-1,i);
	}
}

static uint8_t ProcessLogic(){


////////////////////////////////TODO MAKE CLONE OF THE GAME CALLED "CHAIN REACTION"
/////////////
///////
////
	
	//First, if a network game is in progress, put out our padstate immediately to minimize latency and maximize UART buffer usage. The tick the input happened needs to be sent first
	UartSend_uint16_t(p[0].tick);//we are always player 1 and they are always player 2, in our mind. They view us as player 2, and themself as player 1.
	UartSend_uint8_t(p[0].padState);//this is compressed, since we don't use all the SNES buttons for this game.
	//do not send anymore, until we calculate the outcome of this tick. They will compare the simulation on their side, and compare to our results.

	//GET INPUT STUFF
	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		if(p[i].state == STATE_DISABLED)
			continue;
//DrawBigChar(0,0,'A'+p[0].state);
LOGIC_TOP:
		if(p[!i].state == STATE_LOST_GAME){
			//p[i].state = STATE_WON_GAME;
			//p[i].state_count = 0;
		}
		
		if(p[i].state == STATE_AWAIT_READY){
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//p[0].oldPadState = 0;p[0].padState=BTN_START;
			if(!p[i].next[0])
				SpawnNewPiece(i);//this also draws the preview
			DrawBigString((i?15:2),3,PSTR("READY]"));
			if(p[!i].state == STATE_COUNT_DOWN){
				p[i].state = STATE_COUNT_DOWN;
				p[i].state_count = p[!i].state_count;
			}
			if((p[i].padState & BTN_START) && !(p[i].oldPadState & BTN_START)){
				p[i].state = STATE_COUNT_DOWN;
				p[i].state_count = (30*4)-1;
			}
			
		}else if(p[i].state == STATE_COUNT_DOWN){
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			DrawBigString((i?15:2),3,PSTR("READY["));
			if(!p[!i].state || (p[!i].state >= STATE_COUNT_DOWN)){
					
					if(p[i].state_count > 30)
						DrawBigChar((i?20:7),6,'0'+(p[i].state_count/30));
					else
						DrawBigString((i?17:4),6,PSTR("GO[["));
					if(--p[i].state_count == 0){
						p[i].state = STATE_SPAWN_BLOCKS;
						SpawnNewPiece(i);
						DrawRectangle((i?15:2),3,12,5,FIRST_BORDER_TILE);//blank out the round start text
					}
			}
			
		}else if(p[i].state == STATE_LOST_GAME){
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if(p[i].state_count < 4){
				p[i].state_count++;
				continue;
			}
			p[i].state_count = 0;
			uint8_t loff = WELL_WIDTH+2;
			uint8_t skullplaced = 0;
			for(uint8_t y=1;y<WELL_HEIGHT;y++){

				if(p[i].well[loff+0] && p[i].well[loff+0] != BLOCK_SKULL){
					p[i].well[loff+0] = BLOCK_SKULL;
					skullplaced = 1;
					DrawBlockGrid((loff+0)%WELL_WIDTH,(loff+0)/WELL_WIDTH,i);
				}
				if(p[i].well[loff+1] && p[i].well[loff+1] != BLOCK_SKULL){
					p[i].well[loff+1] = BLOCK_SKULL;
					skullplaced = 1;
					DrawBlockGrid((loff+1)%WELL_WIDTH,(loff+1)/WELL_WIDTH,i);
				}
				if(skullplaced){//only do 1 row per tick
					if(loff == WELL_WIDTH+2){//this is the first tick of the losing animation, call the sound effect
						p[i].state_count = 0;
						StopSong();
						AS_TriggerFx(0);
					}
					break;
				}
				loff += WELL_WIDTH;
			}
			if(!skullplaced){//losing animation is done
				if(p[i].state_count < 60){//give the losing sound effect some time to finish
					if(++p[i].state_count == 60){//start the song
						//StartSong(MUS_GAME_OVER);
					}
				}
			}
			
		}else if(p[i].state == STATE_WON_GAME){//possible transitions to: STATE_AWAIT_READY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
			
			uint8_t loff = (WELL_SIZE-WELL_WIDTH)-((p[i].state_count/6)*WELL_SIZE);
			for(uint8_t x=0;x<WELL_WIDTH;x++)//do 1 row per 6 ticks
				p[i].well[loff++] = BLOCK_EMPTY;

			if((p[i].padState & BTN_START) && !(p[i].oldPadState & BTN_START))
				p[i].state = STATE_AWAIT_READY;
			SynchronizeLFSR();//make sure both players get the same block order next game(and take any network issues into consideration?)

		}else{//active playing states, where the in game menu can be brought up
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if((p[i].padState & BTN_START) && !(p[i].oldPadState & BTN_START) && !p[i].sub_state){
				p[i].sub_state = SUBSTATE_PAUSE_MENU_BACK;
			}
			if(p[!i].sub_state)//we cannot continue our game logic until the other player leaves their menu
				continue;
			
			if(p[i].sub_state){//we need to process the menu and pause logic
			
				if((p[i].padState & BTN_UP) && !(p[i].oldPadState & BTN_UP)){
					if((--p[i].sub_state) == 0)
						p[i].sub_state = SUBSTATE_PAUSE_MENU_QUIT;
				}else if((p[i].padState & BTN_DOWN) && !(p[i].oldPadState & BTN_DOWN)){
					if((--p[i].sub_state) > SUBSTATE_PAUSE_MENU_QUIT)
						p[i].sub_state = SUBSTATE_PAUSE_MENU_BACK;
				}else if((p[i].padState & BTN_START) && !(p[i].oldPadState & BTN_START)){//the user made a selection
					if(p[i].sub_state == SUBSTATE_PAUSE_MENU_BACK)
						p[i].sub_state = 0;
					else
						return 1;//end logic and return to title screen
				}
				continue;
			}
			
			if(p[i].state == STATE_SPAWN_BLOCKS){//possible transitions to: STATE_LOST_GAME, STATE_MOVING
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				p[i].piece_count++;
				if(!(p[i].piece_count&1))
					AddGarbageLine(i);
				if(p[i].well[WELL_WIDTH+2] || p[i].well[WELL_WIDTH+3]){//player loses
					p[i].state = STATE_LOST_GAME;
					p[i].state_count = 0;
				}else{
					SpawnNewPiece(i);
					p[i].state = STATE_MOVING;
				}
				//goto LOGIC_TOP;//this state does not last a whole tick, but it is easier to break it up into more states instead of inline...yes I believe goto makes *some* things easier to understand!

			}else if(p[i].state == STATE_SCORING){//possible transitions to: STATE_SCORE_FLASHING, STATE_CHAIN_END, STATE_GARBAGE
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				p[i].state_count = 0;
				if(!ProcessScoring(i)){//no score happened
					if(!p[i].added_chain && p[i].chain >= 2)
						p[i].state = STATE_CHAIN_END;//the end of the player controlled chain, now drop stars until scores stop happening
					else if(p[i].chain >= 2 && p[i].added_chain){
						p[i].state = STATE_ADDED_CHAIN_END;//last star drop didn't score. this state exists only to escap the loop!
	ClearScoredBlocks(i);//this is necessary to eliminate any stars that fell
	p[!i].total_garbage_rx += p[i].chain-1;
					}else{//no chain
						p[i].state = STATE_GARBAGE;
						p[i].chain = 0;
						p[i].added_chain = 0;
					}
				}else{//a score did happen
					p[i].chain++;
					p[i].total_stars += p[i].iteration_stars;
					AS_TriggerFx(33);
					p[i].state = STATE_SCORE_FLASHING;
				}
		
			}else if(p[i].state == STATE_SCORE_FLASHING){//possible transitions to: STATE_BOMB, STATE_GRAVITY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				if(++p[i].state_count < 48){
					if(p[i].state_count&4)
						UndrawScored(i);
					else
						DrawScored(i);
				}else{//done flashing
					/*uint8_t bombs_cleared = */ClearScoredBlocks(i);//returns >0 if a bomb was cleared
					if(false){//bombs_cleared){
						p[i].state = STATE_BOMB;
						p[i].state_count = 0;
					}else{//no bombs, apply gravity to fill in any newly opened spaces
						if(p[i].added_chain)
							p[i].state = STATE_CHAIN_END;//drop more stars
						else
							p[i].state = STATE_GRAVITY;
						p[i].state_count = 0;
					}
				}
				
			}else if(p[i].state == STATE_GRAVITY){//possible transitions to: STATE_SCORING
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				
				if(p[i].state_count < 5+3){//slow gravity down a bit
					p[i].state_count++;
					continue;
				}
				if(!ProcessGravity(i)){
					p[i].state = STATE_SCORING;
					p[i].state_count = 0;
				}//else continue processing gravity until nothing falls

			}else if(p[i].state == STATE_MOVING){//possible transitions to: STATE_SET_BLOCKS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				//p[i].blocks[0] |= BLOCK_RED;
				//first blank out the block from last frame, in case it moves due to rotation or falling(avoid repeating code, same cycles in worst case)
				uint8_t half_step = p[i].blocks[0] & HALFSTEP;//DrawBlock() always takes into account HALFSTEP(which would draw 1 tile lower), stored in the tile value itself(bitwise). Passing 0(to blank) needs special consideration over the actual block value.
				int8_t xo = (p[i].x<<1)+(i?15:2);
				int8_t yo = (p[i].y<<1);
				DrawBlock(xo,yo,0|half_step);//blank out "planet block", taking HALFSTEP of piece into account
				DrawBlock(xo+pgm_read_byte(&orbital_vpos_lookup[(p[i].r<<1)+0]),yo+pgm_read_byte(&orbital_vpos_lookup[(p[i].r<<1)+1]),0|half_step);//blank out "orbital block"

				int8_t planet_grid_off = (p[i].y*WELL_WIDTH)+p[i].x;//grid position of the planet piece
				int8_t orbital_grid_off = planet_grid_off+pgm_read_byte(&orbital_grid_lookup[(p[i].r)]);//grid position of the piece that orbits/rotates around

				
				if((p[i].padState & (BTN_LEFT|BTN_RIGHT)) && !(p[i].oldPadState & (BTN_LEFT|BTN_RIGHT))){

						if((p[i].padState & BTN_LEFT)){
							if(p[i].x && !p[i].well[planet_grid_off-1] && !p[i].well[orbital_grid_off-1] && ((p[i].r != 3) || (p[i].x > 1))){//would either block move into something, or the edge?
								if(!(p[i].blocks[0] & HALFSTEP) || (!p[i].well[planet_grid_off+WELL_WIDTH-1] && !p[i].well[orbital_grid_off+WELL_WIDTH-1])){//are we halfway between 2 grid spaces vertically? If so are we overlapping the top of a block?
									p[i].x--;//no, then do it
									planet_grid_off--;
									orbital_grid_off--;
								}
							}
						}else if(p[i].x < WELL_WIDTH-1){//BTN_RIGHT was pushed, would it move into the edge?
							if(!p[i].well[planet_grid_off+1] && !p[i].well[orbital_grid_off+1] && ((p[i].r != 1) || (p[i].x < WELL_WIDTH-2))){//would either block move into something?
								if(!(p[i].blocks[0] & HALFSTEP) || (!p[i].well[planet_grid_off+WELL_WIDTH+1] && !p[i].well[orbital_grid_off+WELL_WIDTH+1])){//are we half way between 2 grid spaces vertically? If so are we overlapping the top of a block?
									p[i].x++;
									planet_grid_off++;
									orbital_grid_off++;
								}
							}
						}
				}
			
				if((p[i].padState & BTN_DOWN) && p[i].state_count > 1)//fall fast
					p[i].state_count = 1;
				else if((p[i].padState & BTN_UP)){//instant drop
				
				}

				uint8_t rr = 255;
				if((p[i].padState & BTN_Y) && !(p[i].oldPadState & BTN_Y)){//try to rotate left(counterclockwise)
					rr = p[i].r-1;
					if(rr > 3)//roll over
						rr = 3;
				}else if((p[i].padState & BTN_B) && !(p[i].oldPadState & BTN_B)){//try to rotate right(clockwise)
					rr = p[i].r+1;
					if(rr > 3)
						rr = 0;
				}
				
				if(rr != 255){//some rotation was requested, see if it is possible
					//we already know for certain that, in it's unmodified position, both blocks are in free spaces

					int8_t pgo = (p[i].y*WELL_WIDTH)+p[i].x;//grid position of the planet piece
					int8_t ogo = pgo+pgm_read_byte(&orbital_grid_lookup[rr]);//grid position of the piece that orbits/rotates around
					int8_t pgo2,ogo2;//position in case of HALFSTEP(where basically the block is overlapping 2 grid spaces vertically)
					if(p[i].blocks[0]&HALFSTEP){
						pgo2 = pgo+WELL_WIDTH;
						ogo2 = ogo+WELL_WIDTH;
					}else{
						pgo2 = pgo;
						ogo2 = ogo;
					}

					if(rr == 1 && (p[i].x == WELL_WIDTH-1 || p[i].well[pgo+1] || p[i].well[pgo2+1])){//this rotation would be overlapping the well side, or another block to it's side
						if(!p[i].well[pgo-1] && !p[i].well[ogo-1]){//can do this rotation by moving left?
							pgo--;
							ogo--;
						}else
							rr = 2;
					}
					
					if(rr == 2 && (p[i].y == WELL_HEIGHT-1 || p[i].well[pgo+WELL_WIDTH] || p[i].well[pgo2+WELL_WIDTH])){//this rotation would be overlapping the well bottom, or another block below it
						if(!p[i].well[pgo-WELL_WIDTH] && !p[i].well[ogo+WELL_WIDTH]){//can do this rotation by moving up?
							pgo -= WELL_WIDTH;
							ogo -= WELL_WIDTH;
						}else
							rr = 3;
					
					}
					
					if(rr == 3 && (p[i].x == 0 || p[i].well[pgo-1] || p[i].well[pgo2-1])){//this rotation would be overlapping the side, or another block to the left
						if(!p[i].well[pgo+1] && !p[i].well[ogo+1]){//can do this rotation by moving right?
							pgo++;
							ogo++;
						}else
							rr = 0;//a rotation that should always be possible!
					
					}

					
					if(rr != p[i].r){//we requested a new, and valid rotation. So be it!
						AS_TriggerFx(0);
						p[i].state_count += 2;//add some time...a player can stall the piece indefinitely..if they can keep up a button press rate of 30hz! Their opponent wont mind when they can't!(not good tactics in VS)
						p[i].x = pgo%WELL_WIDTH;//we know both blocks are in bounds
						p[i].y = pgo/WELL_WIDTH;
						p[i].r = rr;
						planet_grid_off = pgo;
						orbital_grid_off = ogo;
					}
				}
				
				
				if(!p[i].state_count){//handle piece dropping
					p[i].state_count = 45;
				
					if(half_step){
						//half_step = 0;
						p[i].blocks[0] ^= HALFSTEP;
						p[i].blocks[1] ^= HALFSTEP;
						p[i].y++;//a half step will always pass, because the check is made before this state that there is a point to move into for each block(planet and orbital)
					}else{
						//we need to make sure there is a place open for the planet and orbital piece before we set the flag. Setting the flag indicates going "half into" a spot, which must be open
						if(((planet_grid_off+WELL_WIDTH) < WELL_SIZE-1) && ((orbital_grid_off+WELL_WIDTH) < WELL_SIZE-1) &&//not going outside of well boundaries?
							!p[i].well[planet_grid_off+WELL_WIDTH] && !p[i].well[orbital_grid_off+WELL_WIDTH]){//an open spot below each block?
						
							//half_step = 1;
							p[i].blocks[0] |= HALFSTEP;
							p[i].blocks[1] |= HALFSTEP;
						}else{//at least one of the blocks is not free to move, don't start the move into the position. We must lock the pieces and let gravity takes it's course
							p[i].state = STATE_SET_BLOCKS;

						}
					}

				
				}else
					p[i].state_count--;

				//draw the blocks in the new position
				xo = (p[i].x<<1)+(i?15:2);//recalculate position in case it changed
				yo = (p[i].y<<1);
				DrawBlock(xo,yo,p[i].blocks[0]);//draw the "planet block"
				DrawBlock(xo+pgm_read_byte(&orbital_vpos_lookup[(p[i].r<<1)+0]),yo+pgm_read_byte(&orbital_vpos_lookup[(p[i].r<<1)+1]),p[i].blocks[1]);//draw the "orbital block"
			
			}else if(p[i].state == STATE_SET_BLOCKS){//possible transitions to: STATE_GRAVITY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				int8_t planet_grid_off = (p[i].y*WELL_WIDTH)+p[i].x;//grid position of the planet piece
				int8_t orbital_grid_off = planet_grid_off+pgm_read_byte(&orbital_grid_lookup[(p[i].r)]);//grid position of the piece that orbits/rotates around
				p[i].well[planet_grid_off] = p[i].blocks[0];
				p[i].well[orbital_grid_off] = p[i].blocks[1];
				p[i].state = STATE_GRAVITY;
				p[i].state_count = 0;
				goto LOGIC_TOP;//this state does not last a whole tick, but it is easier to break it up into more states instead of inline...yes I believe goto makes *some* things easier to understand!

			}else if(p[i].state == STATE_BOMB){//possible transitions to: STATE_GRAVITY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				p[i].state = STATE_GRAVITY;
				p[i].state_count = 0;

			}else if(p[i].state == STATE_CHAIN_END){//possible transitions to: STATE_GRAVITY
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				uint8_t stars = p[i].chain-1;
				stars = pgm_read_byte(&chain_star_count[stars]);//0,2,4,5,6,12,12,12...

				uint8_t weights[WELL_WIDTH];
				
				for(uint8_t x=0;x<WELL_WIDTH;x++){//find the height of each column, where lower columns are more desirable
					weights[x] = WELL_HEIGHT;
					
					for(uint8_t y=1;y<WELL_HEIGHT;y++){
						if(p[i].well[(y*WELL_WIDTH)+x]){
							weights[x] = y;
							break;
						}
					}
				}
				
				for(uint8_t s=0;s<stars;s++){
					if(stars >= WELL_WIDTH){
						for(uint8_t x=0;x<WELL_WIDTH;x++)//the top invisible row is only used if more than 6 stars are dropped
							p[i].well[x+0] = BLOCK_STAR|SCORE;
						stars -= WELL_WIDTH;
						continue;
					}

					uint8_t max = 2;
					uint8_t mag = 0;
					
					for(uint8_t x=0;x<WELL_WIDTH;x++){//scan through and find the lowest column available
						if(weights[x] > mag){
							mag = weights[x];
							max = x;
						}
					}
					weights[max] = 0;//don't use this column again
					p[i].well[max+WELL_WIDTH] = BLOCK_STAR|SCORE;
				}

			//	p[i].well[2] = p[i].well[3] = BLOCK_STAR|SCORE;//set SCORE to make sure it goes away at the end
				p[i].added_chain = 1;//we need to keep track of this, to avoid getting stuck in a loop of STATE_CHAIN_END->STATE_GRAVITY->STATE_SCORING->STATE_CHAIN_END->STATE_GRAVITY...
				//seems to always prefer the lowest stacks first
				p[i].state = STATE_GRAVITY;//pull these stars down, then see if they score
				p[i].state_count = 0;
				
			}else if(p[i].state == STATE_ADDED_CHAIN_END){//TODO IS THIS EVEN NECESSARY???!?!
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				p[i].chain = 0;//break the cycle!
				p[i].state = STATE_GRAVITY;//TODO IS THIS STATE EVEN NECESSARY????!?
				
			}else if(p[i].state == STATE_GARBAGE){//possible transitions: STATE_SPAWN_BLOCKS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				

				if(p[i].state_count){
					p[i].state_count--;
					continue;
				}
				if(p[i].total_garbage_processed < p[i].total_garbage_rx){
					AddGarbageLine(i);
					p[i].state_count = 15;
					p[i].total_garbage_processed++;

					
				}else if(false){//p[i].padState & BTN_)){//player is intentionally adding garbage to fill an empty well(presumably to attack with!)
					AddGarbageLine(i);
					p[i].state_count = 30;//check again in 1/2 second, they can add as much as they want by holding down the button
				}else
					p[i].state = STATE_SPAWN_BLOCKS;
				
			}//else if(p[i].state == ...
		}//active playing states, where the in game menu can be brought up
	}//for(uint8_t i=0;i<MAX_PLAYERS;i++)
		
	
	////every tick, we send the other side an authorative state about 1 grid space of our well. Over time(about 1 second), this should allow us to fix desyncs automatically
	if(++p[0].net_last_sync_spot == WELL_SIZE)
		p[0].net_last_sync_spot = WELL_WIDTH;
	UartSend_uint8_t(p[0].net_last_sync_spot);
	UartSend_uint8_t(p[0].well[p[0].net_last_sync_spot]);
	
	return 0;
}

	
void ResetGame(){
	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		uint8_t *t = (uint8_t *)&p[i];
		for(uint8_t j=0;j<sizeof(player_t);j++)
			*t = 0;
		
		p[i].x = 2;
	}
}


void AnimateStars(){
//return;
	static uint8_t offset = 0;
	uint8_t start = offset;

	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		uint8_t vxoff = (i?15:2);
		offset = start;
		for(uint8_t k=0;k<WELL_WIDTH/1;k++){
			uint8_t t = p[i].well[offset];
			if(!(t & SCORE) && (t & 0x0F) >= BLOCK_STAR && (t & 0x0F) < BLOCK_SKULL){
				if((++t & 0x0F) == BLOCK_SKULL)//roll over
					t = (BLOCK_STAR|(t & HALFSTEP));
				p[i].well[offset] = t;
				DrawBlock(vxoff+((offset%WELL_WIDTH)<<1),0+((offset/WELL_WIDTH)<<1),t);
			}
			offset++;
		}
	}

	if(offset >= WELL_WIDTH*WELL_HEIGHT)
		offset = 0;
}

/*
void DrawBigSpriteChar(uint8_t x, uint8_t y, uint8_t c){
	uint16_t moff = c*4;
	SetSprite(x+0,y+0,pgm_read_byte(&big_font_maps[moff++]),0);
	SetSprite(x+8,y+0,pgm_read_byte(&big_font_maps[moff++]),0);
	SetSprite(x+0,y+8,pgm_read_byte(&big_font_maps[moff++]),0);
	SetSprite(x+8,y+8,pgm_read_byte(&big_font_maps[moff++]),0);

}


void DrawSpriteString(uint8_t x, uint8_t y, const char *str){
	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(str++);//&(str[i++]));		
		if(c!=0){
			if(c == ' '){
				x += 16;
				continue;
			}
			//c&= 127;//c=((c&127)-32);			
			if(c >= '0' && c <= '9')
				c -= '0';
			else if(c >= 'A' && c <= '^')
				c -= '7';

			DrawBigSpriteChar(x,y,c);
			x += 16;
		}else{
			break;
		}
	}
}


void DrawRamTileMap(uint8_t x, uint8_t y, const char *map){
	uint8_t mapWidth=pgm_read_byte(&(map[0]));
	uint8_t mapHeight=pgm_read_byte(&(map[1]));
		
	for(uint8_t dy=0;dy<mapHeight;dy++)
		for(uint8_t dx=0;dx<mapWidth;dx++)
			vram[((y+dy)*VRAM_TILES_H)+(x+dx)] = pgm_read_byte(&map[(dy*mapWidth)+dx+2]);					

}
*/
void DrawBigChar(uint8_t x, uint8_t y, uint8_t c){
	if(c >= '0' && c <= '9')
		c -= '0';
	else if(c >= 'A' && c <= '^')
		c -= '7';
	uint16_t moff = c*4;
	uint16_t voff = (y*VRAM_TILES_H)+x;
	
	vram[voff++]	= pgm_read_byte(&big_font_maps[moff++]);
	vram[voff]		= pgm_read_byte(&big_font_maps[moff++]);
	
	voff += VRAM_TILES_H-1;
	vram[voff++]	= pgm_read_byte(&big_font_maps[moff++]);
	vram[voff]		= pgm_read_byte(&big_font_maps[moff]);

}

void DrawBigString(uint8_t x, uint8_t y, const char *str){
//	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(str++);//&(str[i++]));		
		if(c!=0){
			if(c == ' '){
				x += 2;
				continue;
			}
			//c&= 127;//c=((c&127)-32);			

			DrawBigChar(x,y,c);
			x += 2;
		}else{
			break;
		}
	}
}
void DrawWell(uint8_t i){
	uint8_t woff = WELL_WIDTH;//first row is invisible
	for(uint8_t y=0+2;y<2+24;y+=2){
		for(uint8_t x=0+2;x<2+12;x+=2){
			uint8_t t = p[i].well[woff++];
			DrawBlock(x,y,t);
		}
	}
}

void DrawScored(uint8_t i){
	uint8_t woff = WELL_WIDTH;//first row is invisible
	uint8_t voff = (i?13:0);
	
	for(uint8_t y=0+2;y<2+24;y+=2){
		for(uint8_t x=0+2;x<2+12;x+=2){
			uint8_t t = p[i].well[woff++];
			if(t & SCORE){
				t = 0|(t&HALFSTEP);
				DrawBlock(x+voff,y,t);
			}
		}
	}
}

void UndrawScored(uint8_t i){
	uint8_t woff = WELL_WIDTH;//first row is invisible
	uint8_t voff = (i?13:0);
	
	for(uint8_t y=0+2;y<2+24;y+=2){
		for(uint8_t x=0+2;x<2+12;x+=2){
			uint8_t t = p[i].well[woff++];
			if(!(t & SCORE)){
//while(1);			
			continue;
			}
			DrawBlock(x+voff,y,t);
		}
	}
}

const char round_text_rt_map[] PROGMEM = {
1,2,		5,6,		9,10,		13,14,		17,18,		21,22,
3,4,		7,8,		11,12,		15,16,		19,20,		23,24,
};



void DrawPreview(uint8_t i){
	uint16_t voff = (1*VRAM_TILES_H)+(i?19:6);
	uint8_t toff = ((p[i].next[0]-1)<<2)+RAM_TILES_COUNT+2;

	vram[voff++] = toff++;
	vram[voff++] = toff;
	
	toff = ((p[i].next[1]-1)<<2)+RAM_TILES_COUNT+2;

	vram[voff++] = toff++;
	vram[voff] = toff;
}


void DrawScreen(){
	FillScreenPattern();
	DrawFrame(5,0,6,2);	
	DrawFrame(1,1,14,26);

	DrawFrame(18,0,6,2);
	DrawFrame(14,1,14,26);

	for(uint8_t i=6;i<6+4;i++){
		SetTile(i+0,1,FIRST_BORDER_TILE);
		SetTile(i+13,1,FIRST_BORDER_TILE);
	}

	SetTile(14,1,FIRST_BORDER_TILE+7);
	//SetTile(14,SCREEN_TILES_V-1,FIRST_BORDER_TILE+7);
	SetTile(5,1,FIRST_BORDER_TILE+4);
	SetTile(10,1,FIRST_BORDER_TILE+3);
	SetTile(18,1,FIRST_BORDER_TILE+4);
	SetTile(23,1,FIRST_BORDER_TILE+3);
	
	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		DrawPreview(i);
		DrawWell(i);
	}
	

}


int main(){
	
	SetUserPostVsyncCallback(&VsyncHook);
	InitMusicPlayer(patches);
	GetPrngNumber(GetTrueRandomSeed());
	SetSpritesTileBank(0,sprite_tiles);
	SetTileTable(rom_tiles);
	
//	Intro();
	SetMasterVolume(AS_MASTER_VOLUME);
	
TITLE_TOP:
	TitleScreen();
	ai[0].pawn = 1;
	ai[0].state = AI_WAIT_ROUND_START;
	p[1].network_state = 1;
//GAME_TOP:
/*eof(p[0].well);i++){
	p[0].well[i] = (i%BLOCK_STAR)+1;
	p[1].well[i] = p[0].well[i];
}
*/
	p[0].state = STATE_AWAIT_READY;
	p[1].state = STATE_AWAIT_READY;
	p[0].next[0] = 0;
	p[1].next[0] = 0;
	//p[0].next[0] = BLOCK_BLUE;
	//p[0].next[1] = BLOCK_RED;
	//p[1].next[0] = BLOCK_GREEN;
	//p[1].next[1] = BLOCK_RED;
	ResetGame();
	DrawScreen();
	
	FadeIn(1,false);
	/*
	for(uint8_t i=0;i<WELL_SIZE;i++)
		p[0].well[i] = 0;
	for(uint8_t i=(WELL_WIDTH*1);i<WELL_WIDTH*6;i++){
		uint8_t t;
		if(!(i%WELL_WIDTH) || (i%WELL_WIDTH) == 3)
			t = BLOCK_BLUE;
		else
			t = BLOCK_STAR;
		if((i%WELL_WIDTH) > 3)
			continue;
		p[0].well[i] = t;

		DrawBlockGrid(i%WELL_WIDTH,i/WELL_WIDTH,0);
	}
	p[0].well[(WELL_WIDTH*5)+4] = BLOCK_BLUE;
	DrawBlockGrid(4,5,0);
	
	ProcessScoring(0);
	ClearScoredBlocks(0);
	while(ProcessGravity(0)){
		WaitVsync(1);
	}
	while(1);
	*/
	while(1){
		ResetSprites();
		if(ProcessLogic())
			goto TITLE_TOP;
		ProcessAI();
		AnimateStars();
		ProcessParticles();
		global_frame++;
//DrawScreen();
		WaitVsync(1);
	}
}

uint16_t AISimulateMove(uint8_t i, uint8_t m);
uint16_t AIEvaluateLayout(uint8_t i);

void ProcessAI(){
//	DrawBigChar(14,10,'0'+ai[0].state);
//	DrawBigChar(14,12,'0'+ai[0].target_x);
//	DrawBigChar(14,14,'0'+ai[0].target_rotation);
	for(uint8_t i=0;i<MAX_AIS;i++){
		if(!ai[i].state)
			continue;
		uint8_t pawn = ai[i].pawn;
		
		if(p[pawn].state == STATE_LOST_GAME || p[pawn].state == STATE_WON_GAME)
			continue;
		
		if(p[pawn].state == STATE_SPAWN_BLOCKS){
	//	db();
			ai[i].state = AI_CALCULATE_MOVE;
			ai[i].padState |= BTN_DOWN;
			ai[i].padState ^= BTN_DOWN;
		}
		
		if(ai[i].state & AI_CALCULATE_MOVE){
//db();
		//	if(!(p[pawn].state & STATE_MOVING))
			//	continue;
			if(ai[i].iteration < WELL_WIDTH*4)
				ai[i].score[ai[i].iteration] = AISimulateMove(i,ai[i].iteration++);
			else{//done thinking about all moves, now choose the best one
				uint16_t mag = 0;
				uint8_t best = 0;//(WELL_WIDTH*4)/2;//a default incase of an all-way tie
				
				for(uint8_t j=0;j<WELL_WIDTH*4;j++){
					if(ai[i].score[j] >= mag){
						mag = ai[i].score[j];
						best = j;
					}
				}
				ai[i].state = AI_IMPLEMENT_MOVE;
				ai[i].target_x = best/4;
				ai[i].target_rotation = best%4;
				ai[i].iteration = 0;
			}
			
		}else if(ai[i].state & AI_IMPLEMENT_MOVE){
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//handle rotation
			if(!(ai[i].padState & (BTN_SL|BTN_SR|BTN_Y|BTN_B))){//let the button up first
				if(p[pawn].r != ai[i].target_rotation){
					if(ai[i].target_rotation == 0 && p[pawn].r == 3)
						ai[i].padState |= BTN_SR;
					else{
						if(ai[i].target_rotation < p[pawn].r)
							ai[i].padState |= BTN_Y;
						else
							ai[i].padState |= BTN_B;
					}
				}
			}else{//clear it for this frame
				ai[i].padState |= (BTN_SL|BTN_SR|BTN_Y|BTN_B);
				ai[i].padState ^= (BTN_SL|BTN_SR|BTN_Y|BTN_B);
			}
			
			//handle translation
			if(!(ai[i].padState & (BTN_LEFT|BTN_RIGHT|BTN_UP|BTN_DOWN))){//let the button up first
				if(p[pawn].x != ai[i].target_x){
					if(ai[i].target_x < p[pawn].x)
						ai[i].padState |= BTN_LEFT;
					else
						ai[i].padState |= BTN_RIGHT;
				}
			}else{//clear it for this frame
				ai[i].padState |= (BTN_LEFT|BTN_RIGHT|BTN_UP|BTN_DOWN);
				ai[i].padState ^= (BTN_LEFT|BTN_RIGHT|BTN_UP|BTN_DOWN);
			}
			
			if(p[pawn].r == ai[i].target_rotation && p[pawn].x == ai[i].target_x){
				//if(p[pawn].state != STATE_MOVING)//done moving the piece, think about the next one
					ai[i].state = AI_WAIT_NEXT_MOVE;
				//else
				//	ai[i].padState |= BTN_DOWN;//drop the piece faster
			}

		}else if(ai[i].state & AI_WAIT_NEXT_MOVE){
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ai[i].padState |= BTN_DOWN;
			//if(p[pawn].state & STATE_SPAWN_BLOCKS)
				//ai[i].state = AI_CALCULATE_MOVE;

		}else if(ai[i].state & AI_WAIT){
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if(ai[i].wait == 0)
				ai[i].state ^= AI_WAIT;
			else
				ai[i].wait--;
		}else if(ai[i].state & AI_WAIT_ROUND_START){
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			if((p[pawn].state == STATE_AWAIT_READY) || (p[pawn].state == STATE_COUNT_DOWN)){
				
			}else
				ai[i].state = AI_CALCULATE_MOVE;
			
		}
	}
}

void AIBufferWell(uint8_t i){
	
	for(uint8_t j=0;j<WELL_SIZE;j++)
		ai[i].well[j] = (p[ai[i].pawn].well[j]);//copy the well and drop any flags
}

void AISimulateGravity(uint8_t i){
	uint8_t off;
	
	for(uint8_t x=0;x<WELL_WIDTH;x++){
		off = x+WELL_WIDTH;
		
		for(uint8_t y=1;y<WELL_HEIGHT-1;y++){//move all blocks down until they land on something
			if(ai[i].well[off]){
				if(!ai[i].well[off+WELL_WIDTH]){
					ai[i].well[off+WELL_WIDTH] = ai[i].well[off];
					ai[i].well[off] = 0;
				}
			}
			off += WELL_WIDTH;
		}
	}
}

void AIDrawWell(uint8_t i);

uint8_t AISimulateScoring(uint8_t i){
	

	uint8_t numscores = 0;
	uint8_t chain = 0;
	uint8_t didstars = 0;
	uint8_t firstpass = 0;

AI_SCORING_TOP:
	if(!firstpass){
		firstpass = 1;
		goto AI_SCORING_GRAVITY;//pull the blocks we just placed, down
	}
	uint8_t off = WELL_WIDTH;//first row is not visible
	uint8_t left,right,t;
	uint8_t leftpos=0,rightpos=0;
	uint8_t iterationscores = 0;

	//scan horizontally
	for(uint8_t y=1;y<WELL_HEIGHT;y++){
		left = right = 255;//clear the markers for a new row
		for(uint8_t x=0;x<WELL_WIDTH;x++){
			t = ai[i].well[off++];//flags are dropped during copy
			if(t || right != 255){
				if(left == 255){//we must find a left score marker first
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is a score marker
						left = t;//store it
						leftpos = x;
					}
				}else if(right == 255){//once we have a left, we must find a right
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is  not a star or anything else, this can make or break the score
						if(t != left){//we found a different color marker, so everything to the left is not a score. start a new scan from here
							left = t;//store the new left marker for the new search
							leftpos = x;
						}else{
							right = t;
							rightpos = x;
							if(x == WELL_WIDTH-1)//need to short circuit, otherwise this score wont process. Would need redundant code without the goto
								goto AI_SCORING_HORIZONTAL_SHORT_CIRCUIT;
						}
					}
				}else{//we have a left and a right so a score did happen, we just need to see how far it goes(more of the same color on the right or side of well?)
AI_SCORING_HORIZONTAL_SHORT_CIRCUIT:

					if((x == WELL_WIDTH-1) || (t != right && t <= BLOCK_BLUE)){//the score has ended, found the edge of the well or a different color marker or empty spot
						
						if(t == right)//must have found the well edge, don't leave this one behind(not trivial to understand why this is needed..)
							rightpos = x;

						leftpos += (y*WELL_WIDTH);
						rightpos += (y*WELL_WIDTH);
						
						while(leftpos <= rightpos){
							ai[i].well[leftpos++] |= SCORE;//mark everything from the left to rightmost marker(there could be more than 1 right marker, but we store the last found)
						}
						left = right = 255;//clear the markers so we can score more than once per line
					}else if(t == right){//we found another right marker. the score is NOT over, there might be another matching marker to the right of this
						rightpos = x;
					}
					//else we found a star or something, which neither means the score is over, nor means it isn't
				}
			}else{//empty
				left = right = 255;
			}
		}
	}


	uint8_t up,down;
	uint8_t uppos=0,downpos=0;
	uint8_t fell = 0;
	//scan vertically
	for(uint8_t x=0;x<WELL_WIDTH;x++){
		up = down = 255;//clear the markers for a new column
		off = WELL_WIDTH+x;
		for(uint8_t y=1;y<WELL_HEIGHT;y++){
			t = ai[i].well[off];//flags are dropped during copy
			off += WELL_WIDTH;
			if(t || down != 255){
				if(up == 255){//we must find a top score marker first
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is a score marker
						up = t;//store it
						uppos = y;
					}
				}else if(down == 255){//once we have a top, we must find a bottom
					if(t <= BLOCK_BLUE && t >= BLOCK_RED){//it is  not a star or anything else, this can make or break the score
						if(t != up){//we found a different color marker, so everything to the top is not a score. start a new scan from here
							up = t;//store the new top marker for the new search
							uppos = y;
						}else{
							down = t;
							downpos = y;
							if(y == WELL_HEIGHT-1)//need to short circuit, otherwise this score wont process. Would need redundant code without the goto
								goto AI_SCORING_VERTICAL_SHORT_CIRCUIT;
						}
					}
				}else{//we have an up and a down so a score did happen, we just need to see how far it goes(more of the same color on the bottom or edge of well?)
AI_SCORING_VERTICAL_SHORT_CIRCUIT:

					if((y == WELL_HEIGHT-1) || (t != down && t <= BLOCK_BLUE)){//the score has ended, found the edge of the well or a different color marker or empty spot
						
						if(t == down)//must have found the well edge, don't leave this one behind(not trivial to understand why this is needed..)
							downpos = y;

						//THIS IS THE BROKEN PART??!?
						uppos *= WELL_WIDTH;//uppos += (y*WELL_WIDTH);
						downpos *= WELL_WIDTH;//downpos += (y*WELL_WIDTH);
						uppos += x;
						downpos += x;
						
						while(uppos <= downpos){
							ai[i].well[uppos] |= SCORE;//mark everything from the top to bottom marker(there could be more than 1 bottom marker, but we store the last found)
							uppos += WELL_WIDTH;
						}
						up = down = 255;//clear the markers so we can score more than once per line
					}else if(t == down){//we found another bottom marker. the score is NOT over, there might be another matching marker below this
						downpos = y;
					}
					//else we found a star or something, which neither means the score is over, nor means it isn't
				}
			}else{//empty
				up = down = 255;
			}
		}
	}
	
	

	off = WELL_WIDTH;
	while(off < WELL_SIZE){//clear out scored blocks
		if(ai[i].well[off] & SCORE){
			if(false){//ai[i].well[s] == BLOCK_BOMB){
				
			}else{
				ai[i].well[off] = 0;
db();
				iterationscores++;
			}
		}
		off++;
	}
	AIDrawWell(i);
	if(!iterationscores){//no score this time through
		if(chain > 1){//could stars drop?
			if(didstars)//already dropped stars, and there was no score.
				return numscores;
			else{//need to drop stars and see if they score
				didstars = 1;

			}
		}else//no score, no stars, done.
			return numscores;
	}else{
		chain++;
//db();
		numscores += iterationscores;//add the score, apply gravity, and try for another
		numscores *= chain;
//db();

	}
	
AI_SCORING_GRAVITY:

	do{
		fell = 0;
		off = WELL_WIDTH;
		while(off < (WELL_SIZE-WELL_WIDTH)){//simulate gravity
			if(ai[i].well[off] && !ai[i].well[off+WELL_WIDTH]){
				ai[i].well[off+WELL_WIDTH] = ai[i].well[off];
				ai[i].well[off] = 0;
				fell = 1;
			}
			off++;
		}
//AIDrawWell(i);
	}while(fell);
	
	goto AI_SCORING_TOP;//go back through it all until a score fails to happen
}

void AIDrawWell(uint8_t i){
if(!(p[0].padState & BTN_B))
return;

	uint8_t woff = WELL_WIDTH;//first row is invisible
	for(uint8_t y=0+2;y<2+24;y+=2){
		for(uint8_t x=0+2;x<2+12;x+=2){
			uint8_t t = ai[i].well[woff++]&0x0F;
			DrawBlock(x,y,t);
		}
	}

	WaitVsync(60);
}

uint16_t AISimulateMove(uint8_t i, uint8_t m){
	//the AI does not even think about the next piece to come, better to move faster
	uint8_t rotation = m%4;
	uint8_t column = m/4;
	
	if((column == 0 && rotation == 3) || (column == (WELL_WIDTH-1) && rotation == 1))//impossible, game logic will push the piece off the wall
		return 0;

	AIBufferWell(i);//put the current state of the well into our brain

	uint8_t pawn = ai[i].pawn;
	uint8_t orbit_off;
	
	if(rotation == 0)
		orbit_off = column+(WELL_WIDTH*1);
	else if(rotation == 1)
		orbit_off = column+(WELL_WIDTH*2)+1;
	else if(rotation == 2)
		orbit_off = column+(WELL_WIDTH*3);
	else
		orbit_off = column+(WELL_WIDTH*2)-1;
	
	//put the current piece in the well
	if(!ai[i].well[column+(WELL_WIDTH*3)])
		ai[i].well[column+(WELL_WIDTH*2)] = p[pawn].blocks[0];//planetary
	else
		return 0;
	if(!ai[i].well[orbit_off+(WELL_WIDTH*1)])
		ai[i].well[orbit_off] = p[pawn].blocks[1];//orbital
	else
		return 0;
	
	uint16_t score = 0;
	//AIDrawWell(i);
	/*
	while(1){
		//AISimulateGravity(i);
		AIDrawWell(i);
		uint8_t points = AISimulateScoring(i);
		if(points){
			score += points;
		db();
		}else
			break;
		AIDrawWell(i);
	}
	*/
	score = AISimulateScoring(i);
//	score *= score;
//	score *= score;
	//score *= 32UL;
	score += AIEvaluateLayout(i);

	return score;
}


uint16_t AIEvaluateLayout(uint8_t i){
	return 0;
	return GetPrngNumber(0)%1;
	uint16_t score = 0UL;
	uint8_t t,t2;
	//uint8_t pawn = ai[i].pawn;
	
	for(uint8_t i=1;i<WELL_HEIGHT/2;i++){
		//if(ai[i].well[0+(i*WELL_WIDTH)])
	//		score -= 1;
		//if(ai[i].well[1+(i*WELL_WIDTH)])
			//score -= 1;
		if(!ai[i].well[2+(i*WELL_WIDTH)])
			score += 2;
		if(!ai[i].well[3+(i*WELL_WIDTH)])
			score += 2;
		//if(ai[i].well[4+(i*WELL_WIDTH)])
		//	score -= 1;
		//if(ai[i].well[5+(i*WELL_WIDTH)])
			//score -= 1;

		
		t = ai[i].well[0+(i*WELL_WIDTH)];//two marker stacked over eachother with something in between(chain potential)
		t2 = ai[i].well[0+(WELL_WIDTH*2)+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;

		t = ai[i].well[1+(i*WELL_WIDTH)];//two marker stacked over eachother with something in between(chain potential)
		t2 = ai[i].well[1+(WELL_WIDTH*2)+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;
		
		t = ai[i].well[2+(i*WELL_WIDTH)];//two marker stacked over eachother with something in between(chain potential)
		t2 = ai[i].well[2+(WELL_WIDTH*2)+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;
		
		t = ai[i].well[3+(i*WELL_WIDTH)];//two marker stacked over eachother with something in between(chain potential)
		t2 = ai[i].well[3+(WELL_WIDTH*2)+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;
		
		t = ai[i].well[4+(i*WELL_WIDTH)];//two marker stacked over eachother with something in between(chain potential)
		t2 = ai[i].well[4+(WELL_WIDTH*2)+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;
		
		t = ai[i].well[5+(i*WELL_WIDTH)];//two marker stacked over eachother with something in between(chain potential)
		t2 = ai[i].well[5+(WELL_WIDTH*2)+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;

		
	/*	
		t = ai[i].well[0+(i*WELL_WIDTH)];//check diagonals for fall potential
		t2 = ai[i].well[1+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;

		t = ai[i].well[1+(i*WELL_WIDTH)];
		t2 = ai[i].well[2+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;

		t = ai[i].well[2+(i*WELL_WIDTH)];
		t2 = ai[i].well[3+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;
		
		t = ai[i].well[3+(i*WELL_WIDTH)];
		t2 = ai[i].well[4+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;
		
		t = ai[i].well[4+(i*WELL_WIDTH)];
		t2 = ai[i].well[5+(i*WELL_WIDTH)];
		if(t == t2 && t >= BLOCK_RED && t <= BLOCK_BLUE)
			score += 1;
	*/
	}
	return score;
	
}

