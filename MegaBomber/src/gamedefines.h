#define ARENA_START_X 2//tile offsets for the arena
#define ARENA_START_Y 1

void beep(){
TriggerFx(0,255,true);
}

#define BOT_INITIAL_VARIANCE 8//max initial random weight added to directions
#define BOT_CONTINUE_PREFERENCE 6//weight added to continue last direction
#define BOT_BLOCK_VALUE 2//weight added to a position per explodable block surrounding it
#define BOT_MAX_RANDOM_WEIGHT BOT_INITIAL_VARIANCE+BOT_CONTINUE_PREFERENCE+(BOT_BLOCK_VALUE*3)
#define BOT_WAIT_CHANCE 254
#define BOT_DANGER_WEIGHT 1111
#define BOT_FIRE_HATRED 1222
#define BOT_DANGER_TIME 3*SEC//timer threshold below which a bot considers a bomb dangerous
#define BOT_BOMB_CHANCE 252
#define BOT_BOMB_BLOCK_WEIGHT//weight added to BOMB_CHANCE for each block surrounding the position
#define BOT_CENTER_WEIGHT 5//weight to move towards the center when blocks are dropping


#define MAX_PLAYERS 4
#define NUM_PLAYER_FRAMES 8
#define PLAYER_FRAME_TIME 8

#define BORDER_FRAME_SIZE 52//(15+15+11+11)
#define BORDER_DESCRIPTOR_SIZE 2

#define SEC 60
#define BOMBSTARTTIME 4*SEC
#define BOMBSPEED 4
#define EXPLODETIME 30
#define BOMBTRIGGERDELAY 0//when a bomb would be hit by fire, how long before it explodes
#define STUNLENGTH 255
#define BLOCK_TIME 25

#define NORMAL_VOL 128

#define POWERUPCHANCE 179
#define FIRECHANCE    205
#define SPEEDCHANCE   222
#define BOMBCHANCE    235
#define POISONCHANCE  253
#define KICKCHANCE    254
#define THROWCHANCE   255

extern unsigned char ram_tiles[];
extern unsigned char masterVolume;
extern unsigned char fadeStep;
extern bool fadeActive;
#define FADER_STEPS 12


//input
unsigned int  padstate[MAX_PLAYERS];
unsigned int  oldpadstate[MAX_PLAYERS];
bool multitap = false;

u8 guistate;

//sprite stuff
u8 spritecount;//TODO make these local and get rid of them
u8 rtoverflow;//amount of ram tile required versus actual ram tiles, used to adjust sprite placement
u8 ram_tiles_reserved=0;

#define SPRITE_TILE_SET 0b00000010



#define GMAINMENU        1
#define GCONTINUE        2
#define GINGAMEMENU      4
#define GSETUPMENU       8
#define GMAPSELECTMENU   9
#define GOPTIONMENU      10
#define GUZENETMENU      16
#define GLOBBYMENU       17
#define GPREGAMEMENU     32
#define GUIJUSTSTARTED   64
#define GVICTORYMENU     128

#define ROUNDTIMEBIT0 1
#define ROUNDTIMEBIT1 2
#define ROUNDTIMEBIT2 4
#define NUMROUNDBIT0  8
#define NUMROUNDBIT1  16
#define USETEAM       32
#define USEBLITZ      64
#define USEGHOST      128

u8 gamestate     = 0;
u8 gameoptions = 0b11011011;

#define TICK_ENDED 1//set when all processing is done, checked at vsync and TICK_MISSED set if not set
#define TICK_MISSED 2//if tick was missed, skip logic to catch up
u8 tick_tracker=TICK_ENDED;

//preference flags
#define ALLOWGHOSTS 1
#define SUDDENDEATH 2//allow sudden death


//state flags
#define BLINK       1
#define PLAYING     2
#define MAXFIRE     4//all players have maximum fire
//#define QUICKTIME   8//we are in sudden death
#define STARTMATCH  16
#define GHOSTBOMBER 32
#define VICTORY     64
//#define FADEMUSIC   128
#define BOT_THOUGHT 128//1 bot has thought this tick, thats enough!!

u8 level = 0;
//u8 renderflags = 0;


//bool blink;
u8 blip=2;//hits 0 every 2 frames, used for player speed
u8 timecost;//how expensive have the calculation been so far, skip AI etc after cutoff

u8 prng = 7;
//u8 rammap[13*11];

#define MAX_BOMBS 16

#define EXPLODE 128

u8 bomb_x     [MAX_BOMBS];
u8 bomb_y     [MAX_BOMBS];
u8 bomb_offset[MAX_BOMBS];
u8 bomb_state [MAX_BOMBS];
u8 bomb_timer [MAX_BOMBS];
u8 bomb_owner [MAX_BOMBS];
u8 bomb_xtra [MAX_BOMBS];


u8 player_x      [MAX_PLAYERS];
u8 player_y      [MAX_PLAYERS];
u8 player_wait   [MAX_PLAYERS];//how many ticks to wait before player offset changes(AKA speed)
u8 player_offset [MAX_PLAYERS];
u8 player_state  [MAX_PLAYERS];
u8 player_attrb  [MAX_PLAYERS];//bits: 7-boot  ,6-glove, 5-team2 , 4- , 3-poison LSB, 2-fire range MSB, 1-fire range, 0-fire range LSB (range is num+3)
u8 player_attrb2 [MAX_PLAYERS];//bits: 7-ghost ,6-invuln. MSB 5-invuln. LSB, bombs MSB 4-bombs 3-bombs LSB 2-speed MSB 1-speed 0-speed LSB
u8 player_frame  [MAX_PLAYERS];
u8 player_ftime  [MAX_PLAYERS];
u8 poison_type;
//Attributes

#define POISON 0b00001000
#define TEAM2  0b00100000
#define GLOVE  0b01000000
#define BOOT   0b10000000

//Poison types
#define POISON_NUM_TYPES 6
#define POISON_CHANGE_CHANCE 200//once a second, if prand() > POISON_CHANCE_CHANCE change poison type

#define POISON_SLOW 1
#define POISON_FAST 2
#define POISON_RANGE 3
#define POISON_REVERSE 4
#define POISON_NOBOMBS 5
#define POISON_AUTOBOMB 6



u8 botgoal       [MAX_PLAYERS];//current ai goal, must never be 0 because ai will never fill pad after that
//u8 bot_dir[4];//weights for up,down,left,right
u8 bot_targetx   [MAX_PLAYERS];
u8 bot_targety   [MAX_PLAYERS];
u8 numplayers;
u8 score;//2 bits per player
u8 playerslots;//2 bits per player 2-MAN 1-CPU 0-OFF
u8 block_minx,block_miny,block_maxx,block_maxy;
//u8 spritebase = 0;//used to define an offset into frame table, for using different frame per level(legs in water for instance)

unsigned int roundtime;

u8 block_dir;
u8 block_x;
u8 block_y;
u8 block_z;
u8 block_off;
u8 block_time;

#define UP      1
#define DOWN    2
#define LEFT    4
#define RIGHT   8
#define BOMB    16
#define MOVE    32
#define GRAB    64//pick up bomb
#define STUNNED 128
#define DYING   255

//special bomb states for throwing
#define THROWN 16
#define BLOCKED 64//A block(low time) fell on this bomb, make sure not to redraw TODO NOT NEEDED?
#define STEPPED_OFF 128//owner has totally stepped off the bomb TODO NOT NEEDED??
//#define FALLING 128


//Attributes 2

#define GHOST 128



//AI Goals
#define AVOIDFIRE 1
#define DESTROYBRICK 2
#define AVOIDPLAYER 4
#define SEEKPLAYER 8
#define SEEKPOWERUP 16
#define CHOOSEGOAL  32

//Powerup Types
#define PFIRE 0
#define PBOMB 1
#define PBOOT 2
#define PGLOVE 3
#define PSPEED 4
#define PVEST  5


typedef struct{
   u8 frame,ftime;
   u8 base,max;
} border_;

border_ border;

extern bool playSong;

#include "functiondefs.h"
