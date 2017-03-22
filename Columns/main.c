#include <avr/pgmspace.h>
#include <string.h>

#include <uzebox.h>
#include <stdint.h>

#include "data/graphics/tiles.txt"
#include "data/graphics/pallets.txt"
#include "data/graphics/compfont.inc"
#include "data/sound/patches.inc"
#include "data/sound/music.inc"
#include "data/demo_data.inc"

#include "rtl.h"
#include "columnsdefines.h"
#include "cpu.h"
#include "gui.h"
#include "sound.h"
#include "draw.h"
#include "logic.h"
#include "input.h"




void RamTileStateMachine(){//handle updating of all the ram tile graphics
#ifdef DEBUG
return;
#endif
	//~750 bytes code space, saving 5k of tile bytes and allowing several color effects not possible otherwise(which would be 12k+)
	//TODO, IN VIDEO MODE 3 VRAM IS FOLLOWED BY (0 SPRITE BYTES) RAMTILES, WHICH IS FOLLOWED BY 3*RAM_TILES_COUNT BYTES FOR RAM TILE RESTORE
	//NEED TO TALK TO ALEC ABOUT MAKING RAM TILE RESTORE AN OPTION, BUYS 1.5 RAM TILES
	//useful for other games that redraw entire screen
	//why is vramlinearbuffer(30 bytes) still there for no scrolling???
	//TODO ELIMINATE ALL UNEEDED BLITSPRITE ETC MODE 3 CODE....BIG SAVINGS!!!!!!!!!!!!!!!!
//ramtilestate[0] = NEEDALLRAMMED;


//TODO MAYBE RAMIFY 4 DIGITS FOR PLAYER SCORES, ELIMINATE NUMBER TILES!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		if(cursetype[i]&127)
			ramtilestate[i] |= NEEDGRIDRAMMED;
	}
	
	//ColumnsPrint3num(14,14,ramtilestate[1]);
	//WaitVsync(60);

	if((!ramtilestate[0] && !ramtilestate[1]) || state[0] & PAUSED)
		return;

	uint8_t t,ttoff;
	uint16_t toff,roff;
	
	if(ramtilestate[0] & NEEDALLRAMMED){//came back from pause or starting round, eliminate possible text artifacts
		ramtilestate[0] = ramtilestate[1] = (NEEDMAGICRAMMED|NEEDGRIDRAMMED|NEEDCRASHRAMMED|NEEDEXPLOSIONRAMMED);
		for(uint16_t i=0;i<FIRSTCRASHRT*64;i++)//black out any possible text artifacts from RamifyFont()
			ram_tiles[i] = 0;
	//	WaitVsync(2);
	}		
		
	if(ramtilestate[0] & NEEDMAGICRAMMED){
		ramtilestate[0] ^= NEEDMAGICRAMMED;
		//if(true || magicjewelsonscreen){
			uint8_t poff = 6*magiccolor;
			uint16_t limit;
			if(state[0] == (255^PAUSED))//used for intro, save some ram tiles for text
				limit = (8*64);
			else
				limit = (12*64);
			
			for(uint16_t i=0;i<limit;i++)
				ram_tiles[i] = pgm_read_byte(&MagicPallet[pgm_read_byte(&MagicTiles[i])+poff]);
		//}	
	}

	if(ramtilestate[0] & NEEDCRASHRAMMED){
		ramtilestate[0] ^= NEEDCRASHRAMMED;
		roff = FIRSTCRASHRT*64;
		uint8_t t2;
		for(uint8_t i=0;i<2;i++){
			ttoff = (level*8)+(i*4);

			for(uint8_t j=0;j<64;j++){
				t = pgm_read_byte(&CrashTiles[j]);
				for(uint8_t k=0;k<4;k++){
					t2 = ((t&0b11000000)>>6);
					t <<= 2;
					ram_tiles[roff++] = pgm_read_byte(&CrashPallet[(ttoff+t2)]);
				}				
			}
		}
	}		

	for(uint8_t p=0;p<MAX_PLAYERS;p++){
		if(ramtilestate[p] & NEEDEXPLOSIONRAMMED)
			ramtilestate[p] |= NEEDGRIDRAMMED;//explosion is composited onto copy of grid tiles(which could be varying shaded of red for each player depending on curse)
			
		if(ramtilestate[p] & NEEDGRIDRAMMED){
			ramtilestate[p] ^= NEEDGRIDRAMMED;
			if(cursetype[p] == 0 || cursetime[p] < 4)
				t = pgm_read_byte(&GridPallet[5+level]);
			else
				t = pgm_read_byte(&GetReadyPallet[(level*(GLOBALFLASHCOUNT+1))+(globalflashcounter)]);//pgm_read_byte(&GridPallet[3]);
				
			roff = (FIRSTGRIDRT+(p*3))*64;
			ttoff = 0;
			uint8_t t2;
			for(uint8_t i=0;i<3*64;i+=8){
				t2 = pgm_read_byte(&GridTiles[ttoff++]);//get 1bpp image data
				if(t2&128)
					ram_tiles[roff] = t;
				roff++;
				t2 <<= 1;
				if(t2&128){ram_tiles[roff] = t;}roff++;t2 <<= 1;
				if(t2&128){ram_tiles[roff] = t;}roff++;t2 <<= 1;
				if(t2&128){ram_tiles[roff] = t;}roff++;t2 <<= 1;
				if(t2&128){ram_tiles[roff] = t;}roff++;t2 <<= 1;
				if(t2&128){ram_tiles[roff] = t;}roff++;t2 <<= 1;
				if(t2&128){ram_tiles[roff] = t;}roff++;t2 <<= 1;
				if(t2&128){ram_tiles[roff] = t;}roff++;t2 <<= 1;
			}			
			
					
		}
		if(ramtilestate[p] & NEEDEXPLOSIONRAMMED){
			//TODO MAKE 4BPP
			ramtilestate[p] ^= NEEDEXPLOSIONRAMMED;
			toff = (uint16_t)(FIRSTGRIDRT+(p*3))*64;
			roff = (uint16_t)(FIRSTEXPLOSIONRT+(p*4))*64;
			for(uint8_t i=0;i<64;i++){//first, copy over grid tiles(which could be colored from curse)
				ram_tiles[roff++] = ram_tiles[toff++];
				ram_tiles[roff++] = ram_tiles[toff++];
				ram_tiles[roff++] = ram_tiles[toff++];
			}
			for(uint8_t i=0;i<16;i++){//because the 4th grid tile is NOT a ram tile(just a black one) copy black over old explosion data for this ram tile
				ram_tiles[roff++] = 0;ram_tiles[roff++] = 0;ram_tiles[roff++] = 0;ram_tiles[roff++] = 0;
			}			
			toff = (uint16_t)(256L*stateframe[p]);
			roff = (FIRSTEXPLOSIONRT+(p*4))*64;
			for(uint8_t i=0;i<64*2;i++){
				t = pgm_read_byte(&ExplosionTiles[toff++]);
				if(t)
					ram_tiles[roff] = t;
				roff++;
				//inlined to avoid need for 16bit iterator(TODO CHECK ASM OUTPUT)
				t = pgm_read_byte(&ExplosionTiles[toff++]);
				if(t)
					ram_tiles[roff] = t;
				roff++;
				/*
				t = pgm_read_byte(&ExplosionTiles[toff++]);
				t2 = t&0x0f;
				t = (t&0xf0)>>4;
				if(t)
					ram_tiles[roff] = t;
				roff++;
				if(t2)
					ram_tiles[roff] = t2;
				roff++;
				*/
			}
		}
	}
}


inline void DoPlay(){
	if(game_state == INITIALSTATE)
		SetInitialState();//game_state |= GS_FIRSTTICK;

	if(game_state & GS_FIRSTTICK){
		WaitVsync(1);
		ramtilestate[0] = ramtilestate[1] = NEEDALLRAMMED;//MAKE GRID/CRASHBLOCKS GET DRAWN;
		RamTileStateMachine();
		ramtilestate[0] = ramtilestate[1] = 0;
		WaitVsync(1);
		FadeIn(2,false);
	}
	

/////////////////////////////////////////////////////////////////////////////////////
//PROCESS 1 GAME TICK (30HZ)
/////////////////////////////////////////////////////////////////////////////////////
		UpdatePads();
		for(uint8_t p=0;p<MAX_PLAYERS;p++){
			if(state[p] == GOAHEAD){
				//while(true){};
				state[0] = 0;
				state[1] = 0;
				//game_state |= GS_FIRSTTICK;
				GeneratePreview(0);
				GeneratePreview(1);
				ramtilestate[0] = NEEDALLRAMMED;
				ramtilestate[1] = NEEDALLRAMMED;
				masterVolume = MASTERVOLUME;//needed for quick start presses during gameover
				return;
			}
			
			if(state[p] & PAUSED || state[p] & GETREADY){
				goto PROCESSINPUT;
			}				

/////////////////////////////////////////////////////////////////////////////////////
//GAME OVER STATE
/////////////////////////////////////////////////////////////////////////////////////
			if(state[p] & GAMEOVER){
				musicspeed = 0;
				SetSongTempo(musicspeed);
				if(!playSong){// && musictrack){
					masterVolume = 0;
					ResumeSong();
				}
				if(masterVolume < MASTERVOLUME){
					masterVolume+=2;
					if(masterVolume > MASTERVOLUME)
						masterVolume = MASTERVOLUME;
				}					
				clear_well(p);
				piecey[p] = 0;
				halfstep[p] = 0;
				blank_piece(p);
				goto PROCESSINPUT;
			}

/////////////////////////////////////////////////////////////////////////////////////
//LOSING/2 STATE
/////////////////////////////////////////////////////////////////////////////////////
			if(state[p] & (LOSING|LOSING2)){
	#ifdef DEBUG
//	AdjustEvalWeights();	
	SetInitialState();
			
		return;
	#endif
				crashcomingup[p] = 0;
				crashcomingdown[p] = 0;
				cpustate = 0;
				blank_piece(p);
				piecey[p] = 0;
				if(statetimer[p]){
					statetimer[p]--;
					continue;
				}					
				//statetimer[p] = 0;
				
				if(state[p] & LOSING){
#ifdef DEBUG
while(true){};
#endif
					if(game_state & GS_DEMO){
						game_state = 0;
						return;
					}
					if(crashheight[p] < 13){//still going up
						CrashBarUp(1,p);
						continue;	
					}else{//done going up, wait until other player is up then go down
						state[!p] = LOSING;
						//SetLosing(p);
						//winner = !p;
						ClearTopRows(p);//make sure no jewels are dragged back down
						if(crashheight[!p] > 12){//other player is there								
							magicjewelsonscreen = 0;
							if(!p){//only do once
								state[0] = LOSING2;
								state[1] = LOSING2;
								for(uint8_t jj=0;jj<2;jj++)
								for(uint8_t ii=0;ii<3*6;ii++)
									well[(jj*WELLSIZE)+ii] = 0;
								statetimer[0] = 2;
								statetimer[1] = 2;
								rtl_RamifyFontEx(0,gameovercharmap,charlist,compfont,0,0x37,true);
								WaitVsync(2-vsyncs);//TODO MAKE SURE EVERYTHING ALWAYS LINES ON ON 2 TICK INTERVALS(CPU PLAYER GOES HALF DROP SPEED SOMETIMES?)
								if(game_mode == 0){//adjust cpu skill
									if(winner == 1 && cpuskill > 1)
										cpuskill--;
									else if(winner == 0)// && cpuskill < 9)skill will be adjusted back to 9, used to determine if player just beat 9 or only got to it(for DoDraw())
										cpuskill++;
								}
								p--;//run through this player again so LOSE2 ticks remain even		
							}					
						}
						continue;
					}							
					continue;							
				}	
				
									
				//else LOSING2
				if(crashheight[p])
					CrashBarDown(1,p,1);
				else{
					state[p] = GAMEOVER;
					clear_well(p);
					globalflashcounter = 0;
				}
				continue;				
			}				
								
/////////////////////////////////////////////////////////////////////////////////////
//APPLY GRAVITY STATE
/////////////////////////////////////////////////////////////////////////////////////
			if(state[p] & APPLYGRAVITY){
				blank_piece(p);//TODO IS THIS NECESSARY???
				if(!ApplyGravity(p)){
					if(CheckScore(p)){
						state[p] = SCORING;
						statetimer[p] = SCOREFLASHTIME;
						stateframe[p] = 0;
					}else{
						wasmagicjewel[p] = 0;//enable scoring again, in case was a magic jewel
						state[p] = DROPPING;
						if(chain[p] > 3)
							cursetype[p] |= FLASHINGJEWELCOMING;//SummonMagicJewel(p);
						chain[p] = 0;
						droptimer[p] = DROPTIMER;
	
						if(CheckLoss(p)){//see if there are any jewels in the invisible top 3 rows, if so lose
							SetLosing(p);
							return;
						}
					}
				}//!ApplyGravity()
				add_piece(p);
				continue;
			}

/////////////////////////////////////////////////////////////////////////////////////
//SCORING STATE
/////////////////////////////////////////////////////////////////////////////////////
			if(state[p] & SCORING){
#ifdef DEBUG 
	stateframe[p] = SCOREFLASHITERATIONS*2;
	goto NOSCOREFLASH;
#endif
				if(statetimer[p])
					statetimer[p]--;
				else{
#ifdef DEBUG
NOSCOREFLASH:
#endif
					if(++stateframe[p] > SCOREFLASHITERATIONS*2){//done flashing, switch to exploding
						TriggerPCM(SCOREWAVE,(chain[p]*2)+23,190+(chain[p]*5));
						state[p] = EXPLODING;
						ramtilestate[p] |= NEEDEXPLOSIONRAMMED;//make sure ram tiles get updated for explosion graphics
						statetimer[p] = 0;
						stateframe[p] = 0;
					}else
						statetimer[p] = SCOREFLASHTIME;//still in scoring phase
				}
				continue;
			}



/////////////////////////////////////////////////////////////////////////////////////
//EXPLODING STATE
/////////////////////////////////////////////////////////////////////////////////////
			if(state[p] & EXPLODING){
#ifdef DEBUG
stateframe[p] = 6;
statetimer[p] = 0;
#endif
				if(!statetimer[p]){
					if(++stateframe[p] > 6){//done exploding
						ClearScoredJewels(p);
						state[p] = APPLYGRAVITY;
					}else{
						statetimer[p] = 1;//TODO FIX ME
					}				
				}else
					statetimer[p]--;
				
				ramtilestate[p] |= NEEDEXPLOSIONRAMMED;//make sure explosion ram tiles get updated TODO ONLY WHEN UPDATED
				continue;
			}

/////////////////////////////////////////////////////////////////////////////////////
//DROPPING STATE(FALLS THROUGH)
/////////////////////////////////////////////////////////////////////////////////////
			if(true){//state[p] & DROPPING){
				//blank_piece(p);//blank piece for piece movement code process from ProcessInputState and MovePieceDown()
				if(!droptimer[p]){
					droptimer[p] = DROPTIMER;
					//blank_piece(p);
					MovePieceDown(p);//todo it is possible to LOSE by having a piece down and moving sideways. it ends up looking like the piece didnt land. MIGHT HAVE FIXED NOW SEE INPUTSTATE:
					//add_piece(p);
				}else
					droptimer[p]--;
				if(cursetime[p]){
					if(!(--cursetime[p])){
						cursetype[p] = cursetype[p]&128;//keep FLASHINGJEWELCOMING bit
						ramtilestate[p] |= NEEDGRIDRAMMED;//make sure the grid is back to normal
					}
				}					
				//add_piece(p);
			}			

/////////////////////////////////////////////////////////////////////////////////////
//END OF STATE CODE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
/////////////////////////////////////////////////////////////////////////////////////
PROCESSINPUT:
			if(!(state[p] & LOSING)){//don't let piece move sideways the tick after LOSING was set
				blank_piece(p);
				ProcessInputState(p);
				add_piece(p);
			}			

			if(game_state == INITIALSTATE)//we were in game over and start was pushed
				return;//just let main call game play again

			ProcessCrash(p);			

		}
		
		CalculateMusicTempo();
		DoDraw();
		UpdateTimers();
		
		if(game_state & GS_FIRSTTICK)
			game_state ^= GS_FIRSTTICK;
//	}
//UPDATECRASHEDPIECES://TODO, IF CRASHED PIECES ARE STILL ON SCREEN WHEN GAME OVER COMES, THEY GET STUCK
		UpdateCrashedPieces();
		RamTileStateMachine();//take care of all gameplay ram tile updates
#ifdef DEBUG
WaitVsync(1);
#endif
		CpuThink();//all extra time goes to cpu player, he is expensive!
}


//Boot loader limit 61440
//void main() __attribute__ ((noreturn));//gcc already knows it doesn't return...
void main(){
	InitMusicPlayer(patches);
	SetTileTable(ColumnsTiles);
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = RAM_TILES_COUNT;
		/*
	while(true){
		game_state = 1;
		ProcessInputState(1);
		if(padstate[1]&BTN_Y)
			ColumnsPrint1num(10,10,1);
		else
			ColumnsPrint1num(10,10,0);
		WaitVsync(1);	
	}		
		*/
	lfsr[0] = lfsr[1] = lfsr[2] = 0x2A46l;//0xACE1;
	cpuskill = 9;
	level = 1;
	musictrack = !level;
	StartSong(Song1);//StartSong(Song0);
	SetMasterVolume(MASTERVOLUME);
	//while(1){};
	Intro();
	game_state = GS_TITLE;
	
MAINTOP:
	while(vsyncs < 2){
		WaitVsync(1);
		vsyncs++;
	}
	vsyncs = 0;
	
	if(game_state == GS_TITLE)
		DoTitle();
	else
		DoPlay();
		
	goto MAINTOP;
}


void Intro(){//return; //only 415 bytes!!!
		masterVolume = 0;
		FillVram(RAM_TILES_COUNT);
		uint16_t frames = 0;
		FadeIn(9,false);
		magiccolorwait = magiccolor = 0;
		magicjewelsonscreen = 1;
		state[0] = (255^PAUSED);//used to tell RamTileStateMachine to only draw 8 of the 12 magic jewel ram tiles. we need them for text!!!

		rtl_RamifyFontEx(11,introcharmap,charlist,compfont,0,0x37,true);
		
		while(true){
		ramtilestate[0] = NEEDMAGICRAMMED;
		RamTileStateMachine();
		if(masterVolume < MASTERVOLUME-1)
			masterVolume+=2;
		//TriggerPCM(CRASHWAVE,23,255);
		rtl_Print((VRAM_TILES_H/2)-5,10,PSTR("U Z E B O X"));
		rtl_Print((VRAM_TILES_H/2)-8,11,PSTR("CODING CHALLENGE"));
		ColumnsPrint2num((VRAM_TILES_H/2)-1,12,20);
		ColumnsPrint2num((VRAM_TILES_H/2)+1,12,13);
		if(frames < 180)
			rtl_Print(VRAM_TILES_H-14,VRAM_TILES_V-1,PSTR("LEE WEBER"));
		else
			for(uint8_t i=0;i<14;i++)
				set_tile(i+(VRAM_TILES_H-14),VRAM_TILES_V-1,0);

		for(uint8_t i=0;i<8;i++){
			DrawJewel(7+(i*2),8,8);
			DrawJewel(7+(i*2),13,8);
		}

		if(frames == 180){
			rtl_Print(2,16,PSTR("THANKS EVERYONE FOR KEEPING"));
			rtl_Print(2,18,PSTR("THE HOBBY ALIVE AND FUN FOR"));
			rtl_Print(2,20,PSTR("ALL THESE YEARS!!"));
			TriggerPCM(SCOREWAVE,24,255);
		}
		//WaitVsync(254);
		if(frames == 254+180){
			rtl_Print(4,23,PSTR("YOU GUYS ARE GREAT!!"));
			TriggerPCM(SCOREWAVE,28,255);
		}

		if(frames > 254+160){//color font ram tiles
			for(uint16_t i=8*64;i<(RAM_TILES_COUNT*64);i++){
				if(ram_tiles[i]){
					ram_tiles[i] = ram_tiles[15];
				}
			}
		}
		
		if(!magiccolorwait){
			if(magiccolor < 5)
				magiccolor++;
			else
				magiccolor = 0;
		
			//UpdateMagicRamTiles();
			ramtilestate[0] |= NEEDMAGICRAMMED;
			magiccolorwait = 6;
		}
		else
			magiccolorwait--;

		WaitVsync(1);
		if(++frames > 254+180+60+4+140)
			break;
	}


//	state[0] = 0;
//	magicjewelsonscreen = 0;
	FadeOut(6,true);
//	ramtilestate[0] = NEEDALLRAMMED;
//	RamTileStateMachine();//TODO MAGIC JEWEL STILL HAS FONT LEFTOVER FOR A COUPLE TICKS IF INTRO() IS USED???
//	ramtilestate[0] = 0;
	//FadeIn(5,false);
}
