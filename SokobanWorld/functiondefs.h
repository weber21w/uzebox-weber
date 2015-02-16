////////////TODO CLEAN THIS UP!!////////////
extern int abs();
extern int rand();
extern int srand();
extern u8 ram_tiles[];

u8 fakerand();

void VictoryMenu();
void StripeScreenOut(int speed, bool hrz);
void StripeScreenIn(int speed,  bool hrz);
void BlockScreenOut(int speed);
bool PlayDemo(u8 demonum);
void CalculateOptimum(u8 demonum);
void DoScreenOutEffect();
void DoScreenInEffect();
void ttoram(int toff, int roff, int len);
void storam(int soff, int roff, int len);
void DrawMenu(u8 x, u8 y, u8 w, u8 h);
void DrawOpeningMenu(u8 x, u8 y, u8 w, u8 h, int speed, bool smooth);
void RetryLevel();
void SaveUserMap(u8 slot);
void rotateram(u8 r);

bool HasMetLevelPar(u8 map);
void SetMetLevelPar(u8 map);
void SetLevelCompleted(u8 map);
bool IsLevelCompleted(u8 map);

void SwapTileSet(u8 newset);

inline bool SokoMove();
u8 GetMapTile(u8 x, u8 y);

void AmericaEnterance();
void EgyptEnterance();
void ChinaEnterance();
void SouthPoleEnterance();
void JapanEnterance();
void GameEnding();
bool AllLevelsCompleted();

void SetVram(u8 x, u8 y, u8 t){//direct vram, bypass RAM_TILES_COUNT
	vram[(y*30)+x] = t;
}

void printb(u8 x,u8 y, u8 val){
	u8 c = val%10;
	SetTile(x+1,y,FONTSTART+4+c);
	
	c=(val/10);
	if(!c){SetTile(x,y,BLANKT+1);}else{SetTile(x,y,FONTSTART+4+c);}
}



void printi(u8 x,u8 y, int val){
	 if(val > 999)
		val = 999;

	unsigned char c,i;

	for(i=0;i<4;i++){
		c=val%10;
		if(val>0 || i==0){
			SetTile(x--,y,c+FONTSTART+4);
		}else{
		//	if(zeropad){
		//		SetFont(x--,y,CHAR_ZERO+RAM_TILES_COUNT);
		//	}else{
				SetTile(x--,y,FONTSTART);
		//	}
		}
		val=val/10;
	}

}

void printdigitoverlaid(u8 x,u8 y, u8 val, u8 ramoff, u8 ck, u8 bg){
	u8 c = val;//%10;
	 int toff = (FONTSTART+4+c)*64;	
	 int roff = ramoff*64;
	 u8 t;

	 for(u8 i=0;i<64;i++)//copy over the background tile
		ram_tiles[roff+i] = pgm_read_byte(&GameTiles[(bg*64)+i]);

	vram[(y*30)+x] = ramoff;
	 for(u8 i=0;i<64;i++){
		t = pgm_read_byte(&GameTiles[toff]);
		if(t != ck)
			ram_tiles[roff] = t;

		toff++;
		roff++;
	}
}

void print(u8 x,u8 y,const char *string){
	int i=0;
	char c;

	while(true){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			c=((c&127)-32);		

			SetTile(x++,y,c+FONTSTART-19);
		}else{
			break;
		}
	}	
}



void FillScreen(u8 t);
void ResetSpriteCount();
void DoHideSprites();
bool PutSprite(u8 x, u8 y, u8 t, bool flip);
void Render();
void DrawMapSection(u8 sx, u8 sw, u8 sm, u8 dx);
void DrawSpriteTile(u8 x, u8 y, u8 f);


void Undo();
void Redo();

void HideSprites(u8 start, u8 end){
	 for(u8 i=start;i<end;i++)
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;
}


void ClearSprites(){//hides all sprites, resets sprite count
	HideSprites(0,32);
	//ResetSpriteCount();
}



void SetGuiState(u8 state);
void LoadLevel(bool flushmoves, bool fadein, bool shiftdown, bool skiptoprow, bool fromeep, bool skipsong);

void SetMapTile(u8 x, u8 y, u8 t);
bool IsBlock(u8 t){if(t==BLOCK||t==TBLOCK){return true;}return false;}
bool IsTBlock(u8 t){if(t==TBLOCK){return true;}return false;}
bool IsWall(u8 t){if(t==WALL){return true;}return false;}
bool IsSolid(u8 t){if(IsBlock(t)||IsTBlock(t)||IsWall(t)){return true;}return false;}
