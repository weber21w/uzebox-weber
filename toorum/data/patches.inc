/*
 *  Uzebox Default Patches
 *  Copyright (C) 2008  Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
	Patches are made of a command stream made of 3 bytes per command:
		1=delta time
		2=command
		3=command parameter

	Patches must start with 1 byte describing the sound type:
		0=wave channel (i.e.; channel 0,1 and 2)
		1=noise channel (channel 3)
		2=PCM (channel 3)
		->For type=2 two more byte follows in order: sample adress low byte, sample adress hi byte

	It must end with <0,PATCH_END> and this command takes
	only two bytes (no parameter).
*/


//gold pickup
const char patch00[] PROGMEM ={ 


0,PC_WAVE,2,
//0,PC_ENV_VOL,128,
0,PC_PITCH,108,
2,PC_ENV_SPEED,-15,
0,PC_PITCH,96,
3,PC_TREMOLO_RATE,199,
0,PC_TREMOLO_LEVEL,178,
//0,PC_WAVE,0,
7,PC_PITCH,95,
0,PC_ENV_SPEED,-7,
1,PC_PITCH,96,
10,PC_PITCH,95,
//0,PC_ENV_SPEED,-12,
//1,PC_PITCH,96,
8,PC_NOTE_CUT,0,
0,PATCH_END

};

//jump
const char patch01[] PROGMEM ={ 
0,PC_WAVE,4,//2
0,PC_ENV_VOL,80,
0,PC_ENV_SPEED,12,
0,PC_PITCH,55,
1,PC_PITCH,56,
1,PC_PITCH,57,
1,PC_PITCH,58,
1,PC_PITCH,59,
1,PC_ENV_SPEED,-10,
1,PC_PITCH,61,
1,PC_PITCH,62,
0,PC_TREMOLO_RATE,120,
0,PC_TREMOLO_LEVEL,180,
1,PC_PITCH,64,
0,PC_ENV_SPEED,-15,
1,PC_PITCH,66,
1,PC_PITCH,68,
6,PC_NOTE_CUT,0,
0,PATCH_END
};


//player hurt 
const char patch02[] PROGMEM ={
0,PC_WAVE,2,
1,PC_PITCH,77,
1,PC_PITCH,73,
1,PC_ENV_SPEED,-15,
0,PC_PITCH,61,
1,PC_PITCH,57,
1,PC_PITCH,53,
//0,PC_TREMOLO_RATE,120,
//0,PC_TREMOLO_LEVEL,180,
0,PC_ENV_SPEED,-30,
1,PC_PITCH,50,
1,PC_WAVE,0,
0,PC_PITCH,46,
1,PC_PITCH,42,
1,PC_PITCH,38,

12,PC_NOTE_CUT,0,

0,PATCH_END
};


//menu move
const char patch03[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_VOL,0x70,
0,PC_ENV_SPEED,-40,
0,PC_PITCH,60,
1,PC_NOTE_CUT,0,
0,PATCH_END};


//FX: drop floor noise
const char patch04[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};

//FX: 'move'
const char patch05[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};


//FX: 'drop'
const char patch06[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};


//INST: hi-hat
const char patch07[] PROGMEM ={	
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,0x90,
0,PC_ENV_SPEED,-30, 
8,PC_NOTE_CUT,0,
0,PATCH_END
};


//INST: organ
const char patch08[] PROGMEM ={	
0,PC_WAVE,7,
1,PC_ENV_VOL,200, 
1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-25,
0,PATCH_END
};


//rythm instrument
const char patch09[] PROGMEM ={	
0,PC_TREMOLO_LEVEL,140,     
0,PC_TREMOLO_RATE,90, 
0,PC_WAVE,4,
1,PC_ENV_VOL,90,
1,PC_ENV_SPEED,2,
0,PC_NOTE_HOLD,0,
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,120, 
0,PC_ENV_SPEED,-5,
18,PC_NOTE_CUT,0,
0,PATCH_END
};

//lead instrument
const char patch10[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_VOL,120,
0,PC_ENV_SPEED,2,
0,PC_TREMOLO_RATE,178,
0,PC_TREMOLO_LEVEL,192,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-4/2,  
5,PC_ENV_SPEED,-7/2,
0,PC_WAVE,2,
0,PC_TREMOLO_RATE,168,
0,PC_TREMOLO_LEVEL,202,
5,PC_ENV_SPEED,-8/2,
0,PC_TREMOLO_RATE,128,//added after it already sounded good.............
40,PATCH_END//40
};


//bass instrument
const char patch11[] PROGMEM ={	
0,PC_TREMOLO_LEVEL,170,     
0,PC_TREMOLO_RATE,160, 
0,PC_WAVE,4,
0,PC_ENV_VOL,64,//54
0,PC_ENV_SPEED,16,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-2,
64,PC_NOTE_CUT,0,//32
0,PATCH_END
};


//Square Piano 2
const char patch12[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};


//SawTooth no hold
const char patch13[] PROGMEM ={	
0,PC_WAVE,8,
0,PC_ENV_SPEED,-15,  
4,PC_ENV_SPEED,-7,  
0,PATCH_END
};

//Snare
const char patch14[] PROGMEM ={
0,PC_ENV_SPEED,-19,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
0,PATCH_END 
};

//Clap
const char patch15[] PROGMEM ={
0,PC_ENV_SPEED,-20,
0,PC_NOISE_PARAMS,2,
1,PC_ENV_VOL,0,
2,PC_ENV_VOL,0xff,
0,PC_NOISE_PARAMS,3, 
0,PATCH_END  
};

//flute
const char patch16[] PROGMEM ={
0,PC_WAVE,7,
0,PC_ENV_VOL,100, 
1,PC_ENV_SPEED,20,
5,PC_ENV_SPEED,-9, 
0,PATCH_END
};

//bass drum
const char patch17[] PROGMEM ={	
  	0,PC_NOISE_PARAMS,9,
  	0,PC_ENV_VOL,0x40,
	1,PC_NOISE_PARAMS,12,
	1,PC_ENV_SPEED,-4,
  	1,PC_ENV_SPEED,-12,
	1,PC_ENV_SPEED,-30, 
  	8,PC_NOTE_CUT,0,
  	0,PATCH_END
};

//LINE CLEAR
const char patch18[] PROGMEM ={	
0,PC_ENV_SPEED,-20,
0,PC_NOISE_PARAMS,2,
1,PC_ENV_VOL,0,
2,PC_ENV_VOL,0xff,
0,PC_NOISE_PARAMS,3, 
0,PATCH_END  
};

/////////////////////////////////song 2//////////////////////////////////////////////////


const char patch19[] PROGMEM ={	//song 2 bass lead
0,PC_TREMOLO_LEVEL,140,     
0,PC_TREMOLO_RATE,90, 
0,PC_WAVE,5,//4
0,PC_ENV_VOL,90,
1,PC_ENV_SPEED,11,
0,PC_NOTE_HOLD,0,
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,120,
0,PC_ENV_SPEED,-1, 
10,PC_ENV_SPEED,-2,
90,PC_NOTE_CUT,0,
0,PATCH_END
/*
0,PC_WAVE,4,
0,PC_ENV_VOL,120,
0,PC_ENV_SPEED,2,
0,PC_TREMOLO_RATE,178,
0,PC_TREMOLO_LEVEL,192,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-4/3,  
7,PC_ENV_SPEED,-7/3,
0,PC_WAVE,2,
0,PC_TREMOLO_RATE,168,
0,PC_TREMOLO_LEVEL,202,
//7,PC_ENV_SPEED,-8/3,
0,PC_TREMOLO_RATE,128,//added after it already sounded good.............
110,PATCH_END//40
*/
};

//TETRIS
const char patch20[] PROGMEM ={	//song 2 rythm
0,PC_TREMOLO_LEVEL,170,     
0,PC_TREMOLO_RATE,160, 
0,PC_WAVE,4,
0,PC_ENV_VOL,84,//54
0,PC_ENV_SPEED,16,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-1,
90,PC_NOTE_CUT,0,//32
0,PATCH_END

};

//dead
const char patch21[] PROGMEM ={	//lead
0,PC_TREMOLO_LEVEL,140,     
0,PC_TREMOLO_RATE,90, 
0,PC_WAVE,4,//4
0,PC_ENV_VOL,90,
1,PC_ENV_SPEED,11,
0,PC_NOTE_HOLD,0,
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,120, 
0,PC_ENV_SPEED,-4,
26,PC_NOTE_CUT,0,
0,PATCH_END
};

//back-to-back l1
const char patch22[] PROGMEM ={	
0,PC_ENV_VOL,0,

4,PC_WAVE,4,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,60, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,80,
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_DOWN,5, 
3,PC_NOTE_UP,5, 
3,PC_NOTE_CUT,0,
0,PATCH_END 
};

//back-to-back l2
const char patch23[] PROGMEM ={	//song 2 bass drum
  	0,PC_NOISE_PARAMS,9,
  	0,PC_ENV_VOL,0x40,
	1,PC_NOISE_PARAMS,12,
	1,PC_ENV_SPEED,-4,
  	1,PC_ENV_SPEED,-12,
	1,PC_ENV_SPEED,-30, 
  	8,PC_NOTE_CUT,0,
  	0,PATCH_END
};

//back-to-back l2
const char patch24[] PROGMEM ={//	drum 2 second song
  	0,PC_ENV_VOL,0x40,
0,PC_ENV_SPEED,-17,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
0,PATCH_END 
};

//back-to-back l3
const char patch25[] PROGMEM ={	//drum 3
  	0,PC_ENV_VOL,0x40,
0,PC_ENV_SPEED,-18,
0,PC_NOISE_PARAMS,1,
2,PC_NOISE_PARAMS,3, 
0,PATCH_END  
};


//INST: triangular - no env
const char patch26[] PROGMEM ={	//drum 4
  	0,PC_NOTE_CUT,0,
  	0,PATCH_END,0,
};

//INST: SawTooth -  no env
const char patch27[] PROGMEM ={	//drum 5
  	0,PC_NOTE_CUT,0,
  	0,PATCH_END,0,
};

//INST: organ -  no env
const char patch28[] PROGMEM ={	//drum 6
  	0,PC_NOTE_CUT,0,
  	0,PATCH_END,0,

};


//INST: square -  no env
const char patch29[] PROGMEM ={	//drum 7
  	0,PC_NOTE_CUT,0,
  	0,PATCH_END,0,
};

//INST: Filtered square -  no env
const char patch30[] PROGMEM ={	

  	0,PC_NOTE_CUT,0,
  	0,PATCH_END,0,
};

extern const char waves[];

const struct PatchStruct patches[] PROGMEM = {
//{2,waves+(256*W_SAW),inst16,0,256},
{2,waves+(256*2),patch00,0,256},
{2,waves+(256*4),patch01,0,256},
{2,waves+(256*2),patch02,0,256},
{2,waves+(256*4),patch03,0,256},
{1,NULL,patch04,0,0},
{0,NULL,patch05,0,0},
{0,NULL,patch06,0,0},
{1,NULL,patch07,0,0}, 
{0,NULL,patch08,0,0},
{0,NULL,patch09,0,0},
{0,NULL,patch10,0,0},
{0,NULL,patch11,0,0},
{0,NULL,patch12,0,0},
{0,NULL,patch13,0,0},
{1,NULL,patch14,0,0},
{1,NULL,patch15,0,0},
{0,NULL,patch16,0,0},
{1,NULL,patch17,0,0},
{1,NULL,patch18,0,0},
{0,NULL,patch19,0,0},
{0,NULL,patch20,0,0},
{0,NULL,patch21,0,0},
{0,NULL,patch22,0,0},
{1,NULL,patch23,0,0},
{1,NULL,patch24,0,0},
{1,NULL,patch25,0,0},
{1,NULL,patch26,0,0},
{1,NULL,patch27,0,0},
{1,NULL,patch28,0,0},
{1,NULL,patch29,0,0},
{1,NULL,patch30,0,0}
};
