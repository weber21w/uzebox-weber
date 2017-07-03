void UpdatePad(){
	oldpadstate = padstate;
	padstate = ReadJoypad(0);
}

void Input(){
	//here no gui is present and lolo is not dead(those are blocking functions)
	UpdatePad();
	uint16_t toldpad,tnewpad;
	
	if(GETDEMOPLAYBACKFLAG()){
		if(padstate & BTN_START && !(oldpadstate & BTN_START)){
				
		}else{
		//oldpad = ReadDemoAction();
			toldpad=tnewpad=0;
		}
					toldpad=tnewpad=0;
	}else{
		tnewpad = padstate;
		toldpad = oldpadstate;
	}

	if(tnewpad & BTN_START && !(toldpad & BTN_START)){
		gui_state = GUIINGAMEMENU;
		return;
	}
	if(tnewpad & BTN_B){

	}
	if(poffset)
		return;
	if(tnewpad & BTN_UP)
		pstate = UP|MOVING;
	else if(tnewpad & BTN_DOWN)
		pstate = DOWN|MOVING;
	else if(tnewpad & BTN_LEFT)
		pstate = LEFT|MOVING;
	if(pstate & MOVING)
		SETLOLOMOVED();//lvl.flags |= LOLOMOVED;

}
