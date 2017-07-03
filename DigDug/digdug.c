#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>

#include "data/graphics/graphics.inc"
#include "data/graphics/maps/maps.inc"
#include "data/sound/sound.inc"
#include "data/roundmaps.inc"
#include "dd_defines.h"
#include <sdBase.h>

extern uint8_t ram_tiles[];
extern bool IsUartRxBufferEmpty();
extern bool playSong;
extern void TriggerCommon(Track* track,u8 patch,u8 volume,u8 note);


const char sd_format_string[] PROGMEM = {
'D','I','G','D','U','G','_','F','O','R','M','A','T','T','E','D',
};

#define SD_CMD_GO_IDLE_STATE        ( 0 | 0x40)
#define SD_CMD_SEND_OP_COND         ( 1 | 0x40)
#define SD_CMD_APP_SEND_OP_COND     ( 1 | 0x40)
#define SD_CMD_SEND_IF_COND         ( 8 | 0x40)
#define SD_CMD_SEND_CSD             ( 9 | 0x40)
#define SD_CMD_SEND_CID             (10 | 0x40)
#define SD_CMD_STOPTRANSMISSION     (12 | 0x40)
#define SD_CMD_SET_BLOCKLEN         (16 | 0x40)
#define SD_CMD_READ_SINGLE_BLOCK    (17 | 0x40)
#define SD_CMD_READ_MULTIPLE_BLOCK  (18 | 0x40)
#define SD_CMD_WRITE_SINGLE_BLOCK   (24 | 0x40)
#define SD_CMD_WRITE_MULTIPLE_BLOCK (25 | 0x40)
#define SD_CMD_APP_CMD              (55 | 0x40)
#define SD_CMD_READ_OCR             (58 | 0x40)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************PRIMARY LOGIC FUNCTIONS****************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SDSetup(){/*
	ResetSprites();
	sdCardInitNoBuffer();//using CunningFellow's simpleSD!
	uint32_t sectorStart = sdCardFindFileFirstSectorFlash(PSTR("_HISCOREDAT"));
	sdCardCueSectorAddress(sectorStart+0x11);//goto first byte of Dig Dug slot in shared high score file
	for(uint8_t i=0;i<sizeof(sd_format_string);i++){
		if(sdCardGetChar() != pgm_read_byte(&sd_format_string[i])){
			sd_state = 0;
			DDCrash();
			return;
		}
	}

	sdCardStopTransmission();
	sdCardCueSectorAddress(sectorStart+0x11);//goto first byte of Dig Dug slot in shared high score file
	for(uint8_t i=0;i<sizeof(sd_format_string);i++){
		if(sdCardGetChar() != pgm_read_byte(&sd_format_string[i])){
			sd_state = 0;
			DDCrash();
			return;
		}
	}

//	sdCardStopTransmission();
//	for(uint16_t i=0;i<10000;i++)
//	sdCardGetByte();//eat a stuff byte?
//	sectorStart >>= 9;
//	WaitVsync(1);
//	sdCardSendCommand(SD_CMD_WRITE_SINGLE_BLOCK,(sectorStart>>9UL),0);//(sectorStart&0xFFFF));
//	for(uint16_t i=0;i<514;i++){
//		sdCardSendByte(i);
//	}
//	sdCardSendCommand(SD_CMD_STOPTRANSMISSION,0,0);
 //   sdCardDirectRead(&vram[COORD(0,2)], 13, 39, 1);   // VERTICAL TEXT
  //  sdCardSkipBytes(1);
  */ 
}



int main(){

p_lives = 2;
sd_state = 0;
//40,then start scrolling 1 pixel per frame for 208 frames
//	SetRenderingParameters();//knock off last 2 rows that aren't really that useful...to blit more ram tiles?
	SetUserPostVsyncCallback(&VsyncRoutine);
	SetTileTable(game_tiles);//only required when title is disabled, eliminate when done
	SetPalette(game_palette, sizeof(game_palette));
	SetSpritesTileTable(sprite_tiles);
	SetMasterVolume(DIGDUG_MASTER_VOLUME);
	InitMusicPlayer(patches);
	SDSetup();


	net_state = NET_FIRST_TICK;//start network state machine to use wasted cycles until vsync(see DDWaitVsync())
	UzeboxLogo();

/*while(1){
WaitVsync(100);
DDTriggerFx(SFX_PLAYER_DYING1);
}
*/
MAIN_TOP:
//ShowHighScoreScreen(0);
	TitleScreen();//sets up attract mode, if no selection made
	GameLoop();//normal play, or attract mode happens, depending on action taken in TitleScree()
//	HighScoreEntryScreen();//will pass through if player did not get a high score
//	ShowHighScoreScreen(0);//always show high scores after a game or demo
	goto MAIN_TOP;
	
	return 0;
}

void GameOver(){
	ResetSprites();
	DDWaitVsync(8);
	for(uint8_t y=0;y<2;y++){//original origin(adjusted for our shorter screen height) is (64,85), we change this to (64,88) to lower ram tile usage significantly
		for(uint8_t x=0;x<8;x++){
			uint8_t t = pgm_read_byte(&game_over_text_sprite_map[(y*8)+x]);
			if(t == 255)
				continue;

			SetSprite(64+(x<<3),88+((y<<2)<<3),t,0);//24 pixels in between top and bottom text line, per original
		}
	}
	//finish dying animation like normal, then 8 frames level drawn without any sprites, then "player 1\ngame over" for 120 ticks
	//then just "game over" for 89? ticks, then ? ticks and start title screen over
	StartSong(MUS_GAME_OVER);
	DDWaitVsync(120);
	for(uint8_t i=0;i<7;i++)//hide sprites for "PLAYER X", leaving just game over
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;
	DDWaitVsync(89);
}



void InitPlayer(){
	p_lives = DEFAULT_STARTING_LIVES;
	p_state = 0;
}

void GameLoop(){

	uint8_t r,rr;
	InitPlayer();
GAME_TOP:
	StartRound();
	StopSong();
	StartSong(MUS_WALKING_LOOP);
	StopSong();
GAME_LOOP:
	DDWaitVsync(1);
	if((padState & BTN_START) && !(oldPadState & BTN_START) && !(gameState & GAME_DEMO_PLAYING) && !(p_state & PLAYER_DYING))
		Pause();
	ResetSprites();


	if((gameState & GAME_SONG_NO_INTERRUPT) && !playSong)//!IsSongPlaying()//TODO GET NEW KERNEL VERSION
		gameState ^= GAME_SONG_NO_INTERRUPT;
	r = 	UpdatePlayer(0);
	UpdateEnemies(0);
	rr =	UpdateRocks(0);//returns > 0 if a rock is falling or crumbling
	if(p_state & PLAYER_UPDATE_SCOREBOARD){
		RedrawScore();
		p_state ^= PLAYER_UPDATE_SCOREBOARD;
	}
	global_frame++;
	if(rr)//round cannot end in victory or defeat until a falling/crumbling rock is done. Agitated(not yet falling) rocks do not count
		goto GAME_LOOP;
	if(r == 1){//player died and is done with animation
		DDWaitVsync(2);//in original, there are 2 frames where the player is not drawn before black screen
		if(p_lives == 0){//game over
			GameOver();
			return;
		}else
			p_lives--;
		DDRD = 0;//black screen
		PushLevelState();//save any changes to the map the player made(tunnels,dead enemies/rocks)
		//fall through to reload round
	}else if(r == 2){//player killed all enemies
		DDWaitVsync(4);
		StartSong(MUS_ROUND_CLEAR);
		DDWaitVsync(145);//per original, 145(or 149?? +4 before sound?!?) ticks happen after last enemy is gone(before 2 black frames..etc)
		p_round++;
		p_state &= ~PLAYER_DIED_THIS_ROUND;
		//fall through to new round
	}else//round continues
		goto GAME_LOOP;

	goto GAME_TOP;//round is done

}



void StartRound(){

	//1 black frame after death and before showing level
	//1 frame of level before showing "PLAYER X READY"
	gameState &= ~(GAME_FIRST_TICK_REACHED);
	p_state &= ~(PLAYER_DYING);
	p_frame = 0;
	p_last_dir = PLAYER_RIGHT;
	p_harpoon_off = 255;

//TODO CHECK GAME_CHEAT_USED AND GIVE 30 LIVES IF SO
	ResetSprites();
	NullEnemiesAndRocks();
	ClearVram();
	DDWaitVsync(1);//1 frame of black is shown first, in the original
	InitLevel();
	FadeIn(0,false);
	DDWaitVsync(1);//level is shown without intro text for 1 frame in original
	p_state |= PLAYER_UPDATE_SCOREBOARD;
	p_x = PLAYER_DEFAULT_START_X;//normal position in the center of the map
	p_y = PLAYER_DEFAULT_START_Y;

	bool first_run = false;
	if(!(gameState & GAME_ATTRACT_MODE)){

		if(!(p_state & PLAYER_P1_INTRO_DONE)){//first play for player 1, start from top right corner of playfield
			p_state |= PLAYER_P1_INTRO_DONE;
			first_run = true;
		}

		if(first_run){
			p_x = PLAYER_INTRO_START_X;
			p_y = PLAYER_INTRO_START_Y;
			StartSong(MUS_ROUND_INTRO);//run the intro song
		//	gameState |= GAME_DEMO_PLAYING;//take over player input to dig to center
	//	StartDemo(1);
		}

		UpdatePlayer(1);//just needs to be drawn, will move if demo playing, then stand there until first game tick
		UpdateEnemies(1);//also just need to be drawn, wont move
		UpdateRocks(1);//just draw any that are moving??TODO MAKE SURE THIS IS RIGHT		

		for(uint8_t y=0;y<2;y++){//original origin(adjusted for our shorter screen height) is (64,85), we change this to (64,88) to lower ram tile usage significantly
			for(uint8_t x=0;x<8;x++){
				uint8_t t = pgm_read_byte(&round_start_text_sprite_map[(y*8)+x]);
				if(t == 255)
					continue;

				SetSprite(64+(x<<3),88+((y<<2)<<3),t,0);//24 pixels in between top and bottom text line, per original
			}
		}
	}
	
//if walking out demo for first run, text stays for 85 frames after the player is done moving
	uint8_t count = (first_run)?132:85;//132 frames if already done intro, otherwise 85 frames after demo(digging to center) is done
	while(count){//per original, string is displayed for 132 frames before disappearing and playing beginning
		if(true){//!(gameState & GAME_DEMO_PLAYING)){
			if(count)
				count--;
			else
				break;
		}
		//padState = oldPadState = 0;//override any input the user might be giving(which would otherwise run in UpdatePlayer()
		DDWaitVsync(1);
	}

	ResetSprites();
	gameState |= GAME_FIRST_TICK_REACHED;//let monsters know they can update and control is returned to user

}


#define FIRST_DIRT_TILE		5
void InitLevel(){

	DDRD = 0;
//	uint8_t tile_palette = (p_round[pActive]>>2)%3;
	SetTileTable(game_tiles);
	SetFontTilesIndex(128);
	ClearVram();


	for(uint8_t i=0;i<VRAM_TILES_H*2;i++)//top black
		vram[i] = RAM_TILES_COUNT;

	for(uint8_t y=MAP_Y_OFF;y<(MAP_HEIGHT*2)+MAP_Y_OFF;y++){
		for(uint8_t x=MAP_X_OFF;x<(MAP_WIDTH*2)+MAP_X_OFF;x++){
			uint8_t t;
			if(y < MAP_Y_OFF+6)			{t = FIRST_DIRT_TILE+(0*17);}//first layer
			else if(y < MAP_Y_OFF+12)	{t = FIRST_DIRT_TILE+(1*17);}//second
			else if(y < MAP_Y_OFF+18)	{t = FIRST_DIRT_TILE+(2*17);}//etc
			else {t = 56;}
			SetTile(x,y,t);
		}
	}

	uint8_t effective_level;
	if(p_round < 12)//the first 12 levels run sequentially and are all unique
		effective_level = p_round;
	else//levels 13...16 get repeated for 17...20, 21...24, etc. Only the colors change(and enemies get faster of course)
		effective_level = 12+(p_round%4);

//	for(uint8_t 
	//TODO pallete = effective_level

	CarveTunnel(64,0,0b001100);//carve out center tunnel for player starting place

//effective_level = 0;
	uint16_t moff = effective_level*MAP_STORED_SIZE;
	for(uint8_t i=0;i<7;i++)
		CarveTunnel(pgm_read_byte(&map_data[moff++]),pgm_read_byte(&map_data[moff++]),pgm_read_byte(&map_data[moff++]));
	for(uint8_t i=0;i<5;i++){

		uint8_t x = pgm_read_byte(&map_data[moff++]);
		uint8_t y = (x/12)<<1;
		x %= 12;
		x <<= 1;
y += 2;
		uint8_t offset = 0;
		if(y > 19)
			offset = 3*17;
		else if(y > 13)
			offset = 2*17;
		else if(y > 7)
			offset = 1*17;

		DrawMetaTileMapOffset(x,y,8,offset);
	}
	if(true){//this is not the first level, so Dig Dug starts in the center, with the tunnel pre-digged dug
		for(uint8_t i=2;i<12;i++){
			SetTile(10,i,(i>7?25:8));
			SetTile(11,i,(i>7?26:9));
		}
	}


	DrawSidePanel();
	DrawFlowers();

	if(p_state & PLAYER_DIED_THIS_ROUND)//player died on this round previously, restore anything he might have changed(tunnels,dead enemies,etc)
		PopLevelState();
	DDRD = 255;

}



void DrawHarpoon(){

	if(p_harpoon_off == 255 || p_harpoon_off == 0 || (p_state & PLAYER_DYING))
		return;

	uint8_t hx,hy;
	if(p_state & PLAYER_LEFT){
		hx = p_x-p_harpoon_off;
		hy = p_y+8;
	}else if(p_state & PLAYER_RIGHT){
		hx = p_x+16+p_harpoon_off;
		hy = p_y+8;
	}else if(p_state & PLAYER_UP){
		hx = (p_state & PLAYER_FEET_RIGHT)?p_x+8:p_x;
		hy = p_y-p_harpoon_off;
	}else{//down
		hx = (p_state & PLAYER_FEET_RIGHT)?p_x+8:p_x;
		hy = p_y+16+p_harpoon_off;
	}

	uint8_t segs = p_harpoon_off>>3;
	bool flop = false;

	if(p_state & PLAYER_DOWN){
		SetSprite(hx,hy,FIRST_HARPOON_SPRITE+3,SPRITE_FLIP_Y);//draw the tip
		for(uint8_t i=0;i<segs;i++){
			SetSprite(hx,hy-8-(i<<3),FIRST_HARPOON_SPRITE+4+flop,SPRITE_FLIP_Y);
			flop = !flop;
		}
	}else if(p_state & PLAYER_UP){
		SetSprite(hx,hy,FIRST_HARPOON_SPRITE+3,0);//draw the tip
		for(uint8_t i=0;i<segs;i++){
			SetSprite(hx,hy+8+(i<<3),FIRST_HARPOON_SPRITE+4+flop,0);
			flop = !flop;
		}
	}else if(p_state & PLAYER_RIGHT){
		SetSprite(hx,hy,FIRST_HARPOON_SPRITE+0,SPRITE_FLIP_X);//draw the tip
		for(uint8_t i=0;i<segs;i++){
			SetSprite(hx-8-(i<<3),hy,FIRST_HARPOON_SPRITE+1+flop,SPRITE_FLIP_X);
			flop = !flop;
		}
	}else{//left
		SetSprite(hx,hy,FIRST_HARPOON_SPRITE+0,0);//draw the tip
		for(uint8_t i=0;i<segs;i++){
			SetSprite(hx+8+(i<<3),hy,FIRST_HARPOON_SPRITE+1+flop,0);
			flop = !flop;
		}
	}

}

bool IsRock(uint8_t t){
	return false;
}

bool IsDiggable(uint8_t t){
	return false;
}


const uint8_t horizontal_harpoon_collide[] PROGMEM = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint8_t vertical_harpoon_collide[] PROGMEM = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
inline bool BlocksHorizontalHarpoon(uint8_t t){
	uint8_t c = pgm_read_byte(&horizontal_harpoon_collide[t>>3]);
	c <<= (t&7);
	return (c&128);
}

inline bool BlocksVerticalHarpoon(uint8_t t){
	uint8_t c = pgm_read_byte(&vertical_harpoon_collide[t>>3]);
	c <<= (t&7);
	return (c&128);
}

bool IsDirt(uint8_t t){
return false;
}

bool IsLeftDivet(uint8_t t){
	return false;
}

bool IsRightDivet(uint8_t t){
	return false;
}

bool IsUpDivet(uint8_t t){
	return false;
}

bool IsDownDivet(uint8_t t){
	return false;
}

bool IsHDirtTunnel(uint8_t t){
	return false;
}

bool IsVDirtTunnel(uint8_t t){
	return false;
}
uint8_t UpdatePlayer(bool draw_only){

//BUG IN ORIGINAL GAME, IF YOU BEAT A LEVEL IT PLAYS THE MUSIC AND PROCEEDS. IF YOU ARE KILLED BY A ROCK AT THE SAME TIEM WITH THE LAST ENEMY,
//IT PLAYS THE NORMAL PLAYER DEATH, THEN MAKES ROCKS VANISH, AND PLAYS THE ROUND CLEAR MUSIC. START AGAIN ON THE NEXT ROUND

//DIGGING-looks 1 pixel past the edge of frame in the direction player is moving, then divides by 8 to see if any dirt tile is there.
//digging continues until next 8 pixel boundary is reached, even if just a sliver of dirt

//PLAYER ONLY NEEDS TO OVERLAP 1 8x8 tile of the bonus, not the whole thing

//there is always a 1 tick delay before movement or direction change starts

//enemies are stunned for 47-51 ticks?!?! 49 seems most common?!
//player pushes attack button, displays 2 frames of normal
//then displays 1 frame of attack frame. After 1 attack frame the harpoon travels at 2 pixels/tick starting next tick
//if it hits an enemy, he goes to stunned/first infalted frame. If not pushing another button, player goes to pushed down
//pumping state the frame after the enemy goes stunned
//if has enemy grabbed and holding button, pushing b again has 2 frames delay still in down position, then moves up to up position which lasts 10 frame,

//if stunned enemy recovers and player was in pumping frame, player stays in pumping frame until next tick
//first death frame lasts 74 ticks in original, each frame after lasts 16 ticks



//per 7800 version "Don't slow down for hourglass or bowtie" dirt fragments


	if(draw_only){//this is only for the initial level intro(after the digging intro), just need to draw the player facing right
		DrawMetaSprite(p_x,p_y,FIRST_PLAYER_RIGHT_FRAME);
		return 0;
	}
	DrawBonus();//bonus is done with sprites now(unique tile limits for flash tiles), and must appear under player

	uint8_t frame_base = 0;
	uint8_t enemies_alive = 0;

	if(p_state & PLAYER_UP){
		if((p_state & PLAYER_FEET_RIGHT))
			frame_base = NUM_FRAMES_PER_PLAYER_DIRECTION_STATE*0;
		else
			frame_base = NUM_FRAMES_PER_PLAYER_DIRECTION_STATE*1;
	}else if((p_state & PLAYER_DOWN)){
		if((p_state & PLAYER_FEET_RIGHT))
			frame_base = (NUM_FRAMES_PER_PLAYER_DIRECTION_STATE*2);
		else
			frame_base = (NUM_FRAMES_PER_PLAYER_DIRECTION_STATE*3);
	}else if((p_state & PLAYER_LEFT))
		frame_base = (NUM_FRAMES_PER_PLAYER_DIRECTION_STATE*4);
	else
		frame_base = (NUM_FRAMES_PER_PLAYER_DIRECTION_STATE*5);



	if(p_state & PLAYER_DYING){
		p_idle = 0;
		frame_base += FIRST_PLAYER_DEATH_FRAME;
		if(!(--p_ftime)){//first frame is 74 ticks(+1 tick of normal frame before showing a death frame) long in original, which is assigned when state is set to dying
			p_frame++;
			p_ftime = 16;//all frames after are 16
		}else{
			if(p_ftime == 16){//must be frame 0
				StopSong();
				DDTriggerFx(SFX_PLAYER_DYING1);
			}
		}
		
		if(p_frame >= NUM_PLAYER_DEATH_FRAMES)//done dying, let game logic start a new round
			return 1;//original game has 2 frames displayed without the last "star" frame shown, this is handled in game logic as DDWaitVsync(2) after this
		goto PLAYER_UPDATE_BOTTOM;
	//	return 0;
	}
	//check for enemy collisions, in original the first death frame is not displayed until the next frame after the hit
	bool did_die = false;
	//	bool was_rock = false;		
		//check for colliding with flame


		//check for colliding with enemies
	for(uint8_t i=0;i<MAX_ENEMIES;i++){
		if(e_state[i] == ENEMY_DISABLED)
			continue;
		enemies_alive++;

		if((p_state & PLAYER_DYING))//just counting, no collision
			continue;

		if(	(e_attrb[i] & 0b11) || ((e_state[i] & (ENEMY_POPPED)) == (ENEMY_POPPED)) || ((e_state[i] & (ENEMY_SQUISHED)) == (ENEMY_SQUISHED)) ||
			((e_state[i] & (ENEMY_SHOW_SCORE)) == (ENEMY_SHOW_SCORE)) || (e_state[i]&ENEMY_GHOST))
			continue;//these states can't hurt the player by touching
		if(	e_x[i] <= p_x+12 && e_x[i]+12 >= p_x &&//lines up horizontally
			e_y[i] <= p_y+12 && e_y[i]+12 >= p_y){//vertically too:collided
			did_die = true;
			break;
		}
	}

		//TODO check for colliding with rocks, which overrides anything else(for squished state)

	if(did_die){
		p_state |= PLAYER_DYING;
		p_ftime = 74;
		p_frame = 0;
		//TODO HOW TO SIGNIFY FOR NEXT TICK WHAT TYPE OF DEATH?
		DDStopSong();
		DDTriggerFx(SFX_PLAYER_HIT1);//automatically handles triggering 2nd part of sound effect
	}

	
	if(!enemies_alive)//did we win the round? return to game loop to continue on
		return 2;

	if(enemies_alive == 1)
		p_hurry = 0;
	else if(p_hurry)
		p_hurry--;

	if(p_hurry == 0 && !(gameState & GAME_HURRY_UP)){
		gameState |= GAME_HURRY_UP;
	//	StartSong(MUS_HURRY_UP);
	}

//inclusive of input frame, the entire harpoon process takes 23 ticks before it disappears
//player stays in attack frame for 10 ticks after the harpoon has disappeared if it traveled the whole way, otherwise only 9
//from button press to being able to attack again takes 3+23+10=46 ticks

//pumping starts tick after the enemy is hit, then spends 4 frames in the lowered position(at least when button held?)
//then(when button held, at least) 16 frames in the up position, then 16 down(first tick of this down inflates the enemy)
//then 16 frames up then 16 frames down in the same way until the enemy pops
//the player immediately leaves pumping state the same tick the enemy pops
//if button is not held, player does not advance to up or down until it is held again, counter stops if not held?



	int16_t hx = 255;
	int16_t hy = 255;

	//move harpoon
	//.....

	if(!(p_state & PLAYER_DYING)){//did not get killed above
		//possible actions: idle,walking,digging,attacking,or pumping
			uint8_t do_walk,do_dig;
			do_walk	= pgm_read_byte(&p_walk_table[(p_fine>>3)&(sizeof(p_walk_table)-1)]);//will we move 1 pixel this frame or not(3/5 of the time, yes)
			do_dig	= pgm_read_byte(&p_dig_table[(p_fine>>3)&(sizeof(p_dig_table)-1)]);//will we move 1 pixel this frame or not(3/5 of the time, yes)
			uint8_t b = 128>>(p_fine&7);
			do_walk	= do_walk&b;
			do_dig	= do_dig&b;
			p_fine++;


		if(padState & (DIRECTIONAL_BTN) && !(p_state & PLAYER_ATTACKING)){
			ResumeSong();
			p_idle = 0;
		}else{
			DDStopSong();
			p_fine = 0;//we always want to move, the first tick the player walks. make sure we are pointing to the start of the table
			if(p_idle < 255)
				p_idle++;
		}

		if(p_state & PLAYER_PUMPING){//overrides all other states, cannot leave until harpoon_off == 255
			
			if(p_harpoon_target != 255 && !(e_attrb[p_harpoon_target] & 0b11)){//player waited too long(not holding pump button)
				p_harpoon_off = 255;
				p_state ^= PLAYER_PUMPING;
				p_frame = 0;
				p_ftime = 0;
			}else if(p_harpoon_off == 255){//

			}
			//leave harpoon offset alone until we are done, keep drawing it//TODO TODO
			if(padState & (BTN_B|BTN_Y)){//continue pumping, player must let go and change direction to leave this state(or wait for enemy to deflate)
				if(!p_ftime){//change to other frame
					p_frame = !p_frame;
					if(p_frame){//down stage
						if(++e_frame[p_harpoon_target] > 3){//in original, player recovers instantly when enemy pops
							e_state[p_harpoon_target] = (ENEMY_POPPED);//TODO FIX THIS UP
							p_harpoon_target = 255;
							p_harpoon_off = 255;
							p_state ^= PLAYER_ATTACKING;
//							uint8_t dirt_level = 1;
							DDTriggerFx(SFX_POP);
//							e_restore |= 1<<p_harpoon_target;
//TODO MAKE KillEnemy()?
							//AddScore(100*dirt_level);
							//frame_base =
						}else
							DDTriggerFx(SFX_PUMP);
							
					}//p_frame
				}//p_ftime
			}else{//else pumping does not clock forward if the button is not held(it is faster to press and release while walking towards the enemy anyway...)
				//if player lets go of the button and pushes a direction, there is 1 frame still in pumping state, then leave pump and start moving
				if(padState & (BTN_UP|BTN_DOWN|BTN_LEFT|BTN_RIGHT)){//if not holding action button, and pushing a movement button, leave pump frame NEXT tick
					p_state ^= PLAYER_PUMPING;
					p_harpoon_off = 255;
					//frame_base = 
					goto PLAYER_UPDATE_BOTTOM;
				}
			}
			
		}else if(p_state & PLAYER_ATTACKING){
			p_state &= ~PLAYER_DIGGING;
//VERIFIED!!!
//HARPOON SPENDS 9 TICKS WITH PLAYER IN ATTACK FRAME AND HARPOON GONE IF IT HITS ANYTHING, NO MATTER THE DISTANCE(EVEN VERY SHORT)
//ONLY IF THE HARPOON FULLY EXTENDS, DOES IT TAKE 10 FRAMES!!!!!!!!!!!!!!!!!!!!!!!!!

			//in the original, pressing attack has the player continuing their actions(even if walking/digging) for 2 ticks(inclusive of the tick input was received).
			//the 3rd tick enters the attack frame but the harpoon is not drawn or moving until the 4th. This is emulated by the state transitions
			//based on harpoon offset

			if(p_state & PLAYER_DELAY_FRAME){//harpoon disappeared, player is recovering
				p_frame = FIRST_ATTACK_FRAME;
				if(!p_ftime){
					p_state ^= (PLAYER_DELAY_FRAME|PLAYER_ATTACKING);
					p_frame = 0;
				}else
					p_ftime--;
			}else if(p_harpoon_off == 255){
				//last frame we pushed attack, set this to 0 to indicate we are on 2nd frame after button press
				p_harpoon_off = 0;//harpoon does not appear for 2 frames(inclusive of button press)
			}else if(p_harpoon_off == 0 && p_frame != FIRST_ATTACK_FRAME){
				//last frame we set harpoon offset to 0, this is the right frame to start drawing the attack frame
				p_frame = FIRST_ATTACK_FRAME;//next tick when we find harpoon offset 0 and p_frame attack, we start moving it
			}else{//harpoon is moving, first happens on tick 4(inclusive of input frame)
				if(p_harpoon_off == 0)//harpoon just shot
					DDTriggerFx(SFX_HARPOON_SHOT);
				
				p_harpoon_off += HARPOON_SPEED;
//RECALC_HARPOON:

				uint8_t htile;
				if(p_state & PLAYER_LEFT){
					hx = p_x-p_harpoon_off;
					hy = p_y+8;
					htile = GetTile((hx+3)>>3,hy>>3);
					if(BlocksHorizontalHarpoon(htile))
						goto HARPOON_HIT_DIRT_OR_BOUNDARY;
				}else if(p_state & PLAYER_RIGHT){
					hx = p_x+16+p_harpoon_off;
					hy = p_y+8;
					htile = GetTile((hx+5)>>3,hy>>3);
					if(BlocksHorizontalHarpoon(htile))
						goto HARPOON_HIT_DIRT_OR_BOUNDARY;
				}else if(p_state & PLAYER_UP){
					hx = (p_state & PLAYER_FEET_RIGHT)?p_x+8:p_x;
					hy = p_y-p_harpoon_off;
					htile = GetTile(hx>>3,(hy+3)>>3);
					if(BlocksVerticalHarpoon(htile))
						goto HARPOON_HIT_DIRT_OR_BOUNDARY;
				}else{//down
					hx = (p_state & PLAYER_FEET_RIGHT)?p_x+8:p_x;
					hy = p_y+16+p_harpoon_off;
					htile = GetTile(hx>>3,(hy+5)>>3);
					if(BlocksVerticalHarpoon(htile))
						goto HARPOON_HIT_DIRT_OR_BOUNDARY;
				}

				if(hx < 0 || hx > (0*8)+(MAP_WIDTH*2*8) || hy < 0 || hy > (SCREEN_TILES_V-1)*8){
HARPOON_HIT_DIRT_OR_BOUNDARY:
					p_ftime = 9;//p_harpoon_off -= HARPOON_SPEED;
					goto CANCEL_HARPOON;//RECALC_HARPOON;
				}

				for(uint8_t i=0;i<MAX_ENEMIES;i++){//check if our harpoon hit something
					if(	e_state[i] == ENEMY_DISABLED || (e_state[i] & (ENEMY_POPPED)) == ENEMY_POPPED ||
						(e_state[i] & (ENEMY_SQUISHED)) == ENEMY_SQUISHED || (e_state[i] & (ENEMY_SHOW_SCORE)) == ENEMY_SHOW_SCORE)//can't kill what is already dead
						continue;

					if(	hx > e_x[i] && hx < e_x[i]+13 &&
						hy > e_y[i] && hy < e_y[i]+13){//hit! give them the air until they pop! >:) (thinking about it, this is seriously a brutal way to kill..very dark)
						/*	if(false){//e_state[i] & ENEMY_STUNNED){//was already hit before
							//	e_frame[i]++;
							}else{//first time they were hit
							//	e_frame[i] = 0;
								e_attrb[i] &= ~0b11;//e_state[i] |= ENEMY_STUNNED;
								e_attrb[i] |= 1;
							}
							*/
							e_attrb[i] += 1;
							p_harpoon_target = i;
							p_state |= PLAYER_PUMPING;//in original, doesn't display pumping frame until next tick, enemy displays effect frame this tick(UpdateEnemies() runs after UpdatePlayer)
							break;
					}
				}

				if(p_harpoon_off >= HARPOON_REACH){//don't draw, using this as a counter until player leaves attack state
CANCEL_HARPOON:
					if(p_ftime != 9)//set from another check, and jumped here
						p_ftime = 10;//only 10 if harpoon traveled full distance, in all other cases this delay is 9, per original
					hx = hy = 0;
					p_harpoon_off = 255;
					p_state |= PLAYER_DELAY_FRAME;

				}
			}//harpoon moving

		}else{//just pushed button to start attack?(if walking, will continue for 2 frames before starting attack!)

			 if((padState & (ACTION_BTN)) && !(oldPadState & (ACTION_BTN)) && p_harpoon_off == 255){//not attacking, elligible to attack?
				p_state |= PLAYER_ATTACKING;//next frame, we will find p_harpoon_off to be 255 and so set it to 0

			}
		}
		
		//even if we are attacking, was can still move for 2 frames until it actually launches(as in original, can move until harpoon has moved)
		if(p_harpoon_off == 255 || p_harpoon_off == 0){//possible started an attack, but it is not yet active, or not in an attack at all:then walk


			
			if(!(padState & (DIRECTIONAL_BTN))){//player is idle and not inputting something new	
				if(false){//!(p_state & PLAYER_ATTACKING)){
					p_ftime = 255;//was decremented above, do not advance frame until we get more input
				}else{//see if we should update our harpoon

				}
//				StopMusic();//player can't be moving so stop music TODO MUST MAKE SURE WE ARE NOT STOPPING OTHER MUSIC
				if(p_idle < 255)
					p_idle++;
			}else{//player has given input
//VERIFIED ORIGINAL ONLY CHECKS/CHANGED DIGGING OR NOT FLAG WHEN MOVING, OTHERWISE KEEPS OLD STATE(EVEN IF ARTIFICIALLY MOVED BY RAM MODIFICATION)
//divet, player x is 4 into tile before digging starts
//flat, 1 before or directly on ??!?

			//up is favored over right,left is favored over up, right is favored over down, down is favored over left, per original(if position is blocked, use another direction though)
				//if(p_ftime > PLAYER_WALK_FRAME_LENGTH)//TODO HACK?? WHY IS IT EVER GREATER?
				//	p_ftime = PLAYER_WALK_FRAME_LENGTH;//TODO HACK??!
				if(!p_ftime){
					p_frame = !p_frame;
					p_ftime = PLAYER_WALK_FRAME_LENGTH;
				}else
					p_ftime--;
				p_last_dir = (p_state & (PLAYER_UP|PLAYER_DOWN|PLAYER_LEFT|PLAYER_RIGHT));
				//p_state &= ~(PLAYER_UP|PLAYER_DOWN|PLAYER_LEFT|PLAYER_RIGHT);
		
				if((padState & BTN_LEFT) && p_x){
PLAYER_LEFT_MOVE:
					if(!(p_y&15)){//grid aligned vertically
						if(!(p_state & PLAYER_LEFT)){//we changed directions, always move the first tick
							//p_last_dir = (p_state&PLAYER_DIRECTIONS);
							do_dig = do_walk = true;//always move the first tick of a new direction
						}
						p_state &= ~(PLAYER_DIRECTIONS|PLAYER_FEET_RIGHT);
						p_state |= PLAYER_LEFT;
					}else{//moving up or down, finish that direction until we align with 16 pixel grid
						if((p_last_dir & PLAYER_UP))
							goto PLAYER_UP_MOVE;
						goto PLAYER_DOWN_MOVE;
					}
					uint8_t dtile = GetTile((p_x-1)>>3,(p_y+0)>>3);
					if(!IsRock(dtile)){
						if(	IsDirt(dtile) &&
							(IsLeftDivet(	GetTile((p_x+10)>>3,p_y>>3)) ||//check for a divet, which we start digging earlier than other dirt
							!IsHDirtTunnel(	GetTile((p_x+13)>>3,p_y>>3)))){

							p_state |= PLAYER_DIGGING;
							if(do_dig)
								p_x--;
						}else{
							p_state &= ~PLAYER_DIGGING;
							if(do_walk)
								p_x--;
						}
					}
				}else if((padState & BTN_UP) && p_y){
PLAYER_UP_MOVE:
					if(!(p_x&15)){//grid aligned horizontally
						if(!(p_state & PLAYER_UP)){//we changed directions, always move the first tick
							//p_last_dir = (p_state&PLAYER_DIRECTIONS);
							do_dig = do_walk = true;//always move the first tick of a new direction
						}
						p_state &= ~(PLAYER_DIRECTIONS);
						p_state |= PLAYER_UP;
						
					}else{//moving left or right, finish that direction until we align with 16 pixel grid
						if((p_last_dir & PLAYER_LEFT))
							goto PLAYER_LEFT_MOVE;
						goto PLAYER_RIGHT_MOVE;
					}

					uint8_t dtile = GetTile((p_x+0)>>3,(p_y-1)>>3);
					if(!IsRock(dtile)){
						if(	IsDirt(dtile) &&
							(IsUpDivet(		GetTile(p_x>>3,(p_y+10)>>3)) ||//check for a divet, which we start digging earlier than other dirt
							!IsVDirtTunnel(	GetTile(p_x>>3,(p_y+13)>>3)))){

							p_state |= PLAYER_DIGGING;
							if(do_dig)
								p_y--;
						}else{
							p_state &= ~PLAYER_DIGGING;
							if(do_walk)
								p_y--;
						}
					}
				}else if((padState & BTN_RIGHT) && p_x < (MAP_WIDTH-1)*2*TILE_WIDTH){
PLAYER_RIGHT_MOVE:
					if(!(p_y&15)){//grid aligned vertically
						if(!(p_state & PLAYER_RIGHT)){//we changed directions, always move the first tick
							//p_last_dir = (p_state&PLAYER_DIRECTIONS);
							do_dig = do_walk = true;//always move the first tick of a new direction
						}
						p_state &= ~(PLAYER_DIRECTIONS);
						p_state |= (PLAYER_RIGHT|PLAYER_FEET_RIGHT);
						
					}else{//moving left or right, finish that direction until we align with 16 pixel grid
						if((p_last_dir & PLAYER_UP))
							goto PLAYER_UP_MOVE;
						goto PLAYER_DOWN_MOVE;
					}

					uint8_t dtile = GetTile((p_x+16)>>3,(p_y+0)>>3);
					if(!IsRock(dtile)){
						if(	IsDirt(dtile) &&
							(IsRightDivet(	GetTile((p_x+10)>>3,p_y>>3)) ||//check for a divet, which we start digging earlier than other dirt
							!IsHDirtTunnel(	GetTile((p_x+13)>>3,p_y>>3)))){

							p_state |= PLAYER_DIGGING;
							if(do_dig)
								p_x++;
						}else{
							p_state &= ~PLAYER_DIGGING;
							if(do_walk)
								p_x++;
						}
					}
				}else if((padState & BTN_DOWN) && p_y < (SCREEN_TILES_V-2)*TILE_HEIGHT){
PLAYER_DOWN_MOVE:
					if(!(p_x&15)){//grid aligned horizontally
						if(!(p_state & PLAYER_DOWN)){//we changed directions, always move the first tick
							//p_last_dir = (p_state&PLAYER_DIRECTIONS);
							if(p_last_dir != PLAYER_UP)
								p_state ^= PLAYER_FEET_RIGHT;//need to flip this bit to draw feet on the right side(not needed for up direction, based on left/right behavior)
							do_dig = do_walk = true;//always move the first tick of a new direction
						}
						p_state &= ~(PLAYER_DIRECTIONS);
						p_state |= PLAYER_DOWN;
						
					}else{//moving left or right, finish that direction until we align with 16 pixel grid
						if((p_last_dir & PLAYER_LEFT))
							goto PLAYER_LEFT_MOVE;
						goto PLAYER_RIGHT_MOVE;
					}

					uint8_t dtile = GetTile((p_x+0)>>3,(p_y+16)>>3);
					if(!IsRock(dtile)){
						if(	IsDirt(dtile) &&
							(IsUpDivet(		GetTile(p_x>>3,(p_y+6)>>3)) ||//check for a divet, which we start digging earlier than other dirt
							!IsVDirtTunnel(	GetTile(p_x>>3,(p_y+9)>>3)))){

							p_state |= PLAYER_DIGGING;
							if(do_dig)
								p_y++;
						}else{
							p_state &= ~PLAYER_DIGGING;
							if(do_walk)
								p_y++;
						}
					}
				}
				

			}
		}//player idle
	}//player not dying

PLAYER_UPDATE_BOTTOM:


	//Draw the player and possibly the harpoon(harpoon will always use higher sprite indices to be over the player/enemies, sometimes flickers off)
	DrawMetaSprite(p_x,p_y,(frame_base+p_frame+((p_state&PLAYER_DIGGING)?2:0)));//always draw the player
	if(global_frame & 1)
		DrawHarpoon();//every other frame, the harpoon is drawn before, next frame after, the enemies. This is to help with flicker/sprite rotation

	return 0;
}



uint8_t CheckSolidity(uint8_t x, uint8_t y){

	uint8_t s = 0;

	uint8_t t = GetTile(x+0,y-1);
	t = pgm_read_byte(&tile_solidity[t]);
	s |= t;
	s <<= 2;

	t = GetTile(x+0,y+2);
	t = pgm_read_byte(&tile_solidity[t]);
	s |= t;
	s <<= 2;

	t = GetTile(x-1,y+0);
	t = pgm_read_byte(&tile_solidity[t]);
	s |= t;
	s <<= 2;

	t = GetTile(x+2,y+0);
	t = pgm_read_byte(&tile_solidity[t]);
	s |= t;

	return s;

}



void DrawFlame(uint8_t x, uint8_t y, uint8_t frame){
	//a Fygar who is inflated mid-flame, will return to the point in the flame process that he was in before the inflation
	//the Fygar will not release the flame until that ends, or he has died, to ensure original behavior(within the 1 simultaneous flame limit as well)
	//so another Fygar doesn't flame, then he recovers and flames also
	
	//flames can be up to 3 16x16 meta tiles long/6x2 sprites
	for(uint8_t i=0;i<frame;i++){
		DrawMetaSprite(x,y,frame+i+FIRST_FYGAR_FLAME_FRAME);
	}
}



void UpdateEnemies(bool draw_only){


//enemy leans towars horizontal movement??
/*
          AND     #$08                   HORIZONTAL IS GREATER
          BNE     ZNXT23
          LDA     TEMP3
          AND     #$02
          BNE     ZNXT23
ZNXT26    LDA     TEMP3
          AND     #$04                   VERTICAL IS GREATER
          BNE     ZNXT23
          LDA     TEMP3
          AND     #$01
          BNE     ZNXT23
          LDA     TEMP3
          AND     #$08                   TRY HORIZ. AGAIN
          BNE     ZNXT23
          LDA     TEMP3
          AND     #$02                   TO HERE.
ZNXT23    STA     DIR,X                  SAVE NEW DIRECTION
          JMP     NEWSEQ                 ;NEWSEQ IS A SUBR






*/
//41 frames after player stops moving, roaming sound begines(seems baked into sound effect in original)

//also draws enemies
//deflating enemies are 32 ticks/frame
//all ghost frames are 16 ticks long
//last exploded death frame is 32 ticks long
//enemy turns into score numbers for 60 ticks

//IT SEEMS ENEMIES WILL NEVER GHOST UNLESS THEY HIT A WALL, TESTED WITH A TOTALLY CLEARED LEVEL. FYGAR MIGHT HAVE GHOSTED AFTER
//A FLAME WHEN HE HIT A WALL, BUT DID NOT HAVE TO REVERSE. POOKAS NEVER DO UNLESS THEY NEED TO REVERSE?

//PERHAPS ENEMIES DEFAULT TO UP AND LEFT, WHEN THEY ARE NOT PICKING X/Y MAGNITUDES TOWARDS PLAYER
// OR LESS LIKELY? ENEMY PREFERS PREVIOUSLY MOVED DIRECTION IF MAGNITUDES ARE THE SAME?

//Fygar starts attack with 8 frames of normal colored open mouth frame, then 8 white wings, then 8 normal, 8 white, 8 normal, 8 white, 8 normal,
//and finally 7 frames of white before going to normal open mouth frame and flame is active.
//Fygar's fire can go up to 3 map spaces long(6*8), the flame sequence is 5 ticks of the first shortest frame, 6 of the middle flame frame, and 5 of the longest last one
//then 2? ticks of standing there before returning to chasing

//	uint8_t weights[4];

	if(p_state & PLAYER_DYING)
		draw_only = true;

	uint8_t draw_frame;
	uint8_t enemies_alive = 0;
	uint8_t e = global_frame&0b111;//update/draw starting with a different enemy each frame, as a form of "flicker safety"
	for(uint8_t i=0;i<MAX_ENEMIES;i++){
		if(++e >= MAX_ENEMIES)
			e = 0;
		if(e_state[e] == ENEMY_DISABLED)//totally dead(done showing score) or never initialized
			continue;

		draw_frame = ((e < MAX_ENEMIES/2)?FIRST_POOKA_FRAME+1:FIRST_FYGAR_FRAME+1);
		draw_frame += (global_frame&16)?1:0;

		if((e_state[e] & (ENEMY_SHOW_SCORE)) == (ENEMY_SHOW_SCORE)){//enemy is dead, and we just need to draw the score awarded for 60 ticks total

			//this state is NOT reversible
			uint8_t ftime = (e_attrb[e]&0b11111100)>>2;//extract the timer for this state
			if(!ftime){//done with the state, simply turn the enemy off
				e_state[e] = ENEMY_DISABLED;
			}else{//still showing it, decrement the timer and draw the score
				ftime--;
				e_attrb[e] &= ~0b11111100;
				e_attrb[e] |= (ftime<<2);//store the decremented value
				
				SetSprite(e_x[e]+8,e_y[e],0,0);//draw the trailing zeros
				SetSprite(e_x[e]+15,e_y[e],0,0);
			}
			continue;//either way, do not DrawMetaSprite for this state

		}else if((e_state[e] & (ENEMY_POPPED)) == (ENEMY_POPPED)){//overloaded/combined direction bits to save ram*MAX_ENEMIES(an otherwise impossible state)

			//this state is NOT reversible, otherwise we could not handle it this way
			//this state specifically needs to preserve left/right bits so we know which direction to draw the frame(overloads up+down, otherwise impossible state, instead)
			uint8_t ftime = (e_attrb[e] & 0b11111100)>>2;//get our frame time until dead(then showing score)
			if(!ftime){
				e_state[e] = ENEMY_SHOW_SCORE;
				e_attrb[e] |= 11110000;//(60<<2);//set a timer length for showing the score
			}else{//still showing popped frame
				ftime--;
				e_attrb[e] &= ~0b11111100;//clear the original counter
				e_attrb[e] |= (ftime<<2);//store the new decremented value
			}
			uint8_t f = ((e<MAX_ENEMIES/2)?2:6+2);
			if(e_state[e] & ENEMY_MOVE_RIGHT)
				f += 3;
			DrawBigMetaSprite(e_x[e]-4,e_y[e]-4,f);
			continue;//normal DrawMetaSprite cannot handle these larger frames			

		}else if((e_state[e] & ENEMY_SQUISHED) == (ENEMY_SQUISHED)){//overloaded bits

			//this state is NOT reversible, and we stay here until the rock is done
			if(r_state[e_attrb[e]] == ROCK_DISABLED){//rock is done, now we disappear(we do not display a score, the rock displays a cumulative score by itself)
				e_state[e] = ENEMY_DISABLED;
				continue;
			}else{//rock is still active, adjust our position accordingly
				e_y[e] = r_y[e_attrb[e]]+8;//would be 16, except the top 2 sprites of the meta will not end up drawn(but still used as origin)
			}

		}else{//now we handle the states that are possible to reverse(so we must keep prior knowledge of state and not overload/combo bits as above)
			enemies_alive++;
			if(e_attrb[e] & 0b11){//enemy is inflated. we use the bottom 2 bits of attributes byte instead of dedicating a bit in primary state(inflation state is a counter towards death basically)
//TODO THIS STATE IS ALL SANDBOXING AND TOTALLY BROKEN
//FIX ME
//FIX ME
/*
				//this state is reversible back to:walking or ghost or attacking
				uint8_t ftime = (e_attrb[e] & 0b11111100)>>2;//get our frame time until dead(then showing score)
				uint8_t step = e_attrb[e]&0b11;
				if(!ftime){//done deflating 1 magnitude
					step--;
					if(step == 0){//totally done deflating, nothing to do, the state disabled itself by counting down
						//continue;//TODO CHECK AND SEE IF THE ENEMY MOVES THE TICK IMMEDIATELY AFTER DEFLATED OR THAT TICK ITSELF
					}else{
						e_attrb[e] &= ~0b11;//clear the previous step count
						e_attrb[e] |= step;
						e_attrb[e] |= 00111100;//set a timer length for deflating another step TODO THIS NEEDS TO BE A TABLE PER LEVEL OR SO
					}
				}else{//still showing the same inflated frame
					ftime--;
					e_attrb[e] &= ~0b11111100;//clear the original counter
					e_attrb[e] |= (ftime<<2);//store the new decremented value
					draw_frame = ((e < MAX_ENEMIES/2)?FIRST_POOKA_FRAME+1:FIRST_FYGAR_FRAME+1);
					draw_frame += step;
				}
			uint8_t f = ((e<MAX_ENEMIES/2)?FIRST_LARGE_SPRITE_FRAME+2:FIRST_LARGE_SPRITE_FRAME+6+2);
			if(e_state[e] & ENEMY_MOVE_RIGHT)
				f += 3;
*/
uint8_t f = (e_attrb[e]&0b11);
			DrawBigMetaSprite(e_x[e]-4,e_y[e]-4,f);
				continue;

			}else if(e_state[e] & ENEMY_ATTACKING){//this just counts upwards on the high 6 bits, until they reach the limit or the fire is blocked
				//this state is reversible. possible states after:ghosting, walking



			}else{//must be walking around, most common case
				//this state can go to attacking or ghosting or inflated or squished
				if((e_x[e]&15) && (e_y[e]&15)){//keep going this direction until we get to the next 16 pixel boundary

				}else{//time to possibly choose a new direction
					uint8_t t = CheckSolidity(e_x[e],e_y[e]);//each surrounding direction is encoded in 2 bits(0bUUDDLLRR)
					uint8_t weights[4] = {60,60,60,60};
				
					if(!(t>>6))//up
						weights[0] += 60;
					if(!((t>>4)&0b11))//down
						weights[1] += 60;
					if(!((t>>2)&0b11))//left
						weights[2] += 60;
					if(!(t&0b11))//right
						weights[3] += 60;
				
					t = e_state[e] & (ENEMY_MOVE_UP|ENEMY_MOVE_DOWN|ENEMY_MOVE_LEFT|ENEMY_MOVE_RIGHT);

					if(t == ENEMY_MOVE_UP)//preferably, do not back track unless there is no other option
						weights[1] -= 50;
					else if(t == ENEMY_MOVE_DOWN)
						weights[0] -= 50;
					else if(t == ENEMY_MOVE_LEFT)
						weights[3] -= 50;
					else//ENEMY_RIGHT
						weights[2] -= 50;

					for(uint8_t r=0;r<MAX_ROCKS;r++){//should we flee from a rock?
						if(r_state[r] == ROCK_DISABLED || r_x[r] != e_x[e])
							continue;
						weights[1] += 58;
					}

					if(p_x > e_x[e])
						weights[3] += 3;
					else if(p_x < e_x[e])
						weights[2] += 3;
					if(p_y > e_y[e])
						weights[1] += 2;
					else if(p_y < e_y[e])
						weights[0] += 2;
				
					uint8_t c = 0;
					uint8_t m = 0;
					for(t=0;t<4;t++){
						if(weights[t] > m){
							m = weights[t];
							c = t;
						}
					}

				//now our directional choice in in c, determined from solidity, player position, previous direction, and possibly rock danger


				}//else choose direction

			}
			
		}



//if(!draw_only)
	e_x[e]--;

DrawMetaSprite(e_x[e],e_y[e],draw_frame);
continue;
	



	}//for(MAX_ENEMIES)

	if(!(global_frame&1))
		DrawHarpoon();//odd frames, harpoon is drawn after the enemies. on even, before, to help with sprite flickering/rotation


	if(!draw_only && enemies_alive && (p_idle > 40) && !DDIsEffectPlaying(SFX_ENEMY_ROAM))
		DDTriggerFx(SFX_ENEMY_ROAM);

}

const uint8_t rock_frame_lengths[] PROGMEM = {
1,12,16,16,16,16,//delay tick,initial ticks up,down,up,down,falling(indefinite length)
16,16,16,16,16,12,//16+16+16+16+12=92 ticks landed before cracking
15,15,15,15,//15+15+15+15=60 ticks in cracked frame
15,15,15,15,//15+15+15+15=60 ticks in rubble frame before disappearing
};
#define RFD FIRST_ROCK_FRAME
const uint8_t rock_frame_draw[] PROGMEM = {
255,RFD+1,255,RFD+1,255,RFD+0,//shaking+falling
RFD+0,RFD+0,RFD+0,RFD+0,RFD+0,RFD+0,//landed
RFD+2,RFD+2,RFD+2,RFD+2,//crack frame
RFD+3,RFD+3,RFD+3,RFD+3,//rubble frame
};

bool UpdateRocks(bool draw_only){
	//when rock is distrubed, it moved to tilted up frame(next tick after player makes the move that does it) which lasts for 12 frames
	//then the normal down frame which lasts for 16, then up for 16, then down for 16, then starts falling.
	uint8_t rocks_active;
	for(uint8_t i=0;i<MAX_ROCKS;i++){
		if(r_state[i] == ROCK_DISABLED)//rock not active
			continue;
		rocks_active++;
		uint8_t frame = (r_state[i]&0b11110000)>>4;
		uint8_t ftime = (r_state[i]&0b00001111);

		if(draw_only)//only at beginning of round
			r_state[i] = 1;//rock did not get to fall last round,reset to beginning of shaking state(normal down shake frame, for 1 tick)
		else{//update rock actions
			if(!ftime){//time for next frame?
				if(frame == 5){//falling, which has indefinite frame length
					ftime = 16;
					r_y[i] += ROCK_FALL_SPEED;
					bool landed = false;
					if(r_y[i] >= ((SCREEN_TILES_V-2)*8)){//adjust for boundaries
						r_y[i] = ((SCREEN_TILES_V-2)*8)-2;
						landed = true;
					}
					//TODO CHECK FOR COLLISIONS
					for(uint8_t j=0;j<MAX_ENEMIES;j++){
						if(r_y[i]+16 >= e_y[j] && r_y[i] <= e_y[j]+16 && r_x[i]+16 >= e_x[j] && r_x[i] <= e_x[j]+16){//hit!
							e_state[j] = ENEMY_SQUISHED;
							e_attrb[j] = i;//keep track of which rock squished the enemy, so it can disappear when the rock is done
						}
					}
				}else{
					ftime = pgm_read_byte(&rock_frame_lengths[++frame]);
				}
				if(frame >= sizeof(rock_frame_lengths)){//rock is done
					uint8_t squished = 0;
					for(uint8_t j=0;j<MAX_ENEMIES;j++){//see how many enemies we squished
						if(e_attrb[j] == i)//we got this one
							squished++;
					}
				}
			}else
				ftime--;

			
			r_state[i] = frame<<4;
			r_state[i] |= ftime;
		}

		uint8_t draw_frame = pgm_read_byte(&rock_frame_draw[frame]);

		if(frame == 255){//normal down frame of wiggle animation(not falling), use tiles to save ram tiles
			uint8_t color = (r_y[i]>>3)-2;
			//todo adjust for depth
			//FIRST_ROCK_TILE_FRAME NUM_FRAMES_PER_DEPTH
			DrawMetaTileMapOffset(r_x[i]>>3,r_x[i]>>3,FIRST_ROCK_TILE_FRAME,color);
		}else{//otherwise use sprites
			DrawMetaSprite(r_x[i],r_y[i],draw_frame);
		}
	}


	for(uint8_t y=0;y<MAP_HEIGHT-1;y++){//look for any new rocks that should be falling, rocks should never be on bottom row
		for(uint8_t x=0;x<MAP_WIDTH;x++){
			if(GetMapTileType(x,y) != MAP_TILE_ROCK)
				continue;

			if(!CanSupportRock(x,y+1)){//this one should be falling, find a slot and make it drop
				uint8_t r = 255;
				for(uint8_t i=0;i<MAX_ACTIVE_ROCKS;i++){//find a slot to take for this rock
					if(!r_state[i]){
						r = i;
						break;
					}
				//if we don't find a slot, this rock waits to fall(prevents squished enemy issues, and should never happen during normal play anyway)
				}
				if(r == 255)//didn't find a free one?
					break;
				r_state[r] = 1;//1 tick delay before it starts wiggling
				r_x[r] = (x*16)+0;
				r_y[r] = (y*16)+0;
				break;
			}
		}
	}

	if(draw_only || !rocks_active)
		return false;
	return true;

//the round clear music will wait befoer starting until a crumbling animation has completed
/*TODO 7800 VERSION SAYS:
RFALLTBL  DB      1,2,2,2,1,2,2

BUT NES VERSION APPEARS TO REPEATEDLY FALL 2 PIXELS EVERY FRAME



*/



//RANDROKS  DB      $29,$35,$64,$82,$8D,$9A,$A5,$C9
/*
          ORG     $C100
ROCKS     DB      $35,$B4,$9B,$00,$00
          DB      $23,$39,$8D,$B5,$00
          DB      $23,$6C,$BD,$94,$00
          DB      $43,$2B,$5C,$BD,$84
          DB      $63,$29,$3D,$BB,$A6
          DB      $28,$6B,$97,$B3,$00
          DB      $29,$52,$6D,$AC,$00
          DB      $54,$6D,$96,$B8,$00
          DB      $28,$3C,$7A,$72,$B6
          DB      $34,$72,$8C,$B8,$00
          DB      $33,$3B,$BC,$A6,$00
          DB      $39,$7B,$82,$CA,$00
          DB      $32,$29,$BA,$A3,$00
          DB      $32,$4B,$B9,$83,$00
          DB      $52,$39,$9A,$B4,$00
          DB      $53,$9B,$00,$00,$00    ;BONZO LEVEL 0
          DB      $3B,$00,$00,$00,$00    ;BONZO LEVEL 1

*/
	//score for squishing 1 is 1000
	//score for squishing 2 is 2500
	//score for squishing 3 is 4000
	//score for squishing 4 is 6000
	//score for squishing 5 is 8000
	//score for squishing 6 is 10000
	//score for squishing 7 is 12000




	//rocks will keep falling during player death
	//rocks will NOT keep shaking during death, but will continue where they left off next round?
	//crumbling rock will stall level level ending(after dying) until it is complete

	//rocks in original are pretty glitchy.

	//when rock lands, it is 92 frames until the first crack frame? the first crack frame lasts 60 ticks,
	//then the final rubble frame lasts 60 ticks
	//if this is the second rock dropped, the bonus appears immediately when landing, not after cracking is done

	//if squishing something, it spends 2 frames on the ground(with enemy in squished frame) before shooting back upwards
	//off the ground to on top of the enemies head!!!(1 pixel gap actually), then 88 frames later the enemy disappears, but the rock stays there
	//for 1 frame, then next frame the score appears for 120 ticks

	//the squish or no squish sound seems to happen the same frame the rock makes the initial landing(presumably before flying back up if squishing an enemy)

//ROCK_FALL_SPEED is 2

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************LOGIC SUPPORT FUNCTIONS****************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




uint8_t DDWaitVsyncEx(uint8_t t, bool cancel){
//WaitVsync(t);
//return;

	bool did_wait;
//DDWAIT_VSYNC_TOP:
	did_wait = false;
	while(t--){
		while(!GetVsyncFlag()){
			did_wait = true;
			NetworkStateMachine();//this is designed to do the smallest slice of update possible, called as often as possible
		//	if(IsUartRxBufferEmpty() && IsUartTxBufferEmpty())//do some "blinkenlights" for fun
		//		SetLedOn();
		//	else
		//		SetLedOff();
		}

		ClearVsyncFlag();
		if(net_delay)
			net_delay--;
			/////////////////////////////////TODO, WHEN LAGGING, SET USERRAMTILES TO KEEP THE BLIT SPRITES APPEARING, BUT WAIT.
		if(cancel && (padState & BTN_START))//user cancelled something
			return 1;
	//	if(!did_wait){
	//		ResetSprites();//this is kind of an emergency!! TODO USE USER RAM TILES INDICES INSTEAD!!!!!
	//		goto DDWAIT_VSYNC_TOP;//slow down instead of crashing/etc
	//	}
	}
	return 0;
}



inline void DDWaitVsync(uint8_t t){
	DDWaitVsyncEx(t,0);
}



inline uint8_t DDWaitVsyncWithCancel(uint8_t t){
	return DDWaitVsyncEx(t,1);
}



void AddScore(uint16_t s){

	uint32_t old = p_score;
	p_score += s;
	//extra life at 20,000, then 60,000, and ever 60,000 more after that(120k,180k,240k,etc)
	if(	((old < FIRST_FREE_LIFE_SCORE) && (p_score >= FIRST_FREE_LIFE_SCORE)) 		||//respresented as 10,000, first extra up
		((old < SECOND_FREE_LIFE_SCORE) && (p_score >= SECOND_FREE_LIFE_SCORE)) 	||
		((old%SECOND_FREE_LIFE_SCORE) >= (p_score&SECOND_FREE_LIFE_SCORE))){//passed another 40,000 mark
			StartSong(MUS_1_UP);
	}
	if(p_score > highScore)
		highScore = p_score;
	p_state |= PLAYER_UPDATE_SCOREBOARD;

}



uint8_t DDRandom(){
/*
this is the 7800 version
RANDOM
          LDA     FRMCNT
          ADC     RNDM
          ADC     RNDM+1
          PHA
          LDA     RNDM
          STA     RNDM+1
          PLA
          STA     RNDM
          RTS


	uint8_t a,s;
	
	a = global_frame;
	rnd[0] += a;
	rnd[1] += a;
	s = a;
	a = rnd[0];
	rnd[1] = a;
	a = s;
	rnd[0] = a;

	return a;
*/
return 0;
}



void DDCrash(){
	DDStopSong();
	while(1)
		DDTriggerFx(SFX_HARPOON_SHOT);
}






uint8_t UpdateEEPROM(bool direction){

	struct EepromBlockStruct ebs;
	ebs.id = DIGDUG_EEPROM_ID;
	uint16_t rt_base = (RAM_TILES_COUNT-8)*RAM_TILE_BYTE_COUNT;
	if(EepromReadBlock(ebs.id, &ebs)){//doesn't exist, try to make it
		for(uint8_t i=0;i<30;i++)
				ebs.data[i] = pgm_read_byte(&default_eeprom[i]);

		EepromWriteBlock(&ebs);
	}

	if(EepromReadBlock(ebs.id, &ebs) == 0){//it exists
		if(!direction){//read
			for(uint8_t i=0;i<30;i++)
				ram_tiles[rt_base+i] = ebs.data[i];
		}else{//write
			for(uint8_t i=0;i<30;i++)
				ebs.data[i] = ram_tiles[rt_base+i];

			EepromWriteBlock(&ebs);
		}
	}else if(!direction){//we can't access the block, it's full or there is a problem with the eeprom
			for(uint8_t i=0;i<30;i++)
				ram_tiles[rt_base+i] = pgm_read_byte(&eeprom_error[i]);
			return 1;
	}

	return 0;
}



uint8_t GetMapTileType(uint8_t x, uint8_t y){
	

	return 0;
}



inline uint8_t GetMapTileColor(uint8_t y){//in map tile coordinates
	return y/(MAP_HEIGHT<<2);
}



bool CanSupportRock(uint8_t x, uint8_t y){

	x<<=1;//convert to vram addressing
	y<<=1;
	uint16_t voff = (y*VRAM_TILES_H)+x;
	uint8_t t = vram[voff++]-RAM_TILES_COUNT;

	if(false){//is this not solid? then check if the other tile that could be holding it up is
		t = vram[voff];
		if(false)//this one is not solid either? Make that rock fall then!
			return false;
	}

	return true;
}



void CarveTunnel(uint8_t pos, uint8_t dir, uint8_t flags){

	if(pos == 255)
		return;

//hack, add 12 to all positions in static data?
pos += 12;



	uint8_t y = (pos/12)<<1;
	uint8_t x = (pos%12)<<1;

	uint8_t t;
		
	for(uint8_t i=0;i<3;i++){

		uint8_t offset = 0;
		if(y > 19)
			offset = 3*17;
		else if(y > 13)
			offset = 2*17;
		else if(y > 7)
			offset = 1*17;

		t = GetTile(x,y);

		if((t == 5) || (t == 22) || (t == 39) || (t == 56)){//solid dirt tile
			if(dir)
				t = 4+i;//first vertical meta tile
			else
				t = 1+i;//horizontal
		}else{//overlapped existing tunnel structure, assuming this was an intentional special case for level 1(longer than 3 tunnels)
			if(dir)
				t = 5;
			else
				t = 2;
		}
		DrawMetaTileMapOffset(x,y,t,offset);

		//check to see if there is an enemy at this spot
		t = flags>>(4-(i<<1));
		t &= 0b11;
		if(t == 0b11)//special case, draw squared black meta tile(center of level where player starts)
			DrawMetaTileMap(x,y,0);
		else if(t)//Pooka or Fygar is here
			InitEnemy(x,y,t);

		if(dir)
			y+=2;
		else
			x+=2;
	}

} 



void Pause(){

	if(playSong)//IsSongPlaying())//TODO GET NEW KERNEL VERSION
		DDStopSong();
	DDTriggerFx(SFX_PAUSE);
	uint8_t frame = 0;
	uint8_t old_global_frame = global_frame;
	while(1){
		DDWaitVsync(1);//input is handled in user vsync routine
		ResetSprites();

		frame++;
		global_frame++;//keep sprite rotation working correctly, restore old value when done to not modify enemy thinking
		if(frame &0b10000){
			for(uint8_t i=0;i<5;i++)//blank it
				SetTile(PAUSE_TEXT_X_OFF+i,PAUSE_TEXT_Y_OFF,0);
		}else
			DDPrint(PAUSE_TEXT_X_OFF,PAUSE_TEXT_Y_OFF,PSTR("PAUSE"));
		UpdatePlayer(1);//TODO KEEP SPRITE FLICKERING WORKING
		UpdateEnemies(1);
		UpdateRocks(1);
		if(padState & BTN_START && !(oldPadState & BTN_START)){//unpause
			global_frame = old_global_frame;
			DDWaitVsync(1);
			break;
		}
	}
	for(uint8_t i=0;i<5;i++)//blank it
		SetTile(PAUSE_TEXT_X_OFF+i,PAUSE_TEXT_Y_OFF,0);
	ResumeSong();

}



void PushLevelState(){

}



void PopLevelState(){


}

void StartDemo(uint8_t d){

	demoOffset = 0;
	for(uint8_t i=0;i<d;i++){
		while(!(pgm_read_byte(&demo_data[demoOffset++]) & BTN_START));//scan to first byte of demo
	}
	padState = pgm_read_byte(&demo_data[demoOffset++]);
	demoPadTicks = pgm_read_byte(&demo_data[demoOffset++]);
	gameState |= GAME_DEMO_PLAYING;

}


/////////TODO WHY DOES 64 SPRITES SLOW EVERYTHING DOWN SO MUCH!?!?!?!?!?!?!?!

void VsyncRoutine(){

	oldPadState = padState;

	if(gameState & GAME_DEMO_PLAYING){//take input from a flash stream, until it is done or interrupted
		if(!demoPadTicks){
			padState = pgm_read_byte(&demo_data[demoOffset++]);
			demoPadTicks = pgm_read_byte(&demo_data[demoOffset++]);
			
			if(padState & BTN_START){//end of command stream
				gameState ^= GAME_DEMO_PLAYING;
			}
		}else
			demoPadTicks--;
	}else{
	//	ReadControllers();
	//	oldPadState = padState;
		uint16_t t = ReadJoypad(0);
		padState = t&0xff;
		if(t & (BTN_A|BTN_X))//we use an 8 bit variable for pad states with BTN_A out of range, allow it's use anyway for konami code at title
			padState |= BTN_Y;
	}

}



void NullEnemiesAndRocks(){
	for(uint8_t i=0;i<MAX_ENEMIES;i++)
		e_state[i] = ENEMY_DISABLED;
	for(uint8_t i=0;i<MAX_ROCKS;i++)
		r_state[i] = ROCK_DISABLED;
}



void InitEnemy(uint8_t x, uint8_t y, uint8_t t){//takes coordinates in map meta tiles
//TODO ENEMY DOES NOT NECESSARIYL FACE TOWARDS PLAYER??? NEED EXTRA MAP FORMAT DATA TO DESCRIBE DIRECTION???!?

	//any enemy > index 3 is a Fygar, enemies always spawn facing the center
	uint8_t xmag,ymag;

	if(x < MAP_WIDTH/2)
		xmag = (MAP_WIDTH/2)-x;
	else
		xmag = x-(MAP_WIDTH/2);

	if(y < MAP_HEIGHT/2)
		ymag = (MAP_HEIGHT/2)-y;
	else
		ymag = y-(MAP_HEIGHT/2);
	
	//TODO MAKE FACING DIRECTIONS RIGHT, SINCE THEY ARE SEMI INDEPENDENT OF MOVEMENT
	if(xmag > ymag){

	}else{

	}

	uint8_t index = 255;
	uint8_t start = (t==0b01)?0:(MAX_ENEMIES/2);
	uint8_t end = (t==0b01)?(MAX_ENEMIES/2):MAX_ENEMIES;

	for(uint8_t i=start;i<end;i++){//see if there is an open slot for this type of enemy
		if(e_state[i] == ENEMY_DISABLED){
			index = i;
			break;
		}
	}

	if(index == 255)// || ((p_state&PLAYER_DIED_THIS_ROUND) && (e_restore[pActive]&(1<<index))))//no enemy slot, or this enemy was killed before(player died and reloading round)
		return;

	e_x[index] = x<<3;//convert to sprite coords
	e_y[index] = y<<3;
	e_frame[index] = 0;
	e_state[index] = 0;
	e_attrb[index] = 0;

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************GRAPHICAL FUNCTIONS********************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void ResetSprites(){
	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = (SCREEN_TILES_H*TILE_WIDTH);
	sprite_count = 0;
}



void DDPrint(uint8_t x, uint8_t y, const char *s){

	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(&(s[i++]));		
		if(c!=0){
			c = c&127;
			if(c == ' '){
				x++;
				continue;
			}
			c += 215;
			SetFont(x++,y,c);
		}else
			break;
	}
	
}



void DDPrintInt(uint8_t x,uint8_t y, uint32_t val){
	uint8_t c,i;

	//SetFont(x+1,y,'0'-41);//all numbers are displayed to the player as 10x what is stored(always 0 at end in original)
	for(i=0;i<8;i++){
		c=val%10;

		if(val>0)
			SetFont(x--,y,c+'0'-41);

		(uint32_t)(val=val/10);
	}	
}



void DDPrintIntConstrained(uint8_t x,uint8_t y, uint32_t val, uint8_t mindigits){
	uint8_t c,i,m;//,xs = x;
	mindigits--;//always a trailing 0
	
	if(val > 9999UL)
		x++;
	SetFont(x--,y,'0'-41);
	m = (uint32_t)(val/100000UL);

	for(i=0;i<5;i++){
		c=val%10;

		if(val>0 || i<mindigits)
			SetFont(x--,y,c+'0'-41);

		(uint32_t)(val=val/10);
	}
	//because there is not enough space horizontally on the side panel, we must display millions in an unorthodox way
	//y++;
	//for(uint8_t i=0;i<m;i++)
	//	SetFont(xs+i,y,'M');
	//the original only showed 6 digts of score...we will do the same! ShowHighScore() still shows the full score
}



void UpdateSidePanel(uint8_t flash){//draw the things that change specifically for player 1 or 2

	if(flash){
		SetTile(PANEL_X_OFF+0,9,0);//blank tile
		SetTile(PANEL_X_OFF+1,9,0);//blank tile
	}else{
		SetTile(PANEL_X_OFF+0,9,131);//'1'
		SetTile(PANEL_X_OFF+1,9,133);//'P'
	}

}



void SetSprite(uint8_t x, uint8_t y, uint8_t t, uint8_t f){//what is going on with this....??!?! kerel function named this somewhere?!?!?!? TODO
	if((x >= SPRITE_HORIZONTAL_LIMIT) || (y > (SCREEN_TILES_V-1)*TILE_HEIGHT) || (sprite_count >= MAX_SPRITES))
		return;
	sprites[sprite_count].x = x;
	sprites[sprite_count].y = y;
	sprites[sprite_count].tileIndex = t;
	sprites[sprite_count++].flags = f;
}



void DrawMetaTile(uint8_t x, uint8_t y, uint8_t t){
	SetTile(x+0,y+0,t++);
	SetTile(x+1,y+0,t++);
	SetTile(x+0,y+1,t++);
	SetTile(x+1,y+1,t);
}



void DrawMetaTileMap(uint8_t x, uint8_t y, uint8_t t){
	uint16_t toff = t*4;
	SetTile(x+0,y+0,pgm_read_byte(&meta_tile_maps[toff++]));
	SetTile(x+1,y+0,pgm_read_byte(&meta_tile_maps[toff++]));
	SetTile(x+0,y+1,pgm_read_byte(&meta_tile_maps[toff++]));
	SetTile(x+1,y+1,pgm_read_byte(&meta_tile_maps[toff]));
}



void DrawMetaTileMapOffset(uint8_t x, uint8_t y, uint8_t t, uint8_t o){
	uint16_t toff = t*4;
	SetTile(x+0,y+0,pgm_read_byte(&meta_tile_maps[toff++])+o);
	SetTile(x+1,y+0,pgm_read_byte(&meta_tile_maps[toff++])+o);
	SetTile(x+0,y+1,pgm_read_byte(&meta_tile_maps[toff++])+o);
	SetTile(x+1,y+1,pgm_read_byte(&meta_tile_maps[toff])+o);
}



void DrawMetaSprite(uint8_t x, uint8_t y, uint8_t f){
	uint16_t soff = f*4;
	SetSprite(x+0,y+0,pgm_read_byte(&sprite_frames[soff]),pgm_read_byte(&sprite_flags[soff++]));
	SetSprite(x+8,y+0,pgm_read_byte(&sprite_frames[soff]),pgm_read_byte(&sprite_flags[soff++]));
	SetSprite(x+0,y+8,pgm_read_byte(&sprite_frames[soff]),pgm_read_byte(&sprite_flags[soff++]));
	SetSprite(x+8,y+8,pgm_read_byte(&sprite_frames[soff]),pgm_read_byte(&sprite_flags[soff]));
}



void DrawBigMetaSprite(uint8_t x, uint8_t y, uint8_t f){
	uint16_t soff = FIRST_LARGE_SPRITE_FRAME*4;//get past the 4 byte entries
	soff += f*3*3;//we assume the caller passes f, as an offset past the first large sprite frame
	for(uint8_t yo=0;yo<3;yo++)
		for(uint8_t xo=0;xo<3;xo++)
			SetSprite(x+(xo<<3),y+(yo<<3),pgm_read_byte(&sprite_frames[soff]),pgm_read_byte(&sprite_flags[soff++]));

}



void RedrawScore(){

	if((global_frame &0b11111)==16){//flash "1UP" or "2UP"
		for(uint8_t i=0;i<3;i++)
			SetTile(PANEL_X_OFF+i,5,0);
	}else if((global_frame &0b11111) == 0)
		DDPrint(PANEL_X_OFF+0,5,PSTR(",./"));//draw "1UP" or "2UP"
	
	DDPrintIntConstrained(PANEL_X_OFF+4+(p_score>9999),6,p_score,2);

}



void DrawSidePanel(){//draw the things similar between player 1 and 2
	//TODO, IN THE ORIGINAL, AFTER 2 FRAMES OF BLACK, THE LEVEL IS DRAWN, THEN NEXT TICK "READY!" DRAWN AND "1/2UP", THEN NEXT TICK DRAW THE LIVES
	//HI-SCORE AND NUMBERS ARE DRAWN THE SAME INITIAL FRAME THE LEVEL IS

	for(uint8_t y=0;y<SCREEN_TILES_V;y++)//draw side panel background
		for(uint8_t x=PANEL_X_OFF;x<SCREEN_TILES_H;x++)
			SetTile(x,y,0);

	DDPrint(PANEL_X_OFF+0,0,PSTR("<=>"));//print "HI-" in red font
	DDPrint(PANEL_X_OFF+1,1,PSTR("?@)*+"));//print "SCORE" in red font

	DDPrintIntConstrained(PANEL_X_OFF+4,2,p_score,2);//hi, TODO ADD X ADJUSTMENT FOR >999
	SetTile(PANEL_X_OFF+4,2,135);//add trailing 0, all scores are represented to the player as 10 times the actual stored value

	DDPrint(PANEL_X_OFF+0,5,PSTR(",./"));//"1UP"
	if(gameState & GAME_IS_2_PLAYER)
		DDPrint(PANEL_X_OFF+0,9,PSTR("-./"));//"2UP"

//	RedrawScore();

	DDPrint(PANEL_X_OFF,23,PSTR("ROUND"));
	DDPrintInt(PANEL_X_OFF+3,24,p_round+1);

	uint8_t l = (p_lives&0xf);
	if(l > 10)
		l = 10;
	for(uint8_t y=0;y<3;y++)
		for(uint8_t x=0;x<3;x++){
			if(!l)
				break;
			l--;
			DrawMetaTile(PANEL_X_OFF+(x<<1),16+(y<<1),124);
		} 
//	UpdateSidePanel(0);

}



inline void DrawBonus(){
//TODO DRAW WITH SPRITES
	uint8_t b;
	if(p_round > 15)
		b = 15;
	else
		b = p_round;

	b = pgm_read_byte(&bonus_types[b]);
//	DrawMetaTileMap(BONUS_X_OFF,BONUS_Y_OFF,FIRST_BONUS_FRAME+b);

}




void DrawFlowers(){
	uint8_t round = p_round+1;
	uint8_t i;
	uint8_t xoff = MAP_X_OFF+(MAP_WIDTH*2)-2;

	i = round/100;
	round %= 100;
	for(uint8_t j=0;j<i;j++){
		DrawMetaTile(xoff,0,FIRST_FLOWER_TILE+0);
		xoff -= 2;
	}
	
	i = round/10;
	round %= 10;
	for(uint8_t j=0;j<i;j++){
		DrawMetaTile(xoff,0,FIRST_FLOWER_TILE+4);
		xoff -= 2;
	}

	i = round;
	for(uint8_t j=0;j<i;j++){
		DrawMetaTile(xoff,0,FIRST_FLOWER_TILE+8);
		xoff -= 2;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************USER INTERFACE*************************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//roaming sound is simultaneous with any sound effect...in this version
void DDTriggerFx(uint8_t p){
//	if(gameState & GAME_SONG_NO_INTERRUPT)//no sound effects play with the "one enemy left" or "hurry up" songs running. flag is reset when song is over, in logic functions
//		return;

	uint8_t poff = p*3;
	uint8_t r,c,t;

	do{
		r = pgm_read_byte(&sfx_attributes[poff++]);//priority
		c = pgm_read_byte(&sfx_attributes[poff++]);//channel
		t = pgm_read_byte(&sfx_attributes[poff++]);//2 part?

		if(!(tracks[c].flags&TRACK_FLAGS_PRIORITY) || (r >= (sfx_priority&(0b11<<(c*2))))){
			Track* track=&tracks[c];
			track->flags=TRACK_FLAGS_PRIORITY; //priority=1;
			track->patchCommandStreamPos = NULL;
			TriggerCommon(track,p,255,80);
			track->flags|=TRACK_FLAGS_PLAYING;
			sfx_priority &= ~(0b11<<(c*2));
			sfx_priority |= (r<<(c*2));
		}//else
		//	return;
		p++;//if 2 part effect
	}while(t);
}



bool DDIsEffectPlaying(uint8_t p){
	if((tracks[1].fxPatchNo==p && (tracks[1].flags&TRACK_FLAGS_PRIORITY)) || (tracks[2].fxPatchNo==p && (tracks[2].flags&TRACK_FLAGS_PRIORITY)))
		return true;
	return false;
}



void DDStopSong(){
	playSong = false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************USER INTERFACE*************************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void UzeboxLogo(){

//return;
	//in original, there are 248(+1 or 2 after scrolling is done?) before title is ready to push button. Immediately ready(after a few frames) to go if player pushes start

//	SetTileTable(88);
//screen_tile_bank = 11;
	uint16_t moff = 2;
	DDClearVram();
	for(uint8_t y=0;y<3;y++)
		for(uint8_t x=0;x<9;x++)
			SetTile(x+10,y+11,pgm_read_byte(&ulogo_map[moff++]));

	p_x = 0;
	p_y = 88;
	uint8_t bpushes = 0;
	DDWaitVsync(1);
	StartDemo(0);
	while(gameState & GAME_DEMO_PLAYING){

		ResetSprites();
		UpdatePlayer(1);
		for(uint8_t i=0;i<12;i++)//have to modify the sprite y-coords to look right, but changing player coordinates directly would screw up PlayerUpdate()
			sprites[i].y += 4;
		
		if(padState & BTN_B && !oldPadState & BTN_B){
			DDTriggerFx(SFX_PUMP);
			bpushes++;
		}

		if(bpushes){
			//if(bpushes > 1)
			//p_state |= PLAYER_PUMPING;//lock to pumping state to fool PlayerUpdate, since he didn't really hit an enemy
			moff = (bpushes*3*9)+2;
			for(uint8_t y=0;y<3;y++)
				for(uint8_t x=0;x<9;x++)
					SetTile(x+10,y+11,pgm_read_byte(&ulogo_map[moff++]));
		}
		DDWaitVsync(1);
	}
	DDTriggerFx(SFX_UZEBOX_KLING1);
	DDWaitVsync(60);

}



void DDSetVram(uint8_t x, uint8_t y, uint8_t v){
	vram[(y*VRAM_TILES_H)+x] = v;
}

uint8_t DDGetVram(uint8_t x, uint8_t y){
	return 0;
}

void DDClearVram(){
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = 128;
}

void DrawGUIBG(){
return;
	uint8_t moff = 0;
	for(uint8_t y=0;y<0+6;y++)
		for(uint8_t x=5;x<5+20;x++)
			SetTile(x,y,pgm_read_byte(&title_map[moff++])+51);
	
	for(uint16_t i=0;i<RAM_TILES_COUNT*RAM_TILE_BYTE_COUNT;i++)//load up the artwork for the enemies at either side
		ram_tiles[i] = pgm_read_byte(&enemy_panel_tiles[i]);
	
	moff = 0;
	for(uint8_t y=SCREEN_TILES_V-5;y<SCREEN_TILES_V;y++)//map tiles are adjusted for the removed tile 6(or 5 as 0..5), which was a repeat black
		for(uint8_t x=0;x<SCREEN_TILES_H;x++){
			uint8_t t = pgm_read_byte(&enemy_panel_map[moff++]);
			if(t >= RAM_TILES_COUNT)
				continue;
			DDSetVram(x,y,t);
	}
}

const uint8_t KonamiCode[] PROGMEM = {BTN_UP,BTN_UP,BTN_DOWN,BTN_DOWN,BTN_LEFT,BTN_RIGHT,BTN_LEFT,BTN_RIGHT,BTN_B,BTN_Y};//BTN_Y = BTN_A, see VsyncRoutine()
void TitleScreen(){

//return;

	uint8_t codepos = 0;

	FadeOut(4,true);
	ResetSprites();

//	SetTileTable(title_tiles-(51*64));
	SetTileTable(title_tiles);//tiles+(128*64));

//	for(uint16_t i=0;i<VRAM_SIZE;i++)
//		vram[i] = RAM_TILES_COUNT;

	DrawGUIBG();

	SetFontTilesIndex(0);//relying on wrap around behavior to make the kernel functions work here
	DDPrint(11,16,PSTR("1 PLAYER"));
	DDPrint(11,18,PSTR("2 PLAYERS"));
	DDPrint(8,24,PSTR("LEE WEBER 2017"));
	DDPrint(12,25,PSTR("GPL 3;0"));
	DDWaitVsync(1);
	FadeIn(2,false);


//padState = BTN_START;

	uint8_t cursorpos = 0;
	uint16_t idle_ticks = 0;
	uint8_t pads_connected;
	while(1){
		for(uint8_t i=9;i<VRAM_TILES_H-9;i++)
			SetTile(i,21,51);//erase any controller error messages

		pads_connected = DetectControllers();

		if(false){//(pads_connected & 0b11) != 0b01){//bad or no controller in port 1!
			if(idle_ticks&0b100000)
				DDPrint(2,21,PSTR("PLEASE CONNECT CONTROLLER"));
		}else{//good controller in port 1
			if(padState && !oldPadState){
				if(padState == pgm_read_word(&KonamiCode[codepos])){
					if(++codepos ==(sizeof(KonamiCode)/2)){
						FadeIn(1,false);
						codepos = 0;
						gameState ^= GAME_CHEAT_USED;
						DDTriggerFx((gameState&GAME_CHEAT_USED)?SFX_GRAB_BONUS:SFX_ROCK_DROP);
					}
				}else
					codepos = 0;
			}

			if(padState & BTN_SELECT && !(oldPadState & BTN_SELECT))
				cursorpos = !cursorpos;

			
			SetTile(10,16+(cursorpos*2),50);//cursor tiles
			SetTile(10,18-(cursorpos*2),51);//extra black tile in title tiles

			if(false){//cursorpos && (pads_connected>>2) != 0b01){//is there is a controller connected to port 2

				if(pads_connected>>2 == 0b00){
					if(idle_ticks&0b100000)
						DDPrint(4,21,PSTR("CONNECT P2 CONTROLLER"));
				}else{
					if(idle_ticks&0b100000)
						DDPrint(2,21,PSTR("UNSUPPORTED P2 CONTROLLER"));
				}
			}
			if(padState & BTN_START && !(oldPadState & BTN_START)){
				if(false){//cursorpos && (pads_connected>>2) != 0b01)
					DDTriggerFx(SFX_FYGAR_FLAME);
				}else{
					gameState = 0;
					if(cursorpos)
						gameState |= GAME_IS_2_PLAYER;
					
					DDTriggerFx(SFX_HARPOON_SHOT);
					FadeOut(3,false);
					uint8_t flash = 0;
					while(DDRC){
						flash++;
						SetTile(10,cursorpos?18:16,(flash&0b1000)?51:50);
						DDWaitVsync(1);
					}
				return;
				}
			}
		}

		if(++idle_ticks > TITLE_SCREEN_TIMEOUT){//time to do demo?
			ShowHighScoreScreen(0);
			idle_ticks = 0;
			//return 0;//show demo, then high score
		}
		DDWaitVsync(1);
	}

}



bool ScrollScoreDataDown(){
	bool cancelled;
	for(uint8_t y=6;y<SCREEN_TILES_V-1;y++){
		for(uint8_t x=0;x<24;x++){
			DDSetVram(x,y+1,DDGetVram(x,y));//vram[(y*VRAM_TILES_H)+x+VRAM_TILES_H] = vram[(y*VRAM_TILES_H)+x];
			DDSetVram(x,y,RAM_TILES_COUNT);//vram[(y*VRAM_TILES_H)+x] = RAM_TILES_COUNT;
		}
		cancelled = DDWaitVsyncWithCancel(2);
		if(cancelled)
			break;
	}
	for(uint8_t x=0;x<24;x++)
		DDSetVram(x,(SCREEN_TILES_V-1),RAM_TILES_COUNT);//vram[((SCREEN_TILES_V-1)*VRAM_TILES_H)+x] = RAM_TILES_COUNT;
	return cancelled;
}


uint8_t ShowHighScoreScreen(uint8_t shortcircuit){
//TODO THIS FUNCTION TAKES ALMOST 2K....IS IT REALLY SMALLER TO DO IT THIS WAY??!?!?!

	uint8_t place = DD_MAX_HIGH_SCORES;
	if(!shortcircuit)
		FadeOut(4,true);
	
	ResetSprites();
	DDWaitVsync(1);
	
	//SetTileTable(title_tiles-(51*64));
	SetTileTable(title_tiles);//tiles+(128*64));
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = RAM_TILES_COUNT+51;


	DrawGUIBG();

	SetFontTilesIndex(0);//relying on wrap around behavior to make the kernel functions work here
	DDPrint(9,6,PSTR("HALL OF FAME"));
	UpdateEEPROM(0);

	uint16_t rt_base = (RAM_TILES_COUNT-DD_MAX_HIGH_SCORES)*RAM_TILE_BYTE_COUNT;
	if(shortcircuit){//called from high score entry screen, insert the score into the existing and sort before we exit
		for(uint16_t i=rt_base+30;i<rt_base+35;i++)
			ram_tiles[i] = 0;//clear out any previous pixel data from BlitSprite() here, sprites are already hidden and wont corrupt it

		uint32_t score = p_score;
		ram_tiles[rt_base+30+4] = (score>>8)&0xFF;
		ram_tiles[rt_base+30+5] = (score>>0)&0xFF;
		score >>= 16UL;//get top 3 significant bits(discard anything more, unlikely player can reach this without cheating)
		if(score & 0b001)
			ram_tiles[rt_base+30+2] = 128;
		if(score & 0b010)
			ram_tiles[rt_base+30+1] = 128;
		if(score & 0b100)
			ram_tiles[rt_base+30+0] = 128;
				
		bool moved = false;
		uint32_t score2 = 0;
		do{
			for(uint8_t i=0;i<6;i++){
				score = 0;
				if(ram_tiles[(i*5)+0]&128)
					score |= 0b1000000000000000000;
				if(ram_tiles[(i*5)+1]&128)
					score |= 0b0100000000000000000;
				if(ram_tiles[(i*5)+2]&128)
					score |= 0b0010000000000000000;
				score |= (ram_tiles[(i*5)+3]<<8);
				score |= (ram_tiles[(i*5)+4]<<0);

				if(ram_tiles[((i+1)*5)+0]&128)
					score2 |= 0b1000000000000000000;
				if(ram_tiles[((i+1)*5)+1]&128)
					score2 |= 0b0100000000000000000;
				if(ram_tiles[((i+1)*5)+2]&128)
					score2 |= 0b0010000000000000000;
				score2 |= (ram_tiles[((i+1)*5)+3]<<8);
				score2 |= (ram_tiles[((i+1)*5)+4]<<0);

				if(score2 > score){
					moved = true;
					uint8_t t;
					for(uint8_t j=0;j<5;j++){//swap entries
						t = ram_tiles[(i*5)+j];
						ram_tiles[(i*5)+j] = ram_tiles[((i+1)*5)+j];
						ram_tiles[((i+1)*5)+j] = t;
					}
				}
				place--;//makes the assumption EEPROM data was previously sorted
			}
		}while(moved);
	}

	for(uint8_t i=0;i<8;i++){


		SetFont(8,9+(i<<1),(unsigned char)('1'+215+i));//screw around with rollover due to screwy font, screw it
		SetFont(9,9+(i<<1),(unsigned char)(';'+215));
		SetFont(10,9+(i<<1),(ram_tiles[rt_base+(i*16)+0]&127)+215);
		SetFont(11,9+(i<<1),(ram_tiles[rt_base+(i*16)+1]&127)+215);
		SetFont(12,9+(i<<1),(ram_tiles[rt_base+(i*16)+2]&127)+215);
		SetFont(21,9+(i<<1),(unsigned char)('0'+215));//'0', all saved scores are 1/10 in game score to save space, least significant digit is always zero
		uint32_t total_score =
			(ram_tiles[rt_base+(i*5)+3]<<8) +
			(ram_tiles[rt_base+(i*5)+4]<<0);

		if((ram_tiles[rt_base+(i*5)+2]&128))
			(uint32_t)(total_score |= 0b0010000000000000000UL);
		if((ram_tiles[rt_base+(i*5)+1]&128))
			(uint32_t)(total_score |= 0b0100000000000000000UL);
		if((ram_tiles[rt_base+(i*5)+0]&128))
			(uint32_t)(total_score |= 0b1000000000000000000UL);

		DDPrintInt(20,9+(i<<1),total_score);

	}
	
	if(shortcircuit)//user is going to enter a name on top of what we have drawn, save code space
		return place;
	FadeIn(2,false);
	
	for(uint16_t i=0;i<60*9;i++){
		DDWaitVsync(1);
		if(padState & BTN_START && !(oldPadState & BTN_START))
			break;
	}

	return 0;
}



void HighScoreEntryScreen(){
	FadeOut(2,true);
	if(ShowHighScoreScreen(1) > 5){//internally the score was compared to existing, if user didn't place in the top spots, ignore
		
		return;
	}
	DDPrint(9,9,PSTR("CONGRATULATIONS! YOU HAVE"));
	DDPrint(9,9,PSTR("ENTERED THE HALL OF FAME!"));

	if(net_state != NET_AWAIT_USER){//network subsystem has failed, no module, not properly configured, or otherwise no internet access
		DDPrint(9,10,PSTR("INSTALL AN UZENET MODULE"));
		DDPrint(9,11,PSTR("FOR WORLD RECORDS ACCESS"));
	}else{

	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************NETWORK HIGH SCORES********************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void NetworkStateMachine(){
return;
	//this runs independently of the main program, and stores whether it has valid results to show at the high score screen(otherwise world records is skipped)
	//this is designed to use the least amount of ram, and only use cycles that would otherwise be wasted when a DDWaitVsync() is called
	if(net_delay || net_state <= NET_AWAIT_USER)
		return;

	if(net_state == NET_FIRST_TICK){//reset module, and wait for a long enough time that it should have connected to wifi(if setup properly)
	
		//UBRR0H=0;									//set up UART
		UBRR0L = 185;								//9600 baud	960 bytes/s	16 bytes/field
		UCSR0A = (1<<U2X0);							//double speed mode
		UCSR0C = (1<<UCSZ01)+(1<<UCSZ00)+(0<<USBS0);//8-bit frame, no parity, 1 stop bit
		UCSR0B = (1<<RXEN0)+(1<<TXEN0); 			//enable TX/RX

		DDRD |= (1<<PD3);//hold the module in reset
		PORTD &= ~(1<<PD3);
		net_state = NET_START_MODULE;
		net_delay = 1;//hold reset for a bit

	}else if(net_state == NET_START_MODULE){

		PORTD |= (1<<PD3);//let the module boot
		net_state = NET_SEND_REQUEST; 
		net_delay = 12*60;//should be long enough for boot,default connect in single mode to uzebox.net(if configured and network access)
		net_pos = 0;

	}else if(net_state == NET_SEND_REQUEST){//send anonymous login, and score request in 1 stream(server can deal with it, and re-logins as well)
		
		if(!UartUnreadCount())//should have booted up and connected by now. we cannot fix this problem here
			net_state = NET_STAY_OFF;
		else if(!IsUartTxBufferFull()){

			uint8_t t = pgm_read_byte(&net_request_string[net_pos++]);
			if(!t){//done sending string
				InitUartRxBuffer();
				net_state = NET_AWAIT_USER;
				SetLedOn();
			}else
				UartSendChar(t);
		}
		//else try again later, with our tiny TX buffer!
	}

}


