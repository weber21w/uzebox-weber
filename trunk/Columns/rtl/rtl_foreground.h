#ifdef RTL_FOREGROUND
#define RTL_BIND_POSTVSYNC 1
#define RTL_POSTVSYNC_SPRITE_BLIT 1

extern unsigned char ram_tiles[];
extern struct BgRestoreStruct ram_tiles_restore[];
extern unsigned char free_tile_index;

#ifndef RTL_FOREGROUND_SAFETY
#define RTL_FOREGROUND_SAFETY 0//default no vsync safety(have some free ram...)
#endif

#ifndef RTL_VSYNC_WAIT//0-skip effect if out of time 1-wait vsync and finish effect
#define RTL_VSYNC_WAIT 0
#endif

uint8_t * rtl_ForegroundTiles;//pointer to foreground actual tile data
uint8_t * rtl_ForegroundMap;
uint8_t rtl_ForegroundXscroll;//offsets into map
uint8_t rtl_ForegroundYscroll;
uint8_t rtl_ForegroundStart;//first tile index to consider a foreground
uint8_t rtl_ForegroundEnd;//last tile index to consider a foreground


void __attribute__((always_inline)) rtl_ForegroundBlit(){
	if(rtl_ForegroundTiles == NULL)
		return;	

	//if(rtl_Safety())
	//	return;

	for(uint8_t i=0;i<free_tile_index;i++){

		//if(rtl_Safety())
		//	return;
		
		uint8_t t;
		uint16_t roff,toff;

		if(rtl_ForegroundMap != NULL){
			t = pgm_read_byte(&rtl_ForegroundMap[ram_tiles_restore[i].addr]);
			if(t){
				roff = i*64;
				toff = t*64;

				for(uint8_t j=0;j<64;j++){
					if((t = pgm_read_byte(&rtl_ForegroundTiles[toff++])) != RTL_FOREGROUND_COLOR_KEY)
						ram_tiles[roff] = t;
					roff++;
				}
				continue;
			}
		}

		if(rtl_ForegroundStart == 0 && rtl_ForegroundEnd == 0)//indices disabled
			continue;

		t = vram[ram_tiles_restore[i].addr]-RAM_TILES_COUNT;
//t++;//duck hunt specific hack
		if(t < rtl_ForegroundStart || t > rtl_ForegroundEnd)
			continue;
		else{
			t -= rtl_ForegroundStart;
			roff = i*64;
			toff = t*64;

			for(uint8_t j=0;j<64;j++){
				if((t = pgm_read_byte(&rtl_ForegroundTiles[toff++])) != RTL_FOREGROUND_COLOR_KEY)
					ram_tiles[roff] = t;
					roff++;
			}
			continue;
		}
	}


}


void rtl_SetForeground(const uint8_t * map, const uint8_t * tiles, uint8_t start, uint8_t end){
	rtl_ForegroundMap = (uint8_t *)map;
	rtl_ForegroundTiles = (uint8_t *)tiles;
	rtl_ForegroundStart = start;
	rtl_ForegroundEnd = end;
}



#endif//RTL_FOREGROUND
