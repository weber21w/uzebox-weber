#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "uzebox.h"
#include <avr/interrupt.h>

//inlude C functions required by the current video mode.
#ifdef VMODE_C_SOURCE
	#include VMODE_C_SOURCE
#endif 

#define CHAR_ZERO 16 




//Draws a map of tile at the specified position

void DrawMap2(unsigned char x,unsigned char y,const char *map){
	unsigned char i;
	unsigned char mapWidth=pgm_read_byte(&(map[0]));
	unsigned char mapHeight=pgm_read_byte(&(map[1]));

	for(unsigned char dy=0;dy<mapHeight;dy++){
		for(unsigned char dx=0;dx<mapWidth;dx++){
			
			i=pgm_read_byte(&(map[(dy*mapWidth)+dx+2]));
			
			vram[((y+dy)*VRAM_TILES_H)+x+dx]=(i + RAM_TILES_COUNT) ;
			
		
		}
	}

}


//Wait for the beginning of next frame (60hz)
void WaitVsync(int count){
	int i;
	//ClearVsyncFlag();
	for(i=0;i<count;i++){
		while(!GetVsyncFlag());
		ClearVsyncFlag();		
	}
}


//Fade table created by tim1724 
#define FADER_STEPS 12
unsigned char fader[FADER_STEPS] PROGMEM={
           // BB GGG RRR
    0x00,  // 00 000 000
    0x40,  // 01 000 000
    0x88,  // 10 001 000
    0x91,  // 10 010 001
    0xD2,  // 11 010 010
    0xE4,  // 11 100 100
    0xAD,  // 10 101 101
    0xB5,  // 10 110 101
    0xB6,  // 10 110 110
    0xBE,  // 10 111 110
    0xBF,  // 10 111 111
    0xFF,  // 11 111 111
};


unsigned char fadeStep,fadeSpeed,currFadeFrame;
char fadeDir;
bool volatile fadeActive;


void doFade(unsigned char speed,bool blocking){
	fadeSpeed=speed;
	currFadeFrame=0;
	fadeActive=true;
		
	if(blocking){
		while(fadeActive==true);
	}
	
	
}

void FadeIn(unsigned char speed,bool blocking){
	if(speed==0){
		DDRC=0xff;
		return;
	}
	fadeStep=1;
	fadeDir=1;
	doFade(speed,blocking);
}

void FadeOut(unsigned char speed,bool blocking){
	if(speed==0){
		DDRC=0;
		return;
	}
	
	fadeStep=FADER_STEPS;
	fadeDir=-1;
	doFade(speed,blocking);
}


//called by the kernel at each field end
void ProcessFading(){
	if(fadeActive==true){
		if(currFadeFrame==0){
			currFadeFrame=fadeSpeed;
			DDRC = pgm_read_byte(&fader[fadeStep-1]);
			fadeStep+=fadeDir;
			if(fadeStep==0 || fadeStep==(FADER_STEPS+1)){
				fadeActive=false;
			}
		}else{
			currFadeFrame--;
		}			
	}
}


