/*
 Toorum's Quest II
 Copyright (c) 2013 Petri Hakkinen
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions: 

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/




#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <petitfatfs/pff.h>

#include "data/patches.inc"
#include "data/graphics.inc"
//#include "data/roomdat_compressed.inc"
#include "data/music.inc"

#include "toorum_defines.h"
#include "player.h"
#include "sdstuff.h"
#include "room.h"
#include "enemy.h"



void RamifyScreen(uint8_t y, uint32_t offset, uint32_t len){//offset to first map byte, length of vram data to copy
	
	FRESULT res;
	WORD	br;

	res =	pf_lseek((uint32_t)(offset));
	res |=	pf_read((BYTE *)vram+(y*VRAM_TILES_H),len,&br);
//return;
	res |=	pf_lseek((uint32_t)(offset+len));
	res |=	pf_read((BYTE *)ram_tiles,(64UL*RAM_TILES_COUNT),&br);

	if(res)
		SD_Crash(0,4);


}


void drawText(uint8_t x, uint8_t y, const char* text){
	while(1){
		char ch = pgm_read_byte(text++);
		if(!ch)
			break;
		else{
			uint8_t tile = charToTile(ch);
			if(tile != 0)
				SetTile(x,y, tile);
			x++;

		}
	}

}

void drawByte(uint8_t x, uint8_t y, uint8_t val, bool zeropad){

	for(uint8_t i=0;i<3;i++){
		unsigned char c=val%10;
		if(val>0 || i==0){
			SetTile(x--,y,c+33);
		}else{
			if(zeropad){
				SetTile(x--,y,33);
			}else{
				SetTile(x--,y,0);
			}
		}
		val=val/10;
	}

}


void drawInt(int x,int y, unsigned int val,bool zeropad){

	for(uint8_t i=0;i<5;i++){
		unsigned char c=val%10;
		if(val>0 || i==0){
			SetTile(x--,y,c+33);
		}else{
			if(zeropad){
				SetTile(x--,y,33);
			}else{
				SetTile(x--,y,0);
			}
		}
		val=val/10;
	}
		
}

void drawTextRam(uint8_t x, uint8_t y, uint8_t* text){
	while(1){
		char ch = *text++;
		if(!ch)
			break;
		else{
			uint8_t tile = charToTile(ch);
			if(tile != 0)
				SetTile(x,y, tile);
			x++;

		}
	}

}


void textWriter(uint8_t ox, uint8_t oy, const char* text) {
	ClearVram();
	SetTileTable(font_tiles);
	font_tile_offset = 0;
	uint8_t x = ox;
	uint8_t y = oy;
	char ch = 0xff;
	bool slow = true;
	WaitVsync(1);

//	for(uint16_t i=0;i<VRAM_SIZE;i++)
//		vram[i] = RAM_TILES_COUNT+font_tile_offset;

	while(true) {
		while(true) {
			ch = pgm_read_byte(text);
			if(ch != 0) {
				if(ch == '\n') {
					x = ox;
					y++;
				} else {
				//	ch -= '!';
					uint8_t tile = charToTile(ch);
					//SetTile(y * 14 + x, tile);
			 		SetTile(x,y, tile);
					x++;
			 	}
			 	text++;
			}
			if(slow || ch == 0)
				break;
		}

//		updateController();//done in VsyncRoutine now
		if(y > 0 && (controllerState & (BTN_A|BTN_START)) && prevControllerState == 0) {
			if(ch == 0)
				break;
			else
				slow = false;
		}

	//	if(!instant)
			WaitVsync(1);
	 }
}

//dump uncompressed level data for manual SD file creation...

/*
if(true){
uint16_t eoff = 0;
uint16_t roff = 0;
decompressRoom(0);

for(uint8_t j=0;j<15;j++){
	for(uint16_t i=0;i<ROOM_SIZE;i++){
		
		WriteEeprom(eoff++,decompressByte());
	}

	WriteEeprom(eoff++,pgm_read_byte(&roomadj[roff++]));//output adjacent rooms
	WriteEeprom(eoff++,pgm_read_byte(&roomadj[roff++]));
	WriteEeprom(eoff++,pgm_read_byte(&roomadj[roff++]));
	WriteEeprom(eoff++,pgm_read_byte(&roomadj[roff++]));
	WriteEeprom(eoff++,0xFF);
	WriteEeprom(eoff++,0xFF);
	WriteEeprom(eoff++,0xFF);
	WriteEeprom(eoff++,0xFF);
	WriteEeprom(eoff++,0xFF);
	WriteEeprom(eoff++,0xFF);
	WriteEeprom(eoff++,0xFF);//pad to 128 bytes
}
}
*/
const uint16_t sd_anim_gfx_offsets[] PROGMEM = {0x200,0xBA0,0x1400,0x1C90,0x2470,0x2CA0,0x3670,0x40B0};

void UzeboxIntro(){
//StartSong(song2);
	ClearVram();


//return;


	RamifyScreen(10,pgm_read_word(&sd_anim_gfx_offsets[6]),(6*30UL));//draw Quantumly Intrigued Monkey
	FadeIn(5,false);
	WaitVsync(160);
	for(uint16_t j=0;j<VRAM_SIZE;j++){//flash screen white
		vram[j] = 0;
		ram_tiles[j] = 0xFF;
	}
	WaitVsync(5);//leave screen white for a bit
	ClearVram();
	DDRC = 0;
	RamifyScreen(10,pgm_read_word(&sd_anim_gfx_offsets[5]),(6*30UL));//draw tesseract/4d hypercube logo
	DDRC = 255;
	WaitVsync(130);
	FadeOut(2,false);
	for(uint8_t i=10;i<SCREEN_TILES_V-6;i++){
		ClearVram();
		RamifyScreen(i,pgm_read_word(&sd_anim_gfx_offsets[5]),(6*30UL));
		WaitVsync(2);
	}

	while(DDRC);
	ClearVram();
	FadeIn(3,false);

	for(uint8_t i=0;i<77*8;i++){
		RamifyScreen(10,pgm_read_word(&sd_anim_gfx_offsets[i%5]),(3*30UL));
		WaitVsync(5);
		if(i == 5*8)
			FadeOut(7,false);
		if(i && !DDRC)//wait until the screen is black
			break;
	}
	

	//display credits
//	uint8_t frame = 0;

	SetTileTable(font_tiles);
	ClearVram();
	FadeIn(6,false);
	drawText(2,3,PSTR("ORIGINAL CODE"));
	drawText(5,5,PSTR("PETRI HÄKKINEN"));
	drawText(2,9,PSTR("TILE GRAPHICS"));
	drawText(5,11,PSTR("ANTTI TIIHONEN"));
	drawText(2,15,PSTR("TITLE GRAPHICS"));
	drawText(5,17,PSTR("JUHO SALILA"));
	drawText(2,21,PSTR("UZEBOX CONVERSION"));
	drawText(5,23,PSTR("LEE WEBER"));
	WaitVsync(180);
	FadeOut(6,true);
}





void WriteSD(uint32_t offset, uint16_t len, unsigned char *data){

	//this write cannot overlap sectors!! Data will be corrupted!!
	FRESULT res;
	WORD	br;
	uint32_t first_sector_byte;
	uint32_t inset;

	first_sector_byte = (offset/512);
	first_sector_byte *= 512;//point to first byte of sector this lays in
	inset	= offset-first_sector_byte;//number of bytes in the sector before our data starts
//WriteEeprom(0,inset<<8);
//WriteEeprom(1,inset&0xff);
//dbf();
	//this is needed due to the way PFF handles writes(writes 0 before and after data in the sector if data isn't specified, corrupting existing)
	res =	pf_lseek((uint32_t)(first_sector_byte));//round down to first byte of sector
	res |=	pf_read((BYTE *)ram_tiles,512UL,&br);//copy the whole sector
	if(res)
		SD_Crash(7,5);

//inset = 0;
//first_sector_byte = 0;
	for(uint32_t i=inset;i<inset+len;i++) //insert our data
		ram_tiles[i] = data[i-inset];
//for(uint16_t i=0;i<200;i++)
//	ram_tiles[i] = 0xAA;

	res = pf_lseek((uint32_t)(first_sector_byte));
	res |= pf_write((BYTE *)ram_tiles,512UL,&br);//write the entire sector out, including what we added
	res |= pf_write(0,0,0);//finalize write
	if(res)
		SD_Crash(8,4);
}

void Editor(){

	FadeOut(1,true);

	SD_ChooseEpisode(1);

	ClearVram();
	uint8_t current_room = 1;
	SetTileTable(font_tiles);
	font_tile_offset = 0;
	newgame();
	FadeIn(1,false);



	uint8_t cursorx = 2;
	uint8_t cursory = 2;
	uint8_t subcursorpos = 0;
	uint8_t flash = 0;
	uint8_t mode = 0;
	uint8_t brush = TILE_WALL;
	uint8_t brushpos = 0;
	uint8_t tile_under;
	bool modified = false;
//	uint16_t timelimit = 250UL;
	uint8_t instruction_step = 0;
	uint8_t instruction_time = 0;
	drawText(WINDOW_X_OFF,WINDOW_Y_OFF,PSTR("MAHARADJA'S REVENGE"));
	tile_under = GetTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2));
	while(1){
		
		WaitVsync(1);
		if(++flash > 15)
			flash = 0;
			
		if(++instruction_time > 180){
			instruction_time = 0;
			if(++instruction_step > 7)
				instruction_step = 0;
		}
		brush = pgm_read_byte(&editor_brushes[brushpos]);

		for(uint8_t i=WINDOW_X_OFF;i<WINDOW_X_OFF+22;i++)
			SetTile(i,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,0);

		if(instruction_step == 0)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("B-ERASE"));
		else if(instruction_step == 1)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("A-DRAW"));
		else if(instruction_step == 2)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("X-CHANGE MODE"));
		else if(instruction_step == 3)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("START-SAVE"));
		else if(instruction_step == 4)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("START&LEFT-DOWN 1 ROOM"));
		else if(instruction_step == 5)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("START&RIGHT-UP 1 ROOM"));
		else if(instruction_step == 6)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("START&L-DOWN 10 ROOMS"));
		else if(instruction_step == 7)
			drawText(WINDOW_X_OFF,WINDOW_Y_OFF+(NUM_TILES_Y*2)+1,PSTR("START&R-UP 10 ROOMS"));

		drawText(WINDOW_X_OFF,WINDOW_Y_OFF+1,PSTR("TIME-"));
		uint32_t time = p.time >> 8;
		uint16_t s = 100;
		for(uint8_t i = 0; i < 3; i++){
			uint8_t n = time / s;
			time -= n * s;
			s /= 10;

			SetTile(WINDOW_X_OFF+i+5, WINDOW_Y_OFF+1, FIRST_NUMERIC_TILE+n);
		}

		drawText(WINDOW_X_OFF+19,WINDOW_Y_OFF+1,PSTR("ROOM-"));
		SetTile(WINDOW_X_OFF+24,WINDOW_Y_OFF+1,33+(current_room/10));
		SetTile(WINDOW_X_OFF+25,WINDOW_Y_OFF+1,33+(current_room%10));

		drawText(WINDOW_X_OFF+9,WINDOW_Y_OFF+1,PSTR("MODE-"));

		SetTile(WINDOW_X_OFF-1,WINDOW_Y_OFF+NUM_TILES_Y+0,33+(room_adjacent[(current_room*4)+0])/10);//west adjacent room
		SetTile(WINDOW_X_OFF-1,WINDOW_Y_OFF+NUM_TILES_Y+1,33+(room_adjacent[(current_room*4)+0])%10);
		SetTile(WINDOW_X_OFF+(NUM_TILES_X*2),WINDOW_Y_OFF+NUM_TILES_Y+0,33+room_adjacent[(current_room*4)+1]/10);//east adjacent room
		SetTile(WINDOW_X_OFF+(NUM_TILES_X*2),WINDOW_Y_OFF+NUM_TILES_Y+1,33+room_adjacent[(current_room*4)+1]%10);
		SetTile(WINDOW_X_OFF-1+NUM_TILES_X,WINDOW_Y_OFF-1,33+room_adjacent[(current_room*4)+2]/10);//north adjacent room
		SetTile(WINDOW_X_OFF-0+NUM_TILES_X,WINDOW_Y_OFF-1,33+room_adjacent[(current_room*4)+2]%10);
		SetTile(WINDOW_X_OFF-1+NUM_TILES_X,WINDOW_Y_OFF+(NUM_TILES_Y*2),33+room_adjacent[(current_room*4)+3]/10);//south adjacent room
		SetTile(WINDOW_X_OFF-0+NUM_TILES_X,WINDOW_Y_OFF+(NUM_TILES_Y*2),33+room_adjacent[(current_room*4)+3]%10);


		if(mode == 0){//tile edit
		if(!(controllerState & BTN_START)){

			if(controllerState & BTN_SL && !(prevControllerState & BTN_SL)){
				if(brushpos)
					brushpos--;
				else
					brushpos = sizeof(editor_brushes)-1;
			}
			if(controllerState & BTN_SR && !(prevControllerState & BTN_SR)){
				if(brushpos < sizeof(editor_brushes)-1)
					brushpos++;
				else
					brushpos = 0;
			}
		}
			drawText(WINDOW_X_OFF+14,WINDOW_Y_OFF+1,PSTR("TILE"));
			if(controllerState & BTN_START){
				if(modified){
					modified = false;
					uint32_t sd_offset = 0x4000;
					sd_offset += (current_room>>1)*512UL;//2 rooms per sector
					sd_offset += (current_room&1)*SD_ROOM_SIZE;
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),tile_under);
					WriteSD(sd_offset, SD_ROOM_SIZE, ram_tiles);
					//TODO SAVE STUFF...
				}
				bool load_room = false;
				if(controllerState & BTN_UP && !(prevControllerState & BTN_UP) && room_adjacent[(current_room*4)+2] >= 0){
					current_room = room_adjacent[(current_room*4)+2];
					load_room = true;
				}
				if(controllerState & BTN_DOWN && !(prevControllerState & BTN_DOWN) && room_adjacent[(current_room*4)+3] >= 0){
					current_room = room_adjacent[(current_room*4)+3];
					load_room = true;
				}
				if(controllerState & BTN_LEFT && !(prevControllerState & BTN_LEFT) && room_adjacent[(current_room*4)+0] >= 0){
					current_room = room_adjacent[(current_room*4)+0];
					load_room = true;
				}
				if(controllerState & BTN_RIGHT && !(prevControllerState & BTN_RIGHT) && room_adjacent[(current_room*4)+1] >= 0){
					current_room = room_adjacent[(current_room*4)+1];
					load_room = true;
				}
				if(controllerState & BTN_SL && !(prevControllerState & BTN_SL)){
					if(current_room)
						current_room--;
					else
						current_room = MAX_ROOMS-1;
					load_room = true;
				}
				if(controllerState & BTN_SR && !(prevControllerState & BTN_SR)){
					if(current_room < MAX_ROOMS-1)
						current_room++;
					else
						current_room = 0;
					load_room = true;
				}

				SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),tile_under);
				if(load_room){
					

					initRoom(current_room);
					for(uint8_t e=0;e<numEnemies;e++){
						uint8_t et;
						if(enemies[e].type == ENEMY_GHOST){
							if(enemies[e].dir == -1)
								et = TILE_GHOST_MOVE_LEFT;
							else
								et = TILE_GHOST_MOVE_RIGHT;
						}else{
							if(enemies[e].dir == -1)
								et = TILE_WYVERN_MOVE_UP;
							else
								et = TILE_WYVERN_MOVE_DOWN;

						}

						SetMapTile(WINDOW_X_OFF+(enemies[e].x>>3)-2,WINDOW_Y_OFF+2+(enemies[e].y>>3)-5,et);

					}
					tile_under = GetTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2));
				}

			}else{
			
				SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),(flash < 7)?brush:FIRST_MAP_TILE);
	
				bool cursormoved = false;
				if(cursorx && controllerState & BTN_LEFT && !(prevControllerState & BTN_LEFT)){
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),tile_under);
					cursorx--;
					cursormoved = true;
				}
				if(cursorx < NUM_TILES_X-1 && controllerState & BTN_RIGHT && !(prevControllerState & BTN_RIGHT)){
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),tile_under);
					cursorx++;
					cursormoved = true;
				}
				if(cursory > 1 && controllerState & BTN_UP && !(prevControllerState & BTN_UP)){
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),tile_under);
					cursory--;
					cursormoved = true;
				}
				if(cursory < NUM_TILES_Y-1 && controllerState & BTN_DOWN && !(prevControllerState & BTN_DOWN)){
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),tile_under);
					cursory++;
					cursormoved = true;
				}
				if(cursormoved){
					tile_under = GetTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2));
				}

				if(controllerState & BTN_A || controllerState & BTN_Y){
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),brush);
					tile_under = brush;
				}
				if(controllerState & BTN_B){//erase
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),FIRST_MAP_TILE);
					tile_under = FIRST_MAP_TILE; 
				}
				if(controllerState & BTN_SELECT && !(prevControllerState & BTN_SELECT)){//switch modes
					mode++;
					SetMapTile(WINDOW_X_OFF+(cursorx*2),WINDOW_Y_OFF+(cursory*2),tile_under);
				}
			}

		}else if(mode == 1){//link edit
			drawText(WINDOW_X_OFF+14,WINDOW_Y_OFF+1,PSTR("LINK"));
			if(controllerState & BTN_SELECT && !(prevControllerState & BTN_SELECT)){//switch modes
				subcursorpos = 0;
				mode++;
			}
		
		}else if(mode == 2){//time edit
			drawText(WINDOW_X_OFF+14,WINDOW_Y_OFF+1,PSTR("TIME"));
			if(controllerState & BTN_SELECT && !(prevControllerState & BTN_SELECT)){//switch modes
				mode++;
				subcursorpos = 0;
			}
		}else if(mode == 3){//name edit
			drawText(WINDOW_X_OFF+14,WINDOW_Y_OFF+1,PSTR("NAME"));
			if(controllerState & BTN_SELECT && !(prevControllerState & BTN_SELECT)){//switch modes
				mode=0;
				subcursorpos = 0;
				//cursorx = 5;
				//cursory = 5;

			}else if(controllerState & BTN_LEFT && !(prevControllerState & BTN_LEFT)){
				if(subcursorpos)
					subcursorpos--;
				else
					subcursorpos = 16;
			}else if(controllerState & BTN_RIGHT && !(prevControllerState & BTN_RIGHT)){
				if(subcursorpos < 16)
					subcursorpos++;
				else
					subcursorpos = 0;
			}else if(controllerState & BTN_UP && !(prevControllerState & BTN_UP)){
				if(cursory < 43)
					cursory++;
				else
					cursory = 0;
			}else if(controllerState & BTN_DOWN && !(prevControllerState & BTN_DOWN)){
				if(cursory)
					cursory--;
				else
					cursory = 43;
			}

			for(uint8_t i=0;i<24;i++){
				ram_tiles[((RAM_TILES_COUNT*64)-32)+i] = GetTile(WINDOW_X_OFF+i,WINDOW_Y_OFF);
				SetTile(WINDOW_X_OFF+i,WINDOW_Y_OFF,0);
			}


		}


	}
//EDITOR_ESCAPE:
	FadeOut(1,true);
	FadeIn(2,false);
	return;

}

void GameTick();

void title(){
	hideSprites(0,MAX_SPRITES);

	uint16_t idleticks;
TITLE_TOP:
	idleticks = 0;
	SetTileTable(title_tiles);//read below for the explanation of a trick
	font_tile_offset = 197;
//	DrawMap2(0,0,title_map);
	ClearVram();//needed??
	RamifyScreen(0,pgm_read_word(&sd_anim_gfx_offsets[7]),(26*30UL));//title screen, there is a trick that is a bit cryptic however
	//to save space the map and first RAM_TILES_COUNT tiles are stored in SD, all other tiles are stored in flash. This requires manually
	//grabbing the first RAM_TILES_COUNT tiles from the converted image and putting them on SD(immediately after the map), then cutting them out
	//of the flash data so that all indices with the unmodified map still line up. In the end, it saves VRAM_SIZE+(RAM_TILES_COUNT*64)=3114 bytes,
	//which is enough for a full song maybe more. I only do stuff like that towards the end when I am absolutely sure RAM_TILES_COUNT will not change

//	for(uint16_t i=0;i<RAM_TILES_COUNT*64;i++)
//		ram_tiles[i] = 0x49;
//	uint8_t j=0;
//	for(uint16_t i=(VRAM_TILES_H*11);i<(VRAM_TILES_H*11)+30;i++)//setup episode sub title option(ram tiles can be loaded from SD)
//		vram[i] = j++;
		 
	SetMasterVolume(TQ_MASTER_VOL);
	FadeIn(4,false);
	uint8_t cursorpos = 0;
//	uint8_t blink = 0;
	while(true) {
////////////////////////////////////////////////////////////////
//
//controllerState = BTN_START;
//
////////////////
////
		if(controllerState & BTN_START && !(prevControllerState & BTN_START)){
			TQ_triggerFx(0);
			if(cursorpos == 0){//play
				SD_ChooseEpisode(0);
				break;
			}else{//editor
				Editor();
				goto TITLE_TOP;
			}
		}else if((controllerState & BTN_UP && !(prevControllerState & BTN_UP)) || (controllerState & BTN_DOWN && !(prevControllerState & BTN_DOWN))){
			cursorpos = !cursorpos;
			idleticks = 0;
			uint8_t t = vram[(VRAM_TILES_H*19)+13];
			vram[(VRAM_TILES_H*19)+13] = vram[(VRAM_TILES_H*19)+13+VRAM_TILES_H];//swap cursor tile around
			vram[(VRAM_TILES_H*19)+13+VRAM_TILES_H] = t;
			TQ_triggerFx(2);
		}


		if(++idleticks > 60*15){//start demo playback
			sd_episode = 0;
			episode_base_time_limit = 250;
			demo_hold_pad = 0;
			demo_pad_state = 0;
			demo_pos = 0;
			demo_playing = 1;
			StartSong((const char *)(pgm_read_word(&musicData[trackNo])));
			break;
		}


		WaitVsync(1);
	 }


	FadeOut(2,true);
	FadeIn(2,false);

///////

	if(!demo_playing){
		textWriter(2,4,intro1Str);
		textWriter(3,4,intro2Str);
		FadeOut(2,true);
		if(trackNo)
			StartSong((const char *)(pgm_read_word(&musicData[trackNo])));
	}
	font_tile_offset = 0;//redundant, done in textWriter above...

	SetTileTable(font_tiles);//font tiles are immediately followed by map tiles, because we need font in the title and map and don't want to include twice
	FadeIn(1,false);
	ClearVram();

}


void newgame(){

	for(uint8_t i=0;i<MAX_ROOMS;i++)
		roomstate[i] = 0;

	initRoom(0);
	DrawWindowFrame(WINDOW_X_OFF-1,WINDOW_Y_OFF-1,(NUM_TILES_X*2)+1,(NUM_TILES_Y*2)+1);
	initPlayer();

}


void VsyncRoutine();

void setup(){

	trackNo = 1;
	InitMusicPlayer(patches);
	SetTileTable(font_tiles);
	SetSpritesTileTable(sprite_tiles);
	SetUserPostVsyncCallback(&VsyncRoutine);
	DDRC = 0;
	font_tile_offset = 0;
	ClearVram();
	SD_Startup();//open and if needed, initialize high score file, then load game data file
	UzeboxIntro();//do SD animation stuff and display credits
}

void updateTiles(){
//screen is confirmed 14 tiles wide
//TODO, IMPLEMENT THIS CUSTOM SONG PLAYER FOR OUR TRIGGERNOTE() SO NO CONVERSION NEEDED?

	//frame++;
	if(++frame&1)//fix for 60hz timing
		return;

	// update single row per frame
	//uint8_t base = (frame & 7) + 1;
	//base *= NUM_TILES_X;
	uint8_t y_pos = ((frame>>1)%NUM_TILES_Y)+1;//?+1 why..
	uint16_t voff = (((y_pos*2)+WINDOW_Y_OFF)*VRAM_TILES_H)+WINDOW_X_OFF;//((frame & 7) + 1)*VRAM_TILES_H;
	for(uint8_t i = 0; i < NUM_TILES_X*2; i+=2) {
		uint8_t t = vram[voff]; 
		switch(t) {
		case TILE_GOLD+RAM_TILES_COUNT:
		case TILE_HEART_BIG+RAM_TILES_COUNT:
		case TILE_PRINCESS+RAM_TILES_COUNT:
			vram[voff]+=4;
			vram[voff+1]+=4;
			vram[voff+VRAM_TILES_H]+=4;
			vram[voff+VRAM_TILES_H+1]+=4;
		break;

		case TILE_GOLD+4+RAM_TILES_COUNT:
		case TILE_HEART+RAM_TILES_COUNT:
		case TILE_PRINCESS+4+RAM_TILES_COUNT:
			vram[voff]-=4;
			vram[voff+1]-=4;
			vram[voff+VRAM_TILES_H]-=4;
			vram[voff+VRAM_TILES_H+1]-=4;
		break;
	
		}
		voff+=2;
	}
}


void hideSprites(uint8_t start, uint8_t end){
	for(uint8_t i=start;i<end;i++)
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;

}




void DrawMenuBG(){

	for(uint8_t i=0;i<64;i++){
		ram_tiles[(21*64)+i] = 0x09;
		ram_tiles[(22*64)+i] = 0x49;
	}
	ClearVram();

	for(uint8_t i=0;i<SCREEN_TILES_H;i++){
		vram[(VRAM_TILES_H*3)+i] = 21;//make horizontal lines on top and bottom(same colors as title)
		vram[(VRAM_TILES_H*22)+i] = 21;
	}

	for(uint16_t i=0;i<VRAM_SIZE;i++){//fill in between the 2 lines wi
		if(vram[i] == 21)
			break;
		vram[i] = 22;
	}

	for(uint16_t i=VRAM_TILES_H*23;i<VRAM_SIZE;i++)
		vram[i] = 22;

	SetTileTable(font_tiles);
	font_tile_offset = 0;

}


void SD_InitializeHighScore(){



}

void ShowHighScores(){

//	FadeOut(2,true);
	DrawMenuBG();

	if(SD_OpenFile(SD_SAVE_FILE_NAME)){
	return;
	/*
		drawText(5,10,PSTR("CAN'T OPEN _HISCORE.DAT"));
		drawText(5,12,PSTR("CREATE THIS FILE ON THE"));
		drawText(5,14,PSTR("SD TO SAVE HIGH SCORES"));
		FadeIn(1,false);
		while(1){
			WaitVsync(1);
			if(controllerState & BTN_START && !(prevControllerState & BTN_START))
				goto SHOW_HIGH_SCORE_BOTTOM;//saves flash
		}*/
	}
	//copy the high score data into ram tiles
	SD_ReadHighScoreTable((uint32_t)(SD_TQ_HIGH_SCORE_OFFSET+(sd_episode*SD_EPISODE_HIGH_SCORE_LIST_SIZE)));//fill up the first 512 bytes of ram tiles with the TQ SD save slot

/*
	uint16_t checksum = 0;
	for(uint16_t i=0;i<510;i++)
		checksum += ram_tiles[i];
	if(checksum != ((ram_tiles[510]<<8)+(ram_tiles[511]))){//data is corrupted, do something about it?

	}
*/
	drawText(8,5,PSTR("-HALL OF FAME-"));
	uint16_t off = 0;
	for(uint8_t i=0;i<SD_MAX_HIGH_SCORE_ENTRIES;i++){
		
		uint8_t x = 8;
		uint8_t y = 8+(i*2);
		for(uint8_t j=0;j<SD_HIGH_SCORE_ENTRY_SIZE-2;j++){
			uint8_t ch = ram_tiles[off++];
			uint8_t tile = ch;
			SetTile(x++,y, tile);
		}
		x += 2;

		uint16_t score = ram_tiles[off++]<<8;
		score |= ram_tiles[off++];

		uint16_t s = 10000;
		for(uint8_t j=0;j<5;j++) {
			uint8_t n = score / s;
			score -= n * s;
			s /= 10;
			SetTile(x++, y, 33+n);//charToTile('0'+n));
		}
	}

	FadeIn(2,false);
	
	uint16_t idlecount = 0;

	while(1){
		WaitVsync(1);
		if((!prevControllerState && controllerState) || ++idlecount > 10*60)
			break;
	}
//SHOW_HIGH_SCORE_BOTTOM:
	FadeOut(4,true);

}


void ShowHighScoresEEPROM(){

//	FadeOut(2,true);
	DrawMenuBG();

	struct EepromBlockStruct ebs;
	ebs.id = TQ_ID;

	if(!EepromReadBlock(ebs.id, &ebs) == 0){//can't read high scores that aren't yet there
		for(u8 i=0;i<30;i++)
			ebs.data[i] = pgm_read_byte(&default_high_score_data[i]);//make sure the data here is formatted

		EepromWriteBlock(&ebs);
	}
	//even if the EEPROM has failed, at least the default list will display

	drawText(8,7,PSTR("-HALL OF FAME-"));
	uint8_t off = 0;
	for(uint8_t i=0;i<5;i++){
		
		uint8_t x = 8;
		uint8_t y = 10+(i*2);
		for(uint8_t j=0;j<4;j++){
			uint8_t ch = ebs.data[off++];
			uint8_t tile = ch;//pgm_read_byte(&charset[ch]);
			SetTile(x++,y, tile);
		}
		x += 5;

		uint16_t score = ebs.data[off++]<<8;
		score |= ebs.data[off++];

		uint16_t s = 10000;
		for(uint8_t j = 0; j < 5; j++) {
			uint8_t n = score / s;
			score -= n * s;
			s /= 10;
			SetTile(x++, y, charToTile('0'+n));
		}
	}

	FadeIn(2,false);
	
	uint16_t idlecount = 0;

	while(1){
		WaitVsync(1);
		if((!prevControllerState && controllerState) || ++idlecount > 10*60)
			break;
	}

	FadeOut(4,true);
	FadeIn(2,false);

}



void EnterHighScore(){

//enters with screen faded out
	hideSprites(0,MAX_SPRITES);
	DrawMenuBG();

	if(SD_OpenFile(SD_SAVE_FILE_NAME)){
		drawText(4,10,PSTR("- NO HIGH SCORE FILE -"));
		drawText(4,11,PSTR("SCORES CANNOT BE SAVED"));
		FadeIn(1,false);

		while(1){
			WaitVsync(1);
			if(controllerState & BTN_START && !(prevControllerState & BTN_START))
				goto ENTER_HIGH_SCORE_BOTTOM;//saves flash
		}
	}

	//copy the high score data into ram tiles
	SD_ReadHighScoreTable((uint32_t)(SD_TQ_HIGH_SCORE_OFFSET+(sd_episode*SD_EPISODE_HIGH_SCORE_LIST_SIZE)));//fill up the first 512 bytes of ram tiles with the TQ SD save slot
	
	if(ram_tiles[510] != 'T' || ram_tiles[511] != 'Q'){//first time save

		for(uint8_t i=0;i<SD_MAX_EPISODES;i++){
			uint32_t soff = i*SD_HIGH_SCORE_ENTRY_SIZE*SD_MAX_HIGH_SCORE_ENTRIES;
			for(uint8_t j=0;j<sizeof(default_high_score_data);j++)
				ram_tiles[soff+j] = pgm_read_byte(&default_high_score_data[j]);

		}
		ram_tiles[510] = 'T';
		ram_tiles[511] = 'Q';
	}

	uint8_t place = 255;
	for(uint8_t i=0;i<SD_MAX_HIGH_SCORE_ENTRIES;i++){//compare our score against all current scores, we must beat one of them to enter
		uint16_t total;
		total = ram_tiles[(SD_HIGH_SCORE_ENTRY_SIZE*i)+(SD_HIGH_SCORE_ENTRY_SIZE-2)]<<8;//MSB
		total |= ram_tiles[(SD_HIGH_SCORE_ENTRY_SIZE*i)+(SD_HIGH_SCORE_ENTRY_SIZE-1)];//LSB
		if(total < p.score){
			place = i;
			break;
		}
		
	}
	if(place == 255)//did the player get on the board?
		return;

	FadeIn(2,false);
	//player did make it, get their input then insert it at a dummy slot. Then keep sorting slots until everything is resolved; save.

	uint8_t pos = 0;
	uint8_t flash = 0;

	drawText(3,5,PSTR("CONGRATULATIONS YOU MADE"));
	drawText(6,7,PSTR("THE LEADER BOARD!"));
	drawText(7,9,PSTR("ENTER YOUR NAME"));
	//textWriter(3,4,PSTR("CONGRATULATIONS YOU MADE\n\nTHE LEADER BOARD!!\n\nENTER YOUR NAME"));

	for(uint16_t i=512;i<512+64;i++)//make a white tile for cursor flash, avoid the first 512 bytes we are using for the sector image
		ram_tiles[i] = 0xFF;

	uint16_t voff = (VRAM_TILES_H*11)+7;



	while(1){
		WaitVsync(1);
		if(++flash > 14)
			flash = 0;

		for(uint8_t i=0;i<(SD_HIGH_SCORE_ENTRY_SIZE-2);i++)
			vram[voff+i] = charbuf[i]+RAM_TILES_COUNT;

		if(flash < 7)
			vram[voff+pos] = 8;

		if(pos < (SD_HIGH_SCORE_ENTRY_SIZE-3)	&& (controllerState & BTN_RIGHT) && !(prevControllerState & BTN_RIGHT))
			pos++;
		else if(pos > 0 						&& (controllerState & BTN_LEFT) && !(prevControllerState & BTN_LEFT))
			pos--;
		else if((controllerState & BTN_UP)		&& !(prevControllerState & BTN_UP)){
			if(++charbuf[pos] > 43)
				charbuf[pos] = 0;
		}else if((controllerState & BTN_DOWN) 	&& !(prevControllerState & BTN_DOWN)){
			if(charbuf[pos])
				charbuf[pos]--;
			else
				charbuf[pos] = 43;
		}else if((controllerState & BTN_START) && !(prevControllerState & BTN_START)){

			for(uint8_t i=0;i<SD_HIGH_SCORE_ENTRY_SIZE-2;i++){//did the user enter at least 1 non-space character?
				if(charbuf[i])
					goto ENTER_HIGH_SCORE_BREAKOUT;
			}
		}

	}
ENTER_HIGH_SCORE_BREAKOUT:

	for(uint8_t i=0;i<SD_HIGH_SCORE_ENTRY_SIZE-2;i++)//copy players name directly after the existing list
		ram_tiles[(SD_HIGH_SCORE_ENTRY_SIZE*SD_MAX_HIGH_SCORE_ENTRIES)+i] = charbuf[i];

	ram_tiles[(SD_HIGH_SCORE_ENTRY_SIZE*SD_MAX_HIGH_SCORE_ENTRIES)+(SD_HIGH_SCORE_ENTRY_SIZE-2)] = p.score>>8;
	ram_tiles[(SD_HIGH_SCORE_ENTRY_SIZE*SD_MAX_HIGH_SCORE_ENTRIES)+(SD_HIGH_SCORE_ENTRY_SIZE-1)] = p.score&0xFF;

	bool moved;

	do{
		moved = false;

		for(uint8_t i=0;i<5;i++){//swap out of order entries until there are none
			uint16_t total;
			uint16_t offset = (i*SD_HIGH_SCORE_ENTRY_SIZE);
			total = 	ram_tiles[offset+(SD_HIGH_SCORE_ENTRY_SIZE-2)]<<8;//get a 16bit score for this entry
			total |=	ram_tiles[offset+(SD_HIGH_SCORE_ENTRY_SIZE-1)];

			uint16_t total2;
			total2	= 	ram_tiles[offset+(SD_HIGH_SCORE_ENTRY_SIZE-2)+SD_HIGH_SCORE_ENTRY_SIZE]<<8;//compare versus following entry
			total2	|=	ram_tiles[offset+(SD_HIGH_SCORE_ENTRY_SIZE-1)+SD_HIGH_SCORE_ENTRY_SIZE];
			
			if(total2 > total){//if the next one should be above this, swap them
				uint8_t t;
				for(uint8_t j=0;j<SD_HIGH_SCORE_ENTRY_SIZE;j++){
					t = ram_tiles[offset+j+SD_HIGH_SCORE_ENTRY_SIZE];//copy the byte from the next entry
					ram_tiles[offset+j+SD_HIGH_SCORE_ENTRY_SIZE] = ram_tiles[offset+j];//copy previous over it
					ram_tiles[offset+j] = t;
				}
				moved = true;
			}
		}

	}while(moved);

	SD_WriteHighScoreTable();

ENTER_HIGH_SCORE_BOTTOM:
	FadeOut(4,true);
	return;
}






void tiletest(){
return;
ClearVram();
for(uint16_t i=0;i<VRAM_SIZE;i++){
	vram[i] = i;
}
/*
uint16_t start = 0*64;
uint16_t end = 30*64;
for(uint16_t i=start;i<end;i++){
	ram_tiles[i-(start)] = pgm_read_byte(&titlescreen[i]);
	
	uint8_t p;
	p = 0;
	if(ram_tiles[i-(start)]&0b00000001)//D0 blue 0
		p |= 0b01000000;
	if(ram_tiles[i-(start)]&0b00000010)//D1 blue 1
		p |= 0b10000000;
	if(ram_tiles[i-(start)]&0b00000100)//D2 green 0
		p |= 0b00001000;
	if(ram_tiles[i-(start)]&0b00001000)//D3 green 1
		p |= 0b00010000;
	if(ram_tiles[i-(start)]&0b00010000)//D4 green 2
		p |= 0b00100000;
	if(ram_tiles[i-(start)]&0b00100000)//D5 red 0
		p |= 0b00000001;
	if(ram_tiles[i-(start)]&0b01000000)//D6 red 1
		p |= 0b00000010;
	if(ram_tiles[i-(start)]&0b10000000)//D7 red 2
		p |= 0b00000100;
	ram_tiles[i-(start)] = p;

}
*/
while(1);}


void VsyncRoutine(){

	updateController();

	if(demo_playing){
		if(controllerState & BTN_START){
			p.gameover = 3;
			demo_playing = false;
			music_fade_frames = 1;
			music_fade_step = 6;
		}//rest is done in UpdatePlayer(), since depending on SD load times per card, the tick numbers could be off!! Keep logic timing separate from video!!
	}

	if(music_fade_frames){
		if(!music_fade_delay){
			if(GetMasterVolume() < music_fade_step){
				SetMasterVolume(0);
				music_fade_frames = 0;
				StopSong();
			}else
				SetMasterVolume(GetMasterVolume()-music_fade_step);
			music_fade_delay = music_fade_frames;
		}else
			music_fade_delay--;
	}
//	music_fade_frames = frames;

/*
	for(uint8_t roff=0;roff<RAM_TILES_COUNT;roff++){

		uint8_t t = ram_tiles_restore[roff].tileIndex;
		if(t >= FIRST_BORDER_TILE){//player moved over the window, blit it over them to avoid looking weird
			for(uint8_t rtoff=0;rtoff<64;rtoff++){
				uint8_t t2 = pgm_read_byte(&tiles[(t*64)+rtoff]);//reblit
				ram_tiles[(roff*64)+rtoff] = t2;
			}
		}
	}
*/

}


void main(){

	setup();
MAIN_TOP:
	SD_OpenFile(SD_DATA_FILE_NAME);



//Editor();


	title();
	newgame();

			//	SetRenderingParameters(12,200);
	while(1) {

		//updateController();//done in vsyncRoutine now

		if(!p.gameover) {
			
			updateTiles();
			updatePlayer();//ORDER HAS CHANGED
			hideSprites(4,MAX_SPRITES);
			updateEnemies();
		}else{
			// game over

			if(p.gameover == 3){//user bypassed the attract mode demo or it ended
				FadeOut(2,true);
				demo_playing = false;
				controllerState = 0;//needed??
				hideSprites(0,MAX_SPRITES);
				ShowHighScores();
				goto MAIN_TOP;
			}

			if(p.gameover == 1)//player dead
				hideSprites(0,MAX_SPRITES);


			// score bonus from remaining time
			if(p.gameover == 2) {//won game
				while(p.time) {

					if(p.time < 256UL)
						p.time = 0;
					else{
					
						uint8_t wait;
						if(p.time > 256*20UL)
							wait = 1;
						if(p.time > 256*12UL)
							wait = 5;
						else if(p.time > 256*6UL)
							wait = 10;
						else if(p.time > 256*2UL)
							wait = 14;
						else
							wait = 30;

						WaitVsync(wait);
			
						TriggerFx(SOUND_GOLD,255,1);
						p.score += 25;					
						p.time -= 256;
					
						updateScoreBar();
					}
				}

			}

			if((p.gameover == 1 || (p.gameover == 2 && !p.time)) && controllerState & BTN_START) {
				FadeOut(6,true);
				while(GetMasterVolume()){
					music_fade_frames = 1;
					music_fade_step = 6;
					WaitVsync(1);
				}
				StopSong();
				WaitVsync(25);
				EnterHighScore();
				ShowHighScores();
				goto MAIN_TOP;
			}

		}

		updateScoreBar();
		
		WaitVsync(1);
	}
}
