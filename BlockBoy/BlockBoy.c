/*
 *  Block Boy
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

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>

//external data
#include "data/sprites.inc"
#include "data/tiles.inc"
#include "data/fonts.inc"
#include "data/levels.inc"
#include "data/patches.inc"
#include "data/music.inc"
#include "data/frames.inc"
#include "data/titletiles.inc"
#include "data/pausetiles.inc"

extern void TriggerCommon(Track* track,u8 patch,u8 volume,u8 note);
extern unsigned char ram_tiles[];




//#define FASTDEBUG 1//skip a bunch of things, quicker to load and view a level as it is edited
#define TESTLEVEL 14//if you are a masochist and want to make levels manually(wasn't that fun...) consult frames.inc

#define TILE_SKY	0
#define TILE_ROCK	1
#define TILE_DOOR	2
#define TILE_DIRT	3

#define P_FACE_RIGHT	1
#define P_LIFTING		2
#define P_WALKING		4
#define P_JUMPING		8
#define P_FALLING		16
#define P_HOLDING		32
#define P_DUCKING		64//not really ducking, but walking under sometihng that will cause the player to drop the rock
#define P_THROWING		128
#define P_WINNING		255

#define MAX_LEVEL_WIDTH		32//32//3 more than max width of existing levels I found//SCREEN_TILES_H*2
#define MAX_LEVEL_HEIGHT	20//1 more than max height of existing levels I found//SCREEN_TILES_V*2

#define CAM_PLAYER_CENTERING_X_OFFSET	((SCREEN_TILES_H*8)/2)+8
#define CAM_PLAYER_CENTERING_Y_OFFSET	((SCREEN_TILES_V*8)/2)+8
#define CAM_X_LEFT_LIMIT	
#define CAM_Y_UP_LIMIT
#define CAM_X_RIGHT_LIMIT
#define CAM_Y_DOWN_LIMIT
#define CAM_FOLLOW_PLAYER	0
#define CAM_FREE_MOVE		1

#define BB_MASTER_VOLUME 180




typedef struct{
	int16_t x,y,bx,by;
	uint8_t state,ftime,frame,level;
}Player;


typedef struct{
	uint8_t state;
	int16_t ox,oy,nx,ny;
	uint8_t lvl_width,lvl_height;
}Camera;




Player p;
Camera c;
uint8_t sprite_count;
uint8_t lvl[(MAX_LEVEL_WIDTH/4)*MAX_LEVEL_HEIGHT];//160
uint8_t padState,oldPadState;//cannot use SL,SR,X,A...don't need them, need 2 bytes ram
uint16_t level_start;
uint8_t current_song=0;
uint8_t music_off=0;




void VsyncHook();

void BB_triggerFx(uint8_t patch);

static void DrawMetaTile(uint8_t vx, uint8_t vy, uint8_t lx, uint8_t ly);
void TextWriter(uint8_t ox, uint8_t oy, const char* text);

void ResetSprites();
void SetSprite(int16_t x, int16_t y, uint8_t t, uint8_t f);
void DrawMetaSprite(int16_t x, int16_t y, uint8_t f);
void DrawMetaSpriteString(uint8_t x, uint8_t y, const uint8_t *s);
void DrawMetaTileFrame(uint8_t x, uint8_t y, uint8_t t);

void ShowCredits();
void Intro();
void TitleScreen();
void WinGame();
void LevelIntro();
uint8_t PauseMenu();

void PlayDifferentSong();

static void set_lvl(uint8_t lx, uint8_t ly, uint8_t m);
static uint8_t get_lvl(uint8_t lx, uint8_t ly);
void LoadLevel();
uint8_t UpdateGame();

void DrawVramColumn(uint16_t column, uint8_t lposx, uint8_t lposy);
void DrawVramRow(uint16_t row, uint8_t lposx, uint8_t lposy);
static uint8_t UpdateCamera();




const uint8_t lvl_sprite_string[] PROGMEM = {
FIRST_ALPHA_SPRITE_FRAME+0,FIRST_ALPHA_SPRITE_FRAME+1,FIRST_ALPHA_SPRITE_FRAME+2,FIRST_ALPHA_SPRITE_FRAME+1,FIRST_ALPHA_SPRITE_FRAME+0,0xFF,
};


#define LTO 0
const uint8_t sine_table[] PROGMEM = {
LTO+1,LTO+9,LTO+23,LTO+44,LTO+67,LTO+80,
LTO+80,LTO+55,LTO+33,LTO+15,LTO+4,LTO+0,
};


const char epilogue_string[] PROGMEM =
" -Super\bfluous\b Epilogue-\b\b\n\n"
" Long ago, in a far away\n"
"future there were blocks.\n\b"
"You walked on the blocks,\n\b"
"and when you found rocks,\n\b\b"
"you threw them! \b\bAlso you\n"
"no doubt did more than a\n"
"few jumps as well.\b\b Those\n"
"doors, \b\byou found all the\n"
"doors and walked on them!\b\b\b\b\b\b\n\n"
"Congratulations!\b\b You did\n"
"win the game\b\b-\b\bYes this is\n"
"really the entire ending!\b\b\n\n"
"Thank you for playing,\b\b\b\b\n"
"you are now a Block \b\b\bM\bA\bN\b!!";


const char uzebox_string[] PROGMEM = "UZEBOX";


const char title_sine_table[] PROGMEM = {

3,3,
4,5,7,8,9,11,12,14,
15,17,19,21,23,25,27,30,
32,35,37,40,42,45,48,51,
53,56,59,62,65,68,71,74,
77,81,84,87,90,
90,93,96,99,103,106,109,112,
115,118,121,124,127,129,132,135,
138,140,143,145,148,150,153,155,
157,159,161,163,165,166,168,169,
171,172,173,175,176,177,177,178,
179,179,180,180,180,180,180,180,
180,179,179,178,177,177,176,175,
173,172,171,169,168,166,165,163,
161,159,157,155,153,150,148,145,
143,140,138,135,132,129,127,124,
121,118,115,112,109,106,103,99,
96,
};




void BB_triggerFx(uint8_t patch){//use the 5th channel exclusively to allow music(uses 1-4) to be uninterrupted

	Track* track=&tracks[4];
	tracks[4].flags|=TRACK_FLAGS_PRIORITY;
	track->patchCommandStreamPos = NULL;
	TriggerCommon(track,patch,255,80);
	track->flags|=TRACK_FLAGS_PLAYING;

}




void PlayDifferentSong(){
#ifdef FASTDEBUG
return;
#endif
	if(music_off){
		StopSong();
		return;
	}
	if(current_song == 1){
		StartSong(Song1);
		current_song = 2;
	}else{
		StartSong(Song2);
		current_song = 1;
	}

}




void ResetSprites(){

	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = SCREEN_TILES_H*8;
	sprite_count = 0;

}




static void DrawMetaTile(uint8_t vx, uint8_t vy, uint8_t lx, uint8_t ly){

	uint16_t t = get_lvl(lx,ly);//lvl[(lx>>2)+(ly*SCREEN_TILES_H/2)];//get byte containing 4 map tiles in 2bpp

	if(t == TILE_DIRT)
		t = pgm_read_byte(&lvldata[level_start+(lx+(ly*c.lvl_width))]);//get the actual tile frame, which is not stored specifically in ram
	else if(t == TILE_SKY)
		t = TILE_FRAME_SKY;
	else if(t == TILE_ROCK)
		t = TILE_FRAME_ROCK;
	else if(t == TILE_DOOR)
		t = (p.state == P_WINNING)?TILE_FRAME_DOOR_OPEN:TILE_FRAME_DOOR_CLOSED;

	DrawMetaTileFrame(vx,vy,t);

}




void DrawMetaTileFrame(uint8_t x, uint8_t y, uint8_t f){

	uint16_t toff = f*4;
	SetTile(x+0,y+0,pgm_read_byte(&tile_frame_table[toff++]));
	SetTile(x+1,y+0,pgm_read_byte(&tile_frame_table[toff++]));
	SetTile(x+0,y+1,pgm_read_byte(&tile_frame_table[toff++]));
	SetTile(x+1,y+1,pgm_read_byte(&tile_frame_table[toff]));
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




void DrawMetaSprite(int16_t x, int16_t y, uint8_t f){

	uint8_t foff = f*4;
	SetSprite(x+0,y+0,pgm_read_byte(&sprite_frame_table[foff]),pgm_read_byte(&sprite_mirror_table[foff++]));
	SetSprite(x+8,y+0,pgm_read_byte(&sprite_frame_table[foff]),pgm_read_byte(&sprite_mirror_table[foff++]));
	SetSprite(x+0,y+8,pgm_read_byte(&sprite_frame_table[foff]),pgm_read_byte(&sprite_mirror_table[foff++]));
	SetSprite(x+8,y+8,pgm_read_byte(&sprite_frame_table[foff]),pgm_read_byte(&sprite_mirror_table[foff]));
}




void WinGame(){

	while(GetMasterVolume()){
		if(GetMasterVolume() > 4)
			SetMasterVolume(GetMasterVolume()-4);
		else{
			StopSong();
			break;
		}
		WaitVsync(1);
	}
	FadeOut(9,true);
	SetTileTable(font_tiles);
	ClearVram();
	WaitVsync(45);
	SetMasterVolume(BB_MASTER_VOLUME);
	//TriggerNote(4,24,8,255);//StartSong(main_song);
	FadeIn(2,false);
	TextWriter(1,0,epilogue_string);
	while(1){
		WaitVsync(1);
		if(padState & BTN_START && !(oldPadState & BTN_START))
			break;
	}
	FadeOut(7,true);
}




void LevelIntro(){
#ifdef FASTDEBUG
FadeIn(2,true);
return;
#endif
	FadeIn(2,false);
	ResetSprites();
	uint8_t xpos = 48-16;
	uint8_t soff;
	for(uint8_t i=0;i<8;i++){//"LEVEL XX"
		xpos += 16;
		if(i == 5)//space between level and numbers
			continue;
		soff = 0;
		while(soff < sizeof(sine_table)){
			WaitVsync(1);
			sprite_count = 4*i;
			if(i > 5)
				sprite_count -= 4;//don't lose sprite indices because of space

			uint8_t t0 = pgm_read_byte(&sine_table[soff++]);
			uint8_t t1;
			if(i < 5)
				t1 = pgm_read_byte(&lvl_sprite_string[i]);
			else if(i == 6)//tens
				t1 = FIRST_NUMERAL_SPRITE_FRAME+(p.level/10);
			else//ones
				t1 = FIRST_NUMERAL_SPRITE_FRAME+(p.level%10);

			DrawMetaSprite(xpos,t0,t1);
			WaitVsync(1);
		}
	}
	WaitVsync(80);

}




static void set_lvl(uint8_t lx, uint8_t ly, uint8_t m){

	uint8_t t = lvl[(lx>>2)+(ly*(MAX_LEVEL_WIDTH>>2))];//get byte containing 4, 2 bit entries
	t &= ~(0b00000011	<<((lx%4)<<1));//clear existing bits for our offset
	t |=  (		m	<<((lx%4)<<1));//set new bit values(0-3 value)
	lvl[(lx>>2)+(ly*(MAX_LEVEL_WIDTH>>2))] = t;

}




static uint8_t get_lvl(uint8_t lx, uint8_t ly){

	uint8_t t = lvl[(lx>>2)+(ly*(MAX_LEVEL_WIDTH>>2))];//get byte containing 4, 2 bit entries
	t >>= ((lx%4)<<1);//shift the bits down, 0,2,4,or 6 positions to the normal 0b000000XX range
	t &= 0b00000011;
	return t;

}




uint8_t UpdateGame(){

	ResetSprites();
	uint8_t frame = 0;

	p.ftime++;

	if(p.state != P_WINNING && !(p.state & (P_WALKING|P_FALLING|P_JUMPING)) && get_lvl(p.x>>4,p.y>>4) == TILE_DOOR){
		p.state = P_WINNING;//no facing direction for winning animation
		p.ftime = 0;
		p.frame = 5;
	}
	
	if(p.state != P_WINNING && padState & BTN_SELECT && !(oldPadState & BTN_SELECT)){//player wants to control the camera
		if(c.state)
			c.state = 0;
		else
			c.state = CAM_FREE_MOVE;//player logic can continue on, but new moves cannot happen until the player releases control
	}

	if(p.state == P_WINNING){
		c.state = 0;//force camera to go back to player
		if(p.ftime == 0 && p.frame == 5){//do once only
			StopSong();
			StartSong(levelclear_song);
			DrawMetaSprite(p.x-c.ox,p.y-c.oy,5);
			WaitVsync(16);
			if(p.x >= c.ox && p.x < c.ox+(SCREEN_TILES_H*8) && p.y >= c.oy && p.y < c.oy+(SCREEN_TILES_V*8))
				DrawMetaTile((p.x>>3)&0x1F,(p.y>>3)&0x1F,p.x>>4,p.y>>4);//make door get redrawn as open(internal logic in DrawMetaTile)
			WaitVsync(30);
			ResetSprites();
		}

		if(p.ftime == 9){
			p.ftime = 0;
			p.frame++;
		}

		if(p.frame & 4)
			frame = FRAME_RIGHT_VICTORY;
		else
			frame = FRAME_LEFT_VICTORY;

		if(p.frame > 19){//done gloating
			WaitVsync(80);//player disappears into door

			if(p.level == NUM_LEVELS){//game completed!
				WinGame();
				return 1;
			}else{
				p.level++;
				FadeOut(1,true);
				WaitVsync(60);
			}
			return 2;
		}
	}else if(p.state & P_FALLING){//cannot do anything else until we land
		if(!p.frame){
			p.y++;
			if((p.y&15)>2){
				p.frame = 1;
			}
		}else
			p.y += 2;

		uint8_t t = get_lvl((p.x+8)>>4,(p.y+16)>>4); 
		if(t && t != TILE_DOOR){
			p.state ^= P_FALLING;
			p.frame = 0;
		}
		p.bx = p.x;
		p.by = p.y-16;

		if(p.state & P_FACE_RIGHT)
			frame = p.frame?FRAME_RIGHT_JUMP_FALL:FIRST_RIGHT_FRAME;
		else
			frame = p.frame?FRAME_LEFT_JUMP_FALL:0;

	}else if(p.state & P_LIFTING){//cannot do anything else until we have the rock lifted up
		p.by -= 2;
		if(p.state & P_FACE_RIGHT)
			p.bx -= 2;
		else
			p.bx += 2;

		if(p.by <= p.y-16){
			p.state ^= P_LIFTING;
			p.state |= P_HOLDING;
			p.frame = 0;
			p.ftime = 0;
			p.bx = p.x;
			p.by = p.y-16;
		}else
			p.frame = 1;

		if(p.state & P_FACE_RIGHT)
			frame = p.frame?FRAME_RIGHT_CARRY2:FRAME_RIGHT_CARRY;
		else
			frame = p.frame?FRAME_LEFT_CARRY2:FRAME_LEFT_CARRY;
		if(p.by > p.y-6)
			frame += 3;
		else if(p.by > p.y-12)
			frame += 4;

	}else if(p.state & P_THROWING){//cannot anything until the rock has landed
		if(!p.frame || (p.bx&15)){
			p.frame = FRAME_LEFT_LIFT_HIGH;
			if(p.state & P_FACE_RIGHT)
				p.bx+=2;
			else
				p.bx-=2;

		}else if(p.frame && (!(p.bx&15) || (p.bx&15) > 10)){
			p.by+=2;
			if(get_lvl(p.bx>>4,(p.by+16)>>4)){
				p.state ^= P_THROWING;
				p.frame = 0;
				p.ftime = 0;
				set_lvl(p.bx>>4,p.by>>4,TILE_ROCK);
				if(p.bx >= c.ox && p.bx < c.ox+(SCREEN_TILES_H*8) && p.by >= c.oy && p.by < c.oy+(SCREEN_TILES_V*8))
					DrawMetaTile((p.bx>>3)&0x1F,(p.by>>3)&0x1F,p.bx>>4,p.by>>4);
			}else if(p.ftime > 10){//put arms down
				//p.frame = 2;
			}
		}
		if(p.state & P_FACE_RIGHT)
			frame = p.frame+FIRST_RIGHT_FRAME;
		else
			frame = p.frame;

	}else if(p.state & P_WALKING){//cannot do anything else until we finish stepping to next map tile
		if(p.ftime > 3){
			p.ftime = 0;
			p.frame = !p.frame;
		}
		uint8_t pitx,pity;

		pity = (p.y+16)>>4;
		if(p.state & P_FACE_RIGHT){
			p.x++;
			pitx = (p.x+16)>>4;
		}else{
			p.x--;
			pitx = (p.x-1)>>4;
		}

		if(!(p.x&15)){
			//if(p.state & P_DUCKING){//we were dropping a rock to fit between blocks, we are no longer holding it
			//	p.state &= ~(P_DUCKING|P_HOLDING);
			//}
			p.state ^= P_WALKING;
			p.frame = 0;
			p.ftime = 0;
			if(!get_lvl(p.x>>4,pity))//nothing under our feet?
				p.state |= P_FALLING;
		}else if((p.x&15)>14){//could we be walking off an edge?
			//if(!get_lvl(pitx,pity))//round the edge a bit
			//	p.y++;
		}

		if(p.state & P_DUCKING){//not really ducking...but we are going under something that causes us to lose the rock we were holding
			if((p.by & 15)>6)
				p.by+=1;
			else
				p.by+=2;
			
			if(!(p.by&15)){//rock is done falling
				p.state &= ~(P_DUCKING|P_HOLDING);
				set_lvl(p.bx>>4,p.by>>4,TILE_ROCK);
				if(p.bx >= c.ox && p.bx < c.ox+(SCREEN_TILES_H*8) && p.by >= c.oy && p.by < c.oy+(SCREEN_TILES_V*8))
					DrawMetaTile((p.bx>>3)&0x1F,(p.by>>3)&0x1F,p.bx>>4,p.by>>4);
			}
		}else{
			p.bx = p.x;
			p.by = p.y-16;
		}

		if(p.state & P_FACE_RIGHT)
			frame = (p.state & P_HOLDING)?(p.frame+FRAME_RIGHT_CARRY):(p.frame+FIRST_RIGHT_FRAME);
		else
			frame = (p.state & P_HOLDING)?(p.frame+FRAME_LEFT_CARRY):(p.frame+0);

	}else if(p.state & P_JUMPING){//cannot do anything else until we finish jumping to the next map tile

		if((p.y&15) < 2){
			p.y -= 1;
			if(p.state & P_FACE_RIGHT)
				p.x += 2;
			else
				p.x -= 2;
		}else if((p.y&15))
			p.y -= 2;
		else
			frame = 1;
						
		if((p.x&15)){//start moving horizontally onto the platform?

			if(p.state & P_FACE_RIGHT)
				p.x++;
			else
				p.x--;	
		}


		if(!(p.y&15)){
			p.state ^= P_JUMPING;
			p.state |= P_WALKING;//finish walking onto grid alignment
			//p.frame = 0;
			p.ftime = 250;//roll over
		}
		p.bx = p.x;
		p.by = p.y-16;

		if(p.state & P_FACE_RIGHT)
			frame = p.frame?FIRST_RIGHT_FRAME:FRAME_RIGHT_JUMP_FALL;
		else
			frame = p.frame?0:FRAME_LEFT_JUMP_FALL;
		
	}else{//*CHOOSE A NEW STATE*, see if user has input a valid move

		if(!c.state){//camera under d-pad control? then don't move player

			if(padState & BTN_UP && !(oldPadState & BTN_UP)){//jump?
				uint8_t destx,desty;
				if(p.state&P_FACE_RIGHT)
					destx = (p.x+16)>>4;
				else
					destx = (p.x-1)>>4;
				desty = (p.y-16)>>4;

				uint8_t t0,t1,t2,t3,t4;

				/*	-Left facing example
					t0,t1,
					t2,t3,
					t4,PL
			
					-Right
					t1,t0,
					t3,t2,
					PL,t4
				*/
				if(p.state & P_HOLDING){//have a rock, need extra clearance
					t0 = get_lvl(destx,desty-1);//in front of us, and 2 places up
					t1 = 0;//original behavior, able to "squeeze" through. Level 14 relies on it.//get_lvl(p.x>>4,desty-1);//2 spaces above us
				}else{
					t0 = 0;//no rock, no clearance needed
					t1 = 0;
				}
				t2 = get_lvl(destx,desty);
				t3 = get_lvl(p.x>>4,desty);
				t4 = get_lvl(destx,desty+1);

				if(t4 == TILE_DOOR){t0 = 0;}//can't climb a door!
				if(t2 == TILE_DOOR){t2 = 0;}//a door does not block the player, they need to fall "into" it in some cases to win!

				if(t4 && !(t0|t1|t2|t3)){//anything to jump on, and if so can we fit through?
					BB_triggerFx(SFX_JUMP);
					p.state |= P_JUMPING;
					p.frame = 0;
					p.ftime = 0;
					p.y -= 2;
					p.bx = p.x;
					p.by = p.y-16;
				}else{
				//	p.frame = 1;
				//	p.ftime = 8;
				}

			}else if(padState & BTN_LEFT){
				p.state &= ~P_FACE_RIGHT;
				uint8_t c = get_lvl((p.x-1)>>4,p.y>>4);
				uint8_t c1 = (p.state & P_HOLDING)?get_lvl((p.x-1)>>4,(p.y-16)>>4):0;//updated, player can walk under and it causes him to drop the rock
				if((!c || c == TILE_DOOR)){
					p.x--;
					p.state |= P_WALKING;
					if(c1)//walking under somethingi that will cause us to lose the rock we are holding
						p.state |= P_DUCKING;
					p.frame = 0;
					p.ftime = 0;
				}

			}else if(padState & BTN_RIGHT){
				p.state |= P_FACE_RIGHT;
				uint8_t c = get_lvl((p.x+16)>>4,p.y>>4);
				uint8_t c1 = (p.state & P_HOLDING)?get_lvl((p.x+16)>>4,(p.y-16)>>4):0;

				if((!c || c == TILE_DOOR)){
					p.x++;
					p.state |= P_WALKING;
					if(c1)//walking under something that will cause us to lose the rock we are holding
						p.state |= P_DUCKING;
					p.frame = 0;
					p.ftime = 0;

				}


			}else if(padState & BTN_DOWN && !(oldPadState & BTN_DOWN)){//lift or drop?
				uint8_t destx,desty;
				if(p.state&P_FACE_RIGHT)
					destx = (p.x+16)>>4;
				else
					destx = (p.x-16)>>4;
				desty = p.y>>4;

				uint8_t y0 = get_lvl(destx,desty-1);//an open place in front and above us?(rock travels through this before reaching on top of player)
				uint8_t y1 = get_lvl(destx,desty);//an open place in front of us?
				uint8_t y2 = get_lvl(destx,desty+1);//an open pit in front of us?
				uint8_t y3 = get_lvl(p.x>>4,desty-1);//a place above us to lift a rock over our head to?
			
				if(!(p.state & P_HOLDING)){//not carrying anything
					if(y1 == TILE_ROCK && (!y0) && !(y3)){//can we grab a rock here, is nothing above it?
						BB_triggerFx(SFX_LIFT);
						p.state |= P_LIFTING;
						p.frame = 0;
						p.ftime = 0;
						p.bx = (p.state & P_FACE_RIGHT)?p.x+16:p.x-16;
						p.by = p.y;
						set_lvl(p.bx>>4,p.by>>4,0);
						//DrawMetaTile(p.bx>>3,p.by>>3,p.bx>>4,p.by>>4);
						if(p.bx >= c.ox && p.bx < c.ox+(SCREEN_TILES_H*8) && p.by >= c.oy && p.by < c.oy+(SCREEN_TILES_V*8))
							DrawMetaTile((p.bx>>3)&0x1F,(p.by>>3)&0x1F,p.bx>>4,p.by>>4);
					}
				}else{// if(p.state & P_HOLDING){//drop a rock?
					if(	 !y0 || (!y1 && !y0) || (!y0 && !y1 && !y2)){//some place this can fit?
						BB_triggerFx(SFX_THROW);
						p.state |= P_THROWING;
						p.state ^= P_HOLDING;
						p.ftime = 0;
						p.frame = 0;
					}

				}
			}else{//doing nothing, don't animate
				p.ftime = 0;
				/*
				if(p.ftime > 1)
					p.ftime -= 2;
				else if(p.ftime){
					p.ftime = 0;
					p.frame--;//user tried to do something they couldn't, done with the small step animation for that case
				}
				*/
		
			}

		}//if(!c.state)

		if(p.state & P_FACE_RIGHT)
			frame = (p.state & P_HOLDING)?(p.frame+FRAME_RIGHT_CARRY):(p.frame+FIRST_RIGHT_FRAME);
		else
			frame = (p.state & P_HOLDING)?(p.frame+FRAME_LEFT_CARRY):(p.frame+0);

	}


//	UpdateCamera();
	DrawMetaSprite(p.x-c.ox,p.y-c.oy,frame);

	if(p.state != P_WINNING && (p.state & (P_LIFTING|P_HOLDING|P_THROWING|P_DUCKING)))
		DrawMetaSprite(p.bx-c.ox,p.by-c.oy,FRAME_ROCK);
	UpdateCamera();
	return 0;
}




void DrawMapColumn(uint8_t mx, uint8_t my){

	mx <<= 1;
	my <<= 1;
	for(uint8_t i=0;i<VRAM_TILES_V;i+=2){
		DrawMetaTile(mx&0x1F,my&0x1F,mx>>1,my>>1);
		my += 2;
	}

}




void DrawMapRow(uint8_t mx, uint8_t my){

	mx <<= 1;
	my <<= 1;
	for(uint8_t i=0;i<VRAM_TILES_H;i+=2){
		DrawMetaTile(mx&0x1F,my&0x1F,mx>>1,my>>1);
		mx += 2;
	}
}




void DrawInitialScreen(){//draw from coordinate 0,0

	for(uint8_t i=0;i<VRAM_TILES_H/2;i++)
		DrawMapColumn(i,0);
	
	Screen.scrollX = 0;
	Screen.scrollY = 0;

	//set the initial camera target, the camera handles checking level bounds
	c.ox = c.oy = c.nx = c.ny = 0;

}




static uint8_t UpdateCamera(){

	if(c.state & CAM_FREE_MOVE){//move camera by d-pad

		if((padState & BTN_UP) && c.ny)
			c.ny--;
		else if((padState & BTN_DOWN))// bounds checking is done below    && c.ny < ((c.lvl_width*16)-(SCREEN_TILES_V*8)))
			c.ny++;
		if((padState & BTN_LEFT) && c.nx)
			c.nx--;
		else if((padState & BTN_RIGHT))// bounds checking is done below    && c.nx < ((c.lvl_width*16)-(SCREEN_TILES_H*8)))
			c.nx++;
	}else{//follow player
		if(c.nx < p.x-CAM_PLAYER_CENTERING_X_OFFSET)
			c.nx++;
		else if(c.nx > p.x-CAM_PLAYER_CENTERING_X_OFFSET)
			c.nx--;

		if(c.ny < p.y-CAM_PLAYER_CENTERING_Y_OFFSET)
			c.ny++;
		else if(c.ny > p.y-CAM_PLAYER_CENTERING_Y_OFFSET)
			c.ny--;

	}

	//force the camera to stay within the level limits
	if(c.nx > (c.lvl_width*16)-(SCREEN_TILES_H*8))
		c.nx = (c.lvl_width*16)-(SCREEN_TILES_H*8);
	if(c.nx < 0)
		c.nx = 0;

	if(c.ny > (c.lvl_height*16)-(SCREEN_TILES_V*8))
		c.ny = (c.lvl_height*16)-(SCREEN_TILES_V*8);
	if(c.ny < 0)
		c.ny = 0;


//	if(c.ox == c.nx && c.oy == c.ny)//already there, nothing to do
	//	return 0;
	
	Screen.scrollX = (uint8_t)c.ox;
	Screen.scrollY = (uint8_t)c.oy;

	if(c.ox < c.nx){
		c.ox++;

	}else if(c.ox > c.nx){
		c.ox--;

	}

	if(c.oy < c.ny){
		c.oy++;

	}else if(c.oy > c.ny){
		c.oy--;

	}


	DrawMapColumn(c.ox>>4,c.oy>>4);
	DrawMapColumn((c.ox>>4)+14,c.oy>>4);
	DrawMapRow(c.ox>>4,c.oy>>4);
	DrawMapRow(c.ox>>4,(c.oy>>4)+14);

	return 1;

}




void LoadLevel(){

	ResetSprites();
	PlayDifferentSong();//StartSong(main_song);
	p.x = p.y = 32;
	p.state = p.frame = p.ftime = 0;

	level_start = 0;

	for(uint8_t i=1;i<p.level;i++)//seek to the start of current level(level numbers start at 1 not 0)
		while(pgm_read_byte(&lvldata[level_start++]) != 255);

	c.lvl_width = pgm_read_byte(&lvldata[level_start++]);
	c.lvl_height = pgm_read_byte(&lvldata[level_start++]);

	uint16_t loff = level_start;

	for(uint8_t y=0;y<c.lvl_height;y++){
		for(uint8_t x=0;x<c.lvl_width;x++){
			uint8_t t = pgm_read_byte(&lvldata[loff++]);
			if(t == PS){
				p.x = x*16;
				p.y = y*16;
				t = 0;
			}else if(t > PS)//dirt of some type
				t = TILE_DIRT;

			set_lvl(x,y,t);
		}
	}

}




void TextWriter(uint8_t ox, uint8_t oy, const char* text){

	ClearVram();
	uint8_t x = ox;
	uint8_t y = oy;
	char c;

	while(1){
		c = pgm_read_byte(text);
		if(c == 0)
			break;
		if(c == '\n'){
			x = ox;
			y++;
		}else if(c == '\b'){//a short pause for effect
			WaitVsync(10);
		}else{
			SetFont(x,y,c-32);//((c&127)-32));
			x++;
		}
		text++;
		WaitVsync(4);
	}

}




void ShowCredits(){

	SetTileTable(font_tiles);
	ClearVram();
	FadeOut(1,true);
	Print(1,4,PSTR("Source Code:"));
	Print(6,6,PSTR("GPL 3.0"));
	Print(6,8,PSTR("Lee Weber 2017"));

	Print(1,11,PSTR("GFX,SFX,Music:"));
	Print(6,13,PSTR("CC Share-Alike 4"));
	Print(6,15,PSTR("Lee Weber 2017"));

	Print(1,19,PSTR("TI-83 Game, Levels"));
	Print(6,21,PSTR("Brandon Sterner"));

	WaitVsync(10);
	FadeIn(2,true);
	WaitVsync(240);
	FadeOut(3,true);

}




void Intro(){
#ifdef FASTDEBUG
return;
#endif
	SetTileTable(font_tiles);
	ClearVram();

	for(uint8_t i=0;i<6;i++){
		sprites[i].tileIndex = pgm_read_byte(&uzebox_string[i])-32;
		sprites[i].flags = SPRITE_BANK2;
	}

	for(uint8_t i=0;i<180;i++){
		for(uint8_t j=0;j<6;j++){
			sprites[j].x = (j*8)+88;
			if(i+(j*10) >= sizeof(title_sine_table))
				sprites[j].y = 96;
			else
				sprites[j].y = pgm_read_byte(&title_sine_table[i+(j*10)]);
		}
		WaitVsync(1);
	}
	FadeOut(2,true);
	ResetSprites();
	ClearVram();
	DrawMap2(10,8,da_logo_map);

	TriggerFx(36,255,0);
	BB_triggerFx(37);
	FadeIn(8,true);
	WaitVsync(70);
	FadeOut(5,true);
	StopSong();
	ShowCredits();

	WaitVsync(60);

}




void DrawMetaSpriteString(uint8_t x, uint8_t y, const uint8_t *s){

	while(1){
		uint8_t t = pgm_read_byte(s++);
		if(t == 0xFF)
			break;
		DrawMetaSprite(x,y,t);
		x += 16;
	}

}




void TitleScreen(){

	Screen.scrollX = 0;
	Screen.scrollY = 0;
#ifdef FASTDEBUG
p.level = TESTLEVEL;
return;
#endif
	c.state = 0;
	PlayDifferentSong();

	FadeOut(0,true);
	ClearVram();
	SetTileTable((const char *)title_tiles);
	DrawMap2(0,2,(const char*)title_map);
	WaitVsync(1);
	FadeIn(2,false);

	while(1){
		WaitVsync(1);
		ResetSprites();
		DrawMetaSpriteString(40,144,lvl_sprite_string);
		DrawMetaSprite(144,144,FIRST_NUMERAL_SPRITE_FRAME+(p.level/10));
		DrawMetaSprite(160,144,FIRST_NUMERAL_SPRITE_FRAME+(p.level%10));

		if(padState & BTN_LEFT && !(oldPadState & BTN_LEFT)){
			if(--p.level == 0)
				p.level = NUM_LEVELS;
		}else if(padState & BTN_RIGHT && !(oldPadState & BTN_RIGHT)){
			if(++p.level > NUM_LEVELS)
				p.level = 1;
		}else if(padState & BTN_START && !(oldPadState & BTN_START)){
			FadeOut(2,true);
			break;
		}
	}

}




void VsyncHook(){

	oldPadState = padState;
	padState = ReadJoypad(0);

}




uint8_t PauseMenu(){

//	StopSong();
	ResetSprites();
	uint8_t cursorpos = 0;
	uint8_t moff = 0;

	WaitVsync(1);
	int8_t xoff,yoff;
	if((Screen.scrollX&7)>3)
		xoff = 8-(Screen.scrollX&7);
	else
		xoff = -(Screen.scrollX&7);
	if((Screen.scrollY&7)>3)
		yoff = 8-(Screen.scrollY&7);
	else
		yoff = -(Screen.scrollY&7);
	
	for(uint8_t i=0;i<6;i++){//draw map
		for(uint8_t j=0;j<4;j++){
			SetSprite((96+(j*8))+xoff,(96+(i*8))+yoff,pgm_read_byte(&pause_map[moff++]),SPRITE_BANK1);//(((SCREEN_TILES_H/2)-2)<<3)+(j*8),((SCREEN_TILES_V<<3)-(5*8))+(i*8),pgm_read_byte(&pause_map[moff++]),SPRITE_BANK1);
		}
	}
	SetSprite((96+2)+xoff,(96+8)+yoff,0,SPRITE_BANK1);//(((SCREEN_TILES_H/2)-2)<<3)+2,((SCREEN_TILES_V<<3)-(5*8))+8,0,SPRITE_BANK1);//draw cursor

	while(1){
		WaitVsync(1);
		if(padState & BTN_START && !(oldPadState & BTN_START)){
			if(cursorpos == 0){//resume
				//ResumeSong();
				return 0;
			}else if(cursorpos == 1)//retry
				return 2;
			else if(cursorpos == 3)//quit
				return 1;
		}else if(padState & BTN_UP && !(oldPadState & BTN_UP)){
			if(--cursorpos > 3)//rollover
				cursorpos = 3;
			BB_triggerFx(SFX_MENU);
		}else if(padState & BTN_DOWN && !(oldPadState & BTN_DOWN)){
			if(++cursorpos > 3)
				cursorpos = 0;
			BB_triggerFx(SFX_MENU);
		}else if(((padState & BTN_LEFT) && !(oldPadState & BTN_LEFT)) || ((padState & BTN_RIGHT) && !(oldPadState & BTN_RIGHT))){
			if(!music_off)
				music_off = 1;
			else
				music_off = 0;
			
			PlayDifferentSong();
		}
		sprites[sprite_count-1].y = sprites[0].y+((cursorpos+1)*8);//move the cursor sprite

	}

	return 0;

}




int main(){
	
	SetUserPostVsyncCallback(&VsyncHook);
	InitMusicPlayer(patches);
	SetSpritesTileBank(0,bb_sprites);
	SetSpritesTileBank(1,pause_tiles);
	SetSpritesTileBank(2,font_tiles);
	Screen.scrollHeight = 32;

	p.level = 1;
	current_song = 1;

	SetMasterVolume(255);
	Intro();
	SetMasterVolume(BB_MASTER_VOLUME);
	
TITLE_TOP:
	TitleScreen();
	SetTileTable(game_tiles);
GAME_TOP:

	LoadLevel();
	DrawInitialScreen();
	LevelIntro();
	
	while(1){
		WaitVsync(1);

		uint8_t r;
		r = UpdateGame();
		if(r == 1)//won the game
			goto TITLE_TOP;
		else if(r == 2)//beat the level
			goto GAME_TOP;

		if(padState & BTN_START && !(oldPadState&BTN_START)){
			r = PauseMenu();
			if(r == 1)//user quit?
				goto TITLE_TOP;
			else if(r == 2)//user restarted level?
				goto GAME_TOP;
		}
	}		
	
}