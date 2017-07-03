#include "gamedefines.h"
#include "uzenet.h"
#include "bot.h"
#include "input.h"
#include "logic.h"
#include "gui.h"
#include "render.h"

inline void EngineInit(){
   InitMusicPlayer(patches);
   SetSpritesTileTable(GameSprites);
   level = 0;
   SetTileSet();
   HideSprites(0,MAX_SPRITES);
   SetMasterVolume(NORMAL_VOL);
   numplayers = 3;
   SetSlot(0,0);
   SetSlot(1,1);
   SetSlot(2,1);
   SetSlot(3,1);

   FadeIn(1,false);
   //Intro();


StartSong(StageSong);

u8 f=32;



for(int i=0;i<VRAM_TILES_V*VRAM_TILES_H;i++)
vram[i] = RAM_TILES_COUNT+15;
while(false){
f++;
//f = 1;

if(f > 1)
f = 0;
SetTileTable(GuiTiles);
DoHideSprites();
ResetSpriteCount();
DrawSpriteTile(16,16,0 ,0,0,0);

  WaitVsync(15);

  }

  guistate = GMAINMENU;
  gamestate = STARTMATCH;
  prng = 7;
//  srand(122);



}


inline void Engine(){
/*   EngineInit();

   while(true){
      Input();
      Logic();
      Render();
   }
*/
}

