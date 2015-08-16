//#define SNES_MOUSE 1
#include <avr/pgmspace.h>
#include <string.h>
#include <uzebox.h>
#include <stdint.h>

#include "data/patches.inc"
#include "data/graphics.inc"
#include "data/music.inc"
#include "data/title.inc"

#include "defines.h"
#include "gui.h"

int main(){
	InitMusicPlayer(patches);
	SetSpritesTileTable(GameSprites);
	SetTileTable(GameTiles);
	SetFontTilesIndex(FONTSTART);
	SetMasterVolume(200);
//	EnableSnesMouse(0,NULL);
	FadeIn(1,false);

#ifdef DEBUGGING
	flags |= DEBUG;
#endif
	music = 1;
	mx = my = 128;
	xoff = yoff = 0;

	for(uint8_t i=0;i<MAX_SPRITES;i++)
		sprites[i].x = SCREEN_TILES_H*8;
	WaitVsync(1);	

	skill = 1;
	customwidth = 12;//MAXWIDTH;
	customheight = 10;//MAXHEIGHT;
	custommines = 25;
	flags = SOUND | MARKS;

STACKTOP:
	MainMenu();
	NewGame();
	MainLoop();
	goto STACKTOP;

	return 0;
}



void UpdateTimer(){
	if(!(flags & STARTED) || flags & LOST)
		return;

	if(++fracs > 60){
		fracs = 0;
		if(++seconds > 999)
			seconds = 999;
	}
}

void ResetBoard(){
	FadeOut(1,true);
	FadeIn(1,false);
	seconds=fracs=0;
	NewGame();
}

void MainLoop(){

	while(true){
	Input();
	if(flags & GUIOPEN){//don't use click that got us out of the gui
		if(mlb || mlbwd || mrb || mrbwd){
			Draw();
		 WaitVsync(1);
			continue;
		}
	else{
			flags ^= GUIOPEN;
			WaitVsync(1);
		 continue;
		}
	}

	if(mx < 8+(xoff*8) || my < 57+(yoff*8)+(yoff*8*!(flags & BIGFIELD)) ||
		(flags & BIGFIELD && (mx > (fwidth*8)+7+(xoff*8) || my > (fheight*8)+56+(yoff*8))) ||
		(!(flags & BIGFIELD) && (mx > (fwidth*16)+7+(xoff*16) || my > (fheight*16)+56+(yoff*16))))
		mgridx = mgridy = 255;//mark grid coords invalid
	else{
		if(flags & BIGFIELD){
			mgridx = ((mx-(xoff*8))/8)-1;
			mgridy = ((my-(yoff*8))/8)-7;
		}
		else{
			mgridx = ((mx-(xoff*16))/8)-1;
			mgridy = ((my-(yoff*16))/8)-7;
		}
	}

	if(!(flags & BIGFIELD) && mgridx != 255){
		mgridx /= 2;
		mgridy /= 2;
	}
	
	if(flags & DEBUG){
		print(8,2,PSTR("X"));
		print(8,3,PSTR("Y"));
		print(8,4,PSTR("G"));
		print(17,2,PSTR("V"));
		printnum(20,2,GetVidGrid(mgridx,mgridy),true);
		if(mgridx == 255){
			for(uint8_t i=0;i<3;i++)
				print(9,2+i,PSTR("NULL"));
			
		 print(17,2,PSTR("NULL"));
		}
		else{
			for(uint8_t i=0;i<3;i++)
				print(9,2+i,PSTR("	 "));
			
		 printnum(11,2,mgridx,true);
			printnum(11,3,mgridy,true);
			printnum(11,4,GetGrid(mgridx,mgridy),true);
		}

		if(flags & WON){
			print(0,0,PSTR("WON"));
		}
	}

	if(mlb && !mlbwd && my < 16+25){//mouse is on hud
		if(mx > 107 && mx < 107+24 && my > 15 && my < 16+25){//mouse is on face
			if(flags & LOST){
			 DrawFace(108,16,3);
			WaitVsync(1);
			do{
				Input();
				WaitVsync(1);
				}while(mlb);
				ResetBoard();
		 }
			else if(flags & WON)
			 ResetBoard();
			else
				InGameMenu();

			continue;
		}
		WaitVsync(1);
		continue;
	}
	
	if(flags & LOST){
		Draw();
		WaitVsync(1);
		continue;
	}
	else if(flags & WON){
		Draw();
		WaitVsync(1);
		continue;
	}
	else if(mlbwd && !mlb && mgridx != 255){//player released left mouse button, apply logic
		uint8_t grid = GetGrid(mgridx,mgridy);

		if(grid == MINE){//lose
			if(!(flags & STARTED)){//dont allow a loss on the first turn
				do{
					WaitVsync(1);
					InitField();
				}while(GetGrid(mgridx,mgridy)==MINE);
				WaitVsync(1);
				FloodFill(mgridx,mgridy);
		 	}
			else{
				flags |= LOST;
				Loss();
			}
		}
		else if(GetVidGrid(mgridx,mgridy) == 0){//unclicked spot
			FloodFill(mgridx,mgridy);
			if(CheckVictory()){
				Win();
				continue;
			}
		}

		flags |= STARTED;
		
	}
	else if(mrb && !mrbwd && mgridx != 255){//player pressed right mouse button

		uint8_t mark = GetVidGrid(mgridx,mgridy);

		if(mark < 1){//unexplored
			SetVidGrid(mgridx,mgridy,FLAG);
			numflags++;
		}else if(mark == FLAG){//flag
			if(flags & MARKS)
			 SetVidGrid(mgridx,mgridy,QUESTION);
			else
			 SetVidGrid(mgridx,mgridy,0);
		 numflags--;
		}
		else if(mark == QUESTION)
			SetVidGrid(mgridx,mgridy,0);
		
		if(CheckVictory()){
			Win();
		 continue;
		}
			
	}
	
	UpdateTimer();	
	prand();
	
	Draw();
	WaitVsync(1);

	}
}

void Draw(){
	DrawTimer();
	DrawFace(108,16,255);
	DrawMineCount();
	DrawCursor();
}

void NewGame(){
	if(skill == 0){
		fwidth = 9;
		fheight = 9;
		nummines = 10;
	}
	else if(skill == 1){
		fwidth = 14;//16;
		fheight = 10;//16;
		nummines = 28;//40;
	}
	else if(skill == 2){
		fwidth = 28;//30;
		fheight = 20;//16;
		nummines = 99;
	}
	else{
		fwidth = customwidth;
		fheight = customheight;
		nummines = custommines;
	}

	numflags = 0;
	seconds = 0;
	fracs = 0;
	if(flags & LOST){flags ^= LOST;}
	if(flags & WON){flags ^= WON;}
	if(flags & STARTED){flags ^= STARTED;}
	if(fwidth > 14 || fheight > 10){flags |= BIGFIELD;}else if(flags & BIGFIELD){flags ^= BIGFIELD;}
	
	if(flags & BIGFIELD){
		xoff = (MAXWIDTH-fwidth)/2;
		yoff = (MAXHEIGHT-fheight)/2;
	}
	else{
		xoff = ((MAXWIDTH/2)-fwidth)/2;
		yoff = ((MAXHEIGHT/2)-fheight)/2;
	}

	prand();
	InitField();

	//WaitVsync(1);
	DrawMap2(0,0,BGMap);

	for(uint8_t x=0;x<MAXWIDTH;x++)
	for(uint8_t y=0;y<MAXHEIGHT;y++)
		SetTile(1+x,7+y,13);

	for(uint8_t x=0;x<fwidth;x++)
	for(uint8_t y=0;y<fheight;y++)
		SetVidGrid(x,y,0);
	
	//draw border edge
	if(flags & BIGFIELD){
		if(fheight < MAXHEIGHT)
			for(uint8_t x=0;x<fwidth;x++)
				SetTile(x+1+xoff,fheight+7+yoff,GRIDSTART+5);
		if(fwidth < MAXWIDTH)
			for(uint8_t y=0;y<fheight;y++)
				SetTile(fwidth+1+xoff,y+7+yoff,GRIDSTART+6);
	}
	else{
		if(fheight < MAXHEIGHT)
			for(uint8_t x=0;x<(fwidth*2);x++)
				SetTile(x+1+(xoff*2),(fheight*2)+7+(yoff*2),GRIDSTART+5);
		if(fwidth < MAXWIDTH)
			for(uint8_t y=0;y<(fheight*2);y++)
				SetTile((fwidth*2)+1+(xoff*2),y+7+(yoff*2),GRIDSTART+6);
	}
}

void InitField(){
	uint8_t x,y;

	//clear the field
	for(y=0;y<MAXHEIGHT;y++)
	for(x=0;x<MAXWIDTH;x++)
		SetGrid(x,y,0);
	
	//randomly place the mines in an open spot
	int iteration = 0;

	//WaitVsync(1);	

	for(uint8_t i=0;i<nummines;i++){
		do{
			x = prand()%fwidth;
			y = prand()%fheight;
		 if(++iteration > 8000){//avoid running into vsync
			 iteration = 0;
			WaitVsync(1);
		 }
		}
		while(GetGrid(x,y) == MINE);
		
		SetGrid(x,y,MINE);
	}

	
	for(uint8_t row=0;row<fwidth;row++)
	for(uint8_t col=0;col<fheight;col++){
		if(GetGrid(row,col) != MINE){//No Mine
			uint8_t i,num=0,temp,ecol,erow,startrow;
		 temp=col==0?col:col-1;
		 ecol=col!=fheight-1?col+1:col;
		 startrow=row==0?row:row-1;
		 erow=row==fwidth-1?row:row+1;
		 
		 for(i=startrow;i<=erow;i++)
		 for(uint8_t j=temp;j<=ecol;j++)
			 if(GetGrid(i,j)==MINE)
		 		num++;
		 
		 SetGrid(row,col,num+1);
		}
	}
}

void Input(){
//snesMouseEnabled = true;
	if(false){//!snesMouseEnabled){
		oldpadstate = padstate;
		padstate = ReadJoypad(0);
	
		if((flags & BIGFIELD) && !(flags & GUIOPEN) && !(flags & LOST))
			movemag = 1;
		else
			movemag = 2;

		if(padstate & BTN_RIGHT){
			mx += movemag;
			if(mx > 232)
				mx = 232;
		}else if(padstate & BTN_LEFT){
			mx -= movemag;
			if(mx > 240)
				mx = 0;
		}

		if(padstate & BTN_UP){
			my -= movemag;
			if(my > 240)
				my = 1;
		}
		else if(padstate & BTN_DOWN){
			my += movemag;
			if(my > 216)
				my = 216;
		}

		if(mlb)
			mlbwd = true;
		else
			mlbwd = false;

		if(padstate & BTN_A)
			mlb = true;
		else
			mlb = false;

		if(mrb)
			mrbwd = true;
		else
			mrbwd = false;

		if(padstate & BTN_B)
			mrb = true;
		else
			mrb = false;

		if(flags & GUIOPEN)
			return;

		//if(padstate & BTN_START && !(oldpadstate & BTN_START))
			//InGameMenu();
	}else{
		//mx = GetMouseX();
		//my = GetMouseY();
	}

}

void Win(){
	DrawFace(108,16,4);
	flags |= WON;
	TriggerFx(VICTORYFX,255,true);
	if(skill < 3)
		VictoryMenu();
}

void Loss(){
	uint8_t t,t2;
	
	WaitVsync(1);

	//expose all mines
	for(uint8_t x=0;x<fwidth;x++)
	for(uint8_t y=0;y<fheight;y++){
		t = GetGrid(x,y);
		t2 = GetVidGrid(x,y);
		if(t == MINE){
			if(t2 != FLAG)
				SetVidGrid(x,y,MINE+(x == mgridx && y == mgridy));
		}
		else if(t2 == FLAG)//falsely marked mine
			SetVidGrid(x,y,MINE+2);//draw a mine with an x over it
	}
	
	flags |= LOST;

	if(flags & SOUND){
		TriggerFx(EXPLODEFX,128,false);
		TriggerFx(EXPLODEFX+1,128,false);
	}
}

uint16_t prand(){
	static uint16_t prng_state = 0xACE1u;
	uint16_t bit	= (uint16_t)(((prng_state >> 0) ^ (prng_state >> 2) ^ (prng_state >> 3) ^ (prng_state >> 5) ) & 1);
	prng_state =	(uint16_t)((prng_state >> 1) | (bit << 15));
	return prng_state;
}

void SetGrid(uint8_t x, uint8_t y, uint8_t g){
	uint8_t t;

	if(x%2 || x == 1){
		t = (field[x/2][y] & 0b11110000);
		field[x/2][y] = (t|g);
	}
	else{
		t = (field[x/2][y] & 0b00001111);
		g <<= 4;
		field[x/2][y] = (t|g);
	}
}

uint8_t GetGrid(uint8_t x, uint8_t y){

	if(x%2 || x == 1)
		return (field[x/2][y] & 0b00001111);
	else
		return (field[x/2][y] & 0b11110000)>>4;
}

uint8_t GetVidGrid(uint8_t x, uint8_t y){
	static uint8_t t;
	if(!(flags & BIGFIELD)){
		x *= 2;
		y *= 2;
		x += 2*xoff;
		y += 2*yoff;
		x += 1;//adjust for hud offset
		y += 7;
				
		t = vram[(y*30)+x]-RAM_TILES_COUNT;
		t /= 4;
		t -= 7;
	}
	else{
		x += xoff;
		y += yoff;
		x += 1;//adjust for hud offset
		y += 7;
		t = vram[(y*30)+x]-RAM_TILES_COUNT;
		t -= 60;//...
		t -= 30;
	}
		

	return t;
}

void SetVidGrid(uint8_t x, uint8_t y, uint8_t t){
	if(x >= fwidth || y >= fheight)
		return;

	if(!(flags & BIGFIELD)){
		x *= 2;
		y *= 2;
		t *= 4;
		x += xoff*2;
		y += yoff*2;
		t += GRIDSTART;
		SetTile(x+1,y+7,t+0);
		SetTile(x+2,y+7,t+1);
		SetTile(x+1,y+8,t+2);
		SetTile(x+2,y+8,t+3);
	}
	else{
		x += xoff;
		y += yoff;
		t += GRIDSTART+60;//offset to small tiles
		SetTile(x+1,y+7,t);
	}
}

inline uint8_t MouseGridX(){
	if(flags & BIGFIELD){
		return (mx-8)/8;
	}

	return (mx-8)/16;
}

inline uint8_t MouseGridY(){
	if(flags & BIGFIELD){
		return (my-8)/8;
	}

	return (my-8)/16;
}

void DrawNumber(uint8_t x, uint8_t y, int num){

	for(uint8_t yo=0;yo<3;yo++)
	for(uint8_t xo=0;xo<2;xo++)
		SetTile(x+xo,y+yo,pgm_read_byte(&NumberMaps[(num*6)+(yo*2)+xo])+NUMBERSTART-1);
}

void DrawTimer(){
	uint8_t c,i,x;

	x = 26;
	int val = seconds;
	 
	for(i=0;i<3;i++){
		c=val%10;
		if(val>0 || i==0){
			DrawNumber(x,2,c);
			 x -= 2;
		}else{
			DrawNumber(x,2,0);
			x -= 2;
		}
		val=val/10;
	}
}

void DrawMineCount(){
	uint8_t c,i,x;

	x = 6;
	 int val = abs(nummines-numflags);	
	 if(val > 99)
		val = 99;

	for(i=0;i<3;i++){
		c=val%10;
		if(val>0 || i==0){
			DrawNumber(x,2,c);
			 x -= 2;
		}else{
			DrawNumber(x,2,0);
			x -= 2;
		}
		val=val/10;
	}

	if(numflags > nummines){//draw negative sign
		 DrawNumber(2,2,10);
	}
 
}

void DrawFace(uint8_t x, uint8_t y, uint8_t override){
	
	uint8_t face=0;
	
	if(override != 255)
		face = override;
	else if(flags & LOST)
		face = 2;
	else if(flags & WON)
		face = 4;
	else if(mlb)
		face = 1;
	
	for(uint8_t xo=0;xo<3;xo++)
	for(uint8_t yo=0;yo<3;yo++){
		sprites[xo+(yo*3)].x = (xo*8)+x;
		sprites[xo+(yo*3)].y = (yo*8)+y;
		sprites[xo+(yo*3)].tileIndex = FACESTART+pgm_read_byte(&FaceMaps[face*9+((yo*3)+xo)])-1;
	}
		
}

void DrawCursor(){
	sprites[9].x = mx;
	sprites[9].y = my;
	sprites[9].tileIndex = 28;
}

bool CheckVictory(){
	uint8_t t,t2;

	for(uint8_t x=0;x<fwidth;x++)
	for(uint8_t y=0;y<fheight;y++){
		t = GetGrid(x,y);
		if((t2 = GetVidGrid(x,y)) != FLAG && t == MINE)//unmarked mine
			return false;
		else if(t2 == FLAG && t != MINE)//falsely marked mine
		 return false;
		else if(t2 == 0)//unclicked spot
			return false;
	}

	return true;
}

bool hunt(char val){
	if(fx < 1){fx = 1;}
	else if(fx > fwidth){fx = fwidth;}
	if(fy < 1){fy = 1;}
	else if(fy > fheight){fy = fheight;}

	if(searchgrid[(uint8_t)(fx-1)][(uint8_t)(fy)] == val){fx--;return true;}
	if(searchgrid[(uint8_t)(fx+1)][(uint8_t)(fy)] == val){fx++;return true;}
	if(searchgrid[(uint8_t)(fx)][(uint8_t)(fy-1)] == val){fy--;return true;}
	if(searchgrid[(uint8_t)(fx)][(uint8_t)(fy+1)] == val){fy++;return true;}
	
	return false;
}

void FloodFill(uint8_t x, uint8_t y){//uncover open connected spaces
//FLOODTOP:	
	for(uint8_t i=0;i<MAXHEIGHT+1;i++)//mark boundaries
	for(uint8_t j=0;j<MAXWIDTH+1;j++)
		searchgrid[i][j] = 1;

	for(uint8_t i=0;i<MAXHEIGHT;i++)
	for(uint8_t j=0;j<MAXWIDTH;j++){//setup the search grid
		if(GetGrid(j,i) < 2 && !GetVidGrid(j,i))
			searchgrid[j+1][i+1] = 0;
		else
			searchgrid[j+1][i+1] = 1;
	}				
	
//	WaitVsync(1);
	uint8_t t;

	t = GetGrid(x,y);
	SetVidGrid(x,y,t);//atleast this node gets drawn
	if(t > 1)
		return;	

	fx = x+1;
	fy = y+1;

	char c = -1;
	int iterations = 0;

//	WaitVsync(1);

	while(c < 0){
		searchgrid[(uint8_t)(fx)][(uint8_t)(fy)] = c--;
		SetVidGrid(fx-1,fy-1,GetGrid(fx-1,fy-1));

		
		if(!hunt(0)){
		 if(fx > 1){
			 SetVidGrid(fx-2,fy-1,GetGrid(fx-2,fy-1));
			if(fy > 1)
				SetVidGrid(fx-2,fy-2,GetGrid(fx-2,fy-2));
			 if(fy < fheight)
				SetVidGrid(fx-2,fy+0,GetGrid(fx-2,fy+0));		
		 }
		 if(fx < fwidth){
			 SetVidGrid(fx+0,fy-1,GetGrid(fx+0,fy-1));
			 if(fy > 1)
				SetVidGrid(fx+0,fy-2,GetGrid(fx+0,fy-2));
				if(fy < fheight)
				SetVidGrid(fx+0,fy+0,GetGrid(fx+0,fy+0));
			}
		 if(fy > 1){SetVidGrid(fx-1,fy-2,GetGrid(fx-1,fy-2));}
		 if(fy < fheight){SetVidGrid(fx-1,fy+0,GetGrid(fx-1,fy+0));}


			searchgrid[(uint8_t)(fx)][(uint8_t)(fy)] = 1;
			c += 2;
			hunt(c);
			SetVidGrid(fx-1,fy-1,GetGrid(fx-1,fy-1));
		}
		
			SetVidGrid(fx-1,fy-1,GetGrid(fx-1,fy-1));
		

		if(iterations++ > 900){
		//	WaitVsync(1);
			iterations = 0;
		}
	}
	
}

void SaveHighScore(uint8_t slot, unsigned short time, char *name){
	struct EepromBlockStruct ebs;
	ebs.id = MINE_SWEEPER_ID;

	if(EepromReadBlock(ebs.id, &ebs) != 0){
		EepromWriteBlock(&ebs);
		ebs.data[0] = ebs.data[10] = ebs.data[20] = 'P';
		ebs.data[1] = ebs.data[11] = ebs.data[21] = 'L';
		ebs.data[2] = ebs.data[12] = ebs.data[22] = 'A';
		ebs.data[3] = ebs.data[13] = ebs.data[23] = 'Y';
		ebs.data[4] = ebs.data[14] = ebs.data[24] = 'E';
		ebs.data[5] = ebs.data[15] = ebs.data[25] = 'R';
		ebs.data[6] = ebs.data[16] = ebs.data[26] = ' ';
		ebs.data[7] = ebs.data[17] = ebs.data[27] = ' ';
		ebs.data[8] = ebs.data[18] = ebs.data[28] = 255;
		ebs.data[9] = ebs.data[19] = ebs.data[29] = 255;
	}

	for(uint8_t i=0;i<8;i++)
		ebs.data[i+(slot*10)] = name[i];

	ebs.data[8+(slot*10)] = time & 255;
	ebs.data[9+(slot*10)] = (time>>8)&255;
	
	EepromWriteBlock(&ebs);
}

void LoadHighScore(uint8_t slot){
	struct EepromBlockStruct ebs;
	ebs.id = MINE_SWEEPER_ID;

	if(EepromReadBlock(ebs.id, &ebs) != 0){//no save game
		for(uint8_t i=0;i<3;i++)
			SaveHighScore(i,999,"PLAYER ");//make default save
		
		if(EepromReadBlock(ebs.id, &ebs) != 0){
			time = 999;
		 strcpy(name,"ERROR");
		}
	}
	else{
		for(uint8_t i=0;i<8;i++)
			name[i] = ebs.data[i+(slot*10)];

		time = (ebs.data[8+(slot*10)] & 255)+(ebs.data[9+(slot*10)]<<8);
	}
}
