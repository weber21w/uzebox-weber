//2010 Lee Weber (D3thAdd3r)
//Released under GPL >= 3.0, do what you will with it.

//Don't have the time to clean this up properly, had to get it done and things went wrong :)
//build options for different episodes is in gamedefines.h, have fun.



#include <avr/pgmspace.h>
#include <string.h>
#include <uzebox.h>
#include <stdint.h>

#include "engine.h"
	
int main(){
	EngineInit();

	while(true){
		Input();
		Gui();
		Logic();
		Render();
	}
  
	return 0;
}

