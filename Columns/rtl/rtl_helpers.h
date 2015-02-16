#ifdef RTL_DEBUG
//DEBUG STUFF
void beep(){
TriggerFx(0,255,true);
}
void haltbeep(bool t){
	while(t)
		beep();
}
#endif

void rtl_SetVram(uint8_t x, uint8_t y, uint8_t t){
	vram[(y*VRAM_TILES_H)+x] = t;
}

void rtl_SetVramAddr(uint16_t a, uint8_t t){
	vram[a] = t;
}

void rtl_FillVramSequential(uint8_t sx, uint8_t sy, uint8_t w, uint8_t h, uint8_t start){
	for(uint8_t y=sy;y<sy+h;y++)
	for(uint8_t x=sx;x<sx+w;x++)
		vram[(y*VRAM_TILES_H)+x] = start++;
}

bool __attribute__((always_inline)) rtl_Safety(){
#if RTL_VSYNC_SAFETY > 0
	if(GetVsyncFlag()){if(RTL_VSYNC_WAIT){WaitVsync(1);return false}else return true;}//either slow the game down, or skip effects
#endif
	return false;
}

void rtl_RamifyTile(uint8_t ramtile, uint8_t tile, const uint8_t * tiles){
	uint16_t roff,toff;
	roff = ramtile*64;
	toff = tile*64;
	uint8_t t;

	for(uint8_t i=0;i<64;i++){
		t = pgm_read_byte(&tiles[toff++]);
		if(t != 0xFE)
			ram_tiles[roff] = t;
		roff++;
	}
}

void rtl_RamifyTile4bpp(uint8_t r, uint8_t t, const uint8_t * tiles){
}

void rtl_RamifyTile2bpp(uint8_t r, uint8_t t, const uint8_t * tiles){
}

void rtl_RamifyTile1bpp(uint8_t r, uint8_t t, const uint8_t * tiles){
}

void rtl_RamifyTileRLE(uint8_t r, uint8_t t, const uint8_t * tiles){
}
