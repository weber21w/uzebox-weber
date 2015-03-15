#define FF_EEPROM_ID 70UL
#define FF_MASTER_VOLUME 224
/* Position offsets */
#define	SPLASHX		4
#define	TONGUEX		16
#define	FROG1X		40//80//82
#define	FROG2X		192//152//222
#define	FROGY		172//140
#define	FLYY		60
#define	FLYRANGE	20
#define TITLEIDLETIME 12*60

/* Animation lengths */
#define	LICKLENGTH	8*2
//#define	LEAPLENGTH	160
#define JUMPLENGTH  108
#define	HOPLENGTH	8
#define	SKYLENGTH	7

#define TURNWAIT	6
#define HOPWAIT		5
#define SWIMWAIT	4
#define LANDWAIT	12
#define TONGUEWAIT	5
#define STUNTIME	30*7//30*6
#define STOMPSPINTIME 24
#define STOMPVELOCITY 8
/* Game defines */
#define	MAXFLIES	4
#define	FLYSCORE	2
#define STOMPSCORE	1
#define	GAMETIME	(uint16_t)(1L*(60L*60L))+(30L*(60L))//1:30 @ 60FPS
#define	ENDDELAY	(uint16_t)(10L * 60L)
#define NUMCOLORSHADES sizeof(CustomFadeTable)-1
#define COLORTIMELENGTH (GAMETIME/NUMCOLORSHADES)//-30

#define MAXPLAYERS 2

typedef struct{
	uint8_t	x;//add about 300 bytes with all the int16...
	uint8_t	y;
	int16_t xFrac,yFrac;
	int16_t	xVel,yVel;
	uint8_t JumpCeiling;//the original y-point we jumped from to determine max jump height(support wall jumps better)
	uint8_t InvincibleTimer;
	uint8_t	AirTime;
	uint8_t	TongueTime;
	uint8_t BlinkTime;
	uint8_t Wait;
	uint8_t	AIState;
	uint8_t AITarget;
	uint16_t State;
	uint8_t Frame;
	uint8_t Score;
	uint8_t Chain;
//	uint8_t Name[6];
}Frog;

typedef struct{
	uint8_t	x;
	uint8_t	y;
//	uint8_t	FlyTime;
	uint16_t RebirthTimer;
	uint8_t State;
}Fly;


//AI states
#define	AI_SIT		0	
//#define	AI_LEAP		1
#define AI_JUMP		2
#define	AI_CENTER	4
#define	AI_SWIM		8
#define	AI_TONGUE	16
#define	AI_LAND		32
#define AI_STUNNED	64
#define AI_EVADE	128

//Frog states
#define SITTING		1L
#define TURNING		2L
#define FACING		4L
#define HOPPING		8L
#define JUMPING		16L
#define DROPPING	32L//no steering
#define FALLING		64L//steering
#define SWIMMING	128L
#define TONGUEING	256L
#define STUNNED		512L
#define STOMPING	1024L
#define INWATER		2048L
#define JUMPQUIT	4096L
//#define DELAYEDSTUN 8192L//
#define DOUBLEJUMPED	16384L
//#define DODGE			32768L

//Fly only
#define ACTIVE	16L


//#define LEAP_BUTTON (uint16_t)(BTN_B|BTN_A|BTN_UP)
#define JUMP_BUTTON (uint16_t)(BTN_UP|BTN_A|BTN_Y)//supports NES pads without recompile
#define LICK_BUTTON (uint16_t)(BTN_B|BTN_SL|BTN_SR|BTN_X)


//AI constants
#define	JUMPTOWARDS	31//29//32
#define	JUMPAWAY	34//28//42


inline void TitleScreen();
inline void HighScoreScreen();
inline bool ScrollEntry(uint8_t x, uint8_t y, uint8_t *buffer);
void CreditScreen();//larger when inlined
void GameLoop();//much larger when inlined
inline void GameOver();
inline void CheckFlies();
inline void UpdateFrogs();
inline void UpdateFlies();
inline void DrawScores();
void MoveFrogWater(uint8_t i);
void UpdateFrog(uint8_t i, uint8_t x, uint8_t y);
void UpdateTongue(uint8_t i);
void CheckWater(uint8_t i);
void CheckTongue(uint8_t i);
inline void ResetLogic();
void AddFly();
void KillFly(uint8_t i);

uint16_t ProcessAI(uint8_t i);
uint16_t AIStun(uint8_t i);
uint16_t AISit(uint8_t i);
uint16_t AIJump(uint8_t i);
uint16_t AICenter(uint8_t i);
uint16_t AISwim(uint8_t i);
uint16_t AITongue(uint8_t i);
uint16_t AILand(uint8_t i);
uint8_t AIFlyInJumpRange(uint8_t i);
uint16_t AISwimTowardsPad(uint8_t i);
uint16_t AICheckFacing(uint8_t i);
uint16_t AIFaceCenter(uint8_t i);
uint8_t AIFliesInKillZone(uint8_t i);
inline void UpdatePads();
//extern void SetMasterVolume(uint8_t vol);
void ShowTitle();




uint16_t prng_state;
inline void seedprng(uint16_t seed){
	prng_state = seed;
}

uint16_t prng(){
	uint16_t bit  = (uint16_t)(((prng_state >> 0) ^ (prng_state >> 2) ^ (prng_state >> 3) ^ (prng_state >> 5) ) & 1);
	prng_state =  (uint16_t)((prng_state >> 1) | (bit << 15));
	return prng_state;
}


// Screen defines
#define	SCREENLEFT		0
#define	SCREENRIGHT		239
#define	LEFTEDGE		26
#define RIGHTEDGE		216-12
#define	MIDDLELEFT		120-12
#define	MIDDLERIGHT		124
#define KILLZONELEFT	68
#define KILLZONERIGHT	(SCREEN_TILES_H*TILE_WIDTH)-KILLZONELEFT
#define KILLWIDTH		KILLZONERIGHT-KILLZONELEFT
#define KILLHEIGHT 		112//FROGY-50//y value below which catching another fly is unlikely

// Sprite offsets
#define	SWIMSPRITE		28
#define	FACESPRITE		32
#define	JUMPSPRITE		4
#define	FLYSPRITE		41//24

// Sound defines
#define	SFX_FROG		0
#define SFX_WATER		1
#define	SFX_TONGUE1		2
#define SFX_TONGUE2		3
#define SFX_GRAB1		4
#define	SFX_SMASH		5
#define SFX_JUMP1		6
#define SFX_JUMP2		7
#define SFX_PAUSE		8
#define SFX_STOMP		9


// Score defines
#define	SCOREY			SCREEN_TILES_V-1
#define	SCORE1X			2
#define	SCORE2X			28
#define	TIMEX			13
#define	TIMEY			SCREEN_TILES_V-1
#define	STARTX			7		
#define	STARTY			20
#define	STARTSPRY		169
#define	GAMEOVERX		11
#define	GAMEOVERY		SCREEN_TILES_V-1
#define	PLAYER1X		88
#define	PLAYER2X		144
#define	PLAYERY			FROGY//((GAMEOVERY + 4) * 8)
#define	ENDSCORE1X		13
#define	ENDSCORE2X		18
#define	TIEX			GAMEOVERX//16

// Animation frames
#define TFSPR 64
const uint16_t TongueFrames[LICKLENGTH] PROGMEM	={
//WAS 0,1,2,3,3,2,1,0,
TFSPR+1,TFSPR+2,TFSPR+2,TFSPR+3,TFSPR+3,TFSPR+3,TFSPR+3,TFSPR+3,TFSPR+2,TFSPR+2,TFSPR+2,TFSPR+1,TFSPR+1,TFSPR+1,TFSPR+1,TFSPR+0,
};

Frog		Frogs[MAXPLAYERS];
Fly			Flies[MAXFLIES];
uint16_t	OldJoyVal[MAXPLAYERS];
uint16_t	JoyVal[MAXPLAYERS];
uint16_t	Time;
uint16_t	ColorTime;
uint8_t		ColorIndex;
uint8_t		FlySide;
uint8_t		Players;
uint16_t	LastAction;
uint8_t		Demo;
uint8_t		FlyCount;
bool Cheats[MAXPLAYERS];
bool InGui;
bool MusicOn;
bool SoundsOn;
uint8_t spritecount;

#define GRAVITYPERTICK 		56L
#define GRAVITYMAXPERTICK	(16L*GRAVITYPERTICK)
#define JUMPMAXHEIGHT FROGY-128L//FROGY-120L
#define DOUBLEJUMPCEILING		6
#define JUMPINITIALACCELERATION		768L
#define JUMPACCELERATIONPERTICK 24L//(24L)
#define JUMPMAXACCELERATIONPERTICK	(JUMPINITIALACCELERATION)+(JUMPACCELERATIONPERTICK*6L)
#define DOUBLEJUMPVELOCITYWINDOW	GRAVITYPERTICK*6//max acceleration a frog can have while falling and still double jump(double jump window is only a few frames)

#define STEERACCELERATION 16L
#define STEERMAXACCELERATION 384L
#define JUMPINITIALXMOMENTUM	256L
const uint8_t HopWave[HOPLENGTH+1] PROGMEM ={0,2,3,5,6,6,5,3,2};
const uint8_t TongueLength[LICKLENGTH+1]	PROGMEM ={0,3,5,7,9,9,7,5,3};

const uint8_t CustomFadeTable[] PROGMEM = {
    0b11111111,//255
	0b10111111,//191
    0b10111110,//190
    0b10110110,//182
    0b10110101,//181
	0b01110101,//117
   // 0b10101101,//173
   	//0b10100010,
    //0b01101101,
	//0b01101100,
   // 0b10011010,
    0b10010001,
   // 0b01010010,
   	0b01000000,//64
};

const int8_t titlecharmap[] PROGMEM 	= "!12PLAYERSOIGNMCHDTBUQZXVWF:K*";
const int8_t highscorecharmap[] PROGMEM = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
const int8_t gameovercharmap[] PROGMEM	= "1!WINERTGAMCOD";
const int8_t introcharmap[] PROGMEM = "UZEBOX";
const int8_t pausecharmap[] PROGMEM = "RESUMICONFXQT";
const uint16_t KonamiCode[] PROGMEM = {BTN_UP,BTN_UP,BTN_DOWN,BTN_DOWN,BTN_LEFT,BTN_RIGHT,BTN_LEFT,BTN_RIGHT,BTN_B,BTN_A};



uint8_t EEPromScore(uint8_t slot, bool direction, uint8_t *dat, uint8_t mag);
const uint8_t EEPROMdefault[] PROGMEM = {
'A','L','E','C',' ',28,
'C','N','F','L','W',25,
'G','O','S','M','A',21,
'H','A','R','T','Y',18,
'J','O','N','E','S',16,
};


//inline int8_t	absi8	(int8_t val)	{return (val < 0)?(-val):(val);}
//inline int16_t	absi16	(int16_t val)	{return (val < 0)?(-val):(val);}
	
void FFTriggerFx(uint8_t patch){
	if(SoundsOn)
		TriggerFx(patch,255,1);
}

void FFTriggerPCM(uint8_t patch, uint8_t freq, uint8_t vol){
	if(SoundsOn)
		TriggerNote(4,patch,freq,vol);

}

//inline void db(){TriggerFx(4,255,true);}

uint8_t scorename[6*2]={'A','A','A','A','A',0,'A','A','A','A','A',0,};
//FATFS fs;


#if false//DEV_MODE == 1
//const uint8_t basegenes[] PROGMEM = {
//0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,
//2,0,0,13,16,8,2,10,10,34,5,18,3,6,31,1,5,34,2,31,34,112,68,180,4,0,0,8,8,8,0,8,14,7};
#define NUMAIVARS 36
//#undef TITLEIDLETIME
//#define TITLEIDLETIME 2
//uint8_t aivars[NUMAIVARS];
//uint8_t bestaivars[NUMAIVARS];

//uint16_t bestscore = 0;
//uint8_t roundnum = 0;
//uint16_t generation = 0;

#endif
