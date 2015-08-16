u8 menulastx, menulasty, menulastwidth, menulastheight;

void SaveBG(){//store tile values to be covered by menu and restored later
	for(u8 x=0;x<MAXWIDTH;x++)
	for(u8 y=0;y<MAXHEIGHT;y++)
		searchgrid[x][y] = vram[((y+7)*30)+x+1];//use search grid memory, low on ram during gui
}

void RestoreBG(){
	for(u8 x=0;x<MAXWIDTH;x++)
	for(u8 y=0;y<MAXHEIGHT;y++)
		vram[((y+7)*30)+x+1] = searchgrid[x][y];
}

void printnum(u8 x,u8 y, int val,bool zeropad){
	unsigned char c,i;

	for(i=0;i<3;i++){
		c=val%10;
		if(val>0 || i==0){
			SetTile(x--,y,c+ZEROSTART);
		}else{
			if(zeropad){
				SetTile(x--,y,ZEROSTART);
			}else{
				//SetTile(x--,y,ZEROSTART);
			}
		}
		val=val/10;
	}
		
}

void print(int x,int y,const char *string){


	int i=0;
	char c;

	while(1){
		c=pgm_read_byte(&(string[i++]));		
		if(c!=0){
			 if(c == ' '){c = '3';}//convert space for funky font
			else if(c >= '0' && c <= '9'){printnum(x++,y,c-'0',false);continue;}
			c=((c&127)-32) + RAM_TILES_COUNT;			
			SetTile(x++,y,c+FONTSTART-17);
		}else{
			break;
		}
	}
	
}

void DrawMenu(u8 x, u8 y, u8 w, u8 h){
	SetTile(x+0,y+0,23);//draw the corners
	SetTile(x+w,y+0,25);
	SetTile(x+0,y+h,28);
	SetTile(x+w,y+h,29);

	for(u8 i=x+1;i<x+w;i++){SetTile(i,y,24);SetTile(i,y+h,1);}//draw top and bottom
	for(u8 i=y+1;i<y+h;i++){SetTile(x,i,26);SetTile(x+w,i,27);}

	for(u8 i=y+1;i<y+h;i++)
	for(u8 j=x+1;j<x+w;j++)
		SetTile(j,i,BLANKT);

	
}

void DrawOpeningMenu(u8 x, u8 y, u8 w, u8 h, int speed){
	SaveBG(x,y);

	u8 mt;
	u8 count;
	mt = 1;
	count = speed;
	while(mt<w+1){//open horizontally
		DrawMenu(x,y,mt,1);
		if(speed < 0){
			WaitVsync(abs(speed));
		}
		else{
			if(!count){WaitVsync(1);count=speed;}else{count--;}
		}
		mt++;
	}
	mt = 1;
	count = speed;
	while(mt<h+1){
		DrawMenu(x,y,w,mt);
		if(speed < 0){
			WaitVsync(abs(speed));
		}
		else{
			if(!count){WaitVsync(1);count=speed;}else{count--;}
		}
		mt++;
	}
}

void MainMenu(){
	mx = 124;
	my = 18*8;
	FadeOut(1,true);
	FadeIn(4,false);
	SetTileTable(TitleTiles);
	for(u8 y=0;y<28;y++)
	for(u8 x=0;x<30;x++)
		SetTile(x,y,112);
	DrawMap2(1,4,TitleMap);
	StartSong(Song5);
	while(true){
		Input();
		DrawFace((13*8),(16*8),mlbwd);
		DrawCursor();
		prand();
		WaitVsync(1);
		if(mlbwd && !mlb && mx >= 13*8 && mx <= 16*8 && my >= 16*8 && my <= 19*8)
			break;
	}
	
	FadeOut(3,true);
	SetTileTable(GameTiles);
	for(u8 x=0;x<30;x++)
	for(u8 y=0;y<28;y++)
		SetTile(x,y,BLANKT);
	FadeIn(3,false);
}

void InGameMenu(){
	flags |= GUIOPEN;

	u8 cursorpos = 0;

	DrawOpeningMenu(11,9,9,15,0);

	print(13,10,PSTR("BACK"));
	print(13,11,PSTR("NEW"));

	print(13,13,PSTR("NOVICE"));//BEGINNER
	print(13,14,PSTR("MIDDLE"));//INTERMEDIATE
	print(13,15,PSTR("EXPERT"));//EXPERT
	print(13,16,PSTR("CUSTOM"));

	print(13,18,PSTR("MARKS"));
	print(13,19,PSTR("SOUND"));
	print(13,20,PSTR("MUSIC"));


	print(13,22,PSTR("SCORE"));//BEST3TIMES
	print(13,23,PSTR("ABOUT"));
	//print(13,24,PSTR("HELP"));
	//print(13,25,PSTR("QUIT"));
	
	u8 secretcount = 0;

	while(true){

	Input();
	if(mx > 107 && mx < 108+25 && my > 15 && my < 16+25){//mouse is on face
	 if(mrb && !mrbwd && ++secretcount > 45){//clicked the face 45 times, exciting secret....yeah
		 DrawFace(108,16,4);
		flags |= DEBUG;
		print(11,8,PSTR("DEBUG MODE"));
	}
	}
	else if(mx >= 12*8 && mx <= 21*8 && my >= 9*8 && my <= 25*8)//pointer is in valid menu area
		cursorpos = (my-(10*8))/8;
	else
		cursorpos = 255;

	if(cursorpos != 255 && mlb && !mlbwd){

		if(cursorpos == 0){//back
			RestoreBG(11,9);
			return;
		}
		else if(cursorpos == 1){//new
			ResetBoard();
		 return;
		}
		else if(cursorpos > 2 && cursorpos < 7){//skill level
		 skill = cursorpos-3;
		 if(skill == 3){//custom
				RestoreBG(11,9);
			CustomMenu();
			 return;
		 }
		 NewGame();
		 return;
		}
		else if(cursorpos == 8){
			if(flags & MARKS)
			 flags ^= MARKS;
			else
			 flags |= MARKS;
		}else if(cursorpos == 9){
			if(flags & SOUND)
			 flags ^= SOUND;
			else
			 flags |= SOUND;
		}
		else if(cursorpos == 10){//music
			if(++music > NUMSONGS)
			 music = 0;
			
		 StopSong();
			if(music)
				StartSong((const char *)(pgm_read_word(&musicData[music])));
		}
		else if(cursorpos == 12){//scores
			RestoreBG(11,9);
		 ScoreMenu();
		 return;
		}
		else if(cursorpos == 13){//about
			RestoreBG(11,9);
		 AboutMenu();
		 return;
		}
		else if(cursorpos == 14){//help
			/*RestoreBG(11,9);
		 HelpMenu();
		 return;*/
		}
		else if(cursorpos == 15){//quit

		}
		else{//empty spot

		}
	}		
	
	for(u8 i=0;i<4;i++){SetTile(19,13+i,BLANKT);}//cover skill check mark
	SetTile(19,13+skill,FONTSTART+21);	 
	
	if(flags & MARKS){SetTile(19,18,FONTSTART+21);}else{SetTile(19,18,BLANKT);}	
	if(flags & SOUND){SetTile(19,19,FONTSTART+21);}else{SetTile(19,19,BLANKT);}
	printnum(19,20,music,false);
			
		DrawCursor();
		prand();
		WaitVsync(1);

	}
}

void CustomMenu(){
	flags |= GUIOPEN;
	DrawOpeningMenu(10,9,10,6,0);

	print(11,10,PSTR("HEIGHT"));
	print(11,11,PSTR("WIDTH"));
	print(11,12,PSTR("MINES"));
	print(14,14,PSTR("OK"));

	while(true){
		Input();
		
		if(mlb && !mlbwd && mx >= 18*8 && mx <= 20*8 && my <= 13*8){
			if(my >= 12*8){//mines
			 if(--custommines < 10)
				custommines = 10;
		 }
		 else if(my >= 11*8){//width
				if(--customwidth < 9)
				customwidth = 9;
		 }
		 else if(my >= 10*8){//height
				if(--customheight < 9)
				customheight = 9;
		 }
		}
		else if(mrb && !mrbwd && mx >= 18*8 && mx <= 20*8 && my <= 13*8){
			if(my >= 12*8){//mines
			 if(++custommines > 99)
				custommines = 99;
		 }
		 else if(my >= 11*8){//width
				if(++customwidth > 28)
				customwidth = 28;
		 }
		 else if(my >= 10*8){//height
				if(++customheight > 20)
				customheight = 20;
		 }
		}
		
		if(mlb && !mlbwd && mx >= 14*8 && mx <= 16*8 && my >= 14*8 && my <= 15*8){//ok
			RestoreBG(10,9);
		 skill = 3;
		 NewGame();
		 return;
		}	 
		if(custommines < (customwidth*customheight)/10)
			custommines = (customwidth*customheight)/10;
		else if(custommines > (customwidth*customheight)/5)
		 custommines = (customwidth*customheight)/5;

		print(18,10,PSTR("	"));
		print(18,11,PSTR("	"));
		print(18,12,PSTR("	"));
		printnum(19,10,customheight,false);
		printnum(19,11,customwidth,false);
		printnum(19,12,custommines,false);

		DrawCursor();
		WaitVsync(1);
	}


}


void AboutMenu(){
	flags |= GUIOPEN;
	DrawOpeningMenu(7,9,15,7,0);

	print(8,10,PSTR("	UZESWEEPER"));
	print(8,13,PSTR("LEE WEBER XXXX"));
	printnum(19,13,20,false);printnum(21,13,11,false);

	print(14,15,PSTR("OK"));

	while(true){
		Input();
		
		if(mlb && !mlbwd && mx >= 14*8 && mx <= 16*8 && my >= 15*8 && my <= 16*8){
			RestoreBG(7,9);
		 return;
		}		

		DrawCursor();
		WaitVsync(1);
	}
}

void HelpMenu(){
/*
	flags |= GUIOPEN;
	SaveBG();
	print(1, 7,PSTR("THE OBJECT OF THE GAME IS TO"));
	print(1, 8,PSTR("MARK ALL THE MINES WITHOUT	"));
	print(1, 9,PSTR("EXPOSING ANY OF THEM.		 "));
	print(1,10,PSTR("A NUMBERED SQUARE INDICATES "));
	print(1,11,PSTR("THE NUMBER OF MINES THAT	 "));
	print(1,12,PSTR("ARE IN SURROUNDING SQUARES	"));
	print(1,13,PSTR("USE THIS INFORMATION TO RULE"));
	print(1,14,PSTR("OUT AND DEDUCE THE LOCATIONS"));
	print(1,15,PSTR("OF MINES. AS EXAMPLE FINDING"));
	print(1,16,PSTR("A SQUARE MARKED 1 NEXT TO A"));
	print(1,17,PSTR("KNOWN MINE ALLOWS YOU TO	 "));
	print(1,18,PSTR("SAFELY CLICK OTHER SQUARES	"));
	print(1,19,PSTR("TOUCHING THAT 1 AND USE THE "));
	print(1,20,PSTR("INFORMATION TO CONTINUE ON	"));
	print(1,21,PSTR("									 "));
	print(1,22,PSTR("SOME GUI MENUS SUCH AS NAME "));
	print(1,23,PSTR("ENTRY CONTAIN DATA FIELDS	"));
	print(1,24,PSTR("CLICK ON A LETTER OR NUMBER "));
	print(1,25,PSTR("TO CHANGE IT					 "));
	print(1,26,PSTR("				 OK				 "));

	while(true){
		Input();
		
		if(mlb && !mlbwd && mx >= 14*8 && mx <= 26*8 && my >= 27*8 && my <= 26*8){
			RestoreBG(7,9);
		 return;
		}		

		DrawCursor();
		WaitVsync(1);
	}
	*/
}

void ScoreMenu(){
	flags |= GUIOPEN;
	DrawOpeningMenu(5,9,19,8,0);

	print(6,10,PSTR("BEST MINE SWEEPERS"));
	print(6,12,PSTR("NOVICE"));
	print(6,13,PSTR("MIDDLE"));
	print(6,14,PSTR("EXPERT"));
	print(6,16,PSTR("        OK"));
	
	for(u8 i=0;i<3;i++){
		LoadHighScore(i);	
		printnum(15,12+i,time,true);

		for(u8 j=0;j<7;j++){
			if(name[j] == ' ')
			 continue;
			//else if(name[j] >= '0' && name[j] <= '9')
			// printnum(15+j,12+i,name[j],false);

		 SetTile(17+j,12+i,FONTSTART+name[j]-32);
		}
	}

	while(true){
		Input();
		
		if(mlb && !mlbwd && mx >= 100 && mx <= 200 && my >= 100 && my <= 200){//ok button
			RestoreBG(5,9);
			return;	 
		}

		DrawCursor();
		WaitVsync(1);
	}
}

void VictoryMenu(){
	flags |= GUIOPEN;

	char name[7]="PLAYER3";

	DrawOpeningMenu(3,9,23,8,0);

	print(4,10,PSTR("YOU HAVE THE BEST TIME"));//was FASTEST instead of BEST
	print(4,11,PSTR("	FOR			LEVEL	"));//period omitted
	print(4,12,PSTR("PLEASE ENTER YOUR NAME"));//period omitted
	print(13,16,PSTR("OK"));

	if(skill == 0)		{print(11,11,PSTR("NOVICE"));}
	else if(skill == 1){print(11,11,PSTR("MIDDLE"));}
	else if(skill == 2){print(11,11,PSTR("EXPERT"));}
	
	while(true){
		Input();

		if(mlb && !mlbwd){
			if(mx >= 11*8 && mx < 18*8 && my >=14*8 && my <= 15*8){//is on	name field
				if(--name[(mx-(11*8))/8] < 'A'-14)
				 name[(mx-(11*8))/8] = 'Z';
			}
			else if(mx >= 13*8 && mx <= 15*8 && my >= 16*8 && my <= 17*8){//is on ok
				SaveHighScore(skill,seconds,(char *)&name);
			RestoreBG(3,9);
			ScoreMenu();
			 return;
			}
		}
		if(mrb && !mrbwd){
			if(mx >= 11*8 && mx < 18*8 && my >= 14*8 && my <= 15*8){//is on	name field
				if(++name[(mx-(11*8))/8] > 'Z')
				 name[(mx-(11*8))/8] = 'A'-14;
			}
		}
		
		for(u8 i=0;i<7;i++){SetTile(11+i,14,name[i]+80);}//print(11,14,PSTR("PLAYER1"));
		DrawCursor();
		WaitVsync(1);
	}
}
