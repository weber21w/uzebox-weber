#include <ctype.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>
#include "data/tiles.inc"
#include "data/sprites.inc"
#include "data/title.inc"
#include "data/patches.inc"
#include "data/music.inc"
#include "data/palettes.inc"
//#include <sdBase.h>

#include "data/maps.inc"
#include "data/messages.inc"

void dbf(){while(1){TriggerFx(1,255,1);}}
extern u8 tile_bank;
extern u8 ram_tiles[];
extern u8 palette[256];
extern const u8 PaletteStandardToExtendedTable[];
extern const u8 PaletteEncodingTable[];
extern const u8 PaletteExtendedToStandardTable[];

#define MAP_WDT		32
#define MAP_WDT_BIT	5
#define MAP_HGT		22
#define SCREEN_WIDTH	SCREEN_TILES_H*8

#define TILE_SIZE	8
#define TILE_BIT	3

#define FP_BITS		4
#define FP_MASK		15

#define TILE_EMPTY	0
#define TILE_WALL	1
#define TILE_LADDER	2

#define TILE_NUM_PHONE		0x01
#define TILE_NUM_PLAYER		0x10
#define TILE_NUM_CHIEF		0x11
#define TILE_NUM_BOUNCER	0x12
#define TILE_NUM_CHATTER	0x13
#define TILE_NUM_GEEK		0x14
#define TILE_NUM_MANBOX		0x15
#define TILE_NUM_DIBROV		0x16
#define TILE_NUM_GHOST		0x17
#define TILE_NUM_TABLE		0x0a
#define TILE_NUM_TOPIC		0x76
#define TILE_NUM_TOPIC1		0x66
#define TILE_NUM_TOPIC2		0x68
#define TILE_NUM_TOPIC3		0x55
#define TILE_NUM_TOPIC4		0x57
#define TILE_NUM_COFFEE		0x87

#define TILE_NUM_START		0x10//first number tile
#define TILE_ALPHA_START	0x1A//first alpha tile



const uint8_t tileAttr[] PROGMEM = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,//TILE_EMPTY,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
0,0,0,0,0,0,0,0,0,0,0,TILE_LADDER,TILE_LADDER,0,0,0,//TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_EMPTY,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,
TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,

TILE_EMPTY,	TILE_WALL,	TILE_WALL,	TILE_WALL,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,
TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,
TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,
TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,
TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_WALL,	TILE_EMPTY,	TILE_EMPTY,
TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,
TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,	TILE_EMPTY,
};


#define DIR_NONE	0
#define DIR_UP		1//4
#define DIR_DOWN	2//8
#define DIR_LEFT	4//1
#define DIR_RIGHT	8//2

static int16_t		player_x;
static uint16_t		player_y;
static uint8_t		player_px;
static uint8_t		player_py;
const uint8_t	*player_spr;
const uint8_t	*player_spr_prev;
static uint8_t		player_ladder;
static uint8_t		player_dir;
static uint8_t		player_dir_prev;
static uint16_t		player_dir_cnt;
static uint8_t		player_floor;
static uint8_t		player_topic;
static uint16_t		player_speed;
static uint16_t		player_speed_to;
static uint8_t		player_coffee;
static uint16_t		player_step_cnt;
static uint8_t		player_step_type;
static uint8_t		player_slowdown;
static uint8_t		player_step_anim;
static uint8_t		player_knocked;
static uint8_t		player_knocked_anim;
static uint16_t		player_wisdom;
static uint8_t		player_catch;
static uint8_t		player_answer;

static uint8_t		topics_active;


#define SFX_RINGTONE		0
#define SFX_STEP1		1
#define SFX_STEP2		2
#define SFX_STEP3		3
#define SFX_STEP4		4
#define SFX_FALL		5
#define SFX_DROP		6
#define SFX_TOPIC		7
#define SFX_ANSWER		8
#define SFX_BLA1		9
#define SFX_MISS		10
#define SFX_COFFEE		11
#define SFX_START		12
#define SFX_PAUSE		13
#define SFX_LOSE		14
#define SFX_BLA2		15
#define SFX_COFFEE_READY	16
#define SFX_KNOCK		17
#define SFX_WISDOM		18
#define SFX_EXPLODE		19
#define SFX_TELEPORT		20

#define SFX_ALL			21

#define MUS_LEVEL		0
#define MUS_CLEAR		1
#define MUS_GAMEOVER		2
#define MUS_GAME		3
#define MUS_WELLDONE		4
#define MUS_DREAM		5
#define MUS_NOBONUS		6

#define MUS_ALL			7

const uint8_t* const musicData[MUS_ALL] PROGMEM = {
(uint8_t *)introSong,//mus_level,
mus_clear,
mus_gameover,
mus_game,

mus_welldone,
mus_dream,
mus_nobonus
};


#define NPC_NONE		0
#define NPC_CHIEF		1//TILE_NUM_CHIEF
#define NPC_BOUNCER		2//TILE_NUM_BOUNCER
#define NPC_CHATTER		3//TILE_NUM_CHATTER
#define NPC_GEEK		4//TILE_NUM_GEEK
#define NPC_MANBOX		5//TILE_NUM_MANBOX
#define NPC_DIBROV		6//TILE_NUM_DIBROV
#define NPC_GHOST		7//TILE_NUM_GHOST


static uint8_t	npc_all;
static uint8_t	npc_type;
static int16_t	npc_x;
static int16_t	npc_y;
static uint8_t	npc_dir;
static uint16_t	npc_cnt;
const uint8_t 	*npc_spr;
static uint8_t	npc_tx;
static uint8_t	npc_ty;
static uint8_t	npc_wait;
static uint8_t	npc_speed;
static int16_t	npc_dx ;
static int16_t	npc_dy;

#define PHONE_MAX 4

static uint8_t	phone_all;
static uint8_t	phone_x[PHONE_MAX];
static uint8_t	phone_y[PHONE_MAX];
static uint16_t	phone_cnt[PHONE_MAX];
static uint8_t	phone_level[PHONE_MAX];
static uint8_t	phone_topic[PHONE_MAX];

static uint8_t	phone_runaway;
static uint8_t	phone_runaway_max;

#define TABLE_MAX 6

static uint8_t	table_all;
static uint8_t	table_x[TABLE_MAX];
static uint8_t	table_y[TABLE_MAX];
static uint8_t	table_cur;

#define TOPIC_MAX 4

static uint8_t	topic_all;
static uint8_t	topic_x[TOPIC_MAX];
static uint8_t	topic_y[TOPIC_MAX];
static uint8_t	topic_id[TOPIC_MAX];

static uint8_t	topic_flash_x;
static uint8_t	topic_flash_y;
static uint8_t	topic_flash_type;
static uint8_t	topic_flash_cnt;

static uint8_t	topic_msg[TOPIC_MAX];

#define HEARTS_MAX	8

static uint8_t	heart_ptr;

static uint8_t	heart_x[HEARTS_MAX];
static uint8_t	heart_y[HEARTS_MAX];
static uint8_t	heart_cnt[HEARTS_MAX];

//nametable update list for stats and items

#define UPDATE_LIST_MAX		32
#define UPDL_MESSAGE		0*3
#define UPDL_COFFEE		1*3
#define UPDL_TOPIC		2*3
#define UPDL_STATS		4*3
#define UPDL_PHONES		8*3

static uint8_t	update_list_len;
//static uint8_t	update_list[UPDATE_LIST_MAX*3];

const uint8_t updateListData[] PROGMEM = {
0x20,0x00,0x00,//UPDL_MESSAGE
0x20,0x00,0x00,//UPDL_COFFEE
0x20,0x42,0x00,//UPDL_TOPIC
0x20,0x43,0x00,
0x20,0x51,0x00,//calls count tens
0x20,0x52,0x00,//calls count
0x20,0x5c,0x00//miss
};


static uint8_t i,j;
static uint8_t px,py;

static uint8_t	spr,spr1;
static uint16_t	i16,j16;
static uint16_t	padState,oldPadState;

static uint8_t	frame_cnt;

static uint8_t	calls_count;
static uint8_t	calls_missed;
static uint8_t	calls_missed_max;
static uint8_t	calls_missed_level;
static uint8_t	calls_level;
static uint16_t	call_delay;

static uint8_t	coffee_x;
static uint8_t	coffee_y;
static uint16_t	coffee_wait;

static uint8_t	level;
static uint8_t	pause;
static uint8_t	bonus;

static uint16_t	ring_cnt;

static uint8_t	msg_cnt;
const char	*msg_ptr;
static uint8_t	msg_wait;
static uint8_t	msg_off;

static uint8_t	test_mode;

/*const uint8_t topicList[4*2] PROGMEM = {
0x66,0x67,
0x68,0x69,
0x55,0x56,
0x57,0x58
};*/

const uint8_t* const topicMessages[4] PROGMEM = {
earthMessages,
historyMessages,
booksMessages,
spaceMessages
};

const uint8_t topicMessagesCount[4] PROGMEM = {
MESSAGES_EARTH,
MESSAGES_HISTORY,
MESSAGES_BOOKS,
MESSAGES_SPACE
};


const uint8_t CoffeeMap[] PROGMEM = {
3,4,
76,77,78,
79,80,81,
82,87,84,
85,86,78,
};

const uint8_t CabinetMap[] PROGMEM = {
2,3,
2,3,
4,5,
6,7,
};

const uint8_t ClockMap[] PROGMEM = {
2,2,
12,13,
14,15,
};

const uint8_t DoorMap[] PROGMEM = {
0,0,
};

const uint8_t DeskMap[] PROGMEM = {
3,2,
0,64,0,
29,30,31,
};

const uint8_t gamePalette[] PROGMEM = {
//12/13:cabinet colors, 14,15:floor colors
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0b00111001,0b00101000,0b10011110,0b11001001,//level 1
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0b01010101,0b00010100,0b10100100,0b11001001,//level 2
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0b10011111,0b00010111,0b01100111,0b01001111,//level 3
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0x3e,0x6d,0x9e,0x9d,//level 4
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0x3e,0x6d,0x9e,0x9d,//level bonus
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0x3e,0x6d,0x9e,0x9d,//level 5
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0x3e,0x6d,0x9e,0x9d,//level 6
0x0,0xff,0xd9,0x67,0x34,0xa4,0x15,0xb7,0x53,0xa1,0xe,0x3e,0x6d,0x9e,0x9d,//level 7

};

const uint16_t testCode[] PROGMEM = { BTN_B,BTN_A,BTN_B,BTN_A,BTN_LEFT,BTN_UP,BTN_B,BTN_A,0 };

#define FLOORS_MAX	4

static uint8_t floor_left_cnt;
static uint8_t floor_right_cnt;
static uint8_t floor_left [FLOORS_MAX];
static uint8_t floor_right[FLOORS_MAX];

#define LEVELS_ALL	8
#define LEVEL_BONUS	4


/*
const uint16_t levelSettings[LEVELS_ALL*3] PROGMEM = {//calls, delay between calls, number of topics
10,250,2,
15,250,2,
20,250,3,
25,250,3,
1,0,0,//bonus
30,275,4,
35,400,4,
40,450,4
};
*/



void ZSTriggerFx(uint8_t sfx, uint8_t chan){
	TriggerFx(sfx,255,1);

}

void SetSprite(uint8_t x, uint8_t y, uint8_t t, uint8_t f){
	sprites[spr].x = x;
	sprites[spr].y = y;
	sprites[spr].tileIndex = t;
	sprites[spr++].flags = f;
}



void DrawMetaSprite(uint8_t sx, uint8_t sy, const uint8_t *frame){
	uint8_t t,f,dx,dy;

	while(1){//very strange issue going on if I inline the pgm_read_byte() into SetSprite()?!?!?!
		t = pgm_read_byte(frame++);
		if(t == 128)
			return;
		dx = sx+t;
		dy = sy+pgm_read_byte(frame++);
		t = pgm_read_byte(frame++);
		f = pgm_read_byte(frame++);		
		SetSprite(dx,dy,t,f);
	}
}



void ZSPrint(uint8_t x, uint8_t y, const char* str){
Print(x,y,str);
}

void ResetSprites(){
	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;
}


void PostVsync(){
	oldPadState = padState;
	padState = ReadJoypad(0);
}

uint16_t abs(int num){
	if(num<0)
		return 0-num;
	return num;
}



uint8_t CheckMap(uint8_t x,uint8_t y){
	i16=x>>TILE_BIT;//divide by 8
	j16=y>>TILE_BIT;

//	j16-=6;
//	if(j16>128)
//		j16=0;

	return vram[(j16*VRAM_TILES_H)+i16]-RAM_TILES_COUNT;
}



void PlayerCoordWrap(){
	if(player_x < 0)
		player_x = ((SCREEN_WIDTH-16)<<FP_BITS);//player_x = ((256-16)<<FP_BITS);
	if(player_x > ((SCREEN_WIDTH-16)<<FP_BITS))//if(player_x > ((256-16)<<FP_BITS))
		player_x = 0;
}



void PlayerAlignToLadder(){
	while(!(CheckMap((player_x>>FP_BITS)+3 ,py)&TILE_LADDER))
		player_x += (1<<FP_BITS);
	while(!(CheckMap((player_x>>FP_BITS)+12,py)&TILE_LADDER))
		player_x -= (1<<FP_BITS);

	player_ladder = 1;
	player_dir_cnt = 16<<FP_BITS;
}



void PhoneReset(uint8_t id,uint16_t delay,uint8_t answer){
	if(answer){
		phone_level[id] = 255;
		phone_cnt  [id] = 25;
	}else{//trigger this one again later
		phone_level[id] = 0;
		phone_cnt  [id] = call_delay+delay+(uint16_t)(GetPrngNumber(0)&63);
	}

	phone_topic[id] = GetPrngNumber(0)%topics_active;
		
	/////////id = id*12+2+UPDL_PHONES;

	SetTile(phone_x[id],phone_y[id],pgm_read_byte(&animPhone[0]));///update_list[id]  = pgm_read_byte(&animPhone[0]);
	SetTile(phone_x[i]-1,phone_y[i]-2,0);//erase topic
	SetTile(phone_x[i]-0,phone_y[i]-2,0);
	SetTile(phone_x[i]-1,phone_y[i],0);//erase ring waves
	SetTile(phone_x[i]+1,phone_y[i],0);
	///update_list[id+3] = 0;
	///update_list[id+6] = 0;
	///update_list[id+9] = 0;

	if(answer)
		return;

	if(call_delay>125)
		call_delay -= 10;
}



void SoundSteps(uint8_t dir){
	player_step_cnt += player_speed;

	if(player_step_cnt >= (8<<FP_BITS)){
		ZSTriggerFx(SFX_STEP1+player_step_type+(dir<<1),0);
		player_step_cnt -= (8<<FP_BITS);
		player_step_type ^= 1;
	}
}



void UpdateStats(){
	if(calls_count != calls_level||(frame_cnt&16)){
		SetTile(15,0,TILE_NUM_START+(calls_count/10));
		SetTile(16,0,TILE_NUM_START+(calls_count%10));
	}else{
		SetTile(15,0,0);
		SetTile(16,0,0);
	}

	if(calls_missed != calls_missed_level || (frame_cnt&16))
		SetTile(26,0,TILE_NUM_START+calls_missed);
	else
		SetTile(26,0,0);
}



void HeartsAdd(uint8_t x,uint8_t y){
	for(j = 0;j < HEARTS_MAX;j++){
		heart_ptr++;

		if(heart_ptr >= HEARTS_MAX)
			heart_ptr = 0;

		if(heart_y[heart_ptr] >= 240){
			heart_x[heart_ptr] = x-4+(GetPrngNumber(0)&7);
			heart_y[heart_ptr] = y;
			heart_cnt[heart_ptr] = 24+(GetPrngNumber(0)&7);
			break;
		}
	}
}



void SetMessage(const char *msg){
	msg_ptr = msg;
	msg_cnt = 30;
	msg_wait = 2*50;
	msg_off = 0;
}



void ShowMessage(){
return;
	if(msg_ptr){
		if(msg_cnt){
		SetFont(msg_off,2,pgm_read_byte(msg_ptr+msg_off));//	update_list[UPDL_MESSAGE+1]=0x81+msg_off;
		//	update_list[UPDL_MESSAGE+2]=msg_ptr[msg_off]-0x20;
			msg_off++;
			msg_cnt--;
		}else{
			if(msg_wait){
				msg_wait--;
			}else{
				msg_ptr = NULL;
				msg_off = 0;
			}
		}
	}
	else{
		SetTile(msg_off,2,0);
//		update_list[UPDL_MESSAGE+1]=0x81+msg_off;
//		update_list[UPDL_MESSAGE+2]=0x00;
		msg_off++;
		if(msg_off>=30)
			msg_off=0;
	}
}



void ChangeScreen(){
//return;
	StopSong();
	FadeOut(5,true);
	WaitVsync(10);
	ClearVram();
	ResetSprites();
	FadeIn(3,false);
}



void MovePhone(uint8_t table){
	table_cur = table;

/*
	i16 = table_off[table];
	px = (i16&31)<<3;
	py = ((i16>>5)&31)<<3;
*/
	phone_x[0] = table_x[table];/////phone_x[0] = px;
	phone_y[0] = table_y[table];/////phone_y[0] = py;

	//////////memcpy(&update_list[UPDL_PHONES+9],&update_list[UPDL_PHONES],9);
////	for(uint8_t i=0;i<9;i++)
	////	update_list[UPDL_PHONES+9+i] = update_list[UPDL_PHONES+i];




/*
	update_list[UPDL_PHONES+9+2] = 0;
	update_list[UPDL_PHONES+9+5] = 0;
	update_list[UPDL_PHONES+9+8] = 0;

	update_list[UPDL_PHONES+0] = i16>>8;
	update_list[UPDL_PHONES+1] = i16&255;
	update_list[UPDL_PHONES+2] = animPhone[0];
	--i16;
	update_list[UPDL_PHONES+3] = i16>>8;
	update_list[UPDL_PHONES+4] = i16&255;
	update_list[UPDL_PHONES+5] = 0;
	i16+=2;
	update_list[UPDL_PHONES+6] = i16>>8;
	update_list[UPDL_PHONES+7] = i16&255;
	update_list[UPDL_PHONES+8] = 0;
*/
}



void NPCadd(uint16_t type){
	npc_type = type;
	npc_x = px;
	npc_y = py-16;
	npc_dir = (GetPrngNumber(0)&0xFF)<128?DIR_LEFT:DIR_RIGHT;
	npc_cnt = 0;

	switch(type){
		case TILE_NUM_CHIEF:
			npc_dir = DIR_NONE;
			npc_cnt = 20;
			npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkLeft[0]);
			npc_ty = 240;
			break;

		case TILE_NUM_BOUNCER:
			npc_spr = (uint8_t *)pgm_read_word(&animBouncerWalkLeft[0]);
			break;

		case TILE_NUM_CHATTER:
			npc_dir =DIR_NONE;
			npc_cnt =20;
			npc_wait=0;
			npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkLeft[0]);
			break;

		case TILE_NUM_GEEK:
			npc_spr = (uint8_t *)pgm_read_word(&animGeekWalkLeft[0]);
			break;

		case TILE_NUM_MANBOX:
			npc_spr = (uint8_t *)pgm_read_word(&animManBoxWalkLeft[0]);
			break;

		case TILE_NUM_DIBROV:
			npc_spr  = (uint8_t *)pgm_read_word(&animDibrovWalkLeft[0]);
			npc_speed=1;
			break;

		default://case TILE_NUM_GHOST:
			npc_x = 16<<FP_BITS;
			npc_y = 240<<FP_BITS;
			npc_dx = 0;
			npc_dy = 0;
			npc_spr = sprChiefGhostR;
			npc_cnt = 50;
			npc_wait= 150;
			break;
	}

	npc_all++;
}



void NPCdisplay(){
//return;
	if(!npc_all)
		return;

	switch(npc_type){
		case NPC_GHOST:
			DrawMetaSprite(npc_x>>FP_BITS,(npc_y>>FP_BITS)-1,npc_spr);			
			break;

		case NPC_CHIEF:
			SetSprite(npc_tx,npc_ty,0xFC,1);//bla
			SetSprite(npc_tx+8,npc_ty,0xFC,1);
		default:
			DrawMetaSprite(npc_x,npc_y-1,npc_spr);//NPC

	}
}



void NPCcheckCollision(){

	if(!player_ladder){
		if(player_py == py){
			if(player_px+8 < px+8){
				if(player_px+16 >= px+8){
					player_x = (px+8-16)<<FP_BITS;
					PlayerCoordWrap();
				}
			}else{
				if(player_px < px+8){
					player_x = (px+8)<<FP_BITS;
					PlayerCoordWrap();
				}
			}
		}
	}
}



void NPCchiefSetDelay(){
	npc_cnt = 64+((GetPrngNumber(0)&0xFF)&31);
}



void NPCchief(){
	if(!(py >= (player_py+24-8) || (py+24)<(player_py+8))){
		if(!((px-32) >= (player_px+16) || (px+16+32) < player_px)){
			player_slowdown = 1;
			npc_dir = DIR_NONE;

			if(!npc_cnt || npc_cnt > 25){
				npc_cnt = 25;
				npc_tx = npc_x-8+((GetPrngNumber(0)&0xFF)&15);
				npc_ty = npc_y-12+((GetPrngNumber(0)&0xFF)&3);
				ZSTriggerFx(SFX_BLA1,3);
			}else{
				npc_cnt--;
				if(npc_cnt < 10)
					npc_ty = 240;
			}

			j = (frame_cnt>>3)&3;

			if(player_px < px)
				npc_spr = (uint8_t *)pgm_read_word(&animChiefTalkLeft[j]);
			else
				npc_spr = (uint8_t *)pgm_read_word(&animChiefTalkRight[j]);
		////////	npc_spr = (player_px<px)?animChiefTalkLeft[j]:npc_spr = animChiefTalkRight[j];

			return;
		}
	}

	npc_ty = 240;

	if(npc_cnt){
		j = (npc_x>>2)&3;

		switch(npc_dir){
			case DIR_LEFT:
				npc_x--;

				if(npc_x < 9){//||!CheckMap(npc_x-1,npc_y+24))//no platform edges
					npc_dir = DIR_RIGHT;
					NPCchiefSetDelay();
				}

				npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkLeft[j]);
			
				break;

		case DIR_RIGHT:
			
				npc_x++;

				if(npc_x > (256-16-9)){//||!CheckMap(npc_x+16,npc_y+24))//no platform edges
					npc_dir = DIR_LEFT;
					NPCchiefSetDelay();
				}

				npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkRight[j]);
			
			break;

		default:
			if(!(npc_cnt&15)){
				if((GetPrngNumber(0)&0xFF)&128)
					npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkLeft[j]);
				else
					npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkRight[j]);
				//npc_spr = ((GetPrngNumber(0)&0xFF)&128)?animChiefWalkLeft[j]:npc_spr = animChiefWalkRight[j];
			}		
	}

		npc_cnt--;
	}else{
		if(npc_dir)
			npc_dir = DIR_NONE;
		else{
			if(npc_x < 64)
				npc_dir = DIR_RIGHT;

			if(npc_x > SCREEN_WIDTH-64)//if(npc_x > 256-64)
				npc_dir = DIR_LEFT;

			if(!npc_dir)
				npc_dir = 1+((GetPrngNumber(0)&0xFF)&1);
		}

		NPCchiefSetDelay();
	}
}



void NPCbouncer(){
	NPCcheckCollision();

	if(frame_cnt&3)
		return;

	j=(npc_x>>2)&3;

	switch(npc_dir){
		case DIR_LEFT:
			npc_x--;

			if(npc_x < 9)//||!CheckMap(npc_x+4-1,npc_y+24))//no platform edges
				npc_dir = DIR_RIGHT;

			npc_spr = (uint8_t *)pgm_read_word(&animBouncerWalkLeft[j]);
			break;

		case DIR_RIGHT:
			npc_x++;

			if(npc_x > (SCREEN_WIDTH-16-9))//if(npc_x>(256-16-9))//||!CheckMap(npc_x+16-4,npc_y+24))//no platform edges
				npc_dir = DIR_LEFT;

			npc_spr = (uint8_t *)pgm_read_word(&animBouncerWalkRight[j]);
			break;
	}
}



void NPCchatterSetDelay(){
	npc_cnt = 25+((GetPrngNumber(0)&0xFF)&31);
}



void NPCchatter(){
	if(npc_wait){
		if(npc_cnt){
			npc_cnt--;

			j = (frame_cnt>>3)&3;

			if(player_px < px)
				npc_spr = (uint8_t *)pgm_read_word(&animChatterTalkLeft[j]);
			else
				npc_spr = (uint8_t *)pgm_read_word(animChatterTalkRight[j]);
			//npc_spr = (player_px<px)?animChatterTalkLeft[j]:npc_spr = animChatterTalkRight[j];

			return;
		}

		npc_wait--;

		j = (frame_cnt>>3)%5;

		if(player_px < px)
			npc_spr = (uint8_t *)pgm_read_word(&animChatterGlassesLeft[j]);
		else
			npc_spr = (uint8_t *)pgm_read_word(&animChatterGlassesRight[j]);
		//npc_spr = (player_px<px)?animChatterGlassesLeft[j]:npc_spr = animChatterGlassesRight[j];

		return;
	}

	if(player_py == py){
		if(!((px-16) >= (player_px+16) || (px+16+16) < player_px)){
			if(!npc_wait){
				player_topic = (GetPrngNumber(0)&0xFF)%topics_active;
				npc_dir = DIR_NONE;
				npc_wait = 100;
				npc_cnt = 50;
				ZSTriggerFx(SFX_BLA2,3);
			}

			return;
		}

		if(player_px < px)
			npc_dir = DIR_LEFT;
		else
			npc_dir = DIR_RIGHT;
	}else{
		if(frame_cnt&1)
			return;
	}

	if(npc_cnt){
		j = (npc_x>>2)&3;

		switch(npc_dir){
			case DIR_LEFT:
				npc_x--;

				if(npc_x < 9){//||!CheckMap(npc_x-1,npc_y+24))//no platform edges

					npc_dir = DIR_RIGHT;
					NPCchatterSetDelay();
				}

				npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkLeft[j]);
				break;

		case DIR_RIGHT:
			
				npc_x++;

				if(npc_x > (256-16-9)){//||!CheckMap(npc_x+16,npc_y+24))//no platform edges
				
					npc_dir = DIR_LEFT;
					NPCchatterSetDelay();
				}

				/////////////////npc_spr = animChatterWalkRight[j];
			
			break;

		default:
			if(!(npc_cnt&31)){
				if((GetPrngNumber(0)&0xFF)&128)
					npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkLeft[j]);
				else
					npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkRight[j]);
				//////////////////npc_spr = ((GetPrngNumber(0)&0xFF)&128)?animChatterWalkLeft[j]:npc_spr = animChatterWalkRight[j];
			}
		}

		npc_cnt--;
	}else{
		if(npc_dir){
			npc_dir = DIR_NONE;
		}else{
			if(npc_x < 64)
				npc_dir = DIR_RIGHT;

			if(npc_x > 256-64)
				npc_dir = DIR_LEFT;

			if(!npc_dir)
				npc_dir = 1+((GetPrngNumber(0)&0xFF)&1);
		}

		NPCchatterSetDelay();
	}
}



void NPCgeek(){
	if(!player_ladder){
		if(player_py==py && abs(((int)player_px)-((int)px)) < 64){
			j = (frame_cnt>>3)&1;

			if(player_px+8 < px+8 && npc_dir == DIR_LEFT){
				if(player_px+16 >= px+8 && player_dir == DIR_RIGHT){
					player_x = (px+8-16)<<FP_BITS;
					PlayerCoordWrap();

					if(frame_cnt&1){
						if(npc_x < (256-16))
							npc_x++;
					}
				}

				npc_spr = (uint8_t *)pgm_read_word(&animGeekStandLeft[j]);

				if(!(frame_cnt&31))
					HeartsAdd(px+4,py);

				return;
			}

			if(player_px+8 > px+8 && npc_dir == DIR_RIGHT){
				if(player_px < px+8 && player_dir == DIR_LEFT){
					player_x = (px+8)<<FP_BITS;
					PlayerCoordWrap();

					if(frame_cnt&1){
						if(npc_x>0)
							npc_x--;
					}
				}

				npc_spr = (uint8_t *)pgm_read_word(&animGeekStandRight[j]);

				if(!(frame_cnt&31))
					HeartsAdd(px+4,py);

				return;
			}
		}
	}

	if(frame_cnt&1)
		return;

	j = (npc_x>>2)&3;

	switch(npc_dir){
		case DIR_LEFT:
			npc_x--;

			if(npc_x < 9)//||!CheckMap(npc_x+4-1,npc_y+24))//no platform edges
				npc_dir = DIR_RIGHT;

			npc_spr = (uint8_t *)pgm_read_word(&animGeekWalkLeft[j]);
			break;

		case DIR_RIGHT:
			npc_x++;

			if(npc_x > (256-16-9))//||!CheckMap(npc_x+16-4,npc_y+24))//no platform edges
				npc_dir = DIR_LEFT;

			npc_spr = (uint8_t *)pgm_read_word(&animGeekWalkRight[j]);
			break;
	}
}



void NPCchangeFloorLeft(){
	npc_dir = DIR_RIGHT;
	npc_x = -8;
	npc_y = floor_left[(GetPrngNumber(0)&0xFF)%floor_left_cnt];
}



void NPCchangeFloorRight(){
	npc_dir = DIR_LEFT;
	npc_x = SCREEN_WIDTH-8;//npc_x = 248;
	npc_y = floor_right[(GetPrngNumber(0)&0xFF)%floor_right_cnt];
}



void NPCboxMan(){
	if(npc_cnt){
		npc_cnt--;
		return;
	}

	if(player_py == py){
		if(!((player_px+16-4) < px || player_px >= (px+16-4))){
			if(!player_knocked){
				player_knocked=48;
				player_knocked_anim=8;
				npc_dir = (npc_dir == DIR_LEFT)?DIR_RIGHT:DIR_LEFT;
				ZSTriggerFx(SFX_KNOCK,1);
			}else
				player_knocked++;
		}
	}

	if(frame_cnt&1)
		return;

	j = (npc_x>>2)&3;

	switch(npc_dir){
		case DIR_LEFT:
			npc_x--;

			if(npc_x < -7){
				NPCchangeFloorLeft();
				npc_cnt = 50;
			}else if(!CheckMap(npc_x+4-1,npc_y+24))
				npc_dir = DIR_RIGHT;

			npc_spr = (uint8_t *)pgm_read_word(&animManBoxWalkLeft[j]);
			break;

		case DIR_RIGHT:
			npc_x++;

			if(npc_x >= SCREEN_WIDTH-8){//if(npc_x >= 248){
				NPCchangeFloorRight();
				npc_cnt = 50;
			}else if(!CheckMap(npc_x+16-4,npc_y+24))
				npc_dir = DIR_LEFT;

			npc_spr = (uint8_t *)pgm_read_word(&animManBoxWalkRight[j]);
		break;
	}
}



void NPCdibrov(){
	if(npc_cnt){
		npc_cnt--;
		return;
	}

	if(player_py == py){
		if(!((player_px+16-4) < px || player_px >= (px+16-4))){
			if(!player_wisdom){
				player_wisdom = 10*50;
				ZSTriggerFx(SFX_WISDOM,2);

				for(spr=0;spr<2;++spr)
					HeartsAdd(player_px+4,player_py+8-(spr<<2));
			}

		}

		npc_speed = 2;

		npc_dir = ((int)player_px)<npc_x?DIR_RIGHT:DIR_LEFT;
	}else{
		npc_speed = 1;

		if(abs(((int)player_px)-npc_x) < 64)
			npc_dir = ((int)player_px) < npc_x?DIR_RIGHT:DIR_LEFT;
	}

	if(npc_speed < 2 && frame_cnt&1)
		return;

	j = npc_speed;

	if(j == 2 && frame_cnt&1)
		j = 1;

	switch(npc_dir){//no platform edges on the level

		case DIR_LEFT:
			npc_x -= j;
			break;

		case DIR_RIGHT:
			npc_x += j;
			break;
	}

	if(npc_x >= SCREEN_WIDTH-8 || npc_x < -7){////////if(npc_x>=248||npc_x<-7){
		npc_cnt = player_wisdom?player_wisdom+25:25;

		if(player_px < 128)
			NPCchangeFloorRight();
		else
			NPCchangeFloorLeft();
	}

	j = (npc_x>>2)&3;

	if(npc_dir == DIR_LEFT)
		npc_spr = (uint8_t *)pgm_read_word(&animDibrovWalkLeft[j]);
	else
		npc_spr = (uint8_t *)pgm_read_word(&animDibrovWalkRight[j]);
	//npc_spr = (npc_dir == DIR_LEFT)?animDibrovWalkLeft[j]:npc_spr = animDibrovWalkRight[j];
}



void NPCghost(){
	if(npc_wait){
		npc_wait--;
		return;
	}

	npc_cnt++;

	if(npc_cnt >= 100){
		ZSTriggerFx(SFX_BLA1,3);
		npc_cnt = 0;
	}

	if(!(npc_x+(16<<FP_BITS) < player_x ||
	     npc_x+ (8<<FP_BITS) >= player_x+(16<<FP_BITS) ||
		 npc_y+(16<<FP_BITS) < player_y ||
		 npc_y+(8<<FP_BITS) >= player_y+(24<<FP_BITS))){
		player_catch = 1;
		npc_wait = 255;
		return;
	}

	if(player_x<npc_x){
		if(npc_y < (240<<FP_BITS) && npc_dx>-16)
			npc_dx--;

		npc_spr = npc_cnt<20?sprChiefGhostBlaL:sprChiefGhostL;
	}else{
		if(npc_y<(240<<FP_BITS)&&npc_dx< 16)
			npc_dx++;

		npc_spr = (npc_cnt < 20)?sprChiefGhostBlaR:sprChiefGhostR;
	}

	if(player_y < npc_y){
		if(npc_dy > -12)
			npc_dy--;
	}else{
		if(npc_dy < 12)
			npc_dy++;
	}

	npc_x += npc_dx;
	npc_y += npc_dy;
}


void LoadLevel(){
level = 1;
	uint16_t loff = 0;
	uint8_t x,y,l,t,did_left;
ClearVram();

while(1){
	loff = 0;
	WaitVsync(1);

	if(padState & BTN_UP && !(oldPadState & BTN_UP)){
		level++;
		if(level > 7)
			level = 0;

ClearVram();
	}


		for(i=0;i<level;i++){
			loff += 3;//eat calls, delay between calls, number of topics
			loff += 2;//eat player start
			while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat NPC
				loff += 2;	
			while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat platforms
				loff += 2;
			while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat ladders
				loff += 2;
			while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat cabinets
				loff += 2;
			while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat desks
				loff += 1;
			if(pgm_read_byte(&lvl_data[loff++]) != 255)//eat coffee machine
				loff += 1;
			while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat clock
				loff++;
			while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat door
				loff++;
		}

		calls_level = pgm_read_byte(&lvl_data[loff++]);
		call_delay = pgm_read_byte(&lvl_data[loff++])*5;
		topics_active = pgm_read_byte(&lvl_data[loff++]);

		player_x = (pgm_read_byte(&lvl_data[loff++])*8)<<FP_BITS;
		player_y = ((pgm_read_byte(&lvl_data[loff++])*8)-16)<<FP_BITS;


		while(1){//NPCs

			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			x *= 8;
			y = pgm_read_byte(&lvl_data[loff++])*8;
			t = pgm_read_byte(&lvl_data[loff++]);
			NPCadd(t);
			///////dbf();
			break;

/*
				case TILE_NUM_TOPIC1:
				case TILE_NUM_TOPIC2:
				case TILE_NUM_TOPIC3:
				case TILE_NUM_TOPIC4:
					topic_x[topic_all] = px-2;
					topic_y[topic_all] = py+24;

					switch(spr){
						case TILE_NUM_TOPIC1: spr = 0; break;
						case TILE_NUM_TOPIC2: spr = 1; break;
						case TILE_NUM_TOPIC3: spr = 2; break;
						case TILE_NUM_TOPIC4: spr = 3; break;
						default:              spr = 255;
					}

					topic_id[topic_all] = spr;
					topic_msg[spr] = GetPrngNumber(0)%topicMessagesCount[spr];

					topic_all++;
*/
		}

		while(1){//platforms
			did_left = 0;
			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			y = pgm_read_byte(&lvl_data[loff++]);
			l = pgm_read_byte(&lvl_data[loff++]);

			while(l--){
				if(x == 0)
					did_left = 1;
				if(!did_left){
					t = 59;
					did_left = 1;				
				}else if(l || x == SCREEN_TILES_H-1)
					t = 60;
				else
					t = 61;

				SetTile(x++,y,t);
			}
		}

		while(1){//ladders
			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			y = pgm_read_byte(&lvl_data[loff++]);
			l = pgm_read_byte(&lvl_data[loff++]);
			while(l--){
				SetTile(x+0,y,27);
				SetTile(x+1,y++,28);
			}
		}

		while(1){//cabinets
			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			y = pgm_read_byte(&lvl_data[loff++]);
			t = pgm_read_byte(&lvl_data[loff++]);
			DrawMap(x,y,(const char *)CabinetMap);
			t = 88+(t*4);
			SetTile(x+0,y-2,t+0);
			SetTile(x+1,y-2,t+1);
			topic_x[topic_all] = x;
			topic_y[topic_all] = y+2;
			topic_id[topic_all++] = (t-88)/4;
			//topics_active++;
			
		
		}

		while(1){//desks
			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			y = pgm_read_byte(&lvl_data[loff++]);
			DrawMap(x,y,(const char *)DeskMap);
			phone_x[phone_all] = x+1;
			phone_y[phone_all] = y;
			//PhoneReset(....)
			phone_all++;

			table_x[table_all] = x;
			table_y[table_all] = y;
			table_all++;
/*
				phone_x[phone_all] = j*8;//px;
				phone_y[phone_all] = i*8;//py;

				PhoneReset(phone_all,((uint16_t)phone_all)<<8,0);
				phone_all++;

				j16 = 0x20c0+i16;
				update_list_add(animPhone[0]);//phone
				j16--;
				update_list_add(0);//ring left
				j16 += 2;
				update_list_add(0);//ring right
				j16=0x20c0+i16-64;
				update_list_add(0);//topic
*/



/////////
/*
					table_off[table_all++] = 0x20c0+i16;
*/
		}
		
		while(1){//coffee machine
			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			y = pgm_read_byte(&lvl_data[loff++]);
			DrawMap(x,y,(const char *)CoffeeMap);
			coffee_x = x+1;
			coffee_y = y+2;
		}

		while(1){//clock
			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			y = pgm_read_byte(&lvl_data[loff++]);
			DrawMap(x,y,(const char *)ClockMap);
		}

		while(1){//door
			x = pgm_read_byte(&lvl_data[loff++]);
			if(x == 255)
				break;			
			y = pgm_read_byte(&lvl_data[loff++]);
			DrawMap(x,y,(const char *)DoorMap);
		}
break;}
}

uint8_t GameLoop(){
	bonus = (level == LEVEL_BONUS)?1:0;

	////LoadLevel();
	calls_count = 0;
	calls_missed = 0;


	if(!bonus){
		i = level;
		if(i > LEVEL_BONUS)
			i--;
		//vram_adr(0x204a);
		//vram_put(i+17);
		SetTile(9,0,TILE_NUM_START+1+i);	
	}else
		ZSPrint(5,0,PSTR("BONUS"));

	SetTile(19,0,TILE_NUM_START+(calls_level/10));
	SetTile(20,0,TILE_NUM_START+(calls_level%10));
	SetTile(27,0,TILE_NUM_START+calls_missed_level);


	frame_cnt = 0;
	ring_cnt = 0;
	pause = 0;

	player_spr = (uint8_t *)pgm_read_word(&animWalkRight[0]);
	player_spr_prev = player_spr;
	player_ladder = 0;
	player_dir = DIR_NONE;
	player_dir_prev = DIR_RIGHT;
	player_dir_cnt = 0;
	player_floor = 1;
	player_topic = 255;
	player_speed = 1<<FP_BITS;
	player_speed_to = player_speed;
	player_step_cnt = 0;
	player_step_type = 0;
	player_slowdown = 0;
	player_coffee = 0;
	player_step_anim = 0;
	player_knocked = 0;
	player_knocked_anim = 0;
	player_wisdom = 0;
	player_catch = 0;
	player_answer = 0;

	coffee_y = 0;
	coffee_wait = 150;
	topic_flash_cnt = 0;

	npc_all = 0;
	phone_all = 0;
	topic_all = 0;
	table_all = 0;
	phone_runaway_max = 5;
	phone_runaway = phone_runaway_max;

	msg_ptr = NULL;
	msg_cnt = 0;
	msg_wait = 0;
	msg_off = 0;

////	for(i=0;i<sizeof(update_list);i++)
	////	update_list[i]=0;
	///////memcpy(update_list,updateListData,sizeof(updateListData));
////	for(i=0;i<sizeof(updateListData);i++)
	////	update_list[i] = pgm_read_byte(&updateListData[i]);


	LoadLevel();
	ZSPrint(0,0,PSTR("    DAY:0 CALL:00+00 MISS:0+0"));
	SetTile(18,0,TILE_NUM_START+(calls_level/10));
	SetTile(19,0,TILE_NUM_START+(calls_level%10));
	
	update_list_len = UPDL_PHONES;

	UpdateStats();

	i16 = 0;
	py = 6*8;


	floor_left_cnt = 0;
	floor_right_cnt = 0;
	i16 = 0;
	j16 = 0;
	py = 6*8-24;

	for(i=0;i<MAP_HGT;i++){
break;
	////	map[i16] = map[i16+1];
		/////map[i16+31] = map[i16+30];

		for(j=0;j<32;++j){
/////////////////TODO WE NEED TO MOVE THESE CHECKS INTO CHECK MAP TO ELIMINATE THE NEED FOR map[]////
		///////	map[i16] = pgm_read_byte(&tileAttr[map[i16]]);
			i16++;
		}

		if(i){
			if(0){///////////map[j16]&&!map[j16-32]){
				if(floor_left_cnt<FLOORS_MAX)
					floor_left[floor_left_cnt++]=py;
			}

			if(0){////////////////map[j16+31]&&!map[j16-1]){
				if(floor_right_cnt<FLOORS_MAX)
					floor_right[floor_right_cnt++]=py;
			}
		}

		j16 += 32;
		py += 8;
	}

	if(bonus){
		update_list_len += 6*3;
		phone_all = 1;
		MovePhone(5);
		PhoneReset(0,25,0);
	}

	heart_ptr = 0;

	for(i=0;i<HEARTS_MAX;i++)
		heart_y[i] = 240;/////////TODO HIDE THIS SPRITE RIGHT?????

	//////////////ppu_on_all();

	if(!bonus)
		StartSong((const char *)pgm_read_word(&musicData[MUS_GAME]));
	else
		StartSong((const char *)pgm_read_word(&musicData[MUS_DREAM]));


	WaitVsync(1);

	while(1){
		////////////WaitVsync(1);
		if(pause == 25){//we are paused and the retrigger delay is over

			if((padState & BTN_START) && !(oldPadState & BTN_START)){
				DDRC = 0b11111111;
				ZSTriggerFx(SFX_PAUSE,3);
				ResumeSong();/////////////music_pause(0);
				pause = 24;
				continue;
			}
		}else if(pause)//pause delay
			pause--;

		if(pause >= 25){
			WaitVsync(1);
			continue;
		}

		frame_cnt++;
		ring_cnt++;

		ResetSprites();		






		//display player
		px = player_x>>FP_BITS;
		py = (player_y>>FP_BITS)-1;

		i = 255;

		if(px<8){
			px += (256-16);
			i = 0;
		}else{
			if(px >= (256-16-8)){
				px += 16;
				i = 4;
			}
		}

		DrawMetaSprite(px,py,player_spr);



		if(i<255){
			for(;i<24;i+=8){
				//spr = oam_spr(px+player_spr[i+0],py+player_spr[i+1],player_spr[i+2],player_spr[i+3],spr);
		///////		SetSprite(px+player_spr[i+0],py+player_spr[i+1],player_spr[i+2],player_spr[i+3]);			
			}
		}

		//display npcs
		NPCdisplay();

		//display hearts
		for(i=0;i<HEARTS_MAX;i++){
			if(heart_cnt[i] >= 12)
				j = 0x38;
			else
				j = 0x3d-(heart_cnt[i]>>1);

			//spr = oam_spr(heart_x[i],heart_y[i],j,0,spr);
			SetSprite(heart_x[i],heart_y[i],j,0);
		}

		//display flashing topic icon when needed(player just touched a cabinet)
		if(topic_flash_cnt&2){
			SetTile(topic_flash_x+0,topic_flash_y,90+(topic_flash_type*4));
			SetTile(topic_flash_x+1,topic_flash_y,91+(topic_flash_type*4));
		}else{
			SetTile(topic_flash_x+0,topic_flash_y,88+(topic_flash_type*4));
			SetTile(topic_flash_x+1,topic_flash_y,89+(topic_flash_type*4));
		}


		WaitVsync(1);//This was original location??

		if(!pause && (padState&BTN_START) && !(oldPadState&BTN_START)){
			ZSTriggerFx(SFX_PAUSE,3);
			StopSong();
			DDRC = 0b10010010;
			pause = 50;
			continue;
		}


		//process player movements
		px = player_x>>FP_BITS;
		py = player_y>>FP_BITS;

		if(!player_knocked){
			if(!CheckMap(px+4,py+24)&&!CheckMap(px+12,py+24)){
				player_y += 4<<FP_BITS;
				player_ladder = 0;

				if(player_floor){
					player_floor = 0;
					ZSTriggerFx(SFX_FALL,1);
				}
			}else{
				if(!player_floor){
					player_floor = 1;
					ZSTriggerFx(SFX_DROP,1);
				}

				if(player_dir_cnt){
					switch(player_dir){
						case DIR_UP:
							player_y -= player_speed;

							if(player_y < (32<<FP_BITS))
								player_y += (240-48-32)<<FP_BITS;

							player_spr = (uint8_t *)pgm_read_word(&animWalkUp[(player_y>>FP_BITS>>2)&1]);
							py = player_y>>FP_BITS;

							if(!(CheckMap(px+8,py+23)&TILE_LADDER)){
								player_dir_cnt = 0;
								player_ladder = 0;
								player_y &= ~(7<<FP_BITS);

								if(!(CheckMap(px+8,py+24)&TILE_LADDER))
									player_y += (8<<FP_BITS);
							}
							break;

						case DIR_DOWN:
							player_y += player_speed;

							if(player_y > ((240-48)<<FP_BITS))
								player_y -= (240-48-32)<<FP_BITS;

							player_spr = (uint8_t *)pgm_read_word(&animWalkDown[(player_y>>FP_BITS>>2)&1]);
							py = player_y>>FP_BITS;

							if(!(CheckMap(px+8,py+24)&TILE_LADDER)){
								player_dir_cnt = 0;
								player_ladder = 0;
								player_y &= ~(7<<FP_BITS);
							}
							break;
					}

					if(player_dir_cnt){
						player_dir_cnt-=player_speed;
						if(player_dir_cnt >= 32767)
							player_dir_cnt = 0;
					}

					SoundSteps(1);
				}

				if(!player_dir_cnt){
					px = player_x>>FP_BITS;
					py = (player_y>>FP_BITS)+23;

					if(padState&BTN_UP&&!(padState&BTN_DOWN)){
						if((CheckMap(px+6,py)&TILE_LADDER)&&
						   (CheckMap(px+9,py)&TILE_LADDER)){
							PlayerAlignToLadder();
							player_dir = DIR_UP;
						}else
							player_ladder = 0;
					}

					py++;


					if(padState&BTN_DOWN&&!(padState&BTN_UP)){
						if((CheckMap(px+6,py)&TILE_LADDER)&&
						   (CheckMap(px+9,py)&TILE_LADDER)){
							PlayerAlignToLadder();
							player_dir = DIR_DOWN;
						}else
							player_ladder = 0;
					}
				}

				if(!player_ladder){//not on a ladder
					if((padState&BTN_LEFT) && !(padState & BTN_RIGHT)){
						player_dir = DIR_LEFT;
						player_dir_prev = player_dir;
						player_x -= player_speed;
						PlayerCoordWrap();

						player_step_anim++;

						SoundSteps(0);
					}else if(padState&BTN_RIGHT){
						player_dir = DIR_RIGHT;
						player_dir_prev = player_dir;
						player_x += player_speed;
						PlayerCoordWrap();

						player_step_anim++;

						SoundSteps(0);
					}
				}
			}

			i = ((player_step_anim>>2)&3) + (player_answer?4:0);

			switch(player_dir){
				case DIR_LEFT:
					player_spr = (uint8_t *)pgm_read_word(&animWalkLeft[i]);
					player_spr_prev = player_spr;
					break;

				case DIR_RIGHT:
					player_spr = (uint8_t *)pgm_read_word(&animWalkRight[i]);
					player_spr_prev = player_spr;
					break;
			}
		}else{//player_knocked
			switch(player_dir_prev){
				case DIR_LEFT:  player_spr = (uint8_t *)pgm_read_word(&animKnockedLeft [player_knocked_anim?0:1]); break;
				case DIR_RIGHT: player_spr = (uint8_t *)pgm_read_word(&animKnockedRight[player_knocked_anim?0:1]); break;
			}

			player_knocked--;

			if(player_knocked_anim)
				player_knocked_anim--;

			if(!player_knocked)
				player_spr = player_spr_prev;
		}

		if(player_answer)
			player_answer--;

		player_px = player_x>>FP_BITS;
		player_py = player_y>>FP_BITS;

		if(coffee_y){
			if(coffee_wait){
				coffee_wait--;

				if(coffee_wait == 16)
					ZSTriggerFx(SFX_COFFEE_READY,1);

				if(coffee_wait < 16)
					SetTile(coffee_x,coffee_y,coffee_wait&2?87:83);//update_list[UPDL_COFFEE+2] = coffee_wait&2?0x87:0x8f;
			}else{
				if(!((coffee_y*8) >= (player_py+24)||((coffee_y*8)+8)<player_py)){
					if(!((coffee_x*8) >= (player_px+16) || ((coffee_x*8)+8) < player_px)){
						SetTile(coffee_x,coffee_y,87);//update_list[UPDL_COFFEE+2] = 0x87;
						player_coffee = 20;
						coffee_wait = 20*50;
						ZSTriggerFx(SFX_COFFEE,1);
					}
				}
			}
		}

		i = (1<<FP_BITS) + (1<<FP_BITS>>1) + (player_coffee>>1);

		if(player_slowdown)
			player_speed_to = i*4/6;
		else
			player_speed_to = i;

		if(!(frame_cnt&63) && player_coffee)
			player_coffee--;

		if(player_speed < player_speed_to)
			player_speed++;
		if(player_speed > player_speed_to)
			player_speed--;

		if(bonus)
			player_wisdom=100;
		if(player_wisdom)
			player_wisdom--;


		//process topics areas
		if(!player_wisdom){
			for(i=0;i<topic_all;i++){
				px = topic_x[i]*8;
				py = topic_y[i]*8;

				if(player_topic != topic_id[i]){
					if(!(py >= (player_py+24) || (py+8) < player_py)){
						if(!(px >= (player_px+16) || (px+12) < player_px)){
							ZSTriggerFx(SFX_TOPIC,1);
							player_topic = topic_id[i];
							topic_flash_x = (px/8);//px+2;
							topic_flash_y = (py/8)-4;//py-25;
							topic_flash_cnt = 16;
							topic_flash_type = topic_id[i];
							break;
						}
					}
				}
			}
		}

		if(topic_flash_cnt)
			topic_flash_cnt--;

		//process phone counters

		if(bonus){

			if(!phone_runaway){
				if(player_py == ((phone_y[0]*8)-8)){
					if(!((player_px+32) < ((phone_x[0]*8)-16) || player_px > ((phone_x[0]*8)+8+32))){
						i=table_cur;
						while(i == table_cur)
							i = GetPrngNumber(0)%TABLE_MAX;

						MovePhone(i);
						if(phone_runaway_max < 250)
							phone_runaway_max += 10;
						phone_runaway = phone_runaway_max;
						ZSTriggerFx(SFX_TELEPORT,1);
					}
				}
			}else
				phone_runaway--;
		}

		spr = 0;//if any phone is ringing
		spr1 = (ring_cnt>>2)%3;//ring animation

		j = 2+UPDL_PHONES;

		for(i=0;i<phone_all;i++){

			if(phone_level[i] == 255){
				phone_cnt[i]--;

				if(!phone_cnt[i])
					PhoneReset(i,0,0);
				else{
					//TODO BIG CHANGE, MAYBE NOT RIGHT(CHECK ORIGINAL SOURCE)
				//	SetTile(phone_x[i]-1,phone_y[i]-2,0);/////update_list[j]=0xe6;
				//	SetTile(phone_x[i]-0,phone_y[i]-2,0);
				}				
				j += 12;//TODO............

				continue;
			}

			if(phone_level[i]){
				px = phone_level[i]<<2;
				if(!(ring_cnt&32))
					px += ((phone_cnt[i]>>2)&3);

				SetTile(phone_x[i],phone_y[i],pgm_read_byte(&animPhone[px]));//update_list[j]=animPhone[px];

				if(ring_cnt&32){//TODO THIS COULD BE TOTALLY WRONG..........
					SetTile(phone_x[i]-1,phone_y[i],0);///////update_list[j+3]=0;
					SetTile(phone_x[i]+1,phone_y[i],0);///////update_list[j+6]=0;
				}else{//TODO WRONG OR IS THIS THE SIDE RING WAVES??????????
					SetTile(phone_x[i]-1,phone_y[i],4+spr1);///////update_list[j+3]=0xf0+spr1;
					SetTile(phone_x[i]+1,phone_y[i],5+spr1);///////update_list[j+6]=0xf3+spr1;
				}

				if(player_topic == phone_topic[i] || player_wisdom){
					px = phone_x[i]*8;
					py = phone_y[i]*8;

					if(!(py >= (player_py+24) || (py+8) < player_py)){
						if(!(px >= (player_px+16) || (px+8) < player_px)){
							if(!bonus){
								PhoneReset(i,0,1);
								player_answer = 25;

								SetMessage(pgm_read_word(&topicMessages[player_topic][topic_msg[player_topic]*30]));
/////////////////////////////////TODO READ THIS RIGHT
								topic_msg[player_topic] = (topic_msg[player_topic]+1+(GetPrngNumber(0)&3))%topicMessagesCount[player_topic];
							}

							ZSTriggerFx(bonus?SFX_EXPLODE:SFX_ANSWER,2);
							calls_count++;
						}
					}
				}

				if(!(ring_cnt&63))
					spr = 1;
			}

			if(phone_cnt[i])
				phone_cnt[i]--;
			else{
				if(!phone_level[i]){
					SetTile(phone_x[i]-1,phone_y[i]-2,88+(phone_topic[i]*4));////////////update_list[j+9] = TILE_NUM_TOPIC+phone_topic[i];
					SetTile(phone_x[i]-0,phone_y[i]-2,89+(phone_topic[i]*4));			
					spr = 1;
					ring_cnt = 0;
				}

				if(!bonus){
					phone_level[i]++;

					if(phone_level[i] < 4)
						phone_cnt[i] = 200;
					else{
						PhoneReset(i,0,0);
						calls_missed++;

						ZSTriggerFx(calls_missed<calls_missed_level?SFX_MISS:SFX_LOSE,3);
					}
				}else
					phone_level[i] = 2;
			}

			j += 12;
		}

		if(spr)
			ZSTriggerFx(SFX_RINGTONE,2);

		//process npc movements

		player_slowdown = 0;

		if(npc_all){
			px = npc_x;
			py = npc_y;

			switch(npc_type){
				case NPC_CHIEF:   NPCchief();   break;
				case NPC_BOUNCER: NPCbouncer(); break;
				case NPC_CHATTER: NPCchatter(); break;
				case NPC_GEEK:	  NPCgeek();    break;
				case NPC_MANBOX:  NPCboxMan();  break;
				case NPC_DIBROV:  NPCdibrov();  break;
				case NPC_GHOST:   NPCghost();   break;
			}
		}

		//show message
		ShowMessage();

		//show topic
		SetTile(1,0,0);
		SetTile(2,0,0);
		if(!player_wisdom){
			if(frame_cnt&16 && player_topic != 255){
				i = player_topic<<1;
			SetTile(1,0,88+(player_topic*4));
			SetTile(2,0,89+(player_topic*4));
			}
		}else{
			if(frame_cnt&4){
			SetTile(1,0,104);//////	update_list[UPDL_TOPIC+2]=0x53;
			SetTile(2,0,105);//////	update_list[UPDL_TOPIC+5]=0x54;
			}
		}


		//process hearts
		if(player_wisdom&&!bonus){
			if(!(frame_cnt&7))
				HeartsAdd(player_px+8,player_py+4-(spr<<2));
		}

		if(frame_cnt&1)
			spr = 0;
		else
			spr = HEARTS_MAX/2;

		for(i=0;i<(HEARTS_MAX/2);i++){

			if(heart_y[spr]!=240){
				j = heart_cnt[spr]&7;

				if(!(frame_cnt&2)){
					if(j<4)
						heart_x[spr]--;
					else
						heart_x[spr]++;
				}

				heart_y[spr]--;
				heart_cnt[spr]--;

				if(!heart_cnt[spr])
					heart_y[spr] = 240;////TODO HIDE THIS SPRITE RIGHT????
			}

			spr++;
		}

		//check gameover or level clear

		if(calls_count > calls_level)
			calls_count = calls_level;
		if(calls_missed > calls_missed_level)
			calls_missed = calls_missed_level;

		UpdateStats();

		if(calls_count == calls_level)
			break;//level clear
		if(calls_missed == calls_missed_level)
			break;//level lose
		if(player_catch)
			break;//bonus lose
	}//while(1)




	StopSong();

	if(bonus&&calls_count == calls_level){
		///////////update_list[UPDL_PHONES+2] = 0;
		//////////update_list[UPDL_PHONES+5] = 0;
		//////////update_list[UPDL_PHONES+8] = 0;

		spr = 0xf0;

		for(i=0;i<32;i++){
			WaitVsync(1);
			UpdateStats();
			////SetSprite(phone_x[0],phone_y[0],1,0);//oam_spr(phone_x[0],phone_y[0],spr,1,252);
			if((i&1) == 1 && spr < 0xf8)
				spr++;
			frame_cnt++;
		}

		//////////////update_list[UPDL_STATS+7] += 2;

		for(j=0;j<2;j++){
			calls_missed_max++;
			calls_missed=calls_missed_max;
			ZSTriggerFx(SFX_ANSWER,2);

			for(i=0;i<25;i++){
				WaitVsync(1);
				UpdateStats();
				frame_cnt++;
			}
		}
	}

	j = 4*50;

	if(calls_count == calls_level){
		if(bonus)
			SetMessage(pgm_read_word(&bonusMessages[1*30]));
		StartSong((const char *)pgm_read_word(&musicData[MUS_CLEAR]));
	}else{
		if(player_catch){
			SetMessage(pgm_read_word(&bonusMessages[0*30]));
			StartSong((const char *)pgm_read_word(&musicData[MUS_NOBONUS]));
		}else
			j = 3*50;
	}

	for(i=0;i<j;i++){
		WaitVsync(1);
		UpdateStats();
		ShowMessage();
		frame_cnt++;
	}

	ChangeScreen();

	return(calls_count == calls_level || bonus)?1:0;
}


void Intro(){
return;
	ZSPrint(7 ,4,PSTR("ORIGINAL GAME"));
	ZSPrint(10,6,PSTR("PINWIZZ"));
	ZSPrint(11,8,PSTR("SHIRU"));
	ZSPrint(9,10,PSTR("CC*BY 2011"));

	ZSPrint(8,17,PSTR("UZEBOX PORT"));
	ZSPrint(9,19,PSTR("LEE WEBER"));
	ZSPrint(8,21,PSTR("GPL V3 2017"));

	WaitVsync(240);
	ChangeScreen();
}

void TitleScreen(){
return;
	for(i16=0;i16<(30*64);i16++)
		ram_tiles[i16] = pgm_read_byte(&titleTiles[i16]);////////pgm_read_byte(&PaletteStandardToExtendedTable[pgm_read_byte(&titleTiles[i16+(0*32)])]);

	i16 = 2;
	for(uint8_t y=0+2;y<14+2;y++)
		for(uint8_t x=0+3;x<24+3;x++)
			vram[(y*VRAM_TILES_H)+x] = pgm_read_byte(&titleMap[i16++]);//vram[((y>>3)*256)+(8*x)+(y&7)] = pgm_read_byte(&titleMap[i16++]);





	frame_cnt = 0;
	i = 0;

	while(1){
		WaitVsync(1);
		GetPrngNumber(0);
		frame_cnt++;

		if((frame_cnt & 63) < 32)//flash "PRESS  START"
			ZSPrint(9,21-1,PSTR("PRESS  START"));
		else
			ZSPrint(9,21-1,PSTR("            "));


		if(padState && !oldPadState && i<255){
			if(padState == pgm_read_word(&testCode[i])){
				i++;

				if(!pgm_read_word(&testCode[i])){//code entered correctly
					i = 0;
					ZSTriggerFx(SFX_BLA1,1);
					test_mode = !test_mode;
				}
			}else
				i = 0;
		}

		if((padState&BTN_START) && !(oldPadState & BTN_START))
			break;
	}

	ZSTriggerFx(SFX_START,0);
	frame_cnt = 4;

	for(i=0;i<72;i++){//rapid flash "PRESS  START"
		if((frame_cnt & 7) < 4)
			ZSPrint(9,21-1,PSTR("PRESS  START"));
		else
			ZSPrint(9,21-1,PSTR("            "));

		frame_cnt++;
		WaitVsync(1);
	}




	if(test_mode){//sound test
		ZSPrint(9,21-1,PSTR(" SOUND TEST "));

		j = 0;
		px = 0;
		py = 0;

		while(1){
			WaitVsync(1);

			frame_cnt++;

			PrintByte(13,23-1,px,1);
			PrintByte(21,23-1,py,1);
			ZSPrint(8,23-1,PSTR("SFX:"));
			ZSPrint(16,23-1,PSTR("BGM:"));


			if((frame_cnt & 7) < 4)	
					ZSPrint((j==0)?12:20,23-1,PSTR("  "));


			if((padState & BTN_START) && !(oldPadState & BTN_START))
				break;

			if((padState & BTN_LEFT) && !(oldPadState & BTN_LEFT))
				j = 0;
			if((padState & BTN_RIGHT) && !(oldPadState & BTN_RIGHT))
				j = 1;

			if((padState & BTN_UP) && !(oldPadState & BTN_UP)){
				if(!j){
					if(px < SFX_ALL-1)
						px++;
				}else{
					if(py < MUS_ALL-1)

						py++;
				}
			}

			if((padState & BTN_DOWN) && !(oldPadState & BTN_DOWN)){
				if(!j){
					if(px)
						px--;
				}else if(py)
					py--;
			}

			if((padState & BTN_A) && !(oldPadState & BTN_A))
				StopSong();

			if((padState & BTN_B) && !(oldPadState & BTN_B)){
				if(!j)
					ZSTriggerFx(px,0);
				else
					StartSong((const char *)pgm_read_word(&musicData[py]));
			}
		}
	}

	ChangeScreen();
}



void UpdateLevelStr(uint8_t lev){

update_list_len = 0;///////////TODO NECESSARY?
	if(lev == LEVEL_BONUS)
		ZSPrint(12,12,PSTR("BONUS"));
	else{
		if(lev == LEVELS_ALL){
			ZSPrint(12,12,PSTR("WEEKEND"));
			return;		
		}

		if(lev > LEVEL_BONUS)
			lev--;
		ZSPrint(12,12,PSTR("DAY:"));
		SetTile(16,12,TILE_NUM_START+lev+1);
	}

}



void ShowLevelNumber(){
//return;

	UpdateLevelStr(level);


	if(test_mode){
		while(1){
			WaitVsync(1);

			if((padState & (BTN_START|BTN_A|BTN_B)) && !(oldPadState & (BTN_START|BTN_A|BTN_B)))
				break;

			if((padState & BTN_LEFT) && !(oldPadState & BTN_LEFT)){
				if(level){
					level--;
					UpdateLevelStr(level);
				}
			}

			if((padState & BTN_RIGHT) && !(oldPadState & BTN_RIGHT)){
				if(level<LEVELS_ALL){
					level++;
					UpdateLevelStr(level);
				}
			}
		}
	}

	if(level<LEVELS_ALL)
		StartSong((const char *)pgm_read_word(&musicData[MUS_LEVEL]));

	WaitVsync(50*3);
	ChangeScreen();
}



void ShowGameOver(){


	ZSPrint(2,8, PSTR("YOU MISSED TOO MANY CALLS!"));
	ZSPrint(8,10,PSTR("YOU ARE FIRED!"));

	

/*for(uint8_t vx=8;vx<8+14;vx++){//make the "YOU ARE FIRED!" into ram tile indices
		r = vram[(10*VRAM_TILES_Y)+vx]-RAM_TILES_COUNT;
		for(uint16_t rx=t*64;rx<(t*64)+64;rx++){//copy the tile data into next ram tile
			ram_tiles[rx] = pgm_read_byte(&gameTiles[(r*64)]);
		}
		vram[(10*VRAM_TILES_Y)] = t++;
	}*/

	i16=0;

	StartSong((const char *)pgm_read_word(&musicData[MUS_GAMEOVER]));

	while(1){
		WaitVsync(1);
		i16++;

		//if(i16 >= 50*10)
		//	break;
		if(i16 > 50 && (padState & BTN_START) && !(oldPadState & BTN_START))
			break;
	}

	StopSong();
	ZSTriggerFx(SFX_START,0);

	frame_cnt = 0;

	for(i=0;i<50*3;i++){//flash "AND HIRED AGAIN!"
		WaitVsync(1);
		frame_cnt++;

		if((frame_cnt&7) < 4)
			ZSPrint(7,12,PSTR("AND HIRED AGAIN!"));
		else
			ZSPrint(7,12,PSTR("                "));
	}

	ChangeScreen();
}



void ShowCongratulations(){


	spr = 0;
	py = 31;

	for(i=0;i<10;i++){
		SetSprite(72,py,0x3F,3);//spr = oam_spr( 72,py,0x3f,3,spr);
		SetSprite(176,py,0x3F,3);//spr = oam_spr(176,py,0x3f,3,spr);
		py += 8;
	}

	frame_cnt = 0;

	spr = 0;
	i16 = 0;
	j16 = 0;
	i = 255;

	StartSong((const char *)pgm_read_word(&musicData[MUS_WELLDONE]));

	while(1){
		if(i >= 180)
			spr = frame_cnt&64?1:0;
		else{
			if(i<80 || i>=120)
				spr = 2;
			else
				spr = 3;
			i++;
		}

		DrawMetaSprite(128,95,(uint8_t *)pgm_read_word(&animSecretaryRest[spr]));//spr = oam_meta_spr(128,95,20*4,animSecretaryRest[spr]);
		SetSprite(0,240,0,0);//spr = oam_spr(0,240,0,0,spr);
		SetSprite(0,240,0,0);//spr = oam_spr(0,240,0,0,spr);

		WaitVsync(1);

		if(i16 < 50)
			i16++;
		else if((padState & BTN_START) && !(oldPadState & BTN_START))
			break;

		j16++;

		if(j16 >= 1280)
			j16 = 0;

		if(j16 == 1240)
			ZSTriggerFx(SFX_RINGTONE,0);
		
		if(j16 == 1270)
			i = 0;

		frame_cnt++;
	}

	StopSong();
	FadeOut(3,false);
	WaitVsync(25);

	ChangeScreen();
}




int main(){
	SetTileTable(gameTiles);
	InitMusicPlayer(patches);
	GetTrueRandomSeed();
	SetUserPostVsyncCallback(&PostVsync);
	ClearVram();
	SetSpritesTileTable(gameSprites);
	SetMasterVolume(192);
	ChangeScreen();
	Intro();

	test_mode = 0;

	while(1){
		TitleScreen();

		level = 0;
		calls_missed_max = 3;

		while(1){
			calls_missed_level=calls_missed_max;

			ShowLevelNumber();

			if(level == LEVELS_ALL){
				ShowCongratulations();
				break;
			}

			if(GameLoop())
				level++;
			else
				ShowGameOver();
		}
	}
	return 0;
}
