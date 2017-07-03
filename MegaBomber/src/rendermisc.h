void printb(u8 x,u8 y, u8 val){
	u8 c = val%10;
	SetTile(x+2,y,ZERO_OFFSET+c-88);
	
	c=(val/10)%10;
	if(!c){SetTile(x+1,y,BLANKT);}else{SetTile(x+1,y,ZERO_OFFSET+c-88);}
}

void DrawTimer(){
   if(roundtime%60)//save some time most ticks
      return;

   u8 min,ten,sec;
   int rt = roundtime/60;
   min = (rt/60);
   ten = (rt%60)/10;
   sec = (rt%10);   

   SetTile(13,0,ZERO_OFFSET+min);
   SetTile(14,0,ZERO_OFFSET+10);
   SetTile(15,0,ZERO_OFFSET+ten);
   SetTile(16,0,ZERO_OFFSET+sec);
}

void SetMapTile(u8 x, u8 y, u8 t){
   int off = (y*30)+x;//((y<<1)*30)+(x<<1);
   t+=RAM_TILES_COUNT;//t = (t<<2)+RAM_TILES_COUNT;//t = (tileset*(NUMTILES*4)) + (t*4) + RAM_TILES_COUNT;
   
   vram[off+ 0] = t++;
   vram[off+ 1] = t++;
   vram[off+30] = t++;
   vram[off+31] = t;
}




inline u8 GetMapTile(u8 x, u8 y){
      return (vram[(y*30)+x]-RAM_TILES_COUNT);//return (vram[122+(y*30)+x]-RAM_TILES_COUNT);
}


void RamifyFont(int startoff, u8 numchars){
   u8 t;
   int roff = 0;
   for(int i=startoff;i<(8*numchars)+startoff;i++){
	  t = pgm_read_byte(&FontTiles[i]);
      for(u8 j=0;j<8;j++){
         if(t & (128>>j))
		    ram_tiles[roff++] = 255;
         else
		    ram_tiles[roff++] = 0;
	  }
   }
}
void CompPrint(u8 x, u8 y,const char *string){
	int i=0;
	char c;

	while(true){
	    if(x > 29)
		   return;
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
		if(c == ' '){
		   SetTile(x,y,BLANKT);
		   x++;
		   continue;
        }
        
		if(c > '9')
		   c=((c&127)-('A'-6));	
		else
		   c -= '0';	

		    vram[(y*30)+(x++)] = c; 
		}else{
			break;
		}
	}	
}

void CompPrintNum(u8 x, u8 y, u8 val){
	u8 c = val%10;
	vram[(x+2)+(y*30)] = c;
	
	c=(val/10)%10;
	if(!c){SetTile(x+1,y,BLANKT);}else{vram[(x+1)+(y*30)]=c;}
}
void SwapColors(int start, int length, u8 key, u8 replace){
   while(length--){
      if(ram_tiles[start] == key)
	     ram_tiles[start] = replace;
      start++;
   }
}

void DrawSpriteTile(u8 x, u8 y, u8 f, u8 p, u8 offset, u8 overflow){
   int t,m;
   int fo = f*7;
   int mo = (pgm_read_byte(&FrameTable[fo+6]))*6;
   int to = 0;

   for(u8 j=0;j<3;j++){
   
   for(u8 i=0;i<2;i++){
      t = pgm_read_byte( &FrameTable[fo+to])+offset;
      m = pgm_read_byte(&MirrorTable[mo+to]);
      if(t == 255){
	     x += 8;
		 to++;
         continue;
      }
	  
	  PutSprite(x,y,t,m,p);
      x += 8;
	  to++;
   }
      x -= 16;
      y += 8;
   }
}






inline void IncSpriteCount(){spritecount++;}//if(blink){spritecount++;}else{spritecount--;}}
inline void ResetSpriteCount(){spritecount=0;}//if(blink){spritecount = 0;}else{spritecount = MAX_SPRITES-1;}}
