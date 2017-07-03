////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************FUNCTION DEFINITIONS*******************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//user interface////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t ShowHighScoreScreen(uint8_t shortcircuit);
void HighScoreEntryScreen();
void TitleScreen();
void UzeboxLogo();
//logical///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t DDWaitVsyncEx(uint8_t t, bool cancel);
inline uint8_t DDWaitVsyncWithCancel(uint8_t);
inline void DDWaitVsync(uint8_t t);
void PushLevelState();
void PopLevelState();
void InitEnemy(uint8_t x, uint8_t y, uint8_t t);
void NullEnemiesAndRocks();
void InitLevel();
void VsyncRoutine();
void StartRound();
bool UpdateRocks(bool draw_only);
uint8_t UpdatePlayer(bool draw_only);
void UpdateEnemies(bool draw_only);
void GameLoop();
uint8_t GetMapTileType(uint8_t x, uint8_t y);
void DDSetVram(uint8_t x, uint8_t y, uint8_t v);
void DDClearVram();
uint8_t DDGetVram(uint8_t x, uint8_t y);
void Pause();
bool CanSupportRock(uint8_t x, uint8_t y);
void StartDemo(uint8_t d);
void CarveTunnel(uint8_t pos, uint8_t dir, uint8_t flags);
void NetworkStateMachine();
uint8_t DDRandom();
void DDCrash();//debugging
void AddScore(uint16_t s);
//graphical/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DrawMetaTileMap(uint8_t x, uint8_t y, uint8_t t);
void DrawMetaTileMapOffset(uint8_t x, uint8_t y, uint8_t t, uint8_t o);
void DDPrint(uint8_t x, uint8_t y, const char *s);
void ResetSprites();
void DrawBonus();
void DrawFlowers();
void SetSprite(uint8_t x, uint8_t y, uint8_t t, uint8_t f);
void DrawSidePanel();
void DrawMetaSprite(uint8_t x, uint8_t y, uint8_t f);
void RedrawScore();
void DrawHarpoon();
void DrawBigMetaSprite(uint8_t x, uint8_t y, uint8_t f);
//soundical/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DDTriggerFx(uint8_t p);
bool DDIsEffectPlaying(uint8_t p);
void DDStopSong();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************LOGIC DEFINES**************************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAP_X_OFF			0
#define MAP_Y_OFF			2
#define MAP_WIDTH			12
#define MAP_HEIGHT			12
#define MAX_ENEMIES 		8
#define MAX_TUNNELS 		7
#define MAX_ROCKS			5//per 7800 source code, but we use tiles and only fill rock structures when they fall(using sprites)
#define MAX_ACTIVE_ROCKS 	2//how many rocks can actually be moving
//engine
#define DD_MAX_HIGH_SCORES		8
#define TITLE_SCREEN_TIMEOUT	600//matches the original
#define DIGDUG_EEPROM_ID		0x48
#define DIGDUG_MASTER_VOLUME	192

#define MAP_TILE_EMPTY				0
#define MAP_TILE_TUNNELED_H			1
#define MAP_TILE_TUNNELED_V			2
#define MAP_TILE_TUNNELED_H_DIVET	3
#define MAP_TILE_TUNNELED_V_DIVET	4

#define MAP_TILE_ROCK				16

#define SPRITE_HORIZONTAL_LIMIT		((MAP_X_OFF+(MAP_WIDTH*2))*8)
//////////////////////////////////
#define ENEMY_MOVE_UP			1
#define ENEMY_MOVE_DOWN			2
#define ENEMY_MOVE_LEFT			4
#define ENEMY_MOVE_RIGHT		8
#define ENEMY_GHOST				16
#define ENEMY_ATTACKING			32
//#define ENEMY_INFLATED			128//deduced from e_attrb&0b11
#define ENEMY_SQUISHED			(ENEMY_MOVE_LEFT|ENEMY_MOVE_RIGHT)
#define ENEMY_POPPED			(ENEMY_MOVE_UP|ENEMY_MOVE_DOWN)
#define ENEMY_SHOW_SCORE		(ENEMY_SQUISHED|ENEMY_POPPED)
#define ENEMY_DISABLED			255

#define ENEMY_STUN_TIME			48//how long an enemy is stunned after a hit, if the player does nothing else
///////////////////////////////////
#define PLAYER_DEFAULT_START_X	80
#define PLAYER_DEFAULT_START_Y	96 
#define PLAYER_INTRO_START_X	144//per original, origin adjusted for narrow screen
#define PLAYER_INTRO_START_Y	0
///////////////////////////////////
#define PLAYER_UP					1
#define PLAYER_DOWN					2
#define PLAYER_LEFT					4
#define PLAYER_RIGHT				8
#define PLAYER_DIRECTIONS			(PLAYER_UP|PLAYER_DOWN|PLAYER_LEFT|PLAYER_RIGHT)
#define PLAYER_MOVING				16
#define PLAYER_PUMPING				32
#define PLAYER_PUMPING_UP			64
#define PLAYER_FEET_RIGHT			128//draw players feet either on left or ride side when vertical, depending on horizontal direction he came from
#define PLAYER_ATTACKING			256
#define PLAYER_DYING				512UL
#define PLAYER_DIED_THIS_ROUND		1024UL
#define PLAYER_DELAY_FRAME			2048UL
#define PLAYER_DIGGING				4096UL
#define PLAYER_P1_INTRO_DONE		8192UL
#define PLAYER_P2_INTRO_DONE		16384UL
#define PLAYER_UPDATE_SCOREBOARD	32768UL
////////////////////////////////////
#define ACTION_BTN			(BTN_B|BTN_Y)
#define DIRECTIONAL_BTN		(BTN_UP|BTN_DOWN|BTN_LEFT|BTN_RIGHT)
////////////////////////////////////
#define HARPOON_SPEED			2
#define HARPOON_REACH			48
#define FIRST_FREE_LIFE_SCORE	1000UL//represented as 10 times this value
#define SECOND_FREE_LIFE_SCORE	6000UL//this is also 1/10 the score shown to player
#define DEFAULT_STARTING_LIVES	2
////////////////////////////////////
//squeezing bytes here, upper 4 bits are a frame(from which state is derived), and the lower 4 bits are a frame counter
#define ROCK_AGITATED_LAST_FRAME 	3//0..3
#define ROCK_FALLING_FRAME			4//counter means nothing, stays here until we reach crumbling stage
#define ROCK_CRUMBLING_LAST_FRAME	0//5...onward
#define ROCK_FALL_SPEED				2
#define ROCK_DISABLED				0
//lower 5 bits are used for magnitude of frame length, etc
/////////////////////////////////////
//enemy who's state is dead, and who's frame is > 0, is showing a score
//#define GAME_P1_INTRO_DONE		1
//#define GAME_P2_INTRO_DONE		2
//#define GAME_IN_INTRO			4
#define GAME_SONG_NO_INTERRUPT	1
#define GAME_IS_2_PLAYER		4
#define GAME_HURRY_UP			8//down to 1 enemy?
#define GAME_DEMO_PLAYING		16//demo is playing, read input data from flash(happens in game when playing, when round starts and digging to center)
#define GAME_ATTRACT_MODE		32//user sat at title for a while, start a round and control it via recording input
#define GAME_FIRST_TICK_REACHED	64//let monsters know they can move(instead of just draw during the intro)
#define GAME_CHEAT_USED			128//can't register a high score if cheating

//graphical///////////////////////
/////////////////////////////////
#define PANEL_X_OFF				(MAP_X_OFF+(MAP_WIDTH*2))
#define RAM_TILE_BYTE_COUNT		64//TODO change for mode 13
#define LEVEL_NUM_META_TILES	(MAP_WIDTH*MAP_HEIGHT)//size in meta tiles
#define BONUS_X_OFF				10
#define BONUS_Y_OFF				12
#define PAUSE_TEXT_X_OFF		PANEL_X_OFF+1
#define PAUSE_TEXT_Y_OFF		13
#define FIRST_FLOWER_TILE		69
#define FIRST_BONUS_TILE		81

#define BONUS_APPEAR_LENGTH		380//478??

const uint8_t bonus_types[] PROGMEM = {
//0-carrot,1-turnip,2-mushroom,3-cucumber,4-egg plant,5-green pepper,6-tomato,7-garlic,8-watermelon,9-glaxian ship,10-pineapple
0,1,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,};

const uint8_t bonus_score_values[] PROGMEM = {4,6,8,10,10,20,20,30,30,40,40,50,50,60,60,70,70,80};//multiplied by 100

const uint8_t score_per_enemy[] PROGMEM = {//based on earth level, points are doubled for Fygar from side
2,3,4,5,//multiplied by 100, deeper gives more points
};

//network///////////////////////
#define NET_STAY_OFF		0//an error we can't fix happened, don't reboot over and over since it is going to fail again
#define NET_AWAIT_USER		1
#define NET_FIRST_TICK		2
#define NET_START_MODULE	3
#define NET_SEND_LOGIN		4
#define NET_CHECK_LOGIN		5
#define NET_SEND_REQUEST	6
#define NET_PARSE_REQUEST	7
#define NET_HEART_BEAT		8
#define NET_CHECK_RESPONSE	9


#define MUS_ROUND_INTRO		song0
#define MUS_WALKING_LOOP	song1
#define MUS_ROUND_CLEAR		song2
#define MUS_LAST_ENEMY		song3
#define MUS_HURRY_UP		song4
#define MUS_1_UP			song5
#define MUS_GAME_OVER		song6

#define DIRT_UP		1
#define ROCK_UP		2
#define DIRT_DOWN	4
#define ROCK_DOWN	8
#define DIRT_LEFT	16
#define ROCK_LEFT	32
#define DIRT_RIGHT	64
#define ROCK_RIGHT	128

#define PLAYER_WALK_FRAME_LENGTH	4//per original
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************RAM VARIABLES**************************************************************************************************//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t global_frame;
uint8_t padState,oldPadState;
uint8_t demoPadTicks;
uint16_t demoOffset;

uint8_t p_lives;
//^the original:";MAXIMUM OF 9 EXTRA LIVES AT ONCE."
uint8_t p_round;
uint8_t p_x;
uint8_t p_y;
uint8_t p_fine;
uint8_t p_last_dir;
uint8_t p_frame;
uint8_t p_ftime;
uint16_t p_state;
uint8_t p_harpoon_off;
uint8_t p_harpoon_target;
//uint8_t p_grid_restore[1];//goes in ram_tiles now, ram_tile_reserved protects it, and still allows for bigger ram_tile effects when we don't need it
uint8_t p_idle;
uint16_t p_hurry;

uint32_t p_score;
uint32_t highScore;

uint8_t sd_state;

uint8_t rnd;
uint8_t gameState;

//all enemies with index 4 or greater are Fygars
uint8_t global_frame;//used for sprite flickering on enemies, start on 0-7 different frames
uint8_t e_x[MAX_ENEMIES];
uint8_t e_y[MAX_ENEMIES];
uint8_t e_state[MAX_ENEMIES];
uint8_t e_attrb[MAX_ENEMIES];//lowest 2 bits = inflation stage, highest 6 bits = frame timer(for deflating), frame is otherwise based on global frame
uint8_t e_frame[MAX_ENEMIES];
//uint8_t e_ftime[MAX_ENEMIES];
//uint8_t e_restore[2];//keeps track of which enemies are killed between player changes, up to 8 max 1bpe


uint8_t r_x[MAX_ACTIVE_ROCKS];
uint8_t r_y[MAX_ACTIVE_ROCKS];
uint8_t r_state[MAX_ACTIVE_ROCKS];//there is some bit trickery here to save ram, see UpdateRocks()
uint8_t r_fell;//lo nybble is player 1 rocks dropped, hi is player 2

//Only 1 flame per the 7800 source code? did see the NES version do 2 before?
uint8_t flame_owner;
uint8_t flame_frame;

uint8_t sfx_priority;

#define DD_UZENET_ID	1

uint8_t sprite_count;

uint8_t net_state;
uint8_t net_pos;
uint16_t net_delay;

const char net_request_string[] PROGMEM = {
'@','G',DD_UZENET_ID>>8,DD_UZENET_ID&0xff,'E','^',//anonymous login, set game by ID, dig dug id MSB, dig dug id LSB, request echo, of '^'
'Z',UART_RX_BUFFER_SIZE,//use zmodem protocol, don't send us data until we ask for it, and only up to our rx buffer size at a time
'H'//get high score for dig dug, the server "magically" knows the exact format we want(because it is hard coded per game...)
};
const char net_request_answer[] PROGMEM = "+IPD,2:E^";//correct response to our echo request


#define MAP_DIRT		0
#define MAP_EMPTY		1
#define MAP_FYGAR		2
#define MAP_POOKA		3
#define MAP_ROCK		4



#define FIRST_FONT_SPRITE 234
#define FFS FIRST_FONT_SPRITE

const uint8_t round_start_text_sprite_map[] PROGMEM = {
FFS+1	,FFS+2	,FFS+3	,FFS+4	,FFS+5	,FFS+6	,255	,1,//"PLAYER 1" 
255		,FFS+6	,FFS+5	,FFS+3	,FFS+10	,FFS+4	,FFS+0	,255,
};

const uint8_t game_over_text_sprite_map[] PROGMEM = {
FFS+1	,FFS+2	,FFS+3	,FFS+4	,FFS+5	,FFS+6	,255	,1,//"PLAYER 1" 
FFS+11	,FFS+3	,FFS+11	,FFS+5	,FFS+11	,FFS+0	,255,
};


const char default_eeprom[] PROGMEM = {
//lower 16 bits of score are stored after initials, upper 3 bits are OR'ed onto most significant bit of 3 characters giving a 19 bit number
//this score is then multiplied by 10 to give an in game score, with an upper limit of 5,242,880
'K'|0,'B'|0,'Y'|0,0x0C,0x80,
'Z'|0,'D'|0,'R'|0,0x0A,0xF0,
'U'|0,'Z'|0,'E'|0,0x08,0x98,
'C'|0,'N'|0,'F'|0,0x05,0x14,
'T'|0,'L'|0,'P'|0,0x03,0x08,
'L'|0,'E'|0,'E'|0,0x02,0x07,
};

const char eeprom_error[] PROGMEM = {
'E','R','R',0,0,
'O','R',' ',0,0,
'E','R','R',0,0,
'O','R',' ',0,0,
'E','R','R',0,0,
'O','R',' ',0,0,
};

const uint8_t demo_data[] PROGMEM = {
//Uzebox logo demo
BTN_RIGHT,40,
0,60,
BTN_B,1,
0,30,
BTN_B,1,
0,30,
BTN_B,1,
0,30,
BTN_START,1,


//round start demo
0,14,//in original, 14 frames pass before movement
BTN_LEFT,99,
BTN_DOWN,185,
BTN_RIGHT,0,
0,85,
};

const uint8_t p_walk_table[] PROGMEM = {//observed from the original
0b11010110,0b11010110,0b11011010,0b11011010,0b11011011,0b01011011,0b01101011,0b01101011,
0b01101101,0b01101101,0b01101101,0b10101101,0b10110101,0b10110101,0b10110110,0b10110110,
//0b11010110,0b11010110,
};//dig speed is simply 101010101010101010...

const uint8_t p_dig_table[] PROGMEM = {
0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,0b10101010,
};

const uint8_t tile_solidity[] PROGMEM = {//this needs to be updated any time tile layouts are changed
0,
};
