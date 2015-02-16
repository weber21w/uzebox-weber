void PostProcess();

void Render(){
	if(guijuststarted){return;}
	char xoff = 0;
	char yoff = 0;
	u8 foff = 0;
	if(pstate & UP)		 {yoff=-poffset;foff= 1;}
	else if(pstate & DOWN){yoff= poffset;foff= 5;}
	else if(pstate & LEFT){xoff=-poffset;foff= 9;}
	else/*pstate & RIGHT*/{xoff= poffset;foff=13;}
	foff += pframe;
	if(pstate & PUSH){foff+=2;}
	spritecount = 0;
	DrawSpriteTile((px<<4)+xoff,(py<<4)+yoff,foff);
	if(bx != 255){DrawSpriteTile((bx<<4)+xoff,(by<<4)+yoff,0);}
	HideSprites(spritecount,MAX_SPRITES);
/*	PrintB(0,27,movepos);
	PrintB(4,27,nummoves);


	PrintB(7,27,movelist[0]);
	PrintB(10,27,movelist[1]);
	PrintB(13,27,movelist[2]);
	PrintB(16,27,movelist[3]);
	PrintB(19,27,movelist[4]);
*/
//	PostProcess();
	WaitVsync(1);
}

void DrawMapSection(u8 sx, u8 sw, u8 sm, u8 dx){
	int moff = sm*LEVELTOTALSIZE;
	moff += sx;
	for(u8 x=0;x<sw;x++){
		for(u8 y=0;y<14;y++){
			SetMapTile(x,y,pgm_read_byte(&Maps[moff]));
		  moff += 15;
		}
		++moff;
	}
}

void SetMapTile(u8 x, u8 y, u8 t){//this is relative to tileset
	int off = ((y<<1)*30)+(x<<1);
	t = (tileset*(NUMTILES<<2)) + (t<<2) + RAM_TILES_COUNT;
	
	vram[off+ 0] = t++;
	vram[off+ 1] = t++;
	vram[off+30] = t++;
	vram[off+31] = t;
}

void SwapTileSet(u8 newset){
	WaitVsync(1);
	u8 oldset,t;
	oldset = tileset;
	for(u8 y=0;y<14;y++){
		for(u8 x=0;x<15;x++){
			tileset = oldset;
		  t = GetMapTile(x,y);
			tileset = newset;
		  SetMapTile(x,y,t);
		}
		if(y == py){
		  Render();
		  WaitVsync(1);
		}
		else
			WaitVsync(2);
	}

	tileset = newset;
}

void DrawSpriteTile(u8 x, u8 y, u8 f){//not optimized, probably ok
	int t,m;
	int toff = tileset*SPRITESPERTILESET ;
	int  fo = f*5;
	int mo = (pgm_read_byte(&FrameTable[fo+4]))*4;
	int to = 0;
	
	for(u8 j=0;j<2;j++){
	for(u8 i=0;i<2;i++){
		t = pgm_read_byte( &FrameTable[fo+to]);
		m = pgm_read_byte(&MirrorTable[mo+to]);
		PutSprite(x,y,t+toff,m);
		x += 8;
	  to++;
	}
		x -= 16;
		y += 8;
	}
}

inline bool PutSprite(u8 x, u8 y, u8 t, bool flip){
	if(spritecount >= MAX_SPRITES){return false;}

	sprites[spritecount].x=x; sprites[spritecount].y=y; sprites[spritecount].tileIndex=t;

	//if(flip){sprites[spritecount].flags = SPR_FLIP_X;}
	//else	 {sprites[spritecount].flags = 0;}
	sprites[spritecount].flags = SPR_FLIP_X*flip;
	
	spritecount++;
	return true;
}
/*
inline void PostProcess(){
	static u8 count = 0;
	if(count < 5){count++;return;}else{count=1;}
	static char shift[8] = {0,1,1,1,2,2,3,2,};
	static u8 pos = 0;
	static bool  dir[8]	= {true,true,true,true,true,true,true,false};
 //  if(++count < 6){return;}
  // if(tileset != 1){return;}
	//we use ram tile 30 for the sand
	//do the "mirage" effect
//u8 t = pos;
for(u8 x=0;x<8;x++){
	pos++;
		if(dir[pos]){if(++shift[pos] > 4){dir[pos] = false;}}
	  else		{if(--shift[pos] < -3){dir[pos] = true;}}//rolled over
		if(++pos > 7){pos = 0;}

	  //now shift one line of ram tile pixels over
//	  int roff = (20*64)+(pos<<3);
	  int toff;// = (NUMTILESPERSET)*64;//get the first tile of the second set
	  for(u8 k=0;k<4;k++){
	  int roff = ((20+k)*64)+(pos*8);
		toff = (NUMTILESPERSET+k)*64;//get the first tile of the second set
	  for(int i=shift[pos];i<8;i++){
			ram_tiles[roff+i] = pgm_read_byte(&GameTiles[toff++]);		

	  }
	 // roff+=64;
	  }
	  }
	  printb(27,0,shift[0]);
//pos = t;
}
*/
