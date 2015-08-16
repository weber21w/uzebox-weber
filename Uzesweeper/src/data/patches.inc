//////////////////////////////////////////////////
////////////America Patches///////////////////////
//////////////////////////////////////////////////

//Banjo
const char patch00[] PROGMEM ={ 
0,PC_WAVE,1,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,22,
3,PC_ENV_SPEED,16,
1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-25,
3,PATCH_END
};

//Brass Section
const char patch01[] PROGMEM ={ 
0,PC_WAVE,0,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,2,
7,PC_ENV_SPEED,1,
1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-10,
//0,PC_NOTE_HOLD,0,
9,PATCH_END
};

//Electric bass(finger)
const char patch02[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,11,
4,PC_ENV_SPEED,7,
1,PC_ENV_SPEED,0,
3,PC_ENV_SPEED,-4,
2,PC_ENV_SPEED,-7,
6,PATCH_END
};

//Wood Block High
const char patch03[] PROGMEM ={
0,PC_NOISE_PARAMS,8,
0,PC_ENV_VOL,44,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Wood Block Low
const char patch04[] PROGMEM ={
0,PC_NOISE_PARAMS,5,
0,PC_ENV_VOL,33,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8,  
1,PC_NOTE_CUT,0,
0,PATCH_END
};

///////////////////////////////////////////////////
//////////////Egypt Patches////////////////////////
///////////////////////////////////////////////////

//Flute
const char patch05[] PROGMEM ={ 
0,PC_WAVE,1,
0,PC_ENV_VOL,16,
0,PC_ENV_SPEED,8,
1,PC_ENV_SPEED,32,
3,PC_ENV_SPEED,16,
1,PC_ENV_SPEED,0,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-25,
3,PATCH_END,
};

//Bass
const char patch06[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_ENV_VOL,22,
0,PC_ENV_SPEED,16,
1,PC_ENV_SPEED,18,
1,PC_ENV_SPEED,20,
1,PC_ENV_SPEED,0,
//4,PC_NOTE_HOLD,0,
5,PC_ENV_SPEED,-1,
2,PC_ENV_SPEED,-2,
2,PC_ENV_SPEED,-3,
2,PC_ENV_SPEED,-4,
6,PATCH_END,0,
};

//NOT USED ANYMORE
const char patch07[] PROGMEM ={	
6,PATCH_END
};


//Low Tom
const char patch08[] PROGMEM ={	
0,PC_NOISE_PARAMS,2,
0,PC_ENV_VOL,18,
0,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-5,  
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//High Tom
const char patch09[] PROGMEM ={	
0,PC_NOISE_PARAMS,6,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-5,  
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//Not Used
const char patch10[] PROGMEM ={	
0,PATCH_END
};

//Not Used
const char patch11[] PROGMEM ={	
0,PATCH_END
};

//Not Used
const char patch12[] PROGMEM ={	
0,PATCH_END
};

//Not Used
const char patch13[] PROGMEM ={	
0,PATCH_END
};

//Not Used
const char patch14[] PROGMEM ={	
0,PATCH_END
};

///////////////////////////////////////////////////
//////////////China Patches////////////////////////
///////////////////////////////////////////////////

//Lead
const char patch15[] PROGMEM ={ 
0,PC_WAVE,1,
0,PC_TREMOLO_LEVEL,200,
0,PC_TREMOLO_RATE,55,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,18,
1,PC_ENV_SPEED,26,
2,PC_ENV_SPEED,16,
1,PC_ENV_SPEED,0,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-25,
3,PATCH_END
};

//Bass
const char patch16[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,10,
2,PC_ENV_SPEED,7,
1,PC_ENV_SPEED,4,
1,PC_ENV_SPEED,0,
//0,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-1,
1,PC_ENV_SPEED,-2,
1,PC_ENV_SPEED,-4,
2,PC_ENV_SPEED,-7,
4,PATCH_END
};

//Intro Lead 2
const char patch17[] PROGMEM ={	
0,PC_WAVE,1,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,22,
3,PC_ENV_SPEED,16,
1,PC_ENV_SPEED,0,
0,PC_ENV_SPEED,-25,
3,PATCH_END
};


//High Tom
const char patch18[] PROGMEM ={	
0,PC_NOISE_PARAMS,10,
0,PC_ENV_VOL,18,
0,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-5,  
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//Low Tom
const char patch19[] PROGMEM ={	
0,PC_NOISE_PARAMS,6,
0,PC_ENV_VOL,18,
0,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-5,  
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//Fast Hat
const char patch20[] PROGMEM ={	
0,PC_NOISE_PARAMS,2,
0,PC_ENV_VOL,18,
0,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-5,  
1,PC_NOTE_CUT,0,
0,PATCH_END
};

//Chirp NOT USED
const char patch21[] PROGMEM ={	
0,PATCH_END
};

//Block hit
const char patch22[] PROGMEM ={	
0,PATCH_END
};


///////////////////////////////////////////////////
//////////////South Pole Patches///////////////////
///////////////////////////////////////////////////

//Rock Organ
const char patch23[] PROGMEM ={ 
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,60, 
0,PC_WAVE,1,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,11,
3,PC_ENV_SPEED,17,
1,PC_ENV_SPEED,22,
1,PC_ENV_SPEED,0,
//0,PC_NOTE_HOLD,0,
5,PC_ENV_SPEED,-22,
3,PATCH_END
};

//Electric bass(finger)
const char patch24[] PROGMEM ={ 
0,PC_WAVE,4,
0,PC_ENV_VOL,12,
0,PC_ENV_SPEED,16,
4,PC_ENV_SPEED,0,
3,PC_ENV_SPEED,-11,
9,PATCH_END
};

//Baritone Sax
const char patch25[] PROGMEM ={	
0,PC_WAVE,4,
0,PC_ENV_VOL,2,
1,PC_ENV_SPEED,4,
1,PC_ENV_SPEED,6,
1,PC_ENV_SPEED,8,
1,PC_ENV_SPEED,10,
1,PC_ENV_SPEED,12,
4,PC_ENV_SPEED,0,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-12,
1,PC_ENV_SPEED,-10,
1,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-6,
1,PATCH_END
};

//Wood Block Lowest
const char patch26[] PROGMEM ={	
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,33,
0,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Wood Block Low
const char patch27[] PROGMEM ={	
0,PC_NOISE_PARAMS,5,
0,PC_ENV_VOL,33,
0,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Wood Block High
const char patch28[] PROGMEM ={	
0,PC_NOISE_PARAMS,9,
0,PC_ENV_VOL,33,
0,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Wood Block Highest NOT USED
const char patch29[] PROGMEM ={	
0,PATCH_END
};

///////////////////////////////////////////////////
//////////////Japan Patches////////////////////////
///////////////////////////////////////////////////

//Harpsichord
const char patch30[] PROGMEM ={ 
0,PC_WAVE,1,
//0,PC_TREMOLO_LEVEL,140,
//0,PC_TREMOLO_RATE,39,
0,PC_ENV_VOL,20,

0,PC_ENV_SPEED,33,
4,PC_ENV_SPEED,0,
0,PC_NOTE_HOLD,0,
0,PC_TREMOLO_LEVEL,140,
0,PC_TREMOLO_RATE,39,
0,PC_ENV_SPEED,-25,

3,PATCH_END
};

//Acoustic Bass
const char patch31[] PROGMEM ={ 
0,PC_WAVE,1,
0,PC_ENV_VOL,27,
//0,PC_ENV_SPEED,11,
1,PC_ENV_SPEED,22,
3,PC_ENV_SPEED,0,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-25,
3,PATCH_END
};

//NOT USED
const char patch32[] PROGMEM ={	
0,PC_WAVE,1,
0,PC_ENV_VOL,20,
0,PC_ENV_SPEED,22,
4,PC_ENV_SPEED,0,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-25,
3,PATCH_END
};

//Block Low
const char patch33[] PROGMEM ={	
0,PC_NOISE_PARAMS,1,
0,PC_ENV_VOL,33,
0,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Block High
const char patch34[] PROGMEM ={	
0,PC_NOISE_PARAMS,5,
0,PC_ENV_VOL,33,
0,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Ride Cymbal NOT USED
const char patch35[] PROGMEM ={	
0,PATCH_END
};

//Tamborine NOT USED
const char patch36[] PROGMEM ={	
0,PATCH_END
};


//Light Fast Hat NOT USED
const char patch37[] PROGMEM ={	
0,PATCH_END
};

const char patch38[] PROGMEM ={	
0,PATCH_END
};

const char patch39[] PROGMEM ={	
0,PATCH_END
};


//////////////////////////////////////////////////
////////Start Sound Effects///////////////////////
//////////////////////////////////////////////////
//vaus explodes 1/2
const char patch40[] PROGMEM ={	
0,PC_ENV_VOL,127,
0,PC_WAVE,2, 
0,PC_ENV_SPEED,-1, 


0,PC_PITCH,67,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,100,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,70,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,40,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,

2,PC_ENV_VOL,20,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
 
1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//vaus explodes 2/2
const char patch41[] PROGMEM ={	


0,PC_ENV_VOL,127,
0,PC_WAVE,3,
0,PC_ENV_SPEED,-1, 

0,PC_PITCH,65,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,100,
0,PC_PITCH,60,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,80,
0,PC_PITCH,55,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,50,
0,PC_PITCH,50,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


3,PC_ENV_VOL,40,
0,PC_PITCH,45,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,

3,PC_ENV_VOL,20,
0,PC_PITCH,40,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
1,PC_NOTE_DOWN,4,
//1,PC_NOTE_DOWN,4,


1,PC_NOTE_CUT,0,
0,PATCH_END 
};

//Victory
const char patch42[] PROGMEM ={	
0,PC_ENV_VOL,0,
4,PC_WAVE,1,
0,PC_ENV_VOL,0xff,
0,PC_TREMOLO_LEVEL,0x90,     
0,PC_TREMOLO_RATE,60, 
0,PC_ENV_SPEED,-5,
1,PC_PITCH,80,
5,PC_NOTE_UP,5, 
6,PC_NOTE_UP,5, 
7,PC_NOTE_UP,5, 
8,PC_NOTE_UP,5, 
9,PC_NOTE_UP,5, 
10,PC_NOTE_UP,5, 
11,PC_NOTE_CUT,0,
0,PATCH_END 


};

/////////End Sound Effects/////////////////
///////////////////////////////////////////

#define EXPLODEFX 40
#define VICTORYFX 42

const struct PatchStruct patches[] PROGMEM = {
//Song1
{0,NULL,patch00,0,0},
{0,NULL,patch01,0,0},
{0,NULL,patch02,0,0},
{1,NULL,patch03,0,0},
{1,NULL,patch04,0,0},

//Song2
{0,NULL,patch05,0,0},
{0,NULL,patch06,0,0},
{0,NULL,patch07,0,0},
{1,NULL,patch08,0,0},
{1,NULL,patch09,0,0},
{1,NULL,patch10,0,0},
{1,NULL,patch11,0,0},
{1,NULL,patch12,0,0},
{1,NULL,patch13,0,0},
{1,NULL,patch14,0,0},

//Song3
{0,NULL,patch15,0,0},
{0,NULL,patch16,0,0},
{0,NULL,patch17,0,0},
{1,NULL,patch18,0,0},
{1,NULL,patch19,0,0},
{1,NULL,patch20,0,0},
{1,NULL,patch21,0,0},
{1,NULL,patch22,0,0},

//Song4
{0,NULL,patch23,0,0},
{0,NULL,patch24,0,0},
{0,NULL,patch25,0,0},
{1,NULL,patch26,0,0},
{1,NULL,patch27,0,0},
{1,NULL,patch28,0,0},
{1,NULL,patch29,0,0},

//Song5
{0,NULL,patch30,0,0},
{0,NULL,patch31,0,0},
{0,NULL,patch32,0,0},
{1,NULL,patch33,0,0},
{1,NULL,patch34,0,0},
{1,NULL,patch35,0,0},
{1,NULL,patch36,0,0},
{1,NULL,patch37,0,0},
{1,NULL,patch38,0,0},
{1,NULL,patch39,0,0},

//Sfx
{0,NULL,patch40,0,0},
{0,NULL,patch41,0,0},
{0,NULL,patch42,0,0},

};


