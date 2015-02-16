//PROBLEM HERE MUST BE WITH CRASHBARDOWN COPYING MAGIC JEWEL;

void SetScoringSimilar(uint8_t p, uint8_t t){//used by magic jewel to clear the color it touches from well
if(t > 6){//hit crash bar TODO MAKE SURE IT STILL DELETES THE MAGIC JEWEL
return;
	}		
for(uint16_t i=p*WELLSIZE;i<(p*WELLSIZE)+WELLSIZE;i++)
if(well[i] == t)//shouldn't need to &0b00111111 since nothing is half step and doesnt matter if scoring?
well[i] |= SCORINGJEWEL;
}

inline void MirrorWell(uint8_t p){
	uint16_t welloff = p*WELLSIZE;
	uint8_t t;
	for(uint8_t y=0;y<16;y++){
		t = well[welloff+0];
		well[welloff+0] = well[welloff+5];
		well[welloff+5] = t;

		t = well[welloff+1];
		well[welloff+1] = well[welloff+4];
		well[welloff+4] = t;
		
		t = well[welloff+2];
		well[welloff+2] = well[welloff+3];
		well[welloff+3] = t;
		
		welloff += 6;
	}
}

inline void MovePieceDown(uint8_t p){
#ifdef DEBUG
if(!p)return;
#endif
	blank_piece(p);
	if((piecey[p]<13) && (!get_well(piecex[p],piecey[p]+3,p))){//can move down
		if(halfstep[p]){
			halfstep[p] = false;
			piecey[p]++;
		}
		else
			halfstep[p] = true;
	add_piece(p);
	return;
	}
	//else move down failed
	add_piece(p);
	wellfullness[p] += 3;

	if((piece[(p*3)+2]&0x0F) > 6){//is it a magic jewel? TODO - NO POINTS CAN BE SCORED FROM MAGIC JEWEL EVEN THE CHAINS RESULTING FROM IT!!!!!!!!!
		uint8_t t = piece[(p*3)+2]&0x0f;

		for(uint8_t i=0;i<3;i++)
			set_scoring(piecex[p],piecey[p]+i,p);

		if(t == 7){//up arrow
		//Columns 3 the amount is 2 up or 2 down for arrows
			crashcomingup[!p] += 2;
		}
		else if(t == 8){//delete other jewels of same color
			chain[p] = 1;//make the sound right, no score from magic jewel
			wasmagicjewel[p] = 1;//make sure we dont score for this
			if(piecey[p] < 13)//didn't hit floor
				SetScoringSimilar(p,get_well(piecex[p],piecey[p]+3,p));
		}
		else{//down
			if(crashheight[p] == 1)
				crashcomingdown[p]++;
			else if(crashheight[p])//>1
				crashcomingdown[p] += 2;
		}
		state[p] = SCORING;
		statetimer[p] = SCOREFLASHTIME;
		stateframe[p] = 0;
		magicjewelsonscreen--;
		goto MOVEPIECEDOWNBOTTOM;
	}
		//else not magic jewel

	//HACK HACK HACK THIS IS DONE IN 2 PLACES?????
	if(CheckScore(p)){
		//chain[p]++;
		state[p] = SCORING;
		statetimer[p] = SCOREFLASHTIME;
		stateframe[p] = 0;
	}
	else{
		chain[p] = 0;
		
	//	else
		//	winner = 255;
	}
	
	if(CheckLoss(p)){//see if there are any jewels in the invisible top 3 rows, if so lose
		SetLosing(p);
	}
	
MOVEPIECEDOWNBOTTOM:
		TriggerPCM(DROPWAVE,255,138);
	if((cursetype[p]&CURSESWAPWELL))//SWAP JEWELS AROUND FOR CURSE(TODO MAGIC JEWEL GLITCHES)
		MirrorWell(p);
	GeneratePreview(p);//set the next piece in motion
}

inline void ProcessCrash(uint8_t p){
	blank_piece(p);
	
	//TODO SUBTRACT THE TWO FIRST...
	if(crashcomingup[p]){
		if(crashcomingdown[p] > crashcomingup[p]){
			crashcomingdown[p] -= crashcomingup[p];
			crashcomingup[p] = 0;
		}
		else if(crashcomingup[p] > crashcomingdown[p]){
			crashcomingup[p] -= crashcomingdown[p];
			crashcomingdown[p] = 0;
		}
		else{
			crashcomingup[p] = crashcomingdown[p] = 0;
		}
	}
	
	if(crashheight[p] && crashcomingdown[p]){
		CrashBarDown(crashcomingdown[p],p,0);
		crashcomingdown[p] = 0;
		add_piece(p);
	}
	if(crashcomingup[p]){
		for(uint8_t i=0;i<3;i++){
			if(piece[i+(p*3)] & FLASHINGJEWEL){//eliminate flashing jewels correctly
				piece[i+(p*3)] ^= FLASHINGJEWEL;
				flashingjeweltimer[p] = 0;
			}
		}
		SetCrashedPieces(p);
		blank_piece(p);
		GeneratePreview(p);
		CrashBarUp(crashcomingup[p],p);
		crashcomingup[p] = 0;
		if(CheckLoss(p))
			SetLosing(p);
	}
	add_piece(p);		
}


uint8_t CheckLoss(uint8_t p){
	if(state[p] & SCORING)
		return false;
	//if(CheckScore(p))
		//return false;
	//TODO WHAT ABOUT MAGIC JEWEL COMING TO REST HERE?????
	uint8_t off = p*WELLSIZE;
	for(uint8_t i=0;i<3*6;i++){
	/*	if(well[off]){
			if(well[off]&SCORINGJEWEL)
				return false;//need to finish scoring before we determine loss
			return true;
		}			
		off++;*/
		if(well[off++])
			return true;
	}
	return false;
}



void SummonMagicJewel(uint8_t p){
	TriggerPCM(MAGICWAVE,23,255);
	magicjewelssummoned[p]++;
	preview[(p*3)+0] = 7;
	preview[(p*3)+1] = 8;
	preview[(p*3)+2] = 9;
	magicjewelsonscreen++;
}

void SummonCurse(uint8_t p){
	TriggerPCM(CRASHWAVE,28,255);
	uint8_t t = 1;
	t <<= prng(MAX_PLAYERS)%6;
	cursetype[p] |= t;
	cursetime[p] = 12*30;
}

uint16_t CheckScore(uint8_t p){
	uint16_t total = 0;
	uint8_t t0,t1,t2,t3,t4;
	uint8_t off;
	// CHECK VERTICAL
	for(uint8_t x=0;x<6;x++) {
		off = (p*WELLSIZE)+x;
	    for(uint8_t y=1; y<15-crashheight[p];y++){
			off += 6;
			t1 = (well[off]&7);//get_well_no_scoring(x,y,p);
			if(!t1)
				continue;
			t0 = (well[off-6]&7);//get_well_no_scoring(x,y-1,p);
			t2 = (well[off+6]&7);//get_well_no_scoring(x,y+1,p);

			if(t1==t0 && t1==t2){
				well[off+6] |= SCORINGJEWEL;//set_scoring(x,y-1,p);
				well[off+0] |= SCORINGJEWEL;//set_scoring(x,y,p);
				well[off-6] |= SCORINGJEWEL;//set_scoring(x,y+1,p);
				total+=3;
			}
	    }
	}
	
	// CHECK HORIZONTAL
	/*for(uint8_t x=1;x<5;x++) {
	    for(uint8_t y=0; y<16-crashheight[p];y++){
			t1 = get_well_no_scoring(x,y,p);
			if(!t1)
				continue;
			t0 = get_well_no_scoring(x-1,y,p);
			t2 = get_well_no_scoring(x+1,y,p);

			if(t1==t0 && t1==t2){
				set_scoring(x-1,y,p);
				set_scoring(x,y,p);
				set_scoring(x+1,y,p);
				total+=3;
			}
	    }
	}*/
	off = (p*WELLSIZE);
	for(uint8_t y=0;y<(16-crashheight[p]);y++){
		for(uint8_t x=0;x<4;x++){
			t1 = (well[off++]&7);
			if(!t1)
				continue;
			t0 = (well[off++]&7);
			t2 = (well[off]&7);
			if(t1 == t0 && t1 == t2){
				well[off+0] |= SCORINGJEWEL;
				well[--off] |= SCORINGJEWEL;
				well[off-1] |= SCORINGJEWEL;
				total += 3;
			}
			else
				--off;
			
		}
		off += 2;
	}
	
	
/*
	// CHECK DIAGONALS
	for(uint8_t x=1;x<5;x++) {
		for(uint8_t y=1;y<15-crashheight[p];y++) {
			t0 = get_well_no_scoring(x,y,p);
			if(!t0)
				continue;
			t1 = get_well_no_scoring(x-1,y+1,p);
			t2 = get_well_no_scoring(x+1,y-1,p);
			t3 = get_well_no_scoring(x-1,y-1,p);
			t4 = get_well_no_scoring(x+1,y+1,p);
			
			if(t0==t1 && t0==t2){
				set_scoring(x,y,p);
				set_scoring(x-1,y+1,p);
				set_scoring(x+1,y-1,p);
				total+=3;
			}
			if(t0==t3 && t0==t4){
				set_scoring(x,y,p);
				set_scoring(x-1,y-1,p);
				set_scoring(x+1,y+1,p);
				total+=3;
			}
		}
	}
*/

	// CHECK DIAGONALS
	//off = (p*WELLSIZE)+1+6;
	for(uint8_t x=1;x<5;x++) {
		for(uint8_t y=1;y<15-crashheight[p];y++) {
			off = (p*WELLSIZE)+(y*6)+x;
			if(!(t0 = (well[off]&7)))//(x,y,p);
				continue;
				
			t1 = (well[(off-1)+6]&7);//get_well_no_scoring(x-1,y+1,p);
			t2 = (well[(off+1)-6]&7);//get_well_no_scoring(x+1,y-1,p);
			t3 = (well[(off-1)-6]&7);//get_well_no_scoring(x-1,y-1,p);
			t4 = (well[(off+1)+6]&7);//get_well_no_scoring(x+1,y+1,p);
			
			if(t0==t1 && t0==t2){
				well[(off+0)+0] |= SCORINGJEWEL;//set_scoring(x,y,p);
				well[(off-1)+6] |= SCORINGJEWEL;//set_scoring(x-1,y+1,p);
				well[(off+1)-6] |= SCORINGJEWEL;//set_scoring(x+1,y-1,p);
				total+=3;
			}
			if(t0==t3 && t0==t4){
				well[(off+0)+0] |= SCORINGJEWEL;//set_scoring(x,y,p);
				well[(off-1)-6] |= SCORINGJEWEL;//set_scoring(x-1,y-1,p);
				well[(off+1)+6] |= SCORINGJEWEL;//set_scoring(x+1,y+1,p);
				total+=3;
			}
		}
	}
	
	if(!total)
		return 0;
		
	if(!wasmagicjewel[p]){
		chain[p]++;
		total *= chain[p];
		if(score[p]+total > 30)//I guess we will just update the score here...
			score[p] = 30;
		else
			score[p] += total;
		
		jewels[p] += total/3;
		
		if(jewels[p]/20 > magicjewelssummoned[p])
			SummonMagicJewel(p);

		//see if we eliminated a flashing jewel
		if(flashingjeweltimer[p] && !(FlashingJewelInWell(p))){
			flashingjeweltimer[p] = 0;
			SummonCurse(!p);
		}			

		return total;
	}
	else{	
		return total;
	}
}

void DropPiece(uint8_t p){//MovePieceDown will still run once after this, so no checks needed
	blank_piece(p);
	uint8_t off = (p*WELLSIZE)+piecex[p];
	uint8_t i;
	for(i=0;i<16;i++){
		if(well[off]){
			if(i < 3){//will be loss
				add_piece(p);
				//WaitVsync(120);
				return;
			}				
			break;
		}	
		off += 6;
	}
	piecey[p] = i-3;
	halfstep[p] = 0;
	droptimer[p] = 0;
	add_piece(p);
}



void ClearTopRows(uint8_t p){
	for(uint8_t i=0;i<3*6;i++)
		well[(p*WELLSIZE)+i] = 0;
}

inline bool MovePieceLeft(uint8_t p){
	if(!piecex[p] || !open_for_piece(piecex[p]-1,piecey[p],p)){
		if(p && cpustate)
			cpustate |= CPUMOVEBLOCKED;
		return false;
	}		
//	blank_piece(p);piece is blanked before function is called
	piecex[p]--;
	return true;
}

inline bool MovePieceRight(uint8_t p){
	if(piecex[p] == 5 || !open_for_piece(piecex[p]+1,piecey[p],p)){
		if(p && cpustate)
			cpustate |= CPUMOVEBLOCKED;
		return false;
	}		
//	blank_piece(p);piece is blanked before function is called
	piecex[p]++;
	return true;
}

inline void RotatePiece(uint8_t p){
	uint8_t t = piece[(p*3)];
	piece[(p*3)] = piece[(p*3)+2];
	piece[(p*3)+2] = piece[(p*3)+1];
	piece[(p*3)+1] = t; 
	if(++piecerot[p] > 2)
		piecerot[p] = 0;
}

inline void Attack(uint8_t target){
	uint8_t p = !target;
	uint8_t s = score[p]/10;
	
	if(crashheight[p]){
		if(crashheight[p] >= s){
			crashcomingdown[p] += s;
			s = 0;
		}else{//crashheight is less
			crashcomingdown[p] = crashheight[p];
			s -= crashheight[p];
		}
	}
	if(s)
		crashcomingup[target] += s;
		
	score[p] = score[p]%10;
}	

uint8_t CrashBarUp(uint8_t steps, uint8_t target){
	TriggerPCM(CRASHWAVE,23,150);
	//need to check extended loss now, because otherwise we simply lose jewels that would be above
	uint8_t off = target*WELLSIZE;
		for(uint8_t i=0;i<6*(3+steps);i++){
			if(well[off++]){
				SetLosing(target);
				//return steps;
			}
		}
	
	if((crashheight[target]+steps)>13){		
		steps = 13-crashheight[target];
	}		

	crashheight[target] += steps;

	if(!(state[target] & (LOSING|LOSING2|GAMEOVER))){
		//blank_piece(target);
//SetCrashedPieces(target);
		//GeneratePreview(target);
	}
//	WaitVsync(1);
	uint8_t t;
	off = 3*6;
	for(uint8_t y=3;y<16;y++)
	for(uint8_t x=0;x<6;x++){
		t = get_well(x,y,target);
		if(!t)
			continue;
		set_well(x,y-steps,target,t);	
		//set_well(x,y,target,c--);

	}
	bool flop = target;
	uint8_t c = 20;
	for(uint8_t y=16-crashheight[target];y<16;y++){
		for(uint8_t x=0;x<6;x++){
			set_well(x,y,target,c+flop);
			flop = !flop;
		}
		flop = !flop;
	}

	WaitVsync(1);
	
	wellfullness[target] += 6*steps;
	return steps;
}

uint8_t CrashBarDown(uint8_t steps, uint8_t target, uint8_t silent){
//TODO NOT WORKING RIGHT
	if(crashheight[target] < steps)
		steps = crashheight[target];
		
	if(!steps)
		return 0;

	if(!silent)
		TriggerPCM(CRASHWAVE,23,150);
		
	crashheight[target] -= steps;

	uint8_t t;
//	uint8_t off = (target*WELLSIZE);
	for(uint8_t y=15-steps;y<255;y--)
	for(uint8_t x=0;x<6;x++){
		t = get_well(x,y,target);
		set_well(x,y+steps,target,t);
		set_well(x,y,target,0);	
	}

	wellfullness[target] -= 6*steps;
	return steps;
}

inline bool ApplyGravity(uint8_t p){

	//start from the top of the well
	bool moved,hasmoved;
	hasmoved = false;
	register uint8_t off;
	register uint8_t t;
/*	for(uint8_t x=0;x<6||x==255;x++){
		moved = false;
		for(uint8_t y=0;y<15-crashheight[p];y++){
			uint8_t t = get_well(x,y,p);
			if(!t)
				continue;
			uint8_t t2 = get_well(x,y+1,p);//check below
			if(t2)//solid so check further down
				continue;
			//we are here so we have a block to drop
			moved = true;
			hasmoved = true;
			if(t & HALFSTEP){
				t ^= HALFSTEP;
				set_well(x,y+1,p,t);
				set_well(x,y,p,0);
			}
			else{
				t |= HALFSTEP;
				set_well(x,y,p,t);
			}
		//	break;//start the loop over with the updated position
		}
		//if(moved)
		//	x--;//do this column again
	}
	*/

	for(uint8_t x=0;x<6;x++){
		moved = false;
		off = (x+(p*WELLSIZE))-6;
		//off -= 6;
		for(uint8_t y=0;y<15-crashheight[p];y++){
			//off = ((y*6)+x)+(p*WELLSIZE);
			off += 6;
			if(!(t = well[off]))//get_well(x,y,p);//if(!t)	
				continue;
			//t2 = well[off+6];
			if(well[off+6]){//get_well(x,y+1,p);//check below
			
				continue;//solid so check further down
			}				

			//we are here so we have a block to drop
			moved = true;
			hasmoved = true;
			if(t & HALFSTEP){
				t ^= HALFSTEP;
				well[off+6] = t;//set_well(x,y+1,p,t);
				well[off+0] = 0;//set_well(x,y,p,0);
			}
			else{
				t |= HALFSTEP;
				well[off] = t;//set_well(x,y,p,t);
			}
		//	break;//start the loop over with the updated position
		}
		//if(moved)
		//	x--;//do this column again
	}

	return hasmoved;
}

bool FlashingJewelInWell(uint8_t p){
	uint8_t off = p*WELLSIZE;
	for(uint8_t i=0;i<WELLSIZE;i++){
		if((well[off++] & FLASHINGJEWEL))
			if(!(well[off-1] & SCORINGJEWEL))//will be eliminated
				return true;
	}
	return false;
}
 void EliminateFlashingJewel(uint8_t p){
	uint8_t off = p*WELLSIZE;
	for(uint8_t i=0;i<WELLSIZE;i++){
		if(well[off++] & FLASHINGJEWEL){
			well[--off] ^= FLASHINGJEWEL;
			return;
		}
	}		
}

void GeneratePreview(uint8_t p){//makes the preview piece the current piece, and generates a new one
//COLUMNS 3!! ONCE A PIECE IS DONE SETTING, IT IS 32 TICKS BEFORE YOU CAN SEE THE BOTTOM HALF of the bottom jewel on screen
	if(p == 0 && (game_state & GS_DEMO)){
		demo_inp_wait = pgm_read_byte(&DemoData[demo_off++]);
		demo_decision = pgm_read_byte(&DemoData[demo_off++]);
	}
	if(preview[(p*3)] != 7){//don't want a flashing magic jewel
		if(cursetype[p] & FLASHINGJEWELCOMING){
			if(flashingjeweltimer[p])
				EliminateFlashingJewel(p);
			flashingjeweltimer[p] = 3+(prng(MAX_PLAYERS)%5);
			cursetype[p] &= 127;
			preview[(p*3)+(prng(MAX_PLAYERS)%3)] |= FLASHINGJEWEL;
		}
		else if(flashingjeweltimer[p]){
			if(!(--flashingjeweltimer[p]))
				EliminateFlashingJewel(p);
		}	
	}	
	for(uint8_t i=0;i<3;i++)
		piece[(p*3)+i] = preview[(p*3)+i];
	
	for(uint8_t i=0;i<3;i++)
		preview[(p*3)+i] = 1+(prng(p)%6);

	piecey[p] = 0;
	piecex[p] = 2;
	piecerot[p] = 0;
	if(p == 1 && cpustate)
		cpustate = CPUFIRSTTICK;//let cpu know next move has begun
//	downletup[p] = 0;
}

inline void ClearScoredJewels(uint8_t p){
	for(uint8_t i=p*WELLSIZE;i<(p*WELLSIZE)+WELLSIZE;i++){
		if(well[i] & SCORINGJEWEL){
			well[i] = 0;
			wellfullness[p]--;
		}			
	}
}



inline void SetCrashedPieces(uint8_t p){
	//flashing jewel flag is already eliminated by the time we get here
	crashedredrawtime = 45;//longest possible number of ticks fallings pieces could be over drawing things. after that save some cycles for cpu player
	for(uint8_t i=p*3;i<(p*3)+3;i++){
		crashedpiecex[i] = (piecex[p]<<4)+(p*(16<<3));
		crashedpiecey[i] = ((((piecey[p]-2)<<1)+((bool)halfstep[p]))<<3)+((i-(p*3))*2);
		
		crashedxvel[i] = -7+(prng(MAX_PLAYERS)%14);
		crashedyvel[i] = -14+(prng(MAX_PLAYERS)%20);
		if((piece[i]) < 7)//normal jewel
			crashedtype[i] = (JEWELSTART+((piece[i])*4)-1)+RAM_TILES_COUNT;
		else//magic jewel
			crashedtype[i] = ((piece[i])-7)*4;
	}
}

inline void UpdateCrashedPieces(){
	//x/y values most significant 5 bits are used as tile coords, lowest 3 bits are used for subposition
	//TODO CRASHED MAGIC JEWELS DONT WORK RIGHT
	if(!crashedredrawtime)
		return;
	for(uint8_t i=0;i<MAX_PLAYERS*3;i++){
		if(crashedtype[i] == 255)//not active
			continue;
		
		//apply velocity
		crashedpiecex[i] += crashedxvel[i];
		crashedpiecey[i] += crashedyvel[i];
		crashedyvel[i] += 2;
		
		if(crashedpiecex[i] > (26<<3)){
			crashedtype[i] = 255;//disabled
			continue;
		}
		if(crashedpiecey[i] > (26<<3))
			continue;
		
		uint8_t x = crashedpiecex[i]>>3;
		uint8_t y = crashedpiecey[i]>>3;
		uint8_t t = crashedtype[i];
		set_vram(x+0,y+0,t++);
		set_vram(x+1,y+0,t++);
		set_vram(x+0,y+1,t++);
		set_vram(x+1,y+1,t++);
	}
	crashedredrawtime--;
}

inline void UpdateTimers(){
//HANDLE TIMERS AND PRIORITIES FOR PCM CHANNEL
		if(pcm_timeleft)
			pcm_timeleft--;

		if(globalflashcounter)
			globalflashcounter--;
		else
			globalflashcounter = GLOBALFLASHCOUNT;

		flashcount++;
		
		if(flashcount > 12)//TODO  / 2
			flashcount = 7;

		if(magicjewelsonscreen){
			if(!magiccolorwait){
				magiccolorwait = 6/2;
			if(magiccolor < 5)
				magiccolor++;
			else
				magiccolor = 0;
			ramtilestate[0] |= NEEDMAGICRAMMED;
		}
		else
			magiccolorwait--;
		}

		if((game_state & GS_DEMO) && ++demotime > DEMOLENGTH){
			demotime = 0;
			game_state = GS_TITLE;
			return;
		}

}

void SetLosing(uint8_t p){
	cursetype[p] = 0;
	cursetime[p] = 0;
	ramtilestate[p] = NEEDGRIDRAMMED;
	if(state[p] & (LOSING|LOSING2))
		return;

	//	clear_well(p);
	if(winner == 255)//means other player didn't lose
		winner = !p;
	else if(winner == p){//otherwise other player lose too so draw
		winner = 255;
		return;
	}		
	state[p] = LOSING;
	statetimer[p] = LOSINGWAITTICKS;
	WaitVsync(2);
	TriggerFx(40,255,false);//trigger losing patch duet
	TriggerFx(41,255,false);
	return;
}