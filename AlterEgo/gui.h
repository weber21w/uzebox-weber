//extern uint8_t demotoplay;//TODO

inline void TitleScreen(void){//inline WAS larger
	DDRC = 0;
	RamifyFromSD(SD_SCREEN_TITLE,0);
	StartSong((const char *)pgm_read_word(&musicData[MUS_TITLE]));

	FadeIn(5,true);

	uint8_t j=0;
	uint8_t atr=16;
	frame_cnt=0;
	uint8_t px=0;
	uint8_t py=0;
	uint16_t idle_ticks = AE_TITLE_IDLE_TIME;
	demo_state = DEMO_OFF;

//goto SOUNDTESTHACK;
	while(1){
		WaitVsync(1);
		if(!(--idle_ticks)){//play a demo
			demo_state = DEMO_PENDING;
			//py = 0;//could happen
			break;
		}
		if(frame_cnt&atr){//hide "PRESS START"
			for(uint16_t rt=(20*VRAM_TILES_H);rt<VRAM_SIZE;rt++)
				vram[rt] = FNTSTRT;
		}else
			AEPrintBigRam(9,20,"PRESS START?");

		if(!j){
			//UpdatePad();Done in VsyncRoutine now
			if(padstate){
//if(padstate == BTN_SL && !(oldpadstate & BTN_SL)){demotoplay++;}//TODO
				idle_ticks = AE_TITLE_IDLE_TIME;
				if(!oldpadstate){
					if(pgm_read_word(&code_str[px])!=padstate)
						px=0;
					else{
						px++;
						if(px==(sizeof(code_str)/2)){
							px=0;
							level_skip=1;
							py = 1;
							AETriggerFx(SFX_MENU);
						}
					}
				}
			}

			if(padstate & BTN_START && !(oldpadstate & BTN_START)){
				StopSong();
				if((padstate & BTN_A) && (padstate & BTN_B) && py == 1)
					py=2;

				frame_cnt=4;
				j=50;
				atr=4;
				StopSong();
				AETriggerFx(SFX_START);
			}
		}else{
			j--;
			if(!j) break;
		}

		frame_cnt++;
//		if(nextDeltaTime > 8000)
//			StopSong();
	}

	if(py == 2){//sound test
//SOUNDTESTHACK:
		j=0;
		px=0;
		py=0;
		uint8_t soundtestflashcount = 0;
		while(1){
			WaitVsync(1);
			//UpdatePad();Done in VsyncRoutine now
			if(padstate&BTN_START && !(oldpadstate & BTN_START))
				break;

			if((padstate&BTN_LEFT) &&!(oldpadstate & BTN_LEFT))
				j=0;
			if((padstate&BTN_RIGHT) &&!(oldpadstate & BTN_RIGHT))
				j=1;

			if((padstate&BTN_UP) && !(oldpadstate & BTN_UP)){
				if(!j){
					if(px<SFX_ALL-1)
						px++;
				}else if(py<MUS_ALL-1)
					py++;
			}

			if((padstate & BTN_DOWN) && !(oldpadstate && BTN_DOWN)){
				if(!j){
					if(px)
						px--;
				}else if(py)
					py--;
			}

			if((padstate & BTN_A) && !(oldpadstate && BTN_A))
				StopSong();

			if((padstate & BTN_B) && !(oldpadstate && BTN_B)){
				if(!j)
					AETriggerFx(px);
				else
					StartSong((const char *)pgm_read_word(&musicData[py]));
			}
			for(uint8_t erasei=0;erasei<VRAM_TILES_H;erasei++)
				vram[(VRAM_TILES_H*20)+erasei] = FNTSTRT;
			
			soundtestflashcount++;
			AEPrintByte(13,20,px,false);
			AEPrintByte(20,20,py,false);
			if(((soundtestflashcount&5) < 3) ||  j){AEPrint(10,20,strFX);}
			if(((soundtestflashcount&5) < 3) || !j){AEPrint(16,20,strBGM);}
/*
			uint8_t tfc1=RAM_TILES_COUNT-1;
			uint16_t tfc2,tfc3;
			for(tfc2=VRAM_TILES_H*20;tfc2<VRAM_SIZE;tfc2++){
				if(vram[tfc2] >= FNTSTRT+58 && vram[tfc2] <= FNTSTRT+67){//found a number
					tfc3 = (vram[tfc2]-RAM_TILES_COUNT)*64;//get index into tile data
					for(uint8_t tfc4=0;tfc4<64;tfc4++)//copy 64 pixels to ram tile
						ram_tiles[(tfc1*64)+tfc4] = pgm_read_byte(&font_tiles[tfc3++]);
					vram[tfc2] = tfc1--;//put the ram tile index into vram and point to next ram tile to use for next number 
				}
			}
			
			uint8_t tfm = 0b11001111;
			frame_cnt++;
			tfm &= (frame_cnt&0b00000111);//((frame_cnt&0b00001111)|0b11000000);
			for(uint8_t tfc4=0;tfc4<64;tfc4++){
				if(!j){
					ram_tiles[((RAM_TILES_COUNT-1)*64)+tfc4] &= tfm;
					ram_tiles[((RAM_TILES_COUNT-2)*64)+tfc4] &= tfm;
				}else
					ram_tiles[((RAM_TILES_COUNT-3)*64)+tfc4] &= tfm;
			}
			*/
				
		}
	}
	FadeOut(2,true);
}


inline void IntroScreen(){
	DDRC = 0;
	RamifyFromSD(12,0);
	FadeIn(3,true);
	WaitVsync(7);
	RamifyFromSD(11,1);
	WaitVsync(7);
	RamifyFromSD(10,1);
	WaitVsync(101);

//	RamifyFromSD(10,0);
//	FadeIn(3,true);
//	WaitVsync(109);

//	FadeOut(1,true);
	AEClearVram();
	bool firstrun = true;
//while(1){//THIS IS THE PROBLEM PART
	for(uint8_t j=0;j<10;j++){
		for(uint8_t i=4;i<9;i++){
			RamifyFromSD(i,firstrun);
			firstrun = false;
			WaitVsync(2);
		}
	}
//}
	RamifyFromSD(9,0);
	AETriggerFx(SFX_EXCHANGE);
	WaitVsync(180);
	FadeOut(1,true);
}


inline void GameOverScreen(){
/*
//inline WAS larger
return;//removed whole "5 lives" concept from game. code is all still there just commented out around the source files.
	StopSong();
	WaitVsync(1);
	StartSong(musicData[MUS_GAMEOVER]);
	RamifyFromSD(SD_SCREEN_GAMEOVER);
	FadeIn(2,true);

	uint16_t i16=10*50;

	while(1){
		WaitVsync(1);
		//UpdatePad();Done in VsyncRoutine now
		if(!i16||(padstate & BTN_START && !(oldpadstate & BTN_START))) break;
		i16--;
	}

	StopSong();
	FadeOut(2,true);
*/
}



inline void WellDoneScreen(){//inline WAS larger
DDRC=0;
	StopSong();
	WaitVsync(1);
	ResetSprites();
	RamifyFromSD(SD_SCREEN_WELLDONE,0);
	StartSong(musicData[MUS_WELLDONE]);

	for(uint16_t voff=0;voff<VRAM_SIZE;voff++){
		if(vram[voff] == 5)//black ram tile
			vram[voff] = FNTSTRT;//point to black tile in flash so sprites don't smear
		else
			vram[voff] += 8;
	}
	for(uint16_t roff=RAM_TILES_COUNT*64;roff>8*64;roff--)
		ram_tiles[roff] = ram_tiles[roff-(8*64)];

	FadeIn(3,false);
	scrambler_state = 0;//we just want the player colors corrected


	player_x1=16;
	player_x2=240-16-8;
	player_y1=152-8-8;
	player_y2=152-8-8;
	frame_cnt = 0;
	bool done = false;
	while(masterVolume){
		ResetSprites();
		uint8_t sof = 0;
		if(frame_cnt>=50&&frame_cnt<58) {player_y1--;sof=1;}
		else if(frame_cnt>=58&&frame_cnt<66) {player_y1++;sof=2;}
		else if(frame_cnt>=100&&frame_cnt<108) player_y2--;
		else if(frame_cnt>=108&&frame_cnt<116) player_y2++;
		DrawMetaSprite(player_x1,player_y1,PLAYER_SPR_IDLE+sof,SPRITE_FLIP_X);
		DrawMetaSprite(player_x2,player_y2,PLAYER_SPR_ALTER+(prng()&3),0);


		if(frame_cnt&16){
			for(uint16_t vo=VRAM_TILES_H*22;vo<VRAM_SIZE;vo++)//blank out text
				vram[vo] = FNTSTRT;
		}else
			AEPrintBigRam(7,22,"ALL LEVELS CLEAR[?");

		//UpdatePad();Done in VsyncRoutine now
		if(!done && (padstate&BTN_START) &&!(oldpadstate&BTN_START)){
			AETriggerFx(SFX_MENU);
			FadeOut(16,false);
			done = true;
		}
		frame_cnt=(frame_cnt+1)&127;

		if(done){
			for(uint16_t voff=22*VRAM_TILES_H;voff<VRAM_SIZE;voff++)
				vram[voff] = FNTSTRT;
			AEPrintBigRam(6,22,"THANKS FOR PLAYING[?");

			if(masterVolume > AE_VOL_FADE_STEP*2){
				if(masterVolume < AE_MASTER_VOL/2)
					masterVolume -= AE_VOL_FADE_STEP*2;
				else
					masterVolume -= AE_VOL_FADE_STEP;
			}else
				masterVolume = 0;
		}
		WaitVsync(1);
	}

	StopSong();
	while(DDRC){};//wait until it's totally faded
	AEClearVram();
	ResetSprites();
	AEPrintBigRam(6,22,"THANKS FOR PLAYING[?");
	FadeIn(2,false);
	WaitVsync(240);
	FadeOut(3,true);
	SetMasterVolume(AE_MASTER_VOL);
}


inline void CreditScreen(){
	DDRC=0;

	RamifyFromSD(SD_SCREEN_CREDITS,0);
	FadeIn(2,true);
	oldpadstate = ReadJoypad(0);
	for(uint8_t i=0;i<240;i++){
		WaitVsync(1);
		//if((padstate&BTN_START && !(oldpadstate & BTN_START))) break;//forced to appreciate the work people put into it :)
	}

	FadeOut(4,true);
}

inline void CycloneEffect(uint8_t cursorpos, uint8_t guiframe){
	ResetSprites();
	AEClearVram();
	scrambler_state = SCRAMBLER_OFF;

	for(uint8_t i=0;i<8;i++){
		DrawMetaSprite((i*8),(10*8)+(cursorpos*2*8)-0,(((guiframe/4)+i+30)%8)+30,0);
		DrawMetaSprite((i*8),(10*8)+(cursorpos*2*8)+4,(((guiframe/4)+i+33)%8)+38,0);
		DrawMetaSprite((i*8),(11*8)+(cursorpos*2*8)+0,(((guiframe/4)+i+36)%8)+30,0);
	}

	for(uint8_t i=0;i<SCREEN_TILES_H;i++){
		vram[(VRAM_TILES_H*(10+(cursorpos*2)))+i] = (i%8);
		vram[(VRAM_TILES_H*(11+(cursorpos*2)))+i] = (i%8)+8;
	}
}

inline void LevelSelectScreen(){
//	DDRC = 0;
	music_prev = 255;
	uint8_t level_choice = LevelProgress(0);
	uint8_t max_level = level_choice; 

	if(!level_choice){//no saved progress, skip to first level
		level = 0;
		goto LEVELSELECTDONENOFADE;
	}

	WaitVsync(1);
	FadeIn(2,false);
	uint8_t cursorpos = 0;
	uint8_t guiframe = 0;

	while(1){
		CycloneEffect(cursorpos,guiframe);

		AEPrintBigRam(11,10,"CONTINUE?");
		AEPrintBigRam(11,12,"NEW GAME?");
		AEPrintBigRam(11,14,"ERASE?");

		WaitVsync(1);

		for(uint16_t i=0;i<16*64;i++)//clear old sprite pixels
			ram_tiles[i] = 0;

		if(++guiframe > (8*2)*4)
			guiframe = 0;	
		if((padstate & BTN_START) && !(oldpadstate & BTN_START)){
			AETriggerFx(4);
			if(cursorpos == 1){//new game
				level = 0;
				goto LEVELSELECTDONE;
			}else if(cursorpos == 0){//continue
				oldpadstate = padstate;
		//		ResetSprites();
		//		AEClearVram();
				FadeOut(2,true);//DDRC = 0;
				ResetSprites();
				WaitVsync(1);
				break;
			}else{//erase
				WaitVsync(20);
				AETriggerFx(SFX_FALLING);
				LevelProgress(255);//erase the save data for the episode we are currently on
				level = 0;
				goto LEVELSELECTDONE;
			}
		}
		if((padstate & BTN_UP) && !(oldpadstate & BTN_UP) && cursorpos){
			cursorpos--;
			AETriggerFx(3);
		}
		if((padstate & BTN_DOWN) && !(oldpadstate & BTN_DOWN) && cursorpos < 2){
			cursorpos++;
			AETriggerFx(3);
		}
	}
	
//	scrambler_state = SCRAMBLER_FOREGROUND|SCRAMBLER_SKULL_EYES;
	bool reload = true;
	bool playlocked = false;
	frame_cnt = 0;
	while(1){
		//UpdatePad();Done in VsyncRoutine now
		if(padstate & BTN_START && !(oldpadstate & BTN_START))
			goto LEVELSELECTDONE;
		else if((padstate & BTN_LEFT) && !(oldpadstate & BTN_LEFT)){
			if(level_choice){
				level_choice--;
				reload = true;
			}else
				playlocked = true;
		}else if((padstate & BTN_SL) && !(oldpadstate & BTN_SL)){
			if(level_choice){
				if(level_choice < 5){level_choice = 0;}else{level_choice -= 5;}
					reload = true;
			}else
				playlocked = true;
		}else if((padstate & BTN_RIGHT) && !(oldpadstate & BTN_RIGHT)){
			if(level_choice < max_level){
				level_choice++;
				reload = true;
			}else
				playlocked = true;
		}else if(padstate & BTN_SR && !(oldpadstate & BTN_SR)){
			if(level_choice != max_level){
				if(level_choice > max_level-5){level_choice = max_level;}else{level_choice += 5;}
				reload = true;
			}else
				playlocked = true;
		}

		frame_cnt++;
		//sync alter ego TODO HACK THERE IS A SMALLER WAY TO DO THIS...
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

		level_clear_delay = 0;//why is this needed??!
		Render();

		if(playlocked){
			playlocked = false;
			AETriggerFx(SFX_NO_EXCHANGE);
		}

		//uint16_t rtrest = (RAM_TILES_COUNT-1)*64;
			for(uint16_t i=8;i<VRAM_TILES_H;i++){
				vram[i] = FNTSTRT;
				vram[i+VRAM_TILES_H] = FNTSTRT;
			}		
		if(frame_cnt&32){//save tiles underneath before printing
		//	for(uint16_t ix=(SCREEN_TILES_V/2)*SCREEN_TILES_H;ix<((SCREEN_TILES_V/2)*SCREEN_TILES_H)+(2*SCREEN_TILES_H);ix++)
		//		ram_tiles[rtrest++] = vram[ix];
				//if(frame_cnt&64)
				//	AEPrintBigRam(8,0,"THEN PRESS START?");
				//else
					AEPrintBigRam(9,0,"LEVEL SELECT?");
		}

		if(reload){
			reload = false;
			DDRC = 0;
			level = level_choice;

			GameAddBackground(0);
			//restart = level_choice+1;
			UpdateStats(1);
			for(uint8_t ixo=10;ixo<VRAM_TILES_H;ixo++){
				vram[ixo] = FNTSTRT;
				vram[ixo+VRAM_TILES_H] = FNTSTRT;
			}
			//vram[(0*VRAM_TILES_H)+1] = FNTSTRT+11;//print L in big letter form
			//vram[(1*VRAM_TILES_H)+1] = FNTSTRT+16;
			FadeIn(1,false);
		}
	}
LEVELSELECTDONE:
	FadeOut(3,true);
LEVELSELECTDONENOFADE:
	restart = 5;
}





inline uint8_t InGamePauseMenu(){
	StopSong();
	if(demo_state == DEMO_ON){//a BTN_START in demo data means quit demo and return to title screen
		level_done = DONE_DEMODONE;
		return 1;
	}
//	WaitVsync(12);//menu open sound can be interrupted right away by menu blip...oh well i dont like the delay..
	DDRC = 0b10010001;
	AETriggerFx(SFX_START);
	UpdateStats(0);
	AEPrint(2,VRAM_FIRST_MAP_Y-4,strRESUME);
	if(restart){
		AEPrint(2,VRAM_FIRST_MAP_Y-3,strRESTART);
		AEPrint(2,VRAM_FIRST_MAP_Y-2,strEXIT);
	}else
		AEPrint(2,VRAM_FIRST_MAP_Y-3,strEXIT);
	uint8_t px=0;
	uint8_t py=0;
	while(1){
		for(uint16_t cc=(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-4))+1;cc<(VRAM_TILES_H*(VRAM_FIRST_MAP_Y-1))+1;cc+=VRAM_TILES_H)//clear cursor tile
			vram[cc] = FNTSTRT;

		if(!(px&16))
			vram[(VRAM_TILES_H*((VRAM_FIRST_MAP_Y-4)+py))+1] = FNTSTRT+42;//draw cursor
				
		WaitVsync(1);
		//UpdatePad();Done in VsyncRoutine now
		if((padstate & BTN_START && !(oldpadstate & BTN_START))) break;
		//if(restart){
			if((padstate & BTN_UP && !(oldpadstate & BTN_UP))){
				if(py!=0){
					AETriggerFx(SFX_MENU);
					py--;//py=15;
					px=0;
				}
			}
			if((padstate & BTN_DOWN && !(oldpadstate & BTN_DOWN))){
				if(py!=(1+(restart > 0))){
					AETriggerFx(SFX_MENU);
					py++;//py=23;
					px=0;
				}
			}
		//}
			px++;
		}
		WaitVsync(8);//menu blip sound needs to be higher priority than menu open sound so first blip doesnt get blocked out. this lets the last menu blip end if they pushed start right away...details matter
		AETriggerFx(SFX_START);
		
	if(py == 0){
		UpdateStats(1);
		DDRC = 255;
		ResumeSong();
	}else if(py == 1 && restart){
		level_done = DONE_RESTART;
		//StopSong(); not needed? done inside LoadLevel() if music_prev == 255
		music_prev=255;
	}else{//exit to title or level select screen
		UpdateStats(1);
		DDRC = 255;
		restart = 255;
		FadeOut(3,true);
		return 1;
	}
	WaitVsync(1);
	return 2;//continue;
	//return 0;//keep going
}
