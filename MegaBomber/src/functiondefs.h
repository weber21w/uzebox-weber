extern int abs();
//extern void srand();
extern int rand();

void KillPlayer(u8 p);
u8 GetPlayerRange(u8 p);
bool IsSolidForFire(u8 t);
u8 bomb_range(u8 i);
u8 player_max_bombs(u8 p);

//TODO VERY DISORGANIZED

void SetMapTile(u8 x, u8 y, u8 t);

void FillScreen(u8 t);
void ResetSpriteCount();
void DoHideSprites();
bool PutSprite(u8 x, u8 y, u8 t, bool flip, u8 pallet);
void DrawOpeningMenu(u8 x, u8 y, u8 w, u8 h, int speed);

u8 GetRoundTimeBits();
void SetRoundTimeBits(u8 t);
u8 GetNumRoundBits();
void SetNumRoundBits(u8 t);

void HideSprites(u8 start, u8 end){
    for(u8 i=start;i<end;i++)
	   sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;
}

 
u8 prand(){
  (prng = ((u8)((prng>>1) | ((prng^(prng>>2)^(prng>>3)^(prng>>4))<<7))));
   return prng;
}

void ClearSprites(){//hides all sprites, resets sprite count
   HideSprites(0,MAX_SPRITES);
   ResetSpriteCount();
}


inline u8 GetMapTile(u8 x, u8 y);
void SetMapTile(u8 x, u8 y, u8 t);
void Logic();
void Engine();
void Render();
void printb(u8 x,u8 y, u8 val);
void LoadLevel();
void UpdateBorder();
void SwapColors(int start, int length, u8 key, u8 replace);

void RamifyFont(int startoff, u8 numchars);
void CompPrint(u8 x, u8 y,const char *string);
void CompPrintNum(u8 x, u8 y, u8 val);
u8 player_bombs_out(u8 p);

inline bool IsSolid(u8 t);
inline bool IsSolidEx(u8 x, u8 y);
bool IsFire(u8 t);
bool IsBomb(u8 t);
bool IsBlock(u8 t);
inline void ExplodeBomb(u8 b);


void DrawStaticFrame(u8 x, u8 y, u8 t);

void InGameMenu();


//void debugsound(){TriggerFx(1,0xff,true);};
void debugprint(u8 t){printb(0,VRAM_TILES_V-1,t);}


void DrawSpriteTile(u8 x, u8 y, u8 f, u8 p, u8 offset, u8 overflow);



void print(u8 x,u8 y,const char *string){
	int i=0;
	char c;

	while(true){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			if(c == ' '){SetTile(x++,y,BLANKT);continue;}
			c=(c&127);
			if(c >= '0' && c <= '9'){c -= 58;}
            else if(c == '!'){SetTile(x++,y,'0'-1);continue;}
			else c -= 65;
					

			SetTile(x++,y,c+ALPHASTART);
		}else{
			break;
		}
	}	
}

void printram(u8 x,u8 y,const char *string){
	int i=0;
	char c;

	while(true){
		c=string[i++];		
		if(c!=0){
			if(c == ' '){SetTile(x++,y,BLANKT);continue;}
			c=(c&127);
			if(c >= '0' && c <= '9'){c -= 58;}
            else c -= 65;
					

			SetTile(x++,y,c+ALPHASTART);
		}else{
			break;
		}
	}	
}

u8 colormap[] PROGMEM = {0x0F,0x30,0xF0,0xC0,0x37};

u8 printramtilesfancy(u8 x,u8 y,const char *string,u8 ramoff,u8 sc,u8 dc,u8 sc2,u8 dc2){
	int i=0;
	char c;
	u8 t;
    int off=ramoff*64;//get our offset to the ram tile
	while(true){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
		    if(c == ' '){x++;continue;}
			c=((c&127)-32);
            c = (c+ALPHASTART-33)*64;
			for(u8 j=0;j<64;j++){//copy over the data		
			   t = pgm_read_byte(&GuiTiles[c+j]);
			   if(t == sc){ram_tiles[off++] = dc;}
			   else if(t == sc2){ram_tiles[off++] = dc2;}
			   else                        {ram_tiles[off++] = t;}
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

u8 printramtiles(u8 x, u8 y, const char *string,u8 ramoff,u8 sc,u8 dc){
   return printramtilesfancy(x,y,string,ramoff,sc,dc,0,0);
}

u8 printrainbow(u8 x,u8 y,const char *string,u8 ramoff,u8 sc,u8 dc,u8 coff){
	int i=0;
	char c;
	u8 t;
	u8 sc2,dc2;
    int off=ramoff<<6;//get our offset to the ram tile
	while(true){
		c=pgm_read_byte(&(string[i++]));		
		if(c == ' '){x++;continue;}
		if(c!=0){
		   sc2 = 0xFF;//trade white
		   dc2 = pgm_read_byte(&colormap[coff++]);
		   if(coff > 3){coff = 0;}

            if(c >= '0' && c <= '9')
			   c += 7;

			c=((c&127)-32);

			//c = (c+ALPHASTART-33)*64;


			for(u8 j=0;j<64;j++){//copy over the data		
			   t = pgm_read_byte(&GuiTiles[((c+ALPHASTART-33)*64)+j]);
			   if(t == sc){ram_tiles[off++] = dc;}
			   else if(t == sc2){ram_tiles[off++] = dc2;}
			   else                        {ram_tiles[off++] = t;}
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


