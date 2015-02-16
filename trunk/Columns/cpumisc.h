#define CPUNUMLEVELS 10

#define CPUOFF 0
#define CPUON 1
#define CPUEMERGENCY 2//well is getting crowded!! Don't think so deep, look for immediate scores!
#define CPUEVALUATING 8
#define CPUFIRSTTICK 16
#define CPUDECIDED 32
#define CPUMOVEBLOCKED 64

uint8_t cpuskill;
uint8_t cpustate;

extern uint8_t cpubuffer[WELLSIZE];
uint8_t cpuscoremultiplier;
uint8_t cpuevalmultiplier;
uint8_t cpusecondmovemultiplier;
uint8_t cpumovepos[2];
uint8_t cpudecision[2];
uint8_t cpumovewait;
int32_t cpulasteval;
int32_t cpulastscore;
int32_t cpueval[6*3];//problems with uint32_t???!?!?!?
uint16_t cpubestscore[3];
uint8_t cpubestoff;
uint8_t cpubasemultiplier;
int32_t cputotal;
uint8_t cpuscoretocome;
uint16_t cpumovenum;
uint8_t cpuhighestcolumn;
uint8_t cpuhighestcolumnmag;
uint8_t cpuheightmultiplier;
uint8_t cpulastchain;
int32_t cpubaseval;
uint8_t cpuflashingjeweloffset;
uint8_t cpulastbufferoff;//offset in cpubuffer[] of the top jewel of the last buffered piece
//used by cpuscoreandgravity to keep track of what is left of our original piece for
//cpu evaluate

//const uint8_t CpuMaxChoice[] PROGMEM = {3,3,2,2,1,1,0,0,0,0,};
//const uint8_t CpuMaxMoves[] PROGMEM = {1,1,1,1,1,2,2,2,2,2,};
const uint8_t CpuDropTime[] PROGMEM = {6,4,3,2,1,1,1,0,0,0,0,};
const uint8_t CpuMoveTime[] PROGMEM = {30,20,18,15,10,4,3,2,1,0,0,};

//ticks to think through entire step (step*18)*18
//18,324,5832,104976!!!!

void CpuDebug();
inline int32_t CpuScoreAndGravity(uint8_t c);
inline int32_t CpuEvaluate();
void cpu_crash(uint8_t p);

uint8_t TallestColumn(uint8_t p);

extern void CpuBufferWellAsm(uint8_t *wellstart);
inline void CpuFillPad(){

	if(!(cpustate & CPUDECIDED))
	return;
	if(state[0] & LOSING || state[1] & LOSING){
		cpustate = CPUEVALUATING;//don't do anything stupid!
		return;
	}		
	oldpadstate[1] = padstate[1] = sideheld[1] = 0;	

	if(cpumovewait){
		cpumovewait--;
		return;
	}
	else{
		if(cpudecision[0] == piecex[1] && cpudecision[1] == piecerot[1])
			cpumovewait = pgm_read_byte(&CpuDropTime[cpuskill]);
		else
			cpumovewait = pgm_read_byte(&CpuMoveTime[cpuskill]);
		if(cursetype[1]&63)
			cpumovewait <<=1;//go slower if cursed, except at higher levels where it's 0*2 ticks
	}

	if(cpudecision[0] < piecex[1])
		padstate[1] = BTN_LEFT;
	else if(cpudecision[0] > piecex[1])
		padstate[1] = BTN_RIGHT;
	else if((cursetype[1] != CURSENOROTATE) && (cpudecision[1] != piecerot[1]))
		padstate[1] = BTN_Y;
	else
		padstate[1] = BTN_DOWN;
}


bool CpuShouldCrash(){
	if(score[1] > 9){//can crash
		if((wellfullness[1] > 7*6) || (wellfullness[0]>8*6) ||//if we are in trouble don't die with any unused points! if they are in trouble finish them off!
			((score[1]+cpuscoretocome) > 30) ||//don't waste points, but keep score around in case they get something good!
			(piece[0] > 6) || (crashheight[1] >= (score[1]/10))){//crash their magic jewel!
			padstate[1] |= BTN_B;
			return true;
		}		
	}
	return false;
}


inline uint8_t buffer_column_height(uint8_t c){
	uint8_t off = c;
	for(uint8_t i=0;i<16;i++){
		if(cpubuffer[off])
			return i;
		off += 6;
	}
	return 16;
}

inline uint8_t AverageWellHeight(){//important!!!!
uint16_t total = 0;
uint8_t single = 0;
for(uint8_t x=0;x<6;x++){
	single = 0;
	for(uint8_t y=3;y<16;y++){
		if(!cpubuffer[(y*6)+x])
		single++;
		else
		break;
	}
	//cpueval[x*3] += single;
	total += single;
}
return (total+3)/6;//(total+3)/6;
}



extern uint8_t CpuScoreAndGravityAsm();