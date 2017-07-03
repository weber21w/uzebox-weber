#include "rendermisc.h"

inline void UpdateBorder();



bool PutSprite(u8 x, u8 y, u8 t, bool flip, u8 pallet){
   if(spritecount >= MAX_SPRITES){return false;}//covers roll over for decrementing count on blink

   sprites[spritecount].x=x; 
   sprites[spritecount].y=y; 
   sprites[spritecount].tileIndex=t;

   if(flip){sprites[spritecount].flags = SPR_FLIP_X;}
   else    {sprites[spritecount].flags = 0;}
   sprites[spritecount].flags |= pallet<<2;
   //if(blink){spritecount++;}else{spritecount--;}
   spritecount++;
   return true;
}

void DoHideSprites(){//hide the unused sprites so they don't stick around
   for(u8 i=spritecount;i<MAX_SPRITES;i++){sprites[i].x=SCREEN_TILES_H*TILE_WIDTH;}
   //if(blink){for(u8 i=spritecount;i<MAX_SPRITES;i++){sprites[i].x =(SCREEN_TILES_H*TILE_WIDTH);}}//sprite count was incrementing
   //else     {for(u8 i=0;i<spritecount;i++)          {sprites[i].x =(SCREEN_TILES_H*TILE_WIDTH);}}//sprite count was decrementing
}

void DrawBombs(){

  //Draw the bombs with sprites if they have an offset(ie they are moving: kicked/thrown)
   char xoff,yoff;
   u8 state;
   xoff = yoff = 0;

   for(u8 i=0;i<MAX_BOMBS;i++){
      if(bomb_owner[i] == 255)
	     continue;

      state = bomb_state[i];

	  if(state & THROWN){
	     u8 dist = abs(bomb_x[i]-bomb_offset[i])+abs(bomb_y[i]-bomb_xtra[i]);
	     u8 bomb_z;

		 //bomb_timer used as midpoint of initial distance from throwing source
		 if(dist > bomb_timer[i])//less than half way, upwards arc
		    bomb_z = abs((bomb_timer[i]*2)-dist);
		 else//greater than half way, downards arc
		    bomb_z = (abs(bomb_timer[i]*2)-dist)-abs(bomb_timer[i]-dist);
			 
         DrawSpriteTile(bomb_x[i], bomb_y[i]-bomb_z, BOMBSTARTFRAME,BOMB_PALLET, 0, 0);
		 continue;  
	  }

	  if(false && bomb_offset[i]){       

		 xoff = yoff = 0;
         
         if(state & UP)        {yoff = -bomb_offset[i];}
	     else if(state & DOWN) {yoff =  bomb_offset[i];} 
         else if(state & LEFT) {xoff = -bomb_offset[i];}
	     else                  {xoff =  bomb_offset[i];}

		 DrawSpriteTile((bomb_x[i]<<3)+xoff,((bomb_y[i]<<3)+yoff)-8,BOMBSTARTFRAME,0,0,0);
	  }
	  else{//draw with tiles
         

	  }
   }

}

inline void DrawBlock(){
   if(block_z == 255)
      return;   
   
   bool smallshadow;
   if(block_z > ((block_y+1)*8))
      smallshadow = false;
   else
      smallshadow = true;

   //if(spritecount >= MAX_SPRITES){
      if(blip)
         spritecount = MAX_SPRITES-4;//sprite overflow, steal last 4 sprites
      else{
         PutSprite(((block_x*16)+16)+smallshadow,(block_y*16)+16,SHADOW_SPRITE,false,0);
		 PutSprite(((block_x*16)+24)-smallshadow,(block_y*16)+16,SHADOW_SPRITE,true,0);
	     return;//draw shadow instead
	  }
   //}
   //use the last 4 sprites
   u8 x = (block_x*16)+16;
   u8 y = block_z;//(block_y*16)+16;
   u8 t = (roundtime%12)/4;

   PutSprite(x+0,y+0,BLOCK_SPRITE+t,false,BLOCK_PALLET);
   PutSprite(x+8,y+0,BLOCK_SPRITE+t,true,BLOCK_PALLET);
   PutSprite(x+0,y+8,BLOCK_SPRITE+t,false,BLOCK_PALLET);
   PutSprite(x+8,y+8,BLOCK_SPRITE+t,true,BLOCK_PALLET);
}

void Draw2bppMap(u8 x, u8 y, const char *Map, const char *Data, const char *Pallet){
   u8 t,t2,w,h;
   int roff = 0;
   int toff = 0;
   int voff = 0;
   int moff = 0;

   w = pgm_read_byte(&(Map[moff++]));
   h = pgm_read_byte(&(Map[moff++]));

   voff = (y*30)+x;//our initial position in vram we will draw the map to

   for(u8 i=y;i<y+h;i++){//draw the ram tile map
      for(u8 j=x;j<x+w;j++){
         t = vram[voff]-RAM_TILES_COUNT;//get the tile underneath
         toff = (t*64);//get offset into tile data
	  
	     for(u8 i=0;i<64;i++)//copy tile pixels to the ram tile
		    ram_tiles[i] = pgm_read_byte(&MapTiles[toff++]);
      
	     vram[voff++] = pgm_read_byte(&(Map[moff++]));//set the vram index according to the map
      }
	  voff += 30-w;
   }
toff = 0;
   for(int i=0;i<(16*23);i++){//Get 2bpp data and blit it to our ram tiles
      t = pgm_read_byte(&Data[toff++]);
      for(u8 j=0;j<4;j++){//decompress 4 pixels from byte
         t2 = (( t>>(j*2) ) & 3);

		 if(t2)
		    ram_tiles[roff] = pgm_read_byte(&Pallet[t2]);

         roff++;
	  }
   }
   
}


inline void Render(){
   spritecount = 0;

   /*if(blip){// && roundtime < 62*SEC && roundtime > 60*SEC){//Draw "HURRY UP!"
      DoHideSprites();      
	  Draw2bppMap(0,0,HurryMap, HurryPixels, HurryPallet);
	  return;
   }
   */
   char xoff,yoff;
   u8 foff=0;
   u8 poff;
   u8 t=0;
   u8 state;
   
   static u8 yorder[4]; u8 Y[4];


  if((gamestate & BLINK)){//distribute some cpu time every other frame for time safety
     //Sort back to front
     for (u8 i = 0; i < numplayers; i++) {
       //if(player_state[i] == 255)//dead and done with animation(totally dead, not drawn)
	      //continue;

       yorder[i] = i;
       Y[i] = (player_y[i]<<3);

	   if(player_state[i] & UP)//adjust for offset if applicable
	      Y[i] -= player_offset[i];
       else if(player_state[i] & DOWN)
	      Y[i] += player_offset[i];
     }
  
  

     u8 p;  

     for (u8 i=0;i<numplayers;i++){
        for (u8 j=i+1;j<numplayers;j++){
           if (Y[j] < Y[i]) {
              p = Y[i];
              Y[i] = Y[j];
              Y[j] = p;
              p = yorder[i];
              yorder[i] = yorder[j];
              yorder[j] = p;
         }
       }
     }

  }


numplayers=4;
   //for(u8 i=0;i<numplayers;i++){yorder[i] = i;}
   
   //Precalculate our ram tile usage, so that we can determine how to best rotate the sprites
//   u8 rtcount = 0;
/*
   for(u8 i=0;i<numplayers;i++){
	  if(player_attrb2[i] & GHOST){
		 if(player_offset[i])
			rtcount += 3;
         else
			rtcount += 2;
         continue;
      }

	  if(player_offset[i]){

         state = player_state[i];
         
		 if(state & LEFT || state & RIGHT){//worst case for 2x3 sprites
            rtcount += 4;
		 }
		 else{
            rtcount += 4;
		 }
	  }
	  else{
         rtcount += 4;
	  }
   }
   
   for(u8 i=0;i<MAX_BOMBS;i++){
      if(bomb_owner[i] < 255 && bomb_offset[i]){//it is being drawn with sprites
         rtcount += 6;//ouch!
	  }
   
   } 
   */  

   if((gamestate & BLINK))//draw bombs first on blink, last on !blink. Give players and moving bombs a fair shair of ram tiles
      DrawBombs();

   for(u8 i=0;i<MAX_PLAYERS;i++){
      xoff = yoff = 0;
	  t = yorder[i];

      if(player_x[t] == 255)//player is being used or drawn by a special object, ignore here
		 continue;

	  state = player_state[t];	  
      
	  if(blip && player_attrb2[t] & 0b01100000)//invulnerable
	     continue;
      if(player_attrb2[t] & GHOST){
	     if(blip || !(gameoptions & USEGHOST) || (roundtime < 60*SEC && (gameoptions & USEBLITZ)))//make ghosts "translucent" and save ram tiles
		    continue;
         
         if(player_x[t] < 12){//left side
			if(player_state[t] & LEFT)
			   yoff = player_offset[t];
			else
			   yoff = -player_offset[t];
            state = pgm_read_byte(&bombframelist[player_frame[t]])*4;
			PutSprite(0,(20*8)-(player_x[t]*16)+8+yoff,(FIRSTBOMBSPRITE+1)+state,false,BOMB_PALLET);
		    PutSprite(0,(20*8)-(player_x[t]*16)+16+yoff,(FIRSTBOMBSPRITE+3)+state,false,BOMB_PALLET);
	      }
          else if(player_x[t] > 26){//right side
			if(player_state[t] & RIGHT)
			   yoff = player_offset[t];
			else
			   yoff = -player_offset[t];
            state = pgm_read_byte(&bombframelist[player_frame[t]])*4;
            PutSprite(29*8,((player_x[t]-27)*16)-8+yoff,(FIRSTBOMBSPRITE+1)+state,true,BOMB_PALLET);
		    PutSprite(29*8,((player_x[t]-27)*16)+0+yoff,(FIRSTBOMBSPRITE+3)+state,true,BOMB_PALLET);
		  }
          else{//top side
			if(player_state[t] & RIGHT)
			   yoff = player_offset[t];
			else
			   yoff = -player_offset[t];
            state = pgm_read_byte(&bombframelist[player_frame[t]])*4;
            PutSprite(((player_x[t]-12)*16)+0+yoff,0,(FIRSTBOMBSPRITE+2)+state,false,BOMB_PALLET);
		    PutSprite(((player_x[t]-12)*16)+8+yoff,0,(FIRSTBOMBSPRITE+3)+state,false,BOMB_PALLET);
		  }
		  continue;

      }
      
	  poff = (t*BOMBER_PALLET_ENTRIES);

	  if((gamestate & VICTORY) && player_offset[i] == 255){//victory and player has aligned with grid, draw jumping animation
         yoff = -player_wait[i];
		 foff = VICTORYSTARTFRAME;

			//Draw shadow
		    PutSprite(((player_x[i]+0)*8)+(player_wait[i]>4)+(player_wait[i]>8),(player_y[i]+1)*8,SHADOW_SPRITE,false,0);
		    PutSprite(((player_x[i]+1)*8)-(player_wait[i]>4)-(player_wait[i]>8),(player_y[i]+1)*8,SHADOW_SPRITE,true,0);

	  }
	  else if(player_ftime[t] >= 4*SEC){//player is in idle animation
         foff = pgm_read_byte(&IdleFrames[player_frame[t]]) - player_frame[t];//adjust for player frame, see below
	    /* if(foff == IDLESTARTFRAME+5){//draw extra hands that don't fit in the frame
	        PutSprite((player_x[t]*8)-4,player_y[t]*8,FIRSTIDLESPRITE+9,false,poff);
	        PutSprite((player_x[t]*8)+20,player_y[t]*8,FIRSTIDLESPRITE+9,true ,poff);
		 }
		 */
	  }       
	  else if(state & UP)   {yoff = -player_offset[t]; foff = 0;}
	  else if(state & DOWN) {yoff =  player_offset[t]; foff = 8;} 
      else if(state & LEFT) {xoff = -player_offset[t]; foff = 16;}
	  else                  {xoff =  player_offset[t]; foff = 24;}

	  if(state == DYING)    {poff = DEATH_PALLET;foff = DEATHSTARTFRAME;}
	  else if(state & STUNNED){foff = STUNNEDSTARTFRAME;}
	  
	  if(blip && (player_attrb[t] & POISON)){poff = POISON_PALLET;}//poison
                                                                     
	  DrawSpriteTile((player_x[t]<<3)+xoff,((player_y[t]<<3)+yoff)-8,player_frame[t]+foff,poff,0,0);


   }//FOR PLAYERS

   if(!(gamestate & BLINK))
      DrawBombs();
   
   DrawBlock();   

   DoHideSprites();
   UpdateBorder();
   DrawTimer();

   if(gamestate & BLINK)
      gamestate ^= BLINK;
   else
      gamestate |= BLINK;

   blip--;
   if(blip == 255)
   
      blip = 2;
}

void MaskRamTile(u8 r, u8 t){
   int roff = r*64;
   int toff = t*64;
//   int moff = (t-SPECIAL_FOREGROUND_START)*8;
//   u8 fg;

   for(u8 y=0;y<8;y++){
//      fg = pgm_read_byte(&ForegroundMask[moff++]);
   for(u8 x=0;x<8;x++){
//      if((fg & (127<<x)))
	     ram_tiles[roff] = pgm_read_byte(&MapTiles[toff]);

	  roff++;
	  toff++;

   }
   }
}

inline void UpdateBorder(){
   //don't repeatedly draw for 1 frame borders, does draw the first time called due to !border.ftime
   if(border.ftime == 255)//single frame border, draw once at level load
      return;
   
   if(--border.ftime == 0){
      if(border.max == 1)
         border.ftime = 255;//make sure it doesn't redraw
      else
         border.ftime = pgm_read_byte(&BorderFrameTimes[border.frame]);
      
	  if(++border.frame >= border.max)
	     border.frame = border.base;
      
	  //Redraw the border
	  u8 t;
      int off = 0;//int off = 0;//(border.frame*BORDER_FRAME_SIZE);      
      int voff;
	  voff = 0;
	  for(u8 x=0;x<ARENA_START_Y*30;x++){//TOP ROW
	  
         t = pgm_read_byte(&BorderFrames[off++]);
	//	 if(t == 255)//allow walk through borders
	//	    voff++;
      //   else
		    vram[voff++] = t+BORDER_START+RAM_TILES_COUNT;
	  }

	  voff = (30*ARENA_START_Y);
	  for(u8 y=0;y<22;y++){//LEFT COLUMNS
         t = pgm_read_byte(&BorderFrames[off++]);
      //   if(t == 255)
	//	    voff++;
      //   else
		    vram[voff++] = t+BORDER_START+RAM_TILES_COUNT;

		 t = pgm_read_byte(&BorderFrames[off++]);
		// if(t != 255)
		    vram[voff] = t+BORDER_START+RAM_TILES_COUNT;

		 voff += 29;
	  }
      
	  voff = (30*ARENA_START_Y)+28;
	  for(u8 y=0;y<22;y++){//RIGHT COLUMNS
         t = pgm_read_byte(&BorderFrames[off++]);
        // if(t == 255)
		  //  voff++;
         //else
		    vram[voff++] = t+BORDER_START+RAM_TILES_COUNT;

		 t = pgm_read_byte(&BorderFrames[off++]);
         //if(t != 255)
		    vram[voff] = t+BORDER_START+RAM_TILES_COUNT;

		 voff += 29;
	  }
	  
   }

}

