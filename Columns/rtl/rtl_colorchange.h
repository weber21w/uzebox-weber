void rtl_IncColor(uint8_t start, uint8_t end, uint8_t options){
	uint16_t roff = start*64;
	uint8_t t;
	if(options & RTLBLUE){
		if(true){//options & RTLNOOVERFLOW){
			while(roff < end*64){
				t = (ram_tiles[roff] & 0b11000000)>>6;
				if(!t){
					roff++;
					continue;
				}
				//if(t < 4)
					t++;
				ram_tiles[roff] |= 0b11000000;
				ram_tiles[roff] ^= 0b11000000;
				ram_tiles[roff] |= t;
				roff++;
			}
		}
	}
}

void rtl_SwapColors(uint8_t start, uint8_t end, uint8_t source, uint8_t dest){
	uint16_t roff = start*64;
	while(roff < end*64){
		if(ram_tiles[roff] == source)
			ram_tiles[roff] = dest;
		roff++;
	}
}
