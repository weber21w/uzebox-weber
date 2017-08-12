const uint8_t lvl_data[] PROGMEM = {

//level 1
	10,250,2,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	4,10,22,
	0,18,30,
	255,
	
	//ladders
	9,10,8,
	17,10,8,
	255,

	//cabinets
	5,15,0,
	23,15,1,
	255,

	//desks
	5,8,
	22,8,
	255,

	//coffee machine
	255,

	//clock
	14,4,
	255,

	//door
	13,7,
	255,

//level 2
	15,250,2,//calls, delay between calls, number of topics
	
	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,6,12,
	18,6,12,
	0,14,30,
	0,22,30,
	255,

	//ladders
	9,6,8,
	19,6,8,
	2,14,8,
	14,14,8,
	26,14,8,
	255,

	//cabinets
	7,19,0,
	21,19,1,
	255,

	//desks
	22,4,
	5,12,
	22,12,
	255,

	//coffee machine
	5,2,
	255,

	//clock
	14,7,
	255,

	//door

	255,

//level 3
	20,250,3,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,6,30,
	0,14,30,
	0,22,30,
	255,

	//ladders
	1,6,8,
	27,6,8,
	7,14,8,
	21,14,8,
	255,

	//cabinets
	4,11,0,
	24,11,1,
	14,19,2,
	255,

	//desks
	13,12,
	2,20,
	26,20,
	255,

	//coffee machine
	13,2,
	255,

	//clock

	255,

	//door

	255,

//level 4
	25,250,3,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,6,12,
	18,6,12,
	0,14,30,
	0,22,30,

	255,

	//ladders
	9,2,4,
	19,2,4,
	2,6,8,
	26,6,8,
	5,14,8,
	23,14,8,
	9,22,3,
	19,22,3,
	255,

	//cabinets
	2,19,0,
	14,19,1,
	26,19,2,
	255,

	//desks
	5,4,
	22,4,

	255,

	//coffee machine
	13,10,
	255,

	//clock

	255,

	//door

	255,

//level bonus
	1,0,0,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	6,6,18,
	2,14,26,
	0,22,30,
	255,

	//ladders
	11,6,8,
	17,6,8,
	3,14,8,
	25,14,8,
	255,

	//cabinets

	255,

	//desks
	7,4,
	20,4,
	7,12,
	20,12,
	7,20,
	20,20,
	255,

	//coffee machine
	255,

	//clock

	255,

	//door

	255,

//level 5
	30,275,4,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	4,6,22,
	0,14,30,
	0,22,30,
	255,

	//ladders
	14,1,6,
	5,14,8,
	23,14,8,
	14,22,3,
	255,

	//cabinets
	5,3,1,
	23,3,3,
	1,19,0,
	27,19,2,
	
	255,

	//desks
	8,12,
	19,12,
	18,20,
	255,

	//coffee machine
	9,18,
	255,

	//clock

	255,

	//door

	255,

//level 6
	35,400,4,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,6,6,
	9,7,12,
	24,6,6,
	0,14,12,
	18,14,12,
	0,22,30,
	255,

	//ladders
	4,2,4,
	15,2,5,
	24,2,4,
	7,14,8,
	21,14,8,
	4,22,3,
	15,22,3,
	24,22,3,
	255,

	//cabinets
	10,11,0,
	18,11,2,
	1,19,1,
	27,19,3,
	255,

	//desks
	2,12,
	25,12,
	10,20,
	17,20,

	255,

	//coffee machine
	11,3,
	255,

	//clock

	255,

	//door

	255,

//level 7
	40,450,4,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,6,30,
	0,14,30,
	0,22,30,
	255,

	//ladders
	12,2,4,
	2,6,8,
	16,6,8,
	26,6,8,
	5,14,8,
	23,14,8,
	12,22,3,
	255,

	//cabinets
	8,11,0,
	20,11,2,
	8,19,1,
	20,19,3,
	255,

	//desks
	6,4,
	20,4,
	1,20,
	26,20,
	255,

	//coffee machine
	15,18,
	255,

	//clock

	255,

	//door

	255,
};


#include "title_map.inc"
#include "gameover_map.inc"
#include "welldone_map.inc"
#include "metasprite_map.inc"
#include "animations.inc"
