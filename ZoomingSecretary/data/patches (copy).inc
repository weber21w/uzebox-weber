/* Created with Uzebox Patch Studio 0.0.1 */
const char patch00[] PROGMEM = {//SFX_RINGTONE
  0, PC_WAVE, 3,
  0, PC_ENV_VOL, 70,
  2, PC_ENV_SPEED, -2,
  0, PC_PITCH, 64,
  2, PC_PITCH, 71,
  2, PC_PITCH, 64,
  2, PC_PITCH, 71,
  2, PC_PITCH, 64,
  2, PC_PITCH, 71,
  2, PC_PITCH, 64,
  2, PC_PITCH, 71,
  0, PATCH_END,
};

const char patch01[] PROGMEM = {//SFX_STEP1
  0, PC_WAVE, 7,
  0, PC_ENV_VOL, 25,
  0, PC_ENV_SPEED, -8,
  0, PC_PITCH, 69,
  1, PC_PITCH, 57,
  0, PATCH_END,
};

const char patch02[] PROGMEM = {//SFX_STEP2
  0, PC_WAVE, 7,
  0, PC_ENV_VOL, 25,
  0, PC_ENV_SPEED, -10,
  0, PC_PITCH, 79,
  1, PC_PITCH, 67,
  1, PC_ENV_SPEED, -16,
  0, PATCH_END,
};

const char patch03[] PROGMEM = {//SFX_STEP3
  0, PC_WAVE, 7,
  0, PC_ENV_VOL, 45,
  0, PC_ENV_SPEED, -8,
  0, PC_PITCH, 60,
  1, PC_PITCH, 48,
  1, PC_ENV_SPEED, -16,
  0, PATCH_END,
};

const char patch04[] PROGMEM = {//SFX_STEP4
  0, PC_WAVE, 7,
  0, PC_ENV_SPEED, -12,
  0, PC_ENV_VOL, 45,
  0, PC_PITCH, 67,
  1, PC_PITCH, 55,
  1, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch05[] PROGMEM = {//SFX_FALL
  0, PC_WAVE, 4,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 108,
  2, PC_NOTE_DOWN, 2,
  3, PC_NOTE_DOWN, 2,
  3, PC_NOTE_DOWN, 2,
  3, PC_NOTE_DOWN, 2,
  2, PC_NOTE_DOWN, 2,
  2, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch06[] PROGMEM = {//SFX_DROP
  0, PC_WAVE, 3,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 55,
  1, PC_PITCH, 52,
  1, PC_PITCH, 48,
  1, PC_PITCH, 45,
  1, PC_PITCH, 55,
  1, PC_PITCH, 52,
  1, PC_PITCH, 48,
  1, PC_PITCH, 45,
  1, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch07[] PROGMEM = {//SFX_TOPIC
  0, PC_WAVE, 4,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_ENV_SPEED, -5,
  0, PC_PITCH, 45,
  2, PC_PITCH, 57,
  12, PC_ENV_VOL, 66,
  2, PC_PITCH, 45,
  6, PC_PITCH, 57,
  1, PC_ENV_VOL, 66,
  0, PC_ENV_SPEED, -2,
  2, PC_PITCH, 45,
  1, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch08[] PROGMEM = {//SFX_ANSWER
  0, PC_WAVE, 0,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  3, PC_PITCH, 72,
  3, PC_PITCH, 79,
  3, PC_PITCH, 84,
  3, PC_PITCH, 60,
  3, PC_PITCH, 72,
  3, PC_PITCH, 79,
  3, PC_PITCH, 84,
  3, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch09[] PROGMEM = {//SFX_BLA1 1 of 2
  0, PC_WAVE, 8,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  1, PC_PITCH, 61,
  2, PC_PITCH, 60,
  1, PC_PITCH, 59,
  2, PC_PITCH, 58,
  0, PC_ENV_SPEED, -8,
  2, PC_PITCH, 57,
  2, PATCH_END,
};

const char patch21[] PROGMEM = {//SFX_BLA1 2 of 2
  //0, PC_WAVE, 4,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  1, PC_PITCH, 61,
  2, PC_PITCH, 60,
  1, PC_PITCH, 59,
  2, PC_PITCH, 58,
  2, PC_PITCH, 57,
  2, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch10[] PROGMEM = {//SFX_MISS 1 of 2
  0, PC_WAVE, 5,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_SPEED, -4,
  0, PC_PITCH, 69,
  0, PC_ENV_VOL, 66,
  16, PC_ENV_VOL, 66,
  16, PC_ENV_VOL, 66,
  2, PC_ENV_SPEED, -3,
  0, PATCH_END,
};

const char patch22[] PROGMEM = {//SFX_MISS 2 of 2
  //0, PC_WAVE, 8,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_SPEED, -4,
  0, PC_PITCH, 69,
  0, PC_ENV_VOL, 66,
  16, PC_ENV_VOL, 66,
  16, PC_ENV_VOL, 66,
  2, PC_ENV_SPEED, -3,
  0, PATCH_END,
};

const char patch11[] PROGMEM = {//SFX_COFFEE
  0, PC_WAVE, 5,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_SPEED, -4,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 72,
  2, PC_ENV_VOL, 66,
  0, PC_PITCH, 73,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 74,
  13, PC_ENV_VOL, 66,
  0, PC_PITCH, 73,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 74,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 75,
  9, PC_ENV_VOL, 66,
  0, PC_PITCH, 74,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 75,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 77,
  8, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 77,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 78,
  6, PC_ENV_VOL, 66,
  0, PC_PITCH, 78,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 81,
  0, PATCH_END,
};

const char patch12[] PROGMEM = {//SFX_START

  0, PC_WAVE, 9,
  0, PC_TREMOLO_LEVEL,133,
  0, PC_TREMOLO_RATE,188,
  0, PC_ENV_SPEED, -12,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 64,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 67,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 86,

  0, PC_ENV_SPEED, -14,
 // 0, PC_WAVE, 5,

  3, PC_ENV_VOL, 30,
  0, PC_PITCH, 60,
  3, PC_ENV_VOL, 30,
  0, PC_PITCH, 76,
  3, PC_ENV_VOL, 30,
  0, PC_PITCH, 79,
  3, PC_ENV_VOL, 30,
  0, PC_PITCH, 64,
  3, PC_ENV_VOL, 30,
  0, PC_PITCH, 79,
  3, PC_ENV_VOL, 30,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 30,
  0, PC_PITCH, 67,
  3, PC_ENV_VOL, 25,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 20,
  0, PC_PITCH, 86,


  3, PATCH_END,
/*
 	0,PC_WAVE,1,//modified from Alter Ego
	0,PC_ENV_VOL,132,
	0,PC_ENV_SPEED,-1,
	0,PC_PITCH,67,
	5,PC_PITCH,79,
	5,PC_PITCH,72,
	5,PC_PITCH,84,
	//0,PC_ENV_VOL,64,
	0,PC_ENV_SPEED,-2,
	5,PC_PITCH,67,
	5,PC_PITCH,79,
	5,PC_PITCH,72,
	5,PC_PITCH,84,
	//0,PC_ENV_VOL,34,
	0,PC_ENV_SPEED,-4,
	5,PC_PITCH,67,
	5,PC_PITCH,79,
	5,PC_PITCH,72,
	5,PC_PITCH,84,
	1,PC_NOTE_CUT,0,
0,PATCH_END
*/
};

const char patch13[] PROGMEM = {//SFX_PAUSE 1 of 2
  0, PC_WAVE, 3,
  0, PC_ENV_VOL, 66,
  0, PC_ENV_SPEED, -4,
  0, PC_PITCH, 84,
  6, PC_ENV_VOL, 66,
  0, PC_PITCH, 72,
  6, PC_ENV_VOL, 66,
  0, PC_PITCH, 84,
  6, PC_ENV_VOL, 66,
  0, PC_PITCH, 72,
  0, PATCH_END,
};

const char patch23[] PROGMEM = {//SFX_PAUSE 2 of 2
  //0, PC_WAVE, 3,
  0, PC_ENV_VOL, 66,
  0, PC_ENV_SPEED, -4,
  0, PC_PITCH, 84,
  6, PC_ENV_VOL, 66,
  0, PC_PITCH, 72,
  6, PC_ENV_VOL, 66,
  0, PC_PITCH, 84,
  6, PC_ENV_VOL, 66,
  0, PC_PITCH, 72,
  0, PATCH_END,
};

const char patch14[] PROGMEM = {//SFX_LOSE 1 of 2
  0, PC_WAVE, 3,
  0, PC_ENV_VOL, 66,
  0, PC_ENV_SPEED, -3,
  0, PC_PITCH, 69,
  16, PC_ENV_VOL, 66,
  0, PC_PITCH, 69,
  24, PC_ENV_VOL, 66,
  0, PC_PITCH, 69,
  0, PATCH_END,
};

const char patch24[] PROGMEM = {//SFX_LOSE 2 of 2
  //0, PC_WAVE, 3,
  0, PC_ENV_VOL, 66,
  0, PC_ENV_SPEED, -2,
  0, PC_PITCH, 69,
  16, PC_ENV_VOL, 66,
  0, PC_PITCH, 69,
  24, PC_ENV_VOL, 66,
  0, PC_PITCH, 69,
  0, PATCH_END,
};

const char patch15[] PROGMEM = {//SFX_BLA2 1 of 2
  0, PC_WAVE, 5,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_PITCH, 60,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 86,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  3, PC_ENV_VOL, 66,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 67,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 83,
  0, PC_PITCH, 86,
  0, PC_ENV_SPEED,-10,
  3, PATCH_END,
};

const char patch25[] PROGMEM = {//SFX_BLA2 2 of 2
  //0, PC_WAVE, 3,
  0, PC_TREMOLO_LEVEL, 222,
  0, PC_TREMOLO_RATE, 44,
  0, PC_PITCH, 60,
  4, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 86,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  3, PC_ENV_VOL, 66,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 67,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 83,
  0, PC_PITCH, 86,
  0, PC_ENV_SPEED,-10,
  3, PATCH_END,
};

const char patch16[] PROGMEM = {//SFX_COFFEE_READY
  0, PC_WAVE, 3,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 47,
  1, PC_PITCH, 48,
  1, PC_PITCH, 49,
  1, PC_PITCH, 50,
  1, PC_PITCH, 51,
  1, PC_PITCH, 52,
  1, PC_PITCH, 53,
  1, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch17[] PROGMEM = {//SFX_KNOCK
  0, PC_WAVE, 3,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_VOL, 66,
  0, PC_PITCH, 47,
  1, PC_PITCH, 48,
  1, PC_PITCH, 49,
  1, PC_PITCH, 50,
  1, PC_PITCH, 51,
  1, PC_PITCH, 52,
  1, PC_PITCH, 53,
  1, PC_ENV_SPEED, -8,
  0, PATCH_END,
};

const char patch18[] PROGMEM = {//SFX_WISDOM
  0, PC_WAVE, 5,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_PITCH, 60,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 86,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 76,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  3, PC_ENV_VOL, 66,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 79,
  0, PC_PITCH, 83,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 67,
  3, PC_ENV_VOL, 66,
  0, PC_PITCH, 83,
  0, PC_PITCH, 86,
  3, PATCH_END,
};

const char patch19[] PROGMEM = {//SFX_EXPLODE
  0, PC_WAVE, 5,
  0, PC_TREMOLO_LEVEL, 111,
  0, PC_TREMOLO_RATE, 88,
  0, PC_ENV_SPEED, -4,
  0, PC_PITCH, 60,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 57,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 52,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 48,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 47,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 60,
  4, PC_ENV_VOL, 66,
  0, PC_PITCH, 55,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 52,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 48,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 45,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 40,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 59,
  4, PC_ENV_VOL, 66,
  0, PC_PITCH, 55,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 52,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 48,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 45,
  1, PC_ENV_VOL, 66,
  0, PC_PITCH, 40,
  3, PATCH_END,
};

const char patch20[] PROGMEM = {//SFX_TELEPORT
  0, PC_WAVE, 5,
  0, PC_ENV_VOL, 66,
  0, PC_LOOP_START, 20,
  0, PC_PITCH, 84,
  1, PC_PITCH, 83,
  1, PC_LOOP_END, 2,
  1, PC_ENV_SPEED, -8,
  0, PATCH_END,
};



////////Music///////////////////
const char patch26[] PROGMEM = {
  0,PC_WAVE,4,
  0,PC_ENV_SPEED,-6,
  0,PATCH_END,
};

const char patch27[] PROGMEM = {
  0,PC_WAVE,4,
  0,PC_ENV_SPEED,-6,
  0,PATCH_END,
};

const char patch28[] PROGMEM = {
  0,PC_WAVE,4,
  0,PC_ENV_SPEED,-6,
  0,PATCH_END,
};

const char patch29[] PROGMEM = {
  0,PC_WAVE,4,
  0,PC_ENV_SPEED,-6,
  0,PATCH_END,
};

const char patch30[] PROGMEM = {
  0,PC_WAVE,4,
  0,PC_ENV_SPEED,-6,
  0,PATCH_END,
};

const char patch31[] PROGMEM = {
  0,PC_WAVE,4,
  0,PC_ENV_SPEED,-16,
  0,PATCH_END,
};

const char patch32[] PROGMEM = {
  0,PC_WAVE,4,
  0,PC_ENV_SPEED,-6,
  0,PATCH_END,
};

const char patch33[] PROGMEM = {//intro song lead
0,PC_WAVE,4,
//0,PC_ENV_VOL,120,
0,PC_ENV_SPEED,2,
0,PC_TREMOLO_RATE,178,
0,PC_TREMOLO_LEVEL,99,
//0,PC_NOTE_HOLD,0,
//0,PC_ENV_SPEED,-4/2,  
5,PC_ENV_SPEED,-7/2,
0,PC_WAVE,4,
//0,PC_TREMOLO_RATE,168,
//0,PC_TREMOLO_LEVEL,202,
5,PC_ENV_SPEED,-16,
//0,PC_TREMOLO_RATE,128,//added after it already sounded good.............
0,PATCH_END//40
};

const char patch34[] PROGMEM = {//intro song back
0,PC_TREMOLO_LEVEL,140,     
0,PC_TREMOLO_RATE,111, 
0,PC_WAVE,4,
//1,PC_ENV_VOL,0,
1,PC_ENV_SPEED,2,
//0,PC_NOTE_HOLD,0,
0,PC_TREMOLO_LEVEL,180,     
0,PC_TREMOLO_RATE,120, 
0,PC_ENV_SPEED,-16,
0,PATCH_END
};

const char patch35[] PROGMEM = {//intro song bass
0,PC_ENV_VOL,0,
0,PC_TREMOLO_LEVEL,170,     
0,PC_TREMOLO_RATE,160, 
0,PC_WAVE,4,
//0,PC_ENV_VOL,64,//54
//0,PC_ENV_SPEED,16,
//1,PC_NOTE_HOLD,0,
1,PC_ENV_SPEED,-10,
0,PATCH_END
};

const char patch36[] PROGMEM = {//intro song kick
0,PC_NOISE_PARAMS,9,
0,PC_ENV_VOL,0,
1,PC_NOISE_PARAMS,12,
1,PC_ENV_SPEED,-2,
1,PC_ENV_SPEED,-3,
1,PC_ENV_SPEED,-8, 
0,PATCH_END
};

const char patch37[] PROGMEM = {//intro song hat
0,PC_ENV_VOL,0,
0,PC_ENV_SPEED,-8,
0,PC_NOISE_PARAMS,6,
2,PC_NOISE_PARAMS,0x1,
0,PATCH_END 
};

const char patch38[] PROGMEM = {//intro song bass
//0,PC_ENV_VOL,0,
0,PC_TREMOLO_LEVEL,88,     
0,PC_TREMOLO_RATE,233, 
//0,PC_NOTE_UP,12,
0,PC_WAVE,6,
0,PC_ENV_SPEED,-2,
//0,PC_ENV_VOL,64,//54
//0,PC_ENV_SPEED,16,
//0,PC_NOTE_HOLD,0,
2,PC_ENV_SPEED,-8,
0,PC_TREMOLO_RATE,178, 
1,PC_WAVE,4,
0,PC_TREMOLO_LEVEL,66, 
0,PATCH_END
};

const char patch39[] PROGMEM = {//intro song kick
0,PC_NOISE_PARAMS,9,
0,PC_ENV_VOL,8,
1,PC_NOISE_PARAMS,12,
1,PC_ENV_SPEED,-2,
1,PC_ENV_SPEED,-3,
1,PC_ENV_SPEED,-8, 
0,PATCH_END
};

const char patch40[] PROGMEM ={//D3thAdd3r logo part 1
	0,PC_WAVE,0,
	0,PC_PITCH,40,
	0,PC_ENV_VOL,190,
	0,PC_SLIDE_SPEED,224,
	0,PC_SLIDE,-30,
	15,PC_ENV_SPEED,-1,
	180,PATCH_END
};

const char patch41[] PROGMEM ={//D3thAdd3r logo part 2
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

const uint8_t sfxdepth[] PROGMEM = {//for 2 part sfx
1,255,//SFX_RINGTONE
2,255,//SFX_STEP1
2,255,//SFX_STEP2
2,255,//SFX_STEP3
2,255,//SFX_STEP4
2,255,//SFX_FALL
2,255,//SFX_DROP
1,255,//SFX_TOPIC
1,255,//SFX_ANSWER
2,21,//SFX_BLA1
1,22,//SFX_MISS
1,255,//SFX_COFFEE
1,255,//SFX_START
1,23,//SFX_PAUSE
1,24,//SFX_LOSE
2,25,//SFX_BLA2
1,255,//SFX_COFFEE_READY
1,255,//SFX_KNOCK
1,255,//SFX_WISDOM
1,255,//SFX_EXPLODE
1,255,//SFX_TELEPORT
};

extern const char waves[];
const struct PatchStruct patches[] PROGMEM = {
//sfx
{0,NULL,patch00,0,256},//SFX_RINGTONE
{0,NULL,patch01,0,256},//SFX_STEP1
{0,NULL,patch02,0,256},//SFX_STEP2
{0,NULL,patch03,0,256},//SFX_STEP3
{0,NULL,patch04,0,256},//SFX_STEP4
{0,NULL,patch05,0,256},//SFX_FALL
{0,NULL,patch06,0,256},//SFX_DROP
{0,NULL,patch07,0,256},//SFX_TOPIC
{0,NULL,patch08,0,256},//SFX_ANSWER
{0,NULL,patch09,0,256},//SFX_BLA1 1 of 2 plays with patch21
{0,NULL,patch10,0,256},//SFX_MISS 1 of 2 plays with patch22
{0,NULL,patch11,0,256},//SFX_COFFEE
{0,NULL,patch12,0,256},//SFX_START
{0,NULL,patch13,0,256},//SFX_PAUSE 1 of 2 plays with patch23
{0,NULL,patch14,0,256},//SFX_LOSE 1 of 2 plays with patch24
{0,NULL,patch15,0,256},//SFX_BLA2 1 of 2 plays with patch25
{0,NULL,patch16,0,256},//SFX_COFFEE_READY
{0,NULL,patch17,0,256},//SFX_KNOCK TODO
{0,NULL,patch18,0,256},//SFX_WIDSOM TODO
{0,NULL,patch19,0,256},//SFX_EXPLODE
{0,NULL,patch20,0,256},//SFX_TELEPORT
{2,waves+(256*4),patch21,0,256},//SFX_BLA1 2 of 2 plays with patch09
{2,waves+(256*8),patch22,0,256},//SFX_MISS 2 of 2 plays with patch10
{2,waves+(256*3),patch23,0,256},//SFX_PAUSE 2 of 2 plays with patch13
{2,waves+(256*3),patch24,0,256},//SFX_LOSE 2 of 2 plays with patch 14
{2,waves+(256*3),patch25,0,256},//SFX_BLA2 2 of 2 plays with patch 15


{0,NULL,patch26,0,0},//intro song lead instrument
{0,NULL,patch27,0,0},
{0,NULL,patch28,0,0},
{0,NULL,patch29,0,0},
{0,NULL,patch30,0,0},
{0,NULL,patch31,0,0},//fired song instrument 1
{0,NULL,patch32,0,0},//fired song instrument 2
{0,NULL,patch33,0,0},//intro song instrument 1
{0,NULL,patch34,0,0},//intro song instrument 2
{0,NULL,patch35,0,0},//intro song instrument 3
{1,NULL,patch36,0,0},//intro song instrument 4(kick)
{1,NULL,patch37,0,0},//intro song instrument 5(hat)
{0,NULL,patch38,0,0},//game song instrument 1
{1,NULL,patch39,0,0},//game song instrument 2(kick)

{0,waves+(256*0),patch40,0,256},//D3thAdd3r logo part 1
{2,waves+(256*2),patch41,0,256},//part 2


//instruments
};
