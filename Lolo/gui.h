bool ButtonPushedWithVsync(uint8_t vsyncs){
	for(uint8_t i=0;i<vsyncs;i++){
		UpdatePad();
		if(padstate)
			return true;
		WaitVsync(1);
	}
	return false;
}

void WhiteOut(uint8_t wait){
	uint8_t seg = 0;
	for(uint8_t j=0;j<8*3;j++){
		for(uint16_t i=0;i<RAM_TILES_COUNT*64;i++){
			uint8_t t = ram_tiles[i];
			if((seg == 0) && (t&0b11000000) < (0b11000000))
				t += 0b01000000;
			if((seg == 1) && (t&0b00111000) < (0b00111000))
				t += 0b00001000;
			if((seg == 2) && (t&0b00000111) < (0b00000111))
				t += 0b00000001;
			ram_tiles[i] = t;
		}
		if(++seg > 2)
			seg = 0;
		if(wait)
			WaitVsync(wait);
	}
}

void WhiteIn(uint16_t screen, uint8_t wait){
//the screen should be white before calling
	for(uint16_t i=0;i<RAM_TILES_COUNT*64;i++)
		ram_tiles[i] = 255;
	WaitVsync(1);

	RamifyFromSD(screen,RS_SKIP_RAMTILES);//just get the map preloaded so ram_tiles load is faster(hopefully keeping the effect working)

	for(uint8_t k=0;k<8;k++){
	WaitVsync(1);
	RamifyFromSD(screen,RS_SKIP_VRAM);//hopefully this all works out fast enough!!!

	uint8_t seg = 0;
	for(uint8_t j=0;j<(8-k)*3;j++){
		for(uint16_t i=0;i<RAM_TILES_COUNT*64;i++){
			uint8_t t = ram_tiles[i];
			if((seg == 0) && (t&0b11000000) < (0b11000000))
				t += 0b01000000;
			if((seg == 1) && (t&0b00111000) < (0b00111000))
				t += 0b00001000;
			if((seg == 2) && (t&0b00000111) < (0b00000111))
				t += 0b00000001;
			ram_tiles[i] = t;
		}
		if(++seg > 2)
			seg = 0;
		if(wait)
			WaitVsync(wait);
	}
	}

}


void RamifyFromSD(uint8_t id, uint8_t flags){
	if(!(flags & RS_SKIP_SPRITES))
		HideSprites();//important, sprite blitting during SD read will ruin our buffered data
/*
	FRESULT res = 0;
	uint8_t buffer[8];
	uint8_t buff2[53];
//	if(true){//flags & RS_REMOUNT){
//		FATFS fs;

		pf_mount((FATFS*)&buff2);//res	=	pf_mount(&fs);
		res	|=	pf_open(LOLO_SD_DATA_FILE);
		if(res)
			DebugCrash(0,3,3);
//	}


	uint32_t soff = (uint32_t)(id*16UL);
	WORD br;
	
	res			=		pf_lseek(soff);
	res			|=		pf_read((BYTE *)buffer,8,&br);
	
	if(res)
		DebugCrash(1,99,99);

	if(!(flags & RS_SKIP_RAMTILES)){
		soff	=	(uint32_t)(((uint32_t)buffer[0]<<24UL)+((uint32_t)buffer[1]<<16UL)+((uint32_t)buffer[2]<<8UL)+((uint32_t)buffer[3]<<0UL));//graphics data
		res |=		pf_lseek(soff);
		res |=		pf_read((BYTE *)ram_tiles,(RAM_TILES_COUNT*64),&br);
		if(res)
			DebugCrash(1,res,id);
	}


	if(!(flags & RS_SKIP_VRAM)){
		soff	=	(uint32_t)(0+((uint32_t)buffer[4]<<24UL)+((uint32_t)buffer[5]<<16UL)+((uint32_t)buffer[6]<<8UL)+((uint32_t)buffer[7]<<0UL));//map data
		res |=		pf_lseek(soff);
		res |=		pf_read((BYTE *)vram,(VRAM_TILES_H*SCREEN_TILES_V),&br);
		if(res)
			DebugCrash(1,res+44,id);
	}

	if(res)
		DebugCrash(1,2,3);
		*/
}



void DrawMenu(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t offset){
	for(uint8_t i=y;i<y+h;i++)
	for(uint8_t j=x;j<x+w;j++)
		SetTile(j,i,197+offset);

	for(uint8_t i=x;i<x+w;i++){
		SetTile(i,y+0,75+offset);
		SetTile(i,y+h,75+offset);
	}
	for(uint8_t i=y;i<y+h;i++){
		SetTile(x+0,i,74+offset);
		SetTile(x+w,i,74+offset);
	}
	SetTile(x+0,y+0,73+offset);
	SetTile(x+w,y+0,73+offset);
	SetTile(x+0,y+h,73+offset);
	SetTile(x+w,y+h,73+offset);
	
}

void SaveUnderSquare(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t rt){
	uint16_t rtoff = rt*64;
	for(uint8_t i=y;i<y+h;i++)
	for(uint8_t j=x;j<x+w;j++)
		ram_tiles[rtoff++] = vram[(i*VRAM_TILES_H)+j];
}

void RestoreUnderSquare(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t rt){
	uint16_t rtoff = rt*64;
	for(uint8_t i=y;i<y+h;i++)
	for(uint8_t j=x;j<x+w;j++)
	ram_tiles[rtoff++] = vram[(i*VRAM_TILES_H)+j];
}

inline void SoundTest();

inline bool InGameMenu(){
	uint8_t cursorpos = 0;
	uint8_t musicchoice = music&0x07;
	SaveUnderSquare(10,10,10,10,RAM_TILES_COUNT-2);

	while(true){
		UpdatePad();
		if(padstate & BTN_UP && !(oldpadstate & BTN_UP)){
			if(--cursorpos == 255)
				cursorpos = 2;
		}
		if(padstate & BTN_DOWN && !(oldpadstate & BTN_DOWN)){
			if(++cursorpos > 2)
				cursorpos = 0;
		}
		if(padstate & BTN_LEFT && !(oldpadstate & BTN_LEFT) && cursorpos == 1){
			if(musicchoice&0x07)
				musicchoice--;
		}
		if(padstate & BTN_RIGHT && !(oldpadstate & BTN_RIGHT) && cursorpos == 1){
			if((musicchoice&0x07) < 4)
				musicchoice++;
		}

		if(padstate & BTN_START && !(oldpadstate & BTN_START)){
			if(cursorpos == 0){//resume
				gui_state = 0;
				RestoreUnderSquare(10,10,10,10,RAM_TILES_COUNT-2);
				return false;
			}else if(cursorpos == 1){//music
				musicchoice |= MUSICNOTRANDOM;
				music = musicchoice;
				StopSong();
		StartSong(LoloMusic[music&0x07]);
			}else if(cursorpos == 2){//quit
				FadeOut(1,true);
				gui_state = GUITITLE;
				return false;
			}
		}
		DrawMenu(10,10,10,10,0);
		LoloPrint(12+0,11+0,PSTR("RESUME"),0);
		LoloPrint(12+0,11+1,PSTR("MUSIC"),0);LoloPrintNum(12+6,11+1,musicchoice&0x0F);
		LoloPrint(12+0,11+2,PSTR("TITLE"),0);
		SetTile(11,11+cursorpos,200);
		WaitVsync(1);
	}
}

#define QUICKLOAD 1


inline bool Title();
inline bool MainMenu();
inline bool FloorIntro();
inline bool LevelSelect();
inline bool EndGameSequence();
inline bool StoryScene();
inline bool SecretMenu();


void SetLevelComplete(uint16_t level, bool set) {
	struct EepromBlockStruct eeprom_data;
	if(EepromReadBlock(63+(level/(30*8)),&eeprom_data) == 0){
		eeprom_data.id = 63+(level/(30*8));
		for(uint8_t i=0;i<30;i++)
			eeprom_data.data[i] = 0;
	}
	eeprom_data.data[(level%240)/8] |= (128>>(level%8));
	EepromWriteBlock(&eeprom_data);

}

bool GetLevelComplete(uint16_t level, bool set) {
	struct EepromBlockStruct eeprom_data;
	if(EepromReadBlock(63+(level/(30*8)),&eeprom_data) == 0){
		eeprom_data.id = 63+(level/(30*8));
		for(uint8_t i=0;i<30;i++)
			eeprom_data.data[i] = 0;
	}
	
	return eeprom_data.data[(level%240)/8] & (128>>(level%8));
}

bool LevelLocked(){
	if(lvl.current == 0)
		return false;
	return !GetLevelComplete(lvl.current-1,0);
}

uint8_t Gui(){
	switch(gui_state){
	case GUITITLE:		return Title();break;
	case GUIMAINMENU:	return MainMenu();break;
	case GUIFLOORINTRO:	return FloorIntro();break;
	case GUILEVELSEL:	return LevelSelect();break;
	case GUIENDGAME:	return EndGameSequence();break;
	case GUIINGAMEMENU:	return InGameMenu();break;
	case GUINEWGAME:	return NewGameIntro();break;
	case GUISTORY:		return StoryScene();break;
	case GUISECRET:		return SecretMenu();break;
	default:break;
	};
	return 0;
}

const char LakeSceneAnim[] PROGMEM = {3,4,5, 3,4,5,3,5,3,4,3, };
const char LakeSceneFrameTimes[] PROGMEM = {};
#define NUMLAKESCENEANIMFRAMES 24
const char FlyAwayAnim[] PROGMEM = {8,9,10,10,9,8,8,9,10,10,9,8,8,8,8,8,8,9,10,11,12,13,14,15};
#define NUMFLYAWAYANIMFRAMES 24
const char MagicDustAnim[] PROGMEM = {1,2,3,};
#define NUMMAGICDUSTFRAMES 16
const char ZoomLoloAnim[] PROGMEM = {1,1,};

#define NUMZOOMLOLOFRAMES 4
inline bool StoryScene(){
	WaitVsync(1);

	RamifyFromSD(4,RS_REMOUNT);//load lake scene
	WaitVsync(100);
	
	sprites[0].x = 0;
	sprites[0].y = 128;
	sprites[0].tileIndex = 70;
	uint8_t count = 0;
	while(sprites[0].x < 128){
		for(uint8_t i=0;i<NUMLAKESCENEANIMFRAMES;i++){
			RamifyFromSD(pgm_read_byte(&LakeSceneAnim[i]),0);
			if(ButtonPushedWithVsync(1))
				goto STORYEND;
			if(++count > 6){
				count = 0;
				sprites[0].x++;
			}
		}
	}
	sprites[0].x = SCREEN_TILES_H*TILE_WIDTH;
	RamifyFromSD(11,0);
	WaitVsync(16);
	RamifyFromSD(12,0);


	for(uint8_t i=0;i<NUMFLYAWAYANIMFRAMES;i++){//wizard with lala then fly away
		RamifyFromSD(pgm_read_byte(&FlyAwayAnim[i]),0);
		if(ButtonPushedWithVsync(60))
			goto STORYEND;
	}

	for(uint8_t i=0;i<NUMMAGICDUSTFRAMES;i++){//throwing magic dust on lolo land
		RamifyFromSD(pgm_read_byte(&MagicDustAnim[i]),0);
		if(ButtonPushedWithVsync(1))
			goto STORYEND;
	}

	RamifyFromSD(6,0);//close up of lolos, turn them to stone
	bool found;
	while(found){
		found = false;
		for(uint16_t i=0;i<RAM_TILES_COUNT*64;i++){
			if(ram_tiles[i] == 0x4F || ram_tiles[i] == 0x11 || ram_tiles[i] == 0xC2){
				found = true;
				if(prng(7) == 2)
					ram_tiles[i] = 0xDE;//stone color
			}
		}
		if(ButtonPushedWithVsync(1))
			goto STORYEND;
	}

	FadeOut(1,true);
	FadeIn(1,false);

	for(uint8_t i=0;i<NUMZOOMLOLOFRAMES;i++){//zoom in, as Nick says "Lolo is pissed!"
		RamifyFromSD(pgm_read_byte(&ZoomLoloAnim[i]),0);
		if(ButtonPushedWithVsync(1))
			goto STORYEND;
	}

	FadeOut(1,true);
	FadeIn(1,false);
	RamifyFromSD(8,0);//draw castle scene

	uint8_t y=(SCREEN_TILES_V*8)-8;
	uint8_t frame;
	while(y > ((SCREEN_TILES_V-12)*8)){//lolo walk up to the door
		SetSprite((128),y,22,0,0);
		if(++frame > 2)
			frame = 0;
		y--;
		if(ButtonPushedWithVsync(1))
			goto STORYEND;
	}
	if(ButtonPushedWithVsync(60))//pause at door and place door entry sound effect
		goto STORYEND;
	TriggerFx(1,255,true);
	if(ButtonPushedWithVsync(30))
		goto STORYEND;

STORYEND:
	gui_state = GUITITLE;
	FadeOut(1,true);
	return false;
}


inline bool SecretMenu(){
	RamifyFromSD(12,RS_REMOUNT);
	gui_state = GUITITLE;
	return true;
}

inline bool NewGameIntro(){
	FadeOut(1,true);
	RamifyFromSD(4,RS_REMOUNT);
	FadeIn(1,true);
	return false;
}


const uint16_t SecretCode[] PROGMEM = {BTN_UP,BTN_UP,BTN_DOWN,BTN_DOWN,BTN_LEFT,BTN_RIGHT,BTN_LEFT,BTN_RIGHT,BTN_B,BTN_A};
#define NUMSECRETCHARS 10
inline bool Title(){
//0xb88 first byte of first tile
//1ac8 first byte of map
//	RamifyFromSD(1);



	RamifyFromSD(2,RS_REMOUNT);
	SetTileTable(TitleTiles);
	FadeIn(1,false);

	uint8_t cursorpos = 0;
	uint16_t idleticks = 0;
	uint8_t secretpos = 0;
	uint8_t yoff;
	while(true){
		prng(1);
		UpdatePad();
		if((secretpos < 255) && padstate && (padstate != oldpadstate)){
			if(padstate != pgm_read_word(&SecretCode[secretpos]))
				secretpos = 0;
			else if(++secretpos == NUMSECRETCHARS){//secret activated!
				TriggerFx(1,128,false);
				secretpos = 255;
			}
		}

		if(padstate & BTN_UP && !(oldpadstate & BTN_UP)){
			if(--cursorpos > 200)
				cursorpos = 2+(secretpos==255);
		}
		else if(padstate & BTN_DOWN && !(oldpadstate & BTN_DOWN)){
			if(++cursorpos > 2+(secretpos==255))
				cursorpos = 0;
		}
		else if(padstate & BTN_START){
			if(cursorpos == 0){//new
				gui_state = GUINEWGAME;
			}else if(cursorpos == 1){//continue;
				gui_state = GUILEVELSEL;
			}else if(cursorpos == 2){//editor

			}else{//secret!
				gui_state = GUISECRET;
			}
			FadeOut(1,true);
			return true;
		}
		yoff = 19-((secretpos==255)*2);
		DrawMenu(14,yoff,10,6+((secretpos==255)*2),61-RAM_TILES_COUNT);
		SetTile(15,yoff+1+(cursorpos*2),(61-RAM_TILES_COUNT)+183);
		LoloPrint(16,yoff+1,PSTR("NEW GAME"),0);
		LoloPrint(16,yoff+3,PSTR("CONTINUE"),0);
		LoloPrint(16,yoff+5,PSTR("EDITOR"),0);
		if(secretpos == 255)
			LoloPrint(16,yoff+7,PSTR("SECRETS"),0);

		if(padstate)
			idleticks = 0;
		else if(++idleticks > 480){//jump to story scene
			FadeOut(1,true);
			gui_state = GUISTORY;
			return true;
		}
		WaitVsync(1);
	}

	return true;
}

inline bool MainMenu(){
	return 0;
}

inline bool LevelSelect(){
	StopSong();
	StartSong(PasswordSong);
	lvl.current = 0;
	bool changed = true;
	bool locked = false;
	goto FIRSTLEVELSELECTTICK;

	while(true){
		UpdatePad();

		if(padstate & BTN_LEFT && !(oldpadstate & BTN_LEFT) && lvl.current > 0){
			lvl.current--;
			changed = true;
		}
		if(padstate & BTN_RIGHT && !(oldpadstate & BTN_RIGHT) && lvl.current < 255){
			lvl.current++;
			changed = true;
		}
		if(padstate & BTN_SL && !(oldpadstate & BTN_SL) && lvl.current){
			if(lvl.current < 10){
				lvl.current = 0;
				changed = true;
			}else{
				lvl.current -= 10;
				changed = true;
			}
		}
		if(padstate & BTN_SR && !(oldpadstate & BTN_SR) && lvl.current < 255){
			if(lvl.current > 255-10){
				lvl.current = 255;
				changed = true;
			}else{
				lvl.current += 10;
				changed = true;
			}
		}
		if(changed == true){//load up new level
			FadeOut(1,true);
FIRSTLEVELSELECTTICK:
			LoadLevel(lvl.current);
			locked = LevelLocked();
			DrawLevel();
			changed = false;
			FadeIn(1,false);
		}

		if(padstate & BTN_START && !(oldpadstate & BTN_START)){
			if(LevelLocked()){
				TriggerFx(2,255,true);
			}else{
				DrawLevel();
				TriggerFx(20,255,true);
				gui_state = 0;
				return 0;
			}
		}
		
		AnimateLevel();
		if(locked){
			DrawMenu(10,11,7,2,0);
			LoloPrint(11,12,PSTR("LOCKED"),0);
		}

		WaitVsync(1);
	}
}

inline bool FloorIntro(){

	//StartSong(FloorIntroSong);
	RamifyFromSD(1,RS_REMOUNT);
//	DrawScreenFromSD(1);
	return 0;
}

inline bool EndGameSequence(){
	return 0;
}

	const uint8_t SoundTestFrameList[] PROGMEM = {0+2,1+2,2+2,3+2,2+2,1+2,};
inline void SoundTest(){


	uint8_t cursorpos = 0,sfx = 0, bgm = 0, frame = 0, t;
	while(true){
		RamifyFromSD(pgm_read_byte(&SoundTestFrameList[frame]),RS_REMOUNT);
		t = vram[(16*VRAM_TILES_H)+9];//get cursor tile
		if(++frame > 5){
			frame = 0;
		}
		for(uint8_t i=0;i<4;i++){
		UpdatePad();
		if(padstate & BTN_UP && !(oldpadstate & BTN_UP)){
			if(--cursorpos > 254)//rollover
				cursorpos = 2;
		}else if(padstate & BTN_DOWN && !(oldpadstate & BTN_DOWN)){
			if(++cursorpos > 2)
				cursorpos = 0;
		}
		if(padstate & BTN_LEFT && !(oldpadstate & BTN_LEFT)){
			if(cursorpos == 0){
				if(sfx)
					sfx--;
			}else if(cursorpos == 1){
				if(bgm)
					bgm--;
			}
		}else if(padstate & BTN_RIGHT && !(oldpadstate & BTN_RIGHT)){
			if(cursorpos == 0){
				if(sfx < 10)
					sfx++;
			}else if(cursorpos == 1){
				if(bgm < NUMSONGS-1)
					bgm++;
			}
		}
		if(padstate & BTN_B && !(oldpadstate & BTN_B)){
			if(cursorpos == 0)//play effect
				TriggerFx(sfx+12,255,true);
			else if(cursorpos == 1)//play music
				StartSong(LoloMusic[bgm]);
		}


		LoloPrintNum(8+7,16,sfx);
		LoloPrintNum(8+7,18,bgm);


		for(uint8_t j=0;j<6;j++)
			vram[((16+j)*VRAM_TILES_H)+9] = 0;//blank out heart cursor
		vram[(16*VRAM_TILES_H)+9+(cursorpos*2*VRAM_TILES_H)] = t;
//		SetTile(11,14+i,0);
//	SetTile(11,14+(cursorpos*2),1);//draw cursor
	WaitVsync(1);
		if(padstate & BTN_START && !(oldpadstate & BTN_START) && cursorpos == 2)//quit
			break;
			}//for

	}
	StopSong();
	FadeOut(1,true);
	FadeIn(1,false);
}


void Intro(){
	
	DDRC = 0;
	RamifyFromSD(0,0);//D3thAdd3r logo
	FadeIn(6,false);
	WaitVsync(160);
	FadeOut(2,true);
	FadeIn(2,false);
	for(uint8_t i=0;i<5;i++){
		for(uint8_t j=1;j<5;j++){
			RamifyFromSD(j,0);//multi colored uzebox logo animation
			WaitVsync(4);
		}
	}
	RamifyFromSD(1,0);
	WaitVsync(60);
	FadeOut(6,true);
	FadeIn(2,false);

}
