#define MAX_SD_ATTEMPTS 3

inline void Init();
void RamifyFromSD(uint8_t id, bool skipmap);
void AETriggerFx(unsigned char patch);//,unsigned char volume);
void DebugBeep(){AETriggerFx(1);}
void DebugCrash(){while(true){DebugBeep();WaitVsync(1);}}
void AEPrint(uint8_t x,uint8_t y,const uint8_t *string);
void AEPrintByte(uint8_t x,uint8_t y, uint8_t val, bool zeropad);
inline void LoadLevel(bool showname);
uint8_t LevelProgress(uint8_t level);
void AEClearVram();
////////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t demo_pos;
uint8_t demo_pad_ticks;
#if DEVMODE == 1
void dbp(uint8_t val){AEPrintByte(5,SCREEN_TILES_V-1,val,1);}
uint8_t demo_record;
uint16_t demo_record_pos;
uint8_t record_first_tick;
#warning "COMPILING IN DEVMODE, PLEASE #define DEVMODE 0 BEFORE RELEASE."
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAP_WDT				28UL
#define MAP_HGT				20UL
#define SD_LEVEL_SIZE		(32UL*22UL)
#define SD_DEMO_SIZE		(2UL*64UL)
#define AE_EEPROM_ID		0x45
#define AE_DEMORECORD_ID	2//(BTN_SELECT<<8)|(0)//hold BTN_SLECT for 0 frames
#define AE_TITLE_IDLE_TIME	20UL*60UL
#define DEMO_RT_OFF ((RAM_TILES_COUNT)*64UL)-SD_DEMO_SIZE
#define MAX_EPISODES		16
#define SD_EPISODE_POINTERS	0x210UL
#define SD_FIRST_LEVEL_BYTE	0xD2C0UL


#define SCRAMBLER_DEATH_PALLET	1
#define SCRAMBLER_SKULL_EYES	2
#define SCRAMBLER_FOREGROUND 	4
#define SCRAMBLER_OFF			255

#define DEMO_OFF			0
#define DEMO_ON				1
#define DEMO_PENDING		2

#define DIR_NONE			0
#define DIR_LEFT			1
#define DIR_RIGHT			2
#define DIR_UP				3
#define DIR_DOWN			4
#define DIR_MOVING			8
#define SWAP_PENDING		16
#define ALTER_SKULL			128

#define DONE_NOTYET			0
#define DONE_CLEAR			1
#define DONE_NOLUCK			2
#define DONE_RESTART		4
#define DONE_EXIT			8
#define DONE_DEMODONE		16

#define PLAYER_IDLE_MAX		50
#define PLAYER_START_DELAY	50
#define PLAYER_CLEAR_DELAY	10//used to be 24
#define PLAYER_MIN_BLINK_WAIT (45*1)
#define PLAYER_MAX_BLINK_WAIT (60*5)

#define PLAYER_SPR_IDLE 	0
#define PLAYER_SPR_WALK		1
#define PLAYER_SPR_LADDER 	5
#define PLAYER_SPR_FALL 	6
#define PLAYER_SPR_ALTER 	8

#define TILE_EMPTY			0
#define TILE_WALL			1
#define TILE_LADDER			2
#define TILE_ITEM1			4
#define TILE_ITEM2			8
#define TILE_ITEM3			16
#define TILE_BRIDGE			32
#define TILE_WATER			64
#define TILE_ALTER			128

#define TILE_FLOOR			(TILE_WALL|TILE_LADDER|TILE_BRIDGE)

#define LEFT	0
#define RIGHT	1
#define UP		2
#define DOWN	4
#define ENEMY_MAX			7//was 8
#define ITEM_MAX			16//was 20
#define PART_MAX			4

#define SFX_START			0
#define SFX_MENU			1
#define SFX_EXCHANGE		2
#define SFX_ITEM1			3
#define SFX_ITEM2			4
#define SFX_HIT				5
#define SFX_FALLING			6
#define SFX_DROP			7
#define SFX_NO_EXCHANGE		8
#define SFX_OUT_OF_EXCHANGES 9
#define SFX_BRIDGE			10
#define SFX_WALK1			11
#define SFX_WALK2			12
#define SFX_LADDER			13
#define SFX_LEVEL_CLEAR		14
#define SFX_ALL				15





const uint8_t sfx_priority[] PROGMEM = {
//start,	menu, exchange	,item1	,item2	,  hit	,falling,  drop	,noxchg,outxchg,bridge , walk1,	walk2,ladder,lvlclear,all
	250,	251	,	255		, 252	, 252	,  255	,	250	,	251	,	250,	250,	250,    80,	 80	 ,	130 ,	255  ,255};

#define MUS_TITLE			0
#define MUS_GAME1			1
#define MUS_GAME2			2
#define MUS_GAME3			3
#define MUS_GAME4			4
#define MUS_GAME5			5
//#define MUS_GAME6			6
//#define MUS_GAMEOVER		7
#define MUS_WELLDONE		6
#define MUS_ALL				7

#define AE_MASTER_VOL		255
#define AE_VOL_FADE_STEP	1
#define SFX_VOLUME 255

#define SD_SCREEN_CREDITS	0
#define SD_SCREEN_TITLE		1
#define SD_SCREEN_GAMEOVER	2
#define SD_SCREEN_WELLDONE	3

#define SD_SCREEN_INTRO_FRAME0		4
#define SD_SCREEN_INTRO_FRAME1		5
#define SD_SCREEN_INTRO_FRAME2		6
#define SD_SCREEN_INTRO_FRAME3		7
#define SD_SCREEN_LOGO		9
#define SD_SCREEN_SELECT	10

#define FIRST_MAP_TILE 0
#define FIRST_STAR_TILE 0//FIRST_MAP_TILE+0
#define FIRST_WATER_TILE FIRST_STAR_TILE+16
#define FIRST_ALTERBLOCK_TILE FIRST_WATER_TILE+18

const char *musicData[] PROGMEM={
TitleSong,
Level1Song,
Level2Song,
Level3Song,
Level4Song,
Level5Song,
//Level6Song,
//GameOverSong,
WellDoneSong,
};


//tile type flags

//TODO THERE IS STILL SOME BLACK OR START TILE THAT IS SOLID???!? prevents falling death ocassionally?
const uint8_t tileType[] PROGMEM ={
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,//stars
TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,

TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,//water/lava
TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WATER	,
TILE_WATER	,TILE_WATER	,

TILE_ALTER	,TILE_ALTER	,TILE_ALTER	,TILE_ALTER	,TILE_ALTER	,TILE_ALTER	,TILE_ALTER	,//alter blocks

TILE_WALL	,TILE_WATER	,TILE_WATER	,TILE_WATER	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,//level border frame
TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,

TILE_ITEM1	,TILE_ITEM1	,TILE_ITEM1	,TILE_ITEM1	,TILE_ITEM1	,//pixel item
TILE_ITEM2	,TILE_ITEM2	,TILE_ITEM2	,TILE_ITEM2	,TILE_ITEM2	,
TILE_ITEM3	,TILE_ITEM3	,TILE_ITEM3	,TILE_ITEM3	,TILE_ITEM3	,

TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_LADDER,TILE_WALL	,TILE_WALL	,TILE_EMPTY	,	//tileset 0
TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_BRIDGE,

TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_LADDER,TILE_WALL	,TILE_WALL	,				//tileset 1

TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_LADDER,TILE_EMPTY	,TILE_EMPTY	,				//tileset 2

TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_LADDER,TILE_EMPTY	,TILE_EMPTY	,	//tileset 3

TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_WALL	,TILE_LADDER,TILE_EMPTY	,TILE_EMPTY	,	//tileset 4

TILE_BRIDGE	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,TILE_EMPTY	,//SOMETHING WRONG HERE??

TILE_BRIDGE	,//FONT...
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#define FNTSTRT 111+RAM_TILES_COUNT//could eliminate the last grey bridge block
const uint8_t big_font_chars[] PROGMEM = {
FNTSTRT+1	,FNTSTRT+14,//A
FNTSTRT+2	,FNTSTRT+15,//B
FNTSTRT+3	,FNTSTRT+16,//C
FNTSTRT+4	,FNTSTRT+17,//D
FNTSTRT+5	,FNTSTRT+18,//E
FNTSTRT+5	,FNTSTRT+19,//F
FNTSTRT+6	,FNTSTRT+20,//G
FNTSTRT+7	,FNTSTRT+21,//H
FNTSTRT+8	,FNTSTRT+22,//I
FNTSTRT+9	,FNTSTRT+23,//J
FNTSTRT+10	,FNTSTRT+24,//K
FNTSTRT+11	,FNTSTRT+25,//L
FNTSTRT+12	,FNTSTRT+26,//M
FNTSTRT+13	,FNTSTRT+26,//N
FNTSTRT+1	,FNTSTRT+20,//O
FNTSTRT+4	,FNTSTRT+27,//P
FNTSTRT+1	,FNTSTRT+33,//Q
FNTSTRT+1	,FNTSTRT+34,//R
FNTSTRT+28	,FNTSTRT+35,//S
FNTSTRT+29	,FNTSTRT+36,//T
FNTSTRT+30	,FNTSTRT+20,//U
FNTSTRT+30	,FNTSTRT+37,//V
FNTSTRT+30	,FNTSTRT+38,//W
FNTSTRT+31	,FNTSTRT+39,//X
FNTSTRT+31	,FNTSTRT+36,//Y
FNTSTRT+32	,FNTSTRT+41,//Z
FNTSTRT+68	,FNTSTRT+69,//!

};


#define FBHT 41+RAM_TILES_COUNT
const char BorderHTiles[] PROGMEM = {
FBHT+0,FBHT+1,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+2,FBHT+3,FBHT+0,
};

const char BorderVTiles[] PROGMEM = {
FBHT+4,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+5,FBHT+6,
};

#define MTT 68+RAM_TILES_COUNT

const uint8_t BridgeSpriteType[] PROGMEM = {//per tile set
50,
50,
66,
66,
50,
};
const uint8_t MapTileTable[] PROGMEM ={
//0	,	1h. single piece	,	2h-seg left piece	,	3h-seg right piece	,	4v-seg top piece	,	5v-seg bot piece	,	6fake piece	,	7ladder	,
//8left face platform,	9right face platform	,	10down foliage big	,	11down foliage small	,	12foliage left	,	13foliage right	,	14spike	, 15alter block	,	16bridge	, 17water top	,	18water mid0, 19water mid1	,
//1,   	2		3		4		5		6		7		8		9		10		11		12		13		14		15		16		17		18		19		20		21		22		23
0,MTT+2	,MTT+0	,MTT+1	,MTT+2	,MTT+3	,MTT+2	,MTT+4	,MTT+5	,MTT+6	,MTT+7	,MTT+8	,MTT+9	,MTT+10	,MTT+40	,MTT-54	,MTT+11,MTT-52	,MTT-51	,MTT-50	,0		,0		,0		,0		,
0,MTT+13,MTT+12	,MTT+14	,MTT+15	,MTT+15	,MTT+13	,MTT+16	,MTT+17	,MTT+18	,MTT+7	,MTT+8	,MTT+9	,MTT+10	,MTT+40	,MTT-58	,MTT+11,MTT-46	,MTT-45	,MTT-44	,0		,0		,0		,0		,
0,MTT+19,MTT+21	,MTT+22	,MTT+20	,MTT+20	,MTT+19	,MTT+23	,MTT+5	,MTT+6	,MTT+24	,MTT+25	,FNTSTRT,FNTSTRT,MTT+40	,MTT-58	,MTT+42,MTT-40	,MTT-39	,MTT-38	,0		,0		,0		,0		,
0,MTT+26,MTT+27	,MTT+28	,MTT+29	,MTT+30	,MTT+26	,MTT+31	,MTT+5	,MTT+6	,MTT+32	,MTT+33	,FNTSTRT,FNTSTRT,MTT+40	,MTT-58	,MTT+42,MTT-52	,MTT-51	,MTT-50	,0		,0		,0		,0		,
0,MTT+34,MTT+35	,MTT+36	,MTT+37	,MTT+38	,MTT+41	,MTT+39	,MTT+17	,MTT+18	,MTT+7	,MTT+8	,MTT+9	,MTT+10	,MTT+40	,MTT-58	,MTT+11,MTT-40	,MTT-39	,MTT-38	,MTT-37	,0		,0		,0		,
};

const uint8_t sprite_frames[] PROGMEM = {
//active ego
0, 9,//0
0,10,//1
0,11,//2
0,12,//3
0,13,//4
1,14,//5
2,15,//6
1, 9,//7blink
//inactive ego
3,16,//8
4,17,//9
5,18,//10
6,19,//11
7,20,//12
8,21,//13
//skull horizontal
22,26,//14
22,27,//15
23,28,//16
23,29,//17
//skull vertical
24,30,//18
24,31,//19
25,32,//20
24,33,//21
//alter skull horizontal
34,42,//22
35,43,//23
36,44,//24
37,45,//25
//alter skull vertical
38,46,//26
39,47,//27
40,48,//28
41,49,//29
//item1
255,51,//30
255,52,//31
255,53,//32
255,52,//33
255,51,//34
255,54,//35
255,55,//36
255,54,//37
//item2
255,56,//38
255,57,//39
255,58,//40
255,57,//41
255,56,//42
255,59,//43
255,60,//44
255,59,//45
//item 3
255,61,//46
255,62,//47
255,63,//48
255,62,//49
255,61,//50
255,64,//51
255,65,//52
255,64,//53
};



#define ITSTRT 53+RAM_TILES_COUNT
const uint8_t ItemTileAnim[] PROGMEM = {
ITSTRT+0	,ITSTRT+1	,ITSTRT+2	,ITSTRT+1	,ITSTRT+0	,ITSTRT+3	,ITSTRT+4	,ITSTRT+3	,
ITSTRT+5	,ITSTRT+6	,ITSTRT+7	,ITSTRT+6	,ITSTRT+5	,ITSTRT+8	,ITSTRT+9	,ITSTRT+8	,
ITSTRT+10	,ITSTRT+11	,ITSTRT+12	,ITSTRT+11	,ITSTRT+10	,ITSTRT+13	,ITSTRT+14	,ITSTRT+13	,
};

#define ITMSPR	51
const uint8_t ItemSpriteAnim[] PROGMEM = {
ITMSPR+0 ,ITMSPR+1 ,ITMSPR+2 ,ITMSPR+1 ,ITMSPR+0 ,ITMSPR+3 ,ITMSPR+4 ,ITMSPR+3 ,
ITMSPR+5 ,ITMSPR+6 ,ITMSPR+7 ,ITMSPR+6 ,ITMSPR+5 ,ITMSPR+8 ,ITMSPR+9 ,ITMSPR+8 ,
ITMSPR+10,ITMSPR+11,ITMSPR+12,ITMSPR+11,ITMSPR+10,ITMSPR+13,ITMSPR+14,ITMSPR+13,
};

//const uint8_t SkullEyeColorAnim[] PROGMEM = {0x5F,0x0E,0x05,0x0E,};

const uint8_t IsForegroundTile[] PROGMEM = {
0b00000000,0b00000000,
0b00001111,0b00000000,
0b00111111,0b11111111,
0b11111111,0b11111111,
0b11110000,0b00011110,
0b00000000,0b00001100,
0b00001100,0b00001100,
//font tiles
//..no need

/*
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	//stars-
0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,//liquids-
1,1,1,1,1,1,1,						//alter block
1,1,1,1,1,1,1,1,1,1,1,1,			//level border
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,		//item pixels
0,0,0,0,0,0,0,1,1,1,1,0,			//tileset 0
0,0,0,0,0,0,0,						//tileset 1
0,0,0,0,0,1,1,						//tileset 2
0,0,0,0,0,0,1,1,					//tileset 3
0,0,0,0,0,0,1,1,					//tileset 4
//FONT TILES
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,//end of font tiles
*/
};

#define VRAM_FIRST_MAP_X 1
#define VRAM_FIRST_MAP_Y 4

#define FFNTT FNTSTRT
const uint8_t strFX[] PROGMEM =					{FFNTT+1+42,FFNTT+11+42,255};
const uint8_t strBGM[] PROGMEM =				{FFNTT+2+42,FFNTT+3+42,FFNTT+8+42,255};
const uint8_t strRESUME[] PROGMEM = 			{FFNTT+4+42,FFNTT+5+42,FFNTT+6+42,FFNTT+7+42,FFNTT+8+42,FFNTT+5+42,255};
const uint8_t strRESTART[] PROGMEM =			{FFNTT+4+42,FFNTT+5+42,FFNTT+6+42,FFNTT+9+42,FFNTT+10+42,FFNTT+4+42,FFNTT+9+42,255};
const uint8_t strEXIT[] PROGMEM =				{FFNTT+5+42,FFNTT+11+42,FFNTT+12+42,FFNTT+9+42,255};



const uint8_t DeathPalletColors[] PROGMEM = {102,5};//did color comparison of pallets, this is pretty close to NES colors



uint8_t spritenum;

void DrawMetaSprite(uint8_t x, uint8_t y, uint8_t frame, uint8_t flags){
	if(pgm_read_byte(&sprite_frames[(frame*2)+0]) != 255){
		sprites[spritenum].x = x;
		sprites[spritenum].y = y-8;
		sprites[spritenum].tileIndex = pgm_read_byte(&sprite_frames[(frame*2)+0]);
		sprites[spritenum++].flags = flags;
	}
	sprites[spritenum].x = x;
	sprites[spritenum].y = y-0;
	sprites[spritenum].tileIndex = pgm_read_byte(&sprite_frames[(frame*2)+1]);
	sprites[spritenum++].flags = flags;
}


uint16_t prng_state;
inline void seedprng(uint16_t seed){
	prng_state = seed;
}

uint16_t prng(){/*
	lda <RAND_SEED
	asl a
	bcc @1
	eor #$cf
@1:
	sta <RAND_SEED
	rts
	*/
/*	bool carry = prngnum&128;
	prngnum<<=1;
	if(carry)
		prngnum ^= 0xCF;

	return prngnum;
*/
	/* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
	uint16_t bit  = ((prng_state >> 0) ^ (prng_state >> 2) ^ (prng_state >> 3) ^ (prng_state >> 5) ) & 1;
	prng_state =  (prng_state >> 1) | (bit << 15);
	return prng_state;
}

inline uint8_t MAX(uint8_t a, uint8_t b){if(b > a){return b;}else{return a;}}
inline uint8_t MIN(uint8_t a, uint8_t b){if(b < a){return b;}else{return a;}}




extern const struct PatchStruct *patchPointers;
uint8_t current_patch_priority = 0;

extern void TriggerCommon(u8 channel,u8 patch,u8 volume,u8 note);



 void ResetSprites();
inline void DrawSprites();
inline void AnimateLevel();
void UpdateStats(uint8_t show);
void GameAddBackground(bool showname);
void level_select_screen();
inline void UpdatePad();
uint8_t RecordDemoPad();
void Render();
void SDCrash(uint8_t err);
inline void SDSetup();
void FillDemoBuffer(uint8_t dmo_num);
void FillDemoPad();
void CycloneEffect(uint8_t cursorpos, uint8_t guiframe);

void AEPrintBigRam(uint8_t x,uint8_t y, char *string);
uint8_t spritenum=0;
uint8_t map_frame;
uint8_t map_tile_set;
uint8_t scrambler_state;
//uint8_t scrambler_color;



void PostVsyncScrambler();
extern uint8_t ram_tiles[];
extern uint8_t free_tile_index;
extern uint16_t nextDeltaTime;
extern uint8_t masterVolume;
extern struct BgRestoreStruct ram_tiles_restore[];
FATFS fs;//worth keeping around for speed of not mounting each time
bool swappending;


const uint16_t code_str[6] PROGMEM ={BTN_B,BTN_LEFT,BTN_A,BTN_B,BTN_LEFT,BTN_A};


//player vars
uint8_t player_x1;
uint8_t player_y1;
uint8_t player_x2;
uint8_t player_y2;
uint8_t player_x1_to;
uint8_t player_y1_to;
uint8_t player_x2_to;
uint8_t player_y2_to;
//uint8_t player_x1e;
//uint8_t player_y1e;
//uint8_t player_dir;
uint8_t player_move_cnt;
uint8_t player_spr;
uint8_t player_spr_prev;
uint8_t player_state;
//uint8_t player_atr_dir;
uint8_t player_idle_cnt;
uint16_t player_blink_cnt;
uint8_t player_exchange;
uint8_t player_tile;
uint8_t player_tile_bottom;
uint8_t player_sync_type;
//uint8_t player_step;
uint8_t player_flash_cnt;
uint8_t exchange_flash_cnt;

//misc level vars
uint8_t frame_cnt;
uint8_t items_cnt;
uint8_t level;
uint8_t num_levels;
uint8_t num_demos;
uint8_t level_done;
uint8_t start_delay;
uint8_t restart;
uint8_t exchange;
uint8_t level_skip;
uint8_t level_clear_delay;
uint8_t music_prev;
uint8_t demo_prev = 255;
//uint8_t skull_eye_color;
uint32_t episode_data_base_offset;
uint8_t episode_loaded;
//uint8_t num_episodes;
uint8_t demo_state;
uint16_t oldpadstate,padstate;
//enemy vars

//uint8_t enemy_cnt;
uint8_t enemy_move_cnt;

uint8_t enemy_state[ENEMY_MAX];
uint8_t enemy_x  [ENEMY_MAX];
uint8_t enemy_y  [ENEMY_MAX];

//nametable update list for stats and items

uint8_t update_list_len;
uint8_t update_list[(ITEM_MAX)*3];

//particles

uint8_t part_ptr;
uint8_t part_x  [PART_MAX];
uint8_t part_y  [PART_MAX];
//uint8_t part_dy [PART_MAX];
uint8_t part_spr[PART_MAX];
//uint8_t part_atr[PART_MAX];
uint8_t part_cnt[PART_MAX];


