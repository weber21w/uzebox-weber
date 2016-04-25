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





// room decompression routines
// room data is compressed with simple RLE
// high nibble of each compressed byte holds row length
// low nibble store 4-bit tile
#include <petitfatfs/pff.h>


uint16_t decompressPtr;//*decompressPtr;
uint8_t decompressData;
int8_t decompressRowLength;


uint8_t decompressByte();



uint8_t decompressByte(){
/*
	if(decompressRowLength == 0){
		decompressData = pgm_read_byte(&rooms[decompressPtr++]);//decompressPtr);
		decompressRowLength = decompressData >> 4;
//		decompressData = pgm_read_byte(&roomNibbleToByte[(decompressData & 15)]);
	}
	decompressRowLength--;
	return decompressData&15;
*/
	return 0;
}

void decompressRoom(uint8_t room){
	// init decompression state
//	decompressPtr = 0;//rooms
//	decompressData = 0;
//	decompressRowLength = 0;

	// skip rooms
//	for(int i = 0; i < room * ROOM_SIZE; i++)
//		decompressByte();
//	decompressPtr = room_offsets[room];
}

inline bool isCollectible(uint8_t tile){
	return (tile == TILE_KEY || tile == TILE_HEART || tile == TILE_GOLD || tile == TILE_DOOR);
}


void EditorWriteRoom(uint8_t room){
/*
	hideSprites(0,MAX_SPRITES);
	for(uint8_t i=0;i<RAM_TILES_COUNT;i++){
		ram_tiles_restore[i].addr = 0;
		ram_tiles_restore[i].tileIndex = 0;
	}
*/
	FRESULT res;
	WORD bt;
	
	uint32_t sd_room_off;// = (uint32_t)(SD_FIRST_EPISODE_OFFSET+(sd_episode*SD_EPISODE_SIZE)+(4*MAX_ROOMS)+(room*SD_ROOM_SIZE));//get to the first byte of the room we want
	sd_room_off = (uint32_t)((SD_FIRST_EPISODE_OFFSET)+(sd_episode*SD_EPISODE_SIZE)+(room*SD_ROOM_SIZE));
	sd_room_off <<= 9UL;//round down to start of sector
	sd_room_off >>= 9UL;

	res	=	pf_lseek((uint32_t)sd_room_off);
	res |=	pf_read((BYTE *)ram_tiles,512UL,&bt);//read entire sector so we can write unmodified data back as well

	if(res)
		SD_Crash(8,4);

	sd_room_off = (uint32_t)((SD_FIRST_EPISODE_OFFSET)+(sd_episode*SD_EPISODE_SIZE)+(room*SD_ROOM_SIZE));
	sd_room_off %= 512;//get the offset into the sector buffer we will write out

	//get room tiles

	res =	pf_write((BYTE *)ram_tiles,512UL,&bt);//write the entire sector inluding data that was there before(so we dont overwrite with 0) and our new data
	res |=	pf_write(0,0,0);//finalize write
	
	if(res || bt != 512)
		SD_Crash(5,2);

}

void RamifyRoom(uint8_t room){

	hideSprites(0,MAX_SPRITES);
	for(uint8_t i=0;i<RAM_TILES_COUNT;i++){
		ram_tiles_restore[i].addr = 0;
		ram_tiles_restore[i].tileIndex = 0;
	}
	
	uint32_t sd_room_off;
	sd_room_off = (uint32_t)((SD_FIRST_EPISODE_OFFSET)+(sd_episode*SD_EPISODE_SIZE));
//	sd_room_off = 0xA000;
	sd_room_off += (uint32_t)(room*SD_ROOM_SIZE);

		FRESULT res;
		WORD br;
		WORD btr;

		res = pf_lseek((uint32_t)sd_room_off);
		btr = SD_ROOM_SIZE;
		res |= pf_read((BYTE *)ram_tiles,btr,&br);//rooms are uncompressed so we can jump around without having to scan due to RLE or use a directory requiring more reads
		
		for(uint16_t i=0;i<SD_ROOM_SIZE-(11);i++){//TODO - should build the table into the values on the SD card, but makes manual editing more difficult...
			ram_tiles[i] = pgm_read_byte(&roomNibbleToByte[ram_tiles[i]]);
		}

		if(res || btr != br)
			SD_Crash(4,2);

}


void initRoom(uint8_t room){

	numEnemies = 0;
	uint8_t state = roomstate[room];
	uint8_t bit = 1;

	uint16_t roff = 0;

	RamifyRoom(room);
	
	//at this point all room data(13*9) is in ram_tiles followed immediately by the 4 bytes for adjacent rooms
	uint8_t j = 0;
	for(uint16_t i=SD_ROOM_SIZE-(11);i<SD_ROOM_SIZE-7;i++)//read the rooms adjacent to this one
		room_adjacent[j++] = ram_tiles[i];


	roff = 0;

	for(uint8_t i=WINDOW_Y_OFF+2;i<WINDOW_Y_OFF+((NUM_TILES_Y-1)*2)+2;i+=2){
		for(uint8_t j=WINDOW_X_OFF;j<WINDOW_X_OFF+(NUM_TILES_X*2);j+=2){
			
			uint8_t tile = ram_tiles[roff++];
			
			if(tile == TILE_WYVERN || tile == TILE_WYVERN_2ND || tile == TILE_GHOST_LEFT || tile == TILE_GHOST_RIGHT || tile == TILE_GHOST_LEFT_2ND){
				initEnemy(j,i,tile);
			}else{
				if(isCollectible(tile)){
					if(state & bit)
						tile = FIRST_MAP_TILE;
					bit <<= 1;
				}
				SetMapTile(j,i,tile);
			}		
		}
	}

}


void storeRoomState(uint8_t room){

	// store up to 8 removed items(keys, hearts, gold and doors) per room
//	decompressRoom(room);
	uint8_t state = 0;
	uint8_t bit = 1;

	uint16_t roff = 0;

	/*
		uint32_t sd_room_off = (uint32_t)(SD_FIRST_EPISODE_OFFSET+((sd_episode-1)*SD_EPISODE_SIZE)+(4*MAX_ROOMS)+(room*SD_ROOM_SIZE));//get to the first byte of the room we want
sd_room_off = 4096+128+(room*SD_ROOM_SIZE);
		FRESULT res;
		WORD br;
		WORD btr;

		res = pf_lseek((uint32_t)sd_room_off);
		btr = SD_ROOM_SIZE;
		res |= pf_read((BYTE *)ram_tiles,SD_ROOM_SIZE,&br);//rooms are uncompressed so we can jump around without having to scan due to RLE(way too slow when not in flash)
		
		for(uint16_t i=0;i<SD_ROOM_SIZE;i++){//TODO - should build the table into the values on the SD card, but makes manual editing more difficult...
			ram_tiles[i] = pgm_read_byte(&roomNibbleToByte[ram_tiles[i]&15]);
		}
		if(res || btr != br)
			SD_Crash(5,2);

	}
	*/

	RamifyRoom(room);
	uint16_t voff = ((WINDOW_Y_OFF+2)*VRAM_TILES_H)+WINDOW_X_OFF;
	roff = 0;
	
	for(uint8_t i=0;i<NUM_TILES_Y-1;i++){
		for(uint8_t j=0;j<NUM_TILES_X;j++){
			
			uint8_t tile = ram_tiles[roff++];//decompressByte();
			if(isCollectible(tile)){
				if(vram[voff] == FIRST_MAP_TILE+RAM_TILES_COUNT){//tile has been removed
					state |= bit;
					//while(1){vram[voff] = 80;dbf();}
				}
				bit <<= 1;
			}
		//	vram[voff] = 80;
			voff += 2;
		}
		voff += VRAM_TILES_H+VRAM_TILES_H-((NUM_TILES_X+1)*2)+2;
	}

	roomstate[room] = state;


}

void changeRoom(uint8_t room){

	storeRoomState(p.room);
	p.room = room;
//	hideSprites(0,MAX_SPRITES);
//	ClearWindow(WINDOW_X_OFF,WINDOW_Y_OFF+2,(NUM_TILES_X*2)-1,((NUM_TILES_Y-1)*2)-1);
	initRoom(room);
//	DrawWindowFrame(WINDOW_X_OFF-1,WINDOW_Y_OFF-1,(NUM_TILES_X*2)+1,(NUM_TILES_Y*2)+1);//now done only on first room, never overwritten
//	WaitVsync(1);

}

