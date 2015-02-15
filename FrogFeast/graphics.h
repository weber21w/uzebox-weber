void SetSprite(uint8_t x, uint8_t y, uint8_t frame, uint8_t flags){
	sprites[spritecount].x = x;
	sprites[spritecount].y = y;
	sprites[spritecount].tileIndex = frame;
	sprites[spritecount++].flags = flags;
}

void DrawSpriteFrame(uint8_t x, uint8_t y, uint8_t frame){
	uint16_t foff = frame*5;
	uint8_t flags = pgm_read_byte(&FrameTable[foff++]);
	sprites[spritecount].tileIndex = pgm_read_byte(&FrameTable[foff++]);
	if(sprites[spritecount].tileIndex != 255 && x < 232){
		sprites[spritecount].x = x+0;
		sprites[spritecount].y = y+0;
		sprites[spritecount++].flags = flags;
	}
	sprites[spritecount].tileIndex = pgm_read_byte(&FrameTable[foff++]);
	if(sprites[spritecount].tileIndex != 255 && x < 232){
		sprites[spritecount].x = x+8;
		sprites[spritecount].y = y+0;
		sprites[spritecount++].flags = flags;
	}
	sprites[spritecount].tileIndex = pgm_read_byte(&FrameTable[foff++]);
	if(sprites[spritecount].tileIndex != 255 && x < 232){
		sprites[spritecount].x = x+0;
		sprites[spritecount].y = y+8;
		sprites[spritecount++].flags = flags;
	}
	sprites[spritecount].tileIndex = pgm_read_byte(&FrameTable[foff++]);
	if(sprites[spritecount].tileIndex != 255 && x < 232){
		sprites[spritecount].x = x+8;
		sprites[spritecount].y = y+8;
		sprites[spritecount++].flags = flags;
	}
}


void FFPrint(uint8_t x,uint8_t y,const uint8_t *string){
	uint8_t i=0;
	uint8_t c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c==255)
			break;
		else if(c==254)
			x++;
		else
			SetTile(x++,y,c+201);
	}
}

void HideSprites(){
	spritecount = 0;
	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;
}

#define FIRSTNUMTILE 189
#define BLANKNUMTILE 72
void DrawNumber(uint8_t val, uint8_t x, uint8_t y, bool zeropad){
	unsigned char c,i;
	for(i=0;i<3;i++){
		c=val%10;
		if(val>0 || i==0){
			SetTile(x--,y,c+FIRSTNUMTILE);
		}else if(i==1 && zeropad){//only needed for seconds display on timer
			SetTile(x--,y,FIRSTNUMTILE);
		}
		val=val/10;
	}
}

void DrawString(char *String, uint16_t Palette, uint16_t x, uint16_t y){
  // Draw string
}


inline void DrawScores(){
	DrawNumber(Frogs[0].Score,SCORE1X,SCOREY,0);
	DrawNumber(Frogs[1].Score,SCORE2X,SCOREY,0);
	DrawNumber((Time/60)/60, TIMEX+0, TIMEY,0);//minutes
	DrawNumber((Time/60)%60, TIMEX+3, TIMEY,1);//seconds...
//DrawNumber(ColorIndex, 10,10,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[0]),10,10,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[1]),10,11,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[2]),10,12,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[3]),10,13,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[4]),10,14,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[5]),10,15,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[6]),10,16,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[7]),10,17,0);
//DrawNumber(pgm_read_byte(&CustomFadeTable[8]),10,18,0);
}


void Render(){
	DrawScores();
	HideSprites();
	for(uint8_t i=0;i<MAXPLAYERS;i++){
		if(Frogs[i].InvincibleTimer & 2)
			continue;
		uint8_t rframe = i*32;
		bool facing = false;
		if((Frogs[i].State&FACING)){
			facing = true;
			rframe += 16;
		}

			if(Frogs[i].State & TONGUEING){//Cheats[0] = 1;
				uint8_t xoff;
				if(facing){
					if(Cheats[i]){xoff = Frogs[i].x+(15+24);}else{xoff = Frogs[i].x+15;}
				}else{
					if(Cheats[i]){xoff = Frogs[i].x-(15+24);}else{xoff = Frogs[i].x-15;}
				}
				DrawSpriteFrame(xoff,Frogs[i].y-0,pgm_read_byte(&TongueFrames[Frogs[i].TongueTime])+(facing*4));
			

			if(Cheats[i]){
				if(facing){
					for(uint8_t j=0;j<3;j++)
						SetSprite(Frogs[i].x+15+(j*8),Frogs[i].y-0,49,0);
				}else{
					for(uint8_t j=0;j<3;j++)
						SetSprite(Frogs[i].x-7-(j*8),Frogs[i].y-0,49,0);
				}
			}
			}
		if(Frogs[i].State & SWIMMING){//draw water ripples
			if((!i && (Time & 2)) || (i && !(Time & 2)))//don't draw full ripples on both frogs in the same frame(crashes sometimes/run out of time)
				DrawSpriteFrame(Frogs[i].x-6 +(Frogs[i].x&2),Frogs[i].y,72+((Frogs[i].x&4)>>2));
			else
				DrawSpriteFrame(Frogs[i].x+14-(Frogs[i].x&2),Frogs[i].y,74+((Frogs[i].x&4)>>2));
			
		}
		
		if(Frogs[i].BlinkTime < 4 && (Frogs[i].State & (SITTING|SWIMMING)))
			rframe += 3;
		if(ColorIndex > 5){//swap light and dark frogs, because the fading bits actually make the darker one lighter(annoying/disorienting)
			if(i){rframe -= 32;}else{rframe += 32;}
		}
	//	Frogs[i].State = STUNNED;
//	Frogs[i].Facing = 0;
		rframe += Frogs[i].Frame;
		DrawSpriteFrame(Frogs[i].x,Frogs[i].y,rframe);
	/*	if(Frogs[i].Facing)
			rframe += 6;
		if(Frogs[i].State & (HOPPING|LEAPING|JUMPING))
			rframe++;
		DrawSpriteFrame(Frogs[i].x,Frogs[i].y,rframe);
	*/
		
	}
	for(uint8_t i=0;i<MAXFLIES;i++){
		if(!(Flies[i].State & ACTIVE) || Flies[i].RebirthTimer)
			continue;
		sprites[spritecount].x = Flies[i].x;
		sprites[spritecount].y = Flies[i].y;
		sprites[spritecount].tileIndex = FLYSPRITE+(prng()&1);////((Flies[i].JumpTime>>2)&1);
		sprites[spritecount++].flags = 0;
		if(sprites[spritecount-1].tileIndex != FLYSPRITE)
			sprites[spritecount-1].y += 4;
	}
}
