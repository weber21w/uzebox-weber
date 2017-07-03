void Render(){
//TODO should be able to eliminate ram_tiles_restore[]...but it will be slow/complicated(think about it?)
	uint8_t t = pframe;
	
	if(pstate & UP){t=t;}//frame 6 is blink frame
	else if(pstate & DOWN){t+=6;}
	else if(pstate & LEFT){t+=12;}
	else{t+=24;}
	if(pstate & MOVING)
		t += pframe;
	else if(!pblink)
		t += 6;//frame 6 is blink frame
		
		

	for(uint8_t i=0;i<MAX_ENEMIES;i++){
		t = get_enemy_type(i);
	}
}

void DrawBorder(uint8_t set){
	for(uint8_t i=0;i<22;i+=2){
		DrawMapTile(FIRSTMAPX- 2,FIRSTMAPY+ i,67);//left side
		DrawMapTile(FIRSTMAPX+22,FIRSTMAPY+ i,67);//right side
		DrawMapTile(FIRSTMAPX+ i,FIRSTMAPY- 2,64+(i==0?0:1));//top(handle shadow)
//		DrawMapTile(FIRSTMAPX+ i,FIRSTMAPY+22,66);//bottom handled differently
		vram[FIRSTMAPX+i+0+((FIRSTMAPY+22)*VRAM_TILES_H)] = pgm_read_byte(&TileFrames[(4*66)+0]);//screen is 25 tiles high...
		vram[FIRSTMAPX+i+1+((FIRSTMAPY+22)*VRAM_TILES_H)] = pgm_read_byte(&TileFrames[(4*66)+1]);
	}


	//draw corners
	DrawMapTile(FIRSTMAPX- 2,FIRSTMAPY- 2,60);//top left
	DrawMapTile(FIRSTMAPX+22,FIRSTMAPY- 2,61);//top right
	DrawMapTile(FIRSTMAPX- 2,FIRSTMAPY+21,62);//bottom left
	DrawMapTile(FIRSTMAPX+22,FIRSTMAPY+21,63);//bottom right

//	if(lvl.dooroff&0x0f)//has a y value so it's stairs that will be drawn later
//		return;
	DrawMapTile(FIRSTMAPX+(lvl.dooroff>>4),FIRSTMAPY-2,54);


}

void DrawPowerPanel(){
	for(uint8_t i=0;i<3;i++){
		if(!lvl.powerpanel[i] || ((lvl.powerpanel[i] & POWERACTIVE) && (global_frame & 2)))
			DrawMapTile(SCREEN_TILES_H-3,17+(i*2),56);//draw blank panel since it's either no power or currently flashing
		else//there is a valid power here and it isn't flashing
			DrawMapTile(SCREEN_TILES_H-3,17+(i*2),56+(lvl.powerpanel[i]&0b00000011));//only 2 LSbits used for type
	}
}

inline void LoloClearVram(uint8_t index){
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = index;
}

void DrawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t fill){
	uint16_t voff	= (x+0)+((y+0)*VRAM_TILES_H);//top left
	uint16_t voff2;

	//draw corners
	vram[voff+0+(0*VRAM_TILES_H)] = 55+RAM_TILES_COUNT;
	vram[voff+w+(0*VRAM_TILES_H)] = 55+RAM_TILES_COUNT;
	vram[voff+w+(h*VRAM_TILES_H)] = 55+RAM_TILES_COUNT;
	vram[voff+0+(h*VRAM_TILES_H)] = 55+RAM_TILES_COUNT;

	voff	= (x+1)+((y+0)*VRAM_TILES_H);//top
	voff2	= (x+1)+((y+h)*VRAM_TILES_H);//bottom

	for(uint8_t i=1;i<w;i++){//draw top and bottom
		vram[voff++ ] = RAM_TILES_COUNT+56;
		vram[voff2++] = RAM_TILES_COUNT+56;
	}

	voff	= (x+0)+((y+1)*VRAM_TILES_H);//left
	voff2	= (x+w)+((y+1)*VRAM_TILES_H);//right

	for(uint8_t i=1;i<h;i++){//draw sides
		vram[voff ] = RAM_TILES_COUNT+58;
		vram[voff2] = RAM_TILES_COUNT+58;
		voff	+= VRAM_TILES_H;
		voff2	+= VRAM_TILES_H;
	}
	
	if(fill){
		//draw fill
		voff = (x+1)+((y+1)*VRAM_TILES_H);
		for(uint8_t i=1;i<h;i++){
			for(uint8_t j=1;j<w;j++)
				vram[voff++] = RAM_TILES_COUNT+55;
			voff += (VRAM_TILES_H+1)-w;
		}
	}
}


void DrawSidePanel(){
	//draw "LVL"
	vram[(VRAM_TILES_H*2)+(SCREEN_TILES_H-2)] = RAM_TILES_COUNT+184;
	vram[(VRAM_TILES_H*2)+(SCREEN_TILES_H-3)] = RAM_TILES_COUNT+183;
	LoloPrintNum((SCREEN_TILES_H-2),3,lvl.current+1);


	//draw "SHOT SYMBOL"
	vram[(VRAM_TILES_H*8)+(SCREEN_TILES_H-2)] = RAM_TILES_COUNT+186;
	vram[(VRAM_TILES_H*8)+(SCREEN_TILES_H-3)] = RAM_TILES_COUNT+185;
	LoloPrintNum(SCREEN_TILES_H-2,9,pshots);

	DrawPowerPanel();
}

void DrawMapTile(uint8_t x, uint8_t y, uint8_t t){
	uint16_t toff = (t*4);
	uint16_t voff = x+(y*VRAM_TILES_H);
	
	vram[voff++] = pgm_read_byte(&TileFrames[toff++]);//SetTile(x+0,y+0,pgm_read_byte(&TileFrames[toff++]));
	vram[voff  ] = pgm_read_byte(&TileFrames[toff++]);//SetTile(x+1,y+0,pgm_read_byte(&TileFrames[toff++]));
		voff += (VRAM_TILES_H-1);
	vram[voff++] = pgm_read_byte(&TileFrames[toff++]);//SetTile(x+0,y+1,pgm_read_byte(&TileFrames[toff++]));
	vram[voff  ] = pgm_read_byte(&TileFrames[toff  ]);//SetTile(x+1,y+1,pgm_read_byte(&TileFrames[toff]));
}

void AnimateLevel(){
	bool dowater = true;
	bool dohearts = false;
	uint8_t heartoff = 0;
	if(!lvl.water_ftime){
		if(++lvl.water_frame == NUMWATERFRAMES)
			lvl.water_frame = 0;
		lvl.water_ftime = pgm_read_byte(&water_ftimes[lvl.water_frame]);
	}else{
		lvl.water_ftime--;
		dowater = false;
		if(lvl.water_ftime&1)
			dohearts = true;
	}

	uint8_t t;
	uint8_t goff = 0;

	for(uint8_t y=FIRSTMAPY;y<FIRSTMAPY+22;y+=2)
	for(uint8_t x=FIRSTMAPX;x<FIRSTMAPX+22;x+=2){
		t = lvl.grid[goff++] & 0b00011111;//t = GetLvlGridOff(goff++);
		if(dowater){
			if(t >= WATER && t <= WATERRIGHT){
				t -= WATER;//get offset into water animation table
				t *= NUMWATERFRAMES;
				t += lvl.water_frame;
				t = pgm_read_byte(&water_frames[t]);
				
				uint16_t voff = x+(y*VRAM_TILES_H);
				vram[voff++] = t;
				vram[voff  ] = t;
				voff += VRAM_TILES_H;
				vram[voff  ] = t;
				vram[--voff] = t;
			}
		}else if(false){//dohearts){
			if(t >= WATER && t <= WATERRIGHT){
				t = pgm_read_byte(&MapFrameByType[t+heartoff]);
				DrawMapTile(FIRSTMAPX+(x<<1),FIRSTMAPY+(y<<1),t);
			}
		}
	}
	
	if(dowater){
		goff = 0;
		//fix up water shadows
		for(uint16_t voff=FIRSTMAPX+(FIRSTMAPY*VRAM_TILES_H);voff<FIRSTMAPX+((FIRSTMAPY+22)*VRAM_TILES_H);voff+=VRAM_TILES_H*2){
			if((lvl.grid[goff] & 0b00011111) >= WATER && (lvl.grid[goff] & 0b00011111) <= WATERRIGHT){
				vram[voff] += 5;
				vram[voff+VRAM_TILES_H] += 5;
			}
			goff += 11;
		}
	}
}

void UpdateBlock(){
	if(pb_offset == 255)
		return;

	bool redraw = false;
	if(pb_offset&128){
		if(((++pb_offset)&0x0F) > 7){
			if(pstate & UP){pb_y--;redraw=true;}
			else if(pstate & DOWN){pb_y++;redraw=true;}
			else if(pstate & LEFT){pb_x--;redraw=true;}
			else{pb_x++;redraw=true;}
		}
	}else
		return;
	if(redraw){
		DrawMapTile(pb_x,pb_y,22);
		pb_offset = 255;
	}
}

void DrawLevel(){
//	DrawBorder();

lvl.powerpanel[0] = lvl.powerpanel[1] = lvl.powerpanel[2] = POWERNONE;
	uint8_t off = 0;
	for(uint8_t y=FIRSTMAPY;y<FIRSTMAPY+22;y+=2)
	for(uint8_t x=FIRSTMAPX;x<FIRSTMAPX+22;x+=2){
	//	RestoreMapTile(x*2,y*2);
		uint8_t t = (lvl.grid[off++] & 0b00011111)<<1;//mask out collide bits
		DrawMapTile(x,y,t+(x==FIRSTMAPX?1:0));
continue;
/*
		if(t == SAND || t == GRASS){//determine which frames to draw based on surronding grass or sand
			uint8_t fkey = 0;//basically see how many connections there are U,D,L,R and pick the right frame
			if(y < 1 || t == lvl.grid[((y-1)*11)+x])
				fkey |= 1;
			if(y > 9 || t == lvl.grid[((y+1)*11)+x])
				fkey |= 2;
			if(x < 1 || t == lvl.grid[(y*11)+(x-1)])
				fkey |= 4;
			if(x > 9 || t == lvl.grid[(y*11)+(x+1)])
				fkey |= 8;
			t = pgm_read_byte(&SandGrassFrameKeys[fkey]);

			if(lvl.grid[off-1] == SAND)
				t += 14;//offset past grass frames to sand which are in same order
		}else{
			t = pgm_read_byte(&MapFrameByType[t]);
			if(shadow)
				t++;
		}

		

		DrawMapTile(FIRSTMAPX+(x<<1),FIRSTMAPY+(y<<1),t);
	*/	
	}
/*
	LoloPrintNum(LEVELNUMBEROFFSETX-1,LEVELNUMBEROFFSETY,lvl.current+1);
	LoloPrintNum(SHOTSOFFSETX-1,SHOTSOFFSETY,pshots);

	for(uint8_t i=0;i<3;i++)
		DrawMapTile(POWERPANELXOFF,POWERPANELYOFF+(i<<1),pgm_read_byte(&MapTileByPowerPanel[lvl.powerpanel[i]]));
*/
}


void LoloPrintNum(uint8_t x, uint8_t y, uint16_t val){
	if(val > 999UL)
		val = 999UL;
	if(val > 99)
		x++;

	uint16_t c;
	uint16_t voff = x+(y*VRAM_TILES_H);

	for(uint8_t i=0;i<3;i++){
		c = (uint16_t)(val%10);
		if(val>0 || i==0)
			vram[voff--] = RAM_TILES_COUNT+187+c;
		else
			vram[voff--] = RAM_TILES_COUNT+BLACK_TILE;
			
		val = (uint16_t)(val/10);
	}
}

void LoloPrint(uint8_t x, uint8_t y, const char *string, uint8_t offset){
	uint8_t i=0;
//	uint8_t xo = x;
	uint16_t voff = x+(VRAM_TILES_H);
	char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			if(c == ' ')
				x++;
		/*	else if(c == '\n'){
				x = xo;
				y++;
			}*/else{
				c=((c&127)-32);		
				vram[voff++] = (uint8_t)(RAM_TILES_COUNT+c+offset);
			}
		}else{
			break;
		}
	}

}

inline void SetSprite(uint8_t s, uint8_t x, uint8_t y, uint8_t t, uint8_t flags){
	sprites[s].x = x;
	sprites[s].y = y;
	sprites[s].tileIndex = t;
	sprites[s].flags = flags;
}

void DrawSpriteFrame(uint8_t x, uint8_t y, uint8_t frame, uint8_t pallet){
	uint16_t foff = frame*5;
	uint8_t moff = pgm_read_byte(&FrameTable[foff++])*4;
	SetSprite(freesprite++,x+0,y+0,pgm_read_byte(&FrameTable[foff++]),pgm_read_byte(&MirrorTable[moff++])|(pallet<<3));
	SetSprite(freesprite++,x+8,y+0,pgm_read_byte(&FrameTable[foff++]),pgm_read_byte(&MirrorTable[moff++])|(pallet<<3));
	SetSprite(freesprite++,x+0,y+8,pgm_read_byte(&FrameTable[foff++]),pgm_read_byte(&MirrorTable[moff++])|(pallet<<3));
	SetSprite(freesprite++,x+8,y+8,pgm_read_byte(&FrameTable[foff]),pgm_read_byte(&MirrorTable[moff])|(pallet<<3));
}

void HideSprites(){
	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;
	freesprite = 0;
}
