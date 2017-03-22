inline void DemoFillPad(){
	oldpadstate[0] = padstate[0] = 0;
		/*
	if(demo_inp_wait && demo_inp_wait != 255){
		demo_inp_wait--;
		return;
	}

	if((piecey[0] == 0) && demo_inp_wait == 255){//NEED NEW MOVE
		demo_inp_wait = pgm_read_byte(&DemoData[demo_off++]);
		demo_decision = pgm_read_byte(&DemoData[demo_off++]);
		return;
	}
ColumnsPrint2num(14,14,demo_off);
	*/
	if((demo_decision/3) < piecex[0])
		padstate[0] = BTN_LEFT;
	else if((demo_decision/3) > piecex[0])
		padstate[0] = BTN_RIGHT;
	else
		padstate[0] = BTN_DOWN;
			
	if(score[0] > 9)
		padstate[0] |= BTN_SL;
}

void UpdatePads(){
	//ReadControllers();
	for(uint8_t i=0;i<MAX_PLAYERS;i++){
		oldpadstate[i] = padstate[i];
		padstate[i] = ReadJoypad(i);
		if((i == 1) && (game_mode == 0)){//cpu will take care of this pad
			CpuFillPad();
		}			
		else{		
			if(game_state & GS_DEMO)//demo mode fills in for player 1
				DemoFillPad();
		}
	}
}
uint8_t countsss[2];
inline void ProcessInputState(uint8_t p){
//Works for SNES or NES pads without recompiling. Need to test that on real hardware.
	//#if JOYSTICKTYPE==TYPE_SNES

#ifdef DEBUG
if(state[0] & GETREADY)
	padstate[0] = BTN_START;
#endif
countsss[p]++;
//for(uint8_t i=0;i<2;i++)
//ColumnsPrint3num(14,9+i,countsss[i]);

	if(StartDown(p)){
		if(state[0]&LOSING || state[1]&LOSING)
			return;
			
		if(game_state & GS_DEMO){
			game_state = GS_TITLE;
			return;
		}
		
		if(state[p] & GETREADY || state[p] == GOAHEAD){
			state[p] = GOAHEAD;
			state[!p] = GOAHEAD;
			return;
		}

		if(state[p] & GAMEOVER){
			game_state = INITIALSTATE;
			for(uint8_t i=0;i<MAX_PLAYERS;i++)
				state[i] = GETREADY;
				//GeneratePreview(i);
				//level = !level;
				FadeOut(1,true);
				FadeIn(2,false);
				//DoSong();

			return;
		}

		if(state[p] & PAUSED){
			if(cursorpos == 0){//BACK
				state[p] ^= PAUSED;
				state[!p] |= PAUSED;
				state[!p] ^= PAUSED;
			}
			else{
				game_state = GS_TITLE;
				return;
			}
		}
		else{
			cursorpos = 0;
			state[p] |= PAUSED;
			state[!p] |= PAUSED;
			return;
		}

	//if(debug){
		//piece[(p*3)+0] = 7;piece[(p*3)+1]=8;piece[(p*3)+2]=9;//make magic jewel
		score[p] = 30;
		cursetype[1] = FLASHINGJEWELCOMING;
		SummonMagicJewel(p);

	//}
		return;
	}

	if(state[p] & (GAMEOVER|GETREADY))
		return;
		
	if((state[p] & PAUSED)){
		if(!p && (UpDown(0) || DownDown(0)))
			cursorpos = !cursorpos;
		return;
	}

	if(!(cursetype[p]&CURSENOROTATE) && YDown(p)){
		//TriggerPCM(SWAPWAVE,255,255);
		RotatePiece(p);
		//CrashBarUp(3,0);
	}

	if(BDown(p) || RSDown(p) || LSDown(p) || (p && score[1] > 9)){
		Attack(!p);
	}

	uint16_t oldp = padstate[p];
	if((cursetype[p]&CURSESWAPCONTROLS)){
		if(padstate[p] & BTN_LEFT){
			padstate[p] ^= BTN_LEFT;
			padstate[p] |= BTN_RIGHT;
		}
		else if(padstate[p] & BTN_RIGHT){
			padstate[p] ^= BTN_RIGHT;
			padstate[p] |= BTN_LEFT;
		}
	}
	if(padstate[p] & BTN_LEFT){
		if(++sideheld[p] == 1 || sideheld[p] == 5 || sideheld[p] > 8)
			MovePieceLeft(p);
	}
	else if(padstate[p] & BTN_RIGHT){
		if(++sideheld[p] == 1 || sideheld[p] == 5 || sideheld[p] > 8)
			MovePieceRight(p);
	}
	else
		sideheld[p] = 0;
		
	padstate[p] = oldp;//undo curse control changes

	if(DownHeld(p)){
		//if(downletup[p])
			droptimer[p] = 0;
#ifdef DEBUG
	DropPiece(p);
	cpustate = CPUFIRSTTICK;
#endif
		//downletup[p] = 0;
	}
	//else
		//downletup[p] = 1;
	
	//if(UpDown(p)){
		//DropPiece(p);
	//}
}
