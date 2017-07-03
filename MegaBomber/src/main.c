#include <avr/pgmspace.h>
#include <string.h>

#define MAP 3

#include "data/graphics/pallet.inc"
#include "data/strings.inc"
#include "data/graphics/font.inc"
#include "data/graphics/hurry.inc"
#include "data/graphics/mapincludes.inc"
#include "data/graphics/spriteoffsets.inc"
#include "data/graphics/gui/guitiles.inc"
#include "data/graphics/sprites.inc"
#include "data/graphics/frames.inc"


#include <uzebox.h>
#include <stdint.h>

#include "data/patches.inc"
#include "data/music.inc"

#include "engine.h"


int main(){
   Intro();
   EngineInit();
/*
while(true){
	  Draw2bppMap(0,0,HurryMap, HurryPixels, HurryPallet);
	  WaitVsync(1);}
 */ 
 
   while(true){
     /*
	  if(tick_tracker & TICK_ENDED)
         tick_tracker ^= TICK_ENDED;
      
	  if(tick_tracker & TICK_MISSED){
         tick_tracker ^= TICK_MISSED;
		// beep();
		 WaitVsync(1);
		 continue;
	  }
   */
      Input();
      Gui();
      if(!gamestate)
         continue;
      Logic();
      Render();
      LevelSpecial();
	  tick_tracker |= TICK_ENDED;
	  WaitVsync(1);
   }

  

   return 0;
}
