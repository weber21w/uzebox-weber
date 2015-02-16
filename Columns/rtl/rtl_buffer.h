#if RTL_BUFFER == 1
uint8_t rtl_BufferX,rtl_BufferY;
uint8_t rtl_BufferW,rtl_BufferH;

void rtl_CreateBuffer(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t firstindice){
	for(uint8_t y=y;y<y+h;y++)
	for(uint8_t x=x;x<x+w;x++){
		rtl_SetVram(x,y,firstindice++);
	}
}

void rtl_Plot(uint8_t x, uint8_t y, uint8_t c){

}

uint8_t rtl_GetPixel(uint8_t x, uint8_t y){

}

void rtl_Line(uint8_t sx, uint8_t sy, uint8_t ex, uint8_t ey){

}

void rtl_SetBufferMap(uint8_t mx, uint8_t my){
	uint16_t moff = 0;
	uint8_t w = pgm_read_byte(&map[moff++]);
	uint8_t h = pgm_read_byte(&map[moff++]);

	for(uint8_t y=my;y<my+h;y++)
	for(uint8_t x=mx;x<mx+w;x++)
		rtl_SetVram(x,y,pgm_read_byte(&map[moff++]));
}


#endif
