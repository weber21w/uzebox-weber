//TODO OPTIMIZE - HACKISH///
//////////////////////////////
//MUCH IS NOW OBSOLETE - USING MASKS NOW//////

void replaceramcolor(u8 to, u8 ro, u8 color){
	int toff = (to<<6);
	int roff = (ro<<6);
	u8 t;
	for(u8 i=0;i<64;i++){
		t = ram_tiles[roff];
	  if(t == color){ram_tiles[roff] = pgm_read_byte(&GameTiles[toff]);}//heres the color we wanted, replace it with one from the tileset
	  
	  roff++;
	  toff++;
	}
}

void ComposeRamTile(int toff, int soff, int roff){//make 4 ram tiles at offset composed of sprites drawn over tiles
	int off=0;
	u8 t;
	while(off < (4*64)){
		if((t = pgm_read_byte(&GameSprites[soff])) == 0xFE)
		  ram_tiles[roff] = pgm_read_byte(&GameTiles[toff]);
		else
		  ram_tiles[roff] = t;
	  ++roff; ++toff; ++soff; off++;
	}
}


void ttoram(int toff, int roff, int len){
	while(len--)
		ram_tiles[roff++] = pgm_read_byte(&GameTiles[toff++]);
}

void ttoramcolorkey(int toff, int roff, int len, u8 sc){
	u8 t;
	while(len--){
		t = pgm_read_byte(&GameTiles[toff++]);
		if(t != sc){ram_tiles[roff] = t;}
	
	  roff++; 
	}
}

void storam(int soff, int roff, int len){
	while(len--)
		ram_tiles[roff++] = pgm_read_byte(&GameSprites[soff++]);
}

void rotateram(u8 r){
	int roff = r<<6;
	int doff = (30*64);
	
	for(u8 i=0;i<64;i++)//copy over the data to tile 30
		ram_tiles[doff+i] = ram_tiles[roff+i]; 
	
	for(u8 x=0;x<8;x--)//rotate it 90 degrees
	for(u8 y=7;y<255;y--)
		ram_tiles[roff+((8-x)<<3)+y] = ram_tiles[doff+(y<<3)+x];
		
}


void RamifyTileSet(){//fill first 24 ram tiles with tile data
	int off = tileset*(NUMTILESPERSET)<<6;

	for(int i=0;i<(24*64);i++)
		 ram_tiles[i] = pgm_read_byte(&GameTiles[i+off]);

	WaitVsync(1);//takes a long time, let the kernel go
}

void RamifyScreen(){//convert all tiles on screen to ram tile indexes corresponding to the tile number in the set
	u8 t;
	int off = (NUMTILESPERSET*tileset)+RAM_TILES_COUNT;
	
	for(int i=(0*30);i<(30*28);i++){
		t = vram[i];
	  if(t < RAM_TILES_COUNT || t > ENDGAMETILES+RAM_TILES_COUNT){continue;}
	  vram[i] -= off;
	}

	WaitVsync(1);//takes a long time, let the kernel go
}

/*

void StripeScreenOut(int speed, bool hrz){
	u8 count = 0;
	int off = 0;

	for(u8 i=0;i<8;i++){//we will end up drawing 8  black stripes to each ram tile.
	  if(hrz){off = i<<3;}//shift the first xpos of the first tile on our current y line
	 else	{off = i;}//shift to the first ypos of the first tile
	 for(u8 j=0;j<24;j++){//We will modify 24 ram tiles. Inside here we could do a number of different per tile effects.
		for(u8 k=0;k<8;k++){//We will draw 8 horizontal black pixels to each ram tile. Heres the real heart of it.  
			  ram_tiles[off] = 0;//set the pixel to black
			if(hrz){off++;}
			else{off+=8;}
		}
		if(hrz){off += (64-8);}//shift to the first pixel, next tile
		  else	{}//off += 8;}//just shift right 1 pixel
	 }
	  if(speed > 0){
		  if(++count > speed){
			 count = 0;
			  WaitVsync(1);	
		 }
	  }
	  else
		  WaitVsync(abs(speed));
	}
		 
	  WaitVsync(1);
  
}

void StippleScreenOut(int speed){
	u8 count = 0;
	int off=0;// = tileset*(NUMTILESPERSET);//24 tiles per "tile set"
	
	u8 r,t;
	for(u8 k=0;k<64;k++){//we'll draw 64 pixels, eventually..
		do{
		  r = fakerand();//rand()%64;
		  t = ram_tiles[r];
		}
	  while(t == 0);//go until we find a pixel we haven't drawn

	  for(u8 i=0;i<24;i++){//do 24 tiles
		  off = i<<6;
			ram_tiles[r+off] = 0;

		}
	  if(speed > 0){
		  if(++count > speed){
			 count = 0;
			  WaitVsync(1);	
		 }
	  }
	  else
		  WaitVsync(abs(speed));
	}
	 WaitVsync(1);
}

void BlockScreenOut(int speed){
	int off = 0;
	int roff;

	for(u8 j=0;j<4;j++){
	for(int r=0;r<24;r++){
		roff = r<<6;
		off = 64-(8-j);
		for(u8 i=0;i<8;i++){ram_tiles[off+roff] = 0; off -= 8;}//up
//WaitVsync(1);
		off = 0+(j<<3);
	  for(u8 i=0;i<8;i++){ram_tiles[off+roff] = 0; off++;}//right
//WaitVsync(1);
	  off = 8-j;
	  for(u8 i=0;i<8;i++){ram_tiles[off+roff] = 0; off += 8;}//down
//WaitVsync(1);
	  off = 64-(j<<3);
	  for(u8 i=0;i<8;i++){ram_tiles[off+roff] = 0; off--;} //left	  

	  }
WaitVsync(8);
	}	  

	 WaitVsync(1);

}
*/

void BitMaskScreenOut(u8 start, u8 nframes, u8 delay, bool reverse, u8 finaldelay, bool vflip){
	RamifyTileSet();
	RamifyScreen();

	int t;
	int moff = (start*8);//8 bytes per frame

	while(nframes--){
		

		for(u8 y=0;y<8;y++){//extract 8 bytes total, 1 bit per pixel
			t = pgm_read_byte(&ScreenFadeTable[moff++]);//get our byte from the table
		  for(u8 x=0;x<8;x++){//read 8 bits from the byte and do a row
				if((t>>x) & 1){//is mask bit for this x position set
				for(int i=0;i<24;i++)//draw it to 24 ram tiles
					ram_tiles[(i<<6)+(y<<3)+x] = 0;
			}
		 }	
		}

	  if(reverse)//draw frames from start moving backwards(for lines left to right etc, save space)
			moff -= 16;		 

		WaitVsync(delay);
	}
	
	if(finaldelay)
		WaitVsync(finaldelay);
}


u8 printramtilesfancy(u8 x,u8 y,const char *string,u8 ramoff,u8 sc,u8 dc,u8 sc2,u8 dc2){
	int i=0;
	char c;
	u8 t;
	 int off=ramoff<<6;//get our offset to the ram tile
	while(true){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			c=((c&127)-32);

			for(u8 j=0;j<64;j++){//copy over the data		
				t = pgm_read_byte(&GameTiles[((c+FONTSTART-19)*64)+j]);
				if(t == sc){ram_tiles[off++] = dc;}
				else if(t == sc2){ram_tiles[off++] = dc2;}
				else								{ram_tiles[off++] = t;}
			}
			vram[(y*30)+x] = ramoff++;//SetTile(x++,y,c+FONTSTART-21);
			x++;
			if(x>29){return ramoff;}//clip it to the screen
		}else{
			break;
		}
	}
	return ramoff;//return what ram tile offset we are at
}

u8 printramtilesoverlaid(u8 x,u8 y,const char *string,u8 ramoff,u8 sc,u8 dc,u8 sc2,u8 dc2,u8 ck){
	//this is SO slow, it is designed to be called once!
	int i=0;
	char c;
	u8 t=0;
	 int off=ramoff<<6;//get our offset to the ram tile
	while(true){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			c=((c&127)-32);
				
			if(t >= RAM_TILES_COUNT){//have we done this before?
				t = vram[(y*30)+x];//get the tile under this character
				t -= RAM_TILES_COUNT;//get the actual tile number
				for(u8 j=0;j<64;j++){//copy the tile under this character
					ram_tiles[off+j] = pgm_read_byte(&GameTiles[(t*64)+j]);
				}
			}

			for(u8 j=0;j<64;j++){//copy over the data		
				t = pgm_read_byte(&GameTiles[((c+FONTSTART-19)*64)+j]);
				if(t == ck){off++;}//clear color
				else if(t == sc){ram_tiles[off++] = dc;}
				else if(t == sc2){ram_tiles[off++] = dc2;}
				else								{ram_tiles[off++] = t;}
			}
			vram[(y*30)+x] = ramoff++;
			x++;
			if(x>29){return ramoff;}//clip it to the screen
		}else{
			break;
		}
	}
	return ramoff;//return what ram tile offset we are at
}

u8 printrainbow(u8 x,u8 y,const char *string,u8 ramoff,u8 sc,u8 dc,u8 coff){
	int i=0;
	char c;
	u8 t;
	u8 sc2,dc2;
	 int off=ramoff<<6;//get our offset to the ram tile
	while(true){
		c=pgm_read_byte(&(string[i++]));		
		if(c == '3'){x++;continue;}
		if(c!=0){
			sc2 = 0xFF;//trade white
			dc2 = pgm_read_byte(&colormap[coff++]);
			if(coff > 3){coff = 0;}
			c=((c&127)-32);

			for(u8 j=0;j<64;j++){//copy over the data		
				t = pgm_read_byte(&GameTiles[((c+FONTSTART-19)*64)+j]);
				if(t == sc){ram_tiles[off++] = dc;}
				else if(t == sc2){ram_tiles[off++] = dc2;}
				else								{ram_tiles[off++] = t;}
			}
			vram[(y*30)+x] = ramoff++;
			x++;
			if(x>29){return ramoff;}//clip it to the screen
		}else{
			break;
		}
	}
	return ramoff;//return what ram tile offset we are at if we had management to care
}


u8 printramtiles(u8 x, u8 y, const char *string,u8 ramoff,u8 sc,u8 dc){
	return printramtilesfancy(x,y,string,ramoff,sc,dc,0,0);
}
