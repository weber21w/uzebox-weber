const char pallets[] PROGMEM = {1,1,1,1,};
const char ramgfx[] PROGMEM = {1,1,1,1,};
void Ramify4bpp(uint8_t t, uint8_t r, uint8_t l, uint8_t p){
	uint16_t roff = r*64;
	uint8_t px;
	for(uint16_t i=t*64;i<(t*64)+(l*64);i++){
		px = pgm_read_byte(&pallets[pgm_read_byte(ramgfx[i])]);
	}
}

void RamifyList(){

}
