#define PLAYER_LEFT_BOUNDING_X		6
#define PLAYER_RIGHT_BOUNDING_X		9

int16_t constrain(int16_t x, int16_t a, int16_t b){
	if(x > a && x < b)
		return x;
	if(x < a)
		return a;
	return b;

}

uint8_t sampleTile(int16_t x, int16_t y){
	// clamp sampling point to screen
	//x = constrain(x, 0, SCREEN_WIDTH-1);
	//y = constrain(y, 0, SCREEN_HEIGHT-1);
	x >>= 3;
	y >>= 3;
	return vram[(y*VRAM_TILES_H)+x]-RAM_TILES_COUNT;//GetMapTile((uint8_t)x >> 3, (uint8_t)y >> 3);
}


bool isSolid(uint8_t x, uint8_t y){//WALKING

	uint8_t tile = sampleTile(x, y);

	return ((tile >= TILE_WALL && tile <= TILE_WALL+3) || (tile >= TILE_WALL_DARK && tile <= TILE_WALL_DARK+3) || (tile >= TILE_DOOR && tile <= TILE_DOOR+3));
}

bool canStandOn(uint8_t x, uint8_t y){//FALLING
	uint8_t tile = sampleTile(x, y);

	return ((tile >= TILE_WALL && tile <= TILE_WALL+3) || (tile >= TILE_WALL_DARK && tile <= TILE_WALL_DARK+3) ||
	 (tile >= TILE_DOOR && tile <= TILE_DOOR+3) || (tile >= TILE_LADDER && tile <= TILE_LADDER+3));
}

void initPlayer(){

	p.frame = 0;//TILE_PLAYER_RIGHT;
	p.x = (4*16);
	p.y = (4*16);
	p.dir = 1;
	p.walkPhase = 0;
	p.room = 0;
	p.score = 0;
	p.health = MAX_HEALTH;
	p.time = (uint32_t)(episode_base_time_limit<<8UL);//(249<<8) + 255;
	p.vely = 0;
	p.jumpTimer = 0;
	p.climbing = false;
	p.climbPhase = 0;
	p.hurtTimer = 0;
	p.gameover = 0;
}

void updatePlayer(){
//p.score = SD_EPISODE_SIZE;
	if(0){//record demo, don't push any buttons for a little bit at the start..
		if(controllerState != prevControllerState || (demo_hold_pad == 255)){
			WriteEeprom(demo_pos++,prevControllerState);
			WriteEeprom(demo_pos++,demo_hold_pad);
			demo_hold_pad = 1;
		//	demo_pad_state = controllerState;
		}else{
			demo_hold_pad++;
		}
	}


	if(demo_playing){
		if(!demo_hold_pad){
			demo_pad_state = pgm_read_byte(&demo_data[demo_pos++]);
			demo_hold_pad = pgm_read_byte(&demo_data[demo_pos++]);
		}
		demo_hold_pad--;
		controllerState = demo_pad_state;

		if(controllerState == 255){
			p.gameover = 3;
			music_fade_frames = 1;
			music_fade_step = 6;
		}
	}

	if(p.gameover)
		return;
	updateMoving();
	updateClimbing();
	updateJumpAndFall();
	updatePickup();

	updateCurrentRoom();
	updateHurt();
	updateTime();



	bool flip = (p.dir == 1);

	if(!p.gameover){
		if(p.airBorne){
			if(p.vely < 0)
			p.frame = 4;//(p.dir < 0 ? 4 : 0);
		else if(p.vely>>5 > 0)
			p.frame = 3;//(p.dir < 0 ? 3 : 1);
		//else if(p.vely > 0)//peak of jump?
		//	p.frame = 2;
		p.frame = 3;
		}

		if(p.climbing){
			p.frame = 5;
			flip = p.climbPhase&8;

		}

		//if(!p.climbing)
		//	p.frame += (p.dir < 0 ? TILE_PLAYER_LEFT : TILE_PLAYER_RIGHT);	
	}
		
	if(p.hurtTimer && (p.hurtTimer & 3) < 2)//player is flashing
		hideSprites(0,4);
	else
		updateSprite(0, p.frame, flip, p.x, p.y);

	if(!demo_playing && controllerState & BTN_START && !(prevControllerState & BTN_START)){
		pauseMenu();
	}
}

void gameover(){
	drawText(8+WINDOW_X_OFF, 10+WINDOW_Y_OFF, PSTR("GAME  OVER"));
	p.gameover = 1;
}


void hurtPlayer(){
	if(p.hurtTimer > 0)
		return;	// player is invulnerable

	TQ_triggerFx(SOUND_HURT);

	p.hurtTimer = 1;
	p.vely = -70;

	if(p.health > 0){
		p.health--;

		if(p.score >= 100)
			p.score -= 100;
	}

	if(p.health == 0){
		FadeMusicOut(3,1);
		gameover();
	}
}


int8_t calc_xgrid_adjust(volatile int16_t t){
///*WriteEeprom(0,TILE_GOLD);*/WriteEeprom(1,t);
//	dbf();
	if(t == 0 || t == 2 || t == 4 || t == 6){
//	dbf();
		return 0;
	}
	else{

		return -1;
}
}

int8_t calc_ygrid_adjust(uint8_t t){
	if(t == 0 || t == 1 || t == 4 || t == 5)
		return 0;
	else
		return -1;
}


void updatePickup(){

	for(int8_t dir = -1; dir <= 1; dir += 2){

		int16_t x = (dir < 0 ? (p.x + 1*2):(p.x + 6*2));
		int16_t y = p.y + 7*2;

		x >>= 3;
		y >>= 3;
		uint8_t t = GetTile(x,y);
	
		if(t >= TILE_GOLD && t <= TILE_GOLD+7){// pick up gold

			t -= TILE_GOLD;
			SetMapTile(x+calc_xgrid_adjust(t), y+calc_ygrid_adjust(t), TILE_EMPTY);
			TQ_triggerFx(SOUND_GOLD);
			p.score += 500;

		}


		if(t >= TILE_HEART_BIG && t <= TILE_HEART_BIG+7){// pick up heart
			
			if(t >= TILE_HEART)
				t -= TILE_HEART;
			else
				t -= TILE_HEART_BIG;

			SetMapTile(x+calc_xgrid_adjust(t), y+calc_ygrid_adjust(t), TILE_EMPTY);
			TQ_triggerFx(SOUND_GOLD);
			p.health = MAX_HEALTH;
			p.score += 100;

		}

		if(t >= TILE_KEY && t <= TILE_KEY+3){// pick up key

			t -= TILE_KEY;
			SetMapTile(x+calc_xgrid_adjust(t), y+calc_ygrid_adjust(t), TILE_EMPTY);
			TQ_triggerFx(SOUND_GOLD);

			uint16_t off = (WINDOW_Y_OFF+1)*VRAM_TILES_H;

			while(off < VRAM_SIZE){//WINDOW_X_OFF+((WINDOW_Y_OFF+(NUM_TILES_Y*2)*VRAM_TILES_H))){
				if(vram[off] >= TILE_DOOR+RAM_TILES_COUNT && vram[off] <= TILE_DOOR+3+RAM_TILES_COUNT){
					vram[off] = FIRST_MAP_TILE+RAM_TILES_COUNT;
				}
				off++;
			}


		}

		if(t >= TILE_PRINCESS && t <= TILE_PRINCESS+7){

			t -= TILE_PRINCESS;
			uint8_t xo = x+calc_xgrid_adjust(t);
			uint8_t yo = y+calc_ygrid_adjust(t);
			SetMapTile(xo+1,yo-2,TILE_HEART);
			p.x = (xo+2)<<3;
			p.y = yo<<3;
			p.frame = 0;
			p.hurtTimer = 0;
			p.dir = -1;
			//updateSprite(0, 0, 0, p.x, p.y);
			drawText(xo-2,yo-3, PSTR("YOU WIN!"));

			if(p.gameover == 0){
				p.gameover = 2;
				p.score += 2000;
			}

		}
	}

}

void updateMoving(){

	// moving
	if(controllerState & BTN_LEFT){// move left
		p.dir = -1;
		if(!(prevControllerState & BTN_LEFT))
			p.walkPhase = 6;
		else
			p.walkPhase++;
		if((!isSolid(p.x+PLAYER_LEFT_BOUNDING_X-1, p.y+1) && !isSolid(p.x+PLAYER_LEFT_BOUNDING_X-1, p.y + 14))){//0,2*7
			p.x--;
		}
	}else if(controllerState & BTN_RIGHT){// move right
		p.dir = 1;
		if(!(prevControllerState & BTN_RIGHT))
			p.walkPhase = 6;
		else
			p.walkPhase++;
		if(!isSolid(p.x + PLAYER_RIGHT_BOUNDING_X+1, p.y+1) && !isSolid(p.x + PLAYER_RIGHT_BOUNDING_X+1, p.y + 14)){//7*2,
			p.x++;
		}

	} else{// stand still
		p.walkPhase = 0;
	}



	// animate walking
	//p.frame = (p.walkPhase/3) % 5;	
	p.frame = (p.walkPhase/6) % 5;//update for 60hz
}

void updateClimbing(){
	
	uint8_t t	= sampleTile(p.x+(PLAYER_LEFT_BOUNDING_X),p.y+(15));//1*2/7*2
	uint8_t t2	= sampleTile(p.x+(PLAYER_RIGHT_BOUNDING_X),p.y+(15));//7*2
	uint8_t t3	= sampleTile(p.x+(PLAYER_LEFT_BOUNDING_X),p.y+(0));
	uint8_t t4	= sampleTile(p.x+(PLAYER_RIGHT_BOUNDING_X),p.y+(0));
	
	if((t >= TILE_LADDER 	&& t <= TILE_LADDER+3) 	|| (t2 >= TILE_LADDER && t2 <= TILE_LADDER+3) ||
		(t3 >= TILE_LADDER 	&& t3 <= TILE_LADDER+3) || (t4 >= TILE_LADDER && t4 <= TILE_LADDER+3))
		p.climbing = true;
	else
		p.climbing = false;
//	p.climbing = (sampleTile(p.x + 1*2, p.y + 7*2) == TILE_LADDER || sampleTile(p.x + 6*2, p.y + 7*2) == TILE_LADDER);

	// start climbing up
	if(!p.climbing && (controllerState & BTN_UP) != 0 && sampleTile(p.x + 2*2, p.y - 1*2) == TILE_LADDER && sampleTile(p.x + 5, p.y - 1) == TILE_LADDER){
		p.y--;
		p.climbing = true;
	}

	// start climbing down
	t	= sampleTile(p.x + 2*2, p.y + 8*2);
	t2	= sampleTile(p.x + 5*2, p.y + 8*2);

	if(!p.climbing && (controllerState & BTN_DOWN) != 0 && (t >= TILE_LADDER && t <= TILE_LADDER+3) && (t2 >= TILE_LADDER && t2 <= TILE_LADDER+3)){
		p.y++;
		p.climbing = true;
	}
	
	if(!p.climbing)
		return;

	if(controllerState & BTN_UP){
		t = isSolid(p.x+(1*2), p.y-(1*2));
		t2 = isSolid(p.x+(6*2),p.y-(1*2));
		p.climbPhase++;

		if(!t && !t2)
			p.y--;
		else{//determine if we should align the player horizontally
			if(t && !t2)
				p.x++;
			else if(t2 && !t)
				p.x--;
		}		
			
	} 
	if(controllerState & BTN_DOWN){
		t = isSolid(p.x+(1*2), p.y+(8*2));
		t2 = isSolid(p.x+(6*2),p.y+(8*2));
		p.climbPhase++;

		if(!t && !t2)
			p.y++;
		else{//determine if we should align the player horizontally
			if(t && !t2)
				p.x++;
			else if(t2 && !t)
				p.x--;
		}
	}
	if((controllerState & (BTN_UP|BTN_DOWN)) == 0 && (controllerState & (BTN_LEFT|BTN_RIGHT)) != 0)
		p.climbPhase++;

	// animate climbing
	p.frame = 5;//TILE_PLAYER_CLIMBING + ((p.climbPhase/5) & 1);

	p.vely = 0;
}

void updateJumpAndFall(){
//TODO ELIMINATE FLASH AND BIG ROOMS ADJACENT, JUST PUT 4 BYTES AFTER EACH SCREEN FOR IT!!!!!
//
//
///TODO MAKE NETWORKED ALARM CLOCK FOR CUNNINGFELLOWS UZE AND WATCH
//////
////////
	if(p.climbing)
		return;

	// jump
	bool buttonDown = (controllerState & (BTN_B|BTN_A)) != 0;

	bool buttonPressed = buttonDown && (prevControllerState & (BTN_B|BTN_A)) == 0;
	bool onsolid = (canStandOn(p.x + 3, p.y + 8*2) || canStandOn(p.x + 13, p.y + 8*2));//1*2,8*2,6*2,8*2
	if(buttonPressed && onsolid && p.hasLanded){
		TQ_triggerFx(SOUND_JUMP);//TriggerFx(SOUND_JUMP,255,1);
		p.hasLanded = 0;
		p.vely = -33L;
		p.jumpTimer = 0;
		p.airBorne = 1;
	}else if(!onsolid)
		p.airBorne = 1;

	// jump higher if button is held down
	if(p.jumpTimer >= 4*2 && !buttonDown)
		p.jumpTimer = 256L;
	//if(p.jumpTimer == 4*2)
	//	p.vely = -55*2L;
	//if(p.jumpTimer == 5*2)
	//	p.vely = -55*2L;
	if(p.jumpTimer >= 4*2 && p.jumpTimer <= 9)
		p.vely = -55L;

	// jump/fall
	p.y += p.vely>>5;

	// hit obstacle above?
	uint8_t t 	= isSolid(p.x+PLAYER_LEFT_BOUNDING_X,p.y-0);
	uint8_t t2 	= isSolid(p.x+PLAYER_RIGHT_BOUNDING_X,p.y-0); 
	bool solidAbove = (t || t2);

	if(p.vely < 0 && solidAbove){
		p.vely = 0;
		p.jumpTimer = 255;
	}

	// end of fall
	t 	= canStandOn(p.x+PLAYER_LEFT_BOUNDING_X,p.y+16);//6
	t2 	= canStandOn(p.x+PLAYER_RIGHT_BOUNDING_X,p.y+16); //9
	onsolid = (t || t2);

	if(p.vely > 0 && onsolid){
		p.hasLanded = 1;
		p.airBorne = 0;
		p.vely = 0;
		p.y = p.y & ~7;	// snap player to ground
		//p.y = p.y & ~15;//fix for 16x16 tiles
		p.jumpTimer = 255;
	}

	// fall on spikes
	t 	= sampleTile(p.x + PLAYER_LEFT_BOUNDING_X+0, p.y + 7*2);//5
	t2 	= sampleTile(p.x + PLAYER_RIGHT_BOUNDING_X-1, p.y + 7*2);//10
	bool spikes = ((t == TILE_SPIKES || t == TILE_SPIKES+1) || (t2 == TILE_SPIKES || t2 == TILE_SPIKES+1));
	
	if(p.vely > 0 && spikes)
		hurtPlayer();

	// gravity
	if(p.jumpTimer >= 3*2 && p.vely < 100*2)
		p.vely += 6;//11;

	p.jumpTimer = min(p.jumpTimer + 1, 255);
}


void updateCurrentRoom(){

	if(p.x < (WINDOW_X_OFF*8)-5){//was <=-8
		uint8_t r = room_adjacent[0];
		if(r != 0xff){
			changeRoom(r);
			p.x = ((WINDOW_X_OFF+(NUM_TILES_X*2))*8)-12;//-16
		}
	}

	if(p.x > (((WINDOW_X_OFF+(NUM_TILES_X*2))*8)-16)+5){
		uint8_t r = room_adjacent[1];
		if(r != 0xff){
			changeRoom(r);
			p.x = (WINDOW_X_OFF*8)-5;
		}
	}

	if(p.y < (WINDOW_Y_OFF+2)*8){//was <= 0
		uint8_t r = room_adjacent[2];
		if(r != 0xff){
			changeRoom(r);
			p.y = ((WINDOW_Y_OFF+(NUM_TILES_Y*2))*8)-16;	
		}
	}

	if(p.y > ((WINDOW_Y_OFF+(NUM_TILES_Y*2))*8)-16){//was >= SCREEN_HEIGHT
		uint8_t r = room_adjacent[3];
		if(r != 0xff){
			changeRoom(r);
			p.y = (WINDOW_Y_OFF+2)*8;
		}
	}

}

void updateHurt(){

	if(p.hurtTimer == 0)
		return;

	if(frame & 1)//fix for 60hz
		return;

	p.hurtTimer++;
	if(p.hurtTimer > 60)
		p.hurtTimer = 0;

//	if((p.hurtTimer & 3) < 2)
//		p.frame = TILE_EMPTY;

}

void updateScoreBar(){

//	if(!(frame & 1))//save some cycles
//		return;

	for(uint16_t i=WINDOW_X_OFF+(WINDOW_Y_OFF*VRAM_TILES_H);i<WINDOW_X_OFF+(NUM_TILES_X*2)+(WINDOW_Y_OFF*VRAM_TILES_H);i++){
		vram[i] = RAM_TILES_COUNT;
		vram[i+VRAM_TILES_H] = RAM_TILES_COUNT;
	
	}
	// update score
	uint16_t score = p.score;

	uint16_t s = 10000;
//	bool drew_nonzero = false;
	for(uint8_t i=0;i<5;i++){
		uint8_t n = score / s;
		score -= n * s;
		s /= 10;
//		if(n)
//			drew_nonzero = 1;
//		else if(!drew_nonzero && i<4)
//			continue;

		SetMapTile(WINDOW_X_OFF+(i<<1), WINDOW_Y_OFF, FIRST_HUD_TILE+(n<<2));
	}


	// update time
	uint32_t time = p.time >> 8;
	if(time >= TIME_SPEEDUP || (p.time & 127) <= 64 || p.gameover){
		s = 100;
		for(uint8_t i = 0; i < 3; i++){
			uint8_t n = time / s;
			time -= n * s;
			s /= 10;


			SetMapTile(WINDOW_X_OFF+12+(i<<1), WINDOW_Y_OFF, FIRST_HUD_TILE+(n<<2));
		}
	}

	// update hearts
	for(uint8_t i = 0; i < p.health; i++)
		SetMapTile(WINDOW_X_OFF+(24-(i<<1)), WINDOW_Y_OFF, TILE_HEART_BIG);

}

void updateTime(){

//	if(frame & 1)//fix for 60hz
//		return;

	uint8_t dt = 4;//8;
	if(p.time < TIME_SPEEDUP<<8)
		dt = 2;//4;

	if(p.time >= dt)
		p.time -= dt;
	else{
		p.time = 0;
		gameover();
	}

}


void pushWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h){

	uint16_t roff = 0;
	for(uint8_t i=x;i<=x+w;i++)
		for(uint8_t j=y;j<=y+h;j++)
			ram_tiles[roff++] = vram[i+(j*VRAM_TILES_H)];

}

void popWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h){

	uint16_t roff = 0;
	for(uint8_t i=x;i<=x+w;i++)
		for(uint8_t j=y;j<=y+h;j++)
			vram[i+(j*VRAM_TILES_H)] = ram_tiles[roff++];

}



//////////////////////////////////////////
///////TODO ADD TORCHES ON BACKGROUND
///////ADJUST FADER TO SIMULATE FLICKERS, GRAPHICS LOOKS GOOD AT DARKER LEVELS

uint8_t pauseMenu(){

//TriggerNote(3,0,9,255);
	StopSong();
	TQ_triggerFx(SOUND_GOLD);//TriggerFx(SOUND_GOLD,255,1);
	hideSprites(0,MAX_SPRITES);
	WaitVsync(1);
	uint8_t x = WINDOW_X_OFF+7;
	uint8_t y = WINDOW_Y_OFF+6;
	uint8_t w = 11;
	uint8_t h = 8;
	pushWindow(x,y,w,h);
	ClearWindow(x,y,w,h);
	DrawWindowFrame(x,y,w,h);
	drawText(x+3,y+2,PSTR("RESUME"));
	drawText(x+3,y+4,PSTR("MUSIC"));
	drawText(x+3,y+6,PSTR("QUIT"));

	uint8_t cursorpos = 0;
	while(1){
		for(uint8_t i=y+1;i<y+h-1;i++)
			SetMapTileDirect(x+1,i,FIRST_MAP_TILE);

		SetMapTileDirect(x+1,y+1+(cursorpos*2), TILE_KEY);
		SetTile(x+9,y+4,trackNo+33);

		if(cursorpos == 1){

			bool changed = false;
			if(controllerState & BTN_LEFT && !(prevControllerState & BTN_LEFT)){
				if(trackNo)
					trackNo--;
				changed = true;
			}else if(controllerState & BTN_RIGHT && !(prevControllerState & BTN_RIGHT)){
				if(trackNo < NUM_MUSIC_TRACKS)
					trackNo++;
				changed = true;
			}
			
			if(changed){
				if(trackNo)
					StartSong((const char *)(pgm_read_word(&musicData[trackNo])));
				else
					StopSong();
			}
		}

		if(controllerState & BTN_UP && !(prevControllerState & BTN_UP)){
			if(cursorpos)
				cursorpos--;
			else
				cursorpos = 2;
		}

		if(controllerState & BTN_DOWN && !(prevControllerState & BTN_DOWN)){
			if(cursorpos < 2)
				cursorpos++;
			else
				cursorpos = 0;
		}



		if(controllerState & BTN_START && !(prevControllerState & BTN_START)){
			if(cursorpos < 2){//resume game
				ResumeSong();
				popWindow(x,y,w,h);
				//if(cursorpos == 1)
				//	StartSong((const char *)(pgm_read_word(&musicData[trackNo])));
				return 0;
			}else{
				p.gameover = 1;
				TQ_triggerFx(SOUND_HURT);
				break;
			}
		}

		WaitVsync(1);
	}

	return 1;

}

