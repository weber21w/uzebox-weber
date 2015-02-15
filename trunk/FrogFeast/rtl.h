extern uint8_t ram_tiles[];

uint8_t rtl_FontStartOffset;
int8_t *rtl_CharMap;

inline void rtl_SetVram(uint8_t x, uint8_t y, uint8_t t){
	vram[(y*VRAM_TILES_H)+x] = t;
}

uint8_t rtl_FindChar(int8_t ch, const int8_t *fmap){
	uint8_t pos = 0;

	int8_t t;
	while(true){
		t = pgm_read_byte(&fmap[pos]);

		if(t == 0 || pos == 255)
			return 255;

		if(t != ch){
			pos++;
			continue;
		}
		return pos+rtl_FontStartOffset;
	}
}


uint8_t rtl_SearchCharList(int8_t ch, const int8_t *chrlst){
	int8_t pos=0,t;
	while(true){
		t = pgm_read_byte(&chrlst[pos]);
		if(t == 0 || t == 255)
			return 255;
		if(t == ch)
			return pos;
		pos++;
	}
}

void rtl_RamifyFontEx(uint16_t rtoff, const int8_t *cmap, const int8_t *chrlst, const uint8_t *ftiles, uint8_t backcolor, uint8_t fontcolor){
WaitVsync(1);
	uint8_t moff = 0;
	uint8_t t,t2;
	uint8_t c;
	rtl_FontStartOffset = rtoff;
	rtl_CharMap = (int8_t *)cmap;
	rtoff *= 64;
	uint8_t iteration = 0;

	while(rtoff < (RAM_TILES_COUNT*64)){
		c = pgm_read_byte(&cmap[moff++]);
		if(c == 0)
			return;
		

		t2 = rtl_SearchCharList(c,chrlst);

		if(t2 == 255){
			return;	
		}

		for(uint16_t i=(t2*8);i<(t2*8)+8;i++){
			t = pgm_read_byte(&ftiles[i]);
			for(u8 j=0;j<8;j++){
				if(t & (128>>j)){
						ram_tiles[rtoff] = fontcolor;
				}
				else
					ram_tiles[rtoff] = backcolor;
				rtoff++;
			}
		}
	if(++iteration > 1)
		WaitVsync(1);	
	}
}


void rtl_PrintEx(uint8_t x, uint8_t y, const char *string, const int8_t *cmap, uint8_t offset, uint8_t flags){
	uint8_t t,i=0;
	int8_t c;

	while(true){
		c = pgm_read_byte(&string[i++]);
		
		if(c == 0)
			return;

		if(x > VRAM_TILES_H-1){
			x++;
			continue;
		}		
		else if(c == ' '){
			//if(blank != 255)
		//	rtl_SetVram(x,y,blank);
			x++;
			continue;
		}

		t = rtl_FindChar(c,cmap);

		if(t > RAM_TILES_COUNT){
			x++;
			continue;
		}

		rtl_SetVram(x++,y,t);
	}
}



void rtl_PrintRamEx(uint8_t x, uint8_t y,char *string, const int8_t *cmap, uint8_t offset, uint8_t flags){
	uint8_t t,i=0;
	int8_t c;

	while(true){
		c = string[i++];
		
		if(c == 0)
			return;

		if(x > VRAM_TILES_H-1){
			x++;
			continue;
		}		
		else if(c == ' '){
			//if(blank != 255)
		//	rtl_SetVram(x,y,blank);
			x++;
			continue;
		}

		t = rtl_FindChar(c,cmap);

		if(t > RAM_TILES_COUNT){
			x++;
			continue;
		}

		rtl_SetVram(x++,y,t);
	}
}


void rtl_Print(uint8_t x, uint8_t y, const char *string){
	rtl_PrintEx(x,y,string,rtl_CharMap,rtl_FontStartOffset,0);
}

void rtl_PrintRam(uint8_t x, uint8_t y,char *string){
	rtl_PrintRamEx(x,y,string,rtl_CharMap,rtl_FontStartOffset,0);
}

inline void rtl_Print1num(uint8_t x, uint8_t y, uint8_t val){
	vram[(y*VRAM_TILES_H)+x] = 12+val;
}

void rtl_Print2num(uint8_t x, uint8_t y, uint16_t val){
	if(val > 99)
		val = 99;

	rtl_Print1num(x--,y,(val%10));
	rtl_Print1num(x,y,(val/10));	
}

void rtl_Print3num(uint8_t x, uint8_t y, uint16_t val){
	if(val > 999)
		val = 999;

	rtl_Print1num(x--,y,(val%10));
	rtl_Print1num(x--,y,(val%100)/10);
	rtl_Print1num(x,y,(val/100));
}

void rtl_Print4num(int x,int y, unsigned long val){
	unsigned char c,i;

	for(i=0;i<4;i++){
		c=val%10;
		if(val>0 || i==0){
			rtl_Print1num(x--,y,c);
		}else{
			rtl_Print1num(x--,y,0);
		}
		val=val/10;
	}
		
}
