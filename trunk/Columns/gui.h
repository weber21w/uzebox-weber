void DrawTitleBG(){
	FillVram(RAM_TILES_COUNT);
	for(uint8_t i=0;i<11;i++){
		ColumnsDrawMap((i*4)+0,0,6,4,0,TitleMap,-(RAM_TILES_COUNT+1));
		ColumnsDrawMap((i*4)-2,22,6,4,0,TitleMap,-(RAM_TILES_COUNT+1));
	}

}
void DrawTitleBGFont(){
	DrawTitleBG();
	rtl_Print(11,8,PSTR("COLUMNS"));
}



bool CheckIdleTicks(uint16_t t){//if idle too long, go into demo mode
if(t > TITLEIDLETIME){
	return true;
}
return false;
}


void DoUzenetMenu();

void DoTitle(){
		if(cpuskill > 9)
			cpuskill = 9;
#ifdef DEBUG
		game_mode = 0;
		game_state = INITIALSTATE;
		cpustate = CPUFIRSTTICK;
		return;
#endif

	FadeOut(1,true);
	//rtl_RamifyFontEx(10,menucharmap,charlist,compfont,0,0xFF,false);
	rtl_RamifyFontEx(10,titlecharmap,charlist,compfont,0,0x37,true);
	FadeIn(4,false);

	uint16_t toff = 0;
	uint8_t t;
	for(uint16_t i=0;i<10*64;){//ramify banner tiles
		t = pgm_read_byte(&BannerTiles[toff++]);
		ram_tiles[i++] = pgm_read_byte(&BannerPallet[(t&0b01110000)>>4]);
		ram_tiles[i++] = pgm_read_byte(&BannerPallet[t&0b00001111]);
	}		
	musicspeed = 0;
	SetSongTempo(musicspeed);
	uint8_t flash,o;
	uint16_t idleticks;
titletop:
	flash = 30;
	cursorpos = 0;
	o = 0;
	idleticks = 0;
 

	for(int i=10*64;i<19*64;i++)
		if(ram_tiles[i])
			ram_tiles[i] = 255;


	UpdatePads();
	DrawTitleBGFont();//do here to avoid possible previous start down artifacts


	while(!StartDown(0)){
		UpdatePads();
		DrawTitleBGFont();

		if(flash < 15)
			rtl_Print(9,14,PSTR("PRESS START"));

		if(!--flash)
			flash = 30;
shuffle_prng();
		if(padstate[0])
			idleticks = 0;
		if(CheckIdleTicks(idleticks++)){//play demo
			FadeOut(1,true);
			SetInitialState();
			game_state = GS_DEMO|GS_PLAYING|GS_FIRSTTICK;
			state[0] = GOAHEAD;
			game_mode = 0;
			demo_off = 0;
			if(++demo_num > NUMDEMOS)
				demo_num = 1;
			demo_inp_wait = 255;
			cpustate = CPUFIRSTTICK;
			lfsr[0] = lfsr[1] = pgm_read_word(&DemoLfsr[demo_num]);
			for(uint8_t i=0;i<demo_num;i++)//get to start of demo
				while(pgm_read_byte(&DemoData[demo_off++]) != 255){};
			return;
		}			
		WaitVsync(2);//keep ticks even
	}

	for(uint8_t i=0;i<12;i++){
		UpdatePads();
		ColumnsDrawMenu(9,14,11,i,10);
		rtl_Print(12,14,PSTR("SELECT"));
		if(i > 3){
			rtl_Print(12,17,PSTR("SINGLE"));
		//	rtl_Print(10,18,PSTR("<"));
		}
		if(i > 5)
			rtl_Print(12,19,PSTR("VERSUS"));
		if(i > 7){
			rtl_Print(12,21,PSTR("LVL"));
			ColumnsPrint1num(17,21,cpuskill);
		}
		if(i > 9)
			rtl_Print(12,23,PSTR("UZENET"));
		
		//if(i > 9)
			//rtl_Print(12,24,PSTR("UZENET"));
		WaitVsync(4);
	}

	while(!StartDown(0) || cursorpos == 2){//title menu loop
		UpdatePads();
shuffle_prng();
		if(UpDown(0)){
		//	TriggerPCM(SWAPWAVE,255,255);
			if(cursorpos)
				cursorpos--;
			else
				cursorpos = 3;
		}
		if(DownDown(0)){
		//	TriggerPCM(SWAPWAVE,255,255);
			if(++cursorpos > 3)
				cursorpos = 0;
		}
		if(cursorpos == 2){
			if(LeftDown(0)  && cpuskill > 1)
				cpuskill--;
			if(RightDown(0) && cpuskill < 9)
				cpuskill++;
		}
		ColumnsDrawMenu(9,14,11,11,10);
		rtl_Print(12,14,PSTR("SELECT"));
		rtl_Print(12,17,PSTR("SINGLE"));
		rtl_Print(12,19,PSTR("VERSUS"));
		rtl_Print(12,21,PSTR("LVL"));ColumnsPrint1num(17,21,cpuskill);
		rtl_Print(12,23,PSTR("UZENET"));
		//rtl_Print(12,24,PSTR("UZENET"));

		rtl_Print(10,17+(2*cursorpos),PSTR(">"));

//shuffle_prng();
		if(padstate[0])
			idleticks = 0;
		if(CheckIdleTicks(idleticks++))
			goto titletop;
		WaitVsync(2);//keep ticks even
	}
	

	if(cursorpos == 0){//versus cpu
	//lfsr[0] = lfsr[1] = pgm_read_word(&DemoLfsr[1]);
		game_mode = 0;
		game_state = INITIALSTATE;
		cpustate = CPUFIRSTTICK;
	}
	else if(cursorpos == 1){
		game_mode = 1;//versus
		game_state = INITIALSTATE;
		cpustate = 0;
	}
	else if(cursorpos == 3){
	//	FadeOut(1,true);
		//FadeIn(1,false);

		//goto titletop;
		DoUzenetMenu();
	}
	FadeOut(2,true);
}

uint8_t UzenetInit(){
	return 0;
}

void DoUzenetMenu(){
	uint8_t ret;
	FadeIn(6,false);
	DrawTitleBG();
	rtl_Print(7,10,PSTR("CONNECTING..."));
	if((ret = UzenetInit())){
	//	for(uint8_t i=7;i<12+10;i++)
		//	vram[(17*VRAM_TILES_H)+i] = RAM_TILES_COUNT;
		rtl_Print(7,17,PSTR("ERROR"));ColumnsPrint1num(13,17,ret);
		while(true){
			UpdatePads();
			if(StartDown(0))
				return;
		}
		return;
	}
	
	rtl_Print(10,0,PSTR("PLAYERS    PING"));
	ColumnsDrawMenu(0,4,11,12,10);
	while(true){};
}