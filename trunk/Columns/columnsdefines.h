const uint16_t CheatString[] PROGMEM = {BTN_UP,BTN_UP,BTN_DOWN,BTN_DOWN,BTN_LEFT,BTN_RIGHT,BTN_LEFT,BTN_RIGHT,BTN_B,BTN_A};

const int8_t introcharmap[] PROGMEM = "!ABCDEFGHIKLNOPRSTUVWXYZ";
const int8_t menucharmap[] PROGMEM =  "{}[],.^v";
const int8_t titlecharmap[] PROGMEM = "{}[],.^v>PRESTALCOUMNIGVZ";
const int8_t readycharmap[] PROGMEM = "READY!";
const int8_t pausecharmap[] PROGMEM = "PAUSERMQIT>";//SAVE 12 SPOTS FOR MAGIC JEWELS
//WINNER! TRY AGAIN! LOSER! CPU IS NOW LEVEL-!! BACK TO LEVEL- INCREDIBLE! - CPU FAIL!!
const int8_t gameovercharmap[] PROGMEM = "WINER!TYAGSOLVBKDC";

//#define DEBUG 1
extern uint8_t ram_tiles[];
extern volatile uint8_t vsync_flag;
extern bool playSong;
extern uint8_t masterVolume;
extern int16_t currDeltaTime;
extern int16_t nextDeltaTime;
extern const int8_t * songPos;
const int8_t * oldsongPos;
extern void SetSongTempo(int16_t t);

#define DEFAULTPRNG 27
#define SCOREFLASHTIME 1
#define SCOREFLASHITERATIONS 4
#define LOSINGWAITTICKS 30
#define EXPLOSIONTICKS 2
#define MASTERVOLUME 128//DEFAULT_MASTER_VOLUME is 111
#define TITLEIDLETIME 60*7*1
#define DEMOLENGTH 28*60*1
#define DROPTIMER 32/2
#define MAX_PLAYERS 2
#define WELLSIZE 16*6
#define CPUMAXDEPTH 2
#define GLOBALFLASHCOUNT 30/2//was perfect vs emulator timing until 30hz switch...
#define MINTEMPO 0
#define MAXTEMPO 110

#define JEWELSTART 14//52

//game states
#define GS_TITLE 0
#define GS_PLAYING 1
#define GS_DEMO 2
#define GS_FIRSTTICK 32

//well states
#define DROPPING 0
#define SCORING 1
#define EXPLODING 2
#define LOSING 4//crash bar rising up well
#define LOSING2 8//crash bar going down
#define GAMEOVER 16
#define APPLYGRAVITY 32
#define PAUSED 64
#define GETREADY 128//waiting for round to start
#define GOAHEAD PAUSED+GETREADY//signals to game logic to leave GETREADY state
#define INITIALSTATE 255

//curse types
#define CURSESWAPWELL 1//TODO UPARROW(POSSIBLY ALL) DOES NOT DISAPPEAR WHEN THIS CURSE IS ACTIVE!
#define CURSESWAPCONTROLS 2
#define CURSEUPSIDEDOWN 4
#define CURSENOROTATE 8
#define CURSEALLFLASH 16
#define CURSENOPREVIEW 32
#define FLASHINGJEWELCOMING 128



//jewel states &0b00011111 = jewel type
#define SCORINGJEWEL  0b10000000//will be removed after scoring is complete
#define HALFSTEP      0b01000000//has moved a half step down from its position
#define FLASHINGJEWEL 0b00100000//is a special flashing jewel

#define FIRSTMAGICJEWELRT 0
#define FIRSTEXPLOSIONRT 12
#define FIRSTGRIDRT 20
#define FIRSTCRASHRT 26
#define FIRSTFREERT 34

#define NEEDMAGICRAMMED 1
#define NEEDEXPLOSIONRAMMED 2
#define NEEDGRIDRAMMED 4
#define NEEDCRASHRAMMED 8
#define NEEDBLANKRAMMED 16
#define NEEDALLRAMMED 128
uint8_t ramtilestate[MAX_PLAYERS];

uint8_t vsyncs;

uint16_t padstate[MAX_PLAYERS];
uint16_t oldpadstate[MAX_PLAYERS];

uint8_t well		[WELLSIZE*MAX_PLAYERS];//well has 3 invisible lines above playfield, used for piece initially entering well
uint8_t preview		[MAX_PLAYERS*3];
uint8_t piece		[MAX_PLAYERS*3];
uint8_t piecex		[MAX_PLAYERS];
uint8_t piecey		[MAX_PLAYERS];
uint8_t piecerot	[MAX_PLAYERS];
uint8_t droptimer	[MAX_PLAYERS];
uint8_t state		[MAX_PLAYERS];
uint8_t statetimer	[MAX_PLAYERS];//needed since switch to 30hz gameplay
uint8_t stateframe	[MAX_PLAYERS];
uint16_t lfsr		[MAX_PLAYERS+1];
uint8_t crashcomingup  [MAX_PLAYERS];
uint8_t crashcomingdown[MAX_PLAYERS];

uint16_t crashedpiecex[MAX_PLAYERS*3];
uint16_t crashedpiecey[MAX_PLAYERS*3];
int8_t crashedxvel[MAX_PLAYERS*3];
int8_t crashedyvel[MAX_PLAYERS*3];
uint8_t crashedtype[MAX_PLAYERS*3];
uint8_t crashedredrawtime;

uint8_t cursetype[MAX_PLAYERS];
uint16_t cursetime[MAX_PLAYERS];

uint8_t sideheld[MAX_PLAYERS];//num ticks left or right is held



bool halfstep[MAX_PLAYERS];
//bool downletup[MAX_PLAYERS];
bool wasmagicjewel[MAX_PLAYERS];
uint8_t magicjewelssummoned[MAX_PLAYERS];

uint8_t chain		[MAX_PLAYERS];
uint8_t score		[MAX_PLAYERS];
//uint8_t chainscore[MAX_PLAYERS];
uint8_t crashheight	[MAX_PLAYERS];
uint16_t jewels		[MAX_PLAYERS];
uint16_t wellfullness[MAX_PLAYERS];
uint8_t magicjewelsonscreen;
uint8_t flashingjeweltimer[MAX_PLAYERS];

uint8_t demo_decision;
uint8_t demo_inp_wait;
uint16_t demo_off;
uint8_t demo_num;
//#define NUMDEMOS 3 defined in demo_data.inc

//uint32_t roundtimer;
uint8_t musictrack = 1;
uint16_t demotime;
uint8_t winner;
uint8_t level;
uint8_t debug;
uint16_t totalvsyncs;//used by cpu

uint8_t musicspeed;
uint8_t tickssincetempochange;

uint8_t cursorpos;

uint8_t flashcount;
uint8_t magiccolor;
uint8_t magiccolorwait;

uint8_t game_state;
uint8_t game_mode;
uint8_t globalflashcounter = GLOBALFLASHCOUNT;

uint8_t pcm_priority;//priority of current pcm sample playing
uint8_t pcm_timeleft;//frames until sample is complete



void DoPlay();
void DoTitle();
void DoDebugMenu();

bool StartDown(uint8_t p){return ((padstate[p] & BTN_START) && !(oldpadstate[p] & BTN_START));}
bool SelectDown(uint8_t p){return ((padstate[p] & BTN_SELECT) && !(oldpadstate[p] & BTN_SELECT));}
bool UpDown(uint8_t p){return ((padstate[p] & BTN_UP) && !(oldpadstate[p] & BTN_UP));}
bool LeftDown(uint8_t p){return ((padstate[p] & BTN_LEFT) && !(oldpadstate[p] & BTN_LEFT));}
bool RightDown(uint8_t p){return ((padstate[p] & BTN_RIGHT) && !(oldpadstate[p] & BTN_RIGHT));}
bool DownDown(uint8_t p){return ((padstate[p] & BTN_DOWN) && !(oldpadstate[p] & BTN_DOWN));}
bool LeftHeld(uint8_t p){return (padstate[p] & BTN_LEFT);}
bool RightHeld(uint8_t p){return (padstate[p] & BTN_RIGHT);}
bool DownHeld(uint8_t p){return (padstate[p] & BTN_DOWN);}
bool ADown(uint8_t p){return ((padstate[p] & BTN_A) && !(oldpadstate[p] & BTN_A));}
bool BDown(uint8_t p){return ((padstate[p] & BTN_B) && !(oldpadstate[p] & BTN_B));}
bool XDown(uint8_t p){return ((padstate[p] & BTN_X) && !(oldpadstate[p] & BTN_X));}
bool YDown(uint8_t p){return ((padstate[p] & BTN_Y) && !(oldpadstate[p] & BTN_Y));}
bool LSDown(uint8_t p){return ((padstate[p] & BTN_SL) && !(oldpadstate[p] & BTN_SL));}
bool RSDown(uint8_t p){return ((padstate[p] & BTN_SR) && !(oldpadstate[p] & BTN_SR));}

inline void SetCrashedPieces(uint8_t p);
inline void DoSong();


void DropPiece(uint8_t p);

inline void Intro();

inline void set_vram(uint8_t x, uint8_t y, uint8_t t){vram[(y*VRAM_TILES_H)+x] = t;}
void ColumnsPrint(int16_t x, int16_t y, char *string, int16_t offset);
void ColumnsPrint2num(uint8_t x, uint8_t y, uint16_t val);
void ColumnsPrint3num(uint8_t x, uint8_t y, uint16_t val);

uint16_t prng(uint8_t p);
void reset_prng(){	for(uint8_t i=0;i<MAX_PLAYERS;i++)lfsr[i]=0xACE1u;}
void shuffle_prng(){for(uint8_t i=0;i<MAX_PLAYERS;i++){for(uint8_t j=0;j<4;j++){prng(i);}}}



inline uint8_t get_well(uint8_t x, uint8_t y, uint8_t p){return 		well[(p*WELLSIZE)+x+(y*6)];}
inline uint8_t get_well_no_scoring(uint8_t x, uint8_t y, uint8_t p){return 		well[(p*WELLSIZE)+x+(y*6)]&0b00001111;}
inline void set_well(uint8_t x, uint8_t y, uint8_t p, uint8_t t){		well[(p*WELLSIZE)+x+(y*6)]=t;};
inline void set_scoring(uint8_t x, uint8_t y, uint8_t p){	well[(p*WELLSIZE)+x+(y*6)]|= 	0b10000000;}
inline bool get_scoring(uint8_t x, uint8_t y, uint8_t p){return	well[(p*WELLSIZE)+x+(y*6)]&		0b10000000;}
 void add_piece(uint8_t p){
	for(uint8_t i=0;i<3;i++){//smaller than inlined
		set_well(piecex[p],piecey[p]+i,p,piece[(p*3)+i]|(halfstep[p]*HALFSTEP));
		}
	}//set_well(piecex[p],piecey[p]+0,p,piece[(p*3)+0]|(halfstep[p]*HALFSTEP));set_well(piecex[p],piecey[p]+1,p,piece[(p*3)+1]|(halfstep[p]*HALFSTEP));set_well(piecex[p],piecey[p]+2,p,piece[(p*3)+2]|(halfstep[p]*HALFSTEP));}
 void blank_piece(uint8_t p){
	uint8_t off = piecex[p]+(piecey[p]*6)+(p*WELLSIZE);
	well[off] = 0;//smaller than loop!
	off += 6;
	well[off] = 0;
	off += 6;
	well[off] = 0;
	/*
	for(uint8_t i=0;i<3;i++){
		set_well(piecex[p],piecey[p]+i,p,0);
		}*/
}//set_well(piecex[p],piecey[p]+0,p,0);set_well(piecex[p],piecey[p]+1,p,0);set_well(piecex[p],piecey[p]+2,p,0);}
bool open_for_piece(uint8_t x, uint8_t y, uint8_t p){
	uint8_t off = (y*6)+x+(p*WELLSIZE);
	
	for(uint8_t i=0;i<3+halfstep[p];i++){
		if(well[off]){return false;}
		off += 6;
	}
	return true;
}

void GeneratePreview(uint8_t p);
void clear_well(uint8_t p){uint8_t off = p*WELLSIZE;for(uint8_t i=0;i<WELLSIZE;i++){well[off++]=0;}}//for(uint8_t y=0;y<16;y++){for(uint8_t x=0;x<6;x++){set_well(x,y,p,0);}}}
uint8_t CrashBarDown(uint8_t steps, uint8_t target, uint8_t silent);
uint8_t CrashBarUp(uint8_t steps, uint8_t target);
void ColumnsDrawMap(int16_t sx, int16_t sy, uint8_t w, uint8_t h, uint16_t o, const char *map, int16_t toff);
void ColumnsDrawMenu(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t o);
void RandomizeWells(uint8_t highestrow);
void UpdatePads();
void RamTileStateMachine();
inline void Attack(uint8_t target);
void SetLosing(uint8_t p);
uint8_t CheckLoss(uint8_t p);
uint16_t CheckScore(uint8_t p);
bool FlashingJewelInWell(uint8_t p);
void DrawJewel(uint8_t x, uint8_t y, uint8_t j);

#include "misc_mess.h"

