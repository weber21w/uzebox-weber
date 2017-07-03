/*
 *  Uzebox Kernel
 *  Copyright (C) 2008-2009 Alec Bourque
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
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "uzebox.h"

#define Wait200ns() asm volatile("lpm\n\tlpm\n\t");
#define Wait100ns() asm volatile("lpm\n\t");


extern void InitializeVideoMode();


void ReadButtons();


extern unsigned char sync_phase;
extern unsigned char sync_pulse;
extern unsigned char curr_field;
extern unsigned char  curr_frame;
extern struct TrackStruct tracks[CHANNELS];

extern unsigned char burstOffset;
extern unsigned char vsync_phase;
extern volatile unsigned int joypad1_status_lo,joypad2_status_lo;
extern volatile unsigned int joypad1_status_hi,joypad2_status_hi;
extern unsigned char tileheight, textheight;
extern unsigned char line_buffer[];
extern unsigned char render_start;
extern unsigned char playback_start;


u8 joypadsConnectionStatus;



void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void Initialize(void) __attribute__((naked)) __attribute__((section(".init8")));


void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();
    return;
}

/**
 * Performs a software reset
 */
void SoftReset(void){        
	wdt_enable(WDTO_15MS);  
	while(1);
}


/**
 * Called by the assembler initialization routines, should not be called directly.
 */

void Initialize(void){
	int i;

	cli();
	
	//Initialize the mixer buffer
	for(i=0;i<MIX_BANK_SIZE*2;i++){
		mix_buf[i]=0x80;
	}	
	
	mix_pos=mix_buf;
	mix_bank=0;

	for(i=0;i<CHANNELS;i++){
		mixer.channels.all[i].volume=0;
	}

	
	#if MIXER_CHAN4_TYPE == 0
		//initialize LFSR		
		tr4_barrel_lo=1;
		tr4_barrel_hi=1;		
		tr4_params=0b00000001; //15 bits no divider (1)
	#endif



	
	//stop timers
	TCCR1B=0;
	TCCR0B=0;
	
	//set ports
	DDRC=0xff; //video dac
	DDRB=0xff; //h-sync for ad725
	DDRD=(1<<PD7)+(1<<PD4)+(1<<PD3); //audio-out, midi-in +led + switch
	PORTD|=(1<<PD4); //turn on led


	//setup port A for joypads
	DDRA =0b00001100; //set only control lines as outputs
	PORTA=0b11111011; //activate pullups on the data lines
	
	//PORTD=0;
	
	//set sync parameters. starts at odd field, in pre-eq pulses, line 1
	sync_phase=SYNC_PHASE_PRE_EQ;
	sync_pulse=SYNC_PRE_EQ_PULSES;

	//clear timers
	TCNT1H=0;
	TCNT1L=0;

	//set sync generator counter on TIMER1
	OCR1AH=HDRIVE_CL_TWICE>>8;
	OCR1AL=HDRIVE_CL_TWICE&0xff;

	TCCR1B=(1<<WGM12)+(1<<CS10);//CTC mode, use OCR1A for match
	TIMSK1=(1<<OCIE1A);			//generate interrupt on match

	//set clock divider counter for AD725 on TIMER0
	//outputs 14.31818Mhz (4FSC)
	TCCR0A=(1<<COM0A0)+(1<<WGM01); //toggle on compare match + CTC
	OCR0A=0; //divide main clock by 2
	TCCR0B=(1<<CS00); //enable timer, no pre-scaler

	//set sound PWM on TIMER2
	TCCR2A=(1<<COM2A1)+(1<<WGM21)+(1<<WGM20); //Fast PWM	
	OCR2A=0; //duty cycle (amplitude)
	TCCR2B=(1<<CS20);  //enable timer, no pre-scaler

	SYNC_PORT=(1<<SYNC_PIN)|(1<<VIDEOCE_PIN); //set sync & chip enable line to hi

	burstOffset=0;
	curr_frame=0;
	vsync_phase=0;
	joypad1_status_hi=0;
	joypad2_status_hi=0;

	//enable color correction
	ReadButtons();
	if(ReadJoypad(0)&BTN_B){
		SetColorBurstOffset(4);
	}
	
	InitializeVideoMode();

	sei();
}

void ReadButtons(){
	unsigned int p1ButtonsLo=0,p2ButtonsLo=0;
	unsigned char i;

	//latch controllers
	JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);
    Wait200ns();
	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));


	//read button states
	for(i=0;i<16;i++){

		p1ButtonsLo>>=1;
		p2ButtonsLo>>=1;
	
		//pulse clock pin		
		JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
	    Wait200ns();
		
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) p1ButtonsLo|=(1<<15);
		if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) p2ButtonsLo|=(1<<15);
		
		JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);
		Wait200ns();

	}

		joypad1_status_lo=p1ButtonsLo;
		joypad2_status_lo=p2ButtonsLo;


	if(joypad1_status_lo==(BTN_START+BTN_SELECT+BTN_Y+BTN_B) || joypad2_status_lo==(BTN_START+BTN_SELECT+BTN_Y+BTN_B)){
		SoftReset();
	}

}

void ReadControllers(){

	//detect if joypads are connected
	//when no connector are plugged, the internal AVR pullup will drive the line high
	//otherwise the controller's shift register will drive the line low.
	joypadsConnectionStatus=0;
	if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN))==0) joypadsConnectionStatus|=1;
	if((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN))==0) joypadsConnectionStatus|=2;
			
	//read the standard buttons
	ReadButtons();
}



