//Note, these patches rely on a custom sounds.inc to sound correct. Use the commented out number if using stock sounds.inc
//ie. "PC_WAVE,4,//5," should become "PC_WAVE,5,"

//intro song lead
const char patch00[] PROGMEM ={ 
0,PC_WAVE,5,//5,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-9,
0,PATCH_END
};

//intro song bass
const char patch01[] PROGMEM ={	
0,PC_WAVE,4,//4,
0,PC_ENV_VOL,70,
0,PC_ENV_SPEED,-6,
0,PATCH_END
};


//main theme lead
const char patch02[] PROGMEM ={	
0,PC_WAVE,5,//5,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-6,
0,PATCH_END
};


//main theme bass
const char patch03[] PROGMEM ={	
0,PC_WAVE,4,//4,
0,PC_ENV_VOL,80,
0,PC_ENV_SPEED,-7,
0,PATCH_END
};

//round clear lead
const char patch04[] PROGMEM ={	
0,PC_WAVE,1,//1,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-7,
0,PATCH_END
};

//round clear bass
const char patch05[] PROGMEM ={	
0,PC_WAVE,1,//1,
1,PC_ENV_SPEED,-20, 
0,PATCH_END
};


//
const char patch06[] PROGMEM ={	
0,PC_WAVE,1,//1,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-7,
0,PATCH_END
};

//
const char patch07[] PROGMEM ={	
0,PC_WAVE,5,//5,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-6,
0,PATCH_END
};

//
const char patch08[] PROGMEM ={
0,PC_WAVE,4,//4,
0,PC_ENV_VOL,70,
0,PC_ENV_SPEED,-6,
0,PATCH_END

};

//
const char patch09[] PROGMEM ={//1up song lead
0,PC_WAVE,5,//5,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-3,
0,PATCH_END
};

const char patch10[] PROGMEM ={//1up song bass
0,PC_WAVE,4,//4,
0,PC_ENV_VOL,70,
0,PC_ENV_SPEED,-3,
0,PATCH_END

};

const char patch11[] PROGMEM ={	
0,PC_WAVE,5,//5,
0,PC_ENV_VOL,60,
0,PC_ENV_SPEED,-6,
0,PATCH_END

};

const char patch12[] PROGMEM ={	
0,PC_WAVE,4,//4,
0,PC_ENV_VOL,80,
0,PC_ENV_SPEED,-7,
0,PATCH_END
};

//player dies 1 of 2
const char patch13[] PROGMEM ={
0,PC_ENV_SPEED,0,
0,PC_WAVE,3,//0,
0,PC_ENV_VOL,80,//80
0,PC_PITCH,79,
4,PC_PITCH,78,
4,PC_PITCH,77,
4,PC_PITCH,76,
4,PC_PITCH,77,
4,PC_PITCH,76,
4,PC_PITCH,75,
4,PC_PITCH,74,
4,PC_PITCH,75,
4,PC_PITCH,74,
4,PC_PITCH,73,
4,PC_PITCH,72,
4,PC_PITCH,73,
4,PC_PITCH,72,
4,PC_PITCH,71,
4,PC_PITCH,70,
4,PC_PITCH,69,
4,PC_PITCH,77,
4,PC_PITCH,73,
4,PC_PITCH,81,
0,PC_ENV_SPEED,-14,
4,PATCH_END,
};


//player dies 2 of 2
const char patch14[] PROGMEM ={
0,PC_ENV_SPEED,0,
0,PC_WAVE,3,//1,
0,PC_ENV_VOL,80,//80
0,PC_PITCH,73,
4,PC_PITCH,72,
4,PC_PITCH,71,
4,PC_PITCH,70,
4,PC_PITCH,71,
4,PC_PITCH,70,
4,PC_PITCH,69,
4,PC_PITCH,68,
4,PC_PITCH,69,
4,PC_PITCH,68,
4,PC_PITCH,67,
4,PC_PITCH,66,
4,PC_PITCH,67,
4,PC_PITCH,66,
4,PC_PITCH,65,
4,PC_PITCH,64,
4,PC_PITCH,63,
4,PC_PITCH,71,
4,PC_PITCH,67,
4,PC_PITCH,75,
0,PC_ENV_SPEED,-14,
4,PATCH_END,
};

//pause
const char patch15[] PROGMEM ={
0,PC_ENV_SPEED,0,
0,PC_WAVE,4,//4,
0,PC_ENV_VOL,90,
0,PC_PITCH,80,
4,PC_PITCH,81,
4,PC_PITCH,84,
4,PC_PITCH,88,
4,PC_PITCH,91,
4,PC_PITCH,88,
4,PC_PITCH,84,
4,PC_PITCH,81,
4,PC_ENV_SPEED,-20,
0,PATCH_END
};

//player hit 1 of 2
const char patch16[] PROGMEM ={
0,PC_ENV_SPEED,0,
0,PC_WAVE,4,//4,
0,PC_ENV_VOL,90,
0,PC_PITCH,73,
4,PC_PITCH,77,
4,PC_PITCH,81,
4,PC_PITCH,84,
4,PC_PITCH,73,
4,PC_ENV_SPEED,-20,
0,PATCH_END
};

//player hit 2 of 2
const char patch17[] PROGMEM ={
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_PITCH,71,
4,PC_PITCH,67,
4,PC_PITCH,63,
4,PC_PITCH,60,
4,PC_PITCH,71,
4,PC_ENV_SPEED,-20,
0,PATCH_END
};

//grab bonus
const char patch18[] PROGMEM ={
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_PITCH,83,
4,PC_PITCH,84,
4,PC_PITCH,87,
4,PC_PITCH,88,
4,PC_PITCH,90,
4,PC_PITCH,93,
4,PC_PITCH,91,
4,PC_PITCH,88,
4,PC_ENV_SPEED,-20,
0,PATCH_END
};

//pop
const char patch19[] PROGMEM ={
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_PITCH,54,
2,PC_PITCH,49,
2,PC_PITCH,68,
2,PC_PITCH,63,
2,PC_PITCH,70,
2,PC_PITCH,77,
1,PC_TREMOLO_LEVEL,111,
0,PC_TREMOLO_RATE,4,
1,PC_WAVE,4,//5,
0,PC_PITCH,84,
2,PC_PITCH,83,
2,PC_PITCH,84,
2,PC_PITCH,85,
2,PC_PITCH,84,
2,PC_ENV_SPEED,-20,
0,PATCH_END
};

//squish enemy
const char patch20[] PROGMEM = {
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_PITCH,54,
1,PC_PITCH,57,
1,PC_PITCH,56,
1,PC_PITCH,59,
1,PC_PITCH,58,
1,PC_PITCH,61,
1,PC_PITCH,60,
1,PC_PITCH,63,
1,PC_PITCH,62,
1,PC_PITCH,65,
1,PC_PITCH,64,
1,PC_PITCH,67,
1,PC_PITCH,66,
1,PC_PITCH,69,
1,PC_PITCH,68,
1,PC_PITCH,71,
1,PC_PITCH,70,
1,PC_PITCH,73,
1,PC_PITCH,72,
1,PC_PITCH,75,
1,PC_PITCH,74,
1,PC_PITCH,77,
1,PC_ENV_SPEED,-50,
0,PATCH_END
};

//harpoon hit enemy
const char patch21[] PROGMEM = {
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_PITCH,90,
1,PC_PITCH,95,
1,PC_PITCH,91,
1,PC_PITCH,96,
1,PC_PITCH,92,
1,PC_PITCH,97,
1,PC_PITCH,93,
1,PC_PITCH,98,
1,PC_PITCH,94,
1,PC_PITCH,99,
1,PC_PITCH,95,
1,PC_PITCH,100,
1,PC_PITCH,96,
1,PC_PITCH,101,
0,PC_WAVE,1,//1,
0,PC_ENV_SPEED,-3,
0,PC_TREMOLO_LEVEL,122,
0,PC_TREMOLO_RATE,180,
1,PC_PITCH,65,
2,PC_PITCH,64,
2,PC_PITCH,63,
2,PC_PITCH,62,
2,PC_ENV_SPEED,-20,
0,PATCH_END
};

//shoot harpoon
const char patch22[] PROGMEM = {
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_PITCH,86,
2,PC_PITCH,90,
2,PC_PITCH,91,
2,PC_PITCH,95,
2,PC_PITCH,86,
2,PC_PITCH,90,
2,PC_PITCH,91,
2,PC_PITCH,95,
2,PC_PITCH,86,
2,PC_PITCH,90,
2,PC_PITCH,91,
2,PC_PITCH,95,
2,PC_ENV_SPEED,-20,
0,PATCH_END
};

//rock drop
const char patch23[] PROGMEM = {
0,PC_ENV_SPEED,0,
0,PC_WAVE,1,//1,
0,PC_ENV_VOL,90,
0,PC_PITCH,42,
2,PC_PITCH,55,
2,PC_PITCH,68,
2,PC_PITCH,81,
0,PC_WAVE,0,//0,
2,PC_PITCH,112,
2,PC_PITCH,107,
2,PC_PITCH,111,
2,PC_PITCH,108,
2,PC_ENV_SPEED,-20,
0,PATCH_END
};

//rock land
const char patch24[] PROGMEM = {
0,PC_ENV_SPEED,0,
0,PC_WAVE,0,//0,
0,PC_ENV_VOL,90,
0,PC_TREMOLO_LEVEL,90,
0,PC_TREMOLO_RATE,33,
0,PC_PITCH,54,
1,PC_PITCH,53,
1,PC_PITCH,54,
1,PC_PITCH,55,
1,PC_PITCH,54,
1,PC_PITCH,53,
1,PC_PITCH,54,
1,PC_PITCH,55,
1,PC_PITCH,59,
1,PC_PITCH,58,
1,PC_PITCH,59,
1,PC_PITCH,60,
1,PC_PITCH,59,
1,PC_PITCH,58,
1,PC_PITCH,59,
1,PC_PITCH,60,
1,PC_ENV_SPEED,-20,
0,PATCH_END
};

//Fygar flame
const char patch25[] PROGMEM = {
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_TREMOLO_LEVEL,90,
0,PC_TREMOLO_RATE,33,
0,PC_PITCH,54,
1,PC_PITCH,88,
1,PC_PITCH,83,
1,PC_PITCH,88,
1,PC_PITCH,55,
1,PC_PITCH,88,
1,PC_PITCH,83,
1,PC_PITCH,88,
1,PC_PITCH,56,
1,PC_PITCH,88,
1,PC_PITCH,83,
1,PC_PITCH,88,
1,PC_PITCH,57,
1,PC_PITCH,88,
1,PC_PITCH,83,
1,PC_PITCH,88,
0,PC_PITCH,58,
1,PC_PITCH,88,
1,PC_PITCH,83,
1,PC_PITCH,88,
1,PC_PITCH,57,
1,PC_PITCH,88,
1,PC_PITCH,83,
1,PC_PITCH,88,
1,PC_ENV_SPEED,-20,
0,PATCH_END
};

//enemy roaming
const char patch26[] PROGMEM = {
0,PC_ENV_SPEED,0,
0,PC_WAVE,2,//2,
0,PC_ENV_VOL,90,
0,PC_TREMOLO_LEVEL,90,
0,PC_TREMOLO_RATE,33,
0,PC_PITCH,90,
2,PC_PITCH,91,
2,PC_PITCH,90,
2,PC_PITCH,87,
2,PC_PITCH,90,
2,PC_PITCH,91,
2,PC_PITCH,90,
2,PC_PITCH,87,
2,PC_PITCH,78,
2,PC_PITCH,79,
2,PC_PITCH,78,
2,PC_PITCH,75,
2,PC_PITCH,78,
2,PC_PITCH,79,
2,PC_PITCH,78,
2,PC_PITCH,75,
1,PC_ENV_SPEED,-20,
0,PATCH_END
};

const char patch27[] PROGMEM ={//uzebox logo sound 1 of 2
0,PC_WAVE,2,//2,
0,PC_PITCH,85,
4,PC_PITCH,90,
0,PC_ENV_SPEED,-8,
0,PC_TREMOLO_LEVEL,0x90,
0,PC_TREMOLO_RATE,30,
50,PC_NOTE_CUT,0,
0,PATCH_END
};

const char patch28[] PROGMEM ={//uzebox logo sound 2 of 2
0,PC_WAVE,2,//2,
0,PC_PITCH,85,
4,PC_PITCH,90,
0,PC_ENV_SPEED,-8,
0,PC_TREMOLO_LEVEL,0x90,
0,PC_TREMOLO_RATE,30,
50,PC_NOTE_CUT,0,
0,PATCH_END
};








const struct PatchStruct patches[] PROGMEM = {
{0,NULL,patch00,0,0},
{0,NULL,patch01,0,0},
{0,NULL,patch02,0,0},
{0,NULL,patch03,0,0},
{0,NULL,patch04,0,0},
{0,NULL,patch05,0,0},
{0,NULL,patch06,0,0},
{0,NULL,patch07,0,0},
{0,NULL,patch08,0,0},
{0,NULL,patch09,0,0},
{0,NULL,patch10,0,0},
{0,NULL,patch11,0,0},
{0,NULL,patch12,0,0},
{0,NULL,patch13,0,0},
{0,NULL,patch14,0,0},
{0,NULL,patch15,0,0},
{0,NULL,patch16,0,0},
{0,NULL,patch17,0,0},
{0,NULL,patch18,0,0},
{0,NULL,patch19,0,0},
{0,NULL,patch20,0,0},
{0,NULL,patch21,0,0},
{0,NULL,patch22,0,0},
{0,NULL,patch23,0,0},
{0,NULL,patch24,0,0},
{0,NULL,patch25,0,0},
{0,NULL,patch26,0,0},
{0,NULL,patch27,0,0},
{0,NULL,patch28,0,0},
};

#define SFX_PLAYER_DYING1	13//channel 1
#define SFX_PLAYER_DYING2	14//channel 2
#define SFX_PAUSE		15//channel 1
#define SFX_PLAYER_HIT1	16//channel 1
#define SFX_PLAYER_HIT2	17//channel 2
#define SFX_GRAB_BONUS	18//channel 3
#define SFX_POP			19//channel 3
#define SFX_SQUISH_ENEMY	20//channel 3
#define SFX_HIT_ENEMY		21//channel 3
#define SFX_PUMP		SFX_HIT_ENEMY//channel 3
#define SFX_HARPOON_SHOT	22//channel 3
#define SFX_ROCK_DROP		23//channel 3
#define SFX_ROCK_LAND		24//channel 3
#define SFX_FYGAR_FLAME	25//channel 3
#define SFX_ENEMY_ROAM	26//channel 3
#define SFX_UZEBOX_KLING1	27//channel 1
#define SFX_UZEBOX_KLING2	28//channel 2

const uint8_t sfx_attributes[] PROGMEM = {
//priority,channel,2 part
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0,0,0,//instrument
0b11,0,1,//SFX_PLAYER_DYING1
0b11,1,0,//SFX_PLAYER_DYING2
0b11,0,0,//SFX_PAUSE
0b11,0,1,//SFX_PLAYER_HIT1
0b11,1,0,//SFX_PLAYER_HIT2
0b10,0,0,//SFX_GRAB_BONUS
0b01,0,0,//SFX_POP
0b10,0,0,//SFX_SQUISH_ENEMY
0b01,1,0,//SFX_HIT_ENEMY
0b01,1,0,//SFX_PUMP
0b01,1,0,//SFX_HARPOON_SHOT//never happens when music is playing
0b10,1,0,//SFX_ROCK_DROP
0b10,1,0,//SFX_ROCK_LAND
0b10,2,0,//SFX_FYGAR_FLAME
0b00,2,0,//SFX_ENEMY_ROAM
0b11,0,1,//SFX_UZEBOX_KLING1
0b11,1,0,//SFX_UZEBOX_KLING2
};