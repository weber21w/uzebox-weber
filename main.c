#include <avr/pgmspace.h>
#include <string.h>
#include <avr/interrupt.h>


#include <defines.h>
#include <uzebox.h>
#include <stdint.h>
#include <petitfatfs/pff.h>

#include "data/sound/patches.inc"
#include "data/sound/music.inc"
#include "data/graphics/tiles.inc"
#include "data/graphics/sprites.inc"
#include "aedefines.h"
#include "gui.h"

inline void AEClearVram(){for(uint16_t i=0;i<VRAM_SIZE;i++){vram[i] = FNTSTRT;}}

inline uint8_t CheckMap(uint8_t x,uint8_t y){
	return pgm_read_byte(&tileType[vram[((y>>3)*VRAM_TILES_H)+(x>>3)]-RAM_TILES_COUNT]);
}


void TakeItem(uint8_t x,uint8_t y){
	part_x  [part_ptr]=x;
	part_y  [part_ptr]=y;

	part_cnt[part_ptr]=16;
	x>>=3;
	y>>=3;
	items_cnt--;

	vram[(y*VRAM_TILES_H)+x] = FNTSTRT;

	uint16_t voff = (y*VRAM_TILES_H)+x;

	for(uint8_t i=0;i<update_list_len;i+=3){//scan through and find the item we hit
		if((update_list[i+0] == (uint16_t)(voff>>8)) && (update_list[i+1] == (uint16_t)(voff&0xFF))){
			part_spr[part_ptr]=update_list[i+2];
			update_list[i+0]=255;//disable it from updating
			break;
		}
	}

	if((++part_ptr) >= PART_MAX)
		part_ptr = 1;
}



void DropBridge(){
	uint8_t xx = player_x1;
	uint8_t yy = player_y1+8;
	part_x  [0]=xx;
	part_y  [0]=yy;
	part_cnt[0]=8;
	part_spr[0] = (pgm_read_byte(&BridgeSpriteType[map_tile_set]));
	xx>>=3;
	yy>>=3;

	//if((prng() & 9) == 1)
	//	vram[(yy*VRAM_TILES_H)+xx] = RAM_TILES_COUNT+FIRST_STAR_TILE+((map_frame&0b00010000)?8:0);//details...put a star here, the only one that could have been hidden by the bridge...
	//else
		vram[(yy*VRAM_TILES_H)+xx] = FNTSTRT;
	AETriggerFx(SFX_BRIDGE);
}



void UpdateStats(uint8_t show){
	if(demo_state == DEMO_ON){
		for(uint8_t blx=0;blx<SCREEN_TILES_H;blx++){
			vram[(0*VRAM_TILES_H)+blx] = FNTSTRT;
			vram[(1*VRAM_TILES_H)+blx] = FNTSTRT;
		}
		if(frame_cnt & 16){
			if(frame_cnt & 32)
				AEPrintBigRam(11,0,"DEMO MODE?");
			else
				AEPrintBigRam(10,0,"PRESS START?");
		}
	}else if(!show){
		for(uint8_t blx=0;blx<9;blx++){
			vram[(0*VRAM_TILES_H)+blx] = FNTSTRT;
			vram[(1*VRAM_TILES_H)+blx] = FNTSTRT;
		}
	}else{
		for(uint8_t blx=0;blx<9;blx++){
			vram[(0*VRAM_TILES_H)+blx] = FNTSTRT;
			vram[(1*VRAM_TILES_H)+blx] = FNTSTRT;
			vram[(2*VRAM_TILES_H)+blx] = FNTSTRT;
		}
		//can't exactly replicate offsets without using sprites. not worth it
		AEPrintByte(3,VRAM_FIRST_MAP_Y-3,level+1,false);//AEPrintByte(3,VRAM_FIRST_MAP_Y-3,restart,false);
		AEPrintByte(6+1,VRAM_FIRST_MAP_Y-3,exchange,false);
		if(show > 1)
			vram[6+1+((VRAM_FIRST_MAP_Y-3)*VRAM_TILES_H)] = FNTSTRT;

		vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-4))+1] = FNTSTRT+11;//vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-4))+1] = FNTSTRT+68;//active head
		vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-4))+4+1] = FNTSTRT+66;//inactive head
		vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-3))+1] = FNTSTRT+16;//vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-3))+1] = FNTSTRT+70;//active body
		vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-3))+4+1] = FNTSTRT+67;//inactive body
		vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-3))+2] = FNTSTRT+55;//x
		vram[(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-3))+5+1] = FNTSTRT+55;//x
	}
}

void GameAddBackground(bool showname){
	//draw screen borders
	LoadLevel(showname);

	for(uint16_t rtoff=0;rtoff<((RAM_TILES_COUNT*64)-SD_DEMO_SIZE);rtoff++){ram_tiles[rtoff]=0;}//don't clobber demo data
	
	uint16_t voff=VRAM_FIRST_MAP_Y*VRAM_TILES_H;
	for(uint8_t yy=0;yy<20;yy++)
	for(uint8_t xx=0;xx<30;xx++){
		if(vram[voff] && vram[voff++] < FNTSTRT)//it is some sort of tile, dont draw a star
			ram_tiles[(yy*32)+xx+1] = 255;
	}
	for(uint16_t vv=VRAM_FIRST_MAP_Y*VRAM_TILES_H;vv<VRAM_SIZE-VRAM_TILES_H;vv++){
		if((vram[vv] == FNTSTRT) && (prng()%256) > 162)
			vram[vv] = RAM_TILES_COUNT+FIRST_STAR_TILE+prng()%8;
	}
	//Draw border
	for(uint8_t ii=0;ii<30;ii++){
		uint8_t tt = pgm_read_byte(&BorderHTiles[ii]);
		vram[((VRAM_FIRST_MAP_Y-1 )*VRAM_TILES_H)+ii] = tt;
		vram[((VRAM_FIRST_MAP_Y+20)*VRAM_TILES_H)+ii] = tt;
	}
	for(uint8_t ii=VRAM_FIRST_MAP_Y;ii<VRAM_FIRST_MAP_Y+20;ii++){
		uint8_t tt = pgm_read_byte(&BorderVTiles[ii-VRAM_FIRST_MAP_Y]);
		vram[(ii*VRAM_TILES_H)+VRAM_FIRST_MAP_X-1 ] = tt;
		vram[(ii*VRAM_TILES_H)+VRAM_FIRST_MAP_X+28] = tt;
	}
}

void DrawSprites(){
	ResetSprites();
	uint8_t flipbit = 0;

	if(!start_delay||(start_delay&8)){
	//active ego
		uint8_t st = 0;
		if(player_idle_cnt == PLAYER_IDLE_MAX && player_spr != PLAYER_SPR_LADDER && player_spr != PLAYER_SPR_FALL){//must be idle and not on a ladder and not falling
			if(!player_blink_cnt){//done blinking?
				player_blink_cnt = (uint16_t)(prng()%PLAYER_MAX_BLINK_WAIT);
			//	if(player_blink_cnt < (uint16_t)PLAYER_MIN_BLINK_WAIT && (uint8_t)(prng()&0xFF)>14)//rarely, blink back to back..some life
			//		player_blink_cnt = PLAYER_MIN_BLINK_WAIT;
			}else{
				if(player_blink_cnt < 4)
					st = 7;
				player_blink_cnt--;
			}
		}else{//could be on a ladder or falling
			if((player_spr == PLAYER_SPR_LADDER || player_spr == PLAYER_SPR_FALL) && ((player_y1 & 4)))
				flipbit = true;
//((player_state & DIR_RIGHT) || ((player_spr == PLAYER_SPR_LADDER || player_spr == PLAYER_SPR_FALL) && (player_y1 & 4)))?SPRITE_FLIP_X:0

		}
		if((player_state & DIR_RIGHT) /*&& (true)  && (player_spr == PLAYER_SPR_IDLE || player_spr == PLAYER_SPR_WALK)*/){flipbit^=1;}

		uint8_t fi = (level_clear_delay?0:prng());
		DrawMetaSprite(player_x1,player_y1+(VRAM_FIRST_MAP_Y*0),player_spr+st,flipbit);
		DrawMetaSprite(player_x2,player_y2+(VRAM_FIRST_MAP_Y*0),(PLAYER_SPR_ALTER+(fi&5)),0);
	}

	for(uint8_t i=0;i<ENEMY_MAX;i++){
		if(enemy_state[i] == DIR_NONE)
			break;
		uint8_t spr=((((frame_cnt>>4)&3)+i)%4)+(8*((enemy_state[i]&ALTER_SKULL)>>7));
		switch((enemy_state[i] & (DIR_UP|DIR_DOWN|DIR_LEFT|DIR_RIGHT))){
		case DIR_LEFT:case DIR_RIGHT:
			DrawMetaSprite(enemy_x[i],enemy_y[i],14+spr,(enemy_state[i]&DIR_LEFT)?SPRITE_FLIP_X:0);
		break;
		case DIR_UP:case DIR_DOWN:
			DrawMetaSprite(enemy_x[i],enemy_y[i]+(VRAM_FIRST_MAP_Y*0),18+spr,0);
		break;
		}
	}

	//particles animation
	for(uint8_t i=0;i<PART_MAX;i++){
		if(part_y[i] == 255)
			continue;
		sprites[spritenum].x = part_x[i];
		sprites[spritenum].y = part_y[i];
		sprites[spritenum].flags = 0;
		if(!i)//bridge
			sprites[spritenum].tileIndex = part_spr[i];
		else{//item
			sprites[spritenum].tileIndex = pgm_read_byte(&ItemSpriteAnim[((part_spr[i]&3)*8)+(part_spr[i]>>4)]);

			if((frame_cnt&5) == 2 || (frame_cnt&5) == 4 || (frame_cnt&7) == 7){
				if((part_spr[i]>>4)<7)
					part_spr[i] += 1<<4;
				else
					part_spr[i] ^= 7<<4;
				}
		}
		spritenum++;

		if(part_cnt[i]){
			if(i == 0)//bridge
				part_y[i]++;
			else//item
				part_y[i] -= 2;

			part_cnt[i]--;
			if(!part_cnt[i]) part_y[i]=255;//hide particle
		}
	}
}

void AnimateLevel(){
	//this function is only called on frames that enemies dont do collision checks to save cycles
	map_frame++;
	if((map_frame&1)){//animate items/pixels every other frame
		for(uint8_t ioff=0;ioff<(ITEM_MAX*3);ioff+=3){
			if(update_list[ioff+0] == 255)
				continue;
			uint8_t iframe = (update_list[ioff+2]&3)*8;//item #
			iframe += (update_list[ioff+2]>>4);//frame

			iframe = pgm_read_byte(&ItemTileAnim[iframe]);
			vram[(uint16_t)(((update_list[ioff+0]<<8))+update_list[ioff+1])] = iframe;

			if((update_list[ioff+2]>>4)<7)
				update_list[ioff+2] += 1<<4;
			else
				update_list[ioff+2] ^= 7<<4;
		}
	}

	if((map_frame&3)==1){//animate alter blocks 1 every 4 frames
		for(uint16_t voff=VRAM_FIRST_MAP_Y*VRAM_TILES_H;voff<VRAM_SIZE-VRAM_TILES_H;voff++){
			if(vram[voff] >= FIRST_ALTERBLOCK_TILE+RAM_TILES_COUNT && vram[voff] <= FIRST_ALTERBLOCK_TILE+RAM_TILES_COUNT+6){
				if(vram[voff] < FIRST_ALTERBLOCK_TILE+RAM_TILES_COUNT+5)
					vram[voff]++;
				else
					vram[voff] = FIRST_ALTERBLOCK_TILE+RAM_TILES_COUNT;
			}
		}		
	}else if((map_frame&0x0F) == 15){//(map_frame&0x0F) == 0x0F){//animate stars ORIGINAL WATER AND STAR FRAME IS 19 ticks long
		uint8_t stardir = (map_frame&0b00010000);
		for(uint16_t voff=(VRAM_FIRST_MAP_Y*VRAM_TILES_H);voff<(VRAM_SIZE-VRAM_TILES_H);voff++){
			if(vram[voff] < FIRST_ALTERBLOCK_TILE+RAM_TILES_COUNT){//is stars or water
				if(vram[voff] < FIRST_WATER_TILE+RAM_TILES_COUNT){//is stars
					if(stardir){vram[voff]-=8;}else{vram[voff]+=8;}
				}
			}
		}
	}else if((map_frame&0x0F) == 8){//do water on different ticks, just to be safe on cycles
		uint8_t watrdir = (map_frame&0b00010000);
		for(uint16_t voff=(VRAM_FIRST_MAP_Y*VRAM_TILES_H);voff<(VRAM_SIZE-VRAM_TILES_H);voff++){
			if(vram[voff] < FIRST_ALTERBLOCK_TILE+RAM_TILES_COUNT){//is stars or water
				if(vram[voff] >= FIRST_WATER_TILE+RAM_TILES_COUNT){//is water
					if(watrdir){vram[voff] -= 3;}else{vram[voff] += 3;}		
				}
			}
		}
	}
}




inline void NewPlayerMove(){
//AETriggerFx(1,255);
	bool canmove = true;
	if((!(player_tile_bottom&TILE_FLOOR))&&(!(player_tile&TILE_LADDER))){
		bool movepass = true;
		if(!player_sync_type){//horizontal
			if(CheckMap(player_x2,player_y2+8)&TILE_ALTER)
				movepass = false;
		}else{//vertical /////TODO NOT RIGHT??
			if(CheckMap(player_x2,player_y2-16)&TILE_ALTER)
				movepass = false;
		}
		if(movepass){//|DIR_LEFT|DIR_RIGHT
			player_state = ((player_state & (SWAP_PENDING))|(DIR_DOWN|DIR_MOVING));//	player_dir=DIR_DOWN; IS SWAP PENDING POSSIBLE?
			player_move_cnt=8;
			player_idle_cnt=PLAYER_IDLE_MAX-1;
			player_spr=PLAYER_SPR_FALL;
			canmove = false;//no control available mid-air
		}
	}

	if((padstate & BTN_LEFT) && canmove){
		bool movepass = true;
		if(!player_sync_type){//horizontal
			if(CheckMap(player_x2+8,player_y2)&TILE_ALTER)
				movepass = false;
		}else{//vertical
			if(CheckMap(player_x2-8,player_y2)&TILE_ALTER)
				movepass = false;
		}

		if(movepass){
			if(!(CheckMap(player_x1-8,player_y1)&TILE_WALL)){
				player_state = (player_state & SWAP_PENDING) | (DIR_LEFT|DIR_MOVING);//player_dir=DIR_LEFT; IS SWAP POSSIBLE HERE?
				goto MOVEPASS;//saves 24 bytes, I love goto :)
			}
		}
	}else if((padstate & BTN_RIGHT) && canmove){
		bool movepass = true;
		if(!player_sync_type){//horizontal
			if(CheckMap(player_x2-8,player_y2)&TILE_ALTER)
				movepass = false;
		}else{//vertical
			if(CheckMap(player_x2+8,player_y2)&TILE_ALTER)
				movepass = false;
		}
		if(movepass){
			if(!(CheckMap(player_x1+8,player_y1)&TILE_WALL)){
				player_state = DIR_RIGHT|DIR_MOVING;
MOVEPASS://LEFT case from above jumps here
				player_move_cnt=8;
				player_idle_cnt=0;
				AETriggerFx((prng()&1)?SFX_WALK2:SFX_WALK1);

				if(player_tile_bottom&TILE_BRIDGE) DropBridge();
			}
		}
	}

	if(player_tile&TILE_LADDER){
		if((padstate & BTN_UP)){
			bool movepass = true;
			if(!player_sync_type){//horizontal
				if(CheckMap(player_x2,player_y2-16)&TILE_ALTER)
					movepass = false;
			}else{//vertical
				if(CheckMap(player_x2,player_y2+8)&TILE_ALTER)
					movepass = false;
			}
			if(movepass){
				if(CheckMap(player_x1,player_y1-8)&TILE_LADDER){
					player_state = (DIR_UP|DIR_MOVING);
					player_move_cnt = 8;
					player_idle_cnt = 0;
					player_spr = PLAYER_SPR_LADDER;
					AETriggerFx(SFX_LADDER);
				}
			}
		}
	}

	if((player_tile&TILE_LADDER)||(player_tile_bottom&TILE_LADDER)){
		if((padstate & BTN_DOWN)){// && !(oldpadstate & BTN_DOWN)))
			if(!(player_tile_bottom&(TILE_WALL|TILE_BRIDGE))){
				player_state = (DIR_DOWN|DIR_MOVING);
				player_move_cnt=8;
				player_idle_cnt=0;
				player_spr=PLAYER_SPR_LADDER;
				AETriggerFx(SFX_LADDER);
			}
		}
	}

	if(!(player_state & (DIR_MOVING|SWAP_PENDING)))//TODO SWAP_PENDING NECESSARY?
		if(player_idle_cnt<PLAYER_IDLE_MAX)
			player_idle_cnt++;
}

void Render(){
	if(exchange_flash_cnt || demo_state == DEMO_ON){
		if(!(--exchange_flash_cnt)){UpdateStats(1);}
		else if((exchange_flash_cnt & 16)){UpdateStats(2);}
		else{UpdateStats(1);}
	}

	//sync alter ego
	if(player_exchange != 1){
		if(!player_sync_type){//horizontal
			player_x2=232-player_x1;//player_x2=248-player_x1;
			player_y2=player_y1;
			player_x2=MAX(8 ,player_x2);//not needed?
			player_x2=MIN(224,player_x2);//player_x2=MIN(240,player_x2);
		}else{//vertical
			player_x2=player_x1;
			player_y2=208-player_y1;//player_y2=224-player_y1;//...WAS 208
			player_y2=MAX(32 ,player_y2);//player_y2=MAX(48 ,player_y2);//WAS 32
			player_y2=MIN(208,player_y2);//player_y2=MIN(208,player_y2);//WAS 208
		}
	}
	if(player_flash_cnt){player_flash_cnt--;}
	DrawSprites();
	AnimateLevel();
	WaitVsync(1);
}
//uint8_t demotoplay = 0;//TODO
inline void GameLoop(){
	WaitVsync(1);
	scrambler_state = SCRAMBLER_OFF;
	ResetSprites();
	start_delay = PLAYER_START_DELAY;

	if(demo_state == DEMO_PENDING){
		uint8_t dt;
		do{
			dt = (prng()%num_demos);
		}while(dt == demo_prev);
		demo_prev = dt;

		FillDemoBuffer(dt);//demotoplay);//prng()%num_demos);
		demo_state = DEMO_ON;
		scrambler_state = SCRAMBLER_FOREGROUND|SCRAMBLER_SKULL_EYES;
	}

	GameAddBackground(1);
	FadeIn(2,false);

	player_move_cnt=8;
	player_spr=PLAYER_SPR_IDLE;
	player_spr_prev=0;
	player_idle_cnt=0;
	player_exchange=0;
//	player_step=0;
	player_flash_cnt=0;
	level_done = DONE_NOTYET;
	level_clear_delay=0;
	frame_cnt=0;

	UpdateStats(1);
#if DEVMODE == 1
	record_first_tick = 1;
#endif
	//main game logic loop
	while(!level_done){
		Render();
#if DEVMODE == 1
	if(record_first_tick){
		demo_record_pos = DEMO_RT_OFF+1;
		demo_pad_ticks = 0;
		if(start_delay == 0)
			record_first_tick = 0;
	}

#endif
		if(player_idle_cnt == PLAYER_IDLE_MAX){
		//	if(player_spr==PLAYER_SPR_FALL)//   player_state=player_state_dir; //TODO DO WE NEED THIS? 
			if(player_spr != PLAYER_SPR_LADDER){
				if(player_spr != PLAYER_SPR_IDLE){
					player_spr = PLAYER_SPR_IDLE;
					AETriggerFx(SFX_WALK2);
				}
			}
		}

		//enemy and player aren't moving at the start or end of a level
		if(start_delay){
			start_delay--;
			frame_cnt++;
			continue;
		}

		if(level_clear_delay){
			if(!(--level_clear_delay)){level_done=DONE_CLEAR;}
			continue;
		}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
		//check gamepad, do actions and game logic
		//UpdatePad();Done in VsyncRoutine now
		if(	(padstate & BTN_A && !(oldpadstate & BTN_A)) || (padstate & BTN_B && !(oldpadstate & BTN_B)) ||
			(padstate & BTN_X && !(oldpadstate & BTN_X)) || (padstate & BTN_Y && !(oldpadstate & BTN_Y))){
			swappending = true;//player_state |= SWAP_PENDING;
		}else if(level_skip && (padstate & BTN_SELECT && !(oldpadstate & BTN_SELECT))){
			level_clear_delay=PLAYER_CLEAR_DELAY;
			continue;
		}else if((padstate & BTN_START && !(oldpadstate & BTN_START))){//pause menu
			uint8_t ret = InGamePauseMenu();
			if(ret == 1){return;}
			else if(ret == 2){continue;}
		}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
		if(player_spr == PLAYER_SPR_FALL && player_spr_prev != PLAYER_SPR_FALL)
			AETriggerFx(SFX_FALLING);
		else if(player_spr != PLAYER_SPR_FALL && player_spr_prev == PLAYER_SPR_FALL){
			AETriggerFx(SFX_DROP);
		//TODO FIX LANDING PROBLEM WHERE SPRITE STICKS TO FALLING????
		}
		
		player_spr_prev = player_spr;

		if(player_exchange == 1){
			if(player_x1 < player_x1_to) player_x1 = MIN(player_x1_to,player_x1+4);
			else if(player_x1 > player_x1_to) player_x1 = MAX(player_x1_to,player_x1-4);
			if(player_y1 < player_y1_to) player_y1 = MIN(player_y1_to,player_y1+4);
			else if(player_y1 > player_y1_to) player_y1 = MAX(player_y1_to,player_y1-4);
			if(player_x2 < player_x2_to) player_x2 = MIN(player_x2_to,player_x2+4);
			else if(player_x2 > player_x2_to) player_x2 = MAX(player_x2_to,player_x2-4);
			if(player_y2 < player_y2_to) player_y2 = MIN(player_y2_to,player_y2+4);
			else if(player_y2 > player_y2_to) player_y2 = MAX(player_y2_to,player_y2-4);

			player_flash_cnt = 2;

			if(player_x1 == player_x1_to && player_y1 == player_y1_to && player_x2 == player_x2_to && player_y2 == player_y2_to){
				if(CheckMap(player_x1,player_y1) & TILE_LADDER){player_spr=PLAYER_SPR_LADDER;}//else{player_spr = PLAYER_SPR_WALK;}
				//COMMENTED OUT LAST ELSE CASE....TODO NO BUGS CAUSED?
				player_exchange=2;
			}
		}else{//not exchanging
			if(!(player_state & DIR_MOVING)){			//if(player_state == DIR_NONE){	//HACKS TO MAKE THIS WORK AGAIN!!!!
				player_tile=CheckMap(player_x1,player_y1);//old location
				player_tile_bottom=CheckMap(player_x1,player_y1+8);

				if(player_exchange!=1){//taking items has priority over exchange
					if(player_tile&TILE_ITEM1){
						TakeItem(player_x1,player_y1);
						AETriggerFx(SFX_ITEM1);
					}
					if(CheckMap(player_x2,player_y2)&TILE_ITEM2){
						TakeItem(player_x2,player_y2);
						AETriggerFx(SFX_ITEM2);
					}
					if(CheckMap(player_x1,player_y1)&TILE_ITEM3){
						TakeItem(player_x1,player_y1);
						AETriggerFx(SFX_EXCHANGE);//TODO DIFFERENT SOUND HERE?
						player_sync_type = !player_sync_type;
					}

					if(!items_cnt){
						level_clear_delay = PLAYER_CLEAR_DELAY;
						continue;
					}
				}

				if(swappending){
					swappending = false;
					if(!player_exchange){
						if(exchange){
							if((!(CheckMap(player_x2,player_y2)&TILE_WALL)) && (!(CheckMap(player_x1,player_y1)&TILE_ALTER))){
								player_x1_to=player_x2;
								player_y1_to=player_y2;
								player_x2_to=player_x1;
								player_y2_to=player_y1;
								exchange--;
								UpdateStats(1);
								AETriggerFx(SFX_EXCHANGE);
								player_exchange=1;
							}else{
								AETriggerFx(SFX_NO_EXCHANGE);
								player_exchange=2;
								player_flash_cnt=10;
							}
						}else{
							player_exchange=2;
							if(!exchange_flash_cnt){
								AETriggerFx(SFX_OUT_OF_EXCHANGES);
								exchange_flash_cnt = 90;
							}
						}
					}
				}else
					player_exchange=0;

				if(player_exchange!=1){
					if((player_tile&TILE_WATER) || (player_tile_bottom&TILE_WATER)){
						level_done = DONE_NOLUCK;
						continue;
					}

					NewPlayerMove();
				}
			}else{//if(player_state & DIR_MOVING){

				switch(player_state & (DIR_LEFT|DIR_RIGHT|DIR_UP|DIR_DOWN)){
				case DIR_UP:
					player_y1--;
					break;
				case DIR_DOWN:
					player_y1++;
					break;
					//CHANGED ORDER FROM LEFT RIGHT UP DOWN TO: UP DON LEFT RIGHT..BREAK ANYTHING? TODO
				case DIR_LEFT:
					player_x1--;
					player_spr=PLAYER_SPR_WALK+(3-((player_x1>>1)&3));
					player_state = (DIR_LEFT|DIR_MOVING);
					break;
				case DIR_RIGHT:
					player_x1++;
					player_spr=PLAYER_SPR_WALK+((player_x1>>1)&3);
					player_state = (DIR_RIGHT|DIR_MOVING);
					break;
				}

				if(!(--player_move_cnt)){
				//	if(player_y1 > 199) //TODO COMMENTED THIS OUT, THIS ISN'T NEEDED?	//		level_done=DONE_NOLUCK;
	player_state &= ~(DIR_MOVING);				//player_state = DIR_NONE;//HACKS TO MAKE IT WORK AGAIN
				}
			}
		}

		frame_cnt++;



		if(!(frame_cnt&1) && player_exchange != 1){enemy_move_cnt--;}

	//	if(!enemy_move_cnt){
		if(player_exchange != 1){
			for(uint8_t i=0;i<ENEMY_MAX;i++){
				if(enemy_state[i] == DIR_NONE)
					break;

				uint8_t ex=enemy_x[i];
				uint8_t ey=enemy_y[i];

				if(	( (enemy_state[i] & ALTER_SKULL) && (ey+7) >= player_y2 && ey < (player_y2+8) && ex >= (player_x2-1) && ex < (player_x2+1)) ||
					(!(enemy_state[i] & ALTER_SKULL) && (ey+7) >= player_y1 && ey < (player_y1+8) && ex >= (player_x1-1) && ex < (player_x1+1)))
						level_done = DONE_NOLUCK;

				if(frame_cnt&1)
					continue;
				switch((enemy_state[i] & (DIR_UP|DIR_DOWN|DIR_LEFT|DIR_RIGHT))){
					case DIR_LEFT:	enemy_x[i]--; break;
					case DIR_RIGHT:	enemy_x[i]++; break;
					case DIR_UP:	enemy_y[i]--; break;
					case DIR_DOWN:	enemy_y[i]++; break;
				}

				if(enemy_move_cnt)
					continue;

				switch((enemy_state[i] & (DIR_UP|DIR_DOWN|DIR_LEFT|DIR_RIGHT))){
				case DIR_LEFT:
					if((!(CheckMap(ex-8,ey+8)&TILE_FLOOR))||(CheckMap(ex-8,ey)&TILE_WALL)){
						enemy_state[i] ^= DIR_LEFT;
						enemy_state[i] |= DIR_RIGHT;
					}
					break;
				case DIR_RIGHT:
					if((!(CheckMap(ex+9,ey+8)&TILE_FLOOR))||(CheckMap(ex+9,ey)&TILE_WALL)){
						enemy_state[i] ^= DIR_RIGHT;
						enemy_state[i] |= DIR_LEFT;
					}
					break;
				case DIR_UP:
					if(CheckMap(ex,ey-8)&(TILE_WALL|TILE_BRIDGE|TILE_WATER)){
						enemy_state[i] ^= DIR_UP;
						enemy_state[i] |= DIR_DOWN;
					}
					break;
				case DIR_DOWN:
					if(CheckMap(ex,ey+9)&(TILE_WALL|TILE_BRIDGE|TILE_WATER)){
						enemy_state[i] ^= DIR_DOWN;
						enemy_state[i] |= DIR_UP;
					}
					break;
				}
			}
		}//surgery on logic flow...
		if(!(frame_cnt&1)&&!enemy_move_cnt){enemy_move_cnt=8;}
	}

	if(level_done == DONE_CLEAR){
		AETriggerFx(SFX_LEVEL_CLEAR);//	WaitVsync(1);
		UpdateStats(0);
		WaitVsync(1);

		for(uint8_t i=0;i<150-PLAYER_CLEAR_DELAY;i++){
			frame_cnt++;
			if((i&15) ==0){
				for(uint8_t blx=0;blx<SCREEN_TILES_H;blx++)
					vram[(0*VRAM_TILES_H)+blx] = vram[(1*VRAM_TILES_H)+blx] = FNTSTRT;
			}else if((i&15)==8)
				AEPrintBigRam(9,0,"LEVEL CLEAR?");
			Render();//WaitVsync(1);
		}

	}else if(level_done==DONE_NOLUCK){
		AETriggerFx(SFX_HIT);

		for(uint8_t di=0;di<50;di++){
			scrambler_state = SCRAMBLER_DEATH_PALLET|SCRAMBLER_SKULL_EYES|SCRAMBLER_FOREGROUND;
			Render();
			frame_cnt++;//needed to cycle colors
			//Pallet code handled in PostVsyncScrambler()
		}
		music_prev=255;
	}
	FadeOut(2,true);
}

void FillDemoPad(){//DebugBeep();
//AEPrintByte(5,SCREEN_TILES_V-1,demo_pad_ticks,1);
//AEPrintByte(14,SCREEN_TILES_V-1,demo_pos-DEMO_RT_OFF,1);
//AEPrintByte(22,SCREEN_TILES_V-1,ram_tiles+DEMO_RT_OFF+2,1);
	if(start_delay)
		return;
	if(demo_pad_ticks){
		demo_pad_ticks--;
		return;
	}
	padstate = ram_tiles[demo_pos++];//we only need the lower byte to represent d-pad and a swap button
	demo_pad_ticks = ram_tiles[demo_pos++];
//	AEPrintByte(4,SCREEN_TILES_V-1,(uint16_t)(demo_pos-DEMO_RT_OFF),1);
//	WaitVsync(60);
	if(padstate & BTN_START){//end demo
		level_done = DONE_DEMODONE;
	}
}

void FillDemoBuffer(uint8_t dmo_num){//get demo data from SD card, put it in a ram tile that hopefully wont get corrupted!
	ResetSprites();//loading will take several frames, we dont want sprite blitting corrupting our buffer
	scrambler_state = SCRAMBLER_OFF;//this will wreck it too!
	WaitVsync(1);//useless????????????????????????


	FRESULT res;
	uint32_t off;
	WORD br;
	WORD btr;

	off = (uint32_t)(episode_data_base_offset+(SD_FIRST_LEVEL_BYTE)+(num_levels*(SD_LEVEL_SIZE))+(dmo_num*SD_DEMO_SIZE));
	pf_lseek(off);
	btr = SD_DEMO_SIZE;
	res = pf_read((BYTE *)(ram_tiles+DEMO_RT_OFF),btr,&br);

//	WaitVsync(1);
	if(res || btr != br)
		SDCrash(5);
	demo_pad_ticks = 0;
	demo_pos = DEMO_RT_OFF;
	demo_state = DEMO_PENDING;
	level = ram_tiles[demo_pos++];
}


uint8_t RecordDemoPad(){
#if DEVMODE == 1
	scrambler_state = SCRAMBLER_OFF;

	//demo_pad_ticks++;
	if(padstate != oldpadstate || demo_pad_ticks == 255){
	if(demo_pad_ticks == 0)
		demo_pad_ticks = 1;
	//AETriggerFx(1);
		ram_tiles[demo_record_pos++] = oldpadstate;
		ram_tiles[demo_record_pos++] = demo_pad_ticks;
		ram_tiles[demo_record_pos+0] = BTN_START;//make sure  whenever we dump data BTN_START is there to end it correctly
		ram_tiles[demo_record_pos+1] = 64;
		demo_pad_ticks = 0;
	}else
		demo_pad_ticks++;

#endif
	return 0;
}


inline void UpdatePad(){
	oldpadstate = padstate;
	if(demo_state == DEMO_ON){
		if(ReadJoypad(0) & BTN_START)
			padstate = BTN_START;
		else
			FillDemoPad();//start is handled in InGamePauseMenu()
	//	if(counts++ == 255)
	//		InGamePauseMenu();//padstate = BTN_START;
	}else
		padstate = ReadJoypad(0);
#if DEVMODE == 1
	if(demo_record){
		RecordDemoPad();
		for(uint8_t xo=9;xo<SCREEN_TILES_H;xo++){vram[xo] = FNTSTRT;vram[xo+VRAM_TILES_H] = FNTSTRT;};
		AEPrintBigRam(10,0,"RECORDING?");
	//	AEPrintByte(0,SCREEN_TILES_V-1,demo_record_pos>>8,1);
	//	AEPrintByte(3,SCREEN_TILES_V-1,demo_record_pos,1);
	}
	if(padstate & BTN_SL && !(oldpadstate & BTN_SL)){//dump demo data to eeprom
		demo_record = 1;
		struct EepromBlockStruct ebs;
		uint16_t demoposition = DEMO_RT_OFF;
		for(uint8_t roff=0;roff<(SD_DEMO_SIZE/30)+1;roff++){
			ebs.id = AE_DEMORECORD_ID+roff;
			EepromReadBlock(ebs.id,&ebs);//it will exist
			for(uint8_t eoff=0;eoff<30;eoff++)
				ebs.data[eoff+0] = ram_tiles[demoposition++];
			EepromWriteBlock(&ebs);
		}
	}

#endif
}



uint8_t LevelProgress(uint8_t level_num){
	if(demo_state == DEMO_ON)
		return 0;
	struct EepromBlockStruct ebs;
	ebs.id = AE_EEPROM_ID;

	if(EepromReadBlock(ebs.id, &ebs) || level_num == 255){//doesn't exist, or we want to erase the data
		if(level_num == 255)
			ebs.data[episode_loaded] = 0;//just this episode
		else{
			for(uint8_t i=0;i<30;i++)
				ebs.data[i] = 0;//doesn't existsince we are making a brand new save, make sure progress for all episodes is formatted
		}
		EepromWriteBlock(&ebs);
	}
	
	if(level_num == 255)//we just wanted to erase, bail out
		return 0;

	if(EepromReadBlock(ebs.id, &ebs) == 0){//it exists
		if(level_num == 0)//not saving any data, just requesting the last level completed, or we just erased it
			return ebs.data[episode_loaded];
		else if(ebs.data[episode_loaded] < level_num && level_num < num_levels){
			ebs.data[episode_loaded] = level_num;
			EepromWriteBlock(&ebs);
		}
	}
	return 0;
}




void main(){
//level_skip=1;/////////////*************************************************TODO
RESTARTPROGRAM:
	Init();
//#if DEVMODE != 1 && false
	IntroScreen();
	CreditScreen();
//#endif
	while(1){
//SKIPSTUFF:
		TitleScreen();

		music_prev=255;
		restart = 5;
		if(demo_state == DEMO_OFF)
			LevelSelectScreen();

		while(restart != 255){
			GameLoop();

			switch(level_done){
			case DONE_DEMODONE:
				demo_state = DEMO_OFF;
				level_done = DONE_NOTYET;
				//oldpadstate = padstate = BTN_START;
				restart = 255;
				num_levels = 25;
				WaitVsync(1);
				//goto VERYTOP;
			break;
			case DONE_CLEAR:
				//level++;
				//if(level > 4 && !(level%5)){
				//	if(restart < 9)
				//		restart++;
					LevelProgress(++level);
				//}

				if(level == num_levels){
					restart=255;
					WellDoneScreen();
					goto RESTARTPROGRAM;
				}
				break;

			case DONE_RESTART:
			case DONE_NOLUCK:
				//restart--;
				//if(restart==255)
				//	GameOverScreen();
				break;
			}
		}//while(restart != 255)
	}//while(1)
}

void VsyncRoutine(){
	PostVsyncScrambler();
	UpdatePad();
}

inline void Init(){
	InitMusicPlayer(patches);
	SetTileTable((const char *)map_tiles);
	AEClearVram();
	ResetSprites();
	scrambler_state = SCRAMBLER_OFF;
	SetSpritesTileTable(sprite_tiles);
	SetUserPostVsyncCallback(&VsyncRoutine);
	SetMasterVolume(AE_MASTER_VOL);
	seedprng(0xACE1u);
//	seedprng(GetRandomSeed());
	SDSetup();
}

void SDCrash(uint8_t err){
	ResetSprites();
	StopSong();
	AEClearVram();
	AEPrintByte(20,12,err,0);
	AEPrintBigRam(11,11,"SD ERROR?");
	
	DDRC = 255;
	WaitVsync(180);
	SoftReset();
}


void ChangeEpisode(uint8_t ep){
	ResetSprites();//important, sprite blitting during SD read will ruin our buffered data
	scrambler_state = SCRAMBLER_OFF;
	FRESULT res;
	WORD	br;
	res	=	pf_lseek(SD_EPISODE_POINTERS+(ep*16UL));
	res	|=	pf_read(ram_tiles,4+1+1,&br);//get the 4 byte pointer to the start of the episode data, num levels, num demos
	if(res)
		SDCrash(4);
	episode_data_base_offset = (uint32_t)(((uint32_t)ram_tiles[0]<<24UL)|((uint32_t)ram_tiles[1]<<16UL)|((uint32_t)ram_tiles[2]<<8UL)|((uint32_t)ram_tiles[3]<<0UL));
	num_levels	= ram_tiles[4];
	num_demos	= ram_tiles[5];
	episode_loaded = ep;
}

void RamifyFromSD(uint8_t id, bool skipmap){
	ResetSprites();//important, sprite blitting during SD read will ruin our buffered data
	scrambler_state = SCRAMBLER_OFF;

	FRESULT res;

	uint32_t off = (uint32_t)(episode_data_base_offset+(id*16UL));
	uint32_t off2;
	WORD br;
	
	res	=		pf_lseek(off);
	res	|=		pf_read(ram_tiles,8,&br);
	off		=	(uint32_t)(episode_data_base_offset+((uint32_t)ram_tiles[0]<<24UL)+((uint32_t)ram_tiles[1]<<16UL)+((uint32_t)ram_tiles[2]<<8UL)+((uint32_t)ram_tiles[3]<<0UL));//tile graphics data
	off2	=	(uint32_t)(episode_data_base_offset+((uint32_t)ram_tiles[4]<<24UL)+((uint32_t)ram_tiles[5]<<16UL)+((uint32_t)ram_tiles[6]<<8UL)+((uint32_t)ram_tiles[7]<<0UL));//tile map data
	//WaitVsync(1);//ENABLING THIS DELAY CAUSES THE SAME GRAPHICAL PROBLEM EXHIBITED IN THE VIDEO....BUT WHY??!?
	res |=		pf_lseek(off);
	res |=		pf_read((BYTE *)ram_tiles,(RAM_TILES_COUNT*64),&br);

	if(!skipmap){
		res |=		pf_lseek(off2);
		res |=		pf_read((BYTE *)vram,(VRAM_TILES_H*SCREEN_TILES_V),&br);
	}
	if(res)
		SDCrash(3);

	//scrambler_state = SCRAMBLER_FOREGROUND|SCRAMBLER_SKULL_EYES;
}


void AEPrint(uint8_t x,uint8_t y,const uint8_t *string){
	uint8_t i=0;
	uint8_t c;

	while(1){
		c = pgm_read_byte(&(string[i++]));		
		if(c == 255)
			break;
		vram[(y*VRAM_TILES_H)+x++] = c;
	}
}

void AEPrintBigRam(uint8_t x,uint8_t y, char *string){
	uint8_t i=0;
	uint8_t c;
	uint8_t off=0;

	while(1){
		c=string[i++];
		if(c=='?')
			break;
		if(c == ' '){
	//	vram[((y+0)*VRAM_TILES_H)+x] = FNTSTRT;//for loop blanking takes less space...
	//	vram[((y+1)*VRAM_TILES_H)+x] = FNTSTRT;
			x++;
			continue;
		}
		c -= 'A';
		off = c*2;
		vram[((y+0)*VRAM_TILES_H)+x] = pgm_read_byte(&big_font_chars[off++]);
		vram[((y+1)*VRAM_TILES_H)+x] = pgm_read_byte(&big_font_chars[off++]);
		x++;
	}
}

void AEPrintByte(uint8_t x,uint8_t y, uint8_t val, bool zeropad){
	uint8_t c,i;
	if(val > 99){x+=2;}else if(val > 9){x++;}
	for(i=0;i<3;i++){
		c=val%10;
		if(val>0 || i==0){
			vram[(y*VRAM_TILES_H)+x] = c+FNTSTRT+56;
		}else{
			if(zeropad){
				vram[(y*VRAM_TILES_H)+x] = FNTSTRT+56;
			}else{
				vram[(y*VRAM_TILES_H)+x] = FNTSTRT;
			}
		}
		val=val/10;
		x--;
	}	
}


void AETriggerFx(unsigned char patch){//,unsigned char volume){
	unsigned char channel;
	
	unsigned char type=(unsigned char)pgm_read_byte(&(patchPointers[patch].type));
	uint8_t priority = pgm_read_byte(&sfx_priority[patch]);

	if(type==1){// || (type==2 && MIXER_CHAN4_TYPE == 1)){
		//noise or PCM channel fx		
		channel=3;
	}else if(false){//type==2){
		channel=4;
	}else
		channel=2;			
	
	if(!(tracks[channel].flags&TRACK_FLAGS_PLAYING) || priority >= current_patch_priority){
		current_patch_priority = priority;
		tracks[channel].flags|=TRACK_FLAGS_PRIORITY; //priority=1;	
		TriggerCommon(channel,patch,255,80);
	}
}


inline void SDSetup(){//initializes SD card and loads episode. if more than 1 episode is available, then user is give a choice menu
	DDRC = 0;
//	SetRenderingParameters(FIRST_RENDER_LINE,2);
	uint8_t num_episodes_available = 0;
	FRESULT res;
	WORD	br;
	uint8_t cursorpos = 0;

	pf_mount(&fs);
	for(uint8_t i=0;i<MAX_SD_ATTEMPTS;i++){
		res =	pf_open("aedat.bin");
		WaitVsync(2);
		if(!res)
			break;
	}
	if(res)
		SDCrash(0);

	res =	pf_lseek(0);//the first byte of the header has the number of episodes
	res |=	pf_read((BYTE *)ram_tiles,1,&br);
	num_episodes_available = ram_tiles[0];

	res =	pf_lseek(16);//go to first byte of episode 0 name, read the name of all episodes
	res |=	pf_read((BYTE *)ram_tiles,32*MAX_EPISODES,&br);

	if(res)
		SDCrash(1);
	
	AEPrintBigRam(2,0,"SELECT EPISODE?");
	for(uint8_t i=0;i<num_episodes_available;i++)//print all names for episodes
		AEPrintBigRam(4,3+(i<<1),(char *)&ram_tiles[(uint16_t)(i*32)]);

	while(true){
		WaitVsync(1);

		if((num_episodes_available < 2) || (padstate & BTN_START && !(oldpadstate & BTN_START))){
			ChangeEpisode(cursorpos);
			break;
		}

		if(padstate & BTN_UP && !(oldpadstate & BTN_UP)){
			if(--cursorpos >= num_episodes_available)//rollover
				cursorpos = num_episodes_available-1;
		}else if(padstate & BTN_DOWN && !(oldpadstate & BTN_DOWN)){
			if(++cursorpos >= num_episodes_available)
				cursorpos = 0;
		}
		
		for(uint16_t i=(3*VRAM_TILES_H)+2;i<VRAM_SIZE;i+=VRAM_TILES_H)
			vram[i] = FNTSTRT;
		vram[2+((3+(cursorpos<<1))*VRAM_TILES_H)] = FNTSTRT+66;
		vram[2+((4+(cursorpos<<1))*VRAM_TILES_H)] = FNTSTRT+67;
		DDRC = 255;
	}
	AEClearVram();
	DDRC = 255;
//	SetRenderingParameters(FIRST_RENDER_LINE,SCREEN_TILES_V*TILE_HEIGHT);
/*
bool episodes_available[10];
	uint8_t num_episodes_available = 0;
	bool ep_break_out = false;
	uint8_t ep = 0;
//	uint8_t loadingframe = 0;
	uint8_t ep_last_found = 0;
	uint8_t cursorpos = 0;
	uint16_t rteoff = 64;
	char file_name[] = {'a','e','0','.','b','i','n','\0'};
EPTOP:
	for(;ep<10;ep++){
//ep_break_out = true;//////////////////////////////////////
		file_name[2] = '0'+ep;
	//	attempts = 0;
		while(sdattempts < MAX_SD_ATTEMPTS){//this generally fails the first time when power was just plugged in(at least on my machine/card?)
	//		WaitVsync(1);
		//	if(!ep_break_out){
		//		vram[4+(11*VRAM_TILES_H)] = vram[24+(11*VRAM_TILES_H)] = pgm_read_byte(&ItemTileAnim[loadingframe]);
		//		if(++loadingframe > 5)
		//			loadingframe = 0;
		//	}
			//FATFS fs;
			FRESULT res;
		//	pf_mount(NULL);
			res = pf_mount(&fs);
			res |= pf_open((char *)&file_name);
			WaitVsync(2);
		//	uint32_t off = 0;//0x4000;
			WORD br;
			pf_lseek(0x100);//get offset to level data pointer
			pf_read((BYTE *)ram_tiles,10+24+1,&br);//4 bytes level data pointer, 4 bytes demo data pointer, 1 byte num levels, 1 byte num demos, 24+1 byte episode name
			if(br < 1 || res)
				sdattempts++;
			else{
				//TODO perhaps a 32bit value for an offset is overkill..
//				level_data_offset	= (uint32_t)(((uint32_t)ram_tiles[3]<<24UL)|((uint32_t)ram_tiles[2]<<16UL)|((uint32_t)ram_tiles[1]<<8UL)|((uint32_t)ram_tiles[0]<<0UL));
				//demo data starts right after end of level data	demo_data_offset	= (uint32_t)(((uint32_t)ram_tiles[7]<<24UL)|((uint32_t)ram_tiles[6]<<16UL)|((uint32_t)ram_tiles[5]<<8UL)|((uint32_t)ram_tiles[4]<<0UL));
				num_levels 	= ram_tiles[8];
				num_demos	= ram_tiles[9];
				//print out episode name, if there exists only one we are black screen anyways
				if(ep_break_out)
					return;
				episodes_available[ep] = 1;
				ep_last_found = ep;
				num_episodes_available++;
				for(uint8_t i=10;i<64+10;i++)//save the name for later
					ram_tiles[rteoff++] = ram_tiles[i];
				break;
			}
		}
	}//for(ep)

	ep_break_out = true;
	SetRenderingParameters(FIRST_RENDER_LINE,SCREEN_TILES_V*8);
	AEClearVram();
	sdattempts = 0;//important when we goto eptop

	if(num_episodes_available == 1){//no choices, skip annoying menu
		ep = ep_last_found;
		goto EPTOP;
	}else if(num_episodes_available > 1){//display choice menu
		AEPrintBigRam(2,0,"SELECT EPISODE?");
		while(true){
			WaitVsync(1);
			for(uint16_t i=(3*VRAM_TILES_H)+2;i<VRAM_SIZE;i+=VRAM_TILES_H)
				vram[i] = FNTSTRT;

			if(padstate & BTN_UP && !(oldpadstate & BTN_UP) && cursorpos){
				cursorpos--;
				AETriggerFx(SFX_EXCHANGE);
			}if(padstate & BTN_DOWN && !(oldpadstate & BTN_DOWN) && cursorpos < num_episodes_available-1){
				cursorpos++;
				AETriggerFx(SFX_EXCHANGE);
			}if(padstate & BTN_START && !(oldpadstate & BTN_START)){
				ep = cursorpos;
				goto EPTOP;
				DDRC = 0;
			//	AETriggerFx(SFX_LEVEL_CLEAR,255);
			}

			vram[2+((3+(cursorpos<<1))*VRAM_TILES_H)] = FNTSTRT+66;
			vram[2+((4+(cursorpos<<1))*VRAM_TILES_H)] = FNTSTRT+67;
			for(uint8_t i=0;i<num_episodes_available;i++)
				AEPrintBigRam(4,3+(i<<1),(char *)&ram_tiles[(uint16_t)(64+(i*64))]);
		}
	}else{
		AEPrintBigRam(3,11,"COULD NOT MOUNT DATA FILE?");
		WaitVsync(240);
		SDCrash(0);
	}
	*/
}

inline void LoadLevel(bool showname){
//	WaitVsync(1);
	exchange_flash_cnt = 0;//if num exchanges flashing before load level, make sure it wont after
	ResetSprites();//loading will take several frames, we dont want sprite blitting corrupting our buffer
	scrambler_state = SCRAMBLER_OFF;//this will wreck it too!
	WaitVsync(1);//useless????????????????????????

	FRESULT res;
	WORD br;
	WORD btr;

	res = pf_lseek((uint32_t)episode_data_base_offset+(SD_FIRST_LEVEL_BYTE)+(level*(SD_LEVEL_SIZE)));//level_data_offset 32 FOR INTERLEVEL PADDING OF 0xBB
	btr = SD_LEVEL_SIZE;
	res |= pf_read((BYTE *)ram_tiles,SD_LEVEL_SIZE,&br);
	if(res || btr != br)
		SDCrash(2);

	if(music_prev!=ram_tiles[28]){
		music_prev=ram_tiles[28];
		StopSong();
		//WaitVsync(10);
#if DEVMODE != 1
		StartSong((const char *)(pgm_read_word(&musicData[(uint8_t)(ram_tiles[28]+MUS_GAME1)])));
#endif
	}
	map_tile_set = ram_tiles[29];
//map_tile_set = 0;
	exchange = ram_tiles[30];
	player_sync_type = ram_tiles[31];
	AEClearVram();

	uint16_t rtoff=32;
	uint8_t enemy_cnt = 0;
	items_cnt = 0;
	map_frame = 0;
//	frame_cnt = 0;
	update_list_len = 0;
	enemy_move_cnt=8;
	part_ptr=1;
	player_spr = player_spr_prev = 0;
	player_state = DIR_LEFT;//player_state_dir = DIR_LEFT;


	for(uint8_t i=0;i<(ITEM_MAX)*3;i++)
		update_list[i] = 255;
	for(uint8_t i=0;i<PART_MAX;i++){
		part_y[i]=255;
		part_cnt[i]=0;
	}
	for(uint8_t i=0;i<ENEMY_MAX;i++)
		enemy_state[i] = DIR_NONE;


	uint8_t aboff = 0;
	bool isab = false;
	for(uint8_t yy=VRAM_FIRST_MAP_Y;yy<VRAM_FIRST_MAP_Y+20;yy++){
		rtoff += 2;//skip the 2 bytes on the left side that are used to align to 32byte(to make level editing easier...)
	for(uint8_t xx=VRAM_FIRST_MAP_X;xx<SCREEN_TILES_H-1;xx++){
		uint8_t tt = ram_tiles[rtoff++];
		
		if(tt < 0xA0){//sky, water, or map tile
			if(tt){//not sky
				if(tt == 15)//alter block
					isab = true;
				else
					isab = false;

				tt = pgm_read_byte(&MapTileTable[tt+(24*map_tile_set)]);

					
				vram[(yy*VRAM_TILES_H)+xx] = tt+(isab*aboff);
				if(isab){
					if(++aboff > 5)
						aboff = 0;
				}
			}
			continue;
		}else if(tt < 0xA3){//its an item
			tt -= 0xA0;
			update_list[update_list_len++]=((yy*VRAM_TILES_H)+xx)>>8;//MSB of vram addr
			update_list[update_list_len++]=((yy*VRAM_TILES_H)+xx)&0xFF;//LSB of vram addr
		
			update_list[update_list_len  ]=tt;//item #
			update_list[update_list_len++] |= ((items_cnt&7)<<4);//item frame

			items_cnt++;
		}else if(tt < 0xFF){//its an enemy
			enemy_x[enemy_cnt]=((xx)<<3);
			enemy_y[enemy_cnt]=((yy)<<3);
			//tt = 0xA7;
			if(tt == 0xA3)		//left
				enemy_state[enemy_cnt] = DIR_LEFT;
			else if(tt == 0xA4)	//right
				enemy_state[enemy_cnt] = DIR_RIGHT;
			else if(tt == 0xA5)	//up
				enemy_state[enemy_cnt]= DIR_UP;
			else if(tt == 0xA6)	//down
				enemy_state[enemy_cnt]= DIR_DOWN;
			else if(tt == 0xA7)	//alter left
				enemy_state[enemy_cnt]= DIR_LEFT|ALTER_SKULL;
			else if(tt == 0xA8)	//alter right
				enemy_state[enemy_cnt] = DIR_RIGHT|ALTER_SKULL;
			else if(tt == 0xA9)	//alter up
				enemy_state[enemy_cnt] = DIR_UP|ALTER_SKULL;
			else				//alter down
				enemy_state[enemy_cnt] = DIR_DOWN|ALTER_SKULL;

//enemy_state[enemy_cnt] |= ALTER_SKULL;
			enemy_cnt++;
		}else{//its player start(0xFF)
			player_x1 = (xx<<3);
			player_y1 = ((yy)<<3);
		}
	}//for x
		rtoff += 2;//skip the 2 bytes on the left side that are used to align to 32byte(to make level editing easier...)
	}//for y
	ram_tiles[21] = '?';//level names can only be 21 long
	if(showname)
		AEPrintBigRam(9,0,(char *)&ram_tiles);
	WaitVsync(1);
	scrambler_state = SCRAMBLER_FOREGROUND|SCRAMBLER_SKULL_EYES;//allow ram tile effects to continue
#if DEVMODE == 1
	demo_record_pos = DEMO_RT_OFF+1UL;
	struct EepromBlockStruct ebs;
	for(uint8_t ei=0;ei<4;ei++){//create all the eeprom entries we might need
		ebs.id = AE_DEMORECORD_ID+ei;
		if(EepromReadBlock(ebs.id, &ebs)){//doesn't exist, try to make it
			for(uint8_t io=0;io<30;io++)
				ebs.data[io] = 0xFF;//make sure the data here is formatted
		}
		ebs.data[0] = level-1;
		EepromWriteBlock(&ebs);
	}
	ram_tiles[DEMO_RT_OFF+0] = level;
	demo_record = 1;
#endif
}

inline void ResetSprites(){
	for(uint8_t so=0;so<MAX_SPRITES;so++)
		sprites[so].x = (SCREEN_TILES_H*TILE_WIDTH);
	spritenum=0;
}


inline void PostVsyncScrambler(){
	if(scrambler_state == SCRAMBLER_OFF)//used during SD processing
		return;

	if(scrambler_state & SCRAMBLER_DEATH_PALLET){
		uint8_t rtc = pgm_read_byte(&DeathPalletColors[(frame_cnt>>2)&3]);//NES version each colors lasts 5 frames..
		for(uint16_t rtoff=0;rtoff<(8*64);rtoff++){
			if(ram_tiles[rtoff] == 0x1F || ram_tiles[rtoff] == 0x3F || ram_tiles[rtoff] == 0x4A)
				ram_tiles[rtoff] = rtc;
		}
	}else{//recolor to normal colors(sprite colors are not true players colors)
		for(uint16_t rtoff=0;rtoff<(8*64);rtoff++){
			if(ram_tiles[rtoff] == 0x1F)
				ram_tiles[rtoff] = 0xE9;
			else if(ram_tiles[rtoff] == 0x3F)
				ram_tiles[rtoff] = 0xFF;
			else if(ram_tiles[rtoff] == 0x4A)
				ram_tiles[rtoff] = 0xD8;
		}
	}
//TODO THIS IS NOT WORKING...IT WOULD HAVE TO RUN EVERY TICK TO WORK!!!!
/*	if((scrambler_state & SCRAMBLER_SKULL_EYES) && ((map_frame&0x0F)==12)){//don't do it on frames where water or stars updated. color change every 19 frames in original
		if(false){//(frame_cnt&1)){//don't do it on frames where enemies make their collision checks.
			uint8_t tt = skull_eye_color>>5;//pgm_read_byte(&SkullEyeColorAnim[skull_eye_color>>2]);
			if(++skull_eye_color >= (16<<5))
				skull_eye_color = 0;
			for(uint16_t rtoff=((4)*64);(rtoff<(((RAM_TILES_COUNT))*64));rtoff++)
				if(ram_tiles[rtoff] == 0x87){//skull sprites eye source color
					ram_tiles[rtoff] = 0;//tt;
				//	TriggerFx(1,255,true);
					}
		}
	}
*/
	if(scrambler_state & SCRAMBLER_FOREGROUND){//return;
		for(uint8_t roff=0;roff<RAM_TILES_COUNT;roff++){
	//		if(!ram_tiles_restore[roff].tileIndex)
	//			continue;
			uint8_t t = ram_tiles_restore[roff].tileIndex-RAM_TILES_COUNT;
			if(t && (pgm_read_byte(&IsForegroundTile[(t/8)])&(128>>(t%8)))){
				for(uint8_t rtoff=0;rtoff<64;rtoff++){
					uint8_t t2 = pgm_read_byte(&map_tiles[(t*64)+rtoff]);//reblit
					if(t2)//something other than black
						ram_tiles[(roff*64)+rtoff] = t2;
				}
				t = 0;
			}
		}
	}
}
