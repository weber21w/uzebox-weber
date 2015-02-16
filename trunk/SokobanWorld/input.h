//these functions return true if the button was just pressed and not held
bool StartDown() {return((padstate  & BTN_START) && !(oldpadstate & BTN_START)) ;}
bool SelectDown(){return((padstate  & BTN_SELECT)&& !(oldpadstate & BTN_SELECT));}
bool UpDown()	 {return((padstate  & BTN_UP)	 && !(oldpadstate & BTN_UP))	;}
bool DownDown()  {return((padstate  & BTN_DOWN)  && !(oldpadstate & BTN_DOWN)) ;}
bool LeftDown()  {return((padstate  & BTN_LEFT)  && !(oldpadstate & BTN_LEFT)) ;}
bool RightDown() {return((padstate  & BTN_RIGHT) && !(oldpadstate & BTN_RIGHT));}

void FillPad(){
	oldpadstate = padstate;
	padstate = ReadJoypad(0);
}

extern bool kernelreadpads;

void Input(){
	FillPad();
	
	if(StartDown()) {guijuststarted=true;guistate=GINGAMEMENU;}
	if(SelectDown()){RetryLevel();return;};

/*
	if(padstate & BTN_A && !(oldpadstate & BTN_A))
		opentargets = 0;
	  
	  Undo();
	if(padstate & BTN_B && !(oldpadstate & BTN_B))
		Redo();
*/


	if(padstate & BTN_SR && !(oldpadstate & BTN_SR)){
		if(gamestate & FASTMOVE){gamestate ^= FASTMOVE;}
	  else						  {gamestate |= FASTMOVE;}
	}

	if(poffset)
		return;

	if(padstate & BTN_UP)		  {pstate = UP	| MOVE;}
	else if(padstate & BTN_DOWN) {pstate = DOWN | MOVE;}
	else if(padstate & BTN_LEFT) {pstate = LEFT | MOVE;}
	else if(padstate & BTN_RIGHT){pstate = RIGHT| MOVE;}
}

