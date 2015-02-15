#include <avr/pgmspace.h>
#include <string.h>
#include <avr/interrupt.h>

#include <defines.h>
#include <uzebox.h>
#include <stdint.h>
//#include <petitfatfs/pff.h>

#include "data/graphics/graphics.inc"
#include "data/sound/patches.inc"
#include "data/sound/music.inc"

#include "rtl.h"
#include "FFDefines.h"
#include "graphics.h"
#include "gui.h"
#include "ai.h"


void GameLoop(){

	InGui = false;
	//if(!Demo)
	//	StartSong(TitleSong);
	FadeIn(3,false);
	ResetLogic();

	SetTileTable(StageTiles);
	DrawMap2(0,0,StageMap);

	Time		= GAMETIME;
//Time = 1;//for fast EEPROM debugging, I'm about 30% sure that 90% of the EEPRom bugs are gone :)
//Frogs[0].Score = 39;
	ColorTime	= 11*60L;//COLORTIMELENGTH;
	ColorIndex	= 0;
	LastAction	= 0;

	while (1){
		WaitVsync(1);



//DrawNumber(ColorTime>>8,3,SCREEN_TILES_V-1,0);
//DrawNumber(ColorTime,8,SCREEN_TILES_V-1,0);
//DrawNumber(ColorIndex,19,SCREEN_TILES_V-1,0);
		if(!ColorTime){
			if(ColorIndex < NUMCOLORSHADES){
				ColorTime = 11*60L;//COLORTIMELENGTH;
				DDRC = pgm_read_byte(&CustomFadeTable[++ColorIndex]);
			}
		}
			ColorTime--;

		UpdateFrogs();
		CheckFlies();
		UpdateFlies();
		Render();

		if(!Time && !(Frogs[0].State & (JUMPING)) && !(Frogs[1].State & (JUMPING))){//let each frog finish jumping if they were in the middle of it
			break;
		}
		else if(Time)
			Time--;
		if(Demo == 255)
			break;

		for(uint8_t i=0;i<MAXPLAYERS;i++){
			if((JoyVal[i] & BTN_START) && !(OldJoyVal[i] & BTN_START)){
				PauseMenu();
				break;
			}	
		}
	}

}

void UpdateFrogs(){

	for(uint8_t i=0;i<MAXPLAYERS;i++){
		if(Frogs[i].InvincibleTimer){Frogs[i].InvincibleTimer--;}
		if(Frogs[i].Wait)		{Frogs[i].Wait--;}
		if(Frogs[i].BlinkTime)	{Frogs[i].BlinkTime--;}
		else					{Frogs[i].BlinkTime = (40)+(prng()&0xA0);}

		if(Frogs[i].y == FROGY)
			Frogs[i].Chain = 0;

		if(!Frogs[i].InvincibleTimer && (Frogs[!i].State & STOMPING) && !Frogs[!i].Wait){//we aren't immune, other frog is stomping and done with the spin part
			if(!(	(Frogs[!i].x > Frogs[i].x+14) ||
					(Frogs[!i].x+14 < Frogs[i].x) ||
					(Frogs[!i].y+8 > Frogs[i].y)  ||
					(Frogs[!i].y+16 < Frogs[i].y))){//got hit by the other frog
							FFTriggerPCM(SFX_FROG,30+(prng()%3),200);
						if(Frogs[i].State & (STUNNED|SWIMMING)){//do nothing

						}else if(Frogs[i].y < FROGY-24){//we are in the air, we don't get stunned, but we do fall downwards. We keep x momentum but lose all steering this jump
							Frogs[i].State = ((Frogs[i].State & FACING)|DROPPING);
							//Frogs[i].AIState = DROPPING;//AITODO
						}else if(Frogs[i].y < FROGY){//just barely off the ground, gets pummelled!
							Frogs[i].y += STOMPVELOCITY;
							if(Frogs[i].y >= FROGY){
								Frogs[i].y = FROGY;
								//STOP FORWARD MOMENTUM
							}
							Frogs[i].State = ((Frogs[i].State & FACING)|DROPPING);
							//score is added in DROPPING state below
							//Frogs[i].AIState = DROPPING;//AITODO
						}else{//TODO, check for already stunned, turning should finish turning first, etc.
							Frogs[i].State = STUNNED|(Frogs[i].State&FACING);
							Frogs[i].AIState = AI_STUNNED;
							Frogs[i].Wait = STUNTIME;
							Frogs[i].InvincibleTimer = 254;//this will get set to 48 when stun is done
							Frogs[!i].Score += STOMPSCORE;//too easy to cheat when playing 1player in a 2 player game for high score? Due to timer max would be ~30 pts in 1:30
						}
					}
		}

		if((Frogs[i].State & STUNNED)){//

			if(!Frogs[i].Wait){//done being stunned
				Frogs[i].InvincibleTimer = 48;//frog is immune from being immediately restunned
				Frogs[i].State ^= STUNNED;
				Frogs[i].AIState = AI_SIT;
				Frogs[i].State |= SITTING;
			}else{
				if((JoyVal[i] & (BTN_A|BTN_B|BTN_X|BTN_Y)) && !(OldJoyVal[i] & (BTN_A|BTN_B|BTN_X|BTN_Y)) && ((prng()&5) < 2)){
					if(Frogs[i].Wait < 6)
						Frogs[i].Wait = 0;
					else
						Frogs[i].Wait -= 6;
				}
				Frogs[i].Frame = 6+((Frogs[i].Wait/8)%3);
				continue;
			}
		}

		if((Frogs[i].y < FROGY) && !(Frogs[i].State & STOMPING)){//can stomp
			if(JoyVal[i] & BTN_DOWN && !(OldJoyVal[i] & BTN_DOWN)){
				FFTriggerFx(SFX_STOMP);
				Frogs[i].Wait = STOMPSPINTIME;
				Frogs[i].State = STOMPING|(Frogs[i].State & FACING);
			}
		}

		if(Frogs[i].State & STOMPING){
			if(Frogs[i].Wait)//still spinning
				Frogs[i].Frame = 12+4-((Frogs[i].Wait+((STOMPSPINTIME/4)-1))/(STOMPSPINTIME/4));
			else{//dropping
				Frogs[i].Frame = 12;
				Frogs[i].y += 8;
				if(Frogs[i].y >= FROGY){//landed
					FFTriggerFx(SFX_SMASH);
					Frogs[i].y = FROGY;
					Frogs[i].State ^= STOMPING;
					Frogs[i].State |= SITTING;
					Frogs[i].AIState = AI_CENTER;
				}
			}
		}

		if(Frogs[i].State & SWIMMING){//can only go to SITTING
			if((JoyVal[i]&(LICK_BUTTON|JUMP_BUTTON)) && !(OldJoyVal[i]&(LICK_BUTTON|JUMP_BUTTON)))//player can swim faster by pushing buttons
				Frogs[i].Wait = 0;
				Frogs[i].Frame = 2;
			if(!Frogs[i].Wait){
				if(JoyVal[i] & BTN_LEFT){
					Frogs[i].State &= ~FACING;
					if(Frogs[i].x > SCREENLEFT)
						Frogs[i].x--;
				}else if(JoyVal[i] & BTN_RIGHT){
					Frogs[i].State |= FACING;
					if(Frogs[i].x < SCREENRIGHT-16)
						Frogs[i].x++;
				}

				if((Frogs[i].x >= LEFTEDGE && Frogs[i].x <= MIDDLELEFT) || (Frogs[i].x >= MIDDLERIGHT && Frogs[i].x <= RIGHTEDGE)){
					Frogs[i].State ^= SWIMMING;
					Frogs[i].State |= SITTING;
					Frogs[i].AIState = AI_CENTER;
					Frogs[i].Wait = LANDWAIT;
				}else
					Frogs[i].Wait = SWIMWAIT;
			}
		}
//DrawNumber(Frogs[i].State,(i*5)+4,SCREEN_TILES_V-1,0);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if((Frogs[i].State & SITTING)){//outcomes:turning,hop----jump,leap handled later
			Frogs[i].Frame = 0;
			Frogs[i].xFrac = 0;
			Frogs[i].yFrac = 0;
			Frogs[i].xVel = 0;
			Frogs[i].yVel = 0;
			Frogs[i].AirTime = 0;
			if(true && Time){//!Frogs[i].Wait){//don't update yet if we just: got out of the water,landed,hopped
				if((Frogs[i].State & FACING)){//looking right
					if((JoyVal[i] & BTN_LEFT)){//looking right, wanting to look left
						//Frogs[i].Facing = 0;
						Frogs[i].State ^= SITTING;
						Frogs[i].State |= TURNING;
						Frogs[i].Wait = TURNWAIT;
					}else if((JoyVal[i] & BTN_RIGHT) && !Frogs[i].Wait){
						Frogs[i].State ^= SITTING;
						Frogs[i].State |= HOPPING;
					}
				}else{//looking left
					if((JoyVal[i] & BTN_RIGHT)){//looking left, wanting to look right
						Frogs[i].State ^= SITTING;
						Frogs[i].State |= TURNING;
						Frogs[i].Wait = TURNWAIT;
					}else if((JoyVal[i] & BTN_LEFT) && !Frogs[i].Wait){
						Frogs[i].State ^= SITTING;
						Frogs[i].State |= HOPPING;
					}
				}
			}//Frogs[i].Wait
		}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(Frogs[i].State & TURNING){
			if(!Frogs[i].Wait){
				Frogs[i].State ^= (TURNING|FACING);
				Frogs[i].State |= SITTING;
			}else{
			//	Frogs[i].Frame = (9+3)-((Frogs[i].Wait+((TURNWAIT/3)-1))/(TURNWAIT/3));
			Frogs[i].Frame = 9+1;
			}
		}		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(Frogs[i].State & (SITTING|TURNING|HOPPING)){//outcomes:jump
			if((JoyVal[i] & JUMP_BUTTON) && !(OldJoyVal[i] & JUMP_BUTTON)){
				Frogs[i].State = (JUMPING|(Frogs[i].State & FACING));
				//Frogs[i].AIState = AI_JUMP;//AI_CENTER WILL SOMETIMES JUMP, SO AI HAS TO SET THIS ITSELF
				Frogs[i].yVel = JUMPINITIALACCELERATION;
				Frogs[i].JumpCeiling = JUMPMAXHEIGHT;
				Frogs[i].xVel = JUMPINITIALXMOMENTUM;
				FFTriggerPCM(SFX_FROG,23+(prng()%3),180);
			}
		}

		if(Frogs[i].State & HOPPING){//can't do anything but finish our hop
			Frogs[i].Frame = 1;
			if(++Frogs[i].AirTime == HOPLENGTH){
				Frogs[i].Wait = HOPWAIT;
				Frogs[i].y		= FROGY;
				Frogs[i].State ^= HOPPING;
				Frogs[i].State |= SITTING;
			}else{
				if(!(Frogs[i].State & FACING) && Frogs[i].x > SCREENLEFT)
						Frogs[i].x--;
				else if((Frogs[i].State & FACING) && Frogs[i].x < (SCREENRIGHT - 16))
						Frogs[i].x++;
				Frogs[i].y = FROGY-pgm_read_byte(&HopWave[Frogs[i].AirTime]);
			}

		}else if(Frogs[i].State & JUMPING){
			Frogs[i].Frame = 1;
			if(!(JoyVal[i] & JUMP_BUTTON) || (Frogs[i].State & JUMPQUIT) || (Frogs[i].y <= Frogs[i].JumpCeiling)){
				Frogs[i].State |= JUMPQUIT;
				if(Frogs[i].yVel > GRAVITYPERTICK<<1)
					Frogs[i].yVel -= GRAVITYPERTICK<<1;
				else
					Frogs[i].yVel = 0;

				if(!Frogs[i].yVel){
					Frogs[i].State = ((Frogs[i].State & (FACING|DOUBLEJUMPED))|FALLING);
					Frogs[i].yFrac = 0;
					//if(Frogs[i].y < JUMPMAXHEIGHT)
					//	Frogs[i].y = JUMPMAXHEIGHT;
				}
			}else{//still climbing
				Frogs[i].yVel += JUMPACCELERATIONPERTICK;
				Frogs[i].yVel -= (GRAVITYPERTICK>>2);
				if(Frogs[i].yVel > JUMPMAXACCELERATIONPERTICK)
					Frogs[i].yVel = JUMPMAXACCELERATIONPERTICK;
				Frogs[i].yFrac += Frogs[i].yVel;
				Frogs[i].y -= (Frogs[i].yFrac>>8);//add whole units
				Frogs[i].yFrac &= 0xFF;//save fractional remainder
			}
		}
		
		if(Frogs[i].State & (FALLING|DROPPING)){
			if(Frogs[i].State & FALLING){
				Frogs[i].yVel += GRAVITYPERTICK;
				if(Frogs[i].yVel > GRAVITYMAXPERTICK)
					Frogs[i].yVel = GRAVITYMAXPERTICK;
			}else{//dropping
				Frogs[i].yVel += GRAVITYPERTICK*3;
				if(Frogs[i].yVel > GRAVITYMAXPERTICK*6)
					Frogs[i].yVel = GRAVITYMAXPERTICK*6;
			}
			Frogs[i].yFrac += Frogs[i].yVel;
			Frogs[i].y += (Frogs[i].yFrac>>8);
			Frogs[i].yFrac &= 0xFF;

			if(Frogs[i].y >= FROGY){
				Frogs[i].y = FROGY;
				Frogs[i].yFrac = 0;
				Frogs[i].yVel = 0;
				if(Frogs[i].State & DROPPING){//got hit by a stomp while in the air
					Frogs[i].State = ((Frogs[i].State&FACING)|STUNNED);
					Frogs[!i].Score += STOMPSCORE;
					Frogs[i].AIState = AI_STUNNED;
					Frogs[i].Wait = STUNTIME;
					Frogs[i].InvincibleTimer = 254;//this will get set to 48 when stun is done
				}else{//falling
					Frogs[i].State = ((Frogs[i].State&FACING)|SITTING);
					Frogs[i].AIState = AI_CENTER;
				}
			}

			//check if we did a double jump
			if(!(Frogs[i].State & DOUBLEJUMPED) && Frogs[i].yVel < DOUBLEJUMPVELOCITYWINDOW && (JoyVal[i]&JUMP_BUTTON) && !(OldJoyVal[i]&JUMP_BUTTON)){
				Frogs[i].State = (JUMPING|DOUBLEJUMPED|(Frogs[i].State & FACING));
				//Frogs[i].AIState = AI_JUMP;
				Frogs[i].yVel = JUMPINITIALACCELERATION;
				Frogs[i].JumpCeiling = DOUBLEJUMPCEILING;
				Frogs[i].xVel = JUMPINITIALXMOMENTUM;
				FFTriggerFx(SFX_JUMP1);
			}
		}
		
		if((Frogs[i].State & (JUMPING|FALLING))){//can steer
			bool side = (Frogs[i].x < (SCREEN_TILES_H*TILE_WIDTH)/2);
			if(!(Frogs[i].State & FACING)){//looking left
				if(JoyVal[i] & BTN_LEFT){
					Frogs[i].xVel += STEERACCELERATION;
					if(Frogs[i].xVel > STEERMAXACCELERATION)
						Frogs[i].xVel = STEERMAXACCELERATION;
				}else if(JoyVal[i] & BTN_RIGHT){
					if(Frogs[i].xVel < STEERACCELERATION){
						Frogs[i].xVel = 0;
						Frogs[i].State |= FACING;
					//	Frogs[i].xFrac = 0;
					}else
						Frogs[i].xVel -= STEERACCELERATION; 
				}
			}else{//looking right
				if(JoyVal[i] & BTN_RIGHT){
					Frogs[i].xVel += STEERACCELERATION;
					if(Frogs[i].xVel > STEERMAXACCELERATION)
						Frogs[i].xVel = STEERMAXACCELERATION;
				}else if(JoyVal[i] & BTN_LEFT){
					if(Frogs[i].xVel < STEERACCELERATION){
						Frogs[i].xVel = 0;
						Frogs[i].State ^ FACING;
					//	Frogs[i].xFrac = 0;
					}else
						Frogs[i].xVel -= STEERACCELERATION; 
				}
			}

			//check for wall jump
			if(JoyVal[i] & JUMP_BUTTON && !(OldJoyVal[i] & JUMP_BUTTON)){
				if((Frogs[i].x < 2 && (Frogs[i].State & FACING)) || (Frogs[i].x > (SCREEN_TILES_H*TILE_WIDTH)-((TILE_WIDTH*2)+2) && !(Frogs[i].State & FACING))){
					Frogs[i].State = (JUMPING|(Frogs[i].State & FACING));
					Frogs[i].xVel = JUMPINITIALXMOMENTUM*4;
					Frogs[i].yVel = JUMPINITIALACCELERATION;
					Frogs[i].JumpCeiling = DOUBLEJUMPCEILING;
					FFTriggerFx(SFX_JUMP1);
				}
			}
			//HACK
			if(Frogs[i].xVel == 0){
				if(JoyVal[i] & BTN_RIGHT)
					Frogs[i].State |= FACING;
				else if(JoyVal[i] & BTN_LEFT){
					Frogs[i].State |= FACING;
					Frogs[i].State ^= FACING; 
				}
			}

			
			Frogs[i].xFrac += Frogs[i].xVel;
			if(Frogs[i].State & FACING)//right
				Frogs[i].x += (Frogs[i].xFrac>>8);
			else
				Frogs[i].x -= (Frogs[i].xFrac>>8);

			Frogs[i].xFrac &= 0xFF;//save lower byte



			if(true){
				if(Frogs[i].x >= (SCREEN_TILES_H*TILE_WIDTH)-15){//passed through the screen border
					Frogs[i].xFrac = 0;
					Frogs[i].xVel = 0;
					if(side)
						Frogs[i].x = 0;
					else
						Frogs[i].x = (SCREEN_TILES_H*TILE_WIDTH)-16;
				}
			}

		}

		if((Frogs[i].State & (JUMPING|FALLING)) && !Frogs[i].Wait){//check lick
			if((!(Frogs[i].State & TONGUEING)) && (JoyVal[i] & LICK_BUTTON) && !(OldJoyVal[i] & LICK_BUTTON)){
				Frogs[i].State |= TONGUEING;
				Frogs[i].TongueTime = 0;
				FFTriggerFx(SFX_TONGUE1+(prng()&1));
			}
		}

		if(Frogs[i].State & TONGUEING){//update tongue
			if((++Frogs[i].TongueTime == LICKLENGTH) || !(Frogs[i].State & (JUMPING|FALLING))){
				Frogs[i].State ^= TONGUEING;
				Frogs[i].Wait = TONGUEWAIT;
			}
		}


		if(Frogs[i].y >= FROGY && !(Frogs[i].State & SWIMMING)){
			if(Frogs[i].x < LEFTEDGE || (Frogs[i].x > MIDDLELEFT && Frogs[i].x < MIDDLERIGHT)	|| Frogs[i].x > RIGHTEDGE){
				Frogs[i].State = SWIMMING|(Frogs[i].State & FACING);
				Frogs[i].AIState = AI_SWIM;
				FFTriggerFx(SFX_WATER);
			}
		}
	}

}





inline void CheckFlies(){

	for(uint8_t i=0;i<2;i++){
		if(!(Frogs[i].State  & TONGUEING))
			continue;
		for(uint8_t j=0;j<MAXFLIES;j++){
			if(!(Flies[j].State & ACTIVE) || Flies[j].RebirthTimer)
				continue;
			uint8_t w = pgm_read_byte(&TongueLength[Frogs[i].TongueTime >> 1])+(Cheats[i]*24);
			int16_t x1 = (int16_t)((Frogs[i].State & FACING)?(Frogs[i].x+16):(Frogs[i].x-w));
			if(	(Frogs[i].y+6 < Flies[j].y) ||
				(Frogs[i].y+4 > Flies[j].y+8) ||
				(x1 > Flies[j].x+8) ||
				(x1+w < Flies[j].x))
				continue;
			Flies[j].RebirthTimer = (prng()%140)+20;//kill fly
			Frogs[i].Chain++;
			Frogs[i].Score += Frogs[i].Chain*2;
			FFTriggerFx(SFX_GRAB1);
		}
	}	

}



inline void UpdateFlies(){

	if((uint16_t)(prng()&0x1FF) == 0x1FF){
		if(FlyCount == MAXFLIES){FlyCount--;}else{FlyCount++;}
	}


	for(uint8_t i=0;i<MAXFLIES;i++){
		if(!(Flies[i].State & ACTIVE)){
			if(i == FlyCount-1){//we want another fly for a while
				Flies[i].State |= ACTIVE;
				Flies[i].RebirthTimer = 1;
			}else
				continue;
		}
		if(Flies[i].RebirthTimer){//waiting to come back to life
			if(!(--Flies[i].RebirthTimer)){
				if(i == FlyCount){//we want 1 less fly, for now
					Flies[i].State |= ACTIVE;
					Flies[i].State ^= ACTIVE;
					continue;
				}
				if(!FlySide){
					Flies[i].x = SCREENLEFT;
					Flies[i].State |= FACING;
				}else{
					Flies[i].x = SCREENRIGHT - 8;
					Flies[i].State |= FACING;
					Flies[i].State ^= FACING;
				}
				Flies[i].y = FLYY;
				Flies[i].State |= ACTIVE;
				FlySide	= (FlySide + 1) & 1;
			}else
				continue;
		}

		uint16_t rnd = (uint16_t)((prng())&1023);
//		Flies[i].FlyTime++;
		if(!(Flies[i].State & FACING)){
			if((uint16_t)(rnd > 1021))
				Flies[i].State |= FACING;
			else{
				Flies[i].x--;
				if(SCREENLEFT == Flies[i].x){
					Flies[i].RebirthTimer = (rnd%140)+20;//kill fly
					continue;
				}
			}
		}else{
			if((uint16_t)(rnd > 1021)){
				Flies[i].State ^= FACING;
			}else{
				Flies[i].x++;
				if(SCREENRIGHT-8 == Flies[i].x){
					Flies[i].RebirthTimer = (rnd%140)+20;//kill fly
					continue;
				}
			}
		}
		if(Flies[i].y < FLYY-FLYRANGE)
			Flies[i].y++;
		else if(Flies[i].y > FLYY+FLYRANGE)
			Flies[i].y--;
		else{
			if(rnd < (1024/2))//(prng()%2000) < 1000)
				Flies[i].y--;
			else
				Flies[i].y++;
		}
	}

}

inline void ResetLogic(){

	for(uint8_t i=0;i<2;i++){
		Frogs[i].Score = 0;
		Frogs[i].x					= (!i)?FROG1X:FROG2X;
		Frogs[i].y					= FROGY;
		(Frogs[i].State 			= (SITTING|((!i)*FACING)));
		Frogs[i].AirTime			= 0;
		Frogs[i].TongueTime			= 0;
		Frogs[i].AIState			= AI_SIT;
		Frogs[i].Wait				= 0;
		Frogs[i].BlinkTime			= prng();
		Frogs[i].InvincibleTimer 	= 0;
	}
	FlyCount = 3;
	for(uint8_t i=0;i<MAXFLIES;i++){
		Flies[i].State = 0;
		if(i<FlyCount){
			Flies[i].State |= ACTIVE;
			Flies[i].RebirthTimer = prng()%60;
		}
	}

}

void VsyncRoutine(){
	UpdatePads();
}





void main(){

#if false
	for(uint8_t i=0;i<NUMAIVARS;i++)//start with random noise, let "evolution" improve it over time for unattended optimization
		bestaivars[i] = aivars[i] = pgm_read_byte(&basegenes[i]);
		//interesting note here, the evolution weights worked out to be inferior to values that I "intelligently designed"
		//just points to the crummy evolution system I made, maybe I was impatient and perhaps even a Deist again, who knows..,
#endif

	DDRC = 0;
	MusicOn = 1;
	SoundsOn = 1;
	InitMusicPlayer(patches);
	SetTileTable(TitleTiles);
	SetSpritesTileTable(SpriteTiles);
	SetMasterVolume(255);
	seedprng(0xDEADUL);
	SetUserPostVsyncCallback(&VsyncRoutine);
	Intro();

	while(1){
		TitleScreen();
		GameLoop();
#if false
		DrawBestGeneration();//ai evolution
#else
		GameOver();
		HighScoreScreen();
#endif
	}

}

uint8_t EEPromScore(uint8_t slot, bool direction, uint8_t *dat, uint8_t mag){

	struct EepromBlockStruct ebs;
	ebs.id = FF_EEPROM_ID;

	if(EepromReadBlock(ebs.id, &ebs)){//doesn't exist, try to make it
		for(uint8_t i=0;i<30;i++)
			ebs.data[i] = pgm_read_byte(&EEPROMdefault[i]);

		EepromWriteBlock(&ebs);
	}
//return;
	if(EepromReadBlock(ebs.id, &ebs) == 0){//it exists
		if(!direction){//read
			for(uint8_t i=0;i<6;i++)
				dat[i] = ebs.data[(slot*6)+i];
			return slot;
		}else{//write
			for(uint8_t i=0;i<5;i++)
				ebs.data[(slot*6)+i] = dat[i];

			ebs.data[(slot*6)+5] = mag;//score magnitude
			EepromWriteBlock(&ebs);
		}
	}else if(!direction){
		dat[0] = 'E';
		dat[1] = 'R';
		dat[2] = 'R';
		dat[3] = 'O';
		dat[4] = 'R';
		dat[5] = 0;
		return 1;
	}
	return 0;

}


inline void UpdatePads(){

	OldJoyVal[0] = JoyVal[0];
	OldJoyVal[1] = JoyVal[1];

	if(!InGui && Demo && (ReadJoypad(0) || ReadJoypad(1))){
		Demo = 255;
		return;
	}

	if(Demo && !InGui)
		JoyVal[0] = ProcessAI(0);
	else
		JoyVal[0] = ReadJoypad(0);

	if((Demo || Players == 1) && !InGui)
		JoyVal[1] = ProcessAI(1);
	else
		JoyVal[1] = ReadJoypad(1);

}





