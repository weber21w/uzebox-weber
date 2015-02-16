void LoadLevel(bool flushmoves, bool fadein,bool shiftdown, bool skiptoprow, bool fromeep, bool skipsong){
	gamestate = 0;//turn off fast flag
	moves = pushes = 0;
	px = py = poffset = opentargets = 0;

	if(level > NUMLEVELS){level = NUMLEVELS-1;}

	levelcompleted = IsLevelCompleted(level);
	levelparmet	 = HasMetLevelPar(level);

//	movepos = 0;
	if(flushmoves){//don't flush moves if this was called from Undo()
/*	  // movepos = 0;
	  nummoves = 0;
	  for(int i=0;i<MOVELISTSIZE;i++)
		  movelist[i] = 0;
*/
	}
	if(fadein){//we have to copy the tiles to ram tiles
  /*	 HideSprites(0,MAX_SPRITES);
	  
	  for(int i=0;i<(24*64);i++){
				 
	  }
*/
	}

	bx = 255;
	pstate = DOWN;
	u8 t;

	int moff = (level*((8*14)+LEVELINFOSIZE));//+(fromeep*SOKOBANSAVEOFFSET);
	u8 defaulttileset;


	defaulttileset = pgm_read_byte(&Maps[moff++]);

	if(!graphicspreference)//user hasn't specified a tileset
		tileset = defaulttileset;

//musicoff = skipsong = musicplaying = false;
//musicpreference = false;

	if(!musicoff && !skipsong){//user hasn't specified no music so see if we should load music
		u8 oldtrack = tracknum;

		if(musicpreference)//user has specified a music track
		  tracknum = preferredtrack;
		else
		  tracknum = defaulttileset+1;//use tile set default
		
	  if(!musicplaying || (oldtrack != tracknum)){//music is different so start a new song, otherwise just let the song continue
	  //note that the victory menu will set tracknum to 255 so that it always loads a song after(unless music off ^)
			
		 if(tracknum == 1)			{StartSong(AmericaSong);}
			else if(tracknum == 2)	 {StartSong(EgyptSong);}
			else if(tracknum == 3)	 {StartSong(ChinaSong);}
			else if(tracknum == 4)	 {StartSong(SouthPoleSong);}
			else /*if(tracknum == 5)*/{StartSong(JapanSong);}	

		 musicplaying = true;
	  }

	}





/*
	if(musicpreference)
		tracknum = preferredtrack;
	else
		tracknum = tileset+1;
	
	if(!musicplaying && !skipsong){// && loadsong && (tracknum != tileset+1)){
		StopSong();
		musicplaying = true;
	  if(tracknum == 0)		{musicplaying = false;musicoff = true;}//music off
		if(tracknum == 1)	  {StartSong(AmericaSong);}
		else if(tracknum == 2){StartSong(EgyptSong);}
		else if(tracknum == 3){StartSong(ChinaSong);}
		else if(tracknum == 4){StartSong(SouthPoleSong);}
		else if(tracknum == 5)	{StartSong(JapanSong);}
	}
*/


	for(u8 y=skiptoprow;y<LEVELHIGH;y++)//don't draw over menu
	for(u8 x=0;x<LEVELWIDE;x++){
		if(!fromeep){t = pgm_read_byte(&Maps[moff+(y<<3)+(x>>1)]);}
	  else		  {t = ReadEeprom(moff+(y<<3)+(x>>1));}
	  
	  t = ( t>>((4)*(x%2)) ) & 15; //decompress 4 bits for each tile

	  if(t >= SOKOSTART){
		  px = x;
		 py = y;  
			if(t > SOKOSTART){t=TARGET;}//sokoban starts on a target;
		 else				 {t=FLOOR;}
			
	  }
	  else if(t == TARGET){opentargets++;}
/*	  else if(!fillsky){
		  t2 = GetMapTile(x,y);
		  if(t == 0 || t2 < RAM_TILES_COUNT || t2 > ENDGAMETILES)
		  continue;//don't fill in the sky, we must be drawing inside a menu so don't overwrite anything
	  }
*/	 /* if(false){//t == 0 && tileset == 1){//set up for ram tile effects on background
		  int v = ((y*2)*30)+(x*2);
		  vram[v+ 0] = 20;
		 vram[v+ 1] = 21;
		  vram[v+30] = 22;
		  vram[v+31] = 23;
	  }
	  else*/
		  SetMapTile(x,y+shiftdown,t);//this is relative to tile set
	}
}

void Logic(){
	if(guijuststarted)
		return;
	
	if(!opentargets && !(gamestate & SCANNINGOPTIMUM)){//victory!
		if(demoplaying)
		  return;

		WaitVsync(5);//let box sfx finish
		VictoryMenu();
	  FadeOut(2,true);

		level++;
	  if(level >= (numlevels-1)){
		  level = numlevels-1;

#ifndef NOENDING
		 if(AllLevelsCompleted())
				GameEnding();
#endif
		 guijuststarted = true;
		 guistate = GLEVELSELECT;
		}
	  else{
			//if(level < FIRSTEGYPTLEVEL)
			// AmericaEnterance();
		 //else 
		 if(level == FIRSTEGYPTLEVEL)
				EgyptEnterance();
			else if(level == FIRSTSOUTHPOLELEVEL)
				SouthPoleEnterance();
			else if(level == FIRSTCHINALEVEL)
				ChinaEnterance();
			else if(level == FIRSTJAPANLEVEL)
				JapanEnterance();
		}

	  WaitVsync(1);
	  LoadLevel(true,true,false,false,false,false);
	  FadeIn(2,false);
	  return;
	}

	if(pftime)
		pftime--;
	else{
		pftime = PFRAMELENGTH;
		pframe = !pframe;
	}
	
	if(pstate & MOVE){
		if(poffset == 0){//user just pressed move, see if it's valid
			if(!SokoMove()){
			 pstate ^= MOVE;
			 if(pstate & PUSH){pstate ^= PUSH;}
			
			return;
		  }
		 
		}

		//if(poffset == 0 || poffset == 8){TriggerFx(SFX_STEP,SFX_STEP_VOL,false);}//play the foot step sound
	  poffset++;
	  if(gamestate & FASTMOVE){poffset++;}
	  if(poffset > 15){//we moved a whole square so do whatever needs to be done
		 poffset = 0;
		 pstate ^= MOVE;

/*
			//Add the move to the undo list
			if(movepos != 255){//is there enough space left in the list 256 moves 64*4
			 if(movepos == nummoves){nummoves++;movepos++;}//make sure there are no moves after this that would be overwritten
			  u8 ts;
			if(pstate & UP)		 {ts=0;}
			else if(pstate & DOWN){ts=1;}
			else if(pstate & LEFT){ts=2;}
			else						{ts=3;}
				
			ts = movelist[movepos/4];
			ts +=  ts<<((movepos%4)*2);//shift 2 bits into position
			
			movelist[movepos/4] = ts;//store 2 bits
			 movepos++;
			printb(movepos,0,ts<<(movepos%4));
			printb(20,0,movepos);
			if(gamestate & FASTMOVE)
				poffset++;
		 }
*/

		 if(pstate & UP)		 {py--;}
		 else if(pstate & DOWN){py++;}
		 else if(pstate & LEFT){px--;}
		 else/*pstate & RIGHT*/{px++;}
			
		 moves++;
		 if(bx != 255){//a block was active during this move
			 pushes++;
			if(GetMapTile(bnx,bny)==TARGET){
				SetMapTile(bnx,bny,TBLOCK);//put the block over target in vram
				if(!(gamestate & SCANNINGOPTIMUM)){TriggerFx(SFX_TARGET,SFX_TARGET_VOL,false);}//play the block place sound
				opentargets--;//one less target left
			}
			else
				SetMapTile(bnx,bny,BLOCK);

				//SetMapTile(bnx,bny,bx=(BLOCK+(GetMapTile(bnx,bny)==TARGET)));
			 //if(bx==TBLOCK){opentargets--;}
		 }
		 bx = 255;//deactivate the block
	  }
	}
	else//pstate & MOVE
		if(pstate & PUSH){pstate ^= PUSH;}

}

bool SokoMove(){
	u8 t,t2;//t = block player moves to, t2 = block 1 past t(where a pushed block would go)
	u8 x,x2;//same thing for x values
	u8 y,y2;//ditto
	x = x2 = px;//set defaults
	y = y2 = py;

	if(pstate & UP){
		if(py == 0){return false;}//don't let player move off the screen
	  t = GetMapTile(px,py-1);
		if(py-1 == 0){t2 = FLOOR;}//don't check outside of map just let the move happen
		else			{t2 = GetMapTile(px,py-2);}
		if(py-1 == 0 && IsBlock(t)){return false;}//don't let player push a block off the map
	  y  = py-1;
	  y2 = py-2;
	}
	else if(pstate & DOWN){
		if(py == 13){return false;}//don't let player move off the screen
	  t = GetMapTile(px,py+1);
		if(py+1 == 27){t2 = FLOOR;}//don't check outside of map just let the move happen
		else			 {t2 = GetMapTile(px,py+2);}
		if(py+1 == LEVELHIGH-1 && IsBlock(t)){return false;}//don't let player push a block off the map
	  y  = py+1;
	  y2 = py+2;
	}
	else if(pstate & LEFT){
		if(px == 0){return false;}//don't let player move off the screen
	  t = GetMapTile(px-1,py);
		if(px-1 == 0){t2 = FLOOR;}//don't check outside of map just let the move happen
		else			{t2 = GetMapTile(px-2,py);}
		if(px-1 == 0 && IsBlock(t)){return false;}//don't let player push a block off the map
	  x  = px-1;
	  x2 = px-2;
	}
	else/*if(pstate & RIGHT)*/{
		if(px == 14){return false;}//don't let player move off the screen
	  t = GetMapTile(px+1,py);
		if(px+1 == 29){t2 = FLOOR;}//don't check outside of map just let the move happen
		else			 {t2 = GetMapTile(px+2,py);}
		if(px+1 == LEVELWIDE-1 && IsBlock(t)){return false;}//don't let player push a block off the map
	  x  = px+1;
	  x2 = px+2;
	}

	//ok, we know what the player is moving into and what is beyond that(if relevant). piece of cake now.
	if(IsBlock(t)){//we're pushing a block or a block on a target
		if(IsSolid(t2))//is something solid behind it?
		  return false;
		//if we're here then the move will suceed, check special case block on a target

	  if(IsTBlock(t)){SetMapTile(x,y,TARGET);opentargets++;}//draw the target underneath and update number of targets uncovered
	  else			  {SetMapTile(x,y,FLOOR);}//draw the floor underneath
	  bx = x;//set up the block for rendering
	  by = y;
	  bnx = x2;
	  bny = y2;
	  pstate |= PUSH;
	}
	else if(IsSolid(t)){
		if(pstate & PUSH){pstate ^= PUSH;}
		return false;
	}
	return true;
}

u8 GetMapTile(u8 x, u8 y){
	u8 t = vram[((y<<1)*30)+(x<<1)]-RAM_TILES_COUNT;
	t = t>>2;//adjust it for tile size
	t -= tileset*NUMTILES;//adjust for tile set
	return t;	
}

void RetryLevel(){
	//DoScreenOutEffect();
	FadeOut(3,true);
	WaitVsync(1);
	LoadLevel(true,true,false,false,false,true);
	WaitVsync(1);
	Render();//make sure player gets drawn once so he doesn't "warp" to place
	FadeIn(3,false);
}


/*
void Redo(){
	if(movepos >= nummoves){return;}//nothing to redo	

	u8 ts = (( movelist[movepos/4]>>((movepos%4)*2) ) & 3);//get 2 bits from the current move list position

	if(ts == 0)		{pstate = UP|MOVE;}
	else if(ts == 1) {pstate = DOWN|MOVE;}
	else if(ts == 2) {pstate = LEFT|MOVE;}
	else				 {pstate = RIGHT|MOVE;}

	poffset = 0;	Logic();//let logic setup any blocks
	poffset = 16;  Logic();//let logic finish a move
	//movepos++;// done in logic
}


void Undo(){//do this the easy way
	if(nummoves == 0 || nummoves == 254){return;}//nothing to undo or out of space
	u8 nm = nummoves-1;
	u8 count = 0;
	LoadLevel(false,false,false,false,false);//reload level, don't clear move list
	//movepos = 0;//already done in load level

	while(movepos < nm){//just step back through
		//if(++count > 20){WaitVsync(1);count = 0;}//give the kernel some time...

	  Redo();
	}
	WaitVsync(1);
}
*/

bool IsLevelCompleted(u8 map){
	struct EepromBlockStruct ebs;
	ebs.id = SOKOBAN_WORLD_ID;

	if(EepromReadBlock(ebs.id, &ebs) == 0)
		return (ebs.data[(map/8)+EPISODE_SAVE_OFFSET]>>(map%8))&1;//decode 1 bit from eeprom
	else{
		for(u8 i=0;i<30;i++)
		  ebs.data[i] = 0;//make sure the data here is formatted

	  EepromWriteBlock(&ebs);//make a new one then
	}
	return false;

}

void SetLevelCompleted(u8 map){

	struct EepromBlockStruct ebs;
	ebs.id = SOKOBAN_WORLD_ID;

	if(EepromReadBlock(ebs.id, &ebs) != 0)//no save game
		EepromWriteBlock(&ebs);//make a new one then

		u8 t = (u8)(ebs.data[(map/8)+EPISODE_SAVE_OFFSET]);		
		
	  t |= (1<<(map%8));
	  
	  ebs.data[(map/8)+EPISODE_SAVE_OFFSET] = t;
	  
		EepromWriteBlock(&ebs);
	
}


void SetMetLevelPar(u8 map){
	//To save some code space, well simply store this like level complete progress.
	//We will offset the data by 56 levels(7 bytes) from the start of level completed progress.
	//Maximum 56 levels per pack, can store save progress for 2 episodes in one block
	SetLevelCompleted(map+56);
}

bool HasMetLevelPar(u8 map){//Same concept as above
	return IsLevelCompleted(map+56);
}

bool AllLevelsCompleted(){
	for(u8 i=0;i<NUMLEVELS;i++){
		if(!IsLevelCompleted(i))
		  return false;
	}
	return true;
}


void StartMusicTrack(u8 track){
	
	if(track == 1)	  {StartSong(AmericaSong);}
	else if(track == 2){StartSong(EgyptSong);}
	else if(track == 3){StartSong(ChinaSong);}
	else if(track == 4){StartSong(SouthPoleSong);}
	else if(track == 5){StartSong(JapanSong);}
	
}

