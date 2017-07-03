bool StartDown(u8 i) {return(padstate[i] & BTN_START)  && !(oldpadstate[i] & BTN_START);}
bool UpDown(u8 i)    {return(padstate[i] & BTN_UP)     && !(oldpadstate[i] & BTN_UP);   }
bool DownDown(u8 i)  {return(padstate[i] & BTN_DOWN)   && !(oldpadstate[i] & BTN_DOWN); }
bool LeftDown(u8 i)  {return(padstate[i] & BTN_LEFT)   && !(oldpadstate[i] & BTN_LEFT); }
bool RightDown(u8 i) {return(padstate[i] & BTN_RIGHT)  && !(oldpadstate[i] & BTN_RIGHT); }
bool ADown(u8 i) {return(padstate[i] & BTN_A)  && !(oldpadstate[i] & BTN_A); }
bool BDown(u8 i) {return(padstate[i] & BTN_B)  && !(oldpadstate[i] & BTN_B); }

void FillScreen(u8 t){
   for(int i=0;i<VRAM_TILES_H*VRAM_TILES_V;i++)
      vram[i] = t+RAM_TILES_COUNT;
}

/*void GuiFillScreen(){
   for(u8 y=0;y<VRAM_TILES_V;y+=2)
   for(u8 x=0;x<VRAM_TILES_H;x+=2){
      SetMapTile(x  ,y  ,0);
	  if(x<14){SetMapTile(x+1,y  ,1);}
	  if(y<11){SetMapTile(x  ,y+1,2);}
      if(x<14 && y<11){SetMapTile(x+1,y+1,3);}
   }
}
*/
void BlankCursor(u8 x, u8 y, u8 w, u8 h){
   for(u8 j=0;j<h;j++)
   for(u8 i=0;i<w;i++)
      SetTile(x+i,y+j,BLANKT);
}

void DrawCursor(u8 x, u8 y){
   SetMapTile(x,y,CURSORSTART);
}

void DrawMenu(u8 x, u8 y, u8 w, u8 h){
   SetTile(x+0,y+0,MENUSTART+0);//draw the corners
   SetTile(x+w,y+0,MENUSTART+1);
   

   SetTile(x+0,y+h,MENUSTART+3);
   SetTile(x+w,y+h,MENUSTART+2);
 
   bool flop = true;
   for(u8 i=x+1;i<x+w;i++){//draw top and bottom
      SetTile(i,y,MENUSTART+5-flop);SetTile(i,y+h,MENUSTART+4+flop);
      flop = !flop;
   }
   
   flop = true;
   for(u8 i=y+1;i<y+h;i++){
      SetTile(x,i,MENUSTART+7-flop);SetTile(x+w,i,MENUSTART+6+flop);
      flop = !flop;
   }

   for(u8 i=y+1;i<y+h;i++)
   for(u8 j=x+1;j<x+w;j++)
      SetTile(j,i,BLANKT);

   
}

void DrawOpeningMenu(u8 x, u8 y, u8 w, u8 h, int speed){
   u8 mt;
   u8 count;
   mt = 1;
   count = speed;
   while(mt<w+1){//open horizontally
      DrawMenu(x,y,mt,1);
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
	  if(speed < 0){
         WaitVsync(abs(speed));
	  }
	  else{
	     if(!count){WaitVsync(1);count=speed;}else{count--;}
	  }
	  mt++;
   }
}


void Intro(){




return;
   FadeOut(1,true);
   RamifyFont(16,31);
   FadeIn(8,false);

   SetTileTable(GuiTiles);
   FillScreen(0);

   WaitVsync(1);
   
   for(u8 i=0;i<64;i++){
      ram_tiles[i] = 0;
      ram_tiles[i+(10*64)] = ram_tiles[i+(5*64)];//copy B 
   }

   for(u8 i=29;i>11;i--){
      FillScreen(0);
	  CompPrint(i,10,strUzeboxOffset);//not enough ram tiles to get Z, see strings.inc
      WaitVsync(4);
   }
   WaitVsync(10);
   u8 f;
   for(u8 i=0;i<(7*8)+2;i+=2){
      spritecount = 0;
	  if(i == (7*8))
	     f = 8;
      else
	     f = VICTORYSTARTFRAME+1;
	  DrawSpriteTile(14*8,i,f,0,0,0);
	  WaitVsync(1);
   }



   WaitVsync(120);
   FadeOut(4,true);
   WaitVsync(60);
   FadeIn(2,false);

}

void MainMenu(){
   u8 cursorpos = 0;

   SetTileTable(GuiTiles);
   HideSprites(0,MAX_SPRITES);
   FillScreen(BLANKT);
   RamifyFont(0,31);
   WaitVsync(1);


   DrawOpeningMenu(9,13,10,6,-3);
   CompPrint(9,10,strMegaBomber);
   CompPrint(12,15,strLocal);
   CompPrint(12,16,strNetwork);
   CompPrint(12,17,strOptions);

   CompPrint(8,22,strVanity);
   //CompPrintNum(17,23,20);
   //CompPrintNum(19,23,11);

   while(true){
      prand();
      FillPad(0);

	  if(StartDown(0) || ADown(0)){
	     WaitVsync(1);
		   gamestate = STARTMATCH;
	     if(cursorpos == 0){//Battle Game
		    guistate = GSETUPMENU;

		 }
		 else if(cursorpos == 1){//Uzebox Live
            guistate = GMAINMENU;
			
		 }
		 else{//option
            guistate = GMAINMENU;//GOPTIONMENU;
		 }
		 FadeOut(1,true);
		 FadeIn(1,false);
		 return;
	  }   
	  else if(UpDown(0)){
	     if(--cursorpos > 254)
		    cursorpos = 2;
	  }else if(DownDown(0)){
	     if(++cursorpos > 2)
		    cursorpos = 0;
	  }  
	  

          
		  BlankCursor(10,15,2,4);
		  DrawCursor(10,15+cursorpos);
		  
	  WaitVsync(1);
   }
}

void OptionMenu(){
   unsigned char cursorpos = 0;

   SetTileTable(GuiTiles);
   HideSprites(0,MAX_SPRITES);
   FillScreen(BLANKT);
   RamifyFont(0,31);
   WaitVsync(1);

   DrawMenu(4,2,21,20);
   CompPrint(9,0,strMegaBomber);
   CompPrint(11,4,strOptions);


   CompPrint(8,7,strTeam);
   CompPrint(8,20,strBack);
   

   while(true){
      FillPad(0);

	  DrawCursor(6,7+cursorpos);
      WaitVsync(1);
   }
}

void SetupMenu(){
   unsigned char cursorpos = 0;
   u8 ptype[4];
   u8 teams[4] = {0,0,1,1};
   bool useteams = false;
   u8 roundtime = GetRoundTimeBits();
   u8 numrounds = GetNumRoundBits();
   bool useghosts = true;
   bool useblitz = true;

   SetTileTable(GuiTiles);
   HideSprites(0,MAX_SPRITES);
   FillScreen(BLANKT);
   RamifyFont(0,31);
   WaitVsync(1);

   for(u8 i=0;i<4;i++)
      ptype[i] = GetSlot(i);

   DrawMenu(4,2,21,20);
   CompPrint(9,0,strMegaBomber);
   CompPrint(9,4,strBattleSetup);
   CompPrint(8,7,strTeam);
   CompPrint(8,9,strTime);
   CompPrint(8,11,strRound);
   CompPrint(8,13,strGhost);   
   CompPrint(8,15,strBlitz);
   CompPrint(8,17,str1P);
   CompPrint(8,18,str2P);
   CompPrint(8,19,str3P);
   CompPrint(8,20,str4P);

   while(true){
      FillPad(0);
	  if(StartDown(0) || ADown(0)){
	     SetNumRoundBits(numrounds);
		 SetRoundTimeBits(roundtime);
		 if(useteams){gameoptions |= USETEAM;}
		 if(useblitz){gameoptions |= USEBLITZ;}
		 if(useghosts){gameoptions |= USEGHOST;}
		 
         for(u8 i=0;i<4;i++)
            SetSlot(i,ptype[i]);		    

		 FadeOut(1,true);
         FadeIn(1,false);
		 SetTileSet(MapTiles);
		 LoadLevel();
		 guistate =  0;//GMAPSELECTMENU;
		 return;
	  }
	  else if(DownDown(0)){
         cursorpos++;
		 if(cursorpos > 12)//teams must be on
		    cursorpos = 5;
         else if(!useteams && cursorpos > 8)
		    cursorpos = 0;
	  }
	  else if(UpDown(0)){
	     if(--cursorpos == 255){
		    if(useteams)
			   cursorpos = 12;
            else
			   cursorpos = 8;
		 }
	  }
	  else if(LeftDown(0)){
	     if(cursorpos > 4){
            if(cursorpos < 9){
			   if(ptype[cursorpos-5] > 0){ptype[cursorpos-5]--;}
		    }
			else
			   teams[cursorpos-9] = !teams[cursorpos-9];
               
		 }
		 else if(cursorpos == 0){
            useteams = !useteams;

		 }
		 else if(cursorpos == 1){
            if(roundtime > 1)
			   roundtime--;
         }
		 else if(cursorpos == 2){
            if(numrounds > 1)
			   numrounds--;
		 }
		 else if(cursorpos == 3){
            useghosts = !useghosts;
		 }
		 else if(cursorpos == 4){
            useblitz = !useblitz;
		 }
	  }
	  else if(RightDown(0)){
	     if(cursorpos > 4){
            if(cursorpos < 9){
			   if(ptype[cursorpos-5] < 2){ptype[cursorpos-5]++;}
		    }
			else
			   teams[cursorpos-9] = !teams[cursorpos-9];
		 }
		 else if(cursorpos == 0){
            useteams = !useteams;
		 }
		 else if(cursorpos == 1){
            if(roundtime < 5)
			   roundtime++;
         }
		 else if(cursorpos == 2){
            if(numrounds < 3)
			   numrounds++;
		 }
		 else if(cursorpos == 3){
            useghosts = !useghosts;
		 }
		 else if(cursorpos == 4){
            useblitz = !useblitz;
		 }
	  }
	  else if(BDown(0) || StartDown(0)){
         guistate = GMAINMENU;
		 for(u8 i=0;i<MAX_PLAYERS;i++){
			   botgoal[i] = (ptype[i] == 1);

		 }
		 WaitVsync(1);
		 FadeOut(1,true);
		 SetTileTable(MapTiles);
		 FadeIn(1,false);
		 return;
	  }
      
      
	  if(useteams){CompPrint(17,7,strOn);}else{CompPrint(17,7,strOff);}
	  if(useghosts){CompPrint(17,13,strOn);}else{CompPrint(17,13,strOff);}
	  if(useblitz){CompPrint(17,15,strOn);}else{CompPrint(17,15,strOff);}
	  CompPrintNum(15,9,roundtime);
	  CompPrintNum(15,11,numrounds);

      
      

	  for(u8 i=0;i<4;i++){//draw man,cpu,off
         if(ptype[i] == 0)
		    CompPrint(11,17+i,strMan);
         else if(ptype[i] == 1)
		    CompPrint(11,17+i,strCpu);
         else
		    CompPrint(11,17+i,strOff);

         if(useteams){
			CompPrint(17,17+i,strTeam);
            CompPrintNum(20,17+i,(teams[i]+1));
		 }
		 else
		    CompPrint(17,17+i,PSTR("        "));
	  }
      
	  BlankCursor(6,7,2,15);
	  BlankCursor(15,17,2,5);
      if(cursorpos < 5)
	     DrawCursor(6,7+(cursorpos*2));
      else if(cursorpos < 9)
	     DrawCursor(6,12+(cursorpos));
      else
	     DrawCursor(15,8+(cursorpos));

	  DoHideSprites();
	  prand();
      WaitVsync(1);
   }
}

void MapSelectMenu(){

}

void UzeNetMenu(){/*
   unsigned char cursorpos = 0;
//   int pings[8];
   char snames[16];
   u8 numservers = 5;
//   u8 page;


   SetTileTable(GuiTiles);
   HideSprites(0,MAX_SPRITES);
   //FillScreen();

   //DrawOpeningMenu(4,3,21,18,0);   
   DrawMap2(0,0,GuiMap);
   spritecount=0;
   DoHideSprites();
   FadeOut(1,true);
   SetupCrowd();
   DoHideSprites();
   AnimateCrowd();
   FadeIn(1,false);
   
   WaitVsync(1);
   DrawMenu(2,2,24,20);
   

   print(9,4,PSTR("PLEASE WAIT"));
   if(!UzeNetConnect()){//try to connect with PC host
      print(9,4,PSTR("LINK FAILED"));
	  WaitVsync(150);
	  guistate = GMAINMENU;
	  FadeOut(1,true);
	  FadeIn(1,false);
	  return;
   }
   else
      print(7,4,PSTR("RETRIEVING LIST"));

   if(!UzeNetGetList()){
      print(7,4,PSTR("GET LIST FAILED"));
      print(7,4,PSTR("CHECK CONNECTION"));
   }
   else
      print(7,4,PSTR("UZENET    GAMES"));
	  printb(14,4,numservers);
   
   UzeNetServerNames(snames,0,1);

      printram(7,8,snames);

   print(5,6,PSTR("NAME"));
   print(11,6,PSTR("PLAYERS"));
   print(21,6,PSTR("PING"));
   print(5,19,PSTR("HOST"));
   print(5,20,PSTR("BACK"));

   

   while(true){
      FillPad(0);
	  if(StartDown(0)){
         if(cursorpos == 10){//host
            guistate = GLOBBYMENU;
			return;
		 }
		 else if(cursorpos == 11){//back
		    FadeOut(1,true);
			FadeIn(1,false);
		    guistate = GMAINMENU;
			return;
		 }
	  }
	  else if(DownDown(0) && cursorpos < 11){cursorpos++;}
	  else if(UpDown(0) && cursorpos > 0){cursorpos--;}


     
      
	  BlankCursor(3,7,2,15);
	  DrawCursor(3,8+cursorpos+(cursorpos>9));


      

	  DoHideSprites();
      WaitVsync(1);
   }
*/
}

void UzeNetLobby(){
//   unsigned char cursorpos = 0;

/*

   SetTileTable(GuiTiles);
   HideSprites(0,MAX_SPRITES);
   //FillScreen();

   //DrawOpeningMenu(4,3,21,18,0);   
   DrawMap2(0,0,GuiMap);
   spritecount=0;
   DoHideSprites();
   FadeOut(1,true);
   SetupCrowd();
   DoHideSprites();
   AnimateCrowd();
   FadeIn(1,false);
   
   WaitVsync(1);
   DrawMenu(1,1,27,23);
   print(11,2,PSTR("GAME LOBBY"));
   

   while(true){
      FillPad(0);
	  if(StartDown(0)){
         if(cursorpos == 10){//host
            guistate = GLOBBYMENU;
			return;
		 }
		 else if(cursorpos == 11){//back
		    FadeOut(1,true);
			FadeIn(1,false);
		    guistate = GMAINMENU;
			return;
		 }
	  }
	  else if(DownDown(0) && cursorpos < 11){cursorpos++;}
	  else if(UpDown(0) && cursorpos > 0){cursorpos--;}


     
      
	  BlankCursor(3,7,2,15);
	  DrawCursor(3,8+cursorpos+(cursorpos>9));


      

	  DoHideSprites();
      WaitVsync(1);
   }
   */
}

void LiveMenu(){
 /*  unsigned char cursorpos = 9;

   player_ftime[0] = player_frame[0] = 1;

   SetTileTable(GuiTiles);
   HideSprites(0,MAX_SPRITES);
   FillScreen();
   DrawOpeningMenu(4,2,21,21,1);   

   print(10,3,strUzeboxLive);
   print(8,22,strGettingList);
   print(20,5,strPing);
   print(6,18,strBack);
   print(6,19,strRefresh);

   for(u8 i=0;i<8;i++){
      print(6,i+6,strEmpty);
   }

   WaitVsync(110);
   print(8,22,strNoGames);
   
   while(true){
      FillPad(0);
	  if(StartDown(0)){
	     if(cursorpos == 8){
	        guistate = GMAINMENU;
		    return;
		 }
		 else{//refresh
            return;
		 }

	  }
	  else if(UpDown(0) && cursorpos)
	     cursorpos--;
	  else if(DownDown(0) && cursorpos < 9)
	     cursorpos++;   
   
      for(u8 i=0;i<8;i++){SetTile(4,6+i,MENUSTART+2);}
	  if(cursorpos < 8){
	     SetTile(4,6+cursorpos,MENUSTART+4);
	     SetTile(4,18,MENUSTART+2);
	     SetTile(4,19,MENUSTART+2);
      }
	  else{
	     SetTile(4,18+(cursorpos>8),MENUSTART+4);
	     SetTile(4,19-(cursorpos<9),MENUSTART+4);
	  }
	  
      WaitVsync(1);
   }
   */
}

void InGameMenu(){//This is running with the gamefield drawn...
//Can't draw the menu because we can't change tilesets, instead create a black ramtile and blot the area out

   u8 cursorpos = 0;
   blip = 2;
   int restorepos = 64*29;
   HideSprites(0,MAX_SPRITES);
   WaitVsync(1);//get rid of ram tile indexes
   
   RamifyFont(0,31);
   WaitVsync(1);

   for(u8 i=0;i<64;i++)
      ram_tiles[i] = 0;//make a black tile for us
   	  
   for(u8 y=11;y<15;y++)
   for(u8 x=12;x<18;x++){
      ram_tiles[restorepos++] = vram[x+(y*30)];//save which tiles are underneath
      vram[x+(y*30)] = 0;//our black ram tile(replaced 0 in the font)
   }   
   WaitVsync(1);

   
   while(true){
      FillPad(0);
	  if(StartDown(0)){
	     restorepos = 64*29;
         WaitVsync(1);
         for(u8 y=11;y<15;y++)
         for(u8 x=12;x<18;x++)
            vram[x+(y*30)] = ram_tiles[restorepos++];//restore the tiles underneath
		 
		 if(cursorpos)
		    guistate = GMAINMENU;  
		
		 return;
	  }
	  else if(UpDown(0) || DownDown(0))
	     cursorpos = !cursorpos;   

      blip=!blip;
	  CompPrint(13,12,strBack);
	  CompPrint(13,13,strQuit);

	  if(!blip){//flash the selected item
		 for(u8 x=12;x<18;x++)
		    vram[((12+cursorpos)*30)+x] = 0;//our black ram tile

	  }

	  WaitVsync(3);
	  }
   
}

void VictoryMenu(){
   
   u8 flash = 0;

   DrawMap2(0,0,GuiMap);
   SetTileTable(GuiTiles);

//   SetupCrowd();
   
   HideSprites(0,MAX_SPRITES);


//   u8 victor = CalculateVictor();

   while(true){
      FillPad(0);
      DrawMenu(5,1,20,18);

	  if(StartDown(0) && fadeStep == FADER_STEPS+1){ 
         FadeOut(6,false);
	  }
            
   //   AnimateCrowd();
	  spritecount = 0;
      if(++player_ftime[0] > 7){
	     player_ftime[0] = 0;
         if(++player_frame[0] > 7)
		    player_frame[0] = 0;
  	  }

	  for(u8 i=0;i<4;i++){
         DrawSpriteTile(56,24+(i*32),8+player_frame[0],0,180-(i*60),0);
	  }

      for(u8 y=0;y<4;y++){
         for(u8 x=0;x<GetScore(y);x++){
            if(flash < 30 && ((x == GetScore(y)-1) || GetScore(y) == GetNumRounds())){
			   print(13,4+(y*4),PSTR("WINNER"));
			   continue;
			}

			DrawMap2(11+(x*4),3+(y*4),TrophyMap);
         }
	  }

      if(++flash > 60)
	     flash = 0;

	  DoHideSprites();
	  WaitVsync(1);
	  
	  if(fadeStep == 0){//done fading
         FadeIn(6,false);
		 SetTileSet();
		 gamestate = STARTMATCH;
		 return;
	  }
   }
}

void PreGameMenu(){
   int count = 100;
   while(count--){

      WaitVsync(1);
   }
}

void Gui(){
   while(guistate){
   switch(guistate){
   case 0:
   return;
   case GSETUPMENU:
   SetupMenu();
   return;
   case GMAPSELECTMENU:
   MapSelectMenu();
   return;
   case GOPTIONMENU:
   OptionMenu();
   return;
   default:
   MainMenu();
   };
   }
}
