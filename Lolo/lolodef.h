extern uint8_t ram_tiles[];//we used this for SD stuff
extern struct BgRestoreStruct ram_tiles_restore[];//every 3rd byte will get corrupted every frame, the first 2 bytes the the pointer(+vram) to READ from during restore in asm file. Vram will not get corrupted since
//^<continued>as long as ram_tiles_index == 0 since no tiles in vram get restored then...limited and risky use.
extern volatile uint8_t sprites_tile_banks[];//we use the unused 2...7 bytes as variables
extern uint8_t font_tile_index;
extern uint8_t joypad1_status_lo[2];//is always actually a 16bit variable...
extern uint8_t joypad2_status_lo[2];//this could get corrupted, use for non-important temp var(don't count on it being there next frame)
//extern uint8_t joypad1_status_hi[2];//this shouldn't get corrupted???
//extern uint16_t joypad2_status_hi;//this shouldn't get corrupted???
extern uint8_t sync_flags;//useful to now if we are on even or odd frame on b1, TODO can we write to b2...b7?
extern uint8_t joypadsConnectionStatus;//this is not used for anything inside kernel, but b0,b1 will be written to so dont use those. also gets set to 0 each frame...
extern uint8_t sound_enabled;//careful to always have b0 set or sound turns off
extern uint8_t masterVolume;
extern uint8_t ReadPowerSwitch();
/*
unsigned char fadeStep,fadeSpeed,currFadeFrame;
char fadeDir;
bool volatile fadeActive;
*/

#define LOLO_SD_DATA_FILE "ADV_LOLO.DAT"
//FATFS fs;//42 bytes...
#define FIRSTMAPX 3
#define FIRSTMAPY 2


#define LEVELNUMBEROFFSETX FIRSTMAPX+22+2+2
#define LEVELNUMBEROFFSETY (SCREEN_TILES_V/4)-2
#define SHOTSOFFSETX LEVELNUMBEROFFSETX
#define SHOTSOFFSETY (SCREEN_TILES_V/2)-2
#define POWERPANELXOFF FIRSTMAPX+22+2
#define POWERPANELYOFF (FIRSTMAPY+22)-6

#define ACTIONBUTTON	BTN_B

//#define NUMWATERFRAMES	5
#define NUMLAVAFRAMES	9
#define WF	33+RAM_TILES_COUNT
#define LF	0

uint8_t water_frames[NUMWATERFRAMES] PROGMEM = {WF+0,WF+1,WF+2,WF+3,WF+4,WF+1,};
uint8_t water_ftimes[NUMWATERFRAMES] PROGMEM = {56,16,16,16,8,16};
uint8_t water_shadow_frames[NUMWATERFRAMES] PROGMEM = {WF+5,WF+6,WF+7,WF+8,WF+6,};
uint8_t lava_frames[NUMLAVAFRAMES] PROGMEM = {LF+0,LF+1,LF+0,LF+2,LF+3,LF+4,};
uint8_t lava_ftimes[NUMLAVAFRAMES] PROGMEM = {22,22,11,11,22,11,11,22,22,};
uint8_t lava_shadow_frames[NUMLAVAFRAMES] PROGMEM = {LF+5,LF+6,LF+5,LF+7,LF+8,LF+9};

#define MAX_ENEMIES		12
#define MAX_PROJECTILES	4//including lolo's
#define LEVELSDBYTES	100

#define GUITITLE		1
#define GUIMAINMENU		2
#define GUILEVELSEL		3
#define GUIFLOORINTRO	4
#define GUIENDGAME		5
#define GUIINGAMEMENU	6
#define GUINEWGAME		7
#define GUISTORY		8
#define GUISECRET		9

uint8_t gui_state = 0;
#define freesprite		sprites_tile_banks[2]//used a lot, don't multiplex
#define padstate		sprites_tile_banks[3]//uint8_t padstate,oldpadstate;//only need 8 bits
#define oldpadstate		sprites_tile_banks[4]
#define global_frame	sprites_tile_banks[5]
#define music			sound_enabled//make sure NOT to ever disable bit 0!!
uint16_t lfsr;//#define lfsr			joypad2_status_hi//16bit

#define px			sprites_tile_banks[6]
#define py			sprites_tile_banks[7]
uint8_t pstate,pftime;//#define pstate		joypad1_status_hi[0]//TODO MAKE SURE THIS REALLY DOESN'T GET CORRUPTED!!!!
//#define pftime		joypad1_status_hi[1]	
uint8_t poffset,pframe,panim,pshots,pblink;
uint8_t pb_x,pb_y,pb_offset;





uint8_t prng(uint8_t val);
inline void seed_prng(){//only used for blinking
	lfsr = 0xACE1u;
}

#define MUSICNOTRANDOM 128
void UpdatePad();

void DrawLevel();
void LEDon(){PORTD|=(1<<4);}
void LEDoff(){PORTD&=~(1<<4);}
void DrawMapTile(uint8_t x, uint8_t y, uint8_t t);
void LoloPrintNum(uint8_t x,uint8_t y,uint16_t val);
void LoloPrint(uint8_t x, uint8_t y, const char *string, uint8_t offset);
void LoadLevel(uint8_t level);
void DrawSpriteFrame(uint8_t x, uint8_t y, uint8_t frame, uint8_t pallet);
inline void SetSprite(uint8_t s, uint8_t x, uint8_t y, uint8_t t, uint8_t flags);
inline bool NewGameIntro();

void DrawMenu(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t offset);

void AnimateLevel();

//as used in logic
#define BRICK			0
#define TREE			1
#define ROCK			2
#define WATER			3
#define WATERUP			4
#define WATERDOWN		5
#define WATERLEFT		6
#define WATERRIGHT		7
#define SAND			8
#define GRASS			9
#define ARROWUP			10
#define ARROWDOWN		11
#define ARROWLEFT	12
#define ARROWRIGHT	13
#define XBRIDGE			14
#define YBRIDGE			15
#define WEAKBRIDGE		16
#define WEAKERBRIDGE	17
#define CHEST			18
#define CHESTOPEN		19
#define HEARTFRAMER		20
#define SHOTFRAMER		21
//#define EMERALDFRAMER	22//movable, doesn't stick to 11x11 grid
#define XPLAYERBRIDGE	22
#define YPLAYERBRIDGE	23
#define XPLAYERBRIDGEBURNED	24
#define YPLAYERBRIDGEBURNED	25

//as stored on sd
#define MAPBRICK			0
#define MAPTREE				1
#define MAPROCK				2
#define MAPWATER			3
#define MAPWATERUP			4
#define MAPWATERDOWN		5
#define MAPWATERLEFT		6
#define MAPWATERRIGHT		7
#define MAPXBRIDGE			8
#define MAPYBRIDGE			9
#define MAPCRUMBLEV			10
#define MAPCRUMBLEH			11
#define MAPEMERALD			12
#define MAPHEART			13
#define MAPHEARTSHOT		14
#define MAPGRASS			15
#define MAPSAND				16
#define MAPLOLOSTART		17
#define MAPSNAKEY			18
#define MAPMEDUSA			19
#define MAPDONUP			20
#define MAPDONDOWN			21
#define MAPDONLEFT			22
#define MAPDONRIGHT			23
#define MAPGOLUP			24
#define MAPGOLDOWN			25
#define MAPGOLLEFT			26
#define MAPGOLRIGHT			27
#define MAPALMAUP			28
#define MAPALMADOWN			29
#define MAPALMALEFT			30
#define MAPALMARIGHT		31
#define MAPLEEPERUP			32
#define MAPLEEPERDOWN		33
#define MAPLEEPERLEFT		34
#define MAPLEEPERRIGHT		35
#define MAPSKULLUP			36
#define MAPSKULLDOWN		37
#define MAPSKULLLEFT		38
#define MAPSKULLRIGHT		39
#define MAPROCKYUP			40
#define MAPROCKYDOWN		41
#define MAPROCKYLEFT		42
#define MAPROCKYRIGHT		43
#define MAPMOBYUP			44
#define MAPMOBYDOWN			45
#define MAPMOBYLEFT			46
#define MAPMOBYRIGHT		47
#define MAPWARP0			48
#define MAPWARP1			49
#define MAPWARP2			50
#define MAPWARP3			51
#define MAPWARP4			52
#define MAPWARP5			53
#define MAPWARP6			54
#define MAPWARP7			55
#define MAPCHEST			56
#define MAPARROWUP			57
#define MAPARROWDOWN		58
#define MAPARROWLEFT		59
#define MAPARROWRIGHT		60


//level flags
#define CHESTOPENED		1//chest has already been opened
#define DOOROPENED		2//door has already been opened(and chest was collected and closed again)
#define ENEMIESWIPED	4//enemies have already been exploded
#define ALTERTILES		8//using the underworld tileset
#define LOLOMOVED		16
#define DEMOPLAYBACK	32

#define POWERNONE		0 
#define POWERARROW		1
#define POWERBRIDGE		2
#define POWERHAMMER		3
#define POWERACTIVE		0b00100000

const uint8_t MapTileByPowerPanel[] PROGMEM = {67,41,47,68};


void RestoreTile(uint8_t x, uint8_t y);

typedef struct{
	//bit packed, no one can say I didn't try!
	uint8_t grid[76];//11*11*5=605bits, 76bytes=608bits for tile type. last 3 bits of buffer used for: LOLOMOVED,TBD,TBD
	uint8_t collide[61];//22*22=484bits, 61bytes=488bits for solid bits. last 4 bits of buffer used for: TBD,TBD,TBD,TBD
	uint8_t dooroff;//b0...b6 used as offset, b7 for level flag: TBD
	uint8_t powerpanel[3];//b0.b1 = power type, b2 = power active, b3..b7 = hearts till power(max 31)
	uint8_t warpholes[5];//b0...b6 used as offset(11*11=121), b7 use for level flag: TBD
	uint16_t current;//0b...b9 used as level(1024 max should do), b10...b14 used as hearts_collected
	//uint8_t hearts_collected;//^packed into above
	uint8_t heartsleft;//0b...b5 used as hearts left(max 31), b6...b7 use for: TBD
	uint8_t water_frame,water_ftime;
//b0...b4 of grid11 array used for tile type
//b5
//br for bytes 0...59 used as 
//b6 for bytes 60...119 used as solid bit for lolo shots on 22*22 tile field
//b7 for bytes 0...59, used as solid bit for movement on 22*22 tile field
//b7 for bytes 60...119, used as solid bit for enemy shots on 22*22 tile field


}Level;

Level lvl;


inline bool GetSolidForEnemyShotBit(uint8_t x, uint8_t y);
inline void SetSolidForEnemyShotBit(uint8_t x, uint8_t y);


inline uint8_t GetLvlGrid(uint8_t x, uint8_t y){
	return (lvl.grid[(y*11)+x]&0b00011111);
}

inline uint8_t GetLvlGridOff(uint8_t offset){
	return (lvl.grid[offset]&0b00011111);
}


typedef struct{
	//bit packing to save ram tiles...I'm trying my best here!
	//uint8_t x;			//0...31,	5 bits 			byte 0 00011111
	//uint8_t type;			//0...7		3 bits			byte 0 11100000
	//uint8_t y;			//0...31,	5 bits 			byte 1 00011111
	//uint8_t offset;		//0...7,	3 bits			byte 1 11100000
	//uint8_t state;		//0...255	8 bits			byte 2 11111111
	//uint8_t frame;		//0...3,	2 bits			byte 3 11000000
	//uint8_t frame_time;	//0...15,	4 bits			byte 3 00001111
	//uint8_t projectile;	//0...7,	2 bits(split)	byte 3 00110000//least significant bits
	//--------^second part-------------+1 bit (split)---byte 4 10000000//most significant bit
	//uint8_t origin;		//0...127	7 bits			byte 4 01111111
	uint8_t packedbits[5];
}Enemy;

/*
typedef struct{//TODO THIS CAN BE DEFINED(EXCEPT FOR LOLO'S) as simply which monster owns it, and how far(in the facing direction) it has traveled(type derived from enemy)
	uint8_t x,y;
	uint8_t flags;//b0...b1 = type

}Projectile;
*/

typedef struct{
	uint8_t packedbits[2];//	byte 0	b0...b3 owner,
//								byte 0	b4...b7 offset part 1
//								byte 1	b0...b3 offset part 2
}Projectile;



//Player player;//uses unused kernel bytes, see defines
Enemy enemy[MAX_ENEMIES];
Projectile projectiles[MAX_PROJECTILES];

inline uint8_t get_enemy_x(uint8_t id);
inline void set_enemy_x(uint8_t id,uint8_t val);
inline uint8_t get_enemy_y(uint8_t id);
inline void set_enemy_y(uint8_t id,uint8_t val);
inline uint8_t get_enemy_state(uint8_t id);
inline void set_enemy_state(uint8_t id,uint8_t val);
inline uint8_t get_enemy_frame(uint8_t id);
inline void set_enemy_frame(uint8_t id,uint8_t val);
inline uint8_t get_enemy_ftime(uint8_t id);
inline void set_enemy_ftime(uint8_t id,uint8_t val);
//inline uint8_t get_enemy_anim(uint8_t id);
//inline void set_enemy_anim(uint8_t id,uint8_t val);
inline uint8_t get_enemy_offset(uint8_t id);
inline void set_enemy_offset(uint8_t id,uint8_t val);
inline uint8_t get_enemy_type(uint8_t id);
inline void set_enemy_type(uint8_t id,uint8_t val);

#define ENEMYBYTES		5
#define PLAYERBYTES		4
#define SPAWNHOLEBYTES	6

#define UP			1
#define DOWN		2
#define LEFT		4
//#define RIGHT		8
#define SPECIAL0	8
#define SPECIAL1	16
#define EGGED		32
#define MOVING		64


#define DEAD		255

#define PLAYER_SHOT_SPEED	4

#define STATEBITS	SPECIAL0|SPECIAL1
#define EGGFLYING	SPECIAL0
#define EGGWATER	SPECIAL1
#define EGGSINKING	SPECIAL0|SPECIAL1

#define SNAKEY			0
#define MEDUSA			1
#define DONMEDUSA		2
#define GOL				3
#define ALMA			4
#define LEEPER			5
#define SKULL			6
#define ROCKY			7
#define MOBY			8

#define EXPLOSION		9
#define DUMMY			10




//music defines
#define MUS_LOLO1
#define MUS_LOLO2
#define MUS_LOLO3
#define MUS_LVLSELECT
#define MUS_RETRY
#define MUS_LOLODIE
#define MUS_VICTORY
#define MUS_INTRO

//sfx defines
#define SFX_LOLO_SHOOT		0
#define SFX_LOLO_DIE		1
#define SFX_EGGED			2
#define SFX_EGG_SPLASH		3
#define SFX_FLY_AWAY		4
#define SFX_GRAB_HEART		5
#define SFX_GRAB_SHOT		6


#define SFX_MEDUSA_SHOOT	7
#define SFX_LEEPER_SLEEP	8
#define SFX_GOL_SHOOT		9
#define SFX_EXPLOSION		10

//SD->screen flags
#define RS_REMOUNT				1//remount the data file before reading(save ram when the FATFS object wasn't needed, but slow)
#define RS_SKIP_VRAM			2//do not update the current vram with a new map(faster loading when only ram tiles change)
#define RS_SKIP_RAMTILES		4//do not update the current ram tiles(faster loading when only the map changes)
#define RS_PRE_INTERLACE_ALIGN	8
#define RS_POST_INTERLACE_ALIGN	16
#define RS_SKIP_SPRITES			32

#define SD_LOLO_LEVEL_DATA_START	0

#define BLACK_TILE	32
void VsyncCallBack();//logic.h
void HideSprites();//render.h
inline void LoloClearVram(uint8_t index);//render.h
inline void SetGridTileType(uint8_t off);//logic.h
inline uint8_t GetGridTileType(uint8_t off);//logic.h

void WaitVsyncInterlaceAligned(uint8_t min_frames){
	WaitVsync(min_frames+(global_frame&1?1:0));//frame counter updated in PostVsyncCallback, start on even frame(for Uzem)
}

void DebugCrash(uint8_t i, uint8_t j, uint8_t k){
	FadeIn(0,false);
	SetTileTable((const char *)MapTiles);
	if(i == 0){
		LoloPrint(3,10,PSTR("ERROR CANT OPEN DATA FILE"),134);
		LoloPrintNum(21,10,j);
	}else if(i == 1){
		LoloPrint(6,10,PSTR("SD DATA LOAD FAILED"),134);
		LoloPrintNum(21,10,j);
		LoloPrintNum(21,11,k);
	}else{
		LoloPrint(9,10,PSTR("FATAL ERROR"),134);
		LoloPrintNum(21,10,j);
		LoloPrintNum(21,11,k);
	}
	while(true){
		LEDoff();
		WaitVsync(30);
		LEDon();
	}
}

inline bool GETDEMOPLAYBACKFLAG();
inline void SETLOLOMOVED();
inline bool GETLOLOMOVED();
void RamifyFromSD(uint8_t id, uint8_t flags);


void DrawSidePanel();
void DrawBorder(uint8_t set);
