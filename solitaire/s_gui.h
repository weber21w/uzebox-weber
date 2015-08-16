uint8_t SWaitVsyncWithCancel(uint8_t frames){
	for(uint8_t i=0;i<frames;i++){
		if((padstate & 0x7FFF) && (padstate > oldpadstate))//skip MSBit which is always set if mouse plugged
			return 1;
		SWaitVsync(1);
	}
	return 0;
}

void Intro(){
//HighScoreScreen();return;
//return;//////////////////////////////////

	ClearVram();
	cursorx = (SCREEN_TILES_H*TILE_WIDTH)-(CURSOR_TILES_H*8);
	for(uint8_t i=0;i<14;i++){

		FloatCard(GetPrngNumber(0)%SCREEN_TILES_H,GetPrngNumber(0)%SCREEN_TILES_V,(SCREEN_TILES_H/2)-3,(SCREEN_TILES_V/2)-3,0,0,0);
		DrawCard((SCREEN_TILES_H/2)-4,(SCREEN_TILES_V/2)-3,0);
		FloatCard(GetPrngNumber(0)%SCREEN_TILES_H,GetPrngNumber(0)%SCREEN_TILES_V,(SCREEN_TILES_H/2)-0,(SCREEN_TILES_V/2)-3,0,0,0);
		DrawCard((SCREEN_TILES_H/2),(SCREEN_TILES_V/2)-3,0);
	}

	for(uint8_t i=0;i<14;i++){
		for(uint16_t i=SCREEN_TILES_H*((SCREEN_TILES_V/2)-5);i<SCREEN_TILES_H*((SCREEN_TILES_V/2)+5);i++)
			vram[i] = RAM_TILES_COUNT;

		if(i < 12)
			DrawCard((SCREEN_TILES_H/2)-4,(SCREEN_TILES_V/2)-3,0);
		if(i < 13)
			DrawCard((SCREEN_TILES_H/2),(SCREEN_TILES_V/2)-3,0);
		if(i > 11)
			SPrint((SCREEN_TILES_H/2)-3,(SCREEN_TILES_V/2)-1,PSTR("UZE"));
		if(i > 12)
			SPrint((SCREEN_TILES_H/2),(SCREEN_TILES_V/2)-1,PSTR("BOX"));
		FloatCard(((SCREEN_TILES_H/2)-4)+(i&1?4:0),(SCREEN_TILES_V/2)-1,GetPrngNumber(0)%SCREEN_TILES_H,GetPrngNumber(0)%SCREEN_TILES_V,(GetPrngNumber(0)%MAX_CARDS)|FACEUP,0,0);	
	}

	uint8_t did_move = 0;
	do{
		SWaitVsync(1);

		did_move = 0;
		for(uint8_t i=0;i<2;i++){
			if(cursorx > ((SCREEN_TILES_H/2)-1)*8){
				cursorx--;
				did_move = 1;
			}
			if(cursorx < ((SCREEN_TILES_H/2)-1)*8){
				cursorx++;
				did_move = 1;
			}
			if(cursory < ((SCREEN_TILES_V/2)*8)-4){
				cursory++;
				did_move = 1;
			}
			if(cursory > ((SCREEN_TILES_V/2)*8)-4){
				cursory--;
				did_move = 1;
			}	
		}
		DrawCursor();
	}while(did_move);
	SWaitVsync(12);
	cursorframe = 1;
	DrawCursor();
	SWaitVsync(8);
	FadeOut(2,true);
	FadeIn(2,false);
//	cursorx = 200;
//	cursory = 178;
}

void HowToPlayScreen();

void TitleScreen(){
	SetInitialStacksState();
	uint16_t idle_ticks = 0;
//	cursorx = 80;
//	cursory = 120;
	card_set = old_card_set;
	game_state |= CARDS_DEALT;
	game_state ^= CARDS_DEALT;
	game_state &= ~DRAW_THREE;
	if(old_game_options & DRAW_THREE)
		game_state |= DRAW_THREE;
//TITLETOP:
	idle_ticks = 0;
	ClearVram();
	SPrint((SCREEN_TILES_H/2)-10,1,PSTR("WELCOME TO SOLITAIRE"));

	SPrint(5, 7,PSTR("DECK"));
	SPrint(5,10,PSTR("DRAW   ONE   THREE"));
	SPrint(5,13,PSTR("MUSIC        OFF"));
	SPrint(5,16,PSTR("TIMER  HIDE  SHOW"));
	SPrint(5,19,PSTR("SPEED"));
	SPrint(5,22,PSTR("SAVE"));


	uint8_t flash = 0;
	uint8_t codepos = 0;
//TITLEREENTRY:
	while(true){
		SWaitVsync(1);


		if(padstate && !oldpadstate){
			if(padstate == pgm_read_word(&KonamiCode[codepos])){
				if(++codepos ==(sizeof(KonamiCode)/2) ){
				//	STriggerFx(1,255,true);
					FadeIn(1,false);
					codepos = 0;
					game_state ^= CHEAT_ACTIVE;
				}
			}else
				codepos = 0;
		}
		
		if(++idle_ticks > TITLE_IDLE_TIME){
		//	idle_ticks = 0;
		//	HowToPlayScreen();
		//	goto TITLETOP;
			ai_state = AI_CHOOSING;
			demo_playing = 1;
			old_card_set = card_set;
			card_set = GetPrngNumber(0)%GFX_NUM_CARD_SETS;
			old_game_options = game_state;
			if((GetPrngNumber(0)&3)==1){
				game_state |= DRAW_THREE;
			}
			goto TITLEESCAPE;
		}else if((padstate & 0x7FFF)|| mousestate)//ignore MSbit which is always set if mouse plugged
			idle_ticks = 0;

		Shuffle(10);
		//UpdatePad();//done in vsync routine
		UpdateCursor(1);
		DrawCursor();

		if((padstate & BTN_B) && !(oldpadstate & BTN_B)){
			if(CursorIntersects(12*8,(11-7)*8,CARD_WIDTH,CARD_HEIGHT)){//change card color
				if(++card_set == GFX_NUM_CARD_SETS)
					card_set = 0;
			}else if(CursorIntersects(12*8,10*8,3*8,8)){//draw type ONE
				game_state &= ~DRAW_THREE;
			}else if(CursorIntersects(18*8,10*8,5*8,8)){//draw type THREE
				game_state |= DRAW_THREE;
			}else if(CursorIntersects(12*8,13*8,2*8,8)){//music ON
				if(!(game_state & NO_MUSIC)){
					music_track++;
					if(music_track >= NUM_MUSIC_TRACKS)
						music_track = 0;
				}
				StopSong();
				StartSong((const char *)(pgm_read_word(&musicData[music_track])));
				game_state &= ~NO_MUSIC;
			}else if(CursorIntersects(18*8,13*8,3*8,8)){//music OFF
				game_state |= NO_MUSIC;
				StopSong();
			}else if(CursorIntersects(5*8,22*8,4*8,8)){//save settings
				SetEEPromFlags();
				EEPromScore(1);
			}else if(CursorIntersects(10*8,24*8,12*8,8)){//deal hand
				break;
			}else if(CursorIntersects(18*8,16*8,4*8,8)){//show timer
				game_state |= SHOW_TIMER;
			}else if(CursorIntersects(12*8,16*8,4*8,8)){//hide timer
				game_state &= ~SHOW_TIMER;
			}else if(CursorIntersects(12*8,19*8,2*8,8)){//speed
				if(++cursorspeed > MAX_CURSOR_SPEED)
					cursorspeed = 1;
			}
		}

		if(padstate & BTN_START && !(oldpadstate & BTN_START)){
			old_card_set = card_set;
			break;
		}

		DrawCard(12,10-7,0);
		SPrintNum(12,13,music_track+1,0);
		SPrintNum(12,19,cursorspeed,0);

		if(game_state & DRAW_THREE){
			SetTile(17,10,CHECK_MARK_TILE);
			SetTile(11,10,0);
		}else{
			SetTile(11,10,CHECK_MARK_TILE);
			SetTile(17,10,0);
		}
		if(game_state & NO_MUSIC){
			SetTile(17,13,CHECK_MARK_TILE);
			SetTile(11,13,0);
		}else{
			SetTile(11,13,CHECK_MARK_TILE);
			SetTile(17,13,0);
		}

		if(game_state & SHOW_TIMER){
			SetTile(17,16,CHECK_MARK_TILE);
			SetTile(11,16,0);
		}else{
			SetTile(11,16,CHECK_MARK_TILE);
			SetTile(17,16,0);
		}


		for(uint8_t i=10;i<23;i++)
			SetTile(i,24,0);
		if(++flash > 160)
			flash = 0;
		if(flash  > 80)
			SPrint(10,24,PSTR("TO DEAL HAND"));
		else
			SPrint(10,24,PSTR(" CLICK HERE"));
	}
TITLEESCAPE:
	FadeOut(3,false);
	while(DDRC){
		SWaitVsync(1);
		UpdateCursor(1);
		DrawCursor();
/*
		if(padstate && !oldpadstate){//cancel fade
			DDRC = 255;
			fadeStep = 12;//FADER_STEPS;
			fadeDir = 0;
			idle_ticks = 0;
			ai_state = 0;
			demo_playing = 0;
			card_set = old_card_set;
			game_state = old_game_options;
			goto TITLEREENTRY;
		}
*/
	}
	FadeIn(2,false);
}


void DrawVictoryScoreStats(uint16_t card_score, uint16_t bonus){
	for(uint8_t i=((SCREEN_TILES_H/2)-4);i<((SCREEN_TILES_H/2)-5)+9;i++){
		SetTile(i,10,2);
		SetTile(i,16,2);
	}
	for(uint8_t i=11;i<16;i++){
		SetTile(((SCREEN_TILES_H/2)-5)+0,i,4);
		SetTile(((SCREEN_TILES_H/2)-5)+9,i,5);
	}
	SetTile(((SCREEN_TILES_H/2)-5)+0,10,1);
	SetTile(((SCREEN_TILES_H/2)-5)+9,10,3);
	SetTile(((SCREEN_TILES_H/2)-5)+0,16,6);
	SetTile(((SCREEN_TILES_H/2)-5)+9,16,7);

	SPrint(8,10,PSTR("CARD SCORE"));
	SPrint(8,12,PSTR("TIME BONUS"));
	SPrint(8,14,PSTR("GAME TOTAL"));
	SPrintNum(22,10,card_score,1);
	SPrintNum(22,12,bonus,1);
	SPrintNum(22,14,game_score,1);
}

void VictoryScreen(){//return;//////////////////////

	uint16_t bonus = (uint16_t)(((uint32_t)63000UL)/round_seconds);//(uint32_t)(((uint32_t)140000UL)/round_seconds);
//	uint16_t card_score = game_score;
	game_score += bonus;

	//SPrintNumV(12,1,game_score);//make sure player sees the applied bonus during the winning screen
	if(ai_state){//ai can't win
		ai_state = 0;//main loop will catch !ai_state && demo_playing and exit
		return;
	}


//////TESTING SETUP/////////
uint8_t coff = 0;
for(uint8_t i=STACK_HOME0;i<=STACK_HOME3;i++){
	for(uint8_t j=0;j<13;j++)
		stacks[i].cards[j] = FACEUP|coff++;
	stacks[i].numcards = 13;
}
stacks[STACK_DECK].numcards = 0;



	for(uint8_t i=0;i<10;i++){
		DDRC = i&1?255:0;//flash screen a few times(how creative right?)
		DDRC > 0?(SetLedOn()):(SetLedOff());
		//PWM THE CONSOLE LED INSTEAD??!?
		for(uint8_t j=0;j<6;j++){
			SWaitVsync(1);
			UpdateCursor(1);
			DrawCursor();
		}
		//DDRC = 255;
	}


/*for(uint8_t i=STACK_HOME0;i<=STACK_HOME3;i++){//TODO HACK WHY IS THIS NECESSARY???!?!?! SOME CARDS COME OUT FACE DOWN!!
	for(uint8_t j=0;j<13;j++){
		stacks[i].cards[j] = (j*(i-STACK_HOME0))|FACEUP;
	}
	stacks[i].numcards = 13;
}*/


	uint8_t i=STACK_HOME0;
	awaiting_key = 1;//LET USER END SEQUENCE EARLY BY PRESSING A BUTTON, SPECIAL CASE HANDLED IN UpdatePad()
	while(1){
			if(!stacks[i].numcards){
				if(i++ == STACK_HOME3)
					break;//goto VICTORYBREAK;
				continue;
			}
			uint8_t dx = GetPrngNumber(0)%SCREEN_TILES_H;
			uint8_t dy = GetPrngNumber(0)%SCREEN_TILES_V;
			uint8_t ndx = GetPrngNumber(0)%SCREEN_TILES_H;
			uint8_t ndy = GetPrngNumber(0)%SCREEN_TILES_V;
			uint8_t card = stacks[i].cards[stacks[i].numcards-1];
			FloatCard(13+((i-STACK_HOME0)*CARD_TILES_H),0,dx,dy,card,1,0);//send them somewhere on the table
			FloatCard(dx,dy,ndx,ndy,card,1,0);//send them somewhere else..
			dx = GetPrngNumber(0)%SCREEN_TILES_H;
			dy = GetPrngNumber(0)%SCREEN_TILES_V;
			FloatCard(ndx,ndy,dx,dy,card,1,0);//and somewhere else...
			ndx = GetPrngNumber(0)%SCREEN_TILES_H;
			ndy = GetPrngNumber(0)%SCREEN_TILES_V;
			FloatCard(dx,dy,ndx,ndy,card,1,0);//and somewhere else again...
			card &= ~FACEUP;
			FloatCard(ndx,ndy,1,0,card,1,0);//send them to the deck
			stacks[STACK_DECK].numcards++;
			if(stacks[STACK_DECK].numcards == 49)
				FadeOut(12,false);
			if(!awaiting_key)
				goto VICTORYSHORTCUT;

			stacks[STACK_DECK].cards[stacks[STACK_DECK].numcards-1] = card;
			stacks[i].numcards--;
			DrawStack(STACK_DECK);
			for(uint8_t j=STACK_HOME0;j<=STACK_HOME3;j++)
				DrawStack(i);
		if(++i > STACK_HOME3)
			i = STACK_HOME0;

	}
//VICTORYBREAK:
//	FadeOut(5,false);
	while(DDRC){
		SWaitVsync(1);
		UpdateCursor(1);
		DrawCursor();
	}
	FadeIn(3,false);
	return;
VICTORYSHORTCUT:
	FadeOut(4,false);
	while(DDRC){
		SWaitVsync(1);
		UpdateCursor(1);
		DrawCursor();
	}
	FadeIn(3,false);
	
}


const char default_highscore_name[] PROGMEM = "UNKNOWN ";
void HighScoreEntry(){
	if(game_state & CHEAT_ACTIVE)
		return;


//return;//////////////////////////

	uint8_t score_place,char_place,char_last;
	uint16_t eeprom_score;
HIGHSCOREENTRYTOP:
	score_place = 255;
	char_place = 0;
	char_last = 0;

	for(uint8_t i=0;i<3;i++){
		eeprom_score = eeprom_data[8+(i*10)]<<8UL;
		eeprom_score |= eeprom_data[9+(i*10)];
		if(game_score > eeprom_score){
			score_place = i;
			for(uint8_t j=2;j>i;j--){
				for(uint8_t k=0;k<10;k++){
					if(k < 8)
						eeprom_data[(j*10)+k] = eeprom_data[((j-1)*10)+k]&127;//remove MSbits used for preferences
					else
						eeprom_data[(j*10)+k] = eeprom_data[((j-1)*10)+k];
				}
			}
			eeprom_data[8+(i*10)] = game_score>>8UL;
			eeprom_data[9+(i*10)] = game_score&255UL;
			break;
		}
	}
	if(score_place == 255)
		return;
//score_place = 0;

	ClearVram();
	SPrint(3,2,PSTR("CONGRATULATIONS YOU MADE"));
	SPrint(7,3,PSTR("THE LEADER BOARD"));
	SPrint(7,5,PSTR("ENTER YOUR NAME"));
	
	unsigned char char_buf[2];
	char_buf[1] = 0;
	uint8_t x,y;
	x = 5;
	y = 10;
	for(uint8_t i='A';i<='Z';i++){
		char_buf[0] = i;
		SPrintRam(x,y,char_buf);
		x += 2;
		if(x >= SCREEN_TILES_H-5){
			y += 2;
			x = 5;
		}
	}

	SPrint(23,8,PSTR("POINTS"));
	SPrintNum(21,8,game_score,0);
	SPrint(17,14,PSTR("DEL END"));
	SPrint(12,16,PSTR("SPACE"));
	for(uint8_t i=0;i<8;i++)
		SetTile(8+i,8,2);
	uint8_t flash = 0;
DEFAULTHIGHSCORETOP:
	while(1){
		SWaitVsync(1);
		UpdateCursor(1);
		DrawCursor();
		if(padstate & BTN_START && !(oldpadstate & BTN_START)){
			break;
		}
		if(++flash > 18)
			flash = 0;
		if(flash > 9 && char_place < 8)
			SetTile(8+char_place,7,66);
		else
			SetTile(8+char_place,7,char_last);
		if(padstate & BTN_B && !(oldpadstate & BTN_B) && CursorIntersects(5*8,10*8,19*8,7*8)){//we clicked in the characters area
			if(CursorIntersects((17*8),(14*8),3*8,8)){//clicked "DEL"
				SetTile(8+char_place,7,0);
				if(char_place){
					char_place--;
				}
			//	char_last = vram[(8+char_place)+(7*VRAM_TILES_H)]-RAM_TILES_COUNT;
				char_last = 0;
			}else if(CursorIntersects((21*8),(14*8),3*8,8)){//clicked "END"
				uint8_t not_blank = 0;
				for(uint8_t i=0;i<7;i++){
					if(vram[(8+i)+(7*VRAM_TILES_H)]-RAM_TILES_COUNT != 2){
						not_blank = 1;
						break;
					}
				}
				if(!not_blank){//user did not enter any characters, fill in a default
					uint8_t stroff = 0;
					for(uint8_t i=0;i<7;i++)
						vram[(8+i)+(7*VRAM_TILES_H)] = pgm_read_byte(&default_highscore_name[stroff++])+RAM_TILES_COUNT;
					goto DEFAULTHIGHSCORETOP;//the first "END" click fills in a default name, the player will see what will happen and can click again to accept
				}
				break;
			}else if(CursorIntersects((12*8),16*8,5*8,8)){//space
					char_last = 0;
					SetTile(8+char_place,7,0);
					if(char_place < 7){
						char_place++;
					}
			}else{//either clicked a character, or blank space
				uint8_t clicked_tile = vram[(((cursory+0)/8)*VRAM_TILES_H)+((cursorx+9)/8)]-RAM_TILES_COUNT;
//SetTile(0,0,clicked_tile);
				if(clicked_tile != 0){//not a space, is some character
					SetTile(8+char_place,7,clicked_tile);
					char_last = clicked_tile;
					if(char_place < 7){
						char_place++;
						char_last = 0;
					}
				}else{//space between letters
				/*
					char_last = 0;
					SetTile(8+char_place,7,0);
					if(char_place < 7){
						char_place++;
					}
				*/
				}
			}
		}
	}
	uint8_t found_char;
	for(uint8_t i=0;i<8;i++){
		eeprom_data[(score_place*10)+i] &= 128;//keep bits used for preferences
		if(vram[8+i+(VRAM_TILES_H*7)]-RAM_TILES_COUNT && vram[8+i+(VRAM_TILES_H*7)]-RAM_TILES_COUNT != 66){//don't add the flashing cursor
			found_char = 1;
			eeprom_data[(score_place*10)+i] |= ((vram[8+i+(VRAM_TILES_H*7)]-(RAM_TILES_COUNT+FONT_START_TILE))+'A');
		}else
			eeprom_data[(score_place*10)+i] |= ' ';
	}
	if(!found_char){//user did not enter any characters
		//STriggerFx(10,255,true);
		ClearVram();
		SPrint(12,12,PSTR("PLEASE ENTER A NAME"));
		SWaitVsync(60);
		goto HIGHSCOREENTRYTOP;
	}

	SetEEPromFlags();//fix preference bits that sorting destroyed
	EEPromScore(1);//update data to eeprom, Uzenet will pick this up before long
	FadeOut(4,false);
	while(DDRC){
		SWaitVsync(1);
		UpdateCursor(1);
		DrawCursor();
	}
	FadeIn(1,false);
}



uint8_t ScrollScoreDataUp(uint8_t start_slot, uint8_t total_slots, uint8_t spacing){
	uint8_t ticks = 0;
	uint8_t xoff;
	for(uint8_t k=start_slot;k<start_slot+total_slots;k++){
	xoff = 6;
	for(uint8_t j=k*10;j<(k*10)+10;j++){
		ticks = 0;
		if(j<(k*10+8)){
			for(uint8_t i=SCREEN_TILES_V-1;i>9+((k-start_slot)*spacing);i--){
				uint8_t c;
				c = eeprom_data[j]&127;//eliminate any top bits used to store game preferences
				if(c == ' '){
	
					continue;
				}
				c=(((c&127)-'A')+FONT_START_TILE);
				SetTile(xoff,i,c);
				if(i != SCREEN_TILES_V-1)
					SetTile(xoff,i+1,0);
				if((padstate & 0x7FFF) && !(oldpadstate & 0x7FFF))
					return 1;
				if(ticks++){
					ticks = 0;
					SWaitVsync(1);
				}
			}
			xoff++;
		}else{//we are displaying the score digits
			uint16_t score_digits = eeprom_data[j+0]<<8;
			score_digits |= eeprom_data[j+1];
		//	score_digits = 65535;
			char digit_tiles[5] = {0,0,0,0,0,};
			digit_tiles[0] = (score_digits/10000UL);
			score_digits %= 10000UL;
			digit_tiles[1] = (score_digits/1000UL);
			score_digits %= 1000UL;
			digit_tiles[2] = (score_digits/100UL);
			score_digits %= 100UL;
			digit_tiles[3] = (score_digits/10UL);
			score_digits %= 10UL;
			digit_tiles[4] = (score_digits/1UL);
			for(uint8_t l=0;l<4;l++){
				if(digit_tiles[l] == 0)
					digit_tiles[l] = ' ';
				else
					break;
			}
			xoff = 15;
			for(uint8_t m=0;m<5;m++){

			for(uint8_t l=SCREEN_TILES_V-1;l>9+((k-start_slot)*spacing);l--){
				uint8_t c;
				c = digit_tiles[m];
				if(c == ' '){
	
					continue;
				}

				SetTile(xoff,l,c+NUM_START_TILE);
				if(l != SCREEN_TILES_V-1)
					SetTile(xoff,l+1,0);
				if((padstate & 0x7FFF) && !(oldpadstate & 0x7FFF))
					return 1;
				if(ticks++){
					ticks = 0;
					SWaitVsync(1);
				}
			}
			xoff++;
			}
			break;
		}
	}
	}
	return 0;
}

void ScrollScoreDataDown(){
	for(uint8_t y=6;y<SCREEN_TILES_V-1;y++){
		for(uint8_t x=0;x<24;x++){
			vram[(y*VRAM_TILES_H)+x+VRAM_TILES_H] = vram[(y*VRAM_TILES_H)+x];
			vram[(y*VRAM_TILES_H)+x] = RAM_TILES_COUNT;
		}
		SWaitVsync(2);
	}
	for(uint8_t x=0;x<24;x++)
		vram[((SCREEN_TILES_V-1)*VRAM_TILES_H)+x] = RAM_TILES_COUNT;
}

void HighScoreScreen(){
//return;
//		FadeOut(3,true);
		HideSprites();

		for(uint16_t i=0;i<VRAM_SIZE;i++)
			vram[i] = RAM_TILES_COUNT;
		for(uint8_t y=0;y<6;y++)
			for(uint8_t x=0;x<24;x++)
				vram[(y*VRAM_TILES_H)+x] = RAM_TILES_COUNT+95+(9*old_card_set);//RAM_TILES_COUNT+113;//make the top highlight color
		for(uint8_t y=6;y<SCREEN_TILES_V;y++)
			for(uint8_t x=24;x<SCREEN_TILES_H;x++)
				vram[(y*VRAM_TILES_H)+x] = RAM_TILES_COUNT+95+(9*old_card_set);//RAM_TILES_COUNT+113;//make the bottom highlight color

	
		SPrint(8,6,PSTR("HIGH SCORES"));
		FadeIn(3,true);
		SWaitVsync(4);

		if(ScrollScoreDataUp(0,3,4))
			goto HIGHSCOREDONE;
		if(SWaitVsyncWithCancel(180))
			goto HIGHSCOREDONE;

//uzenet_got_records = 1;
//for(uint8_t i=30;i<80;i++){
//eeprom_data[i] = pgm_read_byte(&UzenetError[i-30]);
//}
		ScrollScoreDataDown();
		if(uzenet_got_records){
//			FadeOut(1,true);
			SPrint(7,6,PSTR("WORLD RECORDS"));
//			FadeIn(1,true);
			ScrollScoreDataUp(3,5,2);
			SWaitVsyncWithCancel(180);
		}

HIGHSCOREDONE:
		FadeOut(3,true);
		FadeIn(1,false);

}


void PauseSaveScreen(){
	uint16_t off = 0;
	for(uint8_t y=10+0;y<10+7;y++)
		for(uint8_t x=0+((SCREEN_TILES_H/2)-5);x<((SCREEN_TILES_H/2)-5)+10;x++){
			table_restore[off++] = vram[x+(y*VRAM_TILES_H)];
			vram[x+(y*VRAM_TILES_H)] = RAM_TILES_COUNT;//make green
		}
}

void PauseRestoreScreen(){
	uint16_t off = 0;
	for(uint8_t y=10+0;y<10+7;y++)
		for(uint8_t x=0+((SCREEN_TILES_H/2)-5);x<((SCREEN_TILES_H/2)-5)+10;x++)
			vram[x+(y*VRAM_TILES_H)] = table_restore[off++];
}
uint8_t PauseMenu(){

	PauseSaveScreen();
//PAUSETOP:
	for(uint8_t i=((SCREEN_TILES_H/2)-4);i<((SCREEN_TILES_H/2)-5)+9;i++){
		SetTile(i,10,2);
		SetTile(i,16,2);
	}
	for(uint8_t i=11;i<16;i++){
		SetTile(((SCREEN_TILES_H/2)-5)+0,i,4);
		SetTile(((SCREEN_TILES_H/2)-5)+9,i,5);
	}
	SetTile(((SCREEN_TILES_H/2)-5)+0,10,1);
	SetTile(((SCREEN_TILES_H/2)-5)+9,10,3);
	SetTile(((SCREEN_TILES_H/2)-5)+0,16,6);
	SetTile(((SCREEN_TILES_H/2)-5)+9,16,7);

		
	SPrint((SCREEN_TILES_H/2)-4,11,PSTR("CONTINUE"));
	SPrint((SCREEN_TILES_H/2)-4,13,PSTR("NEW HAND"));
	SPrint((SCREEN_TILES_H/2)-2,15,PSTR("QUIT"));

	uint8_t step = 0;
	while(true){
		SWaitVsync(1);

		//UpdatePad();//done in vsync routine
		UpdateCursor(1);
		DrawCursor();
		if(!step && (padstate & BTN_B) && !(oldpadstate & BTN_B))
			step = 1;
		else if(step == 1 && (oldpadstate & BTN_B) && !(padstate & BTN_B)){//determine which option the user selected
			if(CursorIntersects(((SCREEN_TILES_H/2)-4)*8,11*8,8*8,8) || CursorIntersects(12*8,0,8,8)){//continue or check mark
				step = 0;
				SWaitVsync(1);
				PauseRestoreScreen();
			}else if(CursorIntersects(((SCREEN_TILES_H/2)-4)*8,13*8,8*8,8)){//new hand
				step = 1;
			}else if(false){//CursorIntersects(((SCREEN_TILES_H/2)-2)*8,12*8,4*8,8)){//help
				//HelpScreen();
				//goto PAUSETOP;
			}else if(CursorIntersects(((SCREEN_TILES_H/2)-2)*8,15*8,4*8,8)){//quit to main menu
				step = 2;
			}else{//selected nothing
				continue;
			}
			break;
		}else if(padstate & BTN_START && !(oldpadstate & BTN_START)){
			step = 0;
			SWaitVsync(1);
			PauseRestoreScreen();
			break;
		}

	}
	SWaitVsync(1);

	return step;
}
