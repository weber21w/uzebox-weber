#define TQ_ID			71
#define TQ_MASTER_VOL	208

void dbf(){while(1){TriggerFx(1,255,1);WaitVsync(5);}}
void updateController();

uint16_t controllerState = 0;
uint16_t prevControllerState = 0;


void updateController() {
	prevControllerState = controllerState;
	controllerState = ReadJoypad(0);	
}

const char intro1Str[] PROGMEM =
	"OH NO!!\n\n\n"
	"TOORUM'S BELOVED PRINCESS\n\n"
	"ADELA HAS BEEN KIDNAPPED\n\n"
	"BY EVIL MAHARADJA KOVALSKY!\n\n";

const char intro2Str[] PROGMEM =
	"HELP TOORUM FIND THE\n\n"
	"PRINCESS BEFORE THE\n\n"
	"MAHARADJA SEDUCES HER!\n\n\n"
	"GO!!!";

const char editor1Str[] PROGMEM =
	"-LEVEL EDITOR INSTRUCTIONS-\n\n\n"
	"DPAD MOVES CURSOR\n\n"
	"B PLACES CURRENT BRUSH\n\n"
	"L&R CHANGE CURRENT BRUSH\n\n"
	"SELECT CHANGES EDIT MODE\n\n"
	"START !!SAVES TO DISK!! &\n"
	"TESTS CURRENT ROOM\n\n\n\n"
	"NEXT...";

const char editor2Str[] PROGMEM =
	"THERE ARE 2 EDIT MODES\n"
	"-OBJECT- AND -LINK- MODE\n\n"
	"OBJECT MODE ALLOWS THE\n"
	"PLACEMENT OF WALLS ITEMS \n"
	"AND ENEMIES\n\n"
	"LINK MODE SETS THE ROOM\n"
	"CONNECTIONS FOR NORTH EAST\n"
	"SOUTH AND WEST EXITS. THIS\n"
	"DETERMINES THE ACTUAL SHAPE\n"
	"OF YOUR WORLD. ROOM NUMBERS\n"
	"BY THEMSELF ARE ARBITRARY.\n"
	"THE PLAYER ALWAYS STARTS IN\n"
	"ROOM 0 SO PLAN ACCORDINGLY.\n\n"
	"!!TAKE CAUTION TO AVOID   !!\n"
	"!!EXTRANEOUS SD CARD WEAR !!\n"
	"!!DO NOT SAVE NEEDLESSLY  !!";
	

#define SOUND_GOLD		0
#define SOUND_JUMP		1
#define SOUND_HURT		2
//#define SOUND_GAME_OVER	3
#define SOUND_MENU_MOVE	3



#define NUM_FONT_TILES		(44)
#define FIRST_NUMERIC_TILE	(33)
#define FIRST_ALPHA_TILE	(0)
#define NUM_MAP_TILES		(53+24)
#define FIRST_MAP_TILE		(NUM_FONT_TILES)
#define FIRST_BORDER_TILE	(NUM_FONT_TILES+NUM_MAP_TILES)
#define FIRST_HUD_TILE		(FIRST_BORDER_TILE+12)
#define TILE_EMPTY			(0+FIRST_MAP_TILE)
#define TILE_DOOR			(1+FIRST_MAP_TILE)//6
#define TILE_SPIKES			(5+FIRST_MAP_TILE)//7
#define TILE_HEART_BIG		(9+FIRST_MAP_TILE)//15
#define TILE_HEART			(13+FIRST_MAP_TILE)//13
#define TILE_WALL			(17+FIRST_MAP_TILE)//16
#define TILE_PRINCESS		(21+FIRST_MAP_TILE)//17		// tiles 17-18
#define TILE_LADDER			(29+FIRST_MAP_TILE)//19
#define TILE_KEY			(33+FIRST_MAP_TILE)//22
#define TILE_GOLD			(37+FIRST_MAP_TILE)//27		// tiles 27-28
#define TILE_WALL_DARK		(45+FIRST_MAP_TILE)//31
#define TILE_UNUSED_WINDOW	(49+FIRST_MAP_TILE)//
//EDITOR TILES
#define TILE_GHOST_MOVE_LEFT		(53+FIRST_MAP_TILE)
#define TILE_GHOST_MOVE_RIGHT		(TILE_GHOST_MOVE_LEFT+4)
#define TILE_WYVERN_MOVE_UP		(TILE_GHOST_MOVE_RIGHT+4)
#define TILE_WYVERN_MOVE_DOWN		(TILE_WYVERN_MOVE_UP+4)
//OLD MAP FORMAT VALUES(NEVER CONVERTED IT)
#define TILE_GHOST_LEFT			23		// tiles 23-24
#define TILE_GHOST_RIGHT		25		// tiles 25-26
#define TILE_WYVERN				29		// tiles 29-30
#define TILE_WYVERN_2ND			32		// wyvern (sprite index 1)
#define TILE_GHOST_LEFT_2ND		33		// ghost (sprite index 2)
#define SPRITE_GHOST			6//24
#define SPRITE_WYVERN			8//32






const uint8_t editor_save_conversion_table[] PROGMEM = {//a cryptic way to do this. Convert vram tile indices back to map format data
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,//first 44 tiles are font


 //tile 44, first map tile
 1,1,1,1,//TILE_EMPTY
 7,7,7,7,//TILE_DOOR
 10,10,10,10,//TILE_SPIKES
 14,14,14,14,//TILE_HEART_BIG
 14,14,14,14,//TILE_HEART_SMALL
 2,2,2,2,//TILE_WALL
 6,6,6,6,//TILE_PRINCESS
 4,4,4,4,//TILE_LADDER
 3,3,3,3,//TILE_KEY
 5,5,5,5,//TILE_GOLD
 0,0,0,0,//TILE_WALL_DARK
 0,0,0,0,//TILE_UNUSED_WINDOW

 //border tiles
0,0,0,0,0,0,0,0,0,0,0,0,



};

const uint8_t editor_brushes[] PROGMEM = {TILE_WALL,TILE_WALL_DARK,TILE_LADDER,TILE_DOOR,TILE_SPIKES,TILE_KEY,TILE_HEART_BIG, TILE_UNUSED_WINDOW, TILE_PRINCESS, TILE_WYVERN, TILE_GHOST_MOVE_LEFT,TILE_GHOST_MOVE_RIGHT,TILE_WYVERN_MOVE_UP,TILE_WYVERN_MOVE_DOWN};

const uint8_t roomNibbleToByte[] PROGMEM = {
TILE_WALL_DARK,
TILE_EMPTY,
TILE_WALL,
TILE_KEY,
TILE_LADDER,
TILE_GOLD,
TILE_PRINCESS,
TILE_DOOR,
TILE_WYVERN,
TILE_WYVERN_2ND,
TILE_SPIKES,
TILE_GHOST_RIGHT,
TILE_GHOST_LEFT,
TILE_GHOST_LEFT_2ND,//TODO ELIMINATE THIS, WAS ONLY NEEDED FOR SPRITE ROTATION ON ORIGINAL SYSTEM????
TILE_HEART,

//	31,0,16,22,19,27,17,6,29,32,7,25,23,33,13,
};


#define TIME_SPEEDUP			30


#define WINDOW_X_OFF			2
#define WINDOW_Y_OFF			3

#define HUD_X_OFF				WINDOW_X_OFF
#define HUD_Y_OFF				WINDOW_Y_OFF
//#define FIRST_HUD_TILE 0

#define NUM_TILES_X					13
#define NUM_TILES_Y					10

#define SCREEN_WIDTH				(NUM_TILES_X*2*8)
#define SCREEN_HEIGHT				(NUM_TILES_Y*2*8)
 



uint8_t numEnemies;

uint8_t GetTileLinear(uint16_t off){
	return vram[off]-RAM_TILES_COUNT;
}


void print(uint8_t x, uint8_t y, const char* text);

uint8_t princessTimer;
uint8_t frame;



void initEnemy(uint8_t x, uint8_t y, uint8_t tile);




#define MAX_HEALTH	3

typedef struct{
	uint8_t		frame;
	int16_t		x, y;
	int8_t		dir;		// -1 = left, 1 = right
	uint8_t		walkPhase;
	uint8_t		room;
	uint16_t	score;
	uint8_t		health;
	uint32_t	time;		// 8.8
	int16_t		vely;		// 3.5
	uint16_t		jumpTimer;
	bool		climbing;
	bool		hasLanded;
	bool		airBorne;
	uint8_t		climbPhase;
	uint8_t		hurtTimer;
	uint8_t		gameover;			// 0 = game not over, 1 = game over, 2 = game won
	uint8_t		startRoom;
	uint8_t		startX, startY;
}Player;


Player p;


void updateMoving();
void updateClimbing();
void updatePickup();
void updateJumpAndFall();
void updateCurrentRoom();
void updateHurt();
void updateTime();




#define ROOM_SIZE		(NUM_TILES_X*(NUM_TILES_Y-1))//13*9
#define MAX_ROOMS 96//128

uint8_t room_adjacent[4];
uint8_t sd_episode;

uint8_t roomstate[MAX_ROOMS];



void initRoom(uint8_t room);
uint8_t getAdjacentRoom(uint8_t room, uint8_t adj);	// 0 = left, 1 = right, 2 = up, 3 = down
void drawText(uint8_t x, uint8_t y, const char* text);

void clearRoomState();
void storeRoomState(uint8_t room);
void restoreRoomState(uint8_t room);

int16_t min(int16_t v1, int16_t v2){
	if(v1 < v2)
		return v1;
	else
		return v2;
}

void updateSprite(uint8_t s, uint8_t t, bool flip, uint8_t x, uint8_t y){

	t <<= 2;
	
	uint8_t flags = flip ? SPRITE_FLIP_X:0;
	if(!flags){
		sprites[s].x = x;
		sprites[s].y = y;
		sprites[s].flags = flags;
		sprites[s++].tileIndex = t++;
		sprites[s].x = x+8;
		sprites[s].y = y;
		sprites[s].flags = flags;
		sprites[s++].tileIndex = t++;
		sprites[s].x = x;
		sprites[s].y = y+8;
		sprites[s].flags = flags;
		sprites[s++].tileIndex = t++;
		sprites[s].x = x+8;
		sprites[s].y = y+8;
		sprites[s].flags = flags;
		sprites[s].tileIndex = t;
	}else{
		sprites[s].x = x+8;
		sprites[s].y = y;
		sprites[s].flags = flags;
		sprites[s++].tileIndex = t++;
		sprites[s].x = x;
		sprites[s].y = y;
		sprites[s].flags = flags;
		sprites[s++].tileIndex = t++;
		sprites[s].x = x+8;
		sprites[s].y = y+8;
		sprites[s].flags = flags;
		sprites[s++].tileIndex = t++;
		sprites[s].x = x;
		sprites[s].y = y+8;
		sprites[s].flags = flags;
		sprites[s].tileIndex = t;

	}
}








void initEnemies();
void hideSprites(uint8_t start, uint8_t end);
void newgame();

const char charset[] PROGMEM = " ABCDEFGHIJKLMNOPQRSTUVWXYZ!&'Ä-.0123456789?";//"ABCDEFGHIJKLMNOPQRSTUVWXYZ!&'(),-.0123456789?";

uint8_t font_tile_offset;

uint8_t charToTile(uint8_t ch) {
	uint8_t i;
	for(i = 0; i < sizeof(charset); i++)
		if((pgm_read_byte(&charset[i])) == ch)
			break;
	return i+font_tile_offset;
}

void SetMapTile(uint8_t x, uint8_t y, uint8_t t){

	if(t == FIRST_MAP_TILE){

		vram[((y)*VRAM_TILES_H)+x+0]	= t+RAM_TILES_COUNT;
		vram[((y)*VRAM_TILES_H)+x+1]	= t+RAM_TILES_COUNT;
		vram[((++y)*VRAM_TILES_H)+x++]	= t+RAM_TILES_COUNT;
		vram[((y)*VRAM_TILES_H)+x+0]	= t+RAM_TILES_COUNT;

	}else{

		vram[((y)*VRAM_TILES_H)+x+0] 	= RAM_TILES_COUNT+t++;
		vram[((y)*VRAM_TILES_H)+x+1] 	= RAM_TILES_COUNT+t++;
		vram[((++y)*VRAM_TILES_H)+x++]	= RAM_TILES_COUNT+t++;
		vram[((y)*VRAM_TILES_H)+x+0]	= RAM_TILES_COUNT+t;
	}
}


void SetMapTileDirect(uint8_t x, uint8_t y, uint8_t t){

	
	if(t == FIRST_MAP_TILE){
		vram[((y)*VRAM_TILES_H)+x+0] = t+RAM_TILES_COUNT;
		//t++;
		vram[((y)*VRAM_TILES_H)+x+1] = t+RAM_TILES_COUNT;
		//t++;
		vram[((++y)*VRAM_TILES_H)+x++] = t+RAM_TILES_COUNT;
		//t++;
		vram[((y)*VRAM_TILES_H)+x+0] = t+RAM_TILES_COUNT;
	}else{

		vram[((y)*VRAM_TILES_H)+x+0] = t+RAM_TILES_COUNT;
		t++;
		vram[((y)*VRAM_TILES_H)+x+1] = t+RAM_TILES_COUNT;
		t++;
		vram[((++y)*VRAM_TILES_H)+x++] = t+RAM_TILES_COUNT;
		t++;
		vram[((y)*VRAM_TILES_H)+x+0] = t+RAM_TILES_COUNT;
	}
}



void DrawWindowFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
	
	//draw horizontal span
	for(uint8_t i=x+1;i<x+1+(w-1);i++){
		uint8_t t;
		if(i == x+1)
			t = FIRST_BORDER_TILE+1;
		else if(i == x+(w-1))
			t = FIRST_BORDER_TILE+3;
		else
			t = FIRST_BORDER_TILE+2;

		SetTile(i,y,t);
		SetTile(i,y+h,t);
	}

	//draw vertical run
	for(uint8_t i=y+1;i<y+1+(h-1);i++){
		uint8_t t;
		if(i == y+1)
			t = FIRST_BORDER_TILE+4;
		else if(i == y+(h-1))
			t = FIRST_BORDER_TILE+6;
		else
			t = FIRST_BORDER_TILE+5;

		SetTile(x,i,t);
		SetTile(x+w,i,t);
	}

	//draw corners
	SetTile(x,y,FIRST_BORDER_TILE+0);
	SetTile(x+w,y,FIRST_BORDER_TILE+0);
	SetTile(x,y+h,FIRST_BORDER_TILE+0);
	SetTile(x+w,y+h,FIRST_BORDER_TILE+0);

}


uint8_t pauseMenu();
void ShowHighScores();

uint32_t episode_base_time_limit;//TODO, WHY ISN'T THIS CONVERTING RIGHT WHEN uint16_t????

uint8_t trackNo;
#define NUM_MUSIC_TRACKS	1
extern unsigned char ram_tiles[];
extern struct BgRestoreStruct ram_tiles_restore[];

uint8_t music_fade_delay;
uint8_t music_fade_frames;
uint8_t music_fade_step;

void FadeMusicOut(uint8_t frames, uint8_t step){
	music_fade_frames = frames;
	music_fade_step = step;
}

extern void TriggerCommon(Track* track,u8 patch,u8 volume,u8 note);

const uint8_t default_high_score_data[] PROGMEM = {
 1,12, 5, 3, 0, 0, 0, 0,0x0A,0xF0,//"ALEC"		2800
 3,21,14,14, 9,14, 7, 0,0x0A,0x8C,//"CUNNING"	2700
 6,12, 5, 3,11,15, 0, 0,0x0A,0x28,//"FLECKO"	2600
10,21, 2, 1,20, 9, 1,14,0x09,0xC4,//"JUBATIAN"	2500
14,5, 2, 15,19,15,19,15,0x09,0x60,//"NEBOSOSO"	2400
14, 9, 3,11, 0, 0, 0, 0,0x08,0xFC,//"NICK"		2300
16, 1,21,12, 0, 0, 0, 0,0x08,0x98,//"PAUL"		2200
};//function will read past this, but it doesn't matter


void TQ_triggerFx(uint8_t patch){//use the 5th channel exclusively to allow music(uses 1-4) to be uninterrupted

	Track* track=&tracks[4];
	tracks[4].flags|=TRACK_FLAGS_PRIORITY;
	track->patchCommandStreamPos = NULL;
	TriggerCommon(track,patch,255,80);
	track->flags|=TRACK_FLAGS_PLAYING;

}



void changeRoom(uint8_t room);




// sine table for wyvern movement
const char sintab[] PROGMEM = {
16-16,17-16,18-16,18-16,19-16,20-16,21-16,21-16,22-16,23-16,24-16,24-16,25-16,26-16,26-16,27-16,
27-16,28-16,28-16,29-16,29-16,30-16,30-16,30-16,31-16,31-16,31-16,32-16,32-16,32-16,32-16,32-16,
32-16,32-16,32-16,32-16,32-16,32-16,31-16,31-16,31-16,30-16,30-16,30-16,29-16,29-16,28-16,28-16,
27-16,27-16,26-16,26-16,25-16,24-16,24-16,23-16,22-16,21-16,21-16,20-16,19-16,18-16,18-16,17-16,
16-16,15-16,14-16,14-16,13-16,12-16,11-16,11-16,10-16,9-16,8-16,8-16,7-16,6-16,6-16,5-16,
5-16,4-16,4-16,3-16,3-16,2-16,2-16,2-16,1-16,1-16,1-16,0-16,0-16,0-16,0-16,0-16,
0-16,0-16,0-16,0-16,0-16,0-16,1-16,1-16,1-16,2-16,2-16,2-16,3-16,3-16,4-16,4-16,
5-16,5-16,6-16,6-16,7-16,8-16,8-16,9-16,10-16,11-16,11-16,12-16,13-16,14-16,14-16,15-16,
/*old for 8x8 objects
	0,1,2,2,3,4,4,5,6,6,7,7,7,8,8,8,8,8,8,8,7,7,7,6,6,5,4,4,3,2,2,1,0,-1,-2,-2,-3,-4,
	-4,-5,-6,-6,-7,-7,-7,-8,-8,-8,-8,-8,-8,-8,-7,-7,-7,-6,-6,-5,-4,-4,-3,-2,-2,-1
*/
};


void ClearWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h){

	for(uint8_t i=x;i<=x+w;i++)
		for(uint8_t j=y;j<=y+h;j++)
			vram[i+(j*VRAM_TILES_H)] = RAM_TILES_COUNT;//clear the screen with black tile

}



void updateScoreBar();

#define SD_MAX_HIGH_SCORE_ENTRIES		(uint32_t)(7UL)//per episode
#define SD_HIGH_SCORE_ENTRY_SIZE		(uint32_t)(8UL+2UL)//EEPROM depreciated...8 bytes name, 2 bytes score
#define SD_SECTOR_SIZE					(uint32_t)(512UL)//PFF writes a whole sector at a time, if you don't specify part of the sector, it will be overwritten with 0
#define SD_MAX_OPEN_ATTEMPTS			(uint32_t)(16UL)//No idea why, but this routinely fails a couple time for some people's SD?
#define SD_MAX_EPISODES					(uint32_t)(5UL)//Arbitrary, but let us not get crazy
#define SD_FIRST_GFX_OFFSET				(uint32_t)(0x200UL)//first byte of first SD vram map
#define SD_GFX_ENTRY_SIZE				(uint32_t)(0xC80UL)//((30UL*28UL)+(36UL*64UL))
#define SD_GFX_SIZE						(uint32_t)(30UL*28UL)+(RAM_TILES_COUNT*64)+5UL//all the graphics for expanded intro animation
#define SD_DEMO_SIZE					(uint32_t)(1024UL)//loaded into 3 ram tiles, buffer filled during room changes or whenever needed so don't stick around in rooms forever
#define SD_EPISODE_HEADER_OFFSET		(uint32_t)(0UL)
#define SD_EPISODE_HEADER_SIZE			(uint32_t)(32UL*SD_MAX_EPISODES)//up to 16 characters, not ASCII encoded. See font bitmap for index to char values(0 is ' ', 1 is 'A', etc)
#define SD_ROOM_SIZE					(uint32_t)(128UL)//levels are not compressed on SD to eliminate scanning. Adjacent room list is immediately after, padded out to multiples of 13(NUM_TILES_X)
#define SD_EPISODE_SIZE					(uint32_t)(0x5000UL)//(0x1000ULL+((MAX_ROOMS/4ULL)*SD_SECTOR_SIZE))//only 2 rooms per sector, faster reading, easier writing
#define SD_FIRST_EPISODE_OFFSET			(uint32_t)(0x5000UL)
#define SD_TQ_HIGH_SCORE_SLOT			(uint32_t)(16UL)//each slot is 512 bytes
#define SD_TQ_HIGH_SCORE_OFFSET			(uint32_t)(512UL*SD_TQ_HIGH_SCORE_SLOT)//the actual first byte in "_HISCORE.DAT" that is reserved for TQ
#define SD_EPISODE_HIGH_SCORE_LIST_SIZE	(uint32_t)(SD_MAX_HIGH_SCORE_ENTRIES*SD_HIGH_SCORE_ENTRY_SIZE)

#define SD_SAVE_FILE_NAME				"_HISCORE.DAT"
#define SD_DATA_FILE_NAME				"TOORUMSQ.DAT"

FATFS fs;//worth keeping around for speed of not opening each time

uint8_t demo_playing,demo_hold_pad,demo_pad_state;
uint16_t demo_pos;
//uint8_t demobuffer[188];

void drawTextRam(uint8_t x, uint8_t y, uint8_t* text);
void DrawMenuBG();
void UzeboxIntro();
void SD_InitializeHighScore();

char charbuf[SD_HIGH_SCORE_ENTRY_SIZE-2];

void WriteSD(uint32_t offset, uint16_t len, unsigned char *data);

const uint8_t demo_data[] PROGMEM = {//took many runs to get this and I still made a small mistake. Edited a few things manually to be smaller.

0x80,0xc0,0x81,0x03,0x80,0x14,0x81,0x03,0x80,0x32,0x81,0x1e,0x80,0x03,
0x00,0x01,0x40,0x27,0x80,0xa1,0x81,0x02,0x80,0x52,0x81,0x02,0x80,0xb6,
0x81,0x17,0x80,0x02,0x00,0x9d,0x40,0x89,0x41,0x14,0x40,0xed,0x00,0x0f,0x40,0x17,
0x00,0x68,0x40,0x27,0x00,0x02,0x80,0xee,0xa0,0x03,0x20,0x15,0x60,0x0c,0x40,0x34,
0x00,0x38,0x40,0x02,0x41,0x0f,0x40,0x40,0x80,0x01,0x81,0x16,0x80,0x04,
0x00,0x01,0x40,0x0e,0x00,0x03,0x80,0x27,0x00,0x66,0x80,0x6e,0x90,0x02,0x10,0x1d,
0x50,0x05,0x40,0x02,0x80,0x06,0xa0,0x05,0x20,0x11,0x60,0x14,0x40,0x1e,0x41,0x04,
0x40,0x32,0x00,0x12,0x40,0x23,0x41,0x03,0x40,0x2f,0x40,0x0a,0x41,0x0b,
0x01,0x03,0x41,0x0a,0x40,0x0d,0x41,0x11,0x80,0x3c,0x81,0x03,0x80,0x39,
0x81,0x17,0x80,0x01,0x00,0x82,0x40,0x09,0x00,0x08,0x80,0x2f,0xa0,0x04,0x20,0x12,
0x00,0x2d,0x20,0x18,0x60,0x09,0x40,0x0e,0x00,0x0a,0x40,0x04,0x60,0x08,0x20,0x0c,
0x00,0x62,0x10,0x10,0x50,0x06,0x40,0x1c,0x41,0x0f,0x40,0x09,0x00,0xd3,0x80,0x01,
0x81,0x11,0x80,0x26,0xa0,0x05,0x20,0x18,0x60,0x02,0x40,0x02,0x00,0x32,0x60,0x1f,
0x40,0x14,0x00,0x02,0x80,0xbd,0x81,0x03,0x80,0x11,0x81,0x03,0x80,0x11,0x81,0x02,
0x80,0x16,0x81,0x04,0x80,0x38,0x00,0x5d,0x80,0x04,0x81,0x25,0x80,0x14,0x00,0x09,
0x40,0x05,0x00,0x05,0x40,0x70,0x80,0xff,0x80,0x11,0x91,0x0e,0x90,0x01,
0x10,0x09,0x50,0x0f,0x40,0x08,0x00,0x0b,0x80,0x01,0x81,0x04,0x91,0x03,0x11,0x01,
0x51,0x10,0x40,0x01,0x00,0x5c,0x40,0x04,0x41,0x12,0x40,0x12,0x41,0x16,0x40,0x0d,
0x00,0x04,0x80,0x05,0x00,0x93,0x81,0x14,0x80,0x11,0x81,0x0b,0x80,0x14,0x81,0x10,
0x90,0x0c,0x10,0x1b,0x50,0x06,0x40,0x36,0x50,0x03,0x10,0x0f,0x00,0x86,0x20,0x06,
0x60,0x17,0x40,0x39,0x41,0x01,0x01,0x05,0x00,0x0f,0x80,0x4a,0x90,0x03,0x10,0x22,
0x00,0x04,0x20,0x26,0xa0,0x03,0x80,0x48,0x81,0x0c,0x80,0x38,0x81,0x02,0x80,0x24,
0x81,0x07,0x80,0x1f,0x00,0x05,0x01,0x01,0x81,0x15,0x80,0x01,0x00,0x92,0x01,0x04,
0x41,0x14,0x01,0x03,0x00,0x09,0x40,0x04,0x00,0x07,0x40,0x52,0x00,0x01,0x80,0x01,
0x81,0x19,0x80,0x04,0x00,0x02,0x40,0x03,0x41,0x1f,0x40,0x61,0x50,0x03,0x10,0x60,
0x50,0x04,0x40,0x0e,0x00,0x3a,0x40,0x0c,0x00,0x15,0x01,0x01,0x81,0x15,0x80,0x67,
0x00,0x0f,0x01,0x06,0x51,0x0b,0x50,0x04,0x10,0x2e,0x50,0x04,0x40,0x44,0x00,0x39,
0x40,0x0c,0x41,0x0c,0x40,0x27,0x41,0x0a,0x40,0x5c,0x00,0x05,0x80,0x0d,0x00,0x01,
0x40,0x32,0x00,0x01,0x80,0x06,0x00,0x04,0x01,0x0d,0x41,0x09,0x40,0x02,0x00,0x11,
0x01,0x06,0x41,0x14,0x40,0x1f,0x00,0x10,0x40,0x10,0x00,0x58,0x01,0x05,0x81,0x14,
0x80,0x06,0x00,0x05,0x01,0x04,0x41,0x14,0x00,0x06,0x80,0x03,0x81,0x0b,0x80,0x06,
0x00,0x0a,0x40,0x07,0x00,0x0c,0x40,0x12,0x00,0x05,0x80,0x1a,0xa0,0x0e,0x20,0x10,
0x00,0x0b,0x20,0x01,0x60,0x11,0x40,0x2c,0x41,0x0f,0x40,0x13,0x00,0x03,0x80,0x1a,
0x40,0x86,0x41,0x14,0x40,0x5e,0x00,0x10,0x40,0x09,0x00,0x08,0x40,0x13,0x00,0x42,
0x40,0x08,0x00,0x06,0x40,0x08,0x00,0x06,0x40,0x08,0x41,0x19,0x40,0x05,0x41,0x15,
0x40,0x60,0x41,0x1a,0x40,0x03,0x41,0x06,0x40,0x2c,0x00,0x2d,0x40,0x3a,0x00,0x02,
0x80,0x03,0x00,0x34,0x80,0x0e,0x81,0x1c,0x80,0x24,0x00,0x05,0x01,0x01,0x41,0x14,
0x40,0x05,0x00,0x0a,0x80,0x01,0x81,0x19,0x80,0x36,0x81,0x0e,0x80,0x17,0x81,0x04,
0x80,0x0f,0x00,0x02,0x01,0x05,0x41,0x15,0x40,0x11,0x50,0x03,0x10,0x21,0x50,0x08,
0x10,0x01,0x00,0x1d,0x10,0x02,0x90,0x08,0x00,0x2c,0x40,0x02,0x00,0xff,0x00,0x16,
0x40,0x0a,0x00,0x75,0x40,0x5b,0x50,0x01,0x51,0x1d,0x50,0x04,0x10,0x17,0x50,0x0a,
0x10,0x5e,0x90,0x0b,0x80,0x07,0x00,0x03,0x40,0x0d,0x60,0x09,0x20,0x37,0x00,0x05,
0x80,0x04,0x00,0x0e,0x80,0x02,0x90,0x0e,0x80,0x08,0x00,0x2f,0x80,0x01,0x81,0x14,
0x80,0x04,0x00,0x03,0x80,0x05,0x00,0x04,0x80,0x09,0x81,0x24,0x80,0x08,0x00,0x2b,
0x80,0x08,0x81,0x24,0x80,0x59,0x00,0x26,0x40,0x06,0x00,0x0b,0x40,0x03,0x00,0x0c,
0x40,0x05,0x00,0x03,0x80,0x08,0x81,0x08,0x80,0x03,0x00,0x08,0x80,0x08,0x81,0x1a,
0x80,0x10,0x90,0x0c,0x10,0x15,0x90,0x07,0x80,0x14,0x91,0x02,0x11,0x0b,0x10,0x09,
0x90,0x01,0x80,0x06,0x00,0x31,0x20,0x0d,0xa0,0x05,0x80,0x0f,0x40,0x17,0x50,0x01,
0x51,0x02,0x11,0x04,0x10,0x10,0x50,0x02,0x40,0x03,0x00,0x74,0x20,0x0c,0x60,0x07,
0x40,0x12,0x60,0x03,0x20,0x17,0x60,0x0a,0x40,0x01,0x00,0x1f,0x40,0x14,0x00,0x03,
0x80,0x18,0xa0,0x01,0x20,0x1d,0xa0,0x03,0x80,0x03,0x00,0x06,0x40,0x07,0x50,0x01,
0x10,0x11,0x00,0x39,0x10,0x11,0x50,0x03,0x40,0x13,0x41,0x06,0x40,0x07,0x00,0x0d,
0x40,0x07,0x41,0x11,0x40,0x0b,0x00,0x0c,0x40,0x08,0x41,0x11,0x40,0x05,0x00,0x10,
0x81,0x19,0x80,0x0e,0x90,0x03,0x10,0x38,0x50,0x05,0x40,0x0f,0x41,0x0c,0x40,0x01,
0x41,0x0b,0x40,0x04,0x41,0x01,0x40,0x1e,0x41,0x02,0x40,0x06,0x00,0x5f,0xff,

};

