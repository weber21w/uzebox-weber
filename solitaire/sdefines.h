const char *musicData[] PROGMEM={
Level6Song,
Level2Song,
Level3Song,
Level4Song,
Level5Song,
//Level1Song,
Level7Song,
};

#define SOLITAIRE_EEPROM_ID	88
#define SOLITAIRE_UZENET_ID			6502
#define TITLE_IDLE_TIME		60*4
#define NUM_MUSIC_TRACKS	sizeof(musicData)/2
#define MAX_CURSOR_SPEED 5
#define MAX_CARDS	52
#define NUM_STACKS	13+1


#define STACK_DECK	0
#define STACK_DRAW	1
#define STACK_HOME0	2
#define STACK_HOME1	3
#define STACK_HOME2	4
#define STACK_HOME3	5
#define STACK_BASE0	6
#define STACK_BASE1	7
#define STACK_BASE2	8
#define STACK_BASE3	9
#define STACK_BASE4	10
#define STACK_BASE5	11
#define STACK_BASE6	12
#define STACK_HELD	13

#define CARD_TILES_H	4
#define CARD_TILES_V	6
#define CARD_WIDTH		CARD_TILES_H*TILE_WIDTH
#define CARD_HEIGHT		CARD_TILES_V*TILE_HEIGHT
#define CURSOR_TILES_H	3
#define CURSOR_TILES_V	3
#define CURSOR_HOTSPOTX	3

//card attributes

#define FACEUP 		128

#define SPADE_ACE	0
#define SPADE_TWO	1
#define SPADE_THREE	2
#define SPADE_FOUR	3
#define SPADE_FIVE	4
#define SPADE_SIX	5
#define SPADE_SEVEN	6
#define SPADE_EIGHT	7
#define SPADE_NINE	8
#define SPADE_TEN	9
#define SPADE_JACK	10
#define SPADE_QUEEN	11
#define SPADE_KING	12

#define CLUB_ACE	13
#define CLUB_TWO	14
#define CLUB_THREE	15
#define CLUB_FOUR	16
#define CLUB_FIVE	17
#define CLUB_SIX	18
#define CLUB_SEVEN	19
#define CLUB_EIGHT	20
#define CLUB_NINE	21
#define CLUB_TEN	22
#define CLUB_JACK	23
#define CLUB_QUEEN	24
#define CLUB_KING	25

#define DIAMOND_ACE		26	
#define DIAMOND_TWO		27	
#define DIAMOND_THREE	28
#define DIAMOND_FOUR	29
#define DIAMOND_FIVE	30
#define DIAMOND_SIX		31
#define DIAMOND_SEVEN	32
#define DIAMOND_EIGHT	33
#define DIAMOND_NINE	34
#define DIAMOND_TEN		35
#define DIAMOND_JACK	36
#define DIAMOND_QUEEN	37
#define DIAMOND_KING	38

#define HEART_ACE		39
#define HEART_TWO		40
#define HEART_THREE		41
#define HEART_FOUR		42
#define HEART_FIVE		43
#define HEART_SIX		44
#define HEART_SEVEN		45
#define HEART_EIGHT		46
#define HEART_NINE		47
#define HEART_TEN		48
#define HEART_JACK		49
#define HEART_QUEEN		50
#define HEART_KING		51

#define SCORE_MULTIPLIER	5UL
#define SCORE_DRAW_TO_TABLE	1UL*SCORE_MULTIPLIER
#define SCORE_DRAW_TO_HOME 2UL*SCORE_MULTIPLIER//fun fact, shortcutting right from the draw to base is less points than first to the table then to home
#define SCORE_TABLE_TO_HOME 2UL*SCORE_MULTIPLIER
#define SCORE_TURN_OVER_CARD 1UL*SCORE_MULTIPLIER
#define SCORE_RECYCLE_DECK_PENALTY 20UL*SCORE_MULTIPLIER
#define SCORE_RECYCLE_HOME_PENALTY 3UL*SCORE_MULTIPLIER
//display types
#define HORIZONTAL	2//active pull pile
#define VERTICAL	4//normal 6 pilex
#define STACKED		8//draw pile and finish piles


#define CARD_PLACEHOLDER	(13*4)
#define CARD_FACEDOWN		(14*4)


#define DRAW_THREE		1
#define CARD_FLOATING	2
#define HAVE_MOVED		4
#define NO_SFX			8
#define NO_MUSIC		16
#define CARDS_DEALT		32
#define CHEAT_ACTIVE	64
#define SHOW_TIMER		128

typedef struct{
	uint8_t x,y;
	uint8_t numcards;
	uint8_t display;
	uint8_t cards[MAX_CARDS];

}Stack;

Stack stacks[NUM_STACKS];//last one is held stack
uint8_t buffer_ops_this_frame;//sadly it has come down to this to avoid cycle problems
uint8_t card_set,old_card_set;
uint8_t old_game_options;
uint8_t stack_held,stack_held_src,stack_held_off;
uint8_t table_restore[(CARD_TILES_H+0)*SCREEN_TILES_V];//CARD_TILES_H*SCREEN_TILES_V];//used to restore the tiles under a stack that is being moved(tile based, no sprites)
uint8_t table_restore_x;
uint16_t padstate,oldpadstate,mousestate,oldmousestate,temppadstate;
uint8_t cursorx,cursory,cursorspeed,oldcursorspeed,cursorframe;
uint8_t awaiting_key;

uint16_t round_seconds;
uint8_t round_ticks;
uint8_t game_state;
uint16_t game_score;
uint8_t music_track;
uint8_t demo_playing;
void UpdateCursor(uint8_t skiplogic);
extern bool ReadPowerSwitch();
void DrawStack(uint8_t s);
void HighScoreScreen();
void SetEEPromFlags();
extern bool snesMouseEnabled;
extern char fadeDir;
extern unsigned char fadeStep;
extern uint8_t playDevice,playPort;
extern int mx,my;
uint8_t last_click_stack,last_click_time, last_click_num;//for double clicking shortcut

void SetTileClipped(uint8_t x, uint8_t y, uint8_t t){
	if(x > SCREEN_TILES_H-1 || y > SCREEN_TILES_V-1)
		return;
	vram[x+(y*SCREEN_TILES_H)] = t+RAM_TILES_COUNT;
}

#define DOUBLE_CLICK_TIME	35


#define GFX_CARD_START 				8
#define GFX_RANK_SMALL_TOP_START	GFX_CARD_START+0
#define GFX_SUIT_SMALL_TOP_START	GFX_CARD_START+59
#define GFX_BLNK_SMALL_TOP_START	GFX_CARD_START+52
#define GFX_CRNR_SMALL_TOP_START	GFX_CARD_START+53

#define GFX_CRNR_SMALL_BOT_START	GFX_CARD_START+56
#define GFX_BLNK_SMALL_BOT_START	GFX_CARD_START+57
#define GFX_SUIT_SMALL_BOT_START	GFX_CARD_START+63
#define GFX_RANK_SMALL_BOT_START	GFX_CARD_START+26

#define GFX_BLNK_LEFT_SIDE_START	GFX_CARD_START+54
#define GFX_BLNK_RIGHT_SIDE_START	GFX_CARD_START+55

#define GFX_CARD_BLNK				GFX_CARD_START+58

#define GFX_SUIT_BIG_START			GFX_CARD_START+67

#define GFX_CARD_FACEDOWN_START		91
#define GFX_NUM_CARD_SETS			7

#define CURSOR_START_TILE	111
#define NUM_START_TILE		91+(9*GFX_NUM_CARD_SETS)
#define FONT_START_TILE		NUM_START_TILE+10
#define CHECK_MARK_TILE		FONT_START_TILE+26
#define COLON_TILE			CHECK_MARK_TILE+1

void SPrint(int x,int y,const char *string){

	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			if((c&127) == ' '){
				x++;
				continue;
			}
			c=(((c&127)-'A')+FONT_START_TILE);			
			SetTileClipped(x++,y,c);
		}else{
			break;
		}
	}
	
}

void SPrintRam(int x,int y,unsigned char *string){
	int i=0;
	char c;

	while(1){
		c=string[i++];		
		if(c!=0){
			if((c&127) == ' '){
				x++;
				continue;
			}		
			c=(((c&127)-'A')+FONT_START_TILE);			
			SetTileClipped(x++,y,c);
		}else{
			break;
		}
	}
}

void SPrintNum(int x,int y, uint16_t val,bool zeropad){
	unsigned char c,i;

	for(i=0;i<4;i++){
		c=val%10;
		if(val>0 || i==0){
			SetTile(x--,y,c+NUM_START_TILE);
		}else{
			if(zeropad){
				SetFont(x--,y,NUM_START_TILE);
			}else{
				//SetFont(x--,y,0);
			}
		}
		val=val/10;
	}
		
}

void SPrintBinary(int x, int y, uint16_t val){
	int i;
	
	for(i=0;i<16;i++){
		
		if(val&0b1000000000000000){
			SetTile(x++,y,NUM_START_TILE+1);
		}else{
			SetTile(x++,y,NUM_START_TILE);
		}
		val<<=1;
	}
}

void SPrintNumV(int x,int y, uint16_t val){
//	unsigned char c,i;
	
	if(val > 9999UL)
		val = 9999UL;

	if(val > 999)
		SetTile(x,y++,(val/1000)+NUM_START_TILE);
	if(val > 99)
		SetTile(x,y++,((val%1000)/100)+NUM_START_TILE);
	if(val > 9)
		SetTile(x,y++,((val%100)/10)+NUM_START_TILE);
//	if(val)
		SetTile(x,y,(val%10)+NUM_START_TILE);
		/*
	for(i=0;i<4;i++){
		c=val%10;
		if(val>0 || i==0){
			SetTile(x,y++,c+NUM_START_TILE);
		}else{
			if(zeropad){
				SetFont(x,y++,NUM_START_TILE);
			}else{
				//SetFont(x--,y,0);
			}
		}
		val=val/10;
	}
		*/
}



#define SWait200ns() asm volatile("lpm\n\tlpm\n\t");

void SolitaireSetMouseSensitivity(){
//TriggerFx(1,255,1);
	for(uint8_t i=0;i<4;i++){

			WaitUs(1000);

			for(i=0;i<31;i++){	
				JOYPAD_OUT_PORT|=_BV(JOYPAD_LATCH_PIN);	

				//pulse clock pin		
				JOYPAD_OUT_PORT&=~(_BV(JOYPAD_CLOCK_PIN));
				SWait200ns();
				SWait200ns();
				SWait200ns();			
				SWait200ns();			
				JOYPAD_OUT_PORT|=_BV(JOYPAD_CLOCK_PIN);

				JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));	
			
				WaitUs(2);
				SWait200ns();
				SWait200ns();
				SWait200ns();			
				SWait200ns();
			}	
			
		}
		WaitVsync(1);
}

void SolitaireInputDeviceHandler(){//requires the kernel to NOT read controllers during VSYNC
	//always assume a controller is attached, also assume there is always mouse data available on eithe rport
	uint16_t Buttons = 0;
	uint8_t i;

	//latch controllers
	JOYPAD_OUT_PORT |= _BV(JOYPAD_LATCH_PIN);
	WaitUs(1);//longer delay incase of mouse
	JOYPAD_OUT_PORT&=~(_BV(JOYPAD_LATCH_PIN));


	//read button states
	for(i=0;i<16;i++){

		Buttons >>= 1;
		WaitUs(5);

		//pulse clock pin		
		JOYPAD_OUT_PORT &= ~(_BV(JOYPAD_CLOCK_PIN));
		//SWait200ns();//necessary????!
		//SWait200ns();//necessary????!
		
		if(((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN)) == 0) || ((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN)) == 0))
			Buttons |= (1<<15);
		
		JOYPAD_OUT_PORT |= _BV(JOYPAD_CLOCK_PIN);
		WaitUs(5);
	}

	//joypad1_status_lo = Buttons;
	temppadstate = Buttons;
	Buttons = 0;

	if(temppadstate == (BTN_START+BTN_SELECT+BTN_Y+BTN_B))
		SoftReset();


	//	WaitUs(5);//necessary???
//Read extended mouse data on both ports(it's fine if there is no mouse there?)
	for(i=0;i<16;i++){
		Buttons <<= 1;
		//pulse clock pin		
		JOYPAD_OUT_PORT &= ~(_BV(JOYPAD_CLOCK_PIN));
		SWait200ns();
		SWait200ns();
	
		if(((JOYPAD_IN_PORT&(1<<JOYPAD_DATA1_PIN)) == 0) || ((JOYPAD_IN_PORT&(1<<JOYPAD_DATA2_PIN)) == 0))
			Buttons |= 1;

		JOYPAD_OUT_PORT |= _BV(JOYPAD_CLOCK_PIN);
		WaitUs(5);

	}

	mousestate = Buttons;
}


void HelpScreen();
void Shuffle();
uint8_t CanGoOnStack(uint8_t s);
void FloatCard(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy, uint8_t c, uint8_t skip_restore, uint8_t ticks_per_frame);
void UpdatePads();
void DrawCursor();
void SetInitialStacksState();
void DrawCard(uint8_t x, uint8_t y, uint8_t c);
uint8_t EEPromScore(uint8_t direction);
uint8_t CursorIntersects(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void MoveAllToDeck();

void HideSprites(){
	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;
}

void UpdateUzenet();

inline void SWaitVsync(uint8_t frames){
	UpdatePads();
//	if(TCNT1 < 1000UL)
		UpdateUzenet();
//	if(ReadPowerSwitch())
//		SoftReset();
	WaitVsync(frames);
}
extern uint8_t ram_tiles[];
uint8_t eeprom_data[30+50];

//b7 for the first byte of each entry indicates whether we have sent this to the high score server yet
//other bits are used to store game options, these are masked out for actual name purposes
const char EEPROMdefault[] PROGMEM = {
('N'|128),'I','C','K',('S'|128),'E','N',' ',1,182,
('K'|128),'I','V','A','N',' ',' ',' ',1,168,
('C'|128),'O',('S'|128),'A','M',' ',' ',' ',1,160,
};

const char EEPROMError[] PROGMEM = {
('E'|128),'E','P','R','O','M',' ',' ',0,0,
('E'|128),'R','R','O','R',' ',' ',' ',0,0,
(' '|128),' ',' ',' ',' ',' ',' ',' ',0,0,
};



const uint16_t KonamiCode[] PROGMEM = {BTN_UP,BTN_UP,BTN_DOWN,BTN_DOWN,BTN_LEFT,BTN_RIGHT,BTN_LEFT,BTN_RIGHT,BTN_B,BTN_A|BTN_Y};




