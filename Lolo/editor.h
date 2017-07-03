void DrawEditorPanel();
void EditorShiftScreenLeft();

void Editor(){
	uint8_t panel=0,cursorx=5,cursory=5;
	uint8_t nummovingenemies=0;
	bool blink = false;
	

	for(uint8_t i=0;i<11*11;i++){
		lvl.grid[i] = 0;
	}
	while(true){
		UpdatePad();
		if(padstate & BTN_SELECT && !(padstate & BTN_SELECT))
			if(++panel > 4)
				panel = 0;

		cursorx=cursory=nummovingenemies=0;

		DrawLevel();
		EditorShiftScreenLeft();
		DrawEditorPanel();
		blink = !blink;
		WaitVsync(2);
	}
}

const char EditorPanelMap[] PROGMEM = {
0,1,
2,3,
4,5,
6,7,
8,9,
10,11,
12,13,
14,15,


};

void DrawEditorPanel(){
	DrawMenu(25,0,6,20,0);
	uint8_t moff = 0;
	for(uint8_t y=0;y<8;y++)
	for(uint8_t x=0;x<2;x++){
		DrawMapTile(26+(x*2),2+(y*2),pgm_read_byte(&EditorPanelMap[moff++]));
	}

}

void EditorShiftScreenLeft(){
	uint16_t voff = 0;
	for(uint8_t y=0;y<SCREEN_TILES_V;y++)
	for(uint8_t x=0;x<SCREEN_TILES_H;x++){
		vram[voff] = vram[voff+2];
		voff++;
	}
}
