////////////////////////////////////////////
/////////                          /////////
/////////START NORMAL PATCHES      /////////
////////////////////////////////////////////

////////////////////////////////////////////
/////////                          /////////
/////////CLASSIC SONG PATCHES      /////////
////////////////////////////////////////////

//Rock Organ
const char patch00[] PROGMEM ={ 
//0,PC_TREMOLO_LEVEL,180,     
//0,PC_TREMOLO_RATE,8, 
0,PC_WAVE,3,
0,PC_ENV_VOL,65,
0,PC_ENV_SPEED,8,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-7,
6,PC_NOTE_CUT,0,
0,PATCH_END


};
//MAIN LINE PATCH
const char patch01[] PROGMEM ={ 
0,PC_TREMOLO_LEVEL,100,     
0,PC_TREMOLO_RATE,60, 
0,PC_WAVE,3,
0,PC_ENV_VOL,65,
0,PC_ENV_SPEED,8,
//1,PC_ENV_SPEED,5,
0,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-3,
12,PC_NOTE_CUT,0,
0,PATCH_END
};

//Baritone Sax
const char patch02[] PROGMEM ={	
//0,PC_TREMOLO_LEVEL,180,     
//0,PC_TREMOLO_RATE,8, 
0,PC_WAVE,0,
0,PC_ENV_VOL,76,
0,PC_ENV_SPEED,8,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-7,
7,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch03[] PROGMEM ={//EXTRA FOR FUTURE SONG MODS
0,PATCH_END
};

////////////////////////////////////////////
/////////                          /////////
/////////HYPER SONG PATCHES        /////////
////////////////////////////////////////////

//Lead UNTIL solo
const char patch04[] PROGMEM ={ 
0,PC_TREMOLO_LEVEL,170,     
0,PC_TREMOLO_RATE,180, 
0,PC_WAVE,4,
0,PC_ENV_VOL,96,
0,PC_ENV_SPEED,16,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-8,
8,PC_NOTE_CUT,0,
0,PATCH_END
};

//Lead Rythm
const char patch05[] PROGMEM ={ 
//0,PC_TREMOLO_LEVEL,180,     
//0,PC_TREMOLO_RATE,8, 
0,PC_WAVE,1,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,8,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-16,
8,PC_NOTE_CUT,0,
0,PATCH_END
};

//Bass line
const char patch06[] PROGMEM ={	
//0,PC_TREMOLO_LEVEL,180,     
//0,PC_TREMOLO_RATE,8, 
0,PC_WAVE,4,
0,PC_ENV_VOL,88,
0,PC_ENV_SPEED,8,
//1,PC_NOTE_HOLD,0,
3,PC_ENV_SPEED,-16,//16
14,PC_NOTE_CUT,0,
0,PATCH_END
};

const char patch07[] PROGMEM ={//SOLO PATCH
0,PC_TREMOLO_LEVEL,170,     
0,PC_TREMOLO_RATE,180, 
0,PC_WAVE,4,
0,PC_ENV_VOL,98,
0,PC_ENV_SPEED,16,
1,PC_NOTE_HOLD,0,
0,PC_ENV_SPEED,-8,
8,PC_NOTE_CUT,0,
0,PATCH_END
};




////////////////////////////////////////////
/////////                          /////////
/////////NOISE PATCHES             /////////
////////////////////////////////////////////


////////////////////////////////////////////
////////                           /////////
////////Classic drums              /////////
////////////////////////////////////////////


//bass drum
const char patch20[] PROGMEM ={	
0,PC_NOISE_PARAMS,14,
0,PC_ENV_VOL,40,
0,PC_ENV_SPEED,30,
1,PC_ENV_SPEED,-20,
1,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-4,  
5,PC_NOTE_CUT,0,
0,PATCH_END
};

//snare drum
const char patch21[] PROGMEM ={	
///////USING PATCH 12 AS NOISE ACTUALLY SOUNDS LIKE A SNARE DRUM???!?!?!?!
//////////////////////////////////////////////////////////////////////////
0,PC_NOISE_PARAMS,8,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Hi-Hat
const char patch22[] PROGMEM ={ 
0,PC_NOISE_PARAMS,2,
0,PC_ENV_VOL,40,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
//1,PC_ENV_SPEED,-8, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char patch23[] PROGMEM ={//EXTRA FOR FUTURE SONG MODS
0,PATCH_END
};

////////////////////////////////////////////
/////////                          /////////
/////////HYPER DRUMS               /////////
////////////////////////////////////////////


//bass drum
const char patch24[] PROGMEM ={	
0,PC_NOISE_PARAMS,14,
0,PC_ENV_VOL,40,
0,PC_ENV_SPEED,30,
1,PC_ENV_SPEED,-20,
1,PC_ENV_SPEED,-8,
1,PC_ENV_SPEED,-4,  
5,PC_NOTE_CUT,0,
0,PATCH_END
};

//snare drum
const char patch25[] PROGMEM ={	
0,PC_NOISE_PARAMS,8,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
1,PC_ENV_SPEED,-8, 
2,PC_NOTE_CUT,0,
0,PATCH_END
};

//Hi-Hat
const char patch26[] PROGMEM ={ 
0,PC_NOISE_PARAMS,2,
0,PC_ENV_VOL,70,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
//1,PC_ENV_SPEED,-8, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};

const char patch27[] PROGMEM ={//EXTRA FOR FUTURE SONG MODS
0,PC_NOISE_PARAMS,22,
0,PC_ENV_VOL,80,
0,PC_ENV_SPEED,-30,
1,PC_ENV_SPEED,-15,
//1,PC_ENV_SPEED,-8, 
1,PC_NOTE_CUT,0,
0,PATCH_END
};




////////////////////////////////////////////
//////////                         /////////
//////////NORMAL EFFECT PATCHES    /////////
////////////////////////////////////////////


//Losing note roll 1 of 2
const char patch40[] PROGMEM ={	
0,PC_WAVE, 3,

0,PC_ENV_VOL,0,

32,PC_ENV_VOL,90,
0,PC_TREMOLO_LEVEL,177,
0,PC_TREMOLO_RATE,104,

0,PC_PITCH,72,
2,PC_PITCH,71,
2,PC_PITCH,70,
2,PC_PITCH,69,
2,PC_PITCH,68,
2,PC_PITCH,67,
3,PC_PITCH,66,
2,PC_PITCH,65,
3,PC_PITCH,64,
3,PC_PITCH,63,
3,PC_PITCH,62,
3,PC_PITCH,61,
3,PC_PITCH,60,
4,PC_PITCH,59,
3,PC_PITCH,58,
4,PC_PITCH,57,
5,PC_PITCH,56,
3,PC_PITCH,55,
5,PC_PITCH,54,
5,PC_PITCH,53,
5,PC_PITCH,52,
6,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,



5,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,

0,PC_TREMOLO_LEVEL,177,
0,PC_TREMOLO_RATE, 111,
0,PC_ENV_SPEED,-1,

5,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,

5,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,

5,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,

5,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,

5,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,

5,PC_PITCH,51,
2,PC_PITCH,50,
6,PC_PITCH,49,

5,PC_NOTE_CUT,0,
0,PATCH_END
};


const char patch41[] PROGMEM ={	
0,PC_WAVE, 0,

0,PC_ENV_VOL,0,

33,PC_ENV_VOL,90,//START AFTER FIRST PATCH TO CREATE SOME DEPTH
//0,PC_TREMOLO_LEVEL,144,
//0,PC_TREMOLO_RATE, 19,

0,PC_PITCH,71,
1,PC_PITCH,70,
1,PC_PITCH,69,
3,PC_PITCH,68,
2,PC_PITCH,67,
2,PC_PITCH,66,
3,PC_PITCH,65,
2,PC_PITCH,64,
3,PC_PITCH,63,
3,PC_PITCH,62,
3,PC_PITCH,61,
4,PC_PITCH,60,
2,PC_PITCH,59,
4,PC_PITCH,58,
3,PC_PITCH,57,
4,PC_PITCH,56,
5,PC_PITCH,55,
3,PC_PITCH,54,
5,PC_PITCH,53,
5,PC_PITCH,52,
5,PC_PITCH,51,
6,PC_PITCH,50,
2,PC_PITCH,49,
6,PC_PITCH,48,



3,PC_PITCH,50,
3,PC_PITCH,49,
7,PC_PITCH,48,

0,PC_TREMOLO_LEVEL,177,
0,PC_TREMOLO_RATE, 33,
0,PC_ENV_SPEED,-2,

3,PC_PITCH,50,
3,PC_PITCH,49,
7,PC_PITCH,48,

3,PC_PITCH,50,
3,PC_PITCH,49,
7,PC_PITCH,48,

3,PC_PITCH,50,
3,PC_PITCH,49,
7,PC_PITCH,48,

3,PC_PITCH,50,
3,PC_PITCH,49,
7,PC_PITCH,48,

3,PC_PITCH,50,
3,PC_PITCH,49,
7,PC_PITCH,48,

3,PC_PITCH,51,
4,PC_PITCH,50,
6,PC_PITCH,49,

5,PC_NOTE_CUT,0,
0,PATCH_END
};



////////////////////////////////////////////
/////////                          /////////
/////////START PCM PATCHES         /////////
////////////////////////////////////////////



#include "pcm.inc"
const uint8_t PCMSPEEDS[]   PROGMEM = {23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,};
const uint8_t PCMPRIORITY[] PROGMEM = {1,2,3,4,4,};
const uint8_t PCMTIMES[]    PROGMEM = {5,15,20,20,15,15,};

#define FIRSTPCMSOUND 42
#define SWAPWAVE FIRSTPCMSOUND
#define DROPWAVE SWAPWAVE+1
#define SCOREWAVE DROPWAVE+1
#define MAGICWAVE SCOREWAVE+1
#define CRASHWAVE MAGICWAVE+1

const struct PatchStruct patches[] PROGMEM = {
{0,NULL,patch00,0,0},//classic lead1
{0,NULL,patch01,0,0},//classic lead2
{0,NULL,patch02,0,0},//classic bass
{0,NULL,patch03,0,0},//classic extra

{0,NULL,patch04,0,0},//hyper lead 1
{0,NULL,patch05,0,0},//hyper lead 2
{0,NULL,patch06,0,0},//hyper bass
{0,NULL,patch07,0,0},//hyper extra

{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1


////////////////////////////////////////////////////////////
////////////////NOISE///////////////////////////////////////
////////////////////////////////////////////////////////////


{1,NULL,patch20,0,0},//classic bass drum
{1,NULL,patch21,0,0},//classic snare//NOT A MISTAKE, NOT SURE WHY 12 SOUNDS BETTER THAN 21(NOT EVEN A NOISE PATCH!!)
{1,NULL,patch22,0,0},//classic hat
{1,NULL,patch23,0,0},//classic extra

{1,NULL,patch24,0,0},//hyper bass drum
{1,NULL,patch25,0,0},//hyper snare
{1,NULL,patch26,0,0},//hyper hat
{1,NULL,patch27,0,0},//hyper extra

{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1
{0,NULL,patch07,0,0},//other lead 1



{0,NULL,patch40,0,0},//LOSING SFX
{0,NULL,patch41,0,0},//LOSING SFX

//////////////////////////////////////////////////////
///////////////////////PCM////////////////////////////
//////////////////////////////////////////////////////


{0,NULL,patch07,0,0},//other lead 1
{2,DropWave,NULL,sizeof(DropWave),sizeof(DropWave)},
{2,ScoreWave,NULL,sizeof(ScoreWave),sizeof(ScoreWave)},
{2,MagicWave,NULL,sizeof(MagicWave),sizeof(MagicWave)},
{2,CrashWave,NULL,sizeof(CrashWave),sizeof(CrashWave)},
};

