#include <ctype.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <uzebox.h>
#include "data/tiles.inc"
#include "data/sprites.inc"
#include "data/patches.inc"
#include "data/music.inc"

#include "data/maps.inc"
#include "data/messages.inc"

void db(){TriggerFx(1,255,1);}
void dbf(){while(1){db();}}
extern u8 ram_tiles[];
extern u8 masterVolume;
extern void TriggerCommon(Track* track,u8 patch,u8 volume,u8 note);

#define MAP_WDT		SCREEN_TILES_H//32
#define MAP_WDT_BIT	5
#define MAP_HGT		22
#define SCREEN_WIDTH	SCREEN_TILES_H*8
#define SCREEN_HEIGHT	SCREEN_TILES_V*8

#define TILE_SIZE	8
#define TILE_BIT	3

#define FP_BITS		4//4
#define FP_MASK		15//0b00011111//15

#define TILE_EMPTY	0
#define TILE_WALL	1
#define TILE_LADDER	2

#define TILE_NUM_START		0x10//first number tile
#define TILE_ALPHA_START	0x1A//first alpha tile


const uint8_t uzebox_string[] PROGMEM = {
53,58,37,34,47,56,
};

const char uze_sine_table[] PROGMEM = {
3,3,4,5,7,8,9,11,
12,14,15,17,19,21,
23,25,27,30,32,35,37,40,
42,45,48,51,53,56,59,62,
65,68,71,74,77,81,84,87,
90,90,93,96,99,103,106,109,
112,115,118,121,124,127,129,132,
135,138,140,143,145,148,150,153,
155,157,159,161,163,165,166,168,
169,171,172,173,175,176,177,177,
178,179,179,180,180,180,180,180,
180,180,179,179,178,177,177,176,
175,173,172,171,169,168,166,165,
163,161,159,157,155,153,150,148,
145,143,140,138,135,132,129,127,
124,121,118,115,112,109,106,103,
99,96,
};

const uint8_t tileAttr[] PROGMEM = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,2,2,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};


#define DIR_NONE	0
#define DIR_UP		4//4
#define DIR_DOWN	8//8
#define DIR_LEFT	1//1
#define DIR_RIGHT	2//2

static int16_t		player_x;
static uint16_t		player_y;
static uint8_t		player_px;
static uint8_t		player_py;
const uint8_t		*player_spr;
const uint8_t		*player_spr_prev;
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

#define ZS_MASTER_VOL		192
#define SFX_RINGTONE		0//phone ringing
#define SFX_STEP1		1//stepping..
#define SFX_STEP2		2//..
#define SFX_STEP3		3//..
#define SFX_STEP4		4//..
#define SFX_FALL		5//step off an edge
#define SFX_DROP		6//land on ground
#define SFX_TOPIC		7//touch file cabinet
#define SFX_ANSWER		8//answer phone
#define SFX_BLA1		9//chief talking, 2 part effect
#define SFX_MISS		10//missed call, 2 part effect
#define SFX_COFFEE		11//get coffee
#define SFX_START		12//title hit start
#define SFX_PAUSE		13//2 part effect
#define SFX_LOSE		14//2 part effect
#define SFX_BLA2		15//2 part effect
#define SFX_COFFEE_READY	16//coffee cup appears at machine
#define SFX_KNOCK		17//hit by box man
#define SFX_WISDOM		18//touched love interest
#define SFX_EXPLODE		19//bonus stage answer phone
#define SFX_TELEPORT		20//bonus stage phone disappears
#define SFX_WAVES		21/////TODO SPLICED THIS ONE IN, NEED TO FIX OFFSETS IN PLACES......

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
(uint8_t *)mus_clear,
(uint8_t *)mus_gameover,
(uint8_t *)mus_game,

(uint8_t *)mus_welldone,
(uint8_t *)mus_dream,
(uint8_t *)mus_nobonus
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

#define TABLE_MAX	6

static uint8_t	table_all;
static uint8_t	table_x[TABLE_MAX];
static uint8_t	table_y[TABLE_MAX];
static uint8_t	table_cur;

#define TOPIC_MAX	4

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

static uint8_t px,py;
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
const uint8_t	*msg_ptr;
static uint8_t	msg_wait;
static uint8_t	msg_off;

static uint8_t	test_mode;



const uint8_t CoffeeMap[] PROGMEM = {
3,4,
131,132,133,
134,135,136,
137,138,139,
140,141,133,
};

const uint8_t CoffeeMap2[] PROGMEM = {
3,4,
142,143,144,
145,146,147,
148,138,149,
150,151,144,
};

const uint8_t CabinetMap[] PROGMEM = {
2,3,
113,114,
115,116,
117,118,
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


const uint16_t testCode[] PROGMEM = { BTN_B,BTN_A,BTN_B,BTN_A,BTN_LEFT,BTN_UP,BTN_B,BTN_A,0 };

#define FLOORS_MAX	4

static uint8_t floor_left_cnt;
static uint8_t floor_right_cnt;
static uint8_t floor_left [FLOORS_MAX];
static uint8_t floor_right[FLOORS_MAX];

#define LEVELS_ALL	8
#define LEVEL_BONUS	4


void ZSTriggerFx(uint8_t patch){

	for(uint8_t i=1;i<4;i++){//always override footsteps
		if(tracks[i].fxPatchNo >= SFX_STEP1 && tracks[i].fxPatchNo <= SFX_STEP4)		
			tracks[i].flags &= ~TRACK_FLAGS_PRIORITY;
	}
TriggerFx(patch,255,1);//return;
//	uint8_t chan = pgm_read_byte(&sfxdepth[(patch*2)+0]);
	uint8_t depth = pgm_read_byte(&sfxdepth[(patch*2)+1]);

	Track* track;//=&tracks[chan];
//	tracks[chan].flags|=TRACK_FLAGS_PRIORITY;
//	track->patchCommandStreamPos = NULL;
//	TriggerCommon(track,patch,255,80);
//	track->flags|=TRACK_FLAGS_PLAYING;

	if(depth != 255){
		track=&tracks[4];
		tracks[4].flags|=TRACK_FLAGS_PRIORITY;
		track->patchCommandStreamPos = NULL;
		TriggerCommon(track,depth,255,80);
		track->flags|=TRACK_FLAGS_PLAYING;
	}
}


void DrawMapOffset(uint8_t x, uint8_t y, const uint8_t *map, int16_t off){

	u8 mapWidth=pgm_read_byte(&(map[0]));
	u8 mapHeight=pgm_read_byte(&(map[1]));
		
	for(u8 dy=0;dy<mapHeight;dy++){
		for(u8 dx=0;dx<mapWidth;dx++){			
			SetTile(x+dx,y+dy,pgm_read_byte(&(map[(dy*mapWidth)+dx+2]))+off);					
		}
	}
}

uint8_t sprite_count;
void SetSprite(uint8_t x, uint8_t y, uint8_t t, uint8_t f){
//	if(x > (SCREEN_TILES_H*8)-8 || y > (SCREEN_TILES_V*8)-8)
//		return;
	sprites[sprite_count].x = x;
	sprites[sprite_count].y = y;
	sprites[sprite_count].tileIndex = t;
	sprites[sprite_count++].flags = f;
}



void DrawMetaSprite(uint8_t sx, uint8_t sy, const uint8_t *frame){
	if(sy >= SCREEN_HEIGHT)
		return;

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



inline void ZSPrint(uint8_t x, uint8_t y, const char* str){
	Print(x,y,str);
}

void ResetSprites(){
	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].flags = SPRITE_OFF;
	sprite_count = 0;
}


void PostVsync(){
	oldPadState = padState;
	padState = ReadJoypad(0);
}

uint16_t zsabs(int16_t num){
	if(num < 0)
		return -num;
	return num;
}



uint8_t CheckMap(uint8_t x,uint8_t y){
	uint16_t x16 = x>>TILE_BIT;
	uint16_t y16 = y>>TILE_BIT;

	return pgm_read_byte(&tileAttr[vram[(y16*VRAM_TILES_H)+x16]-RAM_TILES_COUNT]);
}



void PlayerCoordWrap(){
	if(player_x < 0)
		player_x = ((SCREEN_WIDTH-16)<<FP_BITS);
	if(player_x > ((SCREEN_WIDTH-16)<<FP_BITS))
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

	phone_topic[id] = (GetPrngNumber(0)&0xFF)%topics_active;

	if(!bonus){
		SetTile(phone_x[id],phone_y[id],pgm_read_byte(&animPhone[0]));
		SetTile(phone_x[id]-1,phone_y[id]-2,0);//erase topic
		SetTile(phone_x[id]-0,phone_y[id]-2,0);
		SetTile(phone_x[id]-1,phone_y[id],0);//erase ring waves
		SetTile(phone_x[id]+1,phone_y[id],0);
	}
	if(answer)
		return;

	if(call_delay>125)
		call_delay -= 10;
}



void SoundSteps(uint8_t dir){
	player_step_cnt += player_speed;

	if(player_step_cnt >= (8<<FP_BITS)){
		ZSTriggerFx(SFX_STEP1+player_step_type+(dir<<1));
		player_step_cnt -= (8<<FP_BITS);
		player_step_type ^= 1;
	}
}



void UpdateStats(){
	if(calls_count != calls_level||(frame_cnt&16)){
		SetTile(16,0,TILE_NUM_START+(calls_count/10));
		SetTile(17,0,TILE_NUM_START+(calls_count%10));
	}else{
		SetTile(16,0,0);
		SetTile(17,0,0);
	}

	if(calls_missed != calls_missed_level || (frame_cnt&16)){
		SetTile(28,0,TILE_NUM_START+calls_missed);
		SetTile(30,0,TILE_NUM_START+calls_missed_max);//in case  updated after bonus level
	}else
		SetTile(28,0,0);
}



void HeartsAdd(uint8_t x,uint8_t y){
	for(uint8_t j=0;j<HEARTS_MAX;j++){
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

void HeartsUpdate(){
	uint8_t t;
	if(frame_cnt&1)
		t = 0;
	else
		t = HEARTS_MAX/2;

	for(uint8_t i=0;i<(HEARTS_MAX/2);i++){
		if(heart_y[t]!=240){
			uint8_t h = heart_cnt[t]&7;

			if(!(frame_cnt&2)){
				if(h<4)
					heart_x[t]--;
				else
					heart_x[t]++;
			}

			heart_y[t]--;
			heart_cnt[t]--;

			if(!heart_cnt[t])
				heart_y[t] = 240;////TODO HIDE THIS SPRITE RIGHT????
		}

		t++;
	}
}

void HeartsDisplay(){
	for(uint8_t i=0;i<HEARTS_MAX;i++){
		uint8_t t;

		if(heart_y[i] > 239)
			continue;
		if(heart_cnt[i] >= 12)
			t = 0x38;
		else
			t = 0x3d-(heart_cnt[i]>>1);

		//spr = oam_spr(heart_x[i],heart_y[i],j,0,spr);
		SetSprite(heart_x[i],heart_y[i],t,0);
	}
}

void SetMessage(const uint8_t *msg){
	msg_ptr = msg;
	msg_cnt = 30;
	msg_wait = 2*50;
	msg_off = 0;
}



void ShowMessage(){
	if(msg_ptr != NULL){
		if(msg_cnt){
			SetFont(msg_off+1,2,pgm_read_byte(msg_ptr+msg_off)-32);
			msg_off++;
			msg_cnt--;
		}else{
			if(msg_wait)
				msg_wait--;
			else{
				msg_ptr = NULL;
				msg_off = 0;
			}
		}
	}else{
		SetTile(msg_off+1,2,0);
		msg_off++;
		if(msg_off>=SCREEN_TILES_H)
			msg_off=0;
	}
}



void ChangeScreen(){
	StopSong();
	FadeOut(5,true);
	WaitVsync(10);
	ClearVram();
	ResetSprites();
	FadeIn(3,false);
}



void MovePhone(uint8_t table){//only used for bonus/dream
	table_cur = table;
	SetTile(phone_x[0]-1,phone_y[0],0);//erase ring waves
	SetTile(phone_x[0]+1,phone_y[0],0);
	SetTile(phone_x[0],phone_y[0],0);//pgm_read_byte(&animPhone[0]));
	phone_x[0] = table_x[table]+1;////TODO HACK +1
	phone_y[0] = table_y[table];
}



void NPCadd(uint16_t x, uint16_t y, uint8_t type){
	npc_type = type;
	npc_x = x;
	npc_y = y;
	npc_dir = (GetPrngNumber(0)&0xFF)<128?DIR_LEFT:DIR_RIGHT;
	npc_cnt = 0;

	switch(type){
		case NPC_CHIEF:
			npc_dir = DIR_NONE;
			npc_cnt = 20;
			npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkLeft[0]);
			npc_ty = SCREEN_TILES_V*8;//240;
			break;

		case NPC_BOUNCER:
			npc_spr = (uint8_t *)pgm_read_word(&animBouncerWalkLeft[0]);
			break;

		case NPC_CHATTER:
			npc_dir =DIR_NONE;
			npc_cnt =20;
			npc_wait=0;
			npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkLeft[0]);
			break;

		case NPC_GEEK:
			npc_spr = (uint8_t *)pgm_read_word(&animGeekWalkLeft[0]);
			break;

		case NPC_MANBOX:
			npc_spr = (uint8_t *)pgm_read_word(&animManBoxWalkLeft[0]);
			break;

		case NPC_DIBROV:
			npc_spr  = (uint8_t *)pgm_read_word(&animDibrovWalkLeft[0]);
			npc_speed=1;
			break;

		default://case NPC_GHOST:
			npc_x = 16<<FP_BITS;
			npc_y = 64<<FP_BITS;//(SCREEN_TILES_V*8)<<FP_BITS;///////////npc_y = 240<<FP_BITS;
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
	if(!npc_all)
		return;

	switch(npc_type){
		case NPC_GHOST:
			DrawMetaSprite(npc_x>>FP_BITS,(npc_y>>FP_BITS)-1,npc_spr);			
			break;

		case NPC_CHIEF:
			if(npc_ty != SCREEN_TILES_V*8){
				SetSprite(npc_tx,npc_ty,0x3F,0);//bl
				SetSprite(npc_tx+8,npc_ty,0x46,0);//a..
			}		
		default:
			if(npc_x != 0 && npc_x != SCREEN_WIDTH-16)
				DrawMetaSprite(npc_x,npc_y-1,npc_spr);//NPC

	}
}



void NPCcheckCollision(){
	if(!player_ladder){
		if(player_py == npc_y){
			if(player_px+8 < npc_x+8){
				if(player_px+16 >= npc_x+8){
					player_x = (npc_x+8-16)<<FP_BITS;
					PlayerCoordWrap();
				}
			}else{
				if(player_px < npc_x+8){
					player_x = (npc_x+8)<<FP_BITS;
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
	if(!(npc_y >= (player_py+24-8) || (npc_y+24)<(player_py+8))){
		if(!((npc_x-32) >= (player_px+16) || (npc_x+16+32) < player_px)){
			player_slowdown = 1;
			npc_dir = DIR_NONE;

			if(!npc_cnt || npc_cnt > 25){
				npc_cnt = 25;
				npc_tx = npc_x-8+((GetPrngNumber(0)&0xFF)&15);
				npc_ty = npc_y-12+((GetPrngNumber(0)&0xFF)&3);
				ZSTriggerFx(SFX_BLA1);
			}else{
				npc_cnt--;
				if(npc_cnt < 10)
					npc_ty = SCREEN_TILES_V*8;//240;
			}

			uint8_t f = (frame_cnt>>3)&3;

			if(player_px < npc_x)
				npc_spr = (uint8_t *)pgm_read_word(&animChiefTalkLeft[f]);
			else
				npc_spr = (uint8_t *)pgm_read_word(&animChiefTalkRight[f]);
		////////	npc_spr = (player_px<px)?animChiefTalkLeft[j]:npc_spr = animChiefTalkRight[j];

			return;
		}
	}

	//HIDE SPRITES?
	npc_ty = SCREEN_TILES_V*8;//240;

	if(npc_cnt){
		uint8_t f = (npc_x>>2)&3;

		switch(npc_dir){
			case DIR_LEFT:
				npc_x--;

				if(npc_x < 9){//||!CheckMap(npc_x-1,npc_y+24))//no platform edges
					npc_dir = DIR_RIGHT;
					NPCchiefSetDelay();
				}

				npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkLeft[f]);
				break;

			case DIR_RIGHT:
			
				npc_x++;

				if(npc_x > (SCREEN_WIDTH-16-9)){//||!CheckMap(npc_x+16,npc_y+24))//no platform edges
					npc_dir = DIR_LEFT;
					NPCchiefSetDelay();
				}

				npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkRight[f]);
				break;

			default:
				if(!(npc_cnt&15)){
					if((GetPrngNumber(0)&0xFF)&128)
						npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkLeft[f]);
					else
						npc_spr = (uint8_t *)pgm_read_word(&animChiefWalkRight[f]);
					//npc_spr = ((GetPrngNumber(0)&0xFF)&128)?animChiefWalkLeft[j]:npc_spr = animChiefWalkRight[j];
				}		
		}

		npc_cnt--;
	}else{//npc_cnt
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

	uint8_t f = (npc_x>>2)&3;

	switch(npc_dir){
		case DIR_LEFT:
			npc_x--;

			if(npc_x < 9)//||!CheckMap(npc_x+4-1,npc_y+24))//no platform edges
				npc_dir = DIR_RIGHT;

			npc_spr = (uint8_t *)pgm_read_word(&animBouncerWalkLeft[f]);
			break;

		case DIR_RIGHT:
			npc_x++;

			if(npc_x > (SCREEN_WIDTH-16-9))//if(npc_x>(256-16-9))//||!CheckMap(npc_x+16-4,npc_y+24))//no platform edges
				npc_dir = DIR_LEFT;

			npc_spr = (uint8_t *)pgm_read_word(&animBouncerWalkRight[f]);
			break;
	}
}



void NPCchatterSetDelay(){
	npc_cnt = 25+((GetPrngNumber(0)&0xFF)&31);
}



void NPCchatter(){
	uint8_t f;
	if(npc_wait){
		if(npc_cnt){
			npc_cnt--;

			f = (frame_cnt>>3)&3;

			if(player_px < npc_x)
				npc_spr = (uint8_t *)pgm_read_word(&animChatterTalkLeft[f]);
			else
				npc_spr = (uint8_t *)pgm_read_word(&animChatterTalkRight[f]);
			//npc_spr = (player_px<px)?animChatterTalkLeft[j]:npc_spr = animChatterTalkRight[j];

			return;
		}

		npc_wait--;

		f = (frame_cnt>>3)%5;

		if(player_px < npc_x)
			npc_spr = (uint8_t *)pgm_read_word(&animChatterGlassesLeft[f]);
		else
			npc_spr = (uint8_t *)pgm_read_word(&animChatterGlassesRight[f]);
		//npc_spr = (player_px<px)?animChatterGlassesLeft[j]:npc_spr = animChatterGlassesRight[j];

		return;
	}

	if(player_py == npc_y){
		if(!((npc_x-16) >= (player_px+16) || (npc_x+16+16) < player_px)){
			if(!npc_wait){
				player_topic = (GetPrngNumber(0)&0xFF)%topics_active;
				npc_dir = DIR_NONE;
				npc_wait = 100;
				npc_cnt = 50;
				ZSTriggerFx(SFX_BLA2);
			}

			return;
		}

		if(player_px < npc_x)
			npc_dir = DIR_LEFT;
		else
			npc_dir = DIR_RIGHT;
	}else{
		if(frame_cnt&1)
			return;
	}

	if(npc_cnt){
		f = (npc_x>>2)&3;

		switch(npc_dir){
			case DIR_LEFT:
				npc_x--;

				if(npc_x < 9){//||!CheckMap(npc_x-1,npc_y+24))//no platform edges

					npc_dir = DIR_RIGHT;
					NPCchatterSetDelay();
				}

				npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkLeft[f]);
				break;

		case DIR_RIGHT:
			
				npc_x++;

				if(npc_x > (SCREEN_WIDTH-16-9)){//||!CheckMap(npc_x+16,npc_y+24))//no platform edges
				
					npc_dir = DIR_LEFT;
					NPCchatterSetDelay();
				}

				npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkRight[f]);
			
			break;

		default:
			if(!(npc_cnt&31)){
				if((GetPrngNumber(0)&0xFF)&128)
					npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkLeft[f]);
				else
					npc_spr = (uint8_t *)pgm_read_word(&animChatterWalkRight[f]);
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

			if(npc_x > SCREEN_WIDTH-64)
				npc_dir = DIR_LEFT;

			if(!npc_dir)
				npc_dir = 1+((GetPrngNumber(0)&0xFF)&1);
		}

		NPCchatterSetDelay();
	}
}



void NPCgeek(){
	uint8_t f;
	if(!player_ladder){
		if(player_py == npc_y && zsabs(((int16_t)player_px)-((int16_t)npc_x)) < 64){
			f = (frame_cnt>>3)&1;

			if(npc_dir == DIR_LEFT && player_px+8 < npc_x+8){
				if(player_dir == DIR_RIGHT && player_px+16 >= npc_x+8){
					player_x = (npc_x+8-16)<<FP_BITS;
					PlayerCoordWrap();

					if(frame_cnt&1){
						if(npc_x < (SCREEN_WIDTH-16))
							npc_x++;
					}
				}

				npc_spr = (uint8_t *)pgm_read_word(&animGeekStandLeft[f]);

				if(!(frame_cnt&31))
					HeartsAdd(npc_x+4,npc_y);

				return;
			}

			if(npc_dir == DIR_RIGHT && player_px+8 > npc_x+8){
				if(player_dir == DIR_LEFT && player_px < npc_x+8){
					player_x = (npc_x+8)<<FP_BITS;
					PlayerCoordWrap();

					if(frame_cnt&1){
						if(npc_x > 0)
							npc_x--;
					}
				}

				npc_spr = (uint8_t *)pgm_read_word(&animGeekStandRight[f]);

				if(!(frame_cnt&31))
					HeartsAdd(npc_x+4,npc_y);

				return;
			}
		}
	}

	if(frame_cnt&1)
		return;

	f = (npc_x>>2)&3;

	switch(npc_dir){
		case DIR_LEFT:
			npc_x--;

			if(npc_x < 9)//||!CheckMap(npc_x+4-1,npc_y+24))//no platform edges
				npc_dir = DIR_RIGHT;

			npc_spr = (uint8_t *)pgm_read_word(&animGeekWalkLeft[f]);
			break;

		case DIR_RIGHT:
			npc_x++;

			if(npc_x > (SCREEN_WIDTH-16-9))//||!CheckMap(npc_x+16-4,npc_y+24))//no platform edges
				npc_dir = DIR_LEFT;

			npc_spr = (uint8_t *)pgm_read_word(&animGeekWalkRight[f]);
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
	npc_x = SCREEN_WIDTH-16;
	npc_y = floor_right[(GetPrngNumber(0)&0xFF)%floor_right_cnt];
}



void NPCboxMan(){
	if(npc_cnt){
		npc_cnt--;
		return;
	}

	if(player_py == npc_y){
		if(!((player_px+16-4) < npc_x || player_px >= (npc_x+16-4))){
			if(!player_knocked){
				player_knocked = 48;
				player_knocked_anim = 8;
				npc_dir = (npc_dir == DIR_LEFT)?DIR_RIGHT:DIR_LEFT;
				ZSTriggerFx(SFX_KNOCK);
			}else
				player_knocked++;
		}
	}

	if(frame_cnt&1)
		return;

	uint8_t f = (npc_x>>2)&3;

	switch(npc_dir){
		case DIR_LEFT:
			npc_x--;

			if(npc_x < -7){//-7){//////was hacked to 0
				NPCchangeFloorLeft();
				npc_cnt = 50;
			}else if(!CheckMap(npc_x+4-1,npc_y+24))
				npc_dir = DIR_RIGHT;

			npc_spr = (uint8_t *)pgm_read_word(&animManBoxWalkLeft[f]);
			break;

		case DIR_RIGHT:
			npc_x++;

			if(npc_x >= SCREEN_WIDTH-16){//if(npc_x >= 248){//////was hacked to -16
				NPCchangeFloorRight();
				npc_cnt = 50;
			}else if(!CheckMap(npc_x+16-4,npc_y+24))
				npc_dir = DIR_LEFT;

			npc_spr = (uint8_t *)pgm_read_word(&animManBoxWalkRight[f]);
		break;
	}
}



void NPCdibrov(){
	uint8_t f;
	if(npc_cnt){
		npc_cnt--;
		return;
	}

	if(player_py == npc_y){
		if(!((player_px+16-4) < npc_x || player_px >= (npc_x+16-4))){
			if(!player_wisdom){
				player_wisdom = 10*50;
				ZSTriggerFx(SFX_WISDOM);

				for(uint8_t i=0;i<5;i++)//3
					HeartsAdd(player_px+4,player_py+8-(i<<2));
			}

		}

		npc_speed = 2;

		npc_dir = ((int)player_px)<npc_x?DIR_RIGHT:DIR_LEFT;
	}else{
		npc_speed = 1;

		if(zsabs(((int16_t)player_px)-npc_x) < 64)
			npc_dir = ((int)player_px) < npc_x?DIR_RIGHT:DIR_LEFT;
	}

	if(npc_speed < 2 && frame_cnt&1)
		return;

	f = npc_speed;

	if(f == 2 && frame_cnt&1)
		f = 1;

	switch(npc_dir){//no platform edges on the level

		case DIR_LEFT:
			npc_x -= f;
			break;

		case DIR_RIGHT:
			npc_x += f;
			break;
	}

	if(npc_x >= SCREEN_WIDTH-8 || npc_x < -7){////////if(npc_x>=248||npc_x<-7){
		npc_cnt = player_wisdom?player_wisdom+25:25;

		if(player_px < 128)
			NPCchangeFloorRight();
		else
			NPCchangeFloorLeft();
	}

	f = (npc_x>>2)&3;

	if(npc_dir == DIR_LEFT)
		npc_spr = (uint8_t *)pgm_read_word(&animDibrovWalkLeft[f]);
	else
		npc_spr = (uint8_t *)pgm_read_word(&animDibrovWalkRight[f]);
}



void NPCghost(){
	if(npc_wait){
		npc_wait--;
		return;
	}

	npc_cnt++;

	if(npc_cnt >= 100){
		ZSTriggerFx(SFX_BLA1);
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

	if(player_x < npc_x){
		if(npc_y < (SCREEN_HEIGHT<<FP_BITS) && npc_dx>-16)//if(npc_y < (240<<FP_BITS) && npc_dx>-16)
			npc_dx--;

		npc_spr = npc_cnt<20?sprChiefGhostBlaL:sprChiefGhostL;
	}else{
		if(npc_y < ((SCREEN_HEIGHT-32)<<FP_BITS)&&npc_dx < 16)//if(npc_y<(200<<FP_BITS)&&npc_dx< 16)//if(npc_y<(240<<FP_BITS)&&npc_dx< 16)
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
	uint16_t loff = 0;
	uint8_t x,y,l,t,did_left;
	ClearVram();
	WaitVsync(1);

	floor_left_cnt = 0;
	floor_right_cnt = 0;
	topic_flash_x = 0;

	bonus = (level == LEVEL_BONUS)?1:0;

	for(uint8_t i=0;i<level;i++){
		loff += 3;//eat tile set, calls, delay between calls
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
		while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat coffee machine
			loff += 1;
		while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat clock
			loff += 1;
		while(pgm_read_byte(&lvl_data[loff++]) != 255)//eat door
			loff += 1;
	}

	uint8_t tile_set = pgm_read_byte(&lvl_data[loff++]);
	calls_level = pgm_read_byte(&lvl_data[loff++]);
	call_delay = pgm_read_byte(&lvl_data[loff++])*5;
	//topics_active = pgm_read_byte(&lvl_data[loff++]);
	topics_active = 0;////////////////////////////////OTHERWISE DO NOT ++ WHEN THE CABINET IS FOUND IN MAP DATA

	player_x = (pgm_read_byte(&lvl_data[loff++])*8)<<FP_BITS;
	player_y = ((pgm_read_byte(&lvl_data[loff++])*8)-16)<<FP_BITS;


	while(1){//NPCs

		x = pgm_read_byte(&lvl_data[loff++]);
		if(x == 255)
			break;			
		x *= 8;
		y = pgm_read_byte(&lvl_data[loff++])*8;
		t = pgm_read_byte(&lvl_data[loff++]);
		NPCadd(x,y,t);

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

	//uint8_t marked_left;
	while(1){//platforms
		x = pgm_read_byte(&lvl_data[loff++]);
		if(x == 255)
			break;			
		y = pgm_read_byte(&lvl_data[loff++]);
		l = pgm_read_byte(&lvl_data[loff++]);
		did_left = 0;
		//marked_left = 0;
		uint8_t firsttile = 98+(tile_set*3);
		//floor_left[floor_left_cnt++] = y+1;/////TODO NOT RIGHT
////////	npc_y = floor_left[(GetPrngNumber(0)&0xFF)%floor_left_cnt];
		
		//floor_left[floor_left_cnt++] = ;

		while(l--){
			if(x == 0){
				did_left = 1;
				floor_left[floor_left_cnt++] = (y*8)-24;
				//marked_left = 1;////ok to have a floor marked left and right since npc cant use?
			}

			if(!did_left){
				t = firsttile+0;
				did_left = 1;				
			}else if(l || x == SCREEN_TILES_H-1){
				t = firsttile+1;
				if(x == SCREEN_TILES_H-1){
					floor_right[floor_right_cnt++] = (y*8)-24;
					//if(marked_left)//this is not actually a floor
					//	floor_left_cnt--;
				}
			}else{
				t = firsttile+2;
				//floor_right[floor_right_cnt++] = y+1;
			}
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
		DrawMapOffset(x,y,(const uint8_t *)CabinetMap,(level%3)*6);
		t = 80+(t*4);
		SetTile(x+0,y-2,t+0);
		SetTile(x+1,y-2,t+1);
		topic_x[topic_all] = x;
		topic_y[topic_all] = y+2;
		topic_id[topic_all++] = (t-80)/4;
		//topic_msg[spr] = GetPrngNumber(0)%topicMessagesCount[spr];
		//topic_all++;
		topics_active++;
			
		
	}

	while(1){//desks
		x = pgm_read_byte(&lvl_data[loff++]);
		if(x == 255)
			break;			
		y = pgm_read_byte(&lvl_data[loff++]);
		DrawMap(x,y,(const char *)DeskMap);
		phone_x[phone_all] = x+1;
		phone_y[phone_all] = y;
		PhoneReset(phone_all,((uint16_t)phone_all)<<8,0);
		if(bonus)//hide phones for bonus level
			SetTile(x+1,y,0);
		phone_all++;

		table_x[table_all] = x;
		table_y[table_all++] = y;
	}
		
	while(1){//coffee machine
		x = pgm_read_byte(&lvl_data[loff++]);
		if(x == 255)
			break;			
		y = pgm_read_byte(&lvl_data[loff++]);
		DrawMap(x,y,(const char *)(tile_set%2 ? CoffeeMap2:CoffeeMap));
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


	for(uint8_t i=0;i<3;i++)
		topic_msg[i] = (GetPrngNumber(0) % pgm_read_byte(&topicMessagesCount[i]));

}

uint8_t GameLoop(){

	frame_cnt = 0;
	ring_cnt = 0;
	pause = 0;
	calls_count = 0;
	calls_missed = 0;

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

	UpdateStats();

	LoadLevel();

	ZSPrint(0,0,PSTR("    DAY:   CALL:  +    MISS: + "));
	SetTile(SCREEN_TILES_H-13,0,TILE_NUM_START+(calls_level/10));
	SetTile(SCREEN_TILES_H-12,0,TILE_NUM_START+(calls_level%10));
	SetTile(SCREEN_TILES_H-2,0,TILE_NUM_START+calls_missed_level);

	if(!bonus){
		uint8_t l = level;
		if(l > LEVEL_BONUS)
			l--;
		SetTile(9,0,TILE_NUM_START+1+l);	
	}else
		ZSPrint(0,0,PSTR("     DREAM "));

	if(bonus){
		//update_list_len += 6*3;
		phone_all = 1;
		MovePhone(5);
		PhoneReset(0,25,0);
	}

	heart_ptr = 0;

	for(uint8_t i=0;i<HEARTS_MAX;i++)
		heart_y[i] = 240;/////////TODO HIDE THIS SPRITE RIGHT?????

	if(!bonus)
		StartSong((const char *)pgm_read_word(&musicData[MUS_GAME]));
	else
		StartSong((const char *)pgm_read_word(&musicData[MUS_DREAM]));



	while(1){
		WaitVsync(1);
		GetPrngNumber(0);
//if(bonus)
//DDRC = 0b10100100;

		if(!pause && (padState&BTN_START) && !(oldPadState&BTN_START)){//start pause
			ZSTriggerFx(SFX_PAUSE);
			StopSong();
			DDRC = 0b10100100;//0b10010010;
			pause = 50;
			continue;
		}

		if(pause == 25){//we are paused and the retrigger delay is over

			if((padState & BTN_START) && !(oldPadState & BTN_START)){
				DDRC = 0b11111111;
				ZSTriggerFx(SFX_PAUSE);
				ResumeSong();
				pause = 24;
			}
			continue;
		}else if(pause){//pause delay countdown
			pause--;
			continue;
		}

		ResetSprites();

		//process player movements
		px = player_x>>FP_BITS;
		py = player_y>>FP_BITS;

		if(!player_knocked){
			if(!CheckMap(px+4,py+24) && !CheckMap(px+12,py+24)){//nothing underneath the player?
				player_y += 4<<FP_BITS;
				player_ladder = 0;

				if(player_floor){
					player_floor = 0;
					ZSTriggerFx(SFX_FALL);
				}
			}else{//there is something underneath the player
				if(!player_floor){//player just touched the ground after a fall
					player_floor = 1;
					ZSTriggerFx(SFX_DROP);
				}

				if(player_dir_cnt){
					switch(player_dir){
						case DIR_UP:
							player_y -= player_speed;

							if(player_y < (16<<FP_BITS))//////if(player_y < (32<<FP_BITS))
								player_y += ((SCREEN_TILES_V*8)-8-32)<<FP_BITS;////////////player_y += (240-48-32)<<FP_BITS;

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

							if(player_y > (((SCREEN_TILES_V*8)-24)<<FP_BITS))/////////////if(player_y > ((240-48)<<FP_BITS))
								player_y -= ((SCREEN_TILES_V*8)-8-32)<<FP_BITS;//////////player_y -= (240-48-32)<<FP_BITS;

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
						player_dir_cnt -= player_speed;
						if(player_dir_cnt >= 32767)
							player_dir_cnt = 0;
					}

					SoundSteps(1);
				}

				if(!player_dir_cnt){
					px = player_x>>FP_BITS;
					py = (player_y>>FP_BITS)+23;

					if(padState&BTN_UP && !(padState&BTN_DOWN)){
						if((CheckMap(px+6,py)&TILE_LADDER)&&
						   (CheckMap(px+9,py)&TILE_LADDER)){
							PlayerAlignToLadder();
							player_dir = DIR_UP;
						}else
							player_ladder = 0;
					}

					py++;


					if(padState&BTN_DOWN && !(padState&BTN_UP)){
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

			uint8_t f = ((player_step_anim>>2)&3) + (player_answer?4:0);

			switch(player_dir){
				case DIR_LEFT:
					player_spr = (uint8_t *)pgm_read_word(&animWalkLeft[f]);
					player_spr_prev = player_spr;
					break;

				case DIR_RIGHT:
					player_spr = (uint8_t *)pgm_read_word(&animWalkRight[f]);
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
					ZSTriggerFx(SFX_COFFEE_READY);

				if(coffee_wait < 16)
					SetTile(coffee_x,coffee_y,coffee_wait&2?138:152);
			}else{
				if(!((coffee_y*8) >= (player_py+24)||((coffee_y*8)+8)<player_py)){
					if(!((coffee_x*8) >= (player_px+16) || ((coffee_x*8)+8) < player_px)){
						SetTile(coffee_x,coffee_y,138);
						player_coffee = 20;
						coffee_wait = 20*50;
						ZSTriggerFx(SFX_COFFEE);
					}
				}
			}
		}

		uint8_t t = (1<<FP_BITS) + (1<<FP_BITS>>1) + (player_coffee>>1);

		if(player_slowdown)
			player_speed_to = t*4/6;
		else
			player_speed_to = t;

		if(!(frame_cnt&63) && player_coffee)
			player_coffee--;

		if(player_speed < player_speed_to)
			player_speed++;
		if(player_speed > player_speed_to)
			player_speed--;

		if(bonus)
			player_wisdom = 100;
		if(player_wisdom)
			player_wisdom--;


		//process topics areas
		if(!player_wisdom){
			for(uint8_t i=0;i<topic_all;i++){
				px = topic_x[i]*8;
				py = topic_y[i]*8;

				if(player_topic != topic_id[i]){
					if(!(py >= (player_py+24) || (py+8) < player_py)){
						if(!(px >= (player_px+16) || (px+12) < player_px)){
							ZSTriggerFx(SFX_TOPIC);
							if(topic_flash_x != 0){
								SetTile(topic_flash_x+0,topic_flash_y,80+(topic_flash_type*4));//restore old topic in case back to back topic pickups
								SetTile(topic_flash_x+1,topic_flash_y,81+(topic_flash_type*4));
							}							
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
			uint8_t t;
			if(!phone_runaway){
				if(player_py == ((phone_y[0]*8)-8)){
					if(!((player_px+32) < ((phone_x[0]*8)-16) || player_px > ((phone_x[0]*8)+8+32))){
						t = table_cur;
						while(t == table_cur)
							t = GetPrngNumber(0)%TABLE_MAX;

						MovePhone(t);
						if(phone_runaway_max < 250)
							phone_runaway_max += 10;
						phone_runaway = phone_runaway_max;
						ZSTriggerFx(SFX_TELEPORT);

					}
				}
			}else
				phone_runaway--;

			//do some flicker for this stage
			//if(GetPrngNumber(0)&31 == 12)
			//	DDRC = 0b10100100;
			//else if(frame_cnt&15 < 2)
			//	DDRC = 0b11111111;
		}

		uint8_t phone_ringing = 0;//if any phone is ringing
		uint8_t rframe = (ring_cnt>>2)%3;//ring animation

		for(uint8_t i=0;i<phone_all;i++){

			if(phone_level[i] == 255){
				phone_cnt[i]--;

				if(!phone_cnt[i])
					PhoneReset(i,0,0);
				else{
					//TODO BIG CHANGE, MAYBE NOT RIGHT(CHECK ORIGINAL SOURCE)
				//	SetTile(phone_x[i]-1,phone_y[i]-2,0);/////update_list[j]=0xe6;
				//	SetTile(phone_x[i]-0,phone_y[i]-2,0);
				}	
				//////j += 12;//TODO............

				continue;
			}

			if(phone_level[i]){
				uint8_t ptile = phone_level[i]<<2;
				if(!(ring_cnt&32))
					ptile += ((phone_cnt[i]>>2)&3);

				SetTile(phone_x[i],phone_y[i],pgm_read_byte(&animPhone[ptile]));

				if(ring_cnt&32){
					SetTile(phone_x[i]-1,phone_y[i],0);
					SetTile(phone_x[i]+1,phone_y[i],0);
				}else{
					SetTile(phone_x[i]-1,phone_y[i],73+rframe);
					SetTile(phone_x[i]+1,phone_y[i],76+rframe);
				}

				if(player_topic == phone_topic[i] || player_wisdom){
					px = phone_x[i]*8;
					py = phone_y[i]*8;

					if(!(py >= (player_py+24) || (py+8) < player_py)){
						if(!(px >= (player_px+16) || (px+8) < player_px)){
							if(!bonus){
								PhoneReset(i,0,1);
								player_answer = 25;
								SetMessage((uint8_t *)(pgm_read_word(&topicMessages[player_topic])+(topic_msg[player_topic]*30)));
/////////////////////////////////TODO READ THIS RIGHT
								topic_msg[player_topic] = (topic_msg[player_topic]+1+(GetPrngNumber(0)&3))%pgm_read_byte(&topicMessagesCount[player_topic]);
							}

							ZSTriggerFx(bonus?SFX_EXPLODE:SFX_ANSWER);
							calls_count++;
						}
					}
				}

				if(!(ring_cnt&63))
					phone_ringing = 1;
			}

			if(phone_cnt[i])
				phone_cnt[i]--;
			else{
				if(!phone_level[i]){
					if(!bonus){
						SetTile(phone_x[i]-1,phone_y[i]-2,80+(phone_topic[i]*4));
						SetTile(phone_x[i]-0,phone_y[i]-2,81+(phone_topic[i]*4));			
					}					
					phone_ringing = 1;
					ring_cnt = 0;
				}

				if(!bonus){
					phone_level[i]++;

					if(phone_level[i] < 4)
						phone_cnt[i] = 200;
					else{
						PhoneReset(i,0,0);
						calls_missed++;

						ZSTriggerFx(calls_missed < calls_missed_level ? SFX_MISS:SFX_LOSE);
					}
				}else
					phone_level[i] = 2;
			}
		}

		if(phone_ringing)
			ZSTriggerFx(SFX_RINGTONE);

		//process npc movements
		player_slowdown = 0;

		if(npc_all){
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
		SetTile(1,0,0);//blank the area and draw back over if necessary
		SetTile(2,0,0);
		if(!player_wisdom){
			if(frame_cnt&16 && player_topic != 255){
				///////////i = player_topic<<1;/////todo uneeded????
				SetTile(1,0,80+(player_topic*4));
				SetTile(2,0,81+(player_topic*4));
			}
		}else{
			if(frame_cnt&4){
				SetTile(1,0,96);//////	update_list[UPDL_TOPIC+2]=0x53;
				SetTile(2,0,97);//////	update_list[UPDL_TOPIC+5]=0x54;
			}
		}


		t = phone_ringing;
		//process hearts
		if(player_wisdom && !bonus){
			if(!(frame_cnt&7))
				HeartsAdd(player_px+8,player_py+4-(t<<2));
		}

		HeartsUpdate();

		frame_cnt++;
		ring_cnt++;
		
		//display player
		px = player_x>>FP_BITS;
		py = (player_y>>FP_BITS)-1;

	//	if(px<8)
	//		px += (256-16);
	//	else if(px >= (256-16-8))
	//		px += 16;

		DrawMetaSprite(px,py,player_spr);

		//display npcs
		NPCdisplay();

		//display hearts
		HeartsDisplay();

		//display flashing topic icon when needed(player just touched a cabinet)
		if(topic_flash_cnt&2){
			SetTile(topic_flash_x+0,topic_flash_y,82+(topic_flash_type*4));
			SetTile(topic_flash_x+1,topic_flash_y,83+(topic_flash_type*4));
		}else if(topic_flash_cnt){
			SetTile(topic_flash_x+0,topic_flash_y,80+(topic_flash_type*4));
			SetTile(topic_flash_x+1,topic_flash_y,81+(topic_flash_type*4));
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

	if(bonus && calls_count == calls_level){//beat bonus level

		uint8_t t = 0x2f;
		
		sprites[MAX_SPRITES-1].x = phone_x[0]*8;
		sprites[MAX_SPRITES-1].y = phone_y[0]*8;
		sprites[MAX_SPRITES-1].flags = 0;
		for(uint8_t i=0;i<32;i++){
			sprites[MAX_SPRITES-1].tileIndex = t;
			WaitVsync(1);
			UpdateStats();
			if((i&1) == 1 && t < 0x37)
				t++;
			frame_cnt++;
		}

		for(uint8_t j=0;j<2;j++){
			calls_missed_max++;
			calls_missed = calls_missed_max;
			ZSTriggerFx(SFX_ANSWER);

			for(uint8_t i=0;i<25;i++){
				WaitVsync(1);
				UpdateStats();
				frame_cnt++;
			}
		}
	}

	uint8_t t = 4*50;

	if(calls_count == calls_level){//beat level
		if(bonus)
			SetMessage(bonusMessages+30);
		StartSong((const char *)pgm_read_word(&musicData[MUS_CLEAR]));
	}else{
		if(player_catch){
			SetMessage(bonusMessages+0);
			StartSong((const char *)pgm_read_word(&musicData[MUS_NOBONUS]));
		}else
			t = 3*50;
	}

	for(uint8_t i=0;i<t;i++){
		WaitVsync(1);
		UpdateStats();
		ShowMessage();
		frame_cnt++;
	}

	ChangeScreen();

	return(calls_count == calls_level || bonus)?1:0;
}


void Intro(){
test_mode = 1;
return;
	ResetSprites();
	ClearVram();
	WaitVsync(1);

	TriggerFx(40,255,0);
	Track *track=&tracks[4];
	tracks[4].flags|=TRACK_FLAGS_PRIORITY;
	track->patchCommandStreamPos = NULL;
	TriggerCommon(track,41,255,80);
	track->flags|=TRACK_FLAGS_PLAYING;


	for(uint8_t i=0;i<180;i++){
		ResetSprites();
		sprite_count = 6;
		frame_cnt++;
		HeartsUpdate();
		HeartsDisplay();
		for(uint8_t j=0;j<6;j++){
			sprites[j].tileIndex = pgm_read_byte(&uzebox_string[j]);
			sprites[j].flags = SPRITE_BANK1;
			if(i+(j*10) >= sizeof(uze_sine_table))
				sprites[j].y = 88;
			else
				sprites[j].y = pgm_read_byte(&uze_sine_table[i+(j*10)]);

			//sprites[j].y -= 8;
			sprites[j].x = (j*8)+104;
			if(i == 30+(j*5) || i == 60+(j*11)){//dbf();
				HeartsAdd(sprites[j].x,sprites[j].y);			
			}		
		}
		WaitVsync(1);
	}
	
	ResetSprites();
	for(uint8_t i=0;i<6;i++)//get our ram tile indices back
		vram[(11*VRAM_TILES_H)+i+13] = i;

	uint8_t pixels = 0;
	for(uint8_t j=0;j<2;j++){
		for(uint16_t i=0;i<64*6;i++){
			if(ram_tiles[i])
				ram_tiles[i] = j?0b11010111:0b10000111;
			if(ram_tiles[(64*6)-i])
				ram_tiles[(64*6)-i] = j?0b11010111:0b10100111;
			if(++pixels > 6){
				pixels = 0;
				WaitVsync(1);
			}
		}
	}

	WaitVsync(30);
	FadeOut(2,true);
	ResetSprites();
	ClearVram();


	FadeIn(2,false);
	ZSPrint(8 ,4,PSTR("ORIGINAL GAME"));
	ZSPrint(11,6,PSTR("PINWIZZ"));
	ZSPrint(12,8,PSTR("SHIRU"));
	ZSPrint(10,10,PSTR("CC*BY 2011"));

	ZSPrint(9,17,PSTR("UZEBOX PORT"));
	ZSPrint(10,19,PSTR("LEE WEBER"));
	ZSPrint(9,21,PSTR("GPL V3 2017"));

	WaitVsync(1);
	for(uint16_t i=0;i<VRAM_SIZE;i++){//blue numerical tiles are nice in game, but clash on the intro
		if(vram[i] >= ('0'-32)+RAM_TILES_COUNT && vram[i] <= ('9'-32)+RAM_TILES_COUNT)
			vram[i] -= ('0'-32)+RAM_TILES_COUNT;
	}
	for(uint16_t i=0;i<10*64;i++){//turn numerical tiles white to look better
		ram_tiles[i] = pgm_read_byte(&gameTiles[(64*16)+i]);
		if(ram_tiles[i])
			ram_tiles[i] = 255;
	}
	WaitVsync(240);
	ChangeScreen();
}

void TitleScreen(){

//return;
//	for(i16=0;i16<(30*64);i16++)
//		ram_tiles[i16] = pgm_read_byte(&titleTiles[i16]);////////pgm_read_byte(&PaletteStandardToExtendedTable[pgm_read_byte(&titleTiles[i16+(0*32)])]);

//	i16 = 2;
//	for(uint8_t y=0+2;y<14+2;y++)
//		for(uint8_t x=0+3;x<24+3;x++)
//			vram[(y*VRAM_TILES_H)+x] = pgm_read_byte(&titleMap[i16++]);//vram[((y>>3)*256)+(8*x)+(y&7)] = pgm_read_byte(&titleMap[i16++]);
	DrawMap(4,1,titleMap);
	ZSPrint(7,14,PSTR("ZOOMING   SECRETARY"));
	for(uint8_t x=7;x<7+19;x++){
		uint8_t t = vram[(14*VRAM_TILES_H)+x]-RAM_TILES_COUNT;
		for(uint8_t r=0;r<64;r++){
			uint8_t t2 = pgm_read_byte(&gameTiles[(t*64)+r]);
			if(t2 == 0x00)
				t2 = 0xD8;
			else if(t2 == 0xFF){
				if(x < 14)
					t2 = 0b00111000;
				else
					t2 = 0b00000111;
			}
			ram_tiles[((x-7)*64)+r] = t2;		
		}
		vram[(14*VRAM_TILES_H)+x] = x-7;
	}

	frame_cnt = 0;
	uint8_t t = 0;

	while(1){
		WaitVsync(1);
		GetPrngNumber(0);
		frame_cnt++;

		if((frame_cnt & 63) < 32)//flash "PRESS  START"
			ZSPrint(10,21-1,PSTR("PRESS  START"));
		else
			ZSPrint(10,21-1,PSTR("            "));


		if(padState && !oldPadState && t < 255){
			if(padState == pgm_read_word(&testCode[t])){
				t++;

				if(!pgm_read_word(&testCode[t])){//code entered correctly
					t = 0;
					ZSTriggerFx(SFX_BLA1);
					test_mode = !test_mode;
				}
			}else
				t = 0;
		}

		if((padState&BTN_START) && !(oldPadState & BTN_START))
			break;
	}

	ZSTriggerFx(SFX_START);
	frame_cnt = 4;

	for(uint8_t i=0;i<72;i++){//rapid flash "PRESS  START"
		if((frame_cnt & 7) < 4)
			ZSPrint(10,21-1,PSTR("PRESS  START"));
		else
			ZSPrint(10,21-1,PSTR("            "));

		frame_cnt++;
		WaitVsync(1);
	}




	if(test_mode){//sound test
		ZSPrint(10,21-1,PSTR(" SOUND TEST "));
		uint8_t t2,t3;
		t = t2 = t3 = 0;


		while(1){
			WaitVsync(1);

			frame_cnt++;

			PrintByte(14,23-1,t2,1);
			PrintByte(22,23-1,t3,1);
			ZSPrint(9,23-1,PSTR("SFX:"));
			ZSPrint(17,23-1,PSTR("BGM:"));


			if((frame_cnt & 7) < 4)	
					ZSPrint((t==0)?13:21,23-1,PSTR("  "));


			if((padState & BTN_START) && !(oldPadState & BTN_START))
				break;

			if((padState & BTN_LEFT) && !(oldPadState & BTN_LEFT))
				t = 0;
			if((padState & BTN_RIGHT) && !(oldPadState & BTN_RIGHT))
				t = 1;

			if((padState & BTN_UP) && !(oldPadState & BTN_UP)){
				if(!t){
					if(t2 < SFX_ALL-1)
						t2++;
				}else{
					if(t3 < MUS_ALL-1)
						t3++;
				}
			}

			if((padState & BTN_DOWN) && !(oldPadState & BTN_DOWN)){
				if(!t){
					if(t2)
						t2--;
				}else if(t3)
					t3--;
			}

			if((padState & BTN_A) && !(oldPadState & BTN_A))
				StopSong();

			if((padState & BTN_B) && !(oldPadState & BTN_B)){
				if(!t)
					ZSTriggerFx(t2);
				else
					StartSong((const char *)pgm_read_word(&musicData[t3]));
			}
		}
	}

	ChangeScreen();
}



void UpdateLevelStr(uint8_t lev){

	if(lev == LEVEL_BONUS)
		ZSPrint(12,11,PSTR(" DREAM "));
	else{
		if(lev == LEVELS_ALL){
			ZSPrint(12,11,PSTR("WEEKEND"));
			return;		
		}

		if(lev > LEVEL_BONUS)
			lev--;
		ZSPrint(12,11,PSTR(" DAY:   "));//spaces to cover up "WEEKEND"
		SetTile(17,11,TILE_NUM_START+lev+1);
	}

}



void ShowLevelNumber(){

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

	ZSPrint(3,8, PSTR("YOU MISSED TOO MANY CALLS!"));//blue
	ZSPrint(9,11,PSTR("YOU ARE FIRED!"));//white(unmodified)
	
	uint16_t rtoff = 0;
	uint16_t toff = 0;
	for(uint8_t vx=3;vx<29;vx++){
		if(vram[(8*VRAM_TILES_H)+vx] == RAM_TILES_COUNT)//blank space
			continue;

		toff = vram[(8*VRAM_TILES_H)+vx]-RAM_TILES_COUNT;//get the flash tile index
		vram[(8*VRAM_TILES_H)+vx] = rtoff/64;//convert to ram tile index
		toff *= 64;//get our offset into the flash tile

		for(uint8_t i=0;i<64;i++)//copy the flash tile data into the ram tile we replaced it with
			ram_tiles[rtoff++] = pgm_read_byte(&gameTiles[toff++])&0b11001001;//make white font blue
	}

	StartSong((const char *)pgm_read_word(&musicData[MUS_GAMEOVER]));

	uint16_t minframes = 0;
	while(1){
		WaitVsync(1);
		minframes++;

		if(minframes > 50 && (padState & BTN_START) && !(oldPadState & BTN_START))
			break;
	}

	StopSong();
	ZSTriggerFx(SFX_START);

	frame_cnt = 0;
	uint16_t basertoff = rtoff;

	for(uint8_t i=0;i<50*3;i++){//flash "AND HIRED AGAIN!"
		WaitVsync(1);
		frame_cnt++;

		if((frame_cnt&7) < 4){
			ZSPrint(8,14,PSTR("AND HIRED AGAIN!"));//light blue
			rtoff = basertoff;
			for(uint8_t vx=3;vx<29;vx++){//color the tiles
				if(vram[(14*VRAM_TILES_H)+vx] == RAM_TILES_COUNT)//blank space
					continue;

				toff = vram[(14*VRAM_TILES_H)+vx]-RAM_TILES_COUNT;//get the flash tile index
				vram[(14*VRAM_TILES_H)+vx] = rtoff/64;//convert to ram tile index
				toff *= 64;//get our offset into the flash tile

				for(uint8_t i=0;i<64;i++)//copy the flash tile data into the ram tile we replaced it with
					ram_tiles[rtoff++] = pgm_read_byte(&gameTiles[toff++])&0b11010010;//make white font blue
			}		
		}else if(frame_cnt < 148)
			ZSPrint(8,14,PSTR("                "));
	}

	ChangeScreen();
}



void ShowCongratulations(){

	DrawMap(4,3,winMap);
	ZSPrint(6,21,PSTR("YOU DID A GREAT JOB!"));
	ZSPrint(4,23,PSTR("NOW YOU CAN TAKE A REST!"));

	frame_cnt = 0;

	uint8_t i = 255;
	uint16_t f = 0;
	uint16_t t = 0;
	uint8_t s = 0;
	uint8_t w = 0;

	StartSong((const char *)pgm_read_word(&musicData[MUS_WELLDONE]));

	while(1){


		if(i >= 180)
			s = (frame_cnt&64) ? 1:0;
		else{
			if(i < 80 || i >= 120)
				s = 2;
			else
				s = 3;
			i++;
		}
		ResetSprites();
		DrawMetaSprite((SCREEN_WIDTH/2)-8,96,(uint8_t *)pgm_read_word(&animSecretaryRest[s]));
		WaitVsync(1);

		if(f < 50)
			f++;
		else if((padState & BTN_START) && !(oldPadState & BTN_START))
			break;

		t++;
		w++;

		if(t >= 1280)
			t = 0;

		if(t == 1240)
			ZSTriggerFx(SFX_RINGTONE);
		
		if(t == 1270)
			i = 0;

		//if(w == 30 || w == 153)//do background water sound
		//	ZSTriggerFx(SFX_WAVES);

		frame_cnt++;
	}
	

	FadeOut(6,false);
	while(masterVolume > 5){
		masterVolume -= 5;
		WaitVsync(1);
	}
	while(DDRC);
	StopSong();
	SetMasterVolume(ZS_MASTER_VOL);	
	ClearVram();
	ResetSprites();
	ZSPrint(5,SCREEN_TILES_V/2,PSTR("THANK YOU FOR PLAYING!"));
	FadeIn(6,false);
	WaitVsync(240);
	FadeOut(5,true);
	FadeIn(3,false);
}




int main(){
	InitMusicPlayer(patches);
	GetPrngNumber(GetTrueRandomSeed());
	SetTileTable(gameTiles);
	SetSpritesTileBank(0,gameSprites);
	SetSpritesTileBank(1,gameTiles);
	SetUserPostVsyncCallback(&PostVsync);
	ClearVram();
	SetMasterVolume(ZS_MASTER_VOL);
	ChangeScreen();
	Intro();

	while(1){
		TitleScreen();

		level = 0;
		calls_missed_max = 3;

		while(1){
			calls_missed_level = calls_missed_max;

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
