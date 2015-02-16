void DrawJewel(uint8_t x, uint8_t y, uint8_t j){//for preview and intro, don't inline and simpler case
	uint16_t off = x+(y*VRAM_TILES_H);
	j &= 15;
	if(j > 6){//magic jewel
		j -= 7;
		j = FIRSTMAGICJEWELRT+(j*4);
	}else
		j = RAM_TILES_COUNT+(j*4)+JEWELSTART-1;

	vram[off+0] = j++;//set_vram(x+0,y+0,t++);
	vram[off+1] = j++;//set_vram(x+1,y+0,t++);
	vram[off+VRAM_TILES_H+0] = j++;//set_vram(x+0,y+1,t++);
	vram[off+VRAM_TILES_H+1] = j;//set_vram(x+1,y+1,t);
}

inline void InlineDrawJewelUpsidedown(uint8_t x, int16_t y, uint8_t t, uint8_t p){
	uint16_t off = x+(y*VRAM_TILES_H);
	if(t == 0){
		t = FIRSTGRIDRT+(p*3);
		vram[off+0] = t++;
		vram[off+1] = t++;
		vram[off+VRAM_TILES_H+0] = t;
		vram[off+VRAM_TILES_H+1] = RAM_TILES_COUNT;
		return;
	}

	if(t & HALFSTEP){//is a half step down, so draw y+1
	//BLANK ABOVE THIS JEWEL
	if(y < 25){
		vram[off+VRAM_TILES_H+0] = FIRSTGRIDRT+(p*3)+2;
		vram[off+VRAM_TILES_H+1] = RAM_TILES_COUNT;//FIRSTGRIDRT+(p*3)+3;
	}
	off -= VRAM_TILES_H;
	y--;
}

if(y > 25)
return;

t &= 0b00011111;//get rid of status bits

if(t>19){//crash bar
t -= 20;
t *= 4;
t += FIRSTCRASHRT;
vram[off+0] = t++;//set_vram(x+0,y+0,t++);
vram[off+1] = t++;//set_vram(x+1,y+0,t++);
vram[off+VRAM_TILES_H+0] = t++;//set_vram(x+0,y+1,t++);
vram[off+VRAM_TILES_H+1] = t++;//set_vram(x+1,y+1,t);
return;

	}
	if(t>9){//explosion
		t = FIRSTEXPLOSIONRT+(p*4);
vram[off+0] = t++;//set_vram(x+0,y+0,t++);
vram[off+1] = t++;//set_vram(x+1,y+0,t++);
vram[off+VRAM_TILES_H+0] = t++;//set_vram(x+0,y+1,t++);
vram[off+VRAM_TILES_H+1] = t;//set_vram(x+1,y+1,t);
		return;
	}
	if(t>6){//flashing jewel
		t -= 7;
		if(t == 0){t=2;}else//make them look right for up and down arrows
		if(t == 2){t=0;}
		t *= 4;

		if(y > -1){
			vram[off+0] = t+0;//set_vram(x+0,y+0,t+0);//set to ram tile indices
			vram[off+1] = t+1;//set_vram(x+1,y+0,t+1);
		}
		vram[off+VRAM_TILES_H+0] = t+2;//set_vram(x+0,y+1,t+2);
		vram[off+VRAM_TILES_H+1] = t+3;//set_vram(x+1,y+1,t+3);	
		return;
	}else{//normal jewel
		t--;
		t *= 4;
		t += 3;
	}
	t += JEWELSTART+RAM_TILES_COUNT;
	if(y <= 25){
vram[off+0] = t++;//set_vram(x+0,y+0,t++);
vram[off+1] = t++;//set_vram(x+1,y+0,t++);
	}
	else
		t+= 2;
vram[off+VRAM_TILES_H+0] = t++;//set_vram(x+0,y+1,t++);
vram[off+VRAM_TILES_H+1] = t;//set_vram(x+1,y+1,t);
}

inline void InlineDrawJewel(uint8_t x, int16_t y, uint8_t t, uint8_t p){
	uint16_t off = x+(y*VRAM_TILES_H);

	if(t == 0){
		t = FIRSTGRIDRT+(p*3);
		vram[off+0] = t++;
		vram[off+1] = t++;
		vram[off+VRAM_TILES_H+0] = t;
		vram[off+VRAM_TILES_H+1] = RAM_TILES_COUNT;
		return;
	}

	if(t & HALFSTEP){//is a half step down, so draw y+1
		//BLANK ABOVE THIS JEWEL
		if(y > -1){
			vram[off+0] = FIRSTGRIDRT+(p*3);
			vram[off+1] = FIRSTGRIDRT+(p*3)+1;
		}
		off += VRAM_TILES_H;
		y++;		
	}

	if(y < -1)
		return;

t &= 0b00011111;//get rid of status bits	

if(t>19){//crash bar
t -= 20;
t *= 4;
t += FIRSTCRASHRT;
vram[off+0] = t++;//set_vram(x+0,y+0,t++);
vram[off+1] = t++;//set_vram(x+1,y+0,t++);
vram[off+VRAM_TILES_H+0] = t++;//set_vram(x+0,y+1,t++);
vram[off+VRAM_TILES_H+1] = t++;//set_vram(x+1,y+1,t);
return;

	}
	if(t>9){//explosion
		t = FIRSTEXPLOSIONRT+(p*4);
vram[off+0] = t++;//set_vram(x+0,y+0,t++);
vram[off+1] = t++;//set_vram(x+1,y+0,t++);
vram[off+VRAM_TILES_H+0] = t++;//set_vram(x+0,y+1,t++);
vram[off+VRAM_TILES_H+1] = t;//set_vram(x+1,y+1,t);
		return;
	}
	if(t>6){//flashing jewel
		t -= 7;
		t *= 4;

		if(y > -1){
			vram[off+0] = t+0;//set_vram(x+0,y+0,t+0);//set to ram tile indices
			vram[off+1] = t+1;//set_vram(x+1,y+0,t+1);
		}
		vram[off+VRAM_TILES_H+0] = t+2;//set_vram(x+0,y+1,t+2);
		vram[off+VRAM_TILES_H+1] = t+3;//set_vram(x+1,y+1,t+3);	
		return;
	}else{//normal jewel
		t--;
		t *= 4;
		t += 3;
	}
	t += JEWELSTART+RAM_TILES_COUNT;
	if(y >= 0){
vram[off+0] = t++;//set_vram(x+0,y+0,t++);
vram[off+1] = t++;//set_vram(x+1,y+0,t++);
	}
	else
		t+= 2;
vram[off+VRAM_TILES_H+0] = t++;//set_vram(x+0,y+1,t++);
vram[off+VRAM_TILES_H+1] = t;//set_vram(x+1,y+1,t);
	
}






inline void BlankDrawWell(uint8_t p){
	//Draw blank grid
	uint8_t ox = p*16;
	for(uint8_t x=1+ox;x<13+ox;x++)
	for(uint8_t y=0;y<26;y++){
		set_vram(x,y,RAM_TILES_COUNT);
	}
}

inline void OneTimeDraw(){
	ColumnsDrawMap(13,7,4,26,0,FieldMap+(19*4*level),0);//draw even if just crashredrawtime
	for(uint8_t i=0;i<19;i++){
		vram[(i*VRAM_TILES_H)+0] = vram[((i+7)*VRAM_TILES_H)+16];
		vram[(i*VRAM_TILES_H)+29] = vram[((i+7)*VRAM_TILES_H)+13];
	}		
	for(uint8_t i=19;i<26;i++){
		vram[(i*VRAM_TILES_H)+0] = vram[((i-12)*VRAM_TILES_H)+16];
		vram[(i*VRAM_TILES_H)+29] = vram[((i-12)*VRAM_TILES_H)+13];
	}

	
if(game_state & GS_FIRSTTICK){
		//BlankDrawWell(0);
		//BlankDrawWell(1);
	}
}



inline void StateDraw(uint8_t p){
		uint8_t ox = (p*16)+1;

		if(state[p] == GETREADY){
			if(p == 0){
				uint8_t color = pgm_read_byte(&GetReadyPallet[(level*(GLOBALFLASHCOUNT+1))+(globalflashcounter)]);
				rtl_RamifyFontEx(0,readycharmap,charlist,compfont,0,color,false);
			 }
			 rtl_Print(ox+3,6,PSTR("READY!"));
			return;
		}

		if(state[p] & PAUSED){
		//	for(uint8_t y=0;y<7;y++){//blank preview(done in dodraw())
			//	set_tile(13+(p*2),y,0);
		//		set_tile(14+(p*2),y,0);
		//	}
			BlankDrawWell(p);
			if(p == 0){

				rtl_RamifyFontEx(0,pausecharmap,charlist,compfont,0,255,true);
				rtl_Print(3,13+cursorpos,PSTR(">"));
				rtl_Print(4,13,PSTR("RESUME"));
				rtl_Print(4,14,PSTR("QUIT"));
				ramtilestate[0] = NEEDALLRAMMED;//MAKE SURE EVERYTHING IS SET BACK UP AFTER PAUSE
			}				
			if(globalflashcounter < (GLOBALFLASHCOUNT/2)){
				rtl_Print(ox+3,10,PSTR("PAUSE"));
			}


		/*	for(uint8_t i=0;i<6;i++){//blank out preview
				set_tile(13+(p*2),i,10);
				set_tile(14+(p*2),i,10);
			}
			continue;
		*/
		return;
		}
		
		
		if(state[p] & (GAMEOVER|LOSING2)){
			if(globalflashcounter < (GLOBALFLASHCOUNT)/2 && crashheight[p] < 8){
				if(p == winner){
					rtl_Print(ox+2,10,PSTR("WINNER!!"));
				}
				else if(winner == 255){//draw
					rtl_Print(ox+3,10,PSTR("DRAW!!"));
				}
				else{
				//	if(!game_mode && ox>10){//cpu spot
					//	rtl_Print(ox+1,10,PSTR("CPU FAIL!!"));
				//	}
					//else
						rtl_Print(ox+1,10,PSTR("TRY AGAIN!"));
				}
			}
			if(p == winner && !game_mode){
				if(!p){//only draw this for human player, or never in 2 player
					if(cpuskill < 10){
						if(crashheight[p] < 3){
							rtl_Print(ox+2,20,PSTR("NOW TRY"));
							rtl_Print(ox+2,21,PSTR("LEVEL"));
							ColumnsPrint1num(ox+8,21,cpuskill);
						}					
					}else{
						if(crashheight[0] < 3)
							rtl_Print(ox+1,20,PSTR("INCREDIBLE!"));
						//rtl_Print(ox+1,20,PSTR(""));//rtl_Print(ox+1,20,PSTR("YOU ARE THE"));
						//rtl_Print(ox+1,21,PSTR("COLUMNS"));
						//rtl_Print(ox+1,22,PSTR("MASTER!!!"));
					}
				}
			}
			else if(game_mode == 0 && p == 0){
				if(winner != 255 && crashheight[p] < 3){
					rtl_Print(ox+2,20,PSTR("BACK TO"));
					rtl_Print(ox+2,21,PSTR("LEVEL"));
					ColumnsPrint1num(ox+8,21,cpuskill);
				}
			}
			
		/*	for(uint8_t i=0;i<6;i++){//blank out preview
				vram[13+(p*2)+(i*VRAM_TILES_H)] = RAM_TILES_COUNT+10;
				vram[14+(p*2)+(i*VRAM_TILES_H)] = RAM_TILES_COUNT+10;
				//set_tile(13+(p*2),i,10);
				//set_tile(14+(p*2),i,10);
			}*/

		}
}



inline void DoDraw(){
//need to redraw preview jewels when gameover because crashedtiles overdraw them
	//Draw field map
	if(game_state & GS_FIRSTTICK || crashedredrawtime)// || state[0] & (LOSING|LOSING2|GAMEOVER))
		OneTimeDraw();

	//Draw well
	for(uint8_t p=0;p<MAX_PLAYERS;p++){
		//if(state[p] == GAMEOVER)
		//	continue;

		uint8_t ox = (p*16)+1;
		uint8_t oy = 0;




		uint16_t woffset = (p*WELLSIZE)+WELLSIZE-1;//start at last jewel spot
		uint8_t t;
	//	ox = p*18;//not needed
		//Draw actual well
		if(cursetype[p]&CURSEUPSIDEDOWN){
					//if(cursetype[p] & CURSEUPSIDEDOWN)
						//InlineDrawJewelUpsidedown(ox+(x*2),24-((y-3)*2),t,p);	
		}
		else{
		for(int8_t y=15;y>1;y--){
			for(int8_t x=5;x>-1;x--){
				if((state[p]&SCORING) && (stateframe[p]&1) && get_scoring(x,y,p))				
					t = 0;//flash on and off
				else if((state[p]&EXPLODING) && (well[woffset]&SCORINGJEWEL))
					t = 10+stateframe[p];//>10 IS EXPLODING FRAMES
				else{
					t = well[woffset];
					if((t & FLASHINGJEWEL) && (globalflashcounter&4))
						t = 0;
				}

				InlineDrawJewel(ox+(x*2),((y-3)*2),t,p);
				
				
				
				if(woffset == (p*WELLSIZE)){
					goto wellend;
					//y = -33;
					//break;
				}
				woffset--;
			}
		}
		}		
wellend:
		ox = 13+(p*2);

		StateDraw(p);
		if(!(state[p] & (LOSING|LOSING2|GAMEOVER|PAUSED))){

			if(cursetype[p] != CURSENOPREVIEW){
				for(uint8_t i=0;i<3;i++){
					if(!(preview[(p*3)+i] & FLASHINGJEWEL) || (globalflashcounter&4))
						t = preview[(p*3)+i];
					else
						t = 0;
					
					DrawJewel(ox,oy+(i<<1),t);
				}				
			}
			else{
				uint8_t off = ox+(oy*VRAM_TILES_H);
				for(uint8_t i=0;i<6;i++){
					vram[off++] = RAM_TILES_COUNT;
					vram[off] = RAM_TILES_COUNT;
					off += VRAM_TILES_H-1;
				}
			}				
					
			//draw score
			ColumnsPrint2num(14+(p*2),6,score[p]);

		}
		else{
			for(uint8_t i=13+0;i<13+4;i++)
			for(uint8_t j=0;j<7;j++)
				set_tile(i,j,0);
			
		}
	

	}
#ifdef DEBUG
//for(uint8_t i=0;i<24;i++)
//ColumnsPrint2num(14,i,evalweights[i]);
ColumnsPrint3num(15,25,jewels[1]);
#endif
}