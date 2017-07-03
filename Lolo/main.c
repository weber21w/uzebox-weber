#include <avr/pgmspace.h>
#include <string.h>

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>

#include <sdBase.h>


#include "data/sound/patches.inc"
#include "data/sound/songs.inc"
#include "data/graphics/lolotiles.inc"
#include "data/graphics/tileframes.inc"
#include "data/graphics/pallet.inc"
#include "data/graphics/sprites.inc"
#include "data/graphics/anims.inc"

/*
#include "sdmusic.h"
#include "lolodef.h"
#include "music.h"
#include "enemy.h"
#include "editor.h"
#include "gui.h"
#include "input.h"
#include "logic.h"
#include "render.h"
#include "engine.h"
*/

typedef struct{
	uint32_t start_sector;
	uint32_t song_position;//relative to loop_sector_start, in bytes
	uint8_t buffer[16];
	uint8_t buf_head,buf_tail;
}Music;

Music music;

void MusicFillBuffer(){

	for(uint8_t i=0;i<abs(music.buf_head-music.buf_tail);i++){
		music.buffer[music.buf_head++] = sdCardGetByte();
		if(music.buf_head >= sizeof(music.buf_head))
			music.buf_head = 0;
		music.song_position++;
		if(!(music.song_position%512UL)){
		//	sdCardCueByteAddress(music.song_position);
			break;//don't do too much per fraem
		}
	}
}

uint8_t MusicReadByte(){
	uint8_t t = music.buffer[music.buf_tail++];
	if(music.buf_tail >= sizeof(music.buffer))
		music.buf_tail = 0;
	return t;
}


uint16_t MusicReadVarLen(){
    uint16_t value;
    uint8_t c;


    if ( (value = MusicReadByte() & 0x80 ))
    {
       value &= 0x7F;
       do
       {
         value = (value << 7) + ((c = MusicReadByte() & 0x7F));
       } while (c & 0x80);
    }


    return value;
}



const char filename[] PROGMEM = "LOLODATADAT";

void main(){
	InitMusicPlayer(patches);
	SetTileTable((const char *)MapTiles);
	///Init();
//	Intro();
/*
TOP:
	if(Gui())
		goto TOP;
	Input();
	UpdateLevel();
	UpdateEnemies();
	UpdateLolo();
		
	Render();
	WaitVsync(1);
	goto TOP;
*/




	sdCardInitNoBuffer();
	music.start_sector = sdCardFindFileFirstSector(filename);

	if(!music.start_sector){
		Print(2,2,PSTR("FAILED TO FIND FILE"));
		while(1);
	}
	ClearVram();
	sdCardCueSectorAddress(music.start_sector);
	music.song_position = 0;
	MusicFillBuffer();
	StartSong(0);
	
	while(1){
	MusicFillBuffer();
	vram[100]++;
	WaitVsync(1);
	}































}
















extern unsigned char reserved_ram_tiles_count,ram_tiles[];
void Init(){


//IDEA: ADD OPTION FOR USER TO ADJUST PALLET TO WARM, COOL, ETC WITH MODE 13


	InitMusicPlayer(patches);
	SetTileTable((const char *)MapTiles);
	//SetSpritesTileBank(0,LoloSprites);
//	SetSpritesTileBank(0,testsprites);
	SetMasterVolume(224);
//	SetUserPostVsyncCallback(&VsyncCallBack);
//	seed_prng();



//LoadLevel(0);



//SoundTest();



}
