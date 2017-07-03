void SetTileSet(){
   SetTileTable(MapTiles);
}

void SetRoundTimeBits(u8 t){//bottom3 bits
   gameoptions |= t;
}

u8 GetRoundTimeBits(){
   return gameoptions & 0b00000111;
}



void SetNumRoundBits(u8 t){
   t <<= 3;
   gameoptions |= t;
}

u8 GetNumRoundBits(){
   return (gameoptions & 0b00011000)>>3;
}

void SetPlayerRange(u8 p, u8 t){
   player_attrb[p] |= (t-3);
}

u8 GetPlayerRange(u8 p){
   if(poison_type == POISON_RANGE && player_attrb[p] & POISON)
      return 1;
   if(gamestate & MAXFIRE)//level has max fire
      return 13;
   return (player_attrb[p] & 0b00000111)+3;
}

void SetScore(u8 p, u8 s){//0-3
   score |= (s<<(p*2));
}

u8 GetScore(u8 p){
   return score & (3>>(p*2));
}

void SetSlot(u8 p, u8 s){//0-2
   playerslots |= (s<<(p*2));
}

u8 GetSlot(u8 p){
   return playerslots & (3>>(p*2));
}

inline u8 player_speed(u8 p){
   if(player_attrb[p] & POISON){
      if(poison_type == POISON_FAST)
	     return 7;
      else if(poison_type == POISON_SLOW)
	     return 0;
   }
   
   return player_attrb2[p] & 0b00000111;
}

void SetNumRounds(u8 t){

}

u8 GetNumRounds(){
   return 3;
}

u8 FindBomb(u8 x, u8 y, u8 t){//find a bomb, given a tile fragment of it. used in MovePlayer()
   t -= BOMB_START;
   t %= 4;

   if(t == 3){//bottom right corner
      x--;
	  y--;
   }
   else if(t == 2)//bottom left corner
      y--;
   else if(t == 1)//top right corner
      x--;

   for(u8 i=0;i<MAX_BOMBS;i++){
      if(bomb_owner[i] == 255)
	     continue;
      
	  if(bomb_x[i] == x && bomb_y[i] == y)
	     return i;//found it
   }

   return 255;
}

inline u8 BombPiece(u8 t){
   t -= BOMB_START;
   t %= 3;
   return t;
}

void ZeroState(){
   for(u8 i=0;i<MAX_PLAYERS;i++){
      player_x[i] = 
	  player_y[i] =
	  player_wait[i] =
	  player_offset[i] =
	  player_state[i] = 
	  player_attrb[i] =
	  player_attrb2[i] =
	  player_frame[i] =
	  player_ftime[i] =
	  0;

   player_attrb2[i] = 0b00000001;//1 speed
   }

   for(u8 i=0;i<MAX_BOMBS;i++){
      bomb_x[i] =
	  bomb_y[i] =
	  bomb_offset[i] = 
	  bomb_state[i] =
	  bomb_timer[i] =
	  bomb_xtra[i] =
	  0;

	  bomb_owner[i] = 255;
   }
}

void LoadLevel(){
   WaitVsync(1);
   ZeroState();

   for(u8 y=1;y<VRAM_TILES_V-1;y+=2)//floor fill
   for(u8 x=2;x<VRAM_TILES_H;x+=2)
      SetMapTile(x,y,FLOOR_START);

   //StartSong(StageSong);
   u8 t;
   u8 playercount=0;
   roundtime = (GetRoundTimeBits())*(60*SEC);
   
   numplayers = 3;
   botgoal[1] = botgoal[2] = botgoal[3] = 255;
   
   block_z = 255;
   block_time = 0;
   if(prand() > 128){//start in upper left
      block_dir = UP|MOVE;
	  block_x = 0;
	  block_y = 1;
	  block_minx = 0;
	  block_miny = 0;
      block_maxx = 12;
      block_maxy = 10;
   }
   else{//start in lower right
      block_dir = DOWN|MOVE;
	  block_x = 12;
	  block_y = 9;//10
	  block_minx = 0;
	  block_miny = 0;
	  block_maxx = 12;
	  block_maxy = 10;
   }


   int moff = 0;//13*11*level;
   //Draw the actual level data.
   for(u8 y=0;y<11;y++)
   for(u8 x=0;x<13;x++){
      t = pgm_read_byte(&GameMaps[moff++]);
	  
      if(t == PLAYERSTART){
	     if(playercount <= numplayers){
            player_x[playercount] = (x*2)+ARENA_START_X;
		    player_y[playercount] = (y*2)+ARENA_START_Y;
			player_state[playercount] = DOWN;
		    playercount++;
		 }
		 t = FLOOR_START;
	  }
//if(t != WALL_START)
//t = FLOOR_START;
	  SetMapTile((x*2)+ARENA_START_X,(y*2)+ARENA_START_Y,t);

   }


   moff = 0;//(lvl*BORDER_DESCRIPTOR_SIZE);
   border.base = pgm_read_byte(&BorderTable[moff++]);
   border.max = pgm_read_byte(&BorderTable[moff++]);

   border.frame = border.max;//let it draw once on the first frame
   border.ftime = 1;

   for(u8 i=0;i<MAX_BOMBS;i++){
      bomb_owner[i] = 255;
      bomb_offset[i] = 0;
	  bomb_state[i] = 0;
   }

   WaitVsync(1);

   //Now randomly open up 24 of the blocked spaces. This gives us 70 blocks in accordance with Super Bomberman 5
   u8 rx,ry,count=0;
   for(u8 i=0;i<24;i++){
//continue;
      while(GetMapTile((rx = (((prand()%13)*2)+ARENA_START_X)),(ry = (((prand()%11)*2)+ARENA_START_Y))) != BLOCK_START ){
         if(++count > 254){
		    count = 0;
			WaitVsync(1);
			//for(u8 i=0;i<prand();i++);
	     }
	  }      
	  
	  SetMapTile(rx,ry,0);
	//  WaitVsync(1);
   }


   for(u8 i=0;i<MAX_PLAYERS;i++){
      SetPlayerRange(i,3);
	  if(i > 1 && gameoptions & USETEAM)
	     player_state[i] |= TEAM2;
   }
   
}



u8 player_max_bombs(u8 p){
   return 4;
}

u8 player_bombs_out(u8 p){
   u8 b = 0;
   for(u8 i=0;i<MAX_BOMBS;i++)
      if(bomb_owner[i] == p)
	     b++;

   return b;
}

bool IsSolid(u8 t){
   if(t < 4)
      return false;

   if((t >= BLOCK_START && t <= WALL_END) || (t >= BOMB_START && t <= BOMB_END) || (t >= SPECIAL_SOLID_START && t <= SPECIAL_SOLID_END) || (t >= BORDER_START))
      return true;
   
   return false;
}

bool IsSolidEx(u8 x, u8 y){//extended to check against players and moving bombs, not just tiles
   if(IsSolid(GetMapTile(x,y)))
      return true;

   for(u8 i=0;i<MAX_PLAYERS;i++){
      if(player_attrb2[i] & GHOST)
	     continue;
      
	  //...TODO
   }
     
   return false;
}

inline bool IsSolidForFire(u8 t){
   if(t < 4)
      return false;

   if((t >= BLOCK_START && t <= WALL_END) || (t >= SPECIAL_SOLID_START && t <= SPECIAL_SOLID_END) || (t >= BORDER_START))
      return true;
   
   return false;
}



bool IsSemiSolid(u8 t){
   if(t > 3 && t < 8)
      return true;

   return false;
}

bool IsFire(u8 t){
   if(t >= FIRE_START && t <= FIRE_END)
      return true;
   return false;
}

bool IsHorizontalFire(u8 t){
   if(t < FIRE_START || t > FIRE_END)
      return false;
   t -= FIRE_START;
   t %= 28;
   if(t > 3 && t < 8)
      return true;
   if(t > 19 && t < 28)
      return true;
   return false;
}

bool IsVerticalFire(u8 t){
   if(t < FIRE_START || t > FIRE_END)
      return false;
   t -= FIRE_START;
   t %= 28;
   if(t < 4)
      return true;
   if(t > 11 && t < 20)
      return true;
   return false;
}

bool IsCenterFire(u8 t){
   if(t < FIRE_START || t > FIRE_END)
      return false;
   t -= FIRE_START;
   t %= 28;
   if(t > 7 && t < 12)
      return true;
   
   return false;
}

bool IsTipFire(u8 t){
   if(t < FIRE_START || t > FIRE_END)
      return false;
   t -= FIRE_START;
   t %= 28;
   if(t > 11)// && t < 28
      return true;
   return false;
}

bool IsBomb(u8 t){
   if(t >= BOMB_START && t <= BOMB_END)
      return true;
   return false;
}

bool IsBombTop(u8 t){
   if(t < BOMB_START || t > BOMB_END)
      return false;
   t -= BOMB_START;
   t %= 4;
   if(t < 2)
      return true;
   return false;
}

bool IsBombBottom(u8 t){
   if(t < BOMB_START || t > BOMB_END)
      return false;
   t -= BOMB_START;
   t %= 4;
   if(t > 1)
      return true;
   return false;
}

bool IsBombLeftSide(u8 t){
   if(t < BOMB_START || t > BOMB_END)
      return false;
   t -= BOMB_START;
   t %= 4;
   if(t == 0 || t == 2)
      return true;
   return false;
}

bool IsBombRightSide(u8 t){
   if(t < BOMB_START || t > BOMB_END)
      return false;
   t -= BOMB_START;
   t %= 4;
   if(t == 1 || t == 3)
      return true;
   return false;
}

inline bool IsBlock(u8 t){
   if(t >= BLOCK_START && t <= BLOCK_END)
      return true;
   return false;
}

void RestoreMapTile(u8 x, u8 y){
   SetMapTile(x,y,0);
}

inline bool IsPowerUp(u8 t){
   if(t >= POWERUP_START && t <= POWERUP_END)
      return true;
   return false;
}


void DestroyBlock(u8 x, u8 y){
   u8 t = prand();
   if(t < POWERUPCHANCE)
      SetMapTile(x,y,0);
   else if(t < FIRECHANCE)
      SetMapTile(x,y,POWERUP_START+4);
   else if(t < SPEEDCHANCE)
      SetMapTile(x,y,POWERUP_START+0);
   else if(t < BOMBCHANCE)
      SetMapTile(x,y,POWERUP_START+8);
   else if(t < POISONCHANCE)
      SetMapTile(x,y,POWERUP_START+12);
   else// if(prand() > 240)//weird randomness...// if(t < KICKCHANCE)
      SetMapTile(x,y,POWERUP_START+12);
/*   else{//THROW, we don't like to hand these out...we wont if there are 4 players alive(ram tile concerns)
	  for(u8 i=0;i<MAX_PLAYERS;i++){
         if(player_state[i] == DYING || player_attrb2[i] & GHOST){
	        SetMapTile(x,y,POWERUP_START+12);
		    return;
		 }
      }
   }
   */
}


const char bombframelist[] PROGMEM = {2,1,0,1,};
const char fireframelist[] PROGMEM = {0,1,2,1,0};
const char blockframelist[] PROGMEM = {0,0,1,2,3,4,};

inline u8 bomb_frame(u8 b){
   if(bomb_timer[b] > EXPLODETIME)
      return pgm_read_byte( &bombframelist[(((BOMBSTARTTIME-bomb_timer[b])/10)&3)] );//WAS 20
   else
      return pgm_read_byte( &blockframelist[(((EXPLODETIME  -bomb_timer[b])/5 ))] );
}

inline u8 fire_frame(u8 b){
   return pgm_read_byte(&FireSequence[bomb_timer[b]/5]);
}

inline u8 FindOpenBomb(){

   for(u8 i=MAX_BOMBS-1;i<255;i--){
      if(bomb_owner[i] == 255)//bomb is open
         return i;
   }

   return 255;
}
/*
inline void RestoreMapTile(u8 x, u8 y){
   SetMapTile(x,y,GetMapTileRam(x,y));
}
*/

bool PlacePowerUp(u8 x, u8 y, u8 p){//returns false if position was blocked
   if(GetMapTile(x,y) > 3)// || IsSolid(GetMapTile(x+1,y+1)))
      return false;
   
   SetMapTile(x,y,p);
   return true;
}

void DispersePowerups(u8 p){//take all of a killed players items and disperse them randomly around the map
   u8 i,t;
   //placing the powerups would be potentially slow or infinite. give up after 2 blocked attempts
   if((t = player_speed(p)))
      t--;
   for(i=0;i<t;i++){
      if(!PlacePowerUp(((prand()%11)<<1)+ARENA_START_X,((prand()%11)<<1)+ARENA_START_Y,POWERUP_START+0))   
         PlacePowerUp(((prand()%11)<<1)+ARENA_START_X,((prand()%11)<<1)+ARENA_START_Y,POWERUP_START+0);  
   }

   t = GetPlayerRange(p)-3;
   for(i=0;i<t;i++){
      if(!PlacePowerUp(((prand()%13)<<1)+ARENA_START_X,((prand()%11)<<1)+ARENA_START_Y,POWERUP_START+4))
         PlacePowerUp(((prand()%13)<<1)+ARENA_START_X,((prand()%11)<<1)+ARENA_START_Y,POWERUP_START+4);
   }

   t = player_max_bombs(p)-1;
   for(i=0;i<t;i++){
      if(!PlacePowerUp(((prand()%11)<<1)+ARENA_START_X,((prand()%11)<<1)+ARENA_START_Y,POWERUP_START+8))   
         PlacePowerUp(((prand()%11)<<1)+ARENA_START_X,((prand()%11)<<1)+ARENA_START_Y,POWERUP_START+8);  
   }
}


void KillPlayer(u8 p){
   TriggerFx(PLAYER_DIE_FX,255,false);
   player_state[p] = DYING;
   player_offset[p] = 0;
   player_frame[p] = 0;
   player_ftime[p] = 14;
   player_wait[p] = 255;//set killer to NULL, FindKiller() will set it if applicable

   if(player_attrb[p] & 0b0010000)//keep team bit
      player_attrb[p] = 0b0010000;
   else
      player_attrb[p] = 0;

   player_attrb2[p] = 0;
}

inline u8 FindKiller(u8 p){//for this player killed, determine who's bomb it was so he get's credit in modes that this matters
   return 0;   

//   u8 range;
   u8 dist;
   u8 closestdist = 255;
   u8 closest = 255;
   for(u8 i=0;i<MAX_BOMBS;i++){
      if(bomb_owner[i] == 255 || bomb_timer[i] >= EXPLODETIME)
	     continue;
      if(abs(player_x[i]-bomb_x[i]) < 2 && abs(player_y[i]-bomb_y[i]) < 2)//exploding one right on the player
	     return bomb_owner[i];


      if(abs(bomb_x[i]-player_x[p]) < 2 &&//bomb lined up on the x-axis
	     (dist = abs(bomb_y[i]-player_y[p])) < bomb_range(i)*2){//and within range
         
		 if(dist < closestdist){//worth considering for the killing bomb
		    if(bomb_y[i] < player_y[i]){
               for(u8 j=1;j<bomb_range(i);j++){
                  if(IsSolid(GetMapTile(0,0))){//the bomb is blocked, couldn't have killed him
                     
				  }
			   }
			}
            closestdist = dist;
			closest = i;
		 }  
	  }
   }

   return 255;//didn't find the killer
}

void PlaceBomb(u8 p){
   player_state[p] ^= BOMB;

   //if(player_bombs_out(p) >= player_max_bombs(p))
     // return;

   u8 b = FindOpenBomb();

   if(b == 255)//no free bomb
      return;

   u8 x = player_x[p];
   u8 y = player_y[p];
   //bomb_x[b] = x;
   //bomb_y[b] = y;
   bomb_offset[b] = 0;



   u8 state = player_state[p];

   if(x & 1){//player is offset, bomb must be aligned on 16x16 boundaries. This wouldn't work if he was not aligned on 2 axis'(which wont happen)
      if(state & RIGHT)
	     x--;
      else
	     x++;
   }
   else if(!(y & 1)){
      if(state & UP)
	     y++;
      else
	     y--;
   }
   
   if(IsSolid(GetMapTile(x,y)))//blocked
      return;

   bomb_x[b] = x;
   bomb_y[b] = y;
   bomb_owner[b] = p;
   bomb_timer[b] = BOMBSTARTTIME;
   bomb_offset[b] = 0;
   bomb_state[b] = 0;
   //SetMapTile(x,y,10);

   TriggerFx(PLANT_BOMB_FX,255,true);
}

inline void GrabBomb(u8 p){//if there is a bomb under us, pick it up
   if(!(player_attrb[p] & GLOVE))//no glove, can't pick it up
      return;
   
   for(u8 i=0;i<MAX_BOMBS;i++){
      if(bomb_owner[i] != p)
	     continue;
      
	  if(abs(bomb_x[i]-player_x[p]) < 2 && abs(bomb_y[i]-player_y[p]) < 2){//were on top of our own bomb
      
	  }
   }
}

void KickBomb(u8 x, u8 y, u8 dir){
   u8 b = FindBomb(x,y,255);
   //if(b == 255)
   bomb_state[b] = MOVE | dir;
}

void ThrowBomb(u8 sx, u8 sy, u8 dir, u8 mag, u8 b){
   bomb_x[b]      = sx*8;//convert to sprite coords
   bomb_y[b]      = sy*8;
   bomb_state[b] = THROWN|dir;
   
   if(dir & RIGHT){
      bomb_offset[b] = (sx+mag)*8;//target x
      bomb_xtra[b]   = (sy+0)*8;//target y
   }
   else if(dir & LEFT){
      bomb_offset[b] = (sx-mag)*8;//target x
      bomb_xtra[b]   = (sy+0)*8;//target y
   }
   else if(dir & DOWN){
      bomb_offset[b] = (sx+0)*8;//target x
      bomb_xtra[b]   = (sy+mag)*8;//target y
   }
   else{
      bomb_offset[b] = (sx+0)*8;//target x
      bomb_xtra[b]   = (sy-mag)*8;//target y
   }

   //Calculate midpoint of distance so DrawBombs knows how to draw the arc
   bomb_timer[b] = (abs(bomb_x[b]-bomb_offset[b])+abs(bomb_y[b]-bomb_xtra[b]))/2;
}


inline void ExplodeBomb(u8 b){
   u8 range = GetPlayerRange(bomb_owner[b])*2;//TODO? COULD CHANGE VIA PICKUP DURING EXPLOSION
   u8 x = bomb_x[b];
   u8 y = bomb_y[b];
   u8 bframe = bomb_frame(b);
   u8 fframe = fire_frame(b);
   u8 t;
   bool end = false;
   
   if(bomb_timer[b] == 0){
      end = true;
      SetMapTile(x,y,0);
	  fframe = 0;//blank out fire
	  bomb_owner[b] = 255;
//	  bomb_state[b] = 0;
   }
   else if(bomb_timer[b] == (EXPLODETIME-2)){
      TriggerFx(EXPLODESFX,255,true);
   }
   else if((t = GetMapTile(x,y)) == FIRE_CENTER+((fframe)*28)){//same frame is already drawn, don't bother(this is expensive!)
      return;
   }
   else{
   //No need, block nullifies bomb.
      //if(!IsBlock(t))//Did a block fall on it? Then don't draw center fire
         SetMapTile(x,y,FIRE_CENTER+((fframe)*28));
   }   



   bool u,d,l,r;
   u=d=l=r=true;
   bool tip = false;
   
   fframe *= 28;

   for(u8 i=2;i<=range;i+=2){
   if(i == range)
      tip = true;

      if(y-i < ARENA_START_Y && y-i < 200)
	     u = false;
      if(y+i > 22)
	     d = false;
      if(x-i < ARENA_START_X && x-i < 200)
	     l = false;
      
	  if(u){
         t = GetMapTile(x,y-i);
      if(IsSolidForFire(t)){
            if(!end){
		       if(IsBlock(t)){
                  SetMapTile(x,y-i,(BLOCK_START+4)+(bframe<<2));
		       }
		//	   else if(IsBomb(t))
		//	      TriggerBomb(x,y-i);
		    //   else
		      //    SetMapTile(x,y-i,FIRE_START);
		    }
		    else if(IsBlock(t))
		       DestroyBlock(x,y-i);//SetMapTile(x,y-i,0);
		
			
            u = false;
	     }
		 else{//not solid
            if(end)
		       SetMapTile(x,y-i,0);//DestroyBlock(x,y-i);
            else if(IsCenterFire(t)){

			}
			else if(IsHorizontalFire(t))
			   SetMapTile(x,y-i,FIRE_CENTER+((fframe)*28));
			else if(!tip)
		       SetMapTile(x,y-i,FIRE_VERT+(fframe/**28*/));
            else if(t < FIRE_START || IsTipFire(t))//don't draw a tip over another explosion(looks bad)
			   SetMapTile(x,y-i,FIRE_TOP_TIP+(fframe/**28*/));
		 }
	  }

	  if(d){
         t = GetMapTile(x,y+i);
         if(IsSolidForFire(t)){
            if(!end){
		       if(IsBlock(t)){
                  SetMapTile(x,y+i,(BLOCK_START+4)+(bframe<<2));
		       }
//			   else if(IsBomb(t))
//			      TriggerBomb(x,y+i);
		    //   else
		      //    SetMapTile(x,y-i,FIRE_START);
		    }
		    else if(IsBlock(t))
		       DestroyBlock(x,y+i);//SetMapTile(x,y+i,0);
		
			
            d = false;
	     }
		 else{
            if(end)
		       SetMapTile(x,y+i,0);//DestroyBlock(x,y+i);
            else if(IsCenterFire(t)){

			}
            else if(IsHorizontalFire(t))
			   SetMapTile(x,y+i,FIRE_CENTER+((fframe)*28));
            else if(!tip)
		       SetMapTile(x,y+i,FIRE_VERT+(fframe/**28*/));
            else if(t < FIRE_START || IsTipFire(t))
			   SetMapTile(x,y+i,FIRE_BOTTOM_TIP+(fframe/**28*/));
		 }
	  }

	  if(l){
         t = GetMapTile(x-i,y);
         if(IsSolidForFire(t)){
            if(!end){
		       if(IsBlock(t)){
                  SetMapTile(x-i,y,(BLOCK_START+4)+(bframe<<2));
		       }
//			   else if(IsBomb(t))
//			      TriggerBomb(x-i,y);
		    //   else
		      //    SetMapTile(x,y-i,FIRE_START);
		    }
		    else if(IsBlock(t))
		       DestroyBlock(x-i,y);//SetMapTile(x-i,y,0);
		

            l = false;
	     }			
		 else{
            if(end)
		       SetMapTile(x-i,y,0);//DestroyBlock(x-i,y);
            else if(IsCenterFire(t)){

			}
            else if(IsVerticalFire(t))
			   SetMapTile(x-i,y,FIRE_CENTER+((fframe)*28));
            else if(!tip)
		       SetMapTile(x-i,y,FIRE_HORZ+(fframe/**28*/));
            else if(t < FIRE_START || IsTipFire(t))
			   SetMapTile(x-i,y,FIRE_LEFT_TIP+(fframe/**28*/));
		 }
	  }

	  if(r){
         t = GetMapTile(x+i,y);
         if(IsSolidForFire(t)){
            if(!end){
		       if(IsBlock(t)){
                  SetMapTile(x+i,y,(BLOCK_START+4)+(bframe<<2));
		       }
//			   else if(IsBomb(t))
//			      TriggerBomb(x+i,y);
		    //   else
		      //    SetMapTile(x,y-i,FIRE_START);
		    }
		    else if(IsBlock(t))
		       DestroyBlock(x+i,y);//SetMapTile(x+i,y,0);
		
			
            r = false;
	     }
		 else{
            if(end)
		       SetMapTile(x+i,y,0);//DestroyBlock(x+i,y);
            else if(IsCenterFire(t)){

			}
            else if(IsVerticalFire(t))
			   SetMapTile(x+i,y,FIRE_CENTER+((fframe)*28));
            else if(!tip)
		       SetMapTile(x+i,y,FIRE_HORZ+(fframe/**28*/));
            else if(t < FIRE_START || IsTipFire(t))
			   SetMapTile(x+i,y,FIRE_RIGHT_TIP+(fframe/**28*/));
         }
	  }



   }

}

inline bool IsGhost(u8 i){
   return player_attrb2[i] & GHOST;
}

inline void UpdateGhostBomber(u8 p){
//here only left and right are used. Left will rotate around the play area in a counter-clockwise manner
//and right will be clockwise. player_x will be used as a magnitude starting at 0 in the lowest left corner 
//and ending at the lowest right corner. Easiest, fastest, and smallest way to handle this?
   
   if(--player_ftime[p] > 254){//rolled over
     player_ftime[p] = 8;
	 if(++player_frame[p] > 3)
	    player_frame[p] = 0;
   }
   if(player_state[p] & MOVE && ((player_state[p] & LEFT && player_x[p] > 0) || (player_state[p] & RIGHT && player_x[p] < 38))){
      player_offset[p] += 2;
	  if(player_offset[p] > 15){
         player_offset[p] = 0;
      
	     if(player_state[p] & LEFT)
            --player_x[p];
         else
            ++player_x[p];
         
		 if(player_x[p] == 11 ||
		    player_x[p] == 12 ||
			player_x[p] == 26 ||
			player_x[p] == 27){
			   player_state[p] |= MOVE;
               return;//skip BOMB
		 }else
		    player_state[p] ^= MOVE;
      }
	//  player_state[p] = 0;
   }
   
   if(player_offset[p])
      return;

   if(player_state[p] & BOMB){
      player_state[p] ^= BOMB;
   if(!player_bombs_out(p)){


	  u8 b = FindOpenBomb();
	  if(b == 255)//no open bomb
		 return;
      
	  bomb_owner[b] = p;
	  bomb_timer[b] = BOMBSTARTTIME;

	  if(player_x[p] < 11){//left side
		 ThrowBomb(0,20-(player_x[p]*2),RIGHT,8,b);
	  }
	  else if(player_x[p] > 12 && player_x[p] < 26){//top
         ThrowBomb((player_x[p]-12)*2,1,DOWN,6,b);
	  }
	  else if(player_x[p] > 27){//right side
         ThrowBomb(28,(player_x[p]*2)-24,LEFT,8,b);
	  }

	  
   }
   }
}

void UpdatePlayers(){

   u8 state;
   u8 t,t2;
   u8 x,y;
//   int off;
   for(u8 i=0;i<MAX_PLAYERS;i++){
      if(player_x[i] == 255)//player is being handled by a special case, dont think for this one
	     continue;

	  if(player_attrb2[i] & GHOST){
	     if((gamestate & ALLOWGHOSTS) && roundtime > (60*SEC))//don't allow ghosts in over time
	        UpdateGhostBomber(i);//see above
         
		 continue;       
	  }

      state = player_state[i];
      
	  if(state == DYING){
	     if(!(--player_ftime[i])){
		    player_ftime[i] = 14;
		    if((++player_frame[i]) > 3){//done with death animation
			   if(player_wait[i] != 255){//his killer was saved in _wait, now see if hes a ghost and earned respawn
                  t = player_wait[i];
				  if(player_attrb2[t] & GHOST && roundtime > 60*SEC){//the killer was a ghost, bring him back to life           
					 player_x[t] = player_x[i];
					 player_y[t] = player_y[i];
					 player_state[t] = 0;
					 player_offset[t] = 0;
					 player_attrb2[t] ^= GHOST;
			      }
			   }
			   DispersePowerups(i);//disperse his powerups around the map
			   //Now determine the players position as a ghost(only player_x is used for ghost)
			   player_attrb2[i] = GHOST;//get rid of all powerups
			   player_attrb[i] = player_attrb[i] & TEAM2;//save team bit
			   player_state[i] = 0;
			   player_offset[i] = 0;//make sure the ghost can recieve input
			   x = player_x[i]/2; y = player_y[i]/2;
			   if(x < 7){
                  if(x < y)//left
				     player_x[i] = 11-y;
                  else//top
				     player_x[i] = 12+x;
			   }
			   else{
			      if(y < 15-x)//top
                     player_x[i] = 12+x;
                  else//right
				     player_x[i] = 27+y; 
			   }
               
			   /*if(i == 0){//human player, respawn him for debugging
			      player_state[i] = 0;
                  player_x[i] = player_y[i] = 2;
				  player_ftime[i] = 0;
				  player_frame[i] = 0;
				  player_offset[i] = 0;
			   }
			   */
			}
         }
		 continue;
	  }

      x = player_x[i]; y = player_y[i];

      //First thing, check to see if player is hit with fire against actual vram
	  if(!(gamestate & VICTORY)){
	     if(IsFire(GetMapTile(x+0,y+0)) || 
	        IsFire(GetMapTile(x+1,y+0)) || 
	        IsFire(GetMapTile(x+0,y+1)) || 
	        IsFire(GetMapTile(x+1,y+1))  ){
         
		    KillPlayer(i);
			player_wait[i] = FindKiller(i);//keep track of who killed him

		    continue;
	     }
		 else if((roundtime%(SEC/2))==2){//invulnerability counts down every 1/2 second
            t = ((player_attrb2[i] & 0b01100000)>>5);
            player_attrb2[i] |= 0b01100000;
		    player_attrb2[i] ^= 0b01100000;
		    if(t > 1){
		       t--;
			player_attrb2[i] |= (t<<5);
		 }
         
	     }
      }
	  else{//VICTORY!
	     if(player_offset[i] != 255){//if(!player_offset[i]){//VICTORY! and has stopped moving
            player_offset[i] = 255;//first tick of victory
		    player_wait[i] = 0;
		    player_ftime[i] = 14;
	        player_state[i] = UP;
		    player_frame[i] = 0;
	     }


	  //if(player_offset[i] == 255){//is in victory animation

         if(player_state[i] & UP){
		    if(player_ftime[i])
			   player_ftime[i]--;
		    else if(++player_wait[i] > 15)
               player_state[i] = DOWN;
	     }
		 else{//down
            if(--player_wait[i] < 1){
			   player_state[i] = UP;
			   player_ftime[i] = 14;//wait on the ground a bit
            }
		 }


		 if(player_ftime[i])
		    player_frame[i] = 0;
         else
		    player_frame[i] = 1;

		 continue;
	  
      }
	  //Check to see if we are poisoned, if so do any poison actions and check for touching other players
	  if(player_attrb[i] & POISON){
            
	     for(u8 p=0;p<MAX_PLAYERS;p++){
            if(abs(player_x[i]-player_x[p]) < 2 && abs(player_y[i]-player_y[p]) < 2)
		       player_attrb[p] |= POISON;
	     }
	  }
      
	  if(state & STUNNED){
			if(player_ftime[i]){
			   player_ftime[i]--;
            }
			else{
			   player_ftime[i] = 10;
			   if(++player_frame[i] > 7)
			      player_frame[i] = 0;
			}  
         if(player_wait[i]){
		    player_wait[i]--;
			continue;
         }
		 else
		    player_state[i] ^= STUNNED;

	  }

	  if(state & BOMB){
	     PlaceBomb(i);
	     if(player_ftime[i] >= 4*SEC)//is in idle animation
		    player_ftime[i] = 1;
	  }
      if(state & GRAB)
	     GrabBomb(i);
      
	  u8 speed = player_speed(i);

	  if(state & MOVE){
	     if(!player_ftime[i]){
            player_ftime[i] = PLAYER_FRAME_TIME-(speed);
			if(++player_frame[i] > 7)
			   player_frame[i] = 0;
		 }
		 else if(player_ftime[i] > PLAYER_FRAME_TIME-(speed)){//get rid of idle ticks
		    player_ftime[i] = PLAYER_FRAME_TIME-(speed);
		    player_frame[i] = 0;
		 }
         else
		    player_ftime[i]--;

	     if(!player_offset[i]){//just requested to move, see if it's blocked

			if(state & UP){
			   if(y < ARENA_START_Y+1){
			      player_state[i] ^= MOVE;
				  continue;
			   }
               t = GetMapTile(x,y-1); t2 = GetMapTile(x+1,y-1);
			   if(IsBombTop(t)){
			      
               }
			   else if((player_attrb[i] & BOOT) && IsBomb(t)){
			      bomb_state[FindBomb(x+0,y-1,t)] = MOVE|UP;
				  player_state[i] ^= MOVE;
				  continue;
			   }else if(IsSolid(t) || IsSolid(t2)){
			      if(IsBomb(t) || IsBomb(t2)){
                  
				  }
				  if(!IsSolid(t2) && !IsSolid(GetMapTile(x+2,y)) && !IsSolid(GetMapTile(x+2,y+1))){
                     player_state[i] ^= UP;
					 player_state[i] |= RIGHT;
                  }
				  else if(!IsSolid(t) && IsSolid(t2) ){
                     player_state[i] ^= UP;
					 player_state[i] |= LEFT;
				  }
				  else{
					 player_state[i] ^= MOVE;
					 continue;
				  }
			   }

			}
			else if(state & DOWN){
			   if(y > ARENA_START_Y+19){
			      player_state[i] ^= MOVE;
				  continue;
			   }
               t = GetMapTile(x,y+2); t2 = GetMapTile(x+1,y+2);
               if(IsBombBottom(t)){
			      
               }
			   else if((player_attrb[i] & BOOT && player_y[i] < 19) && IsBomb(t)){
			      bomb_state[FindBomb(x+0,y+2,t)] = MOVE|DOWN;
				  player_state[i] ^= MOVE;
				  continue;
			   }else if(IsSolid(t) || IsSolid(t2)){
				  if(!IsSolid(t2) && !IsSolid(GetMapTile(x+2,y)) && !IsSolid(GetMapTile(x+2,y+1))){
                     player_state[i] ^= DOWN;
					 player_state[i] |= RIGHT;
                  }
				  else if(!IsSolid(t) && IsSolid(t2) && !IsSolid(GetMapTile(x-1,y)) && !IsSolid(GetMapTile(x-1,y+1))){
                     player_state[i] ^= DOWN;
					 player_state[i] |= LEFT;
				  }
				  else{
				     player_state[i] ^= MOVE;
				     continue;
			      }
			   }

			}
			else if(state & LEFT){
			   if(x < ARENA_START_X+1){
			      player_state[i] ^= MOVE;
				  continue;
			   }
               t = GetMapTile(x-1,y); t2 = GetMapTile(x-1,y+1);
               if(IsBombLeftSide(t)){
			      
               }
			   else if((player_attrb[i] & BOOT) && IsBomb(t)){
			      bomb_state[FindBomb(x-1,y+0,t)] = MOVE|LEFT;
				  player_state[i] ^= MOVE;
				  continue;
			   }else if(IsSolid(t) || IsSolid(t2)){
				  if(!IsSolid(t2) && !IsSolid(GetMapTile(x,y+2)) && !IsSolid(GetMapTile(x+1,y+2))){
                     player_state[i] ^= LEFT;
					 player_state[i] |= DOWN;
                  }
				  else if(!IsSolid(t) && IsSolid(t2) && !IsSolid(GetMapTile(x,y-1)) && !IsSolid(GetMapTile(x+1,y-1))){
                     player_state[i] ^= LEFT;
					 player_state[i] |= UP;
				  }
				  else{
				     player_state[i] ^= MOVE;
				     continue;
			      }
			   }

			}
			else{// if(state & RIGHT){
			   if(x > 25){
			      player_state[i] ^= MOVE;
				  continue;
			   }
               t = GetMapTile(x+2,y); t2 = GetMapTile(x+2,y+1);
			   if(IsBombRightSide(t)){
			      
               }
			   else if((player_attrb[i] & BOOT) && IsBomb(t)){
			      bomb_state[FindBomb(x+2,y+0,t)] = MOVE|RIGHT;
				  player_state[i] ^= MOVE;
				  continue;
			   }else if(IsSolid(t) || IsSolid(t2)){
				  if(!IsSolid(t2) && !IsSolid(GetMapTile(x,y+2)) && !IsSolid(GetMapTile(x+1,y+2))){
                     player_state[i] ^= RIGHT;
					 player_state[i] |= DOWN;
                  }
				  else if(!IsSolid(t) && IsSolid(t2) && !IsSolid(GetMapTile(x,y-1)) && !IsSolid(GetMapTile(x+1,y-1))){
                     player_state[i] ^= RIGHT;
					 player_state[i] |= UP;
				  }
				  else{
				     player_state[i] ^= MOVE;
				     continue;
			      }
			   }

			}
	     }
         
		 ////////Calculate player's speed of movement(set near the top of function)
		 ////////Here we either apply wait ticks or double move ticks to achieve speeds in 50% increments
         ////////0 = half speed, 1 = base speed, 2 = fast speed(150% base), 3 = fastest(200% base)
		 
		 ///////TODO, NOT VERY CLEVER OR GRACEFUL :)


///////////////////////SPEEDS AREN'T RIGHT, HALF SPEED IS PROBABLY NORMAL SPEED, ETC.    
         ///!!!!!!!!TODO!!!!!!check if we are on a conveyor belt and adjust speed according to direction

	     if(speed == 0 && blip != 2/*(blink || blip)*/){//quarter speed(not totally smooth movement)
            continue;//only move every 3rd tick
		 }
         if(speed == 1 && (gamestate & BLINK)){//half speed
            continue;//wait 1 tick every other
		 }
		 if(speed == 2 && blip == 2){//3/4 speed
            continue;//wait 1 tick every 3rd tick
		 }
		 else if(speed == 3){
         
		 }
		 else if(speed == 4 && blip){//1 1/4 speed(not totally smooth movement)
            player_offset[i]++;//extra offset every 3rd tick
		 }
		 else if(speed == 5 && (gamestate & BLINK)){//1 1/2 speed
            player_offset[i]++;//extra offset every other tick
         }
		 else if(speed == 6 && ((gamestate & BLINK) || blip)){//1 3/4 speed(not totally smooth)
		    player_offset[i]++;//extra offset every other and every 3rd tick
		 } 
         else{//double speed
            player_offset[i]++;//extra offset every tick
		 }



         if(player_offset[i] % 2)
            player_offset[i]++;

         if(++player_offset[i] > 7){
            player_offset[i] = 0;
            player_state[i] ^= MOVE;

	        if(state & UP)        {player_y[i]--;}
	        else if(state & DOWN) {player_y[i]++;} 
            else if(state & LEFT) {player_x[i]--;}
	        else                  {player_x[i]++;}

            if(player_x[i] & 1 || !(player_y[i] & 1))//not tile aligned, can't interact with special tiles
			   continue;
         
			//Now check for special tiles
			u8 t = GetMapTile(player_x[i],player_y[i]);
			//if(t > POWERUP_START+11)
			//   t -= 12;//adjust for flash

			if(t >= POWERUP_START && t <= POWERUP_START+20){
			   if(player_attrb[i] & POISON)
			      player_attrb[i] ^= POISON;
               RestoreMapTile(player_x[i],player_y[i]);
			   TriggerFx(POWERPICKUPSFX,POWERPICKUPSFXVOL,true);  
			   if(t == POWERUP_START+0){//speed
                  if((player_attrb2[i] & 0b00000111) < 7)
				     player_attrb2[i]++;     
			   }
			   else if(t == POWERUP_START+4){//fire
                  if((player_attrb[i] & 0b00000111) < 7)
				     player_attrb[i]++;
			   }
			   else if(t == POWERUP_START+8){//bomb
                  t=((player_attrb2[i] & 0b00011100)>>2);
				  if(t < 7){
				     player_attrb2[i] |= 0b00011100;
					 player_attrb2[i] ^= 0b00011100;
					 player_attrb2[i] |= (t<<2);
                  }
			   }
			   else if(t == POWERUP_START+12){//kick
                  player_attrb[i] |= 0b10000000;
			   }
			   else if(t == POWERUP_START+16){//glove
                  player_attrb[i] |= GLOVE;
			   }
			   else if(t == POWERUP_START+20){//poison
                  player_attrb[i] |= POISON;
			   }
            }
		 }
	  }
	  else{//not moving
		 if(++player_ftime[i] < 4*SEC){//hasn't been sitting idle long enough...
            player_frame[i] = 0;
			continue;
		 }
		 
		 if(player_ftime[i] > 246){
            player_ftime[i] = 4*SEC;//keep him above idle threshhold
			if(++player_frame[i] >= NUMIDLEFRAMES)
			   player_frame[i] = 0;
		 }
	     

		 //Now check to see if a player is trapped
         //TODO

		 //Check to see how long the player has been idle
	 }
   }//for each player
}


void StartMatch(){
   LoadLevel();
   gamestate = PLAYING;
   if(!playSong)
      StartSong(StageSong);
}

void LevelSpecial(){//handle all special logic and animation for a specific level
   u8 t,t2;
#if true//mine level
   static u8 mineframe = 0;
   static u8 mineframetime = 15;
   static u8 cartowner = 255;
   static u8 cartx = 9;
   static u8 carty = 6;
   static u8 cartoffset;//0..15
   static u8 cartstate;
   static u8 cartlastdir=LEFT;
//   static u8 fadeticks=0;
//   static bool fadedir=false;
 /*  
   fadeActive = true;

   if(++fadeticks > 20){
      fadeticks = 0;
      
      if(fadedir){
		 if(--fadeStep < 1)
	        fadedir = !fadedir;
	  }
	  else{
		 if(++fadeStep > 3)
	        fadedir = !fadedir;
	  }
   }   

   return;
*/

   if(cartowner == 255){//see if any players have walked on
      for(u8 i=0;i<MAX_PLAYERS;i++){
         if(player_x[i] == cartx && player_y[i] == carty){//player has entered cart
            cartowner = i;
			player_x[i] = 255;//make sure Render() doesn't draw him

			   SetMapTile(cartx,carty,SPECIAL_START+((cartlastdir>DOWN)*4));//get rid of cart tiles

			if(cartlastdir == UP)
			   cartlastdir = cartstate = DOWN;
            else if(cartlastdir == DOWN)
			   cartlastdir = cartstate = UP;
            else if(cartlastdir == LEFT)
			   cartlastdir = cartstate = RIGHT;
            else// if(cartlastdir == RIGHT)
			   cartlastdir = cartstate = LEFT;
            
			return;
		 }
	  }
   }
   else{//update cart
      player_x[cartowner] = 255;//make sure player doesnt draw or update on his own
      cartoffset += 2;
	  if(cartoffset > 7){
         cartoffset = 0;

		 if(cartstate == UP)
		    carty--;
         else if(cartstate == DOWN)
		    carty++;
         else if(cartstate == LEFT)
		    cartx--;
         else
		    cartx++;
         
		 for(u8 i=0;i<MAX_PLAYERS;i++){//check for players in the way
            if(abs(player_x[i]-cartx) < 2 && abs(player_y[i]-carty) < 2)
			   KillPlayer(i);
		 }
		 //player_x[cartowner] = cartx*2;
		 //player_y[cartowner] = carty*2;
		 //player_state[cartowner] = cartstate|MOVE;
		 //player_offset[cartowner] = cartoffset/2;                  

		 t = GetMapTile((cartx*2),(carty*2));
         //Only corner pieces can change direction
         if(t < SPECIAL_START+8){

		 }
		 else if(t < SPECIAL_START+12){//6 to 9 oclock

			if(cartlastdir == UP)
			   cartlastdir = cartstate = LEFT;
            else
			   cartlastdir = cartstate = DOWN;
		 }
		 else if(t < SPECIAL_START+16){//9 TO 12 oclock

            if(cartlastdir == DOWN)
			   cartlastdir = cartstate = LEFT;
            else
			   cartlastdir = cartstate = UP;
		 }
		 else if(t < SPECIAL_START+20){//12 TO 3 oclock
            if(cartlastdir == DOWN)
			   cartlastdir = cartstate = RIGHT;
            else
			   cartlastdir = cartstate = UP;
		 }

		 else{// if(t < SPECIAL_START+24){//9 TO 12 oclock
            if(cartlastdir == UP)
			   cartlastdir = cartstate = RIGHT;
            else
			   cartlastdir = cartstate = DOWN;
		 }
	  }
   char xo = 0;
   char yo = 0;
   char f = 0;
   if(cartstate == UP){yo = -cartoffset;f=0;}
   else if(cartstate == DOWN){yo = cartoffset;f=1;}
   else if(cartstate == LEFT){xo = -cartoffset;f=2;}
   else                      {xo = cartoffset;f=3;}
   DrawSpriteTile((cartx*8)+xo,(carty*8)+yo-8,FIRSTSPECIALFRAME+f,0,0,0);
      
   }

   if(!--mineframetime){
      mineframetime = 30;
	  if(++mineframe > 3)
	     mineframe = 0;
   
//Animate the wall tiles
      if(roundtime >= 60*SEC){
         for(u8 y=0;y<10;y+=2)
	        for(u8 x=0;x<12;x+=2){
               SetMapTile(2+ARENA_START_X+(x*2),2+ARENA_START_Y+(y*2),WALL_START+(4*mineframe));
	     } 
      }
	  else{//must check for the extra dropped blocks
         for(u8 y=ARENA_START_Y;y<VRAM_TILES_V;y+=2)
	        for(u8 x=ARENA_START_X;x<VRAM_TILES_H-2;x+=2){
			   t = GetMapTile(x,y);
			   if(t >= WALL_START && t <= WALL_END)
                  SetMapTile(x,y,WALL_START+(4*mineframe));
	     } 
	  }
   }

return;
#endif


#if MAP == 0
   static u8 count=30;
   static bool o;
   if(!--count){
      count = 30;
	  o = !o;
	  }
   u8 t;
   for(u8 i=0;i<8;i++){
      if((t =GetMapTile(8+(i*2),6)) >= SPECIAL_START || !t)
	     SetMapTile(8+(i*2),6,SPECIAL_START+8+(o*4));
      if((t = GetMapTile(6+(i*2),18)) >= SPECIAL_START || !t)
	     SetMapTile(6+(i*2),18,SPECIAL_START+8+(o*4));
   }

   for(u8 i=0;i<6;i++){
      if((t =GetMapTile(6,6+(i*2))) >= SPECIAL_START || !t)
	     SetMapTile(6,6+(i*2),SPECIAL_START+0+(o*4));
      if((t = GetMapTile(22,8+(i*2))) >= SPECIAL_START || !t)
	     SetMapTile(22,8+(i*2),SPECIAL_START+0+(o*4));
   }
#else//if LEVEL == 1//SNOW LEVEL
   static u8 IsOn = 0;
   static u8 WasOn = 0;
   static u8 IsCracked = 0;//needed for fire going over tiles
   ram_tiles_reserved = 0;


if(blip){//should be no need to check every tick? players not fast enough?

   WasOn = IsOn;
   IsOn = 0;
   for(u8 i=0;i<MAX_PLAYERS;i++){//check for players on thin ice
      for(u8 j=0;j<5;j++){
         if(player_x[i] == (t =pgm_read_byte(&IcePos[(j*2)+0])) && 
		    player_y[i] == (t2=pgm_read_byte(&IcePos[(j*2)+1])) ){
			IsOn |= (1<<j);   	
	        if(!(WasOn & (1<<j))){//time to set the tiles
               if(!(IsCracked & (1<<j))){//not yet cracked
                  IsCracked |= (1<<j);//keep track so fire doesn't make it uncracked...
			   }
               else
			      SetMapTile(t,t2,SPECIAL_START+4);
			}
		 }  
	  }
   }
}

   //Now make sure fire doesn't overdraw cracks...
   for(u8 j=0;j<5;j++){
      if((IsCracked & (1<<j)) &&
	  GetMapTile(t =pgm_read_byte(&IcePos[(j*2)+0]),t2=pgm_read_byte(&IcePos[(j*2)+1])) == FLOOR_START)//fire or bomb has overdrawn it, ok to redraw now
	     SetMapTile(t,t2,SPECIAL_START);
   }

   t = 0;

    //PutSprite(5*8,5*8,FIRST_SPECIAL_SPRITE-1,false,0);   

   //Now draw the igloo
   for(u8 x=0;x<6;x++)
   for(u8 y=0;y<6;y++){
      t = vram[((y+4)*30)+(x+4)]-RAM_TILES_COUNT;
      t2 = pgm_read_byte(&IglooMap[(y*6)+x]);

	  if(t2 >= SPECIAL_OCCLUSION_START && t <= SPECIAL_OCCLUSION_END){
	     SetTile(x+4,y+4,t2);
	     continue;
	  }

      if(t < 4 || (t2 <= BLOCK_START && t2 >= BLOCK_END && t2 >= SPECIAL_OCCLUSION_START && t2 < SPECIAL_OCCLUSION_END))
	     SetTile(x+4,y+4,t2);
	  
	  if(t2 >= SPECIAL_FOREGROUND_START && t2 <= SPECIAL_FOREGROUND_END){
	     t2 -= SPECIAL_FOREGROUND_START;
	    // for(u8 i=0;i<MAX_SPRITES;i++){
          //  if(((sprites[i].x+7)/8) == x+4 &&
			//   ((sprites[i].y+7)/8) == y+4){
			      PutSprite((x+4)*8,(y+4)*8,FIRSTSPECIALSPRITE +t2,false,0);
		      //    i = 200;
				//  continue;
			//}
		 //}

	  }
	  /*	  

	  if(t != t2){//something was drawn over this
	     if(t < BOMB_START)//was a bomb or something, now redraw
		    SetTile(x+4,y+4,t2);
         else{//probably an active bomb
		    //PutSprite((x+4)*8,(y+4)*8,10,false,0);
		 }
	  }*/
   }


#endif
}

void DropBlocks(){
   if(block_dir & MOVE){//very first block, doesn't currently drop??
      
	  block_dir ^= MOVE;

   }

   if(block_z < ((block_y+1)*16)){
	  block_z += 8;
	  ++block_time;
      //SetMapTile(ARENA_START_X+(block_x*2),ARENA_START_Y+(block_y*2),WALL_START);//hack to draw first drop
      return;
   }
   
   if(block_z != 255){
      block_z = 255;
      TriggerFx(BLOCK_DROP_FX,254,false);
      SetMapTile(ARENA_START_X+(block_x*2),ARENA_START_Y+(block_y*2),WALL_START);

      for(u8 i=0;i<MAX_PLAYERS;i++)
	     if(!(player_state[i] == DYING) && !(player_attrb2[i] & GHOST) && abs(player_x[i]-(ARENA_START_X+(block_x*2))) < 2 && abs(player_y[i]-(ARENA_START_Y+(block_y*2))) < 2)
		    KillPlayer(i);
   }

   //TODO - rare situation where kicked bomb would be half way into the dropped block

   if(++block_time < BLOCK_TIME)
      return;

   block_z = 0;
   block_time = 0;   
     
   if(block_dir == UP){
      if(block_y > block_miny)
		 block_y--;
      else{
		 block_dir = RIGHT;
	     block_miny++;
	     block_x++;
	  }
   }
   else if(block_dir == RIGHT){
      if(block_x < block_maxx)
		 block_x++;
      else{
		 block_dir = DOWN;
	     block_maxx--;
	     block_y++;
	  }
   }
   else if(block_dir == DOWN){
      if(block_y < block_maxy)
		 block_y++;
      else{
		 block_dir = LEFT;
	     block_maxy--;
	     block_x--;
	  }
   }
   else{//LEFT
      if(block_x > block_minx)
		 block_x--;
      else{
		 block_dir = UP;
	     block_minx++;
	     block_y--;
	  }
   }

   if(GetMapTile(ARENA_START_X+(block_x*2),ARENA_START_Y+(block_y*2)) == WALL_START)//skip this one
      block_z = 255;
}

void Victory(){

}

void RoundFade(){
    //roundtime++;//undo what timer is about to do(freeze time in logic loop);rolls over for 65535/infinite
   if(masterVolume == NORMAL_VOL-96)
      FadeOut(10,false);

   if((gamestate & BLINK) && masterVolume)
      masterVolume--;
}

void FlashPowerUps(){
  /*static u8 section = 0;//NOT ENOUGH FLASH FOR FLASHING POWERUPS ANYMORE!!!!!!! :(
  //TODO - SLOW NOT OPTIMIZED
   u8 t;
   for(u8 y=ARENA_START_Y+(section*6);y<(ARENA_START_Y+(section*6))+6;y+=2)
   for(u8 x=ARENA_START_X;x<VRAM_TILES_H-2;x+=2){
      t = GetMapTile(x,y);
	  if(IsPowerUp(t)){
         if(t < POWERUP_START+11)
		    SetMapTile(x,y,t+12);
         else
		    SetMapTile(x,y,t-12);
	  }
   }
   if(++section > 3)//do 1 fourth of the screen at a time for speed
      section = 0;
*/
}

u8 CalculateVictor(){
   
   u8 numalive=255;
   
   
   for(u8 i=0;i<numplayers;i++){//check for victory
      if(!(player_attrb2[i] & GHOST))
	     numalive++;
   }

   return numalive;

}

void SortBombs(){

}

void UpdateBombs(){
   SortBombs();
   for(u8 i=0;i<MAX_BOMBS;i++){

      if(bomb_owner[i] == 255)
	     continue;

//TODO NO NEED FOR STEPPED OFF???!!!
	 /* if(!(bomb_state[i] & STEPPED_OFF))//owner hasn't stepped off this bomb yet
	     if(abs(player_x[bomb_owner[i]]-bomb_x[i]) > 1 || abs(player_y[bomb_owner[i]]-bomb_y[i]) > 1){
		    bomb_state[i] |= STEPPED_OFF;
		  // beep();
	  }
*/
	  if(bomb_state[i] & THROWN){
      //Thrown bombs are converted to actual sprite coords upon throwing(*=8), the original bomb_x/_y are used
	  //as the starting point while bomb_offset is used as target_x and bomb_xtra is used as target_y(also *= 8)
	  //When the bomb reaches it's target it converts back to tile coordinates and is done throwing. The addittional
	  //y value added for the arc effect is calculated every tick based on distance to target(in DrawBombs()).
	  //bomb_timer is temporarily used to keep track of the mid-point to calculate bomb_z(saving ram is fun...)

      bool landed = false;
	  if(bomb_state[i] & RIGHT){
	     bomb_x[i] += BOMBSPEED;
	     if(bomb_x[i] >= bomb_offset[i])
		    landed = true;
      }
	  else if(bomb_state[i] & LEFT){
	     bomb_x[i] -= BOMBSPEED;
	     if(bomb_x[i] <= bomb_offset[i])
		    landed = true;
	  }
	  else if(bomb_state[i] & DOWN){
	     bomb_y[i] += BOMBSPEED;
	     if(bomb_y[i] >= bomb_xtra[i])
		    landed = true;
	  }
	  else{
	     bomb_y[i] -= BOMBSPEED;
	     if(bomb_y[i] >= bomb_xtra[i])
		    landed = true;
      }
		 
	  if(landed){//we have reached the target
		    TriggerFx(HIT_FLOOR_FX,255,true);
		    TriggerFx(HIT_FLOOR_FX2,255,true);
			bomb_timer[i] = BOMBSTARTTIME;//bomb_timer was used to keep track of arc midpoint
			bomb_x[i] /= 8;
			bomb_y[i] /= 8;

			if((bomb_state[i] & 0b00001100))//up or down
			   bomb_y[i]++;//HACK,WHY??!!
//BUG HERE?? should be bomb_y[i] > 21...doesn't go out of bounds anyways???
            if(bomb_x[i] < ARENA_START_X || bomb_x[i] > ARENA_START_X+25 || bomb_y[i] < ARENA_START_Y || bomb_y[i] > ARENA_START_Y+20){//went out of bounds
			   bomb_owner[i] = 255;
			   continue;
            }

            if(IsSolid(GetMapTile(bomb_x[i],bomb_y[i]))){//we hit something solid, bounce over 1 grid space
			  
			   if((bomb_state[i] & 0b00001100))//up or down
			      bomb_y[i]--;//HACK,WHY??!!

			   ThrowBomb(bomb_x[i],bomb_y[i],(bomb_state[i] & 0b00001111),2,i);
			}
			else{
			   bomb_state[i] = 0;//now a normal bomb
               for(u8 k=0;k<MAX_PLAYERS;k++){
                  if(abs(bomb_x[i]-player_x[k]) < 2 && abs(bomb_y[i]-player_y[k])){
				     player_state[k] |= STUNNED;
					 player_wait[k] = STUNLENGTH;
					 player_frame[k] = 0;
					 player_ftime[k] = 10;
                  }
			   }
		       i--;
			   continue;//make it redraw immediately
			}
	  }


      if(bomb_x[i] > VRAM_TILES_H*8 || bomb_y[i] > VRAM_TILES_V*8){
         bomb_owner[i] = 255;
	  }
         
		 continue;
	 }//state & THROWN

      if(bomb_timer[i])
	     bomb_timer[i]--;

      if(bomb_state[i] & MOVE){
        u8 t,x,y;
	     if(!bomb_offset[i]){//bomb was drawn with tiles last frame, restore it TODO
			//TODO--Never check for colissions when the bomb is unaligned, it cannot stop no matter what
			//TODO--BOMB WILL ENTER SOLID FOR FIRST TICK OF MOVE!!(only if artificially set in input??)
			if((bomb_state[i] & UP    && IsSolid(t=GetMapTile(x=bomb_x[i]+0,y=bomb_y[i]-1))) ||
               (bomb_state[i] & DOWN  && IsSolid(t=GetMapTile(x=bomb_x[i]+0,y=bomb_y[i]+2))) ||
               (bomb_state[i] & LEFT  && IsSolid(t=GetMapTile(x=bomb_x[i]-1,y=bomb_y[i]+0))) ||
			   (bomb_state[i] & RIGHT && IsSolid(t=GetMapTile(x=bomb_x[i]+2,y=bomb_y[i]+0))) ){
							//GOT RID OF BOMBS ACTING LIKE POOL BALLS...	   
			   /*if(IsBomb(t)){
                  u8 b = FindBomb(x,y,t);
				  //if(b == 255)//should never happen...
				  //   beep();
                 // if(!((bomb_state[b] & UP|DOWN)    && (bomb_state[i] & UP|DOWN))//bombs are directly towards each other(otherwise just stop both)
				 // && !((bomb_state[b] & LEFT|RIGHT) && (bomb_state[i] & LEFT|RIGHT)))
				    
					//if(!(bomb_x[b] & 1) && !(bomb_y[b] & 1))//don't move unaligned bombs
				     //bomb_state[b] = bomb_state[i];
			   }*/
			   bomb_state[i] = 0;
			   SetMapTile(bomb_x[i], bomb_y[i], BOMB_START+(bomb_frame(i)<<2));
               continue;
			}
            if(!(bomb_x[i]&1) && !(bomb_y[i]&1)){//bomb is grid aligned so could stop moving

			   for(u8 j=0;j<MAX_PLAYERS;j++){
			  // continue;//TODO THIS ISNT CORRECT, NEED TO CHECK THE BOMB IS MOVING TOWARDS PLAYER(IS STUNNING A KICKER)
			      if(player_state[j] == DYING)
				     continue;
                  if(((bomb_state[i] & LEFT || bomb_state[i] & RIGHT) && abs(player_x[j]-bomb_x[i]) < 3 && abs(player_y[j]-bomb_y[i]) < 1)
				  || (abs(player_x[j]-bomb_x[i]) < 2 && abs(player_y[j]-bomb_y[i]) < 2)){//collided with player
                     bomb_state[i] = 0;
				     SetMapTile(bomb_x[i],bomb_y[i], BOMB_START+(bomb_frame(i)<<2));
                     player_state[i] |= STUNNED;
					 player_wait[i] = STUNLENGTH;
				     continue;
			      }
			   }
			}
		 }

         bomb_offset[i] += BOMBSPEED;

         if(bomb_offset[i] > 7){
            bomb_offset[i] = 0;		    

		    if(bomb_state[i] & UP){
			   if(!(bomb_y[i] & 1)){
                  SetTile(bomb_x[i]+0,bomb_y[i]+1,FLOOR_START+0);
                  SetTile(bomb_x[i]+1,bomb_y[i]+1,FLOOR_START+1);
			   }
			   else{
                  SetTile(bomb_x[i]+0,bomb_y[i]+1,FLOOR_START+2);
                  SetTile(bomb_x[i]+1,bomb_y[i]+1,FLOOR_START+3);
			   }
			   bomb_y[i]--;

			}else if(bomb_state[i] & DOWN){
			   if(!(bomb_y[i] & 1)){
                  SetTile(bomb_x[i]+0,bomb_y[i]+0,FLOOR_START+2);
                  SetTile(bomb_x[i]+1,bomb_y[i]+0,FLOOR_START+3);
			   }
			   else{
                  SetTile(bomb_x[i]+0,bomb_y[i]+0,FLOOR_START+0);
                  SetTile(bomb_x[i]+1,bomb_y[i]+0,FLOOR_START+1);
			   }
			   bomb_y[i]++;
			}else if(bomb_state[i] & LEFT){
			   if((bomb_x[i] & 1)){
                  SetTile(bomb_x[i]+1,bomb_y[i]+0,FLOOR_START+0);
                  SetTile(bomb_x[i]+1,bomb_y[i]+1,FLOOR_START+2);
			   }
			   else{
                  SetTile(bomb_x[i]+1,bomb_y[i]+0,FLOOR_START+1);
                  SetTile(bomb_x[i]+1,bomb_y[i]+1,FLOOR_START+3);
			   }
			   bomb_x[i]--;
			}else{
			   if((bomb_x[i] & 1)){
                  SetTile(bomb_x[i]+0,bomb_y[i]+0,FLOOR_START+1);
                  SetTile(bomb_x[i]+0,bomb_y[i]+1,FLOOR_START+3);
			   }
			   else{
                  SetTile(bomb_x[i]+0,bomb_y[i]+0,FLOOR_START+0);
                  SetTile(bomb_x[i]+0,bomb_y[i]+1,FLOOR_START+2);
			   }
			   bomb_x[i]++;
		   }	
	  }

	  }

	  //if(bomb_x[i] & 1 || !(bomb_y[i] & 1)){//not 16 grid aligned, can't yet explode or be stopped
		 //beep();
		
		// continue; 
		 //}
      
	  u8 t;
	  //Check to see if a block fell on it
	  if( ((t=GetMapTile(bomb_x[i]+0,bomb_y[i]+0))  >= WALL_START && t <= WALL_START+3)
	   || ((t=GetMapTile(bomb_x[i]+1,bomb_y[i]+1))  >= WALL_START && t <= WALL_START+3)){//CHECK SPECIAL BOMB MOVING INTO BLOCK CASE
	     bomb_owner[i] = 255;//bomb_timer[i] = EXPLODETIME-1;
		 continue;
      }

	  if(bomb_timer[i] < EXPLODETIME){
	     bomb_state[i] = EXPLODE;
	     ExplodeBomb(i);
	  }
	  else if(!(bomb_x[i] & 1) && (bomb_y[i] & 1) &&
	  IsFire(GetMapTile(bomb_x[i],bomb_y[i]))){//if not yet exploding and fire was drawn over the bomb
            bomb_timer[i] = EXPLODETIME-1;
			bomb_state[i] = EXPLODE;
			i--;
			continue;//make it explode this tick
	  }
	  else{// if(!bomb_state[i]){//see if it's time to draw a different frame

		 SetMapTile(bomb_x[i], bomb_y[i], BOMB_START+(bomb_frame(i)<<2));

	  }

	  if(!bomb_timer[i]){
         bomb_owner[i] = 255;
	  }

   }
}




void Logic(){

   if(gamestate & STARTMATCH){
   //Just came from map select menu
      SetTileSet();
      StartMatch();
	  WaitVsync(1);
   }

   if(gamestate & BOT_THOUGHT)
      gamestate ^= BOT_THOUGHT;
   
   prand();
   timecost = 0;//cpu time cost counter

   UpdatePlayers();
   UpdateBombs();

   u8 numplayers = 0;

   for(u8 i=0;i<MAX_PLAYERS;i++){
      if(!(player_attrb2[i] & GHOST))
	     numplayers++;
   }
   
   if(numplayers < 2 || ((gameoptions & USETEAM) && numplayers < 3)){//someone wins or draw
      if(!(gamestate & VICTORY)){//first victory tick
         gamestate |= VICTORY;
         TriggerFx(VICTORY_FX,255,true);
	     
	  }
      

	  RoundFade();
	  if(fadeStep == 0){//kernel done fading
         
		 for(u8 i=0;i<MAX_PLAYERS;i++)
		    if(!(player_attrb2[i] & GHOST))
			   SetScore(i,GetScore(i)+1);		 

		 StopSong();
		 WaitVsync(8);
		 SetMasterVolume(NORMAL_VOL);
		 FadeIn(2,false);
		 guistate = GVICTORYMENU;
		 return;
	  }
   }
   else{
      if(!(roundtime%SEC)){//poison type could change every second
         if(prand() > POISON_CHANGE_CHANCE)
		    poison_type = prand()%POISON_NUM_TYPES;
	  }

      if(roundtime < 60*SEC){//hurry up!
         if(roundtime == (60*SEC)-1){
		    TriggerFx(HURRY_FX,255,true);
            //first tick of quick time
		 }
		 DropBlocks();
      }
   
      if(!roundtime){//time up
         for(u8 i=0;i<numplayers;i++){
	        if(player_state[i] == DYING || player_attrb2[i] & GHOST)
		       continue;

	        if(!player_offset[i] && player_state[i] & MOVE){//wait until player isn't moving, then prevent further movement(avoid flicker at end of match)
		       player_state[i] ^= MOVE;
		       player_frame[i] = 0;
	      
		    }
		    else
		       KillPlayer(i);
	     }

      }
      else if(roundtime < 65535)//time left and not infinite time
         roundtime--;
      }

      FlashPowerUps();

   }
