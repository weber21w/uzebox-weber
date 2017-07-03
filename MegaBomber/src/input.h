extern bool StartDown(u8 i);
extern bool ADown(u8 i);
extern bool BDown(u8 i);
extern void KillPlayer(u8 p);
inline void FillPad(u8 i){
   oldpadstate[i] = padstate[i];
   
  if(i > 0){// if(botgoal[i]){
      padstate[i] = BotFillPad(i);
      
	 // if(multitap)//skip over this controller
	   //  ReadJoypad(i);
	   return;
   }

   if(!multitap)
      padstate[i] = ReadJoypad(i);
   else{//multitap code

   }
}

bool DetectMultitap(){
   multitap = false;
   return false;
}

void Input(){
   //if(guistate){return;}
   if(!roundtime)
      return;

   for(u8 i=0;i<numplayers;i++){
	  FillPad(i);

      if(player_state[i] == DYING)
	     continue;
 
      //if(StartDown(0)){guistate=GINGAMEMENU|GUIJUSTSTARTED;}

	  if(!player_offset[i]){
         if(poison_type == POISON_REVERSE && player_attrb[i] & POISON){//REVERSED CONTROLS
		    if(padstate[i] & BTN_UP)        {player_state[i] = MOVE|DOWN;}
            else if(padstate[i] & BTN_DOWN) {player_state[i] = MOVE|UP;}
            else if(padstate[i] & BTN_LEFT) {player_state[i] = MOVE|RIGHT;}
            else if(padstate[i] & BTN_RIGHT){player_state[i] = MOVE|LEFT;}
		    else if(padstate[i] & BTN_B)    {player_state[i] |= GRAB;}
		 }
		 else{
		    if(padstate[i] & BTN_UP)        {player_state[i] = MOVE|UP;}
            else if(padstate[i] & BTN_DOWN) {player_state[i] = MOVE|DOWN;}
            else if(padstate[i] & BTN_LEFT) {player_state[i] = MOVE|LEFT;}
            else if(padstate[i] & BTN_RIGHT){player_state[i] = MOVE|RIGHT;}
		    else if(padstate[i] & BTN_B)    {player_state[i] |= GRAB;}
         }
	  }      
      
	  if(poison_type == POISON_AUTOBOMB && player_attrb[i] & POISON && !(oldpadstate[i] & BTN_A))
	     padstate[i] |= BTN_A;

	  if(padstate[i] & BTN_A && !(oldpadstate[i] & BTN_A))         {player_state[i] |= BOMB;}
	  if(padstate[i] & BTN_B && !(oldpadstate[i] & BTN_B)){
      //for(u8 w=1;w<4;w++)
	  //KillPlayer(w);
//player_state[1] = player_state[2] = player_state[3] = DYING;
//DispersePowerups(0);
//player_state[0] = UP;

	  //player_x[0] = player_y[0] = 2;
	  //player_state[0] = 0;
	  player_frame[0] = player_offset[0] = 0;
	  player_ftime[0] = 0;
	  //player_attrb2[i] ^= GHOST;
	  player_attrb[0] |= POISON;
	 // poison_type = POISON_FAST;
	  //player_attrb2[0] |= 0b01100000;
	  roundtime = 59*SEC;


		}
   }
   
   if(padstate[0] & BTN_START && !(oldpadstate[0] & BTN_START) && !(gamestate & VICTORY))
      InGameMenu();
}

