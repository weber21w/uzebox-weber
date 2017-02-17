

//main song lead
const char patch00[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_NOTE_UP,12,
0,PC_ENV_VOL,67,
0,PC_TREMOLO_RATE,203,
0,PC_TREMOLO_LEVEL,158,

//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
3,PC_ENV_SPEED,-1,
3,PC_ENV_SPEED,-2,
3,PC_ENV_SPEED,-7,
3,PC_ENV_SPEED,-13,
//0,PC_NOTE_HOLD,0,
11,PC_NOTE_CUT,0,
0,PATCH_END

};

//main song bass
const char patch01[] PROGMEM ={ 
0,PC_WAVE,5,
0,PC_ENV_VOL,72,
0,PC_ENV_SPEED,8,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-7,
//7,PC_NOTE_CUT,0,
0,PATCH_END
};

//main song depth
const char patch02[] PROGMEM ={	
0,PC_WAVE,3,
0,PC_ENV_VOL,10,
1,PC_ENV_SPEED,7,
0,PC_TREMOLO_RATE,223,
0,PC_TREMOLO_LEVEL,170,
//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
7,PC_ENV_SPEED,-1,
19,PC_ENV_SPEED,-4,
0,PC_NOTE_HOLD,0,
3,PC_ENV_SPEED,-10,
4,PC_ENV_SPEED,-12,
//0,PC_NOTE_HOLD,0,
//9,PC_NOTE_CUT,0,
0,PATCH_END

};

//main song spare
const char patch03[] PROGMEM ={	  
0,PATCH_END
};

//main song bass drum
const char patch04[] PROGMEM ={	
	0,PC_NOISE_PARAMS,18,
	0,PC_ENV_VOL,46,
	0,PC_ENV_SPEED,-4,
	1,PC_NOISE_PARAMS,23,
	1,PC_NOISE_PARAMS,18,
	1,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//main song crash cymbal
const char patch05[] PROGMEM ={	
  	0,PC_NOISE_PARAMS,2,
  	0,PC_ENV_VOL,34,
  	0,PC_ENV_SPEED,-1,
	1,PC_NOISE_PARAMS,1,
	1,PC_NOISE_PARAMS,2,
	2,PC_NOISE_PARAMS,1,
	5,PC_ENV_SPEED,-2,
  	64,PATCH_END
};

//main song snare drum
const char patch06[] PROGMEM ={	
	0,PC_NOISE_PARAMS,5,
	0,PC_ENV_VOL,32,
	0,PC_ENV_SPEED,-8,
	2,PC_NOISE_PARAMS,7,
	2,PC_NOISE_PARAMS,3,
	1,PC_NOISE_PARAMS,1,
	2,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//main song spare
const char patch07[] PROGMEM ={	
	0,PATCH_END
};



//////////////////////////////////////////////

//title song lead
const char patch08[] PROGMEM ={ 
	0,PC_WAVE,4,
	0,PC_NOTE_UP,12,
	0,PC_ENV_VOL,67,
	0,PC_TREMOLO_RATE,203,
	0,PC_TREMOLO_LEVEL,158,
	3,PC_ENV_SPEED,-1,
	3,PC_ENV_SPEED,-2,
	3,PC_ENV_SPEED,-7,
	3,PC_ENV_SPEED,-13,
	11,PC_NOTE_CUT,0,
	0,PATCH_END
};


//title song bass
const char patch09[] PROGMEM ={ 
	0,PC_WAVE,5,
	0,PC_ENV_VOL,72,
	0,PC_ENV_SPEED,8,
	1,PC_NOTE_HOLD,0,
	0,PC_ENV_SPEED,-7,
	0,PATCH_END
};

//title song depth
const char patch10[] PROGMEM ={	
	0,PC_WAVE,3,
	0,PC_ENV_VOL,10,
	1,PC_ENV_SPEED,7,
	0,PC_TREMOLO_RATE,223,
	0,PC_TREMOLO_LEVEL,170,
	7,PC_ENV_SPEED,-1,
	19,PC_ENV_SPEED,-4,
	0,PC_NOTE_HOLD,0,
	3,PC_ENV_SPEED,-10,
	4,PC_ENV_SPEED,-12,
	0,PATCH_END
};

//title song spare
const char patch11[] PROGMEM ={	  
0,PATCH_END
};

//title song bass drum
const char patch12[] PROGMEM ={	
	0,PC_NOISE_PARAMS,18,
	0,PC_ENV_VOL,46,
	0,PC_ENV_SPEED,-4,
	1,PC_NOISE_PARAMS,23,
	1,PC_NOISE_PARAMS,18,
	1,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//title song crash cymbal
const char patch13[] PROGMEM ={	
  	0,PC_NOISE_PARAMS,2,
  	0,PC_ENV_VOL,34,
  	0,PC_ENV_SPEED,-1,
	1,PC_NOISE_PARAMS,1,
	1,PC_NOISE_PARAMS,2,
	2,PC_NOISE_PARAMS,1,
	5,PC_ENV_SPEED,-2,
  	64,PATCH_END
};

//title song snare drum
const char patch14[] PROGMEM ={	
	0,PC_NOISE_PARAMS,5,
	0,PC_ENV_VOL,32,
	0,PC_ENV_SPEED,-8,
	2,PC_NOISE_PARAMS,7,
	2,PC_NOISE_PARAMS,3,
	1,PC_NOISE_PARAMS,1,
	2,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

//title song spare
const char patch15[] PROGMEM ={	
	0,PC_NOISE_PARAMS,1,
	0,PC_ENV_VOL,0x90,
	0,PC_ENV_SPEED,-30, 
	0,PATCH_END
};

/////////////////////////////////////////////

//level clear lead
const char patch16[] PROGMEM ={ 
	0,PC_WAVE,2,
	0,PC_ENV_VOL,70,
	0,PC_TREMOLO_RATE,180,
	0,PC_TREMOLO_LEVEL,99,
	3,PC_ENV_SPEED,-1,
	3,PC_ENV_SPEED,-2,
	3,PC_ENV_SPEED,-7,
	3,PC_ENV_SPEED,-13,
	0,PATCH_END
};


//level clear bass
const char patch17[] PROGMEM ={ 
	0,PC_WAVE,4,
	0,PC_ENV_VOL,62,
	0,PC_ENV_SPEED,8,
	0,PC_ENV_SPEED,-7,
	0,PATCH_END
};

//level clear depth
const char patch18[] PROGMEM ={	
	0,PC_WAVE,3,//4
	0,PC_ENV_VOL,27,
//	0,PC_TREMOLO_RATE,203,
//	0,PC_TREMOLO_LEVEL,99,
	3,PC_ENV_SPEED,-1,
	3,PC_ENV_SPEED,-2,
	3,PC_ENV_SPEED,-7,
	3,PC_ENV_SPEED,-13,
	11,PC_NOTE_CUT,0,
	0,PATCH_END
};

//level clear spare
const char patch19[] PROGMEM ={	  
	0,PATCH_END
};

//title song bass drum
const char patch20[] PROGMEM ={	
	0,PATCH_END
};

//level clear crash cymbal
const char patch21[] PROGMEM ={	
  	0,PATCH_END
};

//level clear snare drum
const char patch22[] PROGMEM ={	
	0,PATCH_END
};

//level clear spare
const char patch23[] PROGMEM ={	
	0,PATCH_END
};

////////////////song 3
const char patch24[] PROGMEM ={	
0,PC_WAVE,4,
//0,PC_NOTE_DOWN,12,
0,PC_ENV_VOL,67,
//0,PC_TREMOLO_RATE,203,
//0,PC_TREMOLO_LEVEL,158,

//0,PC_ENV_SPEED,2,
//7,PC_ENV_SPEED,1,
//1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-1,
4,PC_ENV_SPEED,-2,
7,PC_ENV_SPEED,-7,
6,PC_ENV_SPEED,-13,
//0,PC_NOTE_HOLD,0,
11,PC_NOTE_CUT,0,
0,PATCH_END

};

const char patch25[] PROGMEM ={	
//0,PC_NOTE_DOWN,12,
0,PC_WAVE,4,
	0,PC_TREMOLO_RATE,223,
	0,PC_TREMOLO_LEVEL,222,

0,PC_ENV_VOL,73,
0,PC_ENV_SPEED,3,
//0,PC_NOTE_HOLD,0,
2,PC_ENV_SPEED,-1,
12,PC_ENV_SPEED,-2,
//7,PC_NOTE_CUT,0,
0,PATCH_END

};

const char patch26[] PROGMEM ={	
	0,PC_WAVE,4,
	0,PC_ENV_VOL,50,
//0,PC_NOTE_DOWN,12,
//	1,PC_ENV_SPEED,7,
	0,PC_TREMOLO_RATE,244,
	0,PC_TREMOLO_LEVEL,170,
//	0,PC_NOTE_DOWN,12,
	0,PC_ENV_SPEED,-1,
	12,PC_ENV_SPEED,-2,
//	0,PC_NOTE_HOLD,0,
	8,PC_ENV_SPEED,-3,
	4,PC_ENV_SPEED,-4,
	0,PATCH_END

};

const char patch27[] PROGMEM ={	
	0,PC_NOISE_PARAMS,18,
	0,PC_ENV_VOL,46,
	0,PC_ENV_SPEED,-4,
	1,PC_NOISE_PARAMS,23,
	1,PC_NOISE_PARAMS,18,
	1,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

const char patch28[] PROGMEM ={	
  	0,PC_NOISE_PARAMS,2,
  	0,PC_ENV_VOL,34,
  	0,PC_ENV_SPEED,-1,
	1,PC_NOISE_PARAMS,1,
	1,PC_NOISE_PARAMS,2,
	2,PC_NOISE_PARAMS,1,
	5,PC_ENV_SPEED,-2,
  	64,PATCH_END

};

const char patch29[] PROGMEM ={	
  	0,PC_NOISE_PARAMS,1,
  	0,PC_ENV_VOL,34,
  	0,PC_ENV_SPEED,-1,
	1,PC_NOISE_PARAMS,1,
	1,PC_NOISE_PARAMS,2,
	2,PC_NOISE_PARAMS,1,
	5,PC_ENV_SPEED,-2,
  	64,PATCH_END

};

const char patch30[] PROGMEM ={	
	0,PC_NOISE_PARAMS,5,
	0,PC_ENV_VOL,32,
	0,PC_ENV_SPEED,-8,
	2,PC_NOISE_PARAMS,7,
	2,PC_NOISE_PARAMS,3,
	1,PC_NOISE_PARAMS,1,
	2,PC_ENV_SPEED,-12,  
	0,PATCH_END
};

const char patch31[] PROGMEM ={	
	0,PATCH_END
};

//lift effect
const char patch32[] PROGMEM ={	
//	0,PC_WAVE,3,
	0,PC_ENV_VOL,60,
	0,PC_PITCH,29,
	2,PC_PITCH,33,
	1,PC_PITCH,37,
	2,PC_PITCH,39,
	3,PC_PITCH,45,
	0,PC_ENV_SPEED,-3,
	4,PC_PITCH,48,
	2,PC_PITCH,52,
	0,PC_ENV_SPEED,-13,
	1,PC_PITCH,60,
	0,PATCH_END
};

//throw effect
const char patch33[] PROGMEM ={	
//	0,PC_WAVE,2,
	0,PC_ENV_VOL,74,
	0,PC_PITCH,69,
	1,PC_PITCH,62,
	2,PC_PITCH,58,
	0,PC_ENV_SPEED,-3,
	1,PC_PITCH,55,
	3,PC_PITCH,47,
	0,PC_ENV_SPEED,-7,
	1,PC_PITCH,40,
	2,PC_PITCH,36,
	1,PC_PITCH,33,
	0,PATCH_END
};

//jump effect
const char patch34[] PROGMEM ={	
//	0,PC_WAVE,3,
	0,PC_ENV_VOL,36,
	0,PC_PITCH,34,
	2,PC_PITCH,39,
	1,PC_PITCH,53,
	2,PC_PITCH,59,
	3,PC_PITCH,66,
	0,PC_ENV_SPEED,-3,
	4,PC_PITCH,74,
	2,PC_PITCH,79,
	0,PC_ENV_SPEED,-13,
	1,PC_PITCH,80,
	0,PATCH_END
};


//menu blip
const char patch35[] PROGMEM ={	
//	0,PC_WAVE,2,
	0,PC_ENV_VOL,74,
	0,PC_PITCH,69,
	1,PC_PITCH,62,
	2,PC_PITCH,58,
	0,PC_ENV_SPEED,-9,
	1,PC_PITCH,55,
	3,PC_PITCH,47,
//	0,PC_ENV_SPEED,-7,
//	1,PC_PITCH,40,
//	2,PC_PITCH,36,
//	1,PC_PITCH,33,
	0,PATCH_END
};


//D3thAdd3r logo part 1
const char patch36[] PROGMEM ={
	0,PC_WAVE,0,
	0,PC_PITCH,40,
	0,PC_ENV_VOL,190,
	0,PC_SLIDE_SPEED,224,
	0,PC_SLIDE,-30,
	15,PC_ENV_SPEED,-1,
	180,PATCH_END

};


//D3thAdd3r logo part 2
const char patch37[] PROGMEM ={
	0,PC_ENV_VOL,0,
	0,PC_WAVE,3,
	//0,PC_TREMOLO_RATE,60,
	//0,PC_TREMOLO_LEVEL,169,
	0,PC_PITCH,39,
	1,PC_ENV_VOL,80,
	0,PC_SLIDE_SPEED,224,
	0,PC_SLIDE,-30,
	0,PC_ENV_SPEED,-1,
	180,PATCH_END

};

extern const char waves[];
const struct PatchStruct patches[] PROGMEM = {
{0,NULL,patch00,0,0},//main theme lead
{0,NULL,patch01,0,0},//main theme bass
{0,NULL,patch02,0,0},//main theme depth
{0,NULL,patch03,0,0},//spare
{1,NULL,patch04,0,0},//main theme bass drum
{1,NULL,patch05,0,0},//main theme crash
{1,NULL,patch06,0,0},//main theme snare
{1,NULL,patch07,0,0},//spare 

{0,NULL,patch08,0,0},//title song lead
{0,NULL,patch09,0,0},//title song bass
{0,NULL,patch10,0,0},//title song depth
{0,NULL,patch11,0,0},//spare
{1,NULL,patch12,0,0},//title song bass drum
{1,NULL,patch13,0,0},//title song crash
{1,NULL,patch14,0,0},//title song snare
{1,NULL,patch15,0,0},//spare

{0,NULL,patch16,0,0},//level clear lead
{0,NULL,patch17,0,0},//level clear bass
{0,NULL,patch18,0,0},//level clear depth
{0,NULL,patch19,0,0},//spare
{1,NULL,patch20,0,0},//spare
{1,NULL,patch21,0,0},//spare
{1,NULL,patch22,0,0},//spare
{1,NULL,patch23,0,0},//spare

{0,NULL,patch24,0,0},//new lead
{0,NULL,patch25,0,0},//new bass
{0,NULL,patch26,0,0},//new depth
{0,NULL,patch27,0,0},//spare
{1,NULL,patch28,0,0},//spare
{1,NULL,patch29,0,0},//spare
{1,NULL,patch30,0,0},//spare
{1,NULL,patch31,0,0},//spare

#define SFX_LIFT	32
{2,waves+(256*3),patch32,0,256},//lift
#define SFX_THROW	SFX_LIFT+1
{2,waves+(256*2),patch33,0,256},//throw
#define SFX_JUMP	SFX_THROW+1
{2,waves+(256*3),patch34,0,256},//jump
#define SFX_MENU	SFX_JUMP+1
{2,waves+(256*2),patch35,0,256},//menu

{0,waves+(256*0),patch36,0,256},//D3thAdd3r logo part 1
{2,waves+(256*2),patch37,0,256},//part 2

};
