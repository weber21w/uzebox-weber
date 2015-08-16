#include <avr/pgmspace.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdint.h>


#include <defines.h>
#include <uzebox.h>
#include <petitfatfs/pff.h>


#include "data/sound/patches.inc"
#include "data/sound/music.inc"
#include "data/graphics/graphics.inc"
#include "sdefines.h"
#include "uzenet.h"
#include "ai.h"
#include "s_gui.h"


void GetEEPromFlags(){
	if(eeprom_data[2] & 128){game_state |= DRAW_THREE;}

	//load card preference
	if(eeprom_data[3] & 128){card_set |= 1;}
	if(eeprom_data[4] & 128){card_set |= 2;}
	if(eeprom_data[5] & 128){card_set |= 4;}
	if(eeprom_data[6] & 128){card_set |= 8;}
	if(card_set > 6)//corrupted eeprom?
		card_set = 0;
	old_card_set = card_set;

	//load music preference
	if(eeprom_data[11] & 128){music_track |= 1;}
	if(eeprom_data[12] & 128){music_track |= 2;}
	if(eeprom_data[13] & 128){music_track |= 4;}
	if(eeprom_data[14] & 128){music_track |= 8;}
	if(music_track >= NUM_MUSIC_TRACKS)//corrupted eeprom?
		music_track = 0;

	//load speed preference
	if(eeprom_data[21] & 128){cursorspeed |= 1;}
	if(eeprom_data[22] & 128){cursorspeed |= 2;}
	if(eeprom_data[23] & 128){cursorspeed |= 4;}
	if(eeprom_data[24] & 128){cursorspeed |= 8;}
	if(cursorspeed > MAX_CURSOR_SPEED)//corrupted eeprom?
		cursorspeed = 2;

	if(eeprom_data[15] & 128){game_state |= NO_MUSIC;}
	if(eeprom_data[7] & 128){game_state |= SHOW_TIMER;}

	if(!cursorspeed)
		cursorspeed = 2;
	oldcursorspeed = cursorspeed;
	old_game_options = game_state;
	old_card_set = card_set;
}

void SetEEPromFlags(){
	if(game_state & DRAW_THREE){eeprom_data[2] |= 128;}else{eeprom_data[2] &= ~128;}
	
	//store card preference
	if(card_set & 1){eeprom_data[3] |= 128;}else{eeprom_data[3] &= ~128;}
	if(card_set & 2){eeprom_data[4] |= 128;}else{eeprom_data[4] &= ~128;}
	if(card_set & 4){eeprom_data[5] |= 128;}else{eeprom_data[5] &= ~128;}
	if(card_set & 8){eeprom_data[6] |= 128;}else{eeprom_data[6] &= ~128;}
	
	//store music preference
	if(music_track & 1){eeprom_data[11] |= 128;}else{eeprom_data[11] &= ~128;}
	if(music_track & 2){eeprom_data[12] |= 128;}else{eeprom_data[12] &= ~128;}
	if(music_track & 4){eeprom_data[13] |= 128;}else{eeprom_data[13] &= ~128;}
	if(music_track & 8){eeprom_data[14] |= 128;}else{eeprom_data[14] &= ~128;}

	//store speed preference
	if(cursorspeed & 1){eeprom_data[21] |= 128;}else{eeprom_data[21] &= ~128;}
	if(cursorspeed & 2){eeprom_data[22] |= 128;}else{eeprom_data[22] &= ~128;}
	if(cursorspeed & 4){eeprom_data[23] |= 128;}else{eeprom_data[23] &= ~128;}
	if(cursorspeed & 8){eeprom_data[24] |= 128;}else{eeprom_data[24] &= ~128;}
	
	if(game_state & NO_MUSIC){eeprom_data[15] |= 128;}else{eeprom_data[15] &= ~128;}
	if(game_state & SHOW_TIMER){eeprom_data[7] |= 128;}else{eeprom_data[7] &= ~128;}
}


uint8_t EEPromScore(uint8_t direction){//whole entry is stored in ram, either fill that ram or update eeprom with it

	struct EepromBlockStruct ebs;
	ebs.id = SOLITAIRE_EEPROM_ID;

	if(EepromReadBlock(ebs.id, &ebs)){//doesn't exist, try to make it
		for(uint8_t i=0;i<30;i++)
				ebs.data[i] = pgm_read_byte(&EEPROMdefault[i]);

		EepromWriteBlock(&ebs);
	}

	if(EepromReadBlock(ebs.id, &ebs) == 0){//it exists
		if(!direction){//read
			for(uint8_t i=0;i<30;i++)
				eeprom_data[i] = ebs.data[i];
		}else{//write
			for(uint8_t i=0;i<30;i++)
				ebs.data[i] = eeprom_data[i];

			EepromWriteBlock(&ebs);
		}
	}else if(!direction){//we can't access the block, it's full or there is a problem with the eeprom
			for(uint8_t i=0;i<30;i++)
				eeprom_data[i] = pgm_read_byte(&EEPROMError[i]);
			return 1;
	}
	return 0;
}

inline void DrawCardTop(uint8_t x, uint8_t y, uint8_t c){
	uint8_t suitoff = (c&63)/13;
	uint8_t rankoff = 0;
	uint8_t cardoff = (c&63)%13;
	if((c & 63) > CLUB_KING)
		rankoff = 13;
//		suitoff = c/13;
//		rankoff = 13;
//	}
	if(c & FACEUP){
		c &= 63;
		//top
		SetTileClipped(x+0,y+0,GFX_RANK_SMALL_TOP_START+rankoff+cardoff);
		SetTileClipped(x+1,y+0,GFX_SUIT_SMALL_TOP_START+suitoff);
		SetTileClipped(x+2,y+0,GFX_BLNK_SMALL_TOP_START+0);
		SetTileClipped(x+3,y+0,GFX_CRNR_SMALL_TOP_START+0);
	}else{//face down
		SetTileClipped(x+0,y+0,GFX_CARD_FACEDOWN_START+0+(card_set*9));
		SetTileClipped(x+1,y+0,GFX_CARD_FACEDOWN_START+1+(card_set*9));
		SetTileClipped(x+2,y+0,GFX_CARD_FACEDOWN_START+1+(card_set*9));
		SetTileClipped(x+3,y+0,GFX_CARD_FACEDOWN_START+2+(card_set*9));
	}
}

inline void DrawCardLeftSlice(uint8_t x, uint8_t y, uint8_t c){
	uint8_t rankoff = 0;
	uint8_t cardoff = (c&63)%13;
	if((c & 63) > CLUB_KING)
		rankoff = 13;
	SetTileClipped(x+0,y+0,GFX_RANK_SMALL_TOP_START+rankoff+cardoff);
	SetTileClipped(x+0,y+1,GFX_BLNK_LEFT_SIDE_START);
	SetTileClipped(x+0,y+2,GFX_BLNK_LEFT_SIDE_START);
	SetTileClipped(x+0,y+3,GFX_BLNK_LEFT_SIDE_START);
	SetTileClipped(x+0,y+4,GFX_BLNK_LEFT_SIDE_START);
	SetTileClipped(x+0,y+5,GFX_CRNR_SMALL_BOT_START+0);
}

inline void AddCard(uint8_t s, uint8_t c){
	stacks[s].cards[stacks[s].numcards++] = c;
}

inline uint8_t RemoveCard(uint8_t s, uint8_t c){
	stacks[s].numcards--;
	return stacks[s].cards[c];
}

inline void DrawCard(uint8_t x, uint8_t y, uint8_t c){
	uint8_t suitoff = (c&63)/13;
	uint8_t rankoff = 0;
	uint8_t cardoff = (c&63)%13;
	if((c & 63) > CLUB_KING)
		rankoff = 13;
//		suitoff = c/13;
//		rankoff = 13;
//	}

	if(c == MAX_CARDS){//blank it out with the holder card
		SetTileClipped(x+0,y+0,1);
		SetTileClipped(x+1,y+0,2);
		SetTileClipped(x+2,y+0,2);
		SetTileClipped(x+3,y+0,3);

		for(uint8_t yo=1;yo<CARD_TILES_V;yo++)
			for(uint8_t xo=0;xo<CARD_TILES_H;xo++){
				if(xo == 0)
					SetTileClipped(x+xo,y+yo,4);//TODO MAKE THIS SMALLER
				else if(xo == CARD_TILES_H-1)
					SetTileClipped(x+xo,y+yo,5);
				else
					SetTileClipped(x+xo,y+yo,0);
			}
		
		SetTileClipped(x+0,y+5,6);
		SetTileClipped(x+1,y+5,2);
		SetTileClipped(x+2,y+5,2);
		SetTileClipped(x+3,y+5,7);

		return;
	}

	if(c & FACEUP){
		c &= 63;
		//top
		SetTileClipped(x+0,y+0,GFX_RANK_SMALL_TOP_START+rankoff+cardoff);
		SetTileClipped(x+1,y+0,GFX_SUIT_SMALL_TOP_START+suitoff);
		SetTileClipped(x+2,y+0,GFX_BLNK_SMALL_TOP_START+0);
		SetTileClipped(x+3,y+0,GFX_CRNR_SMALL_TOP_START+0);

		//bottom
		SetTileClipped(x+0,y+5,GFX_CRNR_SMALL_BOT_START+0);
		SetTileClipped(x+1,y+5,GFX_BLNK_SMALL_BOT_START+0);
		SetTileClipped(x+2,y+5,GFX_SUIT_SMALL_BOT_START+suitoff);
		SetTileClipped(x+3,y+5,GFX_RANK_SMALL_BOT_START+rankoff+cardoff);
	
		for(uint8_t i=1;i<5;i++){//sides
			SetTileClipped(x+0,y+i,GFX_BLNK_LEFT_SIDE_START);
			SetTileClipped(x+3,y+i,GFX_BLNK_RIGHT_SIDE_START);
		}

		if(false){//royal card

		}else{//normal
			suitoff = (c/13)*4;
			SetTileClipped(x+1,y+1,GFX_CARD_BLNK);
			SetTileClipped(x+2,y+1,GFX_CARD_BLNK);
			SetTileClipped(x+1,y+4,GFX_CARD_BLNK);
			SetTileClipped(x+2,y+4,GFX_CARD_BLNK);

			SetTileClipped(x+1,y+2,GFX_SUIT_BIG_START+0+suitoff);
			SetTileClipped(x+2,y+2,GFX_SUIT_BIG_START+1+suitoff);
			SetTileClipped(x+1,y+3,GFX_SUIT_BIG_START+2+suitoff);
			SetTileClipped(x+2,y+3,GFX_SUIT_BIG_START+3+suitoff);
		}
	}else{//facedown
		uint8_t fdt = (card_set*9);
		SetTileClipped(x+0,y+0,GFX_CARD_FACEDOWN_START+0+fdt);
		SetTileClipped(x+1,y+0,GFX_CARD_FACEDOWN_START+1+fdt);	
		SetTileClipped(x+2,y+0,GFX_CARD_FACEDOWN_START+1+fdt);	
		SetTileClipped(x+3,y+0,GFX_CARD_FACEDOWN_START+2+fdt);	

		for(uint8_t i=1;i<5;i++){
			SetTileClipped(x+0,y+i,GFX_CARD_FACEDOWN_START+3+fdt);	
			SetTileClipped(x+1,y+i,GFX_CARD_FACEDOWN_START+4+fdt);	
			SetTileClipped(x+2,y+i,GFX_CARD_FACEDOWN_START+4+fdt);	
			SetTileClipped(x+3,y+i,GFX_CARD_FACEDOWN_START+5+fdt);	
		}

		SetTileClipped(x+0,y+5,GFX_CARD_FACEDOWN_START+6+fdt);
		SetTileClipped(x+1,y+5,GFX_CARD_FACEDOWN_START+7+fdt);	
		SetTileClipped(x+2,y+5,GFX_CARD_FACEDOWN_START+7+fdt);	
		SetTileClipped(x+3,y+5,GFX_CARD_FACEDOWN_START+8+fdt);

	}
}

inline void DrawStack(uint8_t s){
	if(!stacks[s].numcards){//no cards, draw a placeholder card
		DrawCard(stacks[s].x,stacks[s].y,MAX_CARDS);
		return;
	}
//stacks[s].display = VERTICAL;
		if(stacks[s].display & VERTICAL){//1 of 7 stacks
			for(uint8_t i=0;i<stacks[s].numcards;i++){
				if(i == stacks[s].numcards-1){//last card, draw it fully
					DrawCard(stacks[s].x,stacks[s].y+i,stacks[s].cards[i]);
				}else{//a card that will have another card on top, just draw the top part
					DrawCardTop(stacks[s].x,stacks[s].y+i,stacks[s].cards[i]);
				}
			}
		}else if(stacks[s].display & HORIZONTAL){//the horizontal draw stack next to deck the player picks a card from
			uint8_t xo = 0;

			DrawCard(stacks[s].x,stacks[s].y,CARD_PLACEHOLDER);

			if(game_state & DRAW_THREE){//draw 3 cards at a time
				for(uint8_t i=0;i<CARD_TILES_V;i++){
					SetTile(stacks[s].x+CARD_TILES_H+0,stacks[s].y+i,0);//blank out right edges
					SetTile(stacks[s].x+CARD_TILES_H+1,stacks[s].y+i,0);
				}
				if(stacks[s].numcards > 1)
					xo++;	
				if(stacks[s].numcards > 2)
					xo++;

				DrawCard(stacks[s].x+xo,stacks[s].y,stacks[s].cards[stacks[s].numcards-1]);
				for(uint8_t i=0;i<2;i++){
					if(xo){
						DrawCardLeftSlice(stacks[s].x+(xo-1),stacks[s].y,stacks[s].cards[stacks[s].numcards-(2+i)]);
						xo--;
					}
				}
			}else{//single card draw
				DrawCard(stacks[s].x,stacks[s].y,stacks[s].cards[stacks[s].numcards-1]);
			}
		}else{//a stack that is totally vertical. only the top card is shown, an no partial cards
			DrawCard(stacks[s].x,stacks[s].y,stacks[s].cards[stacks[s].numcards-1]);
		}
}

void DrawCursor(){
//return;
	uint8_t soff = 0;
	for(uint8_t i=0;i<9;i++)
		sprites[i].x = SCREEN_TILES_H*TILE_WIDTH;

	if(cursorx == SCREEN_TILES_H*TILE_WIDTH)
		return;
	for(uint8_t y=0;y<3;y++){
		if(cursory+(y*8) < (SCREEN_TILES_V*TILE_HEIGHT)-1){
		for(uint8_t x=0;x<3;x++){
			sprites[soff].x = cursorx+(x*8);
			sprites[soff].y = cursory+(y*8);
			sprites[soff].tileIndex = (cursorframe*9)+soff;
			soff++;
		}
		}
	}
}



void TransferStack(uint8_t src_stack, uint8_t src_start, uint8_t dst_stack, uint8_t dst_start){
	uint8_t transferlimit = stacks[src_stack].numcards;
	for(uint8_t i=src_start;i<transferlimit;i++){
		stacks[dst_stack].cards[dst_start++] = stacks[src_stack].cards[src_start++];
		stacks[src_stack].numcards--;
		stacks[dst_stack].numcards++;
	}

}



void Shuffle(uint16_t reps){//swaps 2 random cards in the deck per iteration
	for(uint16_t i=0;i<reps;i++){
		uint8_t ts = GetPrngNumber(0)%stacks[0].numcards;
		uint8_t td = GetPrngNumber(0)%stacks[0].numcards;
		uint8_t tt = stacks[0].cards[ts];

		stacks[0].cards[ts] = stacks[0].cards[td];
		stacks[0].cards[td] = tt;
	}
}

uint8_t CursorIntersects(uint8_t x, uint8_t y, uint8_t w, uint8_t h){
	if(cursorx+10 < x || cursorx+6 > x+w || cursory+6 < y || cursory > y+h)
		return 0;
	return 1;
}

inline void SaveBuffer(uint8_t sx){
//running out of cyles and its bordering on the edge with this function
//	buffer_ops_this_frame++;
	uint8_t troff = 0;
	uint16_t voff;
	table_restore_x = sx;
	for(uint8_t y=0;y<SCREEN_TILES_V;y++){//
		voff = table_restore_x+(y*SCREEN_TILES_H);
		for(uint8_t x=0;x<CARD_TILES_H;x++)//for(uint8_t x=0;(x<CARD_TILES_H+0)&&(table_restore_x+x<SCREEN_TILES_H);x++)
			table_restore[troff++] = vram[voff++];//table_restore[troff++] = vram[table_restore_x+x+(y*SCREEN_TILES_H)];
	}
}

inline void RestoreBuffer(){
//running out of cyles and its bordering on the edge with this function
	uint8_t troff = 0;
	uint16_t voff;
	for(uint8_t y=0;y<SCREEN_TILES_V;y++){
		voff = table_restore_x+(y*SCREEN_TILES_H);
		for(uint8_t x=0;x<CARD_TILES_H;x++)//for(uint8_t x=0;(x<CARD_TILES_H+0)&&(table_restore_x+x<SCREEN_TILES_H);x++)
			vram[voff++] = table_restore[troff++];//vram[table_restore_x+x+(y*SCREEN_TILES_H)] = table_restore[troff++];
	}
}

 void FloatCard(uint8_t sx, uint8_t sy, uint8_t dx, uint8_t dy, uint8_t c, uint8_t skip_restore, uint8_t ticks_per_frame){
	if(!skip_restore)
		SaveBuffer(sx);
	game_state |= CARD_FLOATING;
	bool flipflop = true;
	uint8_t did_update_cursor = 0;


//ticks_per_frame = 0;
	uint8_t ticks = ticks_per_frame;
	while(sx != dx || sy != dy){
		DrawCard(sx,sy,c);
//		UpdatePad();//done in wait vsync routine
		if(!did_update_cursor){
			UpdateCursor(1);
			DrawCursor();
			did_update_cursor = 1;
		}
		if(ticks_per_frame)
			ticks_per_frame--;
		else{
			ticks_per_frame = ticks;
			SWaitVsync(1);
//SWaitVsync(100);
			did_update_cursor = 0;
		}
//		SWaitVsync(1);//////////////////////////////////////////////////
		flipflop = !flipflop;
		for(uint8_t i=0;i<2;i++){
			if(flipflop)
				i++;
			if(sx < dx)
				sx++;
			else if(sx > dx)
				sx--;
			if(sy < dy)
				sy++;
			else if(sy > dy)
				sy--;
		}
		if(!skip_restore){
			RestoreBuffer();
			SaveBuffer(sx);
		}
	}
	game_state ^= CARD_FLOATING;
}


uint8_t GrabStack(bool selectempty){
	uint8_t grabbed_stack=255, grabbed_offset=0;
	bool grabbed = false;;
	for(uint8_t i=STACK_DECK;i<=STACK_BASE6;i++){//check cursor position against all stacks(except deck which we cant grab and held stack which is right now NULL)
		uint8_t stack_width, stack_height;
		if(!selectempty && !stacks[i].numcards)//we can't grab a stack with no cards, unless overriden
			continue;
		if(stacks[i].display & HORIZONTAL){//we are pulling from the draw stack next to the deck. we can only grab the right most card
			stack_width = CARD_WIDTH+(stacks[i].numcards>1?1:0)+(stacks[i].numcards>2?1:0);
			stack_height = CARD_HEIGHT;

			if(CursorIntersects(stacks[i].x<<3,stacks[i].y<<3,stack_width,stack_height)){
				grabbed = true;
				grabbed_stack = i;
				grabbed_offset = stacks[i].numcards-1;
				break;
			}

		}else if(stacks[i].display & STACKED){//we are pulling from 1 of the 4 home stacks
			stack_width = CARD_WIDTH;
			stack_height = CARD_HEIGHT;

			if(CursorIntersects(stacks[i].x<<3,stacks[i].y<<3,stack_width,stack_height)){
				grabbed = true;
				grabbed_stack = i;
				grabbed_offset = stacks[i].numcards-1;
				break;
			}
		}else{//we are pulling from one of the 7 foundation stacks
			stack_width = CARD_WIDTH;
			stack_height = ((stacks[i].numcards-2)<<3)+CARD_HEIGHT;

			if(CursorIntersects(stacks[i].x<<3,stacks[i].y<<3,stack_width,stack_height)){
				grabbed = true;
				grabbed_stack = i;
				if(cursorx>>3 < stacks[i].y+(stacks[i].numcards-2)){//we grabbed on one of the card only showing the top piece
					grabbed_offset = (cursorx>>3)-stacks[i].y;
				}else{//we grabbed the last card in the stack that is drawn fully
					grabbed_offset = stacks[i].numcards-1;
				}
				break;
			}
		}
	}

//SPrintNum(5,SCREEN_TILES_V-1,grabbed_stack,1);
	if(!grabbed)
		return 255;

	return grabbed_stack;
}


uint8_t CanGoOnStack(uint8_t s){//the source is implied to be STACK_HELD
//SPrintNum(24,23,s,1);
//SPrintNum(24,24,stacks[STACK_HELD].cards[0],1);
	
	if(s == STACK_DECK || s == STACK_DRAW)//can't place it on the deck or draw pile...
		return 0;

	uint8_t cs,cd;
	cs = stacks[STACK_HELD].cards[0] & 63;//eliminate FACEUP
	cd = stacks[s].cards[stacks[s].numcards-1] & 63;

	if(s >= STACK_HOME0 && s <= STACK_HOME3){//one of the home stacks, must be in the right order
		if(stacks[STACK_HELD].numcards > 1)//only 1 card at a time can be transferred
			return 0;
		if(!stacks[s].numcards){//nothing here yet, must be an Ace
			if(cs == 0 || cs == 13 || cs == 26 || cs == 39)
				return 1;
			return 0;
		}
		if(	cs < cd ||//card must be greater
			cs > cd+1 ||//but only by 1
			cd % 13 == 12)//can't stack on the last card in suit, the King
			return 0;
		return 1;
	}
	//else it is one of the base stacks
	if(!stacks[s].numcards){//empty stack, first card must be a king
		if(cs == 12 || cs == 25 || cs == 38 || cs == 51)
			return 1;//it's a king
		return 0;
	}
	if((cs / 26) == (cd / 26)){//can't be same color
	//SPrint(15,2,PSTR("WRONG COLOR"));
		return 0;}
	if((cs % 13) != ((cd % 13)-1)){//must stack a lower card here
		//SPrint(15,2,PSTR("WRONG NUMBR"));
		return 0;}

	return 1;//if none of that is wrong, we can stack it!
}

void HandleDeckClick(uint8_t gt){
	if(!stacks[STACK_DECK].numcards && stacks[STACK_DRAW].numcards > 1){//blank out the draw pile, put the deck back
		DrawCard(stacks[STACK_DRAW].x,stacks[STACK_DRAW].y,MAX_CARDS);
		
		for(uint8_t ty=stacks[STACK_DRAW].y;ty<stacks[STACK_DRAW].y+CARD_TILES_V;ty++)
			for(uint8_t tx=stacks[STACK_DRAW].x+CARD_TILES_H;tx<stacks[STACK_DRAW].x+(CARD_TILES_H*2)-2;tx++)
				SetTileClipped(tx,ty,0);
		
		if(!(game_state & CHEAT_ACTIVE))
			for(uint8_t co=0;co<stacks[STACK_DRAW].numcards;co++)
				stacks[STACK_DRAW].cards[co] ^= FACEUP;
		
		TransferStack(STACK_DRAW,0,STACK_DECK,0);
		DrawStack(STACK_DECK);
		if(game_score > SCORE_RECYCLE_DECK_PENALTY)
			game_score -= SCORE_RECYCLE_DECK_PENALTY;
		else
			game_score = 0;
	}else if(stacks[STACK_DECK].numcards){//put a new card on the draw pile
		for(uint8_t i=0;i<1+((game_state & DRAW_THREE)?2:0);i++){
			gt = RemoveCard(STACK_DECK,stacks[STACK_DECK].numcards-1);
			gt |= FACEUP;
			if(!stacks[STACK_DECK].numcards){
				if(!i)
					DrawCard(stacks[STACK_DECK].x,stacks[STACK_DECK].y,MAX_CARDS);//blank over it
				i = 200;
			}
			FloatCard(stacks[STACK_DECK].x,stacks[STACK_DECK].y,stacks[STACK_DECK].x,stacks[STACK_DECK].y,gt,0,0);
			AddCard(STACK_DRAW,gt);
			DrawStack(STACK_DECK);
			DrawStack(STACK_DRAW);
		}
	}
}

void UpdateCursor(uint8_t skiplogic){

	oldcursorspeed = cursorspeed;
	if(ai_state)
		cursorspeed = 2;
	if(padstate & BTN_SR)
		cursorspeed <<= 1;
	if(padstate & BTN_SL && cursorspeed > 1)
		cursorspeed >>= 1;

	if(padstate & BTN_LEFT){
		if(cursorx > cursorspeed)
			cursorx -= cursorspeed;
		else
			cursorx = 0;
	}else if(padstate & BTN_RIGHT){
		if(cursorx < ((((SCREEN_TILES_H*TILE_WIDTH)-1)-(CURSOR_TILES_H*8))-cursorspeed)+3)
			cursorx += cursorspeed;
		else
			cursorx = (((SCREEN_TILES_H*TILE_WIDTH)-1)-(CURSOR_TILES_H*8))+3;
	}

	if(padstate & BTN_UP){
		if(cursory > cursorspeed)
			cursory -= cursorspeed;
		else
			cursory = 0;
	}else if(padstate & BTN_DOWN){
		if(cursory < ((((SCREEN_TILES_V*TILE_HEIGHT)-1)+18)-(CURSOR_TILES_H*8))-cursorspeed)
			cursory += cursorspeed;
		else
			cursory = ((((SCREEN_TILES_V*TILE_HEIGHT)-1)+18)-(CURSOR_TILES_H*8));
	}

	cursorspeed = oldcursorspeed;
	cursorframe = (padstate & BTN_B)?1:0;
	if(skiplogic){
		return;
	}
	uint8_t gt = 0;

	if(padstate & BTN_B/* && !(oldpadstate & BTN_B)*/){
		if(!(game_state & CARD_FLOATING) && !(oldpadstate & BTN_B)){//can't grab something while a card is floating
			gt = GrabStack(true);
			if(gt != 255){//we grabbed something, see what it is
					if(true){//!(oldpadstate & BTN_B)){//check for double click
						last_click_time = 0;
						if(last_click_stack == gt){
							last_click_num++;
						}else{
							last_click_num = 0;
							last_click_stack = gt;
						}
					}
					game_state |= HAVE_MOVED;
					if(gt == STACK_DECK){//we hit the deck, if there is a card then move it to the draw pile. otherwise bring the draw pile back to the deck
						HandleDeckClick(gt);
					}else if(!stacks[gt].numcards){//we selected an empty stack that wasn't the deck
					
					}else if(gt == STACK_DRAW){//we grabbed a card from the draw pile
//STriggerFx(1,100,true);
						TransferStack(STACK_DRAW,stacks[STACK_DRAW].numcards-1,STACK_HELD,0);
					//	stacks[STACK_HELD].numcards = 1
						DrawStack(STACK_DRAW);//redraw the card underneath the one we took
						SaveBuffer(stacks[stack_held_src].x);//TODO IS THIS NECESSARY??
//ai_skip_tick = 1;
//SWaitVsync(1);
						stack_held_src = STACK_DRAW;
						stack_held = 1;
					}else if(stacks[gt].display == STACKED){//we grabbed a card from a home pile
						TransferStack(gt,stacks[gt].numcards-1,STACK_HELD,0);
						DrawStack(gt);//redraw the card underneath the one we took
						SaveBuffer(stacks[stack_held_src].x);//TODO IS THIS NECESSARY???
//ai_skip_tick = 1;
//SWaitVsync(1);
						stack_held_src = gt;
						stack_held = 1;
					}else{//we grabbed a card from one of the base stacks
						uint8_t poff = (cursory>>3)-stacks[gt].y;
						if(poff >= stacks[gt].numcards){
							poff = stacks[gt].numcards-1;
						//	stacks[gt].cards[poff] |= FACEUP;
						}
						if(stacks[gt].numcards && !(stacks[gt].cards[stacks[gt].numcards-1] & FACEUP)){//we flipped a card up
							stacks[gt].cards[stacks[gt].numcards-1] |= FACEUP;
							game_score += SCORE_TURN_OVER_CARD;
							DrawStack(gt);
						}
						if(stacks[gt].cards[poff] & FACEUP){//////////////////////////////////////
							DrawStack(gt);//TODO IS THIS NECESSARY?????IF CARD WASN'T FACE UP, THEN WE SELECT NEW STACK??
//SaveBuffer(stacks[stack_held_src].x);//DID THIS FIX IT????
							TransferStack(gt,poff,STACK_HELD,0);
							stacks[STACK_HELD].cards[stacks[STACK_HELD].numcards-1] |= FACEUP;//at least the last card should always be face up if we click it
							stack_held_src = gt;
						//	stack_held_off = 0;/////
							for(uint8_t yo=stacks[gt].y;yo<SCREEN_TILES_H;yo++)
								for(uint8_t xo=stacks[gt].x;xo<stacks[gt].x+CARD_TILES_H;xo++)
									SetTileClipped(xo,yo,0);
							DrawStack(stack_held_src);//draw over the cards we took
SaveBuffer(stacks[stack_held_src].x);
//ai_skip_tick = 1;
//SWaitVsync(1);
							stack_held = true;
						}else{//wasn't face up

						}
					}
			}else{//we grabbed something invalid

			}
		}else if(!(game_state & CARD_FLOATING)){//holding card, check if user wants to use a shortcut
				if(((padstate & BTN_A) && !(oldpadstate & BTN_A)) || (last_click_num > 0 && last_click_stack != 255)){
					if(last_click_num){
						last_click_num = 0;
						last_click_time = DOUBLE_CLICK_TIME+1;
						last_click_stack = 255;
					}

					if(stack_held){
						if(stacks[STACK_HELD].numcards == 1 && (stack_held_src < STACK_HOME0 || stack_held_src > STACK_HOME3)){//we might be able to put this on a home stack(user should know he would make more points manually putting it to the table, then to home...)
							for(uint8_t i=STACK_HOME0;i<=STACK_HOME3;i++){
								if(stack_held_src != i && CanGoOnStack(i)){//shortcutting to same stack is useless and would complicate restore code, simply drop it back
									RestoreBuffer();
									TransferStack(STACK_HELD,0,i,stacks[i].numcards);
									DrawStack(i);
//SaveBuffer(stacks[i].x);//HACK TO PREVENT CORRUPTION???
//^REMOVED THIS, ADDED BREAK FOR THIS FOR LOOP TO FIX SCORING ISSUE, EVERYTHING SHOULD BE RIGHT NOW...ANYTHING BROKEN????

									stack_held = 0;
									gt = 255;
									if(stack_held_src == STACK_DRAW){//draw stack to home stack, add score
										game_score += SCORE_DRAW_TO_HOME;
									}else if(stack_held_src >= STACK_BASE0){
										game_score += SCORE_TABLE_TO_HOME;
									}else{//no score for table to table moves

									}
									break;
								}//if(stack_held_src != i && CanGoOnStack(i))
							}//for(uint8_t i=STACK_HOME0;i<=STACK_HOME3;i++)
						}//if(stacks[STACK_HELD].numcards == 1 && (stack_held_src < STACK_HOME0 || stack_held_src > STACK_HOME3))
						
						if(stack_held){//didn't go to a home stack, check the base stacks
							for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++){
								if(stack_held_src != i && CanGoOnStack(i)){//never shortcut the held stack back to its source, can corrupt screen due to how restore buffer works
//SPrint(15,2,PSTR("GOTO BASE"));
									RestoreBuffer();
									TransferStack(STACK_HELD,0,i,stacks[i].numcards);
									DrawStack(i);
SaveBuffer(stacks[i].x);//HACK TO PREVENT CORRUPTION???
//ai_skip_tick = 1;
//SWaitVsync(1);
									stack_held = 0;
									gt = 255;
									if(stack_held_src == STACK_DRAW)
										game_score += SCORE_DRAW_TO_TABLE;
									else if(stack_held_src >= STACK_HOME0 && stack_held_src <= STACK_HOME3){//recycling a card from home stack, add penalty
										if(game_score < SCORE_RECYCLE_HOME_PENALTY)
											game_score = 0;
										else
											game_score -= SCORE_RECYCLE_HOME_PENALTY;
									}		
								}
							}//for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++)
						}//if(stack_held)
					}//if(stack_held)
				}//if(((padstate & BTN_A) && !(oldpadstate & BTN_A)) || (last_click_num > 0 && last_click_stack != 255))
		}//(CARD_FLOATING)
	}else if(gt != 255){// !BTN_B, dropping stack///////////////////////////////////
		if(false){//padstate & BTN_A && !(oldpadstate & BTN_A) /*&& !(padstate & BTN_B)*/ && !(oldpadstate & BTN_B)){//trying to send a card to the home stack
		//	uint8_t hst = GrabStack(false);
		//	if(hst != 255){
		//		for(uint8_t hs=STACK_HOME0;hs<=STACK_HOME3;hs++){
		//			if(stacks[hs].cards[stacks[hs].numcards-1] == stacks[hst].cards[stacks[hst].numcards-1]-1){//we can put it on top
		//				RemoveCard(hst,stacks[hst].numcards-1);//remove the card from the base stack
						//..TODO
		//			}
		//		}
		//	}
		}else{
			stack_held = false;
			//cursorframe = 0;
			if((oldpadstate & BTN_B) && stacks[STACK_HELD].numcards){//we just let go and we were holding onto something so try to put it down
				gt = GrabStack(true);
				if(gt != 255){//see if we dropped it somewhere legitimate
					if(CanGoOnStack(gt)){
						RestoreBuffer();
						TransferStack(STACK_HELD,0,gt,stacks[gt].numcards);
						DrawStack(gt);
						if(gt >= STACK_HOME0 && gt <= STACK_HOME3){//check what score we should give
							//destination is a home stack
							if(stack_held_src == STACK_DRAW)//draw stack straight to a home stack
								game_score += SCORE_DRAW_TO_HOME;
							else if(stack_held_src >= STACK_HOME0 && stack_held_src <= STACK_HOME3){//HOME TO HOME NO SCORE
								
							}else//it's coming from one of the base stacks
								game_score += SCORE_TABLE_TO_HOME;
						}else{//destination is one of the table base stacks
							if(stack_held_src >= STACK_HOME0 && stack_held_src <= STACK_HOME3){//coming from one of the home stacks, score penalty
								if(game_score < SCORE_RECYCLE_HOME_PENALTY)
									game_score = 0;
								else
									game_score -= SCORE_RECYCLE_HOME_PENALTY;
							}else if(stack_held_src == STACK_DRAW){//from the draw stack to a table base stack, add score
								game_score += SCORE_DRAW_TO_TABLE;
							}else{//table to table, no score

							}
						}
					}else
						gt = 255;
				}

				if(gt == 255){//invalid destination, put it back where it came from
					RestoreBuffer();
					TransferStack(STACK_HELD,0,stack_held_src,stacks[stack_held_src].numcards);
					DrawStack(stack_held_src);
				}
			}
		}	
	}
	
	
	if(stack_held){//restore tiles at old position, save tiles at new position, make stack follow cursor
//TODO THIS IS WHAT IS KILLING THE CYCLES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		RestoreBuffer();
		stacks[STACK_HELD].x = cursorx>>3;
		stacks[STACK_HELD].y = cursory>>3;
		SaveBuffer(stacks[STACK_HELD].x);
		DrawStack(STACK_HELD);
	}else if(!(game_state & CARD_FLOATING)){//check for 1 button draw card shortcut
		if(padstate & BTN_Y && !(oldpadstate & BTN_Y)){
			HandleDeckClick(gt);
		}
	}
}

void DrawStacks(){
	for(uint8_t i=0;i<NUM_STACKS-1;i++)
		DrawStack(i);

}

void DealHand(){
//prng_state = 0x34aa;
	round_seconds = round_ticks = game_score = 0;
	DrawCursor();
	uint8_t ct;
	uint8_t xoff = 5;
	if(game_state & CHEAT_ACTIVE){
		for(uint8_t i=0;i<52;i++)
			stacks[STACK_DECK].cards[i] |= FACEUP;
	}

	for(uint8_t i=7;i<NUM_STACKS-1;i++){
		for(uint8_t j=0;j<i-5;j++){
			ct = RemoveCard(STACK_DECK,stacks[STACK_DECK].numcards-1);
			DrawStack(STACK_DECK);
			if(j == i-6)
				ct |= FACEUP;
			FloatCard(1,0,xoff,6+stacks[i].numcards,ct,0,1);
			AddCard(i,ct);
			DrawStack(i);
			if(!ai_state && demo_playing){//ai demo started, but was interrupted by the user. let the main loop handle this
				return;
			}
		}
			xoff += 4;
	}
	game_state |= CARDS_DEALT;

}



void UpdatePads(){

	oldpadstate = padstate;
	oldmousestate = mousestate;

	SolitaireInputDeviceHandler();//read pads and extended mouse data on both ports and compile it to 1 padstate
	if(ai_state){
		if(temppadstate & (BTN_A|BTN_B|BTN_X|BTN_Y|BTN_SL|BTN_SR|BTN_SELECT|BTN_START))
			ai_state = 0;//main loop will check if(demo_playing && !ai_state)

		AIUpdate();
		return;
	}


//SPrintBinary(5,5,temppadstate);


	if(false){//tempstate & 0b0111000000000000){//some unknown controller signature, avoid bizzare actions
		SWaitVsync(1);
		ClearVram();
		SPrint(1,1,PSTR("PLEASE REMOVE UNKNOWN INPUT"));
		SPrint(1,2,PSTR("DEVICE"));
		while(1){
			SWaitVsync(8);
			if(!(temppadstate & 0b0111000000000000))//they disconnected it
				break;
			SolitaireInputDeviceHandler();


		}
		SWaitVsync(8);
		return;
	}

	if(temppadstate & 0x8000){//at least one mouse attached, convert LB and RB into Y and B buttons
	//	if(temppadstate & BTN_MOUSE_RIGHT)
	//	TriggerFx(1,255,1);
//HACK WHY??!?!?!?!
if(temppadstate & BTN_SL)
temppadstate |= BTN_MOUSE_RIGHT;
//HUGE HACK!!!!!!!!!!!!!


		temppadstate &= (BTN_MOUSE_LEFT|BTN_MOUSE_RIGHT);//eliminate bits that are always high for the mouse that correspond to d-pad etc
		padstate = 0x8000;//keep id bit so we can tell when a mouse is plugged/unplugged
		/*
		if(mousestate & BTN_MOUSE_RIGHT && !(oldmousestate & BTN_MOUSE_RIGHT)){
			if(!(padstate & BTN_B) && !(oldpadstate & BTN_Y))
				padstate |= BTN_Y;//MOUSE ONLY SHORTCUT TO CHANGE CARD
			else
				padstate |= BTN_A;
		}
*/
		if(!(oldpadstate & 0x8000)){//a mouse was newly attached this frame(wait a bit?)
	//		SolitaireSetMouseSensitivity();
		}

		//certain glitches are possible if controlling both a pad and mouse at the same time, oh well.
		if(temppadstate & BTN_MOUSE_LEFT)
			padstate |= BTN_B;
		if(temppadstate & BTN_MOUSE_RIGHT)
			padstate |= BTN_Y;

	}else{//regular pads
		padstate = temppadstate;
	//	mousestate = 0;//not necessary?
	}


//	padstate = ReadJoypad(0);//controller in 1st port can always control the cursor

	if(awaiting_key){
		if((padstate&(BTN_A|BTN_B|BTN_X|BTN_Y|BTN_SELECT|BTN_START)) && !(oldpadstate&(BTN_A|BTN_B|BTN_X|BTN_Y|BTN_SELECT|BTN_START)))
			awaiting_key = 0;
	}


	if(padstate & BTN_SELECT && !(oldpadstate & BTN_SELECT)){//user wants to change the cursor speed
		if(demo_playing){//ai is playing, don't change the speed since it will mess it up. instead change oldcursorspeed because it will be restored into cursorspeed when the demo is over...

				//if(++oldcursorspeed > 5)
				//	oldcursorspeed = 1;
		}else{
			if(++cursorspeed > 5)
				cursorspeed = 1;//title screen will detect (cursorspeed != oldcursorspeed) to know the user has changed speed in this function
		}
	}

	

		
	//if(mousestate)
	//	TriggerFx(1,255,true);



//if(mousestate){
//SPrintBinary(5,5,mousestate);
//WaitVsync(10);
//}
			if(mousestate&0x80){//left movement

				if(cursorx > ((mousestate&0x7F)*cursorspeed))
					cursorx -= ((mousestate&0x7f)*cursorspeed);
				else
					cursorx = 0;
			}else{//right movement
				if(cursorx < ((((SCREEN_TILES_H*TILE_WIDTH)-1)-(CURSOR_TILES_H*8))-((mousestate&0x7f)*cursorspeed))+3)
					cursorx += (mousestate&0x7f)*cursorspeed;
				else
					cursorx = (( ( (SCREEN_TILES_H*TILE_WIDTH)-1)-(CURSOR_TILES_H*8))+3);
			}
	
			if(mousestate&0x8000){//up movement
				if(cursory > ((mousestate>>8)&0x7f)*cursorspeed)
					cursory -= ((mousestate>>8)&0x7f)*cursorspeed;
				else
					cursory = 0;
			}else{//down movement
				if(cursory < ((((SCREEN_TILES_V*TILE_HEIGHT)-1)+18)-(CURSOR_TILES_H*8))-(cursorspeed*((mousestate>>8)&0x7f)))
					cursory += ((mousestate>>8)&0x7f)*cursorspeed;
				else
					cursory = ((((SCREEN_TILES_V*TILE_HEIGHT)-1)+18)-(CURSOR_TILES_H*8))-(cursorspeed*((mousestate>>8)&0x7f));
			}

}




uint8_t CheckVictory(){
	if(stacks[STACK_DECK].numcards == 0 && stacks[STACK_DRAW].numcards == 0 && stacks[STACK_HELD].numcards == 0){
		for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++){
			//if(!(stacks[i].cards[0]) & FACEUP)
			//	break;
			//if there are no face down cards left and nothing in the deck or draw stacks, then it is impossible to lose
			//even if the player hasn't manually put each one to the home piles. skip to the victory function which will
			//automatically do that for the player.
			//CHANGE OF PLAN, PLAYER MUST MANUALLY FINISH UP, GAME IS GETTING TOO SHORTCUT AUTOMATED
			if(stacks[i].numcards)
				break;
			if(i == STACK_BASE6){//victory
			/*
				uint32_t bonus = (uint32_t)(((uint32_t)70000UL)/round_seconds);//(uint32_t)(((uint32_t)140000UL)/round_seconds);
				game_score += bonus;
				SPrintNumV(12,1,game_score);//make sure player sees the applied bonus during the winning screen
				if(ai_state){//ai can't win
					ai_state = 0;//main loop will catch !ai_state && demo_playing and exit
					return 0;
				}
			*/
				return 1;
			}
		}
	}
//if(!ai_state && !demo_playing && stacks[STACK_BASE1].numcards < 2){//victory debugging
//game_score = 2799;
//return 1;
//}
	return 0;
}


inline void DrawStats(){
//	if(stack_held || (game_state & CARD_FLOATING))
//		return;
	if(game_state & SHOW_TIMER){
		SPrintNum(4,VRAM_TILES_V-1,round_seconds%60,1);
		SPrintNum(1,VRAM_TILES_V-1,round_seconds/60,0);
		SetTile(2,VRAM_TILES_V-1,COLON_TILE);
	}
	for(uint16_t voff = 12;voff<(12+(VRAM_TILES_H*5));voff+=VRAM_TILES_H)//blank score
		vram[voff] = RAM_TILES_COUNT;

	SPrintNumV(12,1,game_score);
	SetTile(12,0,CHECK_MARK_TILE);//menu check mark
}

void DrawTable(){
	DrawCard(1,0,CARD_FACEDOWN);//deck
	DrawCard(6,0,MAX_CARDS);//draw pile
	DrawCard(13,0,MAX_CARDS);//home 0
	DrawCard(17,0,MAX_CARDS);//home 1
	DrawCard(21,0,MAX_CARDS);//home 2
	DrawCard(25,0,MAX_CARDS);//home 3
	DrawCard(1,6,MAX_CARDS);//base 0
//	for(uint8_t i=STACK_BASE0;i<=STACK_BASE6;i++)
//		DrawCard(stacks[i].x,stacks[i].y,MAX_CARDS);
	SetTile(12,0,CHECK_MARK_TILE);//menu check mark
	SPrintNum(12,1,0,0);
}

uint8_t GameLoop(){
GAMELOOPTOP:
		game_state &= ~CARDS_DEALT;
		ClearVram();
	
		DrawTable();
		DealHand();

	

//		cursorspeed = 2;
	while(true){
		SWaitVsync(1);
		if(!ai_state && demo_playing){//ai won or gaveup, the demo is over
			demo_playing = 0;
			card_set = old_card_set;//we used a random card set to show off the graphics, revert back to the users preference
			game_state ^= DRAW_THREE;
			game_state |= old_game_options;//get the old option for DRAW_THREE or not
			cursorspeed = oldcursorspeed;
		//	FadeOut(3,true);
		//	FadeIn(2,true);
			return 1;
		}

		if((game_state & HAVE_MOVED) && round_seconds < 1000)
			if(++round_ticks > 59){
				round_ticks = 0;
				round_seconds++;
			}

		if(CheckVictory()){
			VictoryScreen();
			HighScoreEntry();
			return 1;
		}

		if(((padstate & BTN_START && !(oldpadstate & BTN_START)) || (padstate & BTN_B && !(oldpadstate & BTN_B) && CursorIntersects(12*8,8,8,8))) && !stack_held){//can't pause while holding a stack
			uint8_t pret = PauseMenu();
			if(pret == 0){//continue

			}else if(pret == 1){//new hand
				FadeOut(3,true);
				MoveAllToDeck();//SetInitialStacksState();
				Shuffle(255);
				SWaitVsync(1);
				FadeIn(2,false);
				goto GAMELOOPTOP;
			}else if(pret == 2){//quit to main menu
				return 0;
			}//else it shouldn't have returned
		}
		
		if(++last_click_time > DOUBLE_CLICK_TIME){
			last_click_time = DOUBLE_CLICK_TIME;
			last_click_num = 0;
			last_click_stack = 255;
		}

		DrawStats();
		UpdateCursor(0);
		DrawCursor();


	}
	return 1;
}

void PreVSRoutine(){

}

void PostVSRoutine(){

}

const char StackSetupTable[] PROGMEM = {
/*STACK_DECK, */ 1,0,52,STACKED,
/*STACK_DRAW, */ 6,0,0,HORIZONTAL,
/*STACK_HOME0,*/13,0,0,STACKED,
/*STACK_HOME1,*/17,0,0,STACKED,
/*STACK_HOME2,*/21,0,0,STACKED,
/*STACK_HOME3,*/25,0,0,STACKED,
/*STACK_BASE0,*/ 1,6,0,VERTICAL,
/*STACK_BASE1,*/ 5,6,0,VERTICAL,
/*STACK_BASE2,*/ 9,6,0,VERTICAL,
/*STACK_BASE3,*/13,6,0,VERTICAL,
/*STACK_BASE4,*/17,6,0,VERTICAL,
/*STACK_BASE5,*/21,6,0,VERTICAL,
/*STACK_BASE6,*/25,6,0,VERTICAL,
/*STACK_HELD, */ 255,255,0,VERTICAL,
};

void SetInitialStacksState(){

	uint8_t soff = 0;
	for(uint8_t i=STACK_DECK;i<=STACK_HELD;i++){
		stacks[i].x = pgm_read_byte(&StackSetupTable[soff++]);
		stacks[i].y = pgm_read_byte(&StackSetupTable[soff++]);
		stacks[i].numcards = pgm_read_byte(&StackSetupTable[soff++]);
		stacks[i].display = pgm_read_byte(&StackSetupTable[soff++]);
	}

	for(uint8_t i=0;i<MAX_CARDS;i++)
		stacks[STACK_DECK].cards[i] = i;
	stacks[STACK_DECK].numcards = 52;
}

void MoveAllToDeck(){
	for(uint8_t i=STACK_DECK+1;i<=STACK_HELD;i++){
		while(stacks[i].numcards){
			stacks[STACK_DECK].cards[stacks[STACK_DECK].numcards-1] = stacks[i].cards[stacks[i].numcards-1];
			stacks[i].numcards--;
			stacks[STACK_DECK].numcards++;
		}
		
	}

	for(uint8_t i=0;i<52;i++){
		bool found_it = false;
		for(uint8_t j=0;j<stacks[STACK_DECK].numcards;j++){
			if(stacks[STACK_DECK].cards[j] == i){
				found_it = true;
				break;
			}
		}
		if(!found_it){
			stacks[STACK_DECK].cards[stacks[STACK_DECK].numcards-1] = i;
			stacks[STACK_DECK].numcards++;
		}
	}
}

void main(void) __attribute__((noreturn));
void main(){

	GetPrngNumber(GetTrueRandomSeed());//seed LFSR
	Shuffle(233);//scramble the deck
	InitMusicPlayer(patches);
	SetTileTable((const char *)TableTiles);
	SetSpritesTileTable(Sprites);
	SetMasterVolume(224);
//	SetUserPostVsyncCallback(&PostVSRoutine);
//	SetUserPreVsyncCallback(&PreVSRoutine);
	SetInitialStacksState();

	SWaitVsync(1);
	EEPromScore(0);//read the eeprom data into the ram buffer(this include preferences and high scores)
	SWaitVsync(1);
	GetEEPromFlags();//load up scores and graphics/sound preferences
	SWaitVsync(1);


	if(!(game_state & NO_MUSIC))
		StartSong((const char *)(pgm_read_word(&musicData[music_track])));
	
	Intro();
	while(1){
		demo_playing = 0;
		TitleScreen();
		if(GameLoop())
			HighScoreScreen();
	}
}


