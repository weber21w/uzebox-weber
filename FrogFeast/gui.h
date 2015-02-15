#define MENUSTART 199
inline void DrawMenu(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
   SetTile(x+0,y+0,MENUSTART+0);//draw the corners
   SetTile(x+w,y+0,MENUSTART+1);
   SetTile(x+0,y+h,MENUSTART+2);
   SetTile(x+w,y+h,MENUSTART+3);

   for(u8 i=x+1;i<x+w;i++){SetTile(i,y,MENUSTART+8);SetTile(i,y+h,MENUSTART+9);}//draw top and bottom
   for(u8 i=y+1;i<y+h;i++){SetTile(x,i,MENUSTART+6);SetTile(x+w,i,MENUSTART+7);}

   for(u8 i=y+1;i<y+h;i++)
   for(u8 j=x+1;j<x+w;j++)
      SetTile(j,i,MENUSTART+5);
   
}


inline void Intro(){//return;

//	WaitVsync(1);
	//print ram tiles UZEBOX
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = 96+RAM_TILES_COUNT;//full white tile that is in TitleTiles[]
	rtl_RamifyFontEx(RAM_TILES_COUNT-7,introcharmap,charlist,compfont,0xFF,0x00);
	rtl_Print(12,12,PSTR("UZEBOX"));

	DrawSpriteFrame(13*8,(10*8)+2,3);
	Flies[0].x = (SCREEN_TILES_H*TILE_WIDTH)-8;
	Flies[0].y = 0;
	FadeIn(3,true);
/*
uint8_t frame = 42;//frame test
while(true){
	WaitVsync(1);
	HideSprites();
	if(JoyVal[0] &BTN_LEFT && !(OldJoyVal[0]&BTN_LEFT))
		frame--;
	if(JoyVal[0] &BTN_RIGHT && !(OldJoyVal[0]&BTN_RIGHT))
		frame++;
		DrawSpriteFrame(13*8,(6*8)+2,frame);

}
*/
	
	while(Flies[0].x){
		uint8_t rnd = prng();
		sprites[4].x = Flies[0].x;
		sprites[4].tileIndex = FLYSPRITE+((Flies[0].x&2)>>1);
		sprites[4].y = Flies[0].y+((sprites[4].tileIndex!=FLYSPRITE)*3);
		WaitVsync(1);
		Flies[0].x-=1;
		if(Flies[0].x > 17*8 || Flies[0].x < 7*8){
			if(Flies[0].y < 3)
				Flies[0].y++;
			else if((rnd&3) == 1)
				Flies[0].y -= 1;
			else if((rnd&3)==2)
				Flies[0].y += 1;
		}else if(Flies[0].x < (14*8)+2){//run away from frog
			spritecount = 0;
			DrawSpriteFrame(13*8,(10*8)+2,0);
			if(Flies[0].y > 24)
				Flies[0].y-=2;
			Flies[0].x--;//extra speed
		}else if(Flies[0].y < 10*8)//tease frog
			Flies[0].y+=4;
	}
	sprites[4].x = SCREEN_TILES_H*TILE_WIDTH;
	FadeOut(2,false);
	FFTriggerPCM(SFX_FROG,23,255);

	uint8_t fx = 13*8;
	uint8_t fy = 10*8;
	while(fx < SCREEN_TILES_H*TILE_WIDTH){
		spritecount = 0;
		DrawSpriteFrame(fx,fy,1);
		fx-=4;
		fy-=2;

		WaitVsync(1);
	}
	WaitVsync(20);
	FFTriggerFx(SFX_GRAB1);
	WaitVsync(60);

}


inline void TitleScreen(){

	InGui = true;//make sure AI doesn't fill pad state
	if(!Demo)
		StartSong(TitleSong);

	uint8_t cursorpos;
	uint16_t idleticks;
	uint8_t codepos[2]={0,0};
	Frogs[0].y = ((SCREEN_TILES_V-12)*8);
TITLETOP:
	rtl_RamifyFontEx(8,titlecharmap,charlist,compfont,0xD0,0xFF);

	OldJoyVal[0] = OldJoyVal[1] = JoyVal[0] = JoyVal[1] = (uint16_t)(BTN_UP|BTN_DOWN|BTN_A|BTN_B|BTN_START|BTN_SELECT);//don't auto trigger something after skipping a demo
	while(1){//break out to credits

		DDRC = 0;
	//	Players = 1;
		cursorpos = 0;
		idleticks = 0;
		Demo = 0;
		SetTileTable(TitleTiles);
		DrawMap2(0,0,TitleMap);
		rtl_Print(3,SCREEN_TILES_V-7,PSTR("SELECT:"));
		rtl_Print(3,SCREEN_TILES_V-5,PSTR("1 PLAYER"));
		rtl_Print(3,SCREEN_TILES_V-3,PSTR("2 PLAYERS"));
		rtl_Print(3,SCREEN_TILES_V-1,PSTR("CREDITS"));
		FadeIn(3,0);

		while(1){
			prng();
			Frogs[0].State = 1;
			Frogs[1].y = ((SCREEN_TILES_V-6)*8)+(cursorpos*16);
			if(Frogs[0].y < Frogs[1].y){
				if(Frogs[1].y - Frogs[0].y > 8)
					Frogs[0].y+=4;
				else
					Frogs[0].y += 2;
			}else if(Frogs[0].y > Frogs[1].y){
				if(Frogs[0].y - Frogs[1].y > 8)
					Frogs[0].y-=4;
				else
					Frogs[0].y -= 2;
			}else
				Frogs[0].State = 0;

			spritecount = 0;
			if(!Frogs[0].BlinkTime)
				Frogs[0].BlinkTime = (40)+(prng()&0xA0);
			else
				Frogs[0].BlinkTime--;
			DrawSpriteFrame(STARTX,Frogs[0].y,0+((Frogs[0].BlinkTime<4)*3)+(Frogs[0].State)+16);
			WaitVsync(1);
//JoyVal[0] = BTN_START;OldJoyVal[0] = 0;Frogs[1].AIState = 0;Players=2;SoundsOn=0;//////////////////////////////////////////////////////////////////////
			if(++idleticks > TITLEIDLETIME){Demo=1;return;
			//	HighScoreScreen();
			//	goto TITLETOP;
			}
			for(uint8_t i=0;i<2;i++){//pads updated in VsyncRoutine
				if(JoyVal[i] && !OldJoyVal[i]){
					if(JoyVal[i] == pgm_read_word(&KonamiCode[codepos[i]])){
						if(++codepos[i] ==(sizeof(KonamiCode)/2) ){
							FFTriggerPCM(SFX_FROG,24,255);
							FadeIn(1,false);
							Cheats[i] = !Cheats[i];
							codepos[i] = 0;
						}
					}else
						codepos[i] = 0;
				}
				if(JoyVal[i])
					idleticks = 0;
				if((JoyVal[i] & BTN_START) && !(OldJoyVal[i] & BTN_START)){
					FFTriggerPCM(SFX_FROG,24,180);//FFTriggerFx(SFX_GRAB1);
					if(cursorpos < 2){
						if(cursorpos == 0)
							Players = 1;
						else{
							Frogs[1].AIState = 0;//disable AI
							Players = 2;
						}
						return;
					}
					CreditScreen();
					goto TITLETOP;
				}

				if((JoyVal[i] & BTN_UP) && !(OldJoyVal[i] & BTN_UP)){
						FFTriggerFx(SFX_GRAB1);
					if(--cursorpos > 254)
						cursorpos = 2;
				}else if((JoyVal[i] & BTN_DOWN) && !(OldJoyVal[i] & BTN_DOWN)){
					if(SoundsOn)
						FFTriggerFx(SFX_GRAB1);
					if(++cursorpos > 2)
						cursorpos = 0;
				}
			}
		}
	}

	FadeOut(1,true);

}


inline bool ScrollEntry(uint8_t x, uint8_t y, uint8_t *buffer){

	uint8_t restore[SCREEN_TILES_H];
	for(uint8_t i=0;i<SCREEN_TILES_H;i++)
		restore[i] = vram[i+(y*VRAM_TILES_H)];

	for(uint8_t i=255-11;i!=x;i++){//pads updated in VsyncRoutine
		if((JoyVal[0] && !(OldJoyVal[0])) || (JoyVal[1] && !(OldJoyVal[1])))
			return true;
		for(uint8_t j=0;j<SCREEN_TILES_H;j++)
			vram[j+(y*VRAM_TILES_H)] = restore[j];
		rtl_PrintRam(i,y,(char *)buffer);
		WaitVsync(1);
	}
	return false;	

}


inline void HighScoreScreen(){

	InGui = true;
	FadeOut(3,true);
	HideSprites();
	for(uint8_t i=0;i<64;i++)
		ram_tiles[i] = 0xD1;//generate bg color
	SetTileTable(HighScoreTiles);
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = 0;//point to bg ramtile color
	for(uint8_t y=0;y<6;y++)
		for(uint8_t x=0;x<20;x++)
			vram[(y*VRAM_TILES_H)+x] = RAM_TILES_COUNT;//make the top highlight color
	for(uint8_t y=6;y<SCREEN_TILES_V;y++)
		for(uint8_t x=20;x<SCREEN_TILES_H;x++)
			vram[(y*VRAM_TILES_H)+x] = RAM_TILES_COUNT;//make the bottom highlight color

	DrawMap2(24,21,HighScoreMap);//just draw the fly as a map, the rest is procedural
	rtl_RamifyFontEx(1,highscorecharmap,charlist,compfont,0xD1,0xFF);
	rtl_Print(20,5,PSTR("TOP FROGS"));
	FadeIn(3,true);
	
	uint8_t stringbuf[33];
	for(uint8_t i=0;i<5;i++){
		EEPromScore(i,0,(uint8_t *)&stringbuf,0);
		uint8_t t = stringbuf[5];
		stringbuf[6] = ' ';
		stringbuf[7] = ' ';
		if(t/100)
			stringbuf[8] = (t/100)+'0';
		else
			stringbuf[8] = ' ';
		stringbuf[9] = ((t%100)/10)+'0';
		stringbuf[10] = (t%10)+'0';
		stringbuf[11] = 0;

		if(ScrollEntry(9,9+(i*3),stringbuf))//was button pressed to skip?
			goto HIGHSCOREDONE;
	}

	uint16_t Delay;
	while(1){
		prng();
		WaitVsync(1);//pads updated in VsyncRoutine
	//	for(uint8_t i=0;i<2;i++){
			if((JoyVal[0] && !(OldJoyVal[0])) || (JoyVal[1] && !(OldJoyVal[1]))){
			FFTriggerFx(SFX_GRAB1);
				goto HIGHSCOREDONE;
			}
		//	if(JoyVal[i] && !OldJoyVal[i])
		//		Delay = 10*60;
	//	}
		if(++Delay > 6*60)
			break;
	}
HIGHSCOREDONE:
		FadeOut(3,true);

}




void CreditScreen(){

	FadeOut(3,true);
	HideSprites();
	for(uint8_t i=0;i<64;i++){
		ram_tiles[(4*64)+i] = 0xD1;
		ram_tiles[(5*64)+i] = 0x69;
	}
	for(uint8_t y=0;y<SCREEN_TILES_V;y++)
		for(uint8_t x=0;x<SCREEN_TILES_H;x++){
			if(x < 19)
				vram[(VRAM_TILES_H*y)+x] = 4;
			else
				vram[(VRAM_TILES_H*y)+x] = 5;
	}
	rtl_RamifyFontEx(8,titlecharmap,charlist,compfont,0xD1,0xFF);
	rtl_Print(1,1,	PSTR("ORIGINAL GAME"));
	rtl_Print(2,3,	PSTR("CHARLES DOTY"));
	rtl_Print(1,10,	PSTR("BG GRAPHICS"));
	rtl_Print(2,12,	PSTR("ALEC BOURQUE"));
	rtl_Print(1,19,	PSTR("UZEBOX VERSION"));
	rtl_Print(2,21,	PSTR("LEE WEBER"));
//	rtl_Print(1,24, PSTR("PLAY TESTING"));

	WaitVsync(1);
	FadeIn(3,false);

	while(1){
		WaitVsync(1);//pads updated in VsyncRoutine
		for(uint8_t i=0;i<2;i++){
			if(JoyVal[i] && !(OldJoyVal[i])){
				FadeOut(3,true);
				return;
			}
		}
	}

}



#define HIGHSCORE_RAMTILES_OFFSET (RAM_TILES_COUNT-2)*64

void RamifyAndSortEEProm(){
	for(uint8_t i=0;i<5;i++)//load all eeprom entries entirely into ram, 5 name byte, and 1 score byte each
		EEPromScore(i,0,(uint8_t *)&ram_tiles[i*6],0);
	
	for(uint8_t i=5;i<7;i++){
		ram_tiles[(i*6)] = i-5;//use a character that can't be in a high score name
		ram_tiles[(i*6)+5] = Frogs[i-5].Score;
	}

	uint8_t keep_sorting = true;
	while(keep_sorting){
		keep_sorting = false;
		for(uint8_t i=6;i>0;i--){
			if(ram_tiles[(i*6)+5] > ram_tiles[((i-1)*6)+5]){
				keep_sorting = true;//we go through the list until it never happens, then it's sorted
				for(uint8_t j=0;j<6;j++){//now swap the entries
					ram_tiles[(10*6)+j] = ram_tiles[((i-1)*6)+j];//keep a temporary copy of the old higher position in unused ram
					ram_tiles[((i-1)*6)+j] = ram_tiles[(i*6)+j];//put the higher score up in the list overwriting the old higher position
					ram_tiles[(i*6)+j] = ram_tiles[(10*6)+j];//use the temporary to put in it's new lower position
				}
			}
		}
	}

}

inline bool HighScoreEntry(){//return;
	InGui = true;
	HideSprites();
	WaitVsync(1);
	uint8_t done[2] = {0,0};
	uint8_t boardpos[2] = {255,255,};//place on the scoreboard
	uint8_t scoreplace[2] = {0,0};//which letter position we are on during entry

	for(uint8_t i=0;i<SCREEN_TILES_H;i++)
		vram[((SCREEN_TILES_V-1)*VRAM_TILES_H)+i] = 72+RAM_TILES_COUNT;//blank out words and time

	WaitVsync(1);

	//sprite are cleared so we can use ram_tiles as a buffer to do our high score sorting. once we know the position of a score
	//this isn't very efficient code but easier this way. this code would need some changes to support more than 2 players

	RamifyAndSortEEProm();

	//player scores are added and the order was updated, now scan back through looking for 0 or 1 to determine board pos
	for(uint8_t i=0;i<5;i++){
		if(ram_tiles[(i*6)] < MAXPLAYERS)
			boardpos[ram_tiles[i*6]] = i;
	}

	if(Players < 2 || Cheats[1])
		boardpos[1] = 255;//bots and cheaters can't enter the list
	if(Cheats[0])
		boardpos[0] = 255;//cheaters never prosper...I wonder if that's really true?

	//now if the boardpos for the player is higher than 0-4 the score didn't make it to the list
	//we know the position to write into eeprom, now we can RamifyFont() and get the names
	rtl_RamifyFontEx(0,highscorecharmap,charlist,compfont,0xD0,255);//load up our fontset for name entry

	if((boardpos[0] == 255) && (boardpos[1] == 255))
		return true;

	WaitVsync(1);



	if(boardpos[0] != 255)
		rtl_Print(ENDSCORE1X-9,SCREEN_TILES_V-1,PSTR("NEW RECORD!"));
	else
		done[0] = 1;

	if(boardpos[1] != 255)
		rtl_Print(ENDSCORE1X+4,SCREEN_TILES_V-1,PSTR("NEW RECORD!"));
	else
		done[1] = 1;


OldJoyVal[0] = OldJoyVal[1] = 255;//I don't think this needs to be here anymore, but I'm too lazy to test if thats always true!
	while(true){
		uint8_t flashcounter;
		WaitVsync(1);
		flashcounter++;
		for(uint8_t i=0;i<MAXPLAYERS;i++){
			if(boardpos[i] == 255 || done[i])//no high score or done entering
				continue;


if(!OldJoyVal[i]){
			if((JoyVal[i] & BTN_LEFT) /*&& !(OldJoyVal[i] & BTN_LEFT)*/ && scoreplace[i]){
				//vram[  (VRAM_TILES_H*16)+5+(i*8)+scoreplace[i]  ] = 72+RAM_TILES_COUNT;//vram[].....overwrite character with blank
				//scorename[(i*6)+scoreplace[i]] = ' ';
				scoreplace[i]--;
				FFTriggerFx(SFX_JUMP2);
			}else if((JoyVal[i] & BTN_RIGHT) /*&& !(OldJoyVal[i] & BTN_RIGHT)*/ && scoreplace[i] < 4){
				scoreplace[i]++;
				FFTriggerFx(SFX_JUMP2);
			}else if((JoyVal[i] & BTN_UP)){// && !(OldJoyVal[i] & BTN_UP)){
				if(scorename[(i*6)+scoreplace[i]] == ' ')
					scorename[(i*6)+scoreplace[i]] = 'Z';
				else if(--scorename[(i*6)+scoreplace[i]] < 'A')
					scorename[(i*6)+scoreplace[i]] = ' ';
			}else if((JoyVal[i] & BTN_DOWN)){// && !(OldJoyVal[i] & BTN_DOWN)){
				if(scorename[(i*6)+scoreplace[i]] == ' ')
					scorename[(i*6)+scoreplace[i]] = 'A';
				else if(++scorename[(i*6)+scoreplace[i]] > 'Z')
					scorename[(i*6)+scoreplace[i]] = ' ';
			}
		
			if((JoyVal[i] & BTN_START)){// && !(JoyVal[i] & BTN_START))){
				scoreplace[i] = 4;
				JoyVal[i] = BTN_A|BTN_START;
				OldJoyVal[i] = BTN_START;
			}
			if(((JoyVal[i] & BTN_A))){// && !(OldJoyVal[i] & BTN_A))){
				if(++scoreplace[i]==5){
					scoreplace[i] = 4;
					FFTriggerPCM(SFX_FROG,23,255);
					done[i] = true;
					continue;
				}
			}
}
			for(uint8_t j=0;j<5;j++){
				if(scorename[(i*6)+j] != ' ')
					vram[  (VRAM_TILES_H*(SCREEN_TILES_V-10))+6+j+(i*12)] = 10+(scorename[(i*6)+j]-'A');//+(i*8)+scoreplace[i]
				else
					vram[(VRAM_TILES_H*(SCREEN_TILES_V-10))+6+j+(i*12)] = RAM_TILES_COUNT+72;
			}
			
			//HACK
			scorename[5] = scorename[11] = 0;//I don't know why it's getting corrupted, A button only?? meh...

			if(!done[i] && (flashcounter&3) > 1){
				if(scorename[(i*6)+scoreplace[i]] != ' ')
					vram[  (VRAM_TILES_H*(SCREEN_TILES_V-10))+6+scoreplace[i]+(i*12)] = 72+RAM_TILES_COUNT; 
				else
				vram[  (VRAM_TILES_H*(SCREEN_TILES_V-10))+6+scoreplace[i]+(i*12)] = 12+RAM_TILES_COUNT; 
			}
		}//for(MAXPLAYERS)
//all name data is entered, put it in the correct spot in the buffer

		if(done[0] && done[1]){
			FadeOut(3,true);
			RamifyAndSortEEProm();
			for(uint8_t j=0;j<5;j++){
				if(ram_tiles[(j*6)] < MAXPLAYERS){
					uint8_t pnameoff = ram_tiles[(j*6)]*6;//we stored 0 or 1 for player num in RamifyAndSortEEProm()
					for(uint8_t k=0;k<5;k++)
						ram_tiles[(j*6)+k] = scorename[pnameoff+k];
				}
				EEPromScore(j,1,(uint8_t *)&ram_tiles[j*6],(uint8_t)ram_tiles[(j*6)+5]);
			}
			return true;
		}


		//if(done[0] && done[1])
		//	return true;
	}//while(true)
	return false;
}



//const uint8_t gameover_colors[] PROGMEM = {};
inline void GameOver(){
	InGui = true;
	if(Demo)
		return;
	//uint16_t	Delay	= ENDDELAY;
	WaitVsync(2);
	HideSprites();


	for(uint8_t i=0;i<SCREEN_TILES_H;i++)
		vram[((SCREEN_TILES_V-1)*VRAM_TILES_H)+i] = vram[(SCREEN_TILES_V-2)*VRAM_TILES_H];//blank out scores and time

//	uint8_t gameover_color_index = 0;
	WaitVsync(1);
	rtl_RamifyFontEx(11,gameovercharmap,charlist,compfont,0xD0,255);
	WaitVsync(1);
	DDRC = 255;//go fullbright since we were doing a night time effect
//Frogs[0].Score=2;

	if(Frogs[0].Score > Frogs[1].Score){
		DrawSpriteFrame(PLAYER1X, PLAYERY, 10);
		DrawSpriteFrame(PLAYER2X, PLAYERY,35);
		rtl_Print(ENDSCORE1X-6,SCREEN_TILES_V-1,PSTR("WINNER!"));
	}else if(Frogs[1].Score > Frogs[0].Score){
		DrawSpriteFrame(PLAYER1X, PLAYERY, 19);
		DrawSpriteFrame(PLAYER2X, PLAYERY,42);
		rtl_Print(ENDSCORE2X,SCREEN_TILES_V-1,PSTR("WINNER!"));
	}else{
		DrawSpriteFrame(PLAYER1X, PLAYERY, 19);
		DrawSpriteFrame(PLAYER2X, PLAYERY, 35);
		rtl_Print(TIEX,SCREEN_TILES_V-1,PSTR("TIE GAME!"));
	}

	DrawNumber(Frogs[0].Score, ENDSCORE1X-(Frogs[0].Score<10)+(Frogs[0].Score>99),SCREEN_TILES_V-9,0);
	DrawNumber(Frogs[1].Score, ENDSCORE2X,SCREEN_TILES_V-9,0);
	WaitVsync(256);
	if(!HighScoreEntry()){//did anyone take the time to enter a score? then don't do the wait loop	
/*		while(1){
			WaitVsync(1);//pads updated in VsyncRoutine

			if(!(--Delay))
				break;
			else{ 
//				rtl_RamifyFontEx(8,titlecharmap,charlist,compfont,0xD0,pgm_read_byte(&gameover_colors[gameover_color_index]));
//				if(++gameover_color_index > 10)
//					gameover_color_index = 0;
			}
		}*/
//	WaitVsync(60);
	WaitVsync(ENDDELAY);
	}
}

#define PAUSEMENUX 10
#define PAUSEMENUY 12
#define PAUSEMENUW 10
#define PAUSEMENUH 8
//const uint8_t FadeAdjustedBG[] PROGMEM = {0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0};
const uint8_t FadeAdjustedFG[] PROGMEM = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0};
inline void PauseMenu(){
	InGui = true;
	uint8_t cursorpos = 0;
	uint8_t escapemenu = 0;
	HideSprites();
	WaitVsync(1);
	FFTriggerFx(SFX_PAUSE);
	for(uint8_t y=0;y<=PAUSEMENUH;y++)
		for(uint8_t x=0;x<=PAUSEMENUW;x++)
			ram_tiles[((RAM_TILES_COUNT-4)*64)+(y*(PAUSEMENUW+1))+x] = vram[(VRAM_TILES_H*((PAUSEMENUY)+y))+PAUSEMENUX+x];//save tiles underneath menu
	

	rtl_RamifyFontEx(0,pausecharmap,charlist,compfont,0xC0,pgm_read_byte(&FadeAdjustedFG[ColorIndex]));
	//WaitVsync(1);

	while(true){
		WaitVsync(1);

		for(uint8_t i=0;i<MAXPLAYERS;i++){
			if((JoyVal[i] & BTN_UP) && !(OldJoyVal[i] & BTN_UP)){
				if(--cursorpos > 254)
					cursorpos = 3;
				FFTriggerFx(SFX_TONGUE1);	
			}else if((JoyVal[i] & BTN_DOWN) && !(OldJoyVal[i] & BTN_DOWN)){
				if(++cursorpos > 3)
					cursorpos = 0;
				FFTriggerFx(SFX_TONGUE1);	
			}else if(((JoyVal[i] & BTN_LEFT) && !(OldJoyVal[i] & BTN_LEFT)) || ((JoyVal[i] & BTN_RIGHT) && !(OldJoyVal[i] & BTN_RIGHT))){
				if(cursorpos == 1){//music
					MusicOn = !MusicOn;
					if(!MusicOn)
						StopSong();
					else
						ResumeSong();
				}else if(cursorpos == 2){//sounds
					SoundsOn = !SoundsOn;
					FFTriggerFx(SFX_TONGUE1);	
				}
			}

			if((JoyVal[i] & (BTN_START|BTN_A|BTN_B)) && !(OldJoyVal[i] & (BTN_START|BTN_A|BTN_B))){
				if(cursorpos == 0){//resume
					escapemenu = true;
				}else if(cursorpos == 3){//quit
					escapemenu = true;
					Demo = 255;//break out to main menu the same way demos do.
				}
			}
		}

		if(escapemenu)
			break;
		DrawMenu (PAUSEMENUX+0,PAUSEMENUY+0,PAUSEMENUW,PAUSEMENUH);
		SetTile	 (PAUSEMENUX+0,PAUSEMENUY+1+(cursorpos*2),MENUSTART+4);
		rtl_Print(PAUSEMENUX+1,PAUSEMENUY+1,PSTR("RESUME"));
		//rtl_Print(PAUSEMENUX+1,PAUSEMENUY+3,PSTR("MUSIC ON"));if(!MusicOn)	{rtl_Print(PAUSEMENUX+7,PAUSEMENUY+3,PSTR("OFF"));}
		if(!MusicOn)	{rtl_Print(PAUSEMENUX+1,PAUSEMENUY+3,PSTR("MUSIC OFF"));}else{rtl_Print(PAUSEMENUX+1,PAUSEMENUY+3,PSTR("MUSIC ON"));}
		if(!SoundsOn)	{rtl_Print(PAUSEMENUX+1,PAUSEMENUY+5,PSTR("SFX   OFF"));}else{rtl_Print(PAUSEMENUX+1,PAUSEMENUY+5,PSTR("SFX   ON"));}
		rtl_Print(PAUSEMENUX+1,PAUSEMENUY+7,PSTR("QUIT"));

	}
	for(uint8_t y=0;y<=PAUSEMENUH;y++)
		for(uint8_t x=0;x<=PAUSEMENUW;x++)
			vram[(VRAM_TILES_H*((PAUSEMENUY)+y))+PAUSEMENUX+x] = ram_tiles[((RAM_TILES_COUNT-4)*64)+(y*(PAUSEMENUW+1))+x];//restore tiles underneath menu
	WaitVsync(1);
	InGui = false;
}


void SDCrash(){
/*
	while(1){
		WaitVsync(1);
		db();
	}
*/
}
void RamifyFromSD(uint16_t id){
/*
	HideSprites();//important, this takes multiple frame and sprite blitting during SD read will ruin our buffered data
	FRESULT res;

	uint32_t off = id*16;
	uint32_t off2;
	WORD br;
	WORD btr = 16;
	
	pf_lseek(off);
	res = pf_read(ram_tiles,btr,&br);
	if(res)
		SDCrash();

	off = ((ram_tiles[1]<<8)+(ram_tiles[2]));
	off2 = ((ram_tiles[4]<<8)+(ram_tiles[5]));
	pf_lseek(off);
	btr = (RAM_TILES_COUNT)*64;

	if(pf_read((BYTE *)ram_tiles,btr,&br)){
		SDCrash(0);
	}
	if(btr != br){
		SDCrash(0);
	}

	pf_lseek(off2);
	pf_read((BYTE *)vram,(VRAM_TILES_H*SCREEN_TILES_V),&br);
*/
}
