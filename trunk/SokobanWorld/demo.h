void DemoSetup();
void DemoShutdown();

bool DemoQuit(){
	demoplaying=false;guijuststarted=true;guistate=GMAINMENU;//return true;
	DoScreenOutEffect();
	//FadeOut(1,true);
	//FillScreen(BLANKT);
	WaitVsync(1);
	return true;
}

bool PlayDemo(u8 demonum){//TODO, don't like this, should be part of input. oh well...

	u8 t,t2;
	demoplaying = true;
	int demopos = 0;
	u8 speed = 0;
	u8 cwait = 30;
	u8 coff = 0;
	t2 = 0;

	HideSprites(0,MAX_SPRITES);
	WaitVsync(1);

	level = demonum;
//	u8 bg = vram[17]-RAM_TILES_COUNT;//we have to store the tile under where we will draw the speed digit(tacky I know but necessary)
	
	u8 restore[5];


	for(u8 i=0;i<5;i++)
		restore[i] = GetMapTile(5+i,0);//save the tiles under the overlaid text	

	while(demonum--){//seek to start position
		while(pgm_read_byte(&Demos[demopos++]) != 17);//terminating number, 17 = UP,DOWN,UP,DOWN a useless move so the demos wont have it	
	}
	LoadLevel(true,true,false,false,false,false);
	FadeIn(1,true);
	Render();//draw player once

	printrainbow(12,27,PSTR("5DEMO5"),24,0xC0,0,0);
		//printramtilesoverlaid(10,0,strSpeed,16,1,1,1,1,0xC0);
		//printdigitoverlaid(18,0,speed+1,15,0xC0,bg);
	
	WaitVsync(60);

	while(true){
		
		t = pgm_read_byte(&Demos[demopos++]);
	  if(!opentargets || t == 17){//demo end
		  //for(u8 i=0;i<5;i++)
			 // SetMapTile(5+i,0,restore[i]);//restore the tiles under the overlaid text, otherwise ram tiles show through on screen fade
		  return DemoQuit();
	  }
	  
	  for(u8 i=0;i<4;i++){//each byte holds up to 4 moves
			if(opentargets){
			 t2 = (( t>>(i*2) ) & 3);//get 2 bits per move
			
			 if(t2==0)	  {t2=UP	|MOVE;}//convert 0,1,2,3 to UP,DOWN,LEFT,RIGHT
			 else if(t2==1){t2=DOWN |MOVE;}
			 else if(t2==2){t2=LEFT |MOVE;}
			 else			 {t2=RIGHT|MOVE;}
			}

		 pstate = t2;
		  for(u8 i=0;i<16;i++){//1 demo tick equals 1 full movement

				FillPad();
				if(StartDown()){//allow user to skip demo
					WaitVsync(1);
					/*for(u8 i=0;i<5;i++)
						SetMapTile(7+i,0,restore[i]);//restore the tiles under the overlaid text, otherwise ram tiles show through on screen fade
					*/
					return DemoQuit();
				}
			
			Logic();
	
			if(padstate & BTN_SR && !(oldpadstate & BTN_SR)){//fast forward
				if(speed == 0)
					speed = 1;
				else if(speed == 1)
					speed = 3;
					else
					speed = 7;

				//printdigitoverlaid(18,0,speed+1,15,0xC0,bg);

				Render();
			 }
			else if(padstate & BTN_SL && !(oldpadstate & BTN_SL)){//slow down
				if(speed == 1)
					speed = 0;
				else if(speed == 3)
					speed = 1;
					else if(speed == 7)
					speed = 3;

				//printdigitoverlaid(18,0,speed+1,15,0xC0,bg);

				Render();
			 }			  

				for(u8 j=0;j<speed;j++){
				Logic();
				i++;
			}

			spritecount = 0;
			Render();

				
			if(!cwait){//handle colored text
				  cwait = 30;
				  printrainbow(12,27,PSTR("5DEMO5"),24,0xC0,0,coff++);
				 if(coff > 3){coff = 0;}
			  }
			  else
				  cwait--;	
				}
		 }
	}//while true
}

void CalculateOptimum(u8 demonum){//scan through a demo and determine optimum moves/pushes(apparently solver program isn't perfect!)
	int demopos = 0;
	u8 t,t2;	
	level = demonum;

	demoplaying = true;//avoid saving progress or anything else

	while(demonum--){//seek to start position
		while(pgm_read_byte(&Demos[demopos++]) != 17);//terminating number, 17 = UP,DOWN,UP,DOWN a useless move so the demos wont have it	
	}

	LoadLevel(false,false,false,false,false,true);
	gamestate |= SCANNINGOPTIMUM;//gross hack to avoid triggering sounds etc.

	while(true){
		
		t = pgm_read_byte(&Demos[demopos++]);
	  if(!opentargets || t == 17)
			goto DemoScanEnd;//just wanted to have 1 ;)

	  for(u8 i=0;i<4;i++){//each byte holds up to 4 moves
			if(opentargets){
			 t2 = (( t>>(i*2) ) & 3);//get 2 bits per move
			
			 if(t2==0)	  {t2=UP	|MOVE;}//convert 0,1,2,3 to UP,DOWN,LEFT,RIGHT
			 else if(t2==1){t2=DOWN |MOVE;}
			 else if(t2==2){t2=LEFT |MOVE;}
			 else			 {t2=RIGHT|MOVE;}

			pstate = t2;
			poffset = 0;//setup move
			Logic();
			poffset = 16;//finish move
			Logic();
			}
		}//for
	}//while

DemoScanEnd://TODO FIX ME IF THERE IS "ENOUGH EXTRA SPACE" :D lol
	optimummoves = moves;
	optimumpushes = pushes;
	gamestate =0;//^= SCANNINGOPTIMUM;
	demoplaying = false;
}

