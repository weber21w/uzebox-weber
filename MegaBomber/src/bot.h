bool IsBlocked(u8 x, u8 y){
   if( (vram[(y*30)+x]-RAM_TILES_COUNT) > 3)
      return true;
   return false;
}

u8 ColumnSafety(u8 x){
   return 0;
}

u8 RowSafety(u8 y){
   return 0;
}

u8 TileSafety(u8 x, u8 y){//return the relative safety of a tile position, higher is safer
   return 0;
}

int TileDesirability(u8 t){
   if(t >= FIRE_START && t <= FIRE_END)//fire is very undesirable!!
      return -127;
   
   if(IsSolid(t))
      return -126+(BOT_MAX_RANDOM_WEIGHT);
   
   //if((t <= BLOCK_END && t >= BLOCK_END))
     // return -127+(BOT_MAX_RANDOM_WEIGHT);//make sure random weights wouldn't make fire look better...
   
   //if(t >= WALL_START && t <= WALL_END)//cant walk through this, better than fire though!
     // return -127+(BOT_MAX_RANDOM_WEIGHT);

   if(t >= POWERUP_START && t <= POWERUP_END){//powerups are very nice!!
     // if(t >= POWERUP_START+20)//except poison!
	   //  return -(127-BOT_INITIAL_VARIANCE);
	  return 128;
   }

   if(t >= FLOOR_START && t <= FLOOR_END)
      return BOT_MAX_RANDOM_WEIGHT+100;

   return 0;
}

u8 SurroundingBlocks(u8 x, u8 y){//number of explodable blocks surrounding a position
   u8 count = 0;
   y *= 30;
   if(vram[y+x-2] == BLOCK_START+0)
      count++;
   if(vram[y+x+2] == BLOCK_START+0)
      count++;
   if(vram[y+x-60] == BLOCK_START+0)
      count++;
   if(vram[y+x+60] == BLOCK_START+0)
      count++;
   
   return count;
}

u8 BombDanger(u8 x, u8 y){
//   u8 t;
   for(u8 i=0;i<MAX_BOMBS;i++){
      if(bomb_owner[i] == 255 || (bomb_state[i] & THROWN) || bomb_timer[i] > 3*SEC ||
	     (bomb_x[i] != x && bomb_y[i] != y))
		    continue;

      if(bomb_x[i] == x && bomb_y[i] == y){
      
	  }

   return true;
   }
   return false;
}

bool IsTrapped(u8 x, u8 y){
   if(IsSolid(GetMapTile(x-2,y)) &&
      IsSolid(GetMapTile(x+2,y)) &&
	  IsSolid(GetMapTile(x,y-2)) &&
	  IsSolid(GetMapTile(x,y+2)))
      return true;

   return false;
}

bool PositionSafety(u8 x, u8 y){//return true if position is unsafe(a dangerous bomb will explode over this position)
   u8 t;
//   u8 low_x,high_x;
//   u8 low_y,high_y;

   for(u8 i=x;i<28;i++){
      
   }
   for(u8 i=0;i<MAX_BOMBS;i++){
      if(bomb_owner[i] == 255 || (bomb_state[i] & THROWN))
	     continue;
	  //if(bomb_timer[i] > BOT_DANGER_TIME)//due to chain explosions(which are to expensive to calculate)consider all bombs dangerous all the time
	     //continue;
    
	  if(abs(bomb_x[i]-x) < 2 && abs(bomb_y[i]-y) < 2)
	     return false;
      
	  if((bomb_x[i]-x) < 2){
	     if(bomb_y[i] < y){//bomb is above
			t = GetPlayerRange(bomb_owner[i])*2;
			for(u8 j=2;j<t;j+=2){//run the line down, seeing if its blocked
               if(IsSolid(GetMapTile(bomb_x[i],bomb_y[i]+j))){
				  j = 250;
				  continue;
				  //break;//no threat
               }
			   if(bomb_y[i]+j == y){
		//	      beep();
				  return false;//Not safe, this bomb endangers the position
			   }
			}
			continue;
		 }
	     else{//bomb is below
			t = GetPlayerRange(bomb_owner[i])*2;
			for(u8 j=2;j<t;j+=2){//run the line up, seeing if its blocked
               if(IsSolid(GetMapTile(bomb_x[i],bomb_y[i]-j))){
				  j = 250;
				  continue;
				  //break;//no threat
               }
			   if(bomb_y[i]-j == y){
		//	      beep();
				  return false;//Not safe, this bomb endangers the position
			   }
			}
			continue;
		 }
	  }
	  else if(bomb_y[i] == y){
	     if(bomb_x[i] < x){//bomb is left
			t = GetPlayerRange(bomb_owner[i])*2;
			for(u8 j=2;j<t;j+=2){//run the line right, seeing if its blocked
               if(IsSolid(GetMapTile(bomb_x[i]+j,bomb_y[i]))){
				  j = 250;
				  continue;
				  //break;//no threat
               }
			   if(bomb_x[i]+j == x){
	//		      beep();
				  return false;//Not safe, this bomb endangers the position
			   }
			}
			continue;
		 }
	     else{//bomb is right
			t = GetPlayerRange(bomb_owner[i])*2;
			for(u8 j=2;j<t;j+=2){//run the line left, seeing if its blocked
               if(IsSolid(GetMapTile(bomb_x[i]-j,bomb_y[i]))){
				  j = 250;
				  continue;
				  break;//no threat
               }
			   if(bomb_x[i]-j == x){
//			      beep();
				  return false;//Not safe, this bomb endangers the position
			   }
			}
			continue;
		 }
	  }
   }

   return true;
}

bool IsDangerous(u8 t){
   return false;
}

u8 CouldEscapeBomb(u8 i){
//return the direction the player could escape to
   u8 x = player_x[i];
   u8 y = player_y[i];
   u8 t;

   for(u8 i=y-2;i>1;i-=2){//up
      t = GetMapTile(x,i);
	  if(IsSolid(t) || IsDangerous(t))
	     break;
	  
	  t = GetMapTile(x-2,i);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_UP;//we could hide here

	  t = GetMapTile(x+2,i);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_UP;
	        
   }  

   for(u8 i=y+2;i<22;i+=2){//down
      t = GetMapTile(x,i);
	  if(IsSolid(t) || IsDangerous(t))
	     break;
	  
	  t = GetMapTile(x-2,i);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_DOWN;//we could hide here

	  t = GetMapTile(x+2,i);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_DOWN;
	        
   } 

   for(u8 i=x-2;i>1;i-=2){//left
      t = GetMapTile(i,y);
	  if(IsSolid(t) || IsDangerous(t))
	     break;
	  
	  t = GetMapTile(i,y-2);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_LEFT;//we could hide here

	  t = GetMapTile(i,y+2);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_LEFT;
	        
   }

   for(u8 i=x+2;i<28;i+=2){//right
      t = GetMapTile(i,y);
	  if(IsSolid(t) || IsDangerous(t))
	     break;
	  
	  t = GetMapTile(i,y-2);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_RIGHT;//we could hide here

	  t = GetMapTile(i,y+2);
	  if(!IsSolid(t) && !IsDangerous(t))
	     return BTN_RIGHT;
	        
   }

   return 0;
}



void CalculateTarget(u8 i){
   signed int dir[5];//up,down,left,right,center
//   u8 t;
   char c;

   bot_targetx[i] = player_x[i];
   bot_targety[i] = player_y[i];

 /*  if(prand() > BOT_WAIT_CHANCE-20){
      player_wait[i] = 30;
	  return;
   }
*/
   for(u8 d=0;d<4;d++)//set initial weights
      dir[d] = 128;
   
   if(IsTrapped(player_x[i],player_y[i]-2))
      dir[0] = -127;
   if(IsTrapped(player_x[i],player_y[i]+2))
      dir[1] = -127;
   if(IsTrapped(player_x[i]-2,player_y[i]))
      dir[2] = -127;
   if(IsTrapped(player_x[i]+2,player_y[i]))
      dir[3] = -127;
   /*
   t = player_state[i];
   if(t & UP)
      dir[0] += BOT_CONTINUE_PREFERENCE;
   else if(t & DOWN)
      dir[1] += BOT_CONTINUE_PREFERENCE;
   else if(t & LEFT)
      dir[2] += BOT_CONTINUE_PREFERENCE;
   else
      dir[3] += BOT_CONTINUE_PREFERENCE;
   */
   //Make walking into fire very undesirable...
   //Also make walking into solid wall impossible
   //Also make misc other things add weight to the decision
   //u8 t2,t3;
   //t3 = 1;

   c = TileDesirability(vram[((player_y[i]-2)*30)+player_x[i]]);
   dir[0] += c;
   if(player_y[i] < 21){
      c = TileDesirability(vram[((player_y[i]+2)*30)+player_x[i]]);
	  dir[1] += c;
   }else
      dir[1] = -128;
  
   c = TileDesirability(vram[((player_y[i])*30)+player_x[i]+2]);
	  dir[2] += c;
   c = TileDesirability(vram[((player_y[i])*30)+player_x[i]-2]);
	  dir[3] += c;
/*
   t2 = player_x[i];
   for(u8 k=player_y[i]-2;k > 1;k-=2){
      t = vram[(k*30)+t2]-RAM_TILES_COUNT;
	  t = TileDesirability(t);
	  if(t < -126){
	     k = 0;
		 continue;
		 }
      dir[0] += t;
	  //t3 *= 2;
   }
   
   for(u8 k=player_y[i]+2;k<22;k+=2){
      t = vram[(k*30)+t2]-RAM_TILES_COUNT;
	  t = TileDesirability(t);
	  if(t < -126){
	     k = 22;
		 continue;
		 }
      dir[1] += t;
	  //t3 *= 2;
   }

   t2 = player_y[i]*30;
   for(u8 k=player_x[i]-2;k > 1;k-=2){
      t = vram[k+t2]-RAM_TILES_COUNT;
	  t = TileDesirability(t);
	  if(t < -126){
	     k = 0;
		 continue;
		 }
      dir[2] += t;
	  //t3 *= 2;
   }
   
   for(u8 k=player_x[i]+2;k<27;k+=2){
      t = vram[k+t2]-RAM_TILES_COUNT;
	  t = TileDesirability(t);
	  if(t < -126){
	     k = 27;
		 continue;
		 }
      dir[3] += t;
	  //t3 *= 2;
   }
   */
   //When blocks are dropping, favor moves that might bring us to the center
 /*  if(roundtime < 60*SEC){
      if(player_x[i] < (VRAM_TILES_H/2)-1)
	     dir[3] += BOT_CENTER_WEIGHT;
      else if(player_x[i] > (VRAM_TILES_H/2))
	     dir[2] += BOT_CENTER_WEIGHT;
      
	  if(player_y[i] < (VRAM_TILES_V/2)-1)
	     dir[1] += BOT_CENTER_WEIGHT;
      else if(player_y[i] > (VRAM_TILES_V/2))
	     dir[0] += BOT_CENTER_WEIGHT;
   }
   */

   //Now add a little randomness to the weights, except where it is 0 (indicating TileDesirability found it was fire/wall)
   for(u8 d=0;d<4;d++){
     if(dir[d] < 1)//!dir[d])
	    dir[d] = -BOT_FIRE_HATRED;
     else
	    dir[d] += (prand()%BOT_INITIAL_VARIANCE);
   }

   if(!PositionSafety(player_x[i]+0,player_y[i]-2)){//up
      dir[1] += BOT_DANGER_WEIGHT;
      dir[2] += BOT_DANGER_WEIGHT;
      dir[3] += BOT_DANGER_WEIGHT;
   }
   if(!PositionSafety(player_x[i]+0,player_y[i]+2)){//down
      dir[0] += BOT_DANGER_WEIGHT;
      dir[2] += BOT_DANGER_WEIGHT;
      dir[3] += BOT_DANGER_WEIGHT;
   }
   if(!PositionSafety(player_x[i]-2,player_y[i]+0)){//left
      dir[0] += BOT_DANGER_WEIGHT;
      dir[1] += BOT_DANGER_WEIGHT;
      dir[3] += BOT_DANGER_WEIGHT;
   }
   if(!PositionSafety(player_x[i]+2,player_y[i]+0)){//right
      dir[0] += BOT_DANGER_WEIGHT;
      dir[1] += BOT_DANGER_WEIGHT;
      dir[2] += BOT_DANGER_WEIGHT;
   }
  /*
   //Now check for positions surrounded by explodable blocks, the more the better
   if(dir[0]){dir[0] += SurroundingBlocks(player_x[i]+0,player_y[i]-2)*BOT_BLOCK_VALUE;}
   if(dir[1]){dir[1] += SurroundingBlocks(player_x[i]+0,player_y[i]+2)*BOT_BLOCK_VALUE;}
   if(dir[2]){dir[2] += SurroundingBlocks(player_x[i]-2,player_y[i]+0)*BOT_BLOCK_VALUE;}
   if(dir[3]){dir[3] += SurroundingBlocks(player_x[i]+2,player_y[i]+0)*BOT_BLOCK_VALUE;}
*/
/*   
   if(player_state[i] & UP)       {dir[0] += BOT_CONTINUE_PREFERENCE;}
   else if(player_state[i] & DOWN){dir[1] += BOT_CONTINUE_PREFERENCE;}
   else if(player_state[i] & LEFT){dir[2] += BOT_CONTINUE_PREFERENCE;}
   else                           {dir[3] += BOT_CONTINUE_PREFERENCE;}
*/
   int highest = 0;//BOT_INITIAL_VARIANCE-1;//implement some waiting time if no obvious direction to go
   u8 pref = 4;
   //choose the direction with the highest weight
   for(u8 d=0;d<4;d++){
      if(dir[d] > highest){
	     highest = dir[d];
		 pref = d;
	  }
   }

   if(pref == 0){//up
    if(!PositionSafety(player_x[i],player_y[i]-2))//after all this, the direction we chose is unsafe. try again later...
	     return;
      bot_targety[i] -= 2;
   }
   else if(pref == 1){//down
      if(!PositionSafety(player_x[i],player_y[i]+2))
	     return;
      bot_targety[i] += 2;
   }
   else if(pref == 2){//left
      if(!PositionSafety(player_x[i]-2,player_y[i]))
	     return;
      bot_targetx[i] -= 2;
   }
   else if(pref == 3){//right
      if(!PositionSafety(player_x[i]+2,player_y[i]))
	     return;
      bot_targetx[i] += 2;
   }
   else{//pref = 4(default)no direction beat the threshold,dont move
//   beep();
     // bot_targetx[i] = player_x[i];
	 // bot_targety[i] = player_y[i];
   }
}

unsigned int GhostThink(u8 i){
   if(player_offset[i])
      return 0;

   u8 t,t2;

   if(bot_targetx[i] == 255){//not targeting a player, choose left top or right to throw from
      t = prand()&3;
	 // if(player_attrb2[t] & GHOST)//don't target ghosts, don't call too many rands() and implement a little wait
	   //  return;

	  bot_targetx[i] = t+10;
   }

   if(bot_targetx[i] > 9){//still moving to the "side" we will throw from
      t = bot_targetx[i]-10;
	  if(t == 0){//left
         if(player_x[i] > 11)
		    return BTN_LEFT;
         else
		    bot_targetx[i] -= 10;//switch to player target index
	  }
	  else if(t == 1){//top
         if(player_x[i] < 12)
		    return BTN_RIGHT;
         else if(player_x[i] > 26)
            return BTN_LEFT;
		 else
		    bot_targetx[i] -= 10;//switch to player target index
	  }
	  else{//right
         if(player_x[i] < 27)
		    return BTN_RIGHT;
		 else
		    bot_targetx[i] -= 10;//switch to player target index
	  }
   }
   

   if(player_x[i] < 12){//left side
	  t = player_y[bot_targetx[i]]/2;
	  t2 = (11-player_x[i]);
	  if(t < t2)
	     return BTN_RIGHT;
      else if(t > t2)
	     return BTN_LEFT;
      else{
	     bot_targetx[i] = 255;
		 return BTN_A;
      }
   }
   else if(player_x[i] < 27){//top
      t = player_x[bot_targetx[i]]/2;
	  t2 = (player_x[i]-12);
	  if(t < t2)
	     return BTN_LEFT;
      else if(t > t2)
	     return BTN_RIGHT;
      else{
	     bot_targetx[i] = 255;
		 return BTN_A;
      }
   }
   else{//right side
      t = player_y[bot_targety[i]]/2;
	  t2 = (player_x[i]-27);
	  if(t < t2)
	     return BTN_LEFT;
      else if(t > t2)
	     return BTN_RIGHT;
      else{
	     bot_targetx[i] = 255;
	     return BTN_A;
	  }
   }

   return 0;
}

unsigned int BotFillPad(u8 i){
return 0;
   if(player_attrb2[i] & GHOST)
	  return GhostThink(i); 


   if(player_state[i] == DYING){
      if(!(gamestate & ALLOWGHOSTS))
	     botgoal[i] = 0;

	  return 0;
   }
   
   if(gamestate & BOT_THOUGHT){//a bot has already had an expensive thought, no more than 1 per tick!!
      return 0;
   }   

   if(player_offset[i])//no need to think until we align
      return 0;

   if(!(player_x[i] & 1) && !(player_y[i] & 1))//we are aligned on map space, time to find a target
      bot_targetx[i] = 255;

   u8 t;
   
   if(false && player_bombs_out(i) < 1
   &&(t=CouldEscapeBomb(i))){
      // = SurroundingBlocks(player_x[i],player_y[i]);
//	  if(prand() < BOT_BOMB_CHANCE+(BOT_BOMB_BLOCK_WEIGHT*t2))
	     return t|BTN_A;
   }
   

   unsigned int t2=0;
//   static bool flop = false;

 //  if(prand() > BOT_BOMB_CHANCE && CouldEscapeBomb(i))//CouldEscapeBomb() is expensive, don't think about moving this tick
   //   return BTN_A;

setmove:
   if(bot_targetx[i] == 255){//need a new target
      CalculateTarget(i);
      goto setmove;
   }
   else if(bot_targetx[i] < player_x[i]){
      t2 = BTN_LEFT;
   }
   else if(bot_targetx[i] > player_x[i]){
      t2 = BTN_RIGHT;
   }
   else if(bot_targety[i] < player_y[i]){
      t2 = BTN_UP;
   }
   else if(bot_targety[i] > player_y[i]){
      t2 = BTN_DOWN;
   }
   else{
      player_state[i] ^= MOVE;
      bot_targetx[i] = 255;

   }

   return t2;
}
