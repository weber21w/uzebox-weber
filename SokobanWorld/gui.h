void GuiScreenFlush();
//This whole gui is a mess. Enjoy...

void SetGuiState(u8 state){
	guijuststarted = true;
	guistate = state;
}

void AttractMode(){
	if(demonum==NUMDEMOS){demonum = 0;}
	PlayDemo(demonum++);
}

void DrawMenu(u8 x, u8 y, u8 w, u8 h){
	int off = (y*30)+x;

	u8 t;
	t = vram[off+0]-RAM_TILES_COUNT;
	if(t < MENUSTART || t == BLANKT){menurestore[0] = vram[off+ 0]-RAM_TILES_COUNT;}//save the tiles under the corners if they aren't already menu tiles
	t = vram[off+w]-RAM_TILES_COUNT;
	if(t < MENUSTART || t == BLANKT){menurestore[1] = vram[off+ w]-RAM_TILES_COUNT;}
	
	off += 30*h;
	
	t = vram[off+0]-RAM_TILES_COUNT;
	if(t < MENUSTART || t == BLANKT){menurestore[2] = vram[off+ 0]-RAM_TILES_COUNT;}
	t = vram[off+w]-RAM_TILES_COUNT;
	if(t < MENUSTART || t == BLANKT){menurestore[3] = vram[off+ w]-RAM_TILES_COUNT;}

	SetTile(x+0,y+0,MENUSTART+5);//draw the corners
	SetTile(x+w,y+0,MENUSTART+6);
	SetTile(x+0,y+h,MENUSTART+7);
	SetTile(x+w,y+h,MENUSTART+8);

	for(u8 i=x+1;i<x+w;i++){SetTile(i,y,MENUSTART+0);SetTile(i,y+h,MENUSTART+1);}//draw top and bottom
	for(u8 i=y+1;i<y+h;i++){SetTile(x,i,MENUSTART+2);SetTile(x+w,i,MENUSTART+3);}

	for(u8 i=y+1;i<y+h;i++)
	for(u8 j=x+1;j<x+w;j++)
		SetTile(j,i,FONTSTART);
	
}
/*
void DrawRamMenu(u8 x, u8 y, u8 w, u8 h, u8 ro){//Note that this should be kept very small, starts at ram tile ro
	ttoram((MENUSTART+5)*64,(ro*64),64); SetVram(x+0,y+0,ro++);
	ttoram((MENUSTART+6)*64,(ro*64),64); SetVram(x+w,y+0,ro++);
	ttoram((MENUSTART+7)*64,(ro*64),64); SetVram(x+0,y+h,ro++);
	ttoram((MENUSTART+8)*64,(ro*64),64); SetVram(x+w,y+h,ro++);

	ttoram((MENUSTART+0)*64,((ro+0)*64),64);
	ttoram((MENUSTART+1)*64,((ro+1)*64),64);
	for(u8 i=x+1;i<x+w;i++){SetVram(i,y,ro++);SetVram(i,y+h,ro++);}//draw top and bottom
	
	ttoram((MENUSTART+2)*64,((ro+0)*64),64);
	ttoram((MENUSTART+3)*64,((ro+1)*64),64);
	for(u8 i=y+1;i<y+h;i++){SetVram(x,i,ro++);SetVram(x+w,i,ro++);}

	ttoram((FONTSTART)*64,(ro*64),64);
	for(u8 i=y+1;i<y+h;i++)
	for(u8 j=x+1;j<x+w;j++)
		SetVram(j,i,ro);

	
}
*/


void SmoothMenuCorners(u8 x,u8 y,u8 w,u8 h,u8 roff){//get rid of the black pixels on menu corners
  
	//TODO - SPRITES ARE WAY MORE EFFICIENT
	ttoram((MENUSTART+5)*64,(26*64),(4*64));//copy over the 4 ram tiles starting from ram tile 26
//	u8 t;
	int off = (y*30)+x;//offset to top left piece of menu

	vram[off +0] = roff;//set the ram tile we pre rendered
	replaceramcolor(menurestore[0],roff++,0);//replace the color black with the colors from the tile that was underneath

	vram[off+ w] = roff;
	replaceramcolor(menurestore[1],roff++,0);

	off += (h*30);

	vram[off+ 0] = roff;
	replaceramcolor(menurestore[2],roff++,0);

	vram[off+ w] = roff;
	replaceramcolor(menurestore[3],roff++,0);

}


void DrawOpeningMenu(u8 x, u8 y, u8 w, u8 h, int speed, bool smooth){
	u8 mt;
	u8 count;
	mt = 1;
	count = speed;
	while(mt<w+1){//open horizontally
		DrawMenu(x,y,mt,1);
	  if(smooth){SmoothMenuCorners(x,y,mt,1,26);}
	  if(speed < 0){
			WaitVsync(abs(speed));
	  }
	  else{
		  if(!count){WaitVsync(1);count=speed;}else{count--;}
	  }
	  mt++;
	}
	mt = 1;
	count = speed;
	while(mt<h+1){
		DrawMenu(x,y,w,mt);
	  if(smooth){SmoothMenuCorners(x,y,w,mt,26);}
	  if(speed < 0){
			WaitVsync(abs(speed));
	  }
	  else{
		  if(!count){WaitVsync(1);count=speed;}else{count--;}
	  }
	  mt++;
	}
}

void FillScreen(u8 t){
	for(u8 i=0;i<30;i++)
	for(u8 j=0;j<28;j++)
		SetTile(i,j,t);
}
/*
void FillSection(u8 x, u8 y, u8 w, u8 h, u8 t){
	u8 i,j;
	i=x;j=y;
	for(j=j;j!=y+h;j++)
	while(i!=x+w){
		SetTile(i,j,t);
	  i++;
	}
}
*/
void GuiScreenFlush(){
	guijuststarted = false;
	WaitVsync(1);
//	SetSpritesTileTable(GameSprites);
//	SetTileTable(GuiTiles);
	FillScreen(2);
	ClearSprites();
	FadeOut(0,true);
}

void CloseGui(){
	guistate = 0;
	guijuststarted = 0;
	WaitVsync(1);
	SetTileTable(GameTiles);
	FadeIn(5,false);
}

inline void DrawRamTileLine(int xoff, int roff, int doff){
  roff = 0;
  doff = 1;
	while(roff < (10*64)){//draw the top line of ten ascending ram tiles
		if((roff>0) && ((roff&7) == 0)){roff+=(64-8);}//hit the next row, so get to the first x pixel of the next tile
		if((doff>0) && ((doff&7) == 0)){doff+=(64-8);}
		  ram_tiles[doff] = ram_tiles[roff];//pgm_read_byte(&Title[doff++]);
		 roff++;
	}


}



void VramFillWindow(u8 x, u8 y, u8 w, u8 h, u8 rt){
	int off = 0;
	int roff = rt*64;
	for(u8 j=y;j<y+h;j++)
	for(u8 i=x;i<x+w;i++)
		vram[(j*30)+i+roff] = off++;
}

void LinearWindow(u8 x, u8 y, u8 w, u8 h, int t){
	for(u8 j=y;j<y+h;j++)
	for(u8 i=x;i<x+w;i++){
		vram[(j*30)+i] = RAM_TILES_COUNT+t;
		t++;
	}
}

void MainMenu(){
	static bool firstrun = false;
	int idleticks = 0;
	cursorpos = 0;
	tileset = 0;
	u8 t;
	
	graphicspreference = false;
	musicpreference = false;

	HideSprites(0,MAX_SPRITES);
	FillScreen(BLANKT);
	WaitVsync(1);
	FadeIn(2,false);

	if(!firstrun){
		StartSong(AmericaSong);
	  musicplaying = true;
	  tracknum = 1;
		firstrun = true;
	}

	WaitVsync(1);

	HideSprites(0,MAX_SPRITES);
	
	bool flop = true;

	u8 flopcount = 4;
	for(u8 yoff = 22;yoff>2;yoff--){

		for(u8 y=0;y<4;y++){
			for(u8 x=0;x<14;x++){
				t = pgm_read_byte(&TitleMap[((y)*14)+x]);
			  vram[((y+yoff)*30)+x+8] = t+169+RAM_TILES_COUNT;//TODO define 169
			}
		}

		spritecount = 0;
	  tileset = 0;
	  u8 yo = yoff<<3;
	  for(u8 i=0;i<5;i++){//pretty time tight, but it makes it(I think/hope). Lack of cleverness here...
		 switch(i){
			 case 0:
				DrawSpriteTile(64,yo+16,3+flop);
			 break;
				
			case 1:
				DrawSpriteTile(96,yo+32,4-flop);
			break;

				case 2:
				DrawSpriteTile(112,yo+32,3+flop);//Out of time now, because of channel 4 turned on
			break;

				case 3:
				DrawSpriteTile(128,yo+32,4-flop);
			break;

			case 4:
				DrawSpriteTile(160,yo+16,3+flop);
			break;
		  };

		 //DrawSpriteTile((i<<4)+80-((i==0)<<4)+((i==4)<<4),(yoff<<3)+32-((i==0 || i==4)<<4),3+flop);
		  ++tileset;
	  }

		WaitVsync(1);
		if(yoff > 2){
		 int off = ((yoff+4)*30)+10;
		 for(u8 x=10;x<20;x++)
			 vram[off++] = BLANKT+RAM_TILES_COUNT;
	  }

	  flopcount--;
		if(!flopcount){
		  flop = !flop;
		 flopcount = 4;
	  }
	  WaitVsync(1);
	}

	WaitVsync(20);

	DrawOpeningMenu(20,20,9,3,-4,false);
	print(21,21,PSTR("NEW3GAME"));
	print(21,22,PSTR("CONTINUE"));
	WaitVsync(40);

	  while(true){
		FillPad();

		if(UpDown() || DownDown()){cursorpos = !cursorpos;idleticks=0;}
	  else if(StartDown()){
		 // TriggerFx(20,255,true);
		  guijuststarted = true;
		 guistate = GLEVELSELECT;
		 level = 0;
			if(cursorpos == 1){//continue
			 while(IsLevelCompleted(level))
				level++;
				
			if(level >= numlevels)
				level = numlevels-1;
		 }
		 //else{guistate=GMAINMENU;}//guistate = GEDITOR;}
		 //else{guistate = GHELP;}
		 FadeOut(1,true);
		 HideSprites(0,MAX_SPRITES); 
		 return;
	  }
		
	  for(u8 i=0;i<2;i++){SetTile(20,21+i,MENUSTART+2);} SetTile(20,21+cursorpos,MENUSTART+4);//overdraw side of window then draw cursor


	  if(++idleticks > TITLEWAITTIME){
		 FadeOut(1,true);
		 HideSprites(0,MAX_SPRITES);
		 guijuststarted = false;
		  AttractMode();
		 guijuststarted = true;
		 return;
	  }
		
		WaitVsync(1);
		
	  spritecount = 0;
	  if(++flopcount>7){flop = !flop; flopcount = 0;}
		for(u8 i=0;i<5;i++){
		 // if(i == 2){continue;}//not enough time for all 5 now(channel 4)
		  tileset = i;
		 DrawSpriteTile((i<<4)+80-(16*(i==0))+((i==4)<<4),56-((i==0 || i==4)<<4),5+flop);

	  }

		WaitVsync(1);
	}
	
}


void InGameMenu(){
	cursorpos = 0;
	u8 oldgfx,oldtrack;
	oldgfx = tileset;
	oldtrack = tracknum;
	u8 gfxnum = tileset;
	//u8 restore[16];
	int off = 14*64;//0;
	for(u8 y=5;y<10;y++)
	for(u8 x=5;x<10;x++)
		ram_tiles[off++] = GetMapTile(x,y);//restore[off++] = GetMapTile(x,y);
	
	HideSprites(0,MAX_SPRITES);
	DrawOpeningMenu  (10,10,8,8,0,true);
	WaitVsync(1);
	print(11,11,PSTR("BACK"));//strGameMenu1);
	print(11,12,PSTR("RETRY"));
	print(11,13,PSTR("GIVEUP"));
	print(11,14,PSTR("SOLVE"));
	print(11,15,PSTR("BGM"));
	print(11,16,PSTR("GFX"));
	print(11,17,PSTR("QUIT"));
	//now replace those ugly blocky corners with rounded ram tiles
	//...
	while(true){
		FillPad();
		if(UpDown() && cursorpos > 0)	  {cursorpos--;}
	  else if(DownDown() && cursorpos < 6){cursorpos++;}
	  else if(StartDown()){
	  //time to restore vram
		  off = 14*64;//0;
			for(u8 y=5;y<10;y++)
			for(u8 x=5;x<10;x++)
			 SetMapTile(x,y,ram_tiles[off++]);//restore[off++]);
			
		 guijuststarted = false;
		 guistate = 0;
		 if(cursorpos == 0){//back
			 tracknum = oldtrack;
			tileset = oldgfx;
		 }
		 else if(cursorpos == 1){//retry
			  RetryLevel();
		 }
		 else if(cursorpos == 2){//give up
			DoScreenOutEffect();
			guijuststarted = true;
			  guistate = GLEVELSELECT;
		 }
		 else if(cursorpos == 3){//solve
			  // DoScreenOutEffect();
			PlayDemo(level);
			HideSprites(0,MAX_SPRITES);
			guijuststarted = true;
			guistate = GLEVELSELECT;
		 }
		 else if(cursorpos == 4){//bgm
			 StopSong();
			WaitVsync(2);//seems like songs are messed up somehow

			musicplaying = true;

				musicpreference = true;
			preferredtrack = tracknum;

			if(musicoff && (tracknum == tileset+1)){//user has selected the default song, so now lets the music be default again
					musicoff = false;
				musicpreference = false;
				musicplaying = true;
			}

			if(tracknum == 1)	  {StartSong(AmericaSong);}
				else if(tracknum == 2){StartSong(EgyptSong);}
				else if(tracknum == 3){StartSong(ChinaSong);}
				else if(tracknum == 4){StartSong(SouthPoleSong);}
				else if(tracknum == 5){StartSong(JapanSong);}
			else						{musicplaying=false;musicoff=true;musicpreference=false;}

		 }
		 else if(cursorpos == 5){//gfx
			 graphicspreference = true;
				if(gfxnum != tileset){
				Render();
				SwapTileSet(gfxnum);
				 graphicspreference = true;
			}
			else
				graphicspreference = false;//allow user to use default tiles again
		 }
		 else{//quit
				guistate = GMAINMENU;
			guijuststarted = true;
			DoScreenOutEffect();//there is a strange bug here, apply band-aid
			HideSprites(0,MAX_SPRITES);
			FillScreen(BLANKT);
		 }

		 if(cursorpos < 4 && cursorpos > 5){
				tileset = oldgfx;
			tracknum = oldtrack;
		 }
		 return;
	  }
	  if(cursorpos == 4){//BGM
		 if(tracknum > 0 && LeftDown()) {tracknum--;}
		 else if(tracknum < 5 && RightDown()){tracknum++;}
		}
	  else if(cursorpos == 5){//GFX
		  if(gfxnum > 0 && LeftDown()) {gfxnum--;}
		else if(gfxnum < 4 && RightDown()){gfxnum++;}
	  }
	  if(tracknum == 0 || tracknum == 255){print(15,15,PSTR("OFF"));}else{SetTile(16,15,FONTSTART);if(tracknum==pgm_read_byte(&Maps[((level<<3)*14)+level])+1){SetTile(17,15,FONTSTART+2);}else{SetTile(17,15,FONTSTART);}printb(15,15,tracknum);}
	  printb(15,16,gfxnum+1); if(pgm_read_byte(&Maps[((level<<3)*14)+level])==gfxnum){SetTile(17,16,FONTSTART+2);}else{SetTile(17,16,FONTSTART);}
	  for(u8 i=0;i<7;i++){SetTile(10,11+i,MENUSTART+2);}//blank out the cursor
	  SetTile(10,11+cursorpos,MENUSTART+4);
	  WaitVsync(1);
	}
};


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void VictoryMenu(){
	StopSong();

	poffset = px*16;//move the player off the screen
	px = 0;
	bx = 255;
	pstate = RIGHT;	
	guijuststarted = false;
	while(poffset < (28*8)){
		poffset += 2;
		if(pftime){pftime--;}else{pftime = 4;pframe=!pframe;}
	  Render();
	}
	HideSprites(0,MAX_SPRITES);

	StartSong(LevelClearSong);
	musicplaying = false;//make sure this is reloaded if necessary inside LoadLevel
	
	WaitVsync(10);
	px = 14;
	py = 6;
	poffset = 0;
	pstate  = LEFT;

	DoScreenOutEffect();
	WaitVsync(1);
	FillScreen(BLANKT);
	FadeOut(1,true);//go blank so we avoid graphical artifacts
	

	int m,p;//save moves/pushes
	m = moves;
	p = pushes;

	//calculate and store achievments
	CalculateOptimum(level);
	FillScreen(BLANKT);
	musicplaying = false;//shouldn't be here

	SetLevelCompleted(level);
	if(m <= optimummoves)
		SetMetLevelPar(level);

	FadeIn(1,true);

	DrawOpeningMenu(9,15,11,10,-2,false);

		print (10,16,PSTR("MOVES")); if(m <= optimummoves) {print (16,16,PSTR("5"));}
		print (10,17,PSTR("PUSHES"));if(p <= optimumpushes){print (16,17,PSTR("5"));}
		printi(19,16,m);
		printi(19,17,p);
		
	  if(m <= optimummoves && p <= optimumpushes){print(11,19,PSTR("GREAT444"));}
	  else													{print(13,19,PSTR("OK44"));}

		print (11,21,PSTR("CPU3BEST"));
		print (10,23,PSTR("MOVES35"));
		print (10,24,PSTR("PUSHES5"));
		printi(19,23,optimummoves);
		printi(19,24,optimumpushes);
	
	WaitVsync(1);

	u8 lvl = level+1;

	u8 banneroff = 29;
	for(u8 i=0;i<90;i++){
		for(u8 j=0;j<30;j++){vram[(12*30)+j]=BLANKT+RAM_TILES_COUNT;}//clear the blur

		printrainbow(banneroff,12,PSTR("STAGE3333CLEARED"),0,0xC0,0x00,i%5);//printrainbow(6,12,strStageCleared,0,0xC0,0x00,i%5);

		for(u8 j=0;j<30;j++){vram[(13*30)+j]=BLANKT+RAM_TILES_COUNT;}//TODO ugly hack for print rainbow messing up??

		if(banneroff < 30-9){
		  if(lvl>9){printdigitoverlaid(banneroff+6,12,lvl/10,21,0xC0,BLANKT);}//printdigitoverlaid(13,12,lvl/10,21,0xC0,BLANKT);}
		  printdigitoverlaid(banneroff+7,12,lvl%10,20,0xC0,BLANKT);
	  }

	  if(banneroff > 7)
		  banneroff--;
		  

	  for(u8 i=0;i<6;i++){
		  FillPad();
		  if(StartDown()){
			  StopSong();
			WaitVsync(1);
			return;
			}

  
			 WaitVsync(1);
	  }
	}
	StopSong();
	WaitVsync(1);
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void LevelSelect(){
	WaitVsync(1);
	FadeIn(1,false);
	FillScreen(BLANKT);
	WaitVsync(1);
	u8 mappos = level;
	u8 coff = 0;
	u8 cwait = 30;
	bool load = false;
	LoadLevel(true,false,false,false,false,true);

	DrawOpeningMenu(0,0,29,2,0,false);
		 if(levelcompleted){print(26,1,PSTR("5"));}else{print(26,1,PSTR("6"));}
		 if(levelparmet)		 {print(27,1,PSTR("5"));}else{print(27,1,PSTR("6"));}

	while(true){
		FillPad();
	  if	  (padstate & BTN_SL && !(oldpadstate & BTN_SL) && mappos > 0)		  {load=true;if(mappos < 10){mappos = 0;}else{mappos-=10;}}
	  else if(padstate & BTN_SR && !(oldpadstate & BTN_SR) && mappos < numlevels-1){load=true;mappos+=10;if(mappos >= numlevels){mappos = numlevels-1;}}
		//if(UpDown() && cursorpos > 0)		 {cursorpos--;}
	  //else if(DownDown() && cursorpos < 3){cursorpos++;}
	  else if(StartDown()){
			guijuststarted = false;
		 guistate = 0;
		 level = mappos;
		 LoadLevel(true,true,false,false,false,false);

		 return;
	  }
	  else if(LeftDown() && mappos > 0)			{mappos--;load=true;}
	  else if(RightDown() && mappos < NUMLEVELS-1){mappos++;load=true;}

		if(load){
		 DrawMenu(0,0,29,2);
		  DoScreenOutEffect();
		  WaitVsync(1);
		 load = false;
		 level = mappos;
		 LoadLevel(true,true,false,true,false,true);
			DrawMenu(0,0,29,2);
		 if(levelcompleted){print(26,1,PSTR("5"));}else{print(26,1,PSTR("6"));}
		 if(levelparmet)		 {print(27,1,PSTR("5"));}else{print(27,1,PSTR("6"));}
		 //ComposeRamTile((NUMTILESPERSET*tileset)*64,(tileset*28)+4,(25*64));
		 //WaitVsync(1);
		 VramFillWindow(px*2,py*2,2,2,25);
		 // printrainbow(2,1,strLevelSelect,4,0,0,coff);
		 //printb(10+7,1,mappos+1);
		  WaitVsync(1);
	  }
	  if(!cwait){
		  cwait = 30;
	  //	printrainbow(9,1,strLevelSelect,4,0,0,coff++);
		 
		 if(tileset==0)	  {printrainbow(6,1,PSTR("STAGE3333AMERICA")  ,11,0,0,coff++);}
		 else if(tileset==1){printrainbow(6,1,PSTR("STAGE3333EGYPT")	 ,11,0,0,coff++);}
		 else if(tileset==2){printrainbow(6,1,PSTR("STAGE3333CHINA")	 ,11,0,0,coff++);}
		 else if(tileset==3){printrainbow(6,1,PSTR("STAGE3333SOUTHPOLE"),11,0,0,coff++);}
		 else					{printrainbow(6,1,PSTR("STAGE3333JAPAN")	 ,11,0,0,coff++);}
		 printb(6+6,1,mappos+1);
			if(coff > 3){coff = 0;}
	  }
	  else
		  cwait--;

		spritecount = 0;
		DrawSpriteTile((px*16),(py*16),5+(cwait>14));
	  WaitVsync(1);
	}
}


/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


void OptionsMenu(){
	
	while(true){
		FillPad();
		WaitVsync(1);
	}
}


/*
void HelpMenu(){
	FillScreen(BLANKT);
	u8 coff = 0;
	u8 coffcount = 0;
	cursorpos = 0;
	FadeIn(1,true);
	
	DrawOpeningMenu(0,25,5,2,0);
	DrawOpeningMenu(2,2,21,6,0);
	DrawOpeningMenu(10,18,16,5,0);
	
 //  DrawMenu(10,18,16,8);
	print(11,19,strHelpMenu2);
	print(11,20,strHelpMenu3);
	//print(11,21,strHelpMenu4);
	print(11,21,strHelpMenu5);
	//print(11,23,strHelpMenu6);
	print(11,22,strBack);
	printramtiles(10,17,strHelpMenu1,0,0xC0,0x00);
//	print(11,24,strHelpMenu7);
	while(true){
		FillPad();
	  if(UpDown() && cursorpos > 0)		 {cursorpos--;DrawMenu(2,2,21,6);}
	  else if(DownDown() && cursorpos < 3){cursorpos++;DrawMenu(2,2,21,6);}
	  else if(StartDown()){
			if(cursorpos == 0){

		 }
		 else{
				guistate = GMAINMENU;
			 FadeOut(1,true);
			 FadeIn(1,false);
			 return;
		 }
  
	  }

	  for(u8 i=0;i<4;i++){SetTile(10,19+i,MENUSTART+2);}//blank out cursor
		SetTile(10,19+cursorpos,MENUSTART+4);
	  //Draw the menu specific stuff
	  if(cursorpos == 0){//game controls
			print(3,3,strControls1);
			print(3,4,strControls2);
			print(3,5,strControls3);
			print(3,6,strControls4);
			print(3,7,strControls5);
	  }
	  else if(cursorpos == 1){//game basics
			print(3,3,strGameBasics1);
			print(3,4,strGameBasics2);
			print(3,5,strGameBasics3);
			print(3,6,strGameBasics4);
			print(3,7,strGameBasics5);
	  }
	  else if(cursorpos == 2){//editor controls
			print(3,3,strEdControls1);
			print(3,4,strEdControls2);
			print(3,5,strEdControls3);
			print(3,6,strEdControls4);
			print(3,7,strEdControls5);
			print(3,7,strEdControls6);
			print(3,7,strEdControls7);
	  }
	  else{//back
			print(3,3,strHelpMenu7);
	  }
	  if(coffcount < 5){coffcount++;}else{coffcount=0;coff++; if(coff>4){coff = 0;}}
		printrainbow(1,26,strMainMenu4,19,0,0,coff);
	  WaitVsync(1);
	}
}
*/

void DoScreenOutEffect(){
	WaitVsync(1);
	HideSprites(0,MAX_SPRITES);
	RamifyTileSet();
	RamifyScreen();

	int off = outeffectnum*6;
	u8 parms[6];

	for(u8 i=0;i<6;i++)
		parms[i] = pgm_read_byte(&ScreenFadeParms[off++]);
	
	WaitVsync(1);
	BitMaskScreenOut(parms[0], parms[1], parms[2], parms[3], parms[4], parms[5]);


	if((++outeffectnum)>=(NUMOUTEFFECTS)){outeffectnum=0;}
}



void DoScreenInEffect(){
	if(++ineffectnum>NUMINEFFECTS){ineffectnum=0;}
}

void AmericaEnterance(){

}

void EgyptEnterance(){

}


void SouthPoleEnterance(){

}

void ChinaEnterance(){

}

void JapanEnterance(){


}



void ColorSection(u8 x, u8 y, u8 w, u8 h, u8 c, u8 rt){//fill a section with color
	for(u8 j=y;j<y+h+1;j++){
	for(u8 i=x;i<x+w+1;i++){//first set up the section with the ram tile index
		vram[(j*30)+i] = rt;
	  }
	}
	int off = rt<<6;//get offset to the ram tile we are using
	for(u8 i=0;i<64;i++){ram_tiles[off++] = c;}
}

const char strIntro1[] PROGMEM = "UZEBOX";

void GameIntro(){
#ifndef NOINTRO
	FillScreen(FONTSTART-1);//ColorSection(0,0,29,27,255,29);
	FadeIn(2,false);
	
	for(u8 i=0;i<3;i++){
		printramtilesfancy(13,14,strIntro1,22,0xFF,8-i,0xC0,0);WaitVsync(10);
	}
 
	
	WaitVsync(45);

	for(u8 i=4;i<10;i++){
		printramtilesfancy(13,14,strIntro1,22,0xFF,8-i,0xC0,0);WaitVsync(10);
	}

	
	WaitVsync(60);
	FadeOut(3,true);
	FadeIn(1,false);

/*

	tileset = 1;
	u8 xpos = 27;
	u8 flop = false;
	while(xpos > 4){
		printramtiles(xpos,14,strIntro1,22,0xC0,0);
		for(u8 i=xpos+6;i<30;i++){SetTile(i,14,FONTSTART-1);}//vram[(14*30)+i] = 29;}
	  xpos--;
	  spritecount = 0;
	  tileset = 0;
		DrawSpriteTile((xpos+7)*8,(13*8),11+flop);
		WaitVsync(4);
	  flop = !flop;
	}

	while(xpos < 30-9){
		printramtiles(xpos,14,strIntro1,22,0xC0,0);
		for(u8 i=xpos-1;i>0;i--){SetTile(i,14,FONTSTART-1);}//vram[(14*30)+i] = 29;}
	  xpos++;
	  spritecount = 0;
	  tileset = 0;
		DrawSpriteTile((xpos+5)<<3,(13*8),11+flop);
		tileset = 2;
		DrawSpriteTile((xpos-3)<<3,(13*8),15+flop);
		tileset = 4;
		DrawSpriteTile((xpos-4)<<3,(13*8),15+flop);
	  WaitVsync(4);
	  flop = !flop;
	}

	

	while(xpos > 12){
		printramtiles(xpos,14,strIntro1,22,0xC0,0);
		for(u8 i=xpos+6;i<30;i++){SetTile(i,14,FONTSTART-1);}//vram[(14*30)+i] = 29;}
	  xpos--;
	  spritecount = 0;
	  tileset = 0;
		DrawSpriteTile((xpos+7)<<3,(13*8),11+flop);
	  tileset = 1;
		DrawSpriteTile((xpos+8)<<3,(13*8),11+flop);
	  tileset = 2;
		DrawSpriteTile((xpos-1)<<3,(13*8),15+flop);
	  tileset = 4;
		DrawSpriteTile((xpos-2)<<3,(13*8),15+flop);
		WaitVsync(4);
	  flop = !flop;
	}


//	u8 ypos = 1;
	
	//Not enough time to draw 5 characters plus printramtile, trashing stack. 
	//So compose the sprites onto ram tiles so we don't have to process them anymore. Now it works?? I wont ask questions...

	u8 ypos = 1;
	while(ypos < (13*8)){
		tileset = 3;
	  spritecount = 20;
		DrawSpriteTile((15*8),ypos,6);
	  ypos += 8;
	  WaitVsync(3);
	}


  // HideSprites(0,MAX_SPRITES);

	for(u8 i=0;i<2;i++)
	for(u8 y=0;y<2;y++)
	for(u8 x=0;x<3;x++){
	//	vram[((y+13)*30)+x+19] = pgm_read_byte(&IntroMap[(y*3)+x]);
	//	vram[((y+13)*30)+x+10] = pgm_read_byte(&IntroMap[(y*3)+x+6]);
	}

	WaitVsync(1);
		

	for(u8 i=0;i<8;i++){
	  spritecount = 20;
		DrawSpriteTile(15*8,12*8,6-flop);
		printramtilesfancy(13,14,strIntro1,22,0xFF,8-i,0xC0,0);WaitVsync(10);
		flop = !flop;
	}
	
	for(u8 i=0;i<8;i++){
	  spritecount = 20;
		DrawSpriteTile((15*8),(12*8),6-flop);
		printramtilesfancy(13,14,strIntro1,22,0xFF,i,0xC0,0);WaitVsync(10);
		//flop = !flop;
	}

//	u8 r,t;
//	int off;
	tileset = 0;
	spritecount = 0;
	FadeOut(5,true);
	for(u8 k=0;k<64;k++){//we'll draw 64 pixels, eventually..
		do{
		  r = rand()%64;//get a random global index
		  t = ram_tiles[r];
		}
	  while(t == 0);//go until we find a pixel we haven't drawn

	  for(u8 i=0;i<11;i++){
		  off = i*64;
			ram_tiles[r+off] = 255;

		}
	  WaitVsync(1);//finish in clumps of 3
	}
	//WaitVsync(20);
	*/
#endif
}









#ifndef NOENDING
const char strEnd1[] PROGMEM = "WOW44333YOU3DID3IT44";  
const char strEnd2[] PROGMEM = "IF3YOU3DID3NOT3USE3EMUZE"; 
const char strEnd3[] PROGMEM = "THEN3GREAT3JOB444"; 
const char strEnd4[] PROGMEM = "YOU3ARE3LIKELY3THE3ONLY"; 
const char strEnd5[] PROGMEM = "PERSON3TO3EVER3SEE3THIS4";//even now that the source is released :) 
const char strEnd6[] PROGMEM = "CONSIDER3YOURSELF3THE3BEST";
const char strEnd7[] PROGMEM = "SOKOBAN3WORLD3PLAYER3EVER4";  
const char strEnd8[] PROGMEM = "THANKS3FOR3PLAYING3ALL3THE";
const char strEnd9[] PROGMEM = "WAY3THROUGH43SERIOUSLY3THANKS";
const char strEnd10[] PROGMEM = "SORRY3FOR3THE3LAME3ENDING";
const char strEnd11[] PROGMEM = "I3AM3JUST3OUT3OF3TIME3NOW";
const char strEnd12[] PROGMEM = "CREATED3BY3LEE3WEBER";
//const char strWasteSpace[] PROGMEM = "5WASTE3THE3SPACE33";
#endif

void GameEnding(){
#ifndef NOENDING
	FadeIn(3,false);
	//print(0,0,strWasteSpace);
	//print(1,0,strWasteMore);
	FillScreen(BLANKT+1);
	WaitVsync(1);
	
	for(u8 i=0;i<sizeof(strEnd1)-1;i++){
		SetTile(6+i,4,(pgm_read_byte(&strEnd1[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd2)-1;i++){
		SetTile(4+i,5,(pgm_read_byte(&strEnd2[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd3)-1;i++){
		SetTile(8+i,6,(pgm_read_byte(&strEnd3[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd4)-1;i++){
		SetTile(4+i,8,(pgm_read_byte(&strEnd4[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd5)-1;i++){
		SetTile(4+i,9,(pgm_read_byte(&strEnd5[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd6)-1;i++){
		SetTile(3+i,12,(pgm_read_byte(&strEnd6[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd7)-1;i++){
		SetTile(3+i,13,(pgm_read_byte(&strEnd7[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	WaitVsync(60);

	for(u8 i=0;i<sizeof(strEnd8)-1;i++){
		SetTile(3+i,15,(pgm_read_byte(&strEnd8[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd9)-1;i++){
		SetTile(1+i,16,(pgm_read_byte(&strEnd9[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd10)-1;i++){
		SetTile(3+i,18,(pgm_read_byte(&strEnd10[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	for(u8 i=0;i<sizeof(strEnd11)-1;i++){
		SetTile(3+i,19,(pgm_read_byte(&strEnd11[i])&127)+FONTSTART-51);
	  WaitVsync(8);
	}

	print(4,24,strEnd12);
	//printb(12,25,20);
	//printb(14,25,10);

	

	WaitVsync(255);
#endif
}

void Gui(){
	if(!guistate)
		return;
	switch(guistate){
	case GMAINMENU:
	MainMenu();
	break;
	case GLEVELSELECT:
	LevelSelect();
	break;
//	case GEDITOR:
//	Editor();
//	break;
//	case GOPTIONS:
//	OptionsMenu();
//	break;

//	case GHELP:
//	HelpMenu();
//  break;

	case GVICTORY:
	VictoryMenu();
	break;
	case GINGAMEMENU:
	InGameMenu();
	break;
	}
}
