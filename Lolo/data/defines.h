//All offsets based on Castlevania(u) PRG 0
////////////////////////////////////////////

#define NUMLEVELS 7
#define NUMENEMIES 401

#define NUMUNDERWORLDCOLUMNTABLES 10

unsigned int ColumnTablePointers[16];

const int numscreens[NUMLEVELS] = {16,15,19,16,19,14,5};
const int levelwidths[NUMLEVELS] = {14,8,14,15,9,9,3,};
const int levelheights[NUMLEVELS] =  {2, 4, 3, 2,4,2,2,};
const int numstages[NUMLEVELS] = {4,3,3,3,3,2,1,};
const int numdoors[NUMLEVELS] = {2,2,2,2,2,1,0,};
const int romdooroffsets[NUMLEVELS] = {};
const int romstairoffsets[] = {
0x1FBD8,0x1FBDA,0x1FBDC,0x1FBDE,
0x1FBE0,0x1FBE2,0x1FBE4,
0x1FBE6,0x1FBE8,0x1FBEA,
0x1FBEC,0x1FBEE,0x1FBF0,
0x1FBF2,0x1FBF4,0x1FBF6,
0x1FBF8,0x1FBFA,
0x1FBFC,
};

const unsigned int firstscreenoffset[NUMLEVELS] = {65562,67685,69078,70763,72529,74263,76052};
const int numPatterns[NUMLEVELS] = {78,38,44,57,47,56,43};
const unsigned int Patternsoffsets[NUMLEVELS] = {0x1031A,0x10B35,0x11166,0x1176B,0x11EE1,0x12547,0x12A04};
const unsigned int NameTableOffsets[NUMLEVELS] = {0x6018,0x8898,0x9A18,};
const unsigned int palletoffsets[NUMLEVELS] = {0x1CDEB,0x1CDFE,0x1CE11,0x1CE24,0x1CE37,0x1CE4A,0x1CE5D};
const unsigned int gfxoffsets[NUMLEVELS] = {0x6018,0x8898,0x9A18,0xAB98,0xC018,0xD198,0xE318,};
const int LevelStartX[NUMLEVELS] = {0,7,2,0,3,8,2,};
const int LevelStartY[NUMLEVELS] = {0,3,2,1,3,1,1,};

