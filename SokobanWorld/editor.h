//Lost cause - better off writing it from scratch(no idea how it got this bad...)
void EditorMenu();
void SetUpEdTile(u8 t){//fill up ram tiles with the current editor tile and stipple it with black
	int off,oldoff,toff;
	off = (26*64);
	oldoff = off;
	if(t < 6){
		toff = (tileset*NUMTILESPERSET)*64;
		toff += (t*4)*64;
	  ttoram(toff,off,(64*4));
		//while(off < 30*64)
		  // ram_tiles[off++] = pgm_read_byte(&GameTiles[toff++]);
	}
	else{//it's a player tile so we have to get it from sprites
		toff = (tileset*NUMSPRITESPERSET)*64;
		toff += (12)*64;//get down direction
		off = (22*64);
	  storam(toff,off,(4*64));
	}	

	for(u8 i=0;i<8;i++){
		ram_tiles[oldoff+i] = ram_tiles[oldoff+i+64] = TITLERED;//top
		ram_tiles[oldoff+i+184] = ram_tiles[oldoff+i+248] = TITLERED;//bottom
		ram_tiles[oldoff+(i*8)] = ram_tiles[oldoff+(i*8)+128] = TITLERED;//left
		ram_tiles[oldoff+(i*8)+71] = ram_tiles[oldoff+(i*8)+199] = TITLERED;//right
	}
}

void EdBlankMap(){
	for(u8 y=0;y<14;y++)
	for(u8 x=0;x<15;x++)
		SetMapTile(x,y,0);
}

inline void EdSaveMap(u8 slot){
	unsigned int off = SOKOBANSAVEOFFSET+(slot*8*14);
	u8 t[2];
	u8 t2;	

	WriteEeprom(off++,0);//write the tileset

	for(u8 y=0;y<14;y++)
	for(u8 x=0;x<15;x+=2){
		t[0] = GetMapTile(x+0,y);
		t[1] = GetMapTile(x+1,y);
		t2	= t[0] + (t[1]<<4);
	  WriteEeprom(off++,t2);
	}
}


void EdLoadMap(){
	LoadLevel(true,false,false,true,false);
}

void EdTest(){

}

void CopyRamTiles(u8 st, u8 dt){//copy 4 ram tiles to some other ram tiles. used for making a player tile
	int soff = st*64;
	int doff = dt*64;
	for(int i=0;i<(4*64);i++)
		ram_tiles[doff++] = ram_tiles[soff++];
}

void SetUpPlayerTiles(){
	int soff = ((tileset*NUMTILESPERSET)+4)*64;//get floor tile
	int doff = 7*64;
	int oldoff = doff;
	u8 t;
	int spoff = ((tileset*NUMSPRITESPERSET)+12)*64;//get the down direction
	for(int i=0;i<(4*64);i++){
		t = pgm_read_byte(&GameSprites[spoff++]);
	  if(t == 0xFE)//transparent color
		  ram_tiles[doff] = pgm_read_byte(&GameTiles[soff]);
		else
			ram_tiles[doff] = t;
	  doff++;
	  soff++;
	}
	for(u8 i=0;i<8;i++){ram_tiles[oldoff+i] = ram_tiles[oldoff+i+64] = TITLEBLUE;}//top
	for(u8 i=0;i<8;i++){ram_tiles[oldoff+i+184] = ram_tiles[oldoff+i+248] = TITLEBLUE;}//bottom
	for(u8 i=0;i<8;i++){ram_tiles[oldoff+(i<<3)] = ram_tiles[oldoff+(i*8)+128] = TITLEBLUE;}//left
	for(u8 i=0;i<8;i++){ram_tiles[oldoff+(i<<3)+71] = ram_tiles[oldoff+(i*8)+199] = TITLEBLUE;}//right
}

void EdSwapTileSet(bool dir, u8 curtile){
	int t;
	int o;
	if(dir){t = 24;tileset++;}
	else	{t = -24;tileset--;}
	SetUpPlayerTiles();
	SetUpEdTile(curtile);
	WaitVsync(1);
	for(u8 y=0;y<28;y++)
	for(u8 x=0;x<30;x++){
		o = (y*30)+x;
		if(vram[o] >= RAM_TILES_COUNT)
		  vram[o] += t;
	}
}

void Editor(){//This is horrible code, all I have to say about it.

	level = 0;
	opentargets = 0;
	px = 255;
	EdBlankMap();
	SetUpPlayerTiles();
	FadeIn(1,false);
	u8 ex,ey;
	u8 etile,rtile,etileset;//edit tile, restore tile stores what we wrote over
	u8 mpos;
	u8 lpos = 0;
	u8 spos = 0;
	u8 restore[24];
	int off=0;
	ex = ey = 10;
	tileset = 0;
	etile = 6; 
	rtile = 0;
	etileset = 0;

	DrawOpeningMenu(10,12,9,2,-4);
	for(u8 i=0;i<2;i++)
	for(u8 i=0;i<4;i++){
		printb(17,13,off==0);
		printrainbow(11,13,strSokoEd,0,0,0,i);
		print(17,13,strVersion);
		WaitVsync(12);
	}
	EdBlankMap();
	SetUpEdTile(etile);
	while(true){
		FillPad();
	  if(UpDown()){
		  SetMapTile(ex,ey,rtile);
			if(--ey >254){ey=0;}//rolled over
		 rtile = GetMapTile(ex,ey);
	  }
	  else if(DownDown()){
		  SetMapTile(ex,ey,rtile);
			if(++ey > 13){ey = 13;}
		 rtile = GetMapTile(ex,ey);
	  }
	  else if(LeftDown()){
		  SetMapTile(ex,ey,rtile);
		  if(--ex > 254){ex=0;}
		 rtile = GetMapTile(ex,ey);
	  }
	  else if(RightDown()){
		  SetMapTile(ex,ey,rtile);
		  if(++ex > 14){ex=14;}
		 rtile = GetMapTile(ex,ey);
	  }
	  else if(padstate & BTN_A){
		 rtile = etile;
			if(etile < 6){//it's not a player
			SetMapTile(ex,ey,etile);
			 if(ex==px && ey==py){
			  // if((px > 0 && GetMapTile(px-1,py)) || (px < 14 && GetMapTile(px+1,py)) || (py < 13 && GetMapTile(px,py+1)) || (py > 0 && GetMapTile(px,py-1)))
				 //  SetMapTile(px,py,1);//don't put a floor under the player if he was just in the middle of nowhere
				px=255;
			}
			
			if(etile==2){opentargets++;}
		 
		 }
		 else{SetMapTile(px,py,0);px = ex; py = ey;}
	  }
	  else if(padstate & BTN_SR && !(oldpadstate & BTN_SR) && etile < 6){SetUpEdTile(++etile);}
	  else if(padstate & BTN_SL && !(oldpadstate & BTN_SL) && etile > 0){SetUpEdTile(--etile);}
	  else if(padstate & BTN_X && !(oldpadstate & BTN_X) && etileset > 0){etileset--;SetMapTile(ex,ey,rtile);EdSwapTileSet(false,etile);}
	  else if(padstate & BTN_Y && !(oldpadstate & BTN_Y) && etileset < NUMTILESETS-1){etileset++;SetMapTile(ex,ey,rtile);EdSwapTileSet(true,etile);}
	  else if(StartDown()){
			FillPad();//make sure user has to let up
		 mpos = 0;
		 off = 0;
			for(u8 y=5;y<9;y++)
			for(u8 x=5;x<10;x++)
			  restore[off++] = GetMapTile(x,y);//save what we will draw over
		  
		DrawOpeningMenu(11,10,7,7,0);

		  print(12,11,strBack);  
		  print(12,12,strEdMenu1);
		  print(12,13,strEdMenu2);
		  print(12,14,strEdMenu3);
		print(12,15,strEdMenu4);
		print(12,16,strQuit);
		 
		 while(!StartDown()){
			 FillPad();
				if(UpDown() && mpos > 0){--mpos;}
			else if(DownDown() && mpos < 5){++mpos;}
			  else if(LeftDown()){
				if(mpos == 1 && spos > 0){spos--;}
				else if(mpos == 2 && lpos > 0){lpos--;}
			}
			  else if(RightDown()){
				if(mpos == 1 && spos < 2){spos++;}
				else if(mpos == 2 && lpos < 2){lpos++;}
			}
			 for(u8 i=0;i<6;i++){SetTile(11,11+i,MENUSTART+2);}//blank out the cursor
			  SetTile(11,11+mpos,MENUSTART+4);

			SetTile(17,12,FONTSTART+14+spos);
			SetTile(17,13,FONTSTART+14+lpos);
			WaitVsync(1);
		 }
			 off = 0;//have to restore the window
				for(u8 y=5;y<9;y++)
				for(u8 x=5;x<10;x++)
				 SetMapTile(x,y,restore[off++]);
		 //ok start is down see what menu position we are at
			if(mpos == 0){//back

		 }
		 else if(mpos == 1){//save
			 if(px == 255){px = py = 0;SetMapTile(px,py,1);}//HACK, why isn't this done already??!!?!
				EdSaveMap(spos);
		 }
		 else if(mpos == 2){//load
				EdLoadMap(lpos);
		 }
		 else if(mpos == 3){//new
				EdBlankMap();
			px = 255;
		 }
		 else if(mpos == 4){//test
			 if(px != 255)
				SetMapTile(px,py,1);
				
			off = 14*64;
			for(u8 y=0;y<14;y++)//we have to store the current map state but that takes a lot of ram				
				for(u8 x=0;x<15;x++)//so use extra ramtiles we wont use during the test so we don't crunch the stack 
				ram_tiles[off++] = GetMapTile(x,y);

			SetMapTile(ex,ey,rtile);//have to restore the editor tile
				
			u8 tx,ty;//will have to restore px,py
			tx = px; ty = py;
			 guistate = 0;
			guijuststarted = false;
			 if(!opentargets){opentargets++;}//make sure there is atleast a fake target count so map doesn't exit immediately if there are 0 targets
			if(px == 255){px=py=0;}
			while(!guistate && opentargets){//keep going until player hits start or wins
					Input();
				Logic();
				Render();
			}
			HideSprites(0,MAX_SPRITES);
			off = 14*64;
			for(u8 y=0;y<14;y++)//restore the map to it's previous state we used ram tiles because this take a lot of ram,
			for(u8 x=0;x<15;x++)//we have tons of ram tiles we wont use during the test. if there is a problem look here...
				SetMapTile(x,y,ram_tiles[off++]);

			px = tx; py = ty;
				SetUpPlayerTiles();//HACK, for some reason the ram tiles are getting screwed up, just redraw them instead of figuring it out...
		 }
		 else{//quit
				guijuststarted = true;
			guistate = GMAINMENU;
			return;
		 }

		 SetUpEdTile(etile);//restore the ed tile
		 off = 0;//time to restore the window
			for(u8 y=5;y<9;y++)
			for(u8 x=5;x<10;x++){
			 if(mpos != 2){SetMapTile(x,y,restore[off++]);}
			else			{SetMapTile(x,y,0);}//don't restore if we blanked the map
			}
		  // if(mpos != 2)
			 //  SetMapTile(ex,ey,0);
	  }

	  if(px != 255){
		  off = ((py*2)*30)+(px*2);
		  vram[off+ 0] = 22;
		  vram[off+ 1] = 23;
		  vram[off+30] = 24;
		  vram[off+31] = 25;		
		}
	  u8 t = 26;
	  if(etile == 6){t = 22;}else{t=26;}
		  off = ((ey*2)*30)+(ex*2);//get offset into vram
		 vram[off+ 0] = t++;//point vram at our ram tiles we set up
		 vram[off+ 1] = t++;
		 vram[off+30] = t++;
		 vram[off+31] = t;

HideSprites(0,MAX_SPRITES);//BIG HACK! SOMETHING WEIRD HAPPENS WHEN YOU PLACE A PLAYER?
		WaitVsync(1);
	}
	
}

