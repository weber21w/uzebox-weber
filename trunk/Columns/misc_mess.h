extern void CpuFillPad();


inline void set_tile(uint8_t x, uint8_t y, uint8_t t){
	vram[(y*VRAM_TILES_H)+x] = t+RAM_TILES_COUNT;
}

void FillVram(uint8_t t){
	for(uint16_t i=0;i<VRAM_TILES_H*VRAM_TILES_V;i++)
		vram[i] = t;
}

extern uint8_t cpustate;
extern uint8_t cpuskill;
extern uint8_t cpumovewait;

void SetInitialState(){
	if(cpuskill > 9)//shouldnt be necessary? TEST WITH VICTORY AGAINST LEVEL 9 CPU!!
		cpuskill = 9;
	game_state = GS_PLAYING|GS_FIRSTTICK;
	masterVolume = MASTERVOLUME;
	if(game_mode == 0){
		cpustate = 16;//CPUFIRSTTICK;
		cpumovewait = 1;
	}
ramtilestate[0] = NEEDALLRAMMED;
	cursetime[0] = cursetime[1] = 2;
	RamTileStateMachine();
	cursetime[0] = cursetime[1] = 0;
	
	//if(!game_mode)
	//	cpustate = CPUFIRSTTICK;
	//else
		//cpustate = 0;
	level = !level;
	vsyncs = 0;
	DoSong();
		
	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		for(uint8_t j=0;j<3;j++)
			crashedtype[(i*3)+j] = 255;
		if(i){
			lfsr[i] = lfsr[i-1];
			piece[3+0] = piece[0];
			piece[3+1] = piece[1];
			piece[3+2] = piece[2];
			preview[3+0] = preview[0];
			preview[3+1] = preview[1];
			preview[3+2] = preview[2];
		}else{
			GeneratePreview(0);
			GeneratePreview(0);
		}			
		clear_well(i);
		piecey[i] = 0;
		piecex[i] = 2;
		piecerot[i] = 0;
		state[i] = GETREADY;
		statetimer[i] = 0;
		stateframe[i] = 0;
		droptimer[i] = DROPTIMER;
		crashcomingup[i] = 0;
		crashcomingdown[i] = 0;
		crashheight[i] = 0;
		wellfullness[i] = 0;
		sideheld[i] = 0;
		halfstep[i] = 0;
		wasmagicjewel[i] = 0;
		magicjewelssummoned[i] = 0;
		chain[i] = 0;
		score[i] = 0;
		jewels[i] = 0;
		flashingjeweltimer[i] = 0;
	}
	magicjewelsonscreen = 0;
	tickssincetempochange = 0;
	musicspeed = 0;
	//roundtimer = 0;
#ifdef DEBUG
lfsr[0] = lfsr[1] = 0xA42Fl;
GeneratePreview(0);GeneratePreview(0);GeneratePreview(0);
GeneratePreview(1);GeneratePreview(1);GeneratePreview(1);
#endif
}



uint16_t prng(uint8_t p){
	uint16_t bit;
  /* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
	bit  = ((lfsr[p] >> 0) ^ (lfsr[p] >> 2) ^ (lfsr[p] >> 3) ^ (lfsr[p] >> 5) ) & 1;
	lfsr[p] =  (lfsr[p] >> 1) | (bit << 15);
	return lfsr[p];
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
//PRINTING TYPE STUFF
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////




void ColumnsPrint2num(uint8_t x, uint8_t y, uint16_t val){
	if(val > 99)
		val = 99;

	set_vram(x--,y,(val%10)+RAM_TILES_COUNT+41);
//	if(val/10)
		set_vram(x,y,(val/10)+RAM_TILES_COUNT+41);	
//	else
//		SetTile(x,y,10);
}

void ColumnsPrint3num33(uint8_t x, uint8_t y, uint16_t val){
	if(val > 999)
		val = 999;

	set_vram(x--,y,(val%10)+RAM_TILES_COUNT+41);
	if(val/10)
	set_vram(x--,y,(val/10)+RAM_TILES_COUNT+41);
	if(val/100)
	set_vram(x,y,(val/100)+RAM_TILES_COUNT+41);
//	else
//		SetTile(x,y,10);
}

void ColumnsPrint3num(uint8_t x,uint8_t y, uint16_t val){
	unsigned char c,i;

	for(i=0;i<4;i++){
		c=val%10;
		if(val>0 || i==0){
			set_vram(x--,y,c+41+RAM_TILES_COUNT);
		}else{
				set_vram(x--,y,0+RAM_TILES_COUNT);
		}
		val=val/10;
	}
	
}

inline void ColumnsPrint1num(uint8_t x, uint8_t y, int val){
	set_vram(x,y,RAM_TILES_COUNT+41+val);
}



void ColumnsPrintLong(uint8_t x,uint8_t y, unsigned long val){
	unsigned char c,i;

	for(i=0;i<10;i++){
		c=val%10;
		if(val>0 || i==0){
			ColumnsPrint1num(x--,y,c);//SetFont(x--,y,c+CHAR_ZERO+RAM_TILES_COUNT);
		}else{
		//	ColumnsPrint1num(x--,y,0);
		}
		val=val/10;
	}
		
}


/*void ColumnsPrint(int16_t x, int16_t y, char *string, int16_t offset){
	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));
		if(c==' '){
			x++;
			continue;
		}		
		if(c==0)
			break;

		c=((c&127)-32)+(offset);			
		SetTile(x++,y,c);
	}
}*/

void ColumnsDrawMap(int16_t sx, int16_t sy, uint8_t w, uint8_t h, uint16_t o, const char *map, int16_t toff){
	int16_t t;
	for(int16_t y=sy;y<sy+h;y++){
		if(y > VRAM_TILES_V-1 || y < 0){
			map += w;
			continue;
		}
		for(int16_t x=sx;x<sx+w;x++){
			if(x < VRAM_TILES_H && x > -1){
				t = pgm_read_byte(map+o);
				if(!t)
				t = -toff;

				vram[(y*VRAM_TILES_H)+x] = RAM_TILES_COUNT+t+toff;
			}
			map++;
		}
	}
}

inline void SetVram(uint8_t x, uint8_t y, uint8_t t){
	vram[(y*SCREEN_TILES_H)+x] = t;
}

void ColumnsDrawMenu(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t o){
	for(u8 i=0;i<w;i++)
	for(u8 j=0;j<h;j++)
	set_tile(x+i,y+j,0);

	for(u8 i=0;i<w;i++){
		SetVram(x+i,y,o+6);
		SetVram(x+i,y+h,o+7);
	}
	for(u8 i=0;i<h;i++){
		SetVram(x,y+i,o+4);
		SetVram(x+w,y+i,o+5);
	}
	SetVram(x,y,o+0);
	SetVram(x+w,y,o+1);
	/*
	//smooth the nenu corners, nasty code
	uint16_t tv;
	uint8_t t;
	tv = vram[x+((y+h)*VRAM_TILES_H)];
	if(tv < RAM_TILES_COUNT){
		tv *= 64;
		for(uint8_t i=0;i<64;i++){
			t = ram_tiles[(2*64)+i];//save graphics for lower left menu tile
			ram_tiles[(2*64)+i] = ram_tiles[tv++];//copy over pixels from tile underneath
			if(cpubuffer[i])
		}
	}*/
	SetVram(x,y+h,o+2);
	SetVram(x+w,y+h,o+3);

}





