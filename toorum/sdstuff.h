void SD_Crash(uint8_t step, uint8_t error){

	DDRC = 255;
	DrawMenuBG();
	drawText(9,10,PSTR("SD ERROR- ."));
	SetTile(9+9,10,step+33);
	SetTile(9+11,10,error+33);

	drawText(5,18,		PSTR("PRESS START TO RESET"));

	if(error == 0)
		drawText(3,12,	PSTR("CANNOT OPEN TOORUMSQ.DAT"));
	else if(error == 1)
		drawText(1,12,	PSTR("FAILED TO READ EPISODES INDEX"));
	else if(error == 2)
		drawText(5,12,	PSTR("FAILED TO READ LEVEL"));
	else if(error == 3)
		drawText(5,12,	PSTR("LEVEL DATA IS CORRUPT"));
	else if(error == 4)
		drawText(3,12,	PSTR("NO EDITOR LEVEL AVAILABLE"));
	else
		drawText(5,12,	PSTR("FAILED TO WRITE LEVEL"));

	uint8_t count = 0;

	while(1){
		WaitVsync(1);
		if(controllerState & BTN_START && !(prevControllerState & BTN_START))
			SoftReset();
		if(++count > 15)
			SetLedOn();
		else
			SetLedOff();
		if(count > 30)
			count = 0;
	}
}

uint8_t SD_OpenFile(char *file){

	FRESULT	res;
	
	for(uint8_t i=0;i<SD_MAX_OPEN_ATTEMPTS;i++){

		pf_mount(NULL);
		res = pf_mount(&fs);
		res |= pf_open(file);

		if(!res)
			break;

		if(i == SD_MAX_OPEN_ATTEMPTS-1)
			return 1;//cannot open file, doesn't exist?
		WaitVsync(4);
	}
	return 0;
}

uint8_t SD_WriteHighScore(uint32_t block, uint32_t offset, uint32_t len, void *data){//user must first pf_mount(), must pass 512 byte buffer

//	if(block < 8)
		//return 1;//block is reserved for kernel use
	FRESULT	res = 0;
	WORD	bt;

	if(SD_OpenFile(SD_SAVE_FILE_NAME))
		return 1;//couldn't open file

	pf_lseek((uint32_t)(block*512UL));//go to the first block byte

	if(offset || offset+len != 512UL){//copy the existing data so we don't corrupt it
		if(offset)
			res |= pf_read((BYTE *)data,offset,&bt);//read existing inset bytes
		if(offset+len < 512UL)
			res |= pf_read((BYTE *)data+(offset+len),(512UL-(offset+len)),&bt);//read existing bytes that will follow our data
	}//else overwrite entire block

	res |= 	pf_write((BYTE *)data,512UL,&bt);//write the aggregated score data
	pf_write(0,0,0);//finalize write operation

	if(res || bt != 512UL)
		return 2;//something strange happened, write past file boundary?

	return 0;//write success
}

uint8_t SD_ReadHighScoreTable(){//file must already be open, fill first 512 bytes of ram_tiles with high score data

	FRESULT	res;
	WORD	bt;

	pf_lseek((uint32_t)(SD_TQ_HIGH_SCORE_OFFSET));//(slot_no*512)
	res = pf_read((BYTE *)ram_tiles,512UL,&bt);

	if(bt != 512 || res)
		return 1;
	return 0;
}


uint8_t SD_WriteHighScoreTable(){//file must already be open, write the first 512 bytes of ram_tiles to the sector

	FRESULT	res;
	WORD	bt;

	pf_lseek((uint32_t)(SD_TQ_HIGH_SCORE_OFFSET));//(slot_no*512)
	res = pf_write((BYTE *)ram_tiles,512UL,&bt);
	res |= pf_write(0,0,0);//finalize write
	
	if(bt != 512 || res)
		return 1;
	return 0;
}

void EditorWriteLevel(uint8_t episode, uint8_t level){

	uint16_t voff = ((WINDOW_Y_OFF+2)*VRAM_TILES_H)+WINDOW_X_OFF;
	uint16_t roff = (level&3)*SD_ROOM_SIZE;//4 levels are stored per 512 byte sector
//	uint32_t soff;

	for(uint8_t i=0;i<NUM_TILES_Y-1;i++){
		for(uint8_t j=0;j<NUM_TILES_X;j++){
			
			uint8_t tile = vram[voff];
			if(tile > TILE_GHOST_MOVE_LEFT){


			}else{//normal map tile
				ram_tiles[roff++] = pgm_read_byte(&editor_save_conversion_table[tile]);
			}

			voff += 2;
		}
		voff += VRAM_TILES_H+VRAM_TILES_H-((NUM_TILES_X+1)*2)+2;
	}

}

void SD_Startup(){

	WORD br;
	//initialize our high score slot in the universal SD high score file, if it isn't already
	if(SD_OpenFile(SD_SAVE_FILE_NAME)){

		DrawMenuBG();
		FadeOut(0,true);
		FadeIn(2,false);
		drawText(4,10,PSTR("- NO HIGH SCORE FILE -"));
		drawText(4,11,PSTR("SCORES CANNOT BE SAVED"));
		WaitVsync(180);
		FadeOut(2,true);
		ClearVram();

	}else{

		pf_lseek((uint32_t)(SD_TQ_HIGH_SCORE_OFFSET));
		pf_read((BYTE *)ram_tiles,SD_SECTOR_SIZE,&br);//load up the whole table for all episodes
	
		if(ram_tiles[510] == 'T' && ram_tiles[511] == 'Q'){//already initialized?


		}else{
			for(uint8_t i=0;i<SD_MAX_EPISODES;i++){//put the default table in place so there is something to show
				uint32_t soff = i*SD_HIGH_SCORE_ENTRY_SIZE*SD_MAX_HIGH_SCORE_ENTRIES;
				for(uint8_t j=0;j<sizeof(default_high_score_data);j++)
					ram_tiles[soff+j] = pgm_read_byte(&default_high_score_data[j]);

			}
	
			ram_tiles[510] = 'T';//initialize it
			ram_tiles[511] = 'Q';

			SD_WriteHighScoreTable();

			WaitVsync(1);
		}
	}

	if(SD_OpenFile(SD_DATA_FILE_NAME))
		SD_Crash(0,0);

}
void SD_ChooseEpisode(bool editor_only){//initializes SD card and loads episode. if more than 1 episode is available, then user is give a choice menu
//return;
	DDRC = 0;
	sd_episode = 0;
	hideSprites(0,MAX_SPRITES);//todo weird weird SD reading problem....
	DrawMenuBG();
//	SetRenderingParameters(FIRST_RENDER_LINE,2);

	uint8_t num_episodes_available = 0;
	FRESULT res;
	WORD	br;
	uint8_t cursorpos = 0;



	res =	pf_lseek((uint32_t)(SD_EPISODE_HEADER_OFFSET));
	res |=	pf_read((BYTE *)ram_tiles,SD_EPISODE_HEADER_SIZE,&br);



	drawText(5,4,PSTR("-SELECT EPISODE-"));


//	res =	pf_lseek(0);
//	res |=	pf_read((BYTE *)ram_tiles,SD_EPISODE_HEADER_SIZE,&br);//WHY IN THE FUCK DOES THIS WORK, THEN CHANGE UNRELATED CODE AND IT DOESN'T?!?!?!

	bool found_editor_slot = false;

	for(uint8_t i=0;i<SD_MAX_EPISODES;i++){//print all names for available episodes
		
		if(!ram_tiles[(i*32)])//no name, no episodes can then follow
			break;

		num_episodes_available++;
		if(ram_tiles[(i*32)] == '.')
			found_editor_slot = true;

		if(editor_only && ram_tiles[(i*32)] != '.'){
			drawText(5,7+(i<<1),PSTR("-SLOT NOT WRITABLE-"));
			if(cursorpos == i)
				cursorpos++;
		}else
			drawTextRam(5,7+(i<<1),ram_tiles+(i*32UL));
	}

	if(num_episodes_available == 0)//can only use episode from flash, SD disabled
		SD_Crash(2,3);//mounted it OK, no read error, but no data here??

	if(editor_only && !found_editor_slot)
		SD_Crash(5,4);

	DDRC = 255;
	while(true){
		WaitVsync(1);

		if(controllerState & BTN_START && !(prevControllerState & BTN_START)){
			if(editor_only && ram_tiles[(cursorpos*32)] != '.'){
				TQ_triggerFx(SOUND_HURT);
				
			}else{
				sd_episode = cursorpos;

				episode_base_time_limit		= ram_tiles[(sd_episode*32)+30]<<8UL;
				episode_base_time_limit		|= ram_tiles[(sd_episode*32)+31];
				//episode_base_time_limit = (uint16_t)300UL;
				break;
			}

		}

		if(controllerState & BTN_UP && !(prevControllerState & BTN_UP)){
			if(--cursorpos >= num_episodes_available)//rollover
				cursorpos = num_episodes_available-1;
		}else if(controllerState & BTN_DOWN && !(prevControllerState & BTN_DOWN)){
			if(++cursorpos >= num_episodes_available)
				cursorpos = 0;
		}
		
		for(uint8_t i=7-1;i<SCREEN_TILES_V-4;i++){
			SetTile(3,i,FIRST_MAP_TILE);
			SetTile(4,i,FIRST_MAP_TILE);
		}
		SetMapTile(3,7+(cursorpos<<1)-1,TILE_KEY);

	}

	FadeOut(1,true);
	ClearVram();
	FadeIn(3,false);
}
