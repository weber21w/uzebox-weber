int8_t * rtl_CharMap;
uint8_t rtl_FontStartOffset;
uint8_t rtl_PrintSpace = 255;

uint8_t rtl_FindChar(int8_t c, const int8_t *fmap){
	uint8_t pos = 0;

	int8_t t;
	while(true){
		t = pgm_read_byte(&fmap[pos]);

		if(t == 0 || pos == 255)
			return 255;

		if(t != c){
			pos++;
			continue;
		}
		return pos+rtl_FontStartOffset;
	}
}

//TODO ADD MINUS LIST WHERE ALL CHARS IN LIST WILL NOT BE RAMIFIED(EVEN IF THERE ARE IN THE CMAP)
void rtl_RamifyFontEx(uint16_t rtoff, const uint8_t *cmap, const uint8_t *ftiles, uint8_t backcolor, uint8_t fontcolor, uint8_t flags){
	uint8_t moff = 0;
	uint8_t t;
	int8_t c;
	rtl_CharMap = (int8_t *)cmap;
	rtl_FontStartOffset = rtoff;
	rtoff *= 64;

	while(rtoff < (RAM_TILES_COUNT*64)){
		c = pgm_read_byte(&cmap[moff++]);
		if(c == 0)
			return;

		c=((c&127)-('A'-6));		
	
		for(uint16_t i=(c*8);i<(c*8)+8;i++){
			t = pgm_read_byte(&ftiles[i]);
			for(u8 j=0;j<8;j++){
				if(t & (128>>j)){
					if(!(flags & RTLFONTSKIPFG))
						ram_tiles[rtoff] = fontcolor;
				}
				else if(!(flags & RTLFONTSKIPBG))
					ram_tiles[rtoff] = backcolor;
				rtoff++;
			}
		}
	}
}

void rtl_RamifyFont(uint16_t rtoff, uint8_t *cmap, uint8_t *ftiles){
	rtl_RamifyFontEx(rtoff,cmap,ftiles,0x00,0xFF,0);
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

void rtl_Print(uint8_t x, uint8_t y, const char *string){
	rtl_PrintEx(x,y,string,rtl_CharMap,rtl_FontStartOffset,0);
}

void rtl_PrintCharOverlaid(uint8_t x, uint8_t y, uint8_t c, uint8_t color){

}

void rtl_PrintOverlaid(uint8_t x, uint8_t y, const int8_t *string){
	uint8_t i=0;
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
	}
}
