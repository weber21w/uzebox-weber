#include "cpumisc.h"

uint16_t cpulastrunscore = 0;
uint8_t cpulastevalchange = 5;
bool cpulastchangeddir;
#ifdef DEBUG
/*uint8_t lastevalweights[25];//={4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,};
uint8_t evalweights[25];// = {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,};

void AdjustEvalWeights(){
	WaitVsync(1);
	if(jewels[1] < cpulastrunscore){//revert back
		for(uint8_t i=0;i<24;i++)
			evalweights[i] = lastevalweights[i];
	}
	else{
		cpulastrunscore = jewels[1];
		for(uint8_t i=0;i<24;i++)
			lastevalweights[i] = evalweights[i];
	}
//reroll:
//	cpulastevalchange = prng(MAX_PLAYERS)&25;
	
//	if(prng(MAX_PLAYERS)&1)
		evalweights[(prng(MAX_PLAYERS)&25)]++;
	//else if(evalweights[cpulastevalchange])
//		evalweights[cpulastevalchange]--;
//	else goto reroll;
}*/
#endif
inline bool CpuBufferMove(uint8_t m, bool prvw){
	uint8_t c = m/3;
	uint8_t r = m%3;
	bool ret = true;
	//how high are the jewels in this column?
	//we want to set the jewel down without slowing CpuScoreAndGravity()
	uint8_t h = buffer_column_height(c);
	if(h < 6 || (h<9 && c == 2)){//HACK WAS h<6 losing move
		////TODO TAKE INTO CONSIDERATION SCORING
		//if(prvw && ((preview[(1*3)+0] == preview[(1*3)+1]) && (preview[(1*3)+1] == preview[(1*3)+2]))){}//not all 1 color so will be a loss
		//else if(!prvw && ((piece[(1*3)+0] == piece[(1*3)+1]) && (piece[(1*3)+1] == piece[(1*3)+2]))){}
		//else
		ret = false;//	return false;
	}		
//NOW JUST PUT PIECE AT TOP AND LET CPUSCOREANDGRAVITY() DO THE REST
	uint8_t offset = (((h-3)*6)+c);
	uint8_t pt[3];
	
	if(!prvw){//not the preview piece
		for(uint8_t i=0;i<3;i++)
			pt[i] = piece[(1*3)+i];
	}
	else{//the preview piece
		for(uint8_t i=0;i<3;i++)
			pt[i] = preview[(1*3)+i];
	}
	
//	if(pt[0] == pt[1] && pt[1] == pt[2])//don't need consideration
//		return false;
	for(uint8_t i=0;i<r;i++){
		uint8_t t = pt[2];
		pt[2] = pt[1];
		pt[1] = pt[0];
		pt[0] = t;
	}
	
	cpulastbufferoff = offset;
	cpubuffer[offset] = pt[0];
	offset += 6;
	cpubuffer[offset] = pt[1];
	offset += 6;
	cpubuffer[offset] = pt[2];
	
	return ret;
}


inline uint16_t CpuEmergencyEvaluate(){
	uint16_t score = 0;
	uint16_t off = (WELLSIZE-6);
	uint8_t t;

	for(uint8_t y=15;y>2+2;y--){
		for(uint8_t x=0;x<5;x++){
			if(!(t = cpubuffer[off++]))
				continue;
			
			if(t == cpubuffer[off+1]){
				score += 1;
				//if(x<3 && cpubuffer[off+3])
				//score += 2;
			}
			
			if(x < 4 && t == cpubuffer[off+2]){
				if(cpubuffer[off+1] == 0)
				score += 1;
			}
			
			if(x < 4 && t == cpubuffer[off-5]){//above right
			score+=1;
			
		}
		if(x > 1 && t == cpubuffer[off-7]){//above left
		score += 1;
	}
	
	if(t == cpubuffer[off-6]){
		score+=1;
		//if(cpubuffer[off-12] == 0 || cpubuffer[off-18] == t)
		//score += 3;
		//else{
			////if(cpubuffer[off-12] == cpubuffer[off-17])
			//		score += 4;
		//	}
	}
	off++;
}//x
off -= 11;
	}//y
	
	return score;
}	

inline int32_t CpuEvaluate(){
	uint16_t score = 1000;

	uint8_t t;
/*
{-20}{-19}{-18}{-17}{-16}{-15}
{-14}{-13}{-12}{-11}{-10}{ -9}
{ -8}{ -7}{ -6}{ -5}{ -4}{ -3}
{ -2}{ -1}{ X }{ +1}{ +2}{ +3}

*/
	uint16_t off = (3*6)+1;	
	for(uint8_t y=3;y<15;y++){
		for(uint8_t x=1;x<5;x++){
			/*if(padstate[0] & BTN_A){
				set_vram(18+(x*2),(y-3)*2,111);
				WaitVsync(2);
			}*/
			t = cpubuffer[off++];
			if(!t){
				score++;
				continue;
			}				
			if(t == cpubuffer[off-2])//left
				score += 2;
			else if(cpubuffer[off-2])
				score -= 1;
			
			if(t == cpubuffer[off])//right
				score += 2;
			else if(cpubuffer[off])
				score -= 1;
			
			if(t == cpubuffer[off-7])//up
				score += 2;
			else if(cpubuffer[off-7])
				score -= 1;
			
			if(t == cpubuffer[off+5])//down
				score += 2;
			else if(cpubuffer[off+5])
				score -= 1;

			if(t == cpubuffer[off-8])//up left
				score += 3;
			else if(cpubuffer[off-2])
				score -= 1;
			
			if(t == cpubuffer[off-6])//up right
				score += 3;
			else if(cpubuffer[off-6])
				score -= 1;

	/*		if(t == cpubuffer[off+4])//down left
				score += 2;
			else if(cpubuffer[off+4])
				score -= 1;
			
			if(t == cpubuffer[off+6])//down right
				score += 2;
			else if(cpubuffer[off+6])
				score -= 1;
	*/
		}//x
		off += 1;
	}//y
	
	if(score > 1000)
		score -= 1000;
	else
		score = 0;
	return score;
}

inline int32_t CpuEvaluateHeight(){
//	TriggerFx(0,255,true);
	int16_t total = 0;
	for(uint8_t i=0;i<16*6;i++){
		if(cpubuffer[i])
			return (int32_t)(10*(i/6));
	}
	return 16*10;			
	/*for(uint8_t i=3*6;i<(12+(score[0]/10))*6;i++){	
		if(!cpubuffer[i])
			total+=(32-(i/6));
	}
	*/
	/*
	for(uint8_t i=15*6;i<16*6;i++)
		if(!cpubuffer[i]){
			total += 10*cpuevalmultiplier*cpuscoremultiplier;
			break;
		}*/
//	for(uint8_t i=20;i<12*6;i+=6)
	//	if(!cpubuffer[i])
		//	total = (64)-(i/6);		
		
	return total*cpuevalmultiplier;
}

inline int32_t CpuScoreAndGravity(uint8_t c){//MOST SPEED CRITICAL SECTION OF THE GAME!!!!!!!!!!!!!!!!
	register uint8_t off;
	uint8_t t;
	uint8_t overall = 0;//uint16_t overall = 0;
	uint8_t chain = 0;
	uint8_t total = 0;//uint16_t total = 0;
//TODO HAVE VERY FAST INITIAL VERSION THAT CHECKS EXACTLY WHERE THE JEWEL LANDED
//CHECKS FOR ANY SCORES IF SO RUNS THIS FULL VERSION!!!! 
	//off
	bool quickcheck = true;
	off = cpulastbufferoff;
/*	ColumnsPrint3num(15,14,off);
	ColumnsPrint2num(15,15,cpumovepos[0]);
	ColumnsPrint2num(15,16,cpumovepos[1]);
	WaitVsync(60);
*/
/*
	for(uint8_t i=0;i<3;i++){
		t = cpubuffer[off];
		if(t == cpubuffer[off-6]){//1 above
			if(t == cpubuffer[off-12] || t == cpubuffer[off+6])//2 above or 1 below
				goto CPUFULLSCORECHECK;
		}
		if(t == cpubuffer[off+6] && t == cpubuffer[off+12])//1 below and 2 below
			goto CPUFULLSCORECHECK;
			
			
		if(t == cpubuffer[off-1]){//1 left
			if(t == cpubuffer[off-2] || t == cpubuffer[off+1])//2 left or 1 right
				goto CPUFULLSCORECHECK;
		}
		if(t == cpubuffer[off+1] && t == cpubuffer[off+2])//1 right and 2 right
			goto CPUFULLSCORECHECK;
			
			
		if((t == cpubuffer[off-5])){//1 above right
			if((t == cpubuffer[off-10]) || (t == cpubuffer[off+7]))//2 above right or 1 below left
				goto CPUFULLSCORECHECK;
		}
		if(t == cpubuffer[off+7] && t == cpubuffer[off+14])//1 below left and 2 below left
			goto CPUFULLSCORECHECK;
			
		if(t == cpubuffer[off-7]){//1 above left
			if(t == cpubuffer[off-14] || t == cpubuffer[off+5])//2 above left or 1 below right
				goto CPUFULLSCORECHECK;
		}
		if(t == cpubuffer[off+5] && t == cpubuffer[off+10])//1 below right and 2 below right
			goto CPUFULLSCORECHECK;
		off += 6;
	}
			

//	return 0;

//	uint8_t t0,t1;//,t2,t3,t4;
	//register uint8_t off;
goto CPUSCORETOP;
CPUFULLSCORECHECK:
quickcheck = true;
*/
CPUSCORETOP:
//TODO GET RID OF ALL THESE DOUBLE LOOPS!!!!

	// CHECK VERTICAL
	for(uint8_t x=0;x<6;x++) {
		off = x;
	    for(uint8_t y=1; y<16-crashheight[1];y++){//1
			off += 6;//
			if(!(t = (cpubuffer[off]&15))){//get_buffer(x,y);
				continue;
			}				
			//t0 = cpubuffer[off-6];//get_buffer(x,y-1);
			//t2 = cpubuffer[off+6];//get_buffer(x,y+1);

			if(((cpubuffer[off-6]&15) == t) && (t == (cpubuffer[off+6]&15))){//t1==t0 && t1==t2){
				cpubuffer[off-6] |= SCORINGJEWEL;//set_buffer_scoring(x,y-1);
				cpubuffer[off] |= SCORINGJEWEL;//set_buffer_scoring(x,y);
				cpubuffer[off+6] |= SCORINGJEWEL;//set_buffer_scoring(x,y+1);
				total+=3;
			}
	    }
	}

	if(vsync_flag)
		return 0;
	// CHECK HORIZONTAL
	for(uint8_t x=1;x<5;x++) {//TODO ELIMINATE DOUBLE LOOPS JUST SCAN THROUGH BUFFER INLINED CASES FOR ENTIRE ROW
		off = x;//
	    for(uint8_t y=0; y<16-crashheight[1];y++){
			if(!(t = (cpubuffer[off]&15))){//get_buffer(x,y);
				off += 6;
				continue;
			}				
			//t0 = cpubuffer[off-1];//get_buffer(x-1,y);
			//t2 = cpubuffer[off+1];//get_buffer(x+1,y);

			if(((cpubuffer[off-1]&15) == t) && (t == (cpubuffer[off+1]&15))){//t1==t0 && t1==t2){
				cpubuffer[off-1] |= SCORINGJEWEL;//set_buffer_scoring(x-1,y);
				cpubuffer[off] |= SCORINGJEWEL;//set_buffer_scoring(x,y);
				cpubuffer[off+1] |= SCORINGJEWEL;//set_buffer_scoring(x+1,y);
				total+=3;
			}
			off += 6;//
	    }
		if(vsync_flag)
			return 0;
	}

	/*

//wow this version is actually slower????
	off = (WELLSIZE-2)-(crashheight[1]*6);//cpuhighestcolumnmag...
	do{
		if((cpubuffer[off]&127)){
			//putting parentheses around DESTROYS CPUT THINKING TIME!!!! WHY
			if((cpubuffer[off+0]&127 == cpubuffer[off+1]&127) && (cpubuffer[off+0]&127 == cpubuffer[off-1]&127)){
				cpubuffer[off-1] |= SCORINGJEWEL;//set_buffer_scoring(x-1,y);
				cpubuffer[off] |= SCORINGJEWEL;//set_buffer_scoring(x,y);
				cpubuffer[off+1] |= SCORINGJEWEL;//set_buffer_scoring(x+1,y);	
				total += 3;
			}				
		}
		
	}while(--off);
	*/
	// CHECK DIAGONALS
	for(uint8_t x=1;x<5;x++) {
		off = x;
		for(uint8_t y=1;y<15-crashheight[1]-1;y++) {
			off += 6;
			if(!(t = (cpubuffer[off]&15)))//get_buffer(x,y);
				continue;
			//t1 = cpubuffer[(off-1)+6];//get_buffer(x-1,y+1);
			//t2 = cpubuffer[(off+1)-6];//get_buffer(x+1,y-1);
			//t3 = cpubuffer[(off-1)-6];//get_buffer(x-1,y-1);
			//t4 = cpubuffer[(off+1)+6];//get_buffer(x+1,y+1);
			
			if((t == (cpubuffer[(off-1)+6]&15)) && (t == (cpubuffer[(off+1)-6]&15))){//t0==t1 && t0==t2){
				cpubuffer[off] |= SCORINGJEWEL;//set_buffer_scoring(x,y);
				cpubuffer[(off-1)+6]|= SCORINGJEWEL;//set_buffer_scoring(x-1,y+1);
				cpubuffer[(off+1)-6] |= SCORINGJEWEL;//set_buffer_scoring(x+1,y-1);
				total+=3;
			}
			if((t == (cpubuffer[(off-1)-6]&15)) && (t == (cpubuffer[(off+1)+6]&15))){//t0==t3 && t0==t4){
				cpubuffer[off] |= SCORINGJEWEL;//set_buffer_scoring(x,y);
				cpubuffer[(off-1)-6] |= SCORINGJEWEL;//set_buffer_scoring(x-1,y-1);
				cpubuffer[(off+1)+6] |= SCORINGJEWEL;//set_buffer_scoring(x+1,y+1);
				total+=3;
			}
		}
		if(vsync_flag)
			return 0;
	}
	

		
	if(total){
		for(uint8_t i=0;i<WELLSIZE-(crashheight[1]*6);i++)
			cpubuffer[i] = 0;		
	}	

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
////////DO GRAVITY/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
	//start from the top of the well

//CPUGRAVITYTOP:
//	bool moved;
	if(total){
	for(int8_t x=0;x<6;x++){
CPUGRAVITYTOP:
		off = x;
		for(uint8_t y=0;y<15-crashheight[1];y++){
			if(cpubuffer[off]){
				if(!cpubuffer[off+6]){//!t1){
					cpubuffer[off+6] = cpubuffer[off];
					cpubuffer[off] = 0;
					goto CPUGRAVITYTOP;//rescan the column
				}
			}
			off+=6;
		}
	}
	}		
	if(total){
		chain++;
		overall += (total*chain);
		total = 0;//<--
		if(vsync_flag)
			return 0;
		goto CPUSCORETOP;
	}
	if(overall > cpuscoretocome)//for CpuShouldCrash()
		cpuscoretocome = overall;
	
	cpulastchain = chain;
	if(flashingjeweltimer[1] && !(cpubuffer[cpuflashingjeweloffset]&FLASHINGJEWEL))//eliminated flashing jewel, worth a lot
		overall += 12;
	return overall*overall*overall*overall;
}

uint8_t APopulatedColumn(){
	uint8_t off = WELLSIZE+15*6;
	for(uint8_t i=0;i<6;i++)
		if(well[off++])
			return i;
	return 255;
}

uint8_t TallestColumn(uint8_t p){
	for(uint8_t y=2;y<15;y++)
	for(uint8_t x=0;x<6;x++)
		if(get_well(x,y,p))
			return x;
	return 4;
}

uint8_t BestPopulatedColumn(){//return which column to drop magic jewel on based on how many jewels it will remove from the well
	return TallestColumn(1);//TODO!
}
/*
const uint8_t CpuBaseWeights[] PROGMEM = {
32,32,32,31,31,31,30,30,30,31,31,31,32,32,32,33,33,33,
};

inline void CpuApplyBaseWeights(){
//return;
	if(!cpubasemultiplier)
		return;
	for(uint8_t i=0;i<3*6;i++)
		cpueval[i] = pgm_read_byte(&CpuBaseWeights[i])*cpubasemultiplier;
}
*/
inline void CpuFindHighestColumn(){//speed irrelevant
	for(uint8_t i=3*6;i<WELLSIZE;i++){
		if(cpubuffer[i]){
			cpuhighestcolumn = i%6;
			cpuhighestcolumnmag = 16-(i/6);
			return;
		}
	}		
}

inline void CpuDecideMagicUsage(){
	CpuFindHighestColumn();
	cpudecision[0] = cpuhighestcolumn;
	
	if(wellfullness[1] > 8*6 && crashheight[1] < 3)
		cpudecision[1] = 1;
	else if(crashheight[1] > 1){//might consider down jewel
		if(cpuhighestcolumnmag > 7 && crashheight[0] < 4)//we are on defense, consider down jewel(a little variety I guess...if not the best strategy)
			cpudecision[1] = 0;
	}
	else
		cpudecision[1] = 2;//attack!
}


inline void CpuHandleBlocked(){
	if(piecex[1] > cpudecision[0]){//we tried to go left but it was blocked, so eliminate all choices to the left
		for(uint8_t i=0;i<piecex[1]*3;i++)
			cpueval[i] = 0;
	}
	else if(piecex[1] < cpudecision[0]){//eliminate choices to the right
		for(uint8_t i=(piecex[1]+1)*3;i<3*6;i++)
		cpueval[i] = 0;
	}
}	



inline void CpuHandleFirstTick(){
	
	cpuflashingjeweloffset = 255;
	if(flashingjeweltimer[1]){
		for(uint8_t i=0;i<16*6;i++)
			if(cpubuffer[i] & FLASHINGJEWEL){
				cpuflashingjeweloffset = i;	
				break;
			}				
	}
	if(wellfullness[1] < 3){//start off with a random move
		cpustate = CPUDECIDED;
		cpudecision[1] = (prng(MAX_PLAYERS)+3)%3;
		cpudecision[0] = (prng(MAX_PLAYERS)+6)%6;
		return;
	}
		
	cpumovepos[0] = 0;
	cpumovepos[1] = 0;
	cpuscoretocome = 0;
	for(uint8_t i=0;i<6*3;i++)
		cpueval[i] = 800L;
	if(piece[(1*3)] > 6){//magic jewel, see which one to use and set decision
		CpuDecideMagicUsage();
		cpustate = CPUDECIDED;
		return;
	}else if((piece[3+0] == piece[3+1]) && (piece[3+1] == piece[3+2])){//all same color
		cpuevalmultiplier = 1;
		cpuscoremultiplier = 16;//just override base values
		cpusecondmovemultiplier = 0;
		cpubasemultiplier = 0;
	}else{//normal, not all same color or magic jewel
		cpuheightmultiplier = 1;
		cpuscoremultiplier = 5;//1+((wellfullness[1]/4));//8;
		cpuevalmultiplier = 1;
		cpusecondmovemultiplier = 10;//8;
		cpubasemultiplier = 1;
			
		//if(wellfullness[1] < 5*6){
			if(wellfullness[1] > 4*6){
				cpuheightmultiplier = 1;
				cpuscoremultiplier = 1;
				cpusecondmovemultiplier = 2;
				cpuevalmultiplier = 2;
			}
			else{
				cpuscoremultiplier = 5;
				cpuevalmultiplier = 10;
				cpusecondmovemultiplier = 0;
			}		
		//}
		/*	else{
				cpuevalmultiplier = 2;
				cpuscoremultiplier = 3;
				cpusecondmovemultiplier = 4;
			//	if(wellfullness[1] > 9*6)
				//	cpusecondmovemultiplier = 0;
			}*/
			//else if(wellfullness[1] > 5*6){
				//cpuevalmultiplier = 0;
			//}
			
		if(cpuevalmultiplier)
			(int32_t)(cpubaseval = CpuEvaluate());
	}			
	//CpuApplyBaseWeights();
	cpustate = CPUEVALUATING;
}




inline void CpuThink(){//TODO USE LOCAL VARIABLES FOR ANYTHING POSSIBLE
	//TODO FIND SOME RANDOM INITIAL SEEDS THE CPU PLAYS GOOD GAMES WITH AND STORE ~200 IN ARRAY FOR "GUARANTEED" GOOD CPU PLAYER
	//TODO SEE IF ALL EVALUATION CAN BE DONE WITH 8 BIT
	//FOR EACH INITIAL MOVE, ONLY ADD SCORE FOR BEST SECOND MOVE NOT ALL. 
	oldpadstate[1] = padstate[1] = 0;		
	if((game_state & GS_FIRSTTICK) || !cpustate || state[1] & (APPLYGRAVITY|SCORING|EXPLODING|LOSING|LOSING2|GAMEOVER|GETREADY))
		return;
	
	if(cpustate & CPUMOVEBLOCKED){
		CpuHandleBlocked();
		goto CPUPICKMOVE;
	}else if(cpustate & CPUDECIDED)//done thinking, implement decision
		return;

	if(CpuShouldCrash()){//besides anything else, could and should we crash the enemy
	//	padstate[1] |= BTN_SL;
	}
	if(cpustate == CPUFIRSTTICK){//last move is done, start new one
		CpuHandleFirstTick();
	}

	if(cpustate & CPUEVALUATING){//lets get down to the numbers!!!
		if(halfstep[1]){
			if(well[WELLSIZE+piecex[1]+((piecey[1]+3)*6)])//about to land piece, choose the  best move we have so far
				goto CPUPICKMOVE;
		}
#ifdef DEBUG

//ColumnsPrint2num(14,9,cpuscoremultiplier);
//ColumnsPrint2num(14,10,cpusecondmovemultiplier);
//ColumnsPrint2num(14,12,cpuevalmultiplier);
//ColumnsPrint3num(16,13,cpumovenum);
//ColumnsPrint3num(16,25,jewels[1]);//totalvsyncs/10);
state[1] |= PAUSED;state[1] ^= PAUSED;
#endif
//TODO IF WE HAVE A 3+ CHAIN AVAILABLE, DO NOT EVALUATE
//TODO CPU IS WASTING CRASHPOINTS!!!!! NOT CORRECTLY CALCULATING POINTS TO COME
		while(true){
			if(vsync_flag){
				vsync_flag = 0;
				totalvsyncs++;
				if(++vsyncs > 1)
					return;//TODO, IF WE ARE LEAVING BEFORE A DECISION, SEE IF WE ARE ABOUT TO MOVEPIECEDOWN() ONTO A BLOCK. IF SO CHOOSE CURRENT BEST MOVE.
			}
		//	if(cpueval[cpumovepos[0]] == 0)//first move is a losing move, don't check 18 second move variations
			//	goto cpunextmove;
			cputotal = 0;
			blank_piece(1);
			CpuBufferWellAsm(&well[WELLSIZE]);
			add_piece(1);
		//	cpueval[cpumovepos[0]] = 1000L;
		//	cputotal = (int32_t)(800L);
			uint8_t bufferreturn;
			if(true){//CpuBufferMove(cpumovepos[0],false)){
				bufferreturn = CpuBufferMove(cpumovepos[0],false);
				cpulastscore = CpuScoreAndGravity(cpumovepos[0]/3);
				//ColumnsPrint3num(14,7+cpumovepos[0],cpulastscore);
				//if(cpulastscore)
				//	WaitVsync(60);
				if(!bufferreturn){
				
					if(!cpulastscore){//move is a loss if it didn't score
						cputotal = 0;
						//cpueval[cpumovepos[0]] = 0;
						goto CPUNEXTMOVE;
					}
					//even if it did
				}
				//cpueval[cpumovepos[0]] += 1000;				
				//cpueval[cpumovepos[0]] += 100;//better than a losing move...
				//cpulastscore = cpulastscore*cpulastscore*cpulastscore*cpulastscore;
			//	if(cpulastscore){
					//ColumnsPrint3num(16,16,cpulastscore);
				//	while(true){};
			//	}
				if(cpuscoremultiplier)
					cputotal += (int32_t)(cpulastscore*cpuscoremultiplier);
				if(cpulastchain > 1){//good chain, we will take these all day long!
					cpustate = CPUDECIDED;
					cpudecision[0] = cpumovepos[0]/3;
					cpudecision[1] = (cpumovepos[0]+3)%3;
					return;
				}
				
				if(!cpusecondmovemultiplier){//not thinking about second move, see if we are still doing evaluation
					if(cpuevalmultiplier){
						(int32_t)(cpulasteval = CpuEvaluate());
						if(cpulasteval > cpubaseval)
							cputotal += (int32_t)((cpulasteval-cpubaseval)*cpuevalmultiplier);	
					}						
					(int32_t)(cputotal += (int32_t)(CpuEvaluateHeight()*cpuheightmultiplier));
					goto CPUNEXTMOVE;
				}else if(CpuBufferMove(cpumovepos[1],true)){//do second move
					//cputotal += 100;
					if(vsync_flag){//todo get rid of last score/eval calculated since we will redo it next time
						vsync_flag = 0;
						totalvsyncs++;
						if(++vsyncs > 1)
							return;
				
					}
					//TODO DONT CALCULATE IF SECOND JEWEL IS MAGIC OR ALL SAME
					//cpubestscore[cpubestoff] += 1;//better than a losing move...
					(int32_t)(cpulastscore = CpuScoreAndGravity(cpumovepos[1]/3));
					(int32_t)(cputotal += (cpulastscore*cpusecondmovemultiplier));
					(int32_t)(cputotal += (int32_t)(CpuEvaluateHeight()*cpuheightmultiplier));
					if(cpuevalmultiplier){
						(int32_t)(cpulasteval = CpuEvaluate());
						if(cpulasteval > cpubaseval)
							(int32_t)(cputotal += ((cpulasteval-cpubaseval)*cpuevalmultiplier));
					}
					//	cputotal += CpuEvaluateHeight()*cpuheightmultiplier;
				}//second move
			}
			else{//can't even make first move
				cputotal = 0;
				cpueval[cpumovepos[0]] = 0;
				
				goto CPUNEXTMOVE;
			}
CPUNEXTMOVE:
			cpueval[cpumovepos[0]] += cputotal;

			if(++cpumovepos[0] > 17){
				cpumovepos[0] = 0;
				if(!cpusecondmovemultiplier)
					cpumovepos[1] = 17;
				if(++cpumovepos[1] > 17){//tried all moves, use the best
CPUPICKMOVE:			cpumovepos[1] = 0;

					uint8_t best = 255;
					int32_t mag = 0;
					for(uint8_t i=0;i<18;i++){
//ColumnsPrint3num(16,8+i,cpueval[i]/10);
						if((int32_t)(cpueval[i] > mag)){
							best = i;
							(int32_t)(mag = cpueval[i]);
						}
					}
cpumovenum++;
				//	if(best == 255)//could not make a move that doesn't lose, don't make a suicide move run out the time(maybe they will lose first)
					//	return;   //DOESNT WORK!!!
					if(false){//padstate[0] & BTN_A){
						for(uint8_t i=0;i<18;i++)
							ColumnsPrint3num(16,8+i,cpueval[i]);
						
						WaitVsync(180);
						
					}
					cpustate = CPUDECIDED;
					cpudecision[0] = best/3;
					cpudecision[1] = (best+3)%3;//HACK? WHAT IS WRONG HERE!

					if(cpustate & CPUMOVEBLOCKED){//got here because we needed to choose another unblocked move
						cpustate ^= CPUMOVEBLOCKED;
						return;
					}
				//	if(debug){
					//CpuDebug();
				//	rtl_Print(10,5+best,PSTR("("));//SetTile(10,(5+best),120);
					//	WaitVsync(120);
					//}
					break;
				}
			}
		}//WHILE(TRUE)

	}//CPUEVALUATING
}

void CpuDisplayBuffer();


void CpuDebug(){
return;
	if(!(state[0] & PAUSED) || !debug)
		return;

	ColumnsPrint2num(16,7,cpumovepos[0]);
	ColumnsPrint2num(16,8,cpumovepos[1]);
	ColumnsPrint2num(16,9,cpudecision[0]);
	ColumnsPrint2num(16,10,cpudecision[1]);
	ColumnsPrint2num(16,11,cpulastscore);
	ColumnsPrint2num(16,12,cpulasteval);
	
	rtl_Print(12,7,PSTR("CR"));
	rtl_Print(12,8,PSTR("NX"));
	rtl_Print(12,9,PSTR("DX"));
	rtl_Print(12,10,PSTR("DR"));
	rtl_Print(12,11,PSTR("SC"));
	rtl_Print(12,12,PSTR("EV"));


	for(uint8_t i=0;i<18;i++)
			ColumnsPrintLong(9,i+5,cpueval[i]);

	CpuDisplayBuffer();
}

void CpuDisplayBuffer(){
	for(uint8_t x=0;x<6;x++)
	for(uint8_t y=3;y<16;y++)
		SetTile(12+x,10+y,(cpubuffer[(y*6)+x]*4)+(JEWELSTART));
	if(state[1] & (SCORING|APPLYGRAVITY|EXPLODING|LOSING|LOSING2|GAMEOVER))
		return;
/*	if(!(cpustate & CPUDECIDED) && !(padstate[0] & BTN_A || padstate[0] & BTN_B))
		WaitVsync(16);
	if(cpueval[cpumovepos[0]] > 2)
		WaitVsync(60);
*/
//	WaitVsync(60);

}
