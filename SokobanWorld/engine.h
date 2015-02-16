#include "data/tiles.inc"
#include "data/sprites.inc"
#include "data/frames.inc"
#include "data/patches.inc"
#include "data/songs.inc"

#include "gamedefines.h"
#include "ramfx.h"
#include "input.h"
#include "logic.h"
#include "gui.h"
#include "render.h"
#include "demo.h"


void EngineInit(){
	ClearVram();
	demonum = 0;
	gamestate = 0;
	guistate = GMAINMENU;
	guijuststarted = true;
	tileset = 0;
	tracknum = 1;
	numlevels = NUMLEVELS;
	InitMusicPlayer(patches);
	SetSpritesTileTable(GameSprites);
	SetTileTable(GameTiles);
	SetMasterVolume(192);
	GameIntro();
}

/*
inline void Engine(){
	EngineInit();
	while(true){
		Input();
		Gui();
		Logic();
		Render();
	}
}
*/
