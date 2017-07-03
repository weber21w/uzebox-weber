#include<stdlib.h>

#define SD_MUSIC_MIN_BYTES 32

u8 sd_MusicBuffer[64];
u8 sd_mus_buf_head,sd_mus_buf_tail;
uint32_t sd_songPos;//actual position on the SD card
uint8_t sd_MusicState;
uint32_t sd_songPos,sd_songStart,sd_loopStart;
uint16_t sd_currDeltaTime,sd_nextDeltaTime;
u8 sd_lastStatus=0;

#define CONTROLER_VOL 7
#define CONTROLER_EXPRESSION 11
#define CONTROLER_TREMOLO 92
#define CONTROLER_TREMOLO_RATE 100

#define DONT_READ_MORE	1//don't fill the buffer up(something else is using the SD)
#define PLAY_SONG		2

u16 SDReadVarLen();
void SDMusicFillBuffer();
u8 SDMusicReadByte();

void SDStartSong(uint32_t midiSong){/*
	for(unsigned char t=0;t<CHANNELS;t++){
		tracks[t].flags&=(~TRACK_FLAGS_PRIORITY);// priority=0;	
	}

	sd_songPos=midiSong+1; //skip first delta-time
	sd_songStart=midiSong+1;//skip first delta-time
	sd_loopStart=midiSong+1;
	sd_nextDeltaTime=0;
	sd_currDeltaTime=0;
	sd_lastStatus=0;
	sd_MusicState |= (PLAY_SONG|NEED_RECUE);
*/
}

u8 SDMusicBufferBytes(){
	return (abs(sd_mus_buf_tail-sd_mus_buf_head));
}

void SDMusicDetectEndLoop(){
/*
	//scan through entire buffer and see if the end has come
	//we want to have bytes from the beginning of the song already in the buffer before the loop actually happens
	//this avoids a stall hopefully
	u8 sd_mus_buf_old_head = sd_mus_buf_head;
	u8 sd_mus_buf_old_tail = sd_mus_buf_tail;
	uint32_t sd_old_songPos = sd_songPos;
	u8 c1,c2;

	while(SDMusicBufferBytes() > 5){
		c1 = SDMusicReadByte();
			
		if(c1==0xff){
			//META data type event
			c1 = SDMusicReadByte();

			if(c1==0x6){ //marker
				SDMusicReadByte(); //read len
				c2 = SDMusicReadByte(); //read data
				if(c2=='S'){ //loop start
					sd_loopStart=sd_songPos;
				}else if(c2=='E'){//loop end
					sd_songPos=sd_loopStart;
					sdStopTransmission();
					sd_MusicState |= NEED_RECUE;
				}
			}
				

		}else{
			if(c1&0x80)
				sd_lastStatus=c1;	
			if(c1&0x80)
				c1 = SDMusicReadByte(); 

			switch(sd_lastStatus&0xf0){
				//note-on
				case 0x90://c1 = note						
				case 0xb0://c1 = controller #
				SDMusicReadByte(); //get volume
				break;
			}//end switch(c1&0xf0)


		}//end if(c1==0xff)

		//read next delta time
		SDReadVarLen();
	}//while

	sd_mus_buf_head = sd_mus_buf_old_head;//restore original values for data still in buffer
	sd_mus_buf_tail = sd_mus_buf_old_tail;
	sd_songPos = sd_old_songPos;
*/
}

void SDMusicFillBuffer(){//run this after all game logic and before WaitVsync()
/*
	if((sd_MusicState & DONT_READ_MORE))
		return;
	if((sd_MusicState & NEED_RECUE)){
		sd_MusicState ^= NEED_RECUE;
		sdCueSectorExactLocation(sd_loopStart);
		return;
	}
	
	//not the fastest throughput, but we never want to overcommit cycles
	while(!GetVsyncFlag() && sd_mus_buf_tail != sd_mus_buf_head){//use only the cycles we have left this frame, if any
		sd_MusicBuffer[sd_mus_buf_tail] = sdCardGetByte();
		if(++sd_mus_buf_tail >= sizeof(sd_MusicBuffer))
			sd_mus_buf_tail = 0;
	}
	//and hopefully that is enough to keep the music going!
	return;
	*/
}

u8 SDMusicReadByte(){
	if(!SDMusicBufferBytes())
		return 0;

	sd_songPos++;

	uint8_t rval = sd_MusicBuffer[sd_mus_buf_head];
	
	if(sd_mus_buf_head >= sizeof(sd_MusicBuffer))
		sd_mus_buf_head = 0;
	return rval;
}

u16 SDReadVarLen(){
    unsigned int value;
    unsigned char c;


    if ( (value = SDMusicReadByte()) & 0x80 )
    {
       value &= 0x7F;
       do
       {
         value = (value << 7) + ((c = SDMusicReadByte()) & 0x7F);
       } while (c & 0x80);
    }


    return value;
}

void SDMusicUpdate(){
	u8 channel,c1,c2;
	//Process song MIDI notes
	if(sd_MusicState & PLAY_SONG && SDMusicBufferBytes() > 5){
		//process all simultaneous events
		while(sd_currDeltaTime==sd_nextDeltaTime){

			c1=SDMusicReadByte();
			
			if(c1==0xff){
				//META data type event
				c1=SDMusicReadByte();

				
				if(c1==0x2f){ //end of song
					sd_MusicState ^= PLAY_SONG;
					break;	
				}else if(c1==0x6){ //marker
					c1=SDMusicReadByte(); //read len
					c2=SDMusicReadByte(); //read data
					//^ eat the bytes
					//the looping is already handled in SDMusicDetectEndLoop()
					//the player is constantly fed the right sequence of bytes
				}
				

			}else{

				if(c1&0x80) sd_lastStatus=c1;					
				channel=sd_lastStatus&0x0f;
				
				//get next data byte			
				if(c1&0x80)
					c1=SDMusicReadByte(); 

				switch(sd_lastStatus&0xf0){

					//note-on
					case 0x90:
						//c1 = note						
						c2=SDMusicReadByte()<<1; //get volume
						
						if(tracks[channel].flags|TRACK_FLAGS_ALLOCATED){ //allocated==true
							TriggerNote(channel,tracks[channel].patchNo,c1,c2);
						}
						break;

					//controllers
					case 0xb0:
						///c1 = controller #
						c2=SDMusicReadByte(); //get controller value
						
						if(c1==CONTROLER_VOL){
							tracks[channel].trackVol=c2<<1;
						}else if(c1==CONTROLER_EXPRESSION){
							tracks[channel].expressionVol=c2<<1;
						}else if(c1==CONTROLER_TREMOLO){
							tracks[channel].tremoloLevel=c2<<1;
						}else if(c1==CONTROLER_TREMOLO_RATE){
							tracks[channel].tremoloRate=c2<<1;
						}
						
						break;

					//program change
					case 0xc0:
						// c1 = patch #						
						tracks[channel].patchNo=c1;
						break;

				}//end switch(c1&0xf0)


			}//end if(c1==0xff)

			//read next delta time
			sd_nextDeltaTime=SDReadVarLen();			
			sd_currDeltaTime=0;
		}//end while
		
		if(++sd_currDeltaTime == 65535)
			sd_MusicState ^= PLAY_SONG;
	
	}//end if(playSong)
	
	SDMusicDetectEndLoop();
	SDMusicFillBuffer();
}
