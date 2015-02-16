//////////////BUILD CONTROL FOR DIFFERENT EPISODES//////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define SOKOBAN_EPISODE_NUMBER 0//original episode
//#define SOKOBAN_EPISODE_NUMBER 1  //the first level pack


#if SOKOBAN_EPISODE_NUMBER == 0

	#define SOKOBAN_WORLD_ID 62
	#define EPISODE_SAVE_OFFSET 0
	#include "data/ep1Maps.inc"
	#include "data/ep1Demos.inc"

#else// SOKOBAN_EPISODE_NUMBER == 1

	#define SOKOBAN_WORLD_ID 62//will need to change to 63 for next 2 episodes(room for 2 eps. per block)
	#define EPISODE_SAVE_OFFSET 15//offset into save block for new episode
	#define NOENDING 1//needed to fit large demos
	#define NOINTRO 1//ditto
	#include "data/ep2Maps.inc"
	#include "data/ep2Demos.inc"

#endif

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




#define TITLEWAITSEC 5

#define TITLEWAITTIME  TITLEWAITSEC*60
#define SCREEN_WIDTH  30*8
#define SCREEN_HEIGHT 28*8



#define NUMTILES 6
#define NUMTILESETS 5
#define NUMTILESPERSET NUMTILES*4
#define NUMSPRITESPERSET 36

#define BLANK  0
#define FLOOR  1
#define TARGET 2
#define BLOCK  3
#define TBLOCK 4
#define WALL	5
#define SOKOSTART 6//not counted

#define LEVELWIDE 15
#define LEVELHIGH 14
#define LEVELSIZE LEVELWIDE*LEVELHIGH
#define LEVELINFOSIZE 1//tile set
#define LEVELTOTALSIZE LEVELSIZE+LEVELINFOSIZE

#define MOVELISTSIZE 64//undo/redo where 4 moves/byte (64=256moves)

u8 spritecount;
u8 tileset;

u8 level;
u8 opentargets;
u8 demo;
/*
u8 movepos,nummoves;
u8 movelist[MOVELISTSIZE];
*/
int moves,pushes;
int optimummoves,optimumpushes;
bool levelcompleted,levelparmet;//whether the level has been completed before and the par met
u8 numlevels;

u8 demonum;
bool demoplaying;

bool musicoff = false;//user wants the music off, don't restart it
bool musicplaying = true;//a song is currently playing
bool musicpreference = false;//user has manually set music, don't change this setting from now on
bool graphicspreference = false;
u8	preferredtrack = 0;

unsigned int padstate,oldpadstate;
#define UP		1
#define DOWN	 2
#define LEFT	 4
#define RIGHT	8
#define MOVE	16
#define PUSH	32

u8 px,py,pstate,poffset;
u8 pframe,pftime;
#define NUMPFRAMES 0
#define PFRAMELENGTH 7

u8 bx,by,bnx,bny;

u8 guistate,gamestate;
bool			 guijuststarted;
u8 cursorpos;
#define GMAINMENU	 1
#define GLEVELSELECT 2
#define GOPTIONS	  3
#define GEDITOR		4
#define GVICTORY	  5
#define GHELP		  6
#define GINGAMEMENU  7

#define SCANNINGOPTIMUM 64//we are only running logic to calculate the optimum moves/pushes for a solution. kind of ugly hack.
#define FASTMOVE 128


u8 tracknum;

u8 outeffectnum,ineffectnum;
u8 menurestore[4];




#include "functiondefs.h"
