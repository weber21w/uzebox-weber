const uint8_t lvl_data[] PROGMEM = {

//level 1
	10,(250/5),2,//calls, delay between calls, number of topics

	//player
	10,9,	

	//NPC
	255,

	//platforms
	4,11,22,
	0,19,30,
	255,
	
	//ladders
	9,11,8,
	17,11,8,
	255,

	//cabinets
	5,16,0,
	23,16,1,
	255,

	//desks
	5,9,
	22,9,
	255,

	//coffee machine
	255,

	//clock
	14,5,
	255,

	//door
	13,8,
	255,

//level 2
	15,(250/5),2,//calls, delay between calls, number of topics
	
	//player
	10,10,	

	//NPC
	10,19,2,

	//platforms
	0,7,12,
	18,7,12,
	0,15,30,
	0,23,30,
	255,

	//ladders
	9,7,8,
	19,7,8,
	2,15,8,
	14,15,8,
	26,15,8,
	255,

	//cabinets
	7,20,0,
	21,20,1,
	255,

	//desks
	22,5,
	5,13,
	22,13,
	255,

	//coffee machine
	5,3,
	255,

	//clock
	14,8,
	255,

	//door

	255,

//level 3
	20,(250/5),3,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	10,10,1,

	//platforms
	0,7,30,
	0,15,30,
	0,23,30,
	255,

	//ladders
	1,7,8,
	27,7,8,
	7,15,8,
	21,15,8,
	255,

	//cabinets
	4,12,0,
	24,12,1,
	14,20,2,
	255,

	//desks
	13,13,
	2,21,
	26,21,
	255,

	//coffee machine
	13,3,
	255,

	//clock

	255,

	//door

	255,

//level 4
	25,(250/5),3,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,7,12,
	18,7,12,
	0,15,30,
	0,23,30,

	255,

	//ladders
	9,3,4,
	19,3,4,
	2,7,8,
	26,7,8,
	5,15,8,
	23,15,8,
	9,23,3,
	19,23,3,
	255,

	//cabinets
	2,20,0,
	14,20,1,
	26,20,2,
	255,

	//desks
	5,5,
	22,5,

	255,

	//coffee machine
	13,11,
	255,

	//clock

	255,

	//door

	255,

//level bonus
	1,(0/5),0,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	6,7,18,
	2,15,26,
	0,23,30,
	255,

	//ladders
	11,7,8,
	17,7,8,
	3,15,8,
	25,15,8,
	255,

	//cabinets

	255,

	//desks
	7,5,
	20,5,
	7,13,
	20,13,
	7,21,
	20,21,
	255,

	//coffee machine
	255,

	//clock

	255,

	//door

	255,

//level 5
	30,(275/5),4,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	4,7,22,
	0,15,30,
	0,23,30,
	255,

	//ladders
	14,2,6,
	5,15,8,
	23,15,8,
	14,23,3,
	255,

	//cabinets
	5,4,1,
	23,4,3,
	1,20,0,
	27,20,2,
	
	255,

	//desks
	8,13,
	19,13,
	18,21,
	255,

	//coffee machine
	9,19,
	255,

	//clock

	255,

	//door

	255,

//level 6
	35,(400/5),4,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,7,6,
	9,8,12,
	24,7,6,
	0,15,12,
	18,15,12,
	0,23,30,
	255,

	//ladders
	4,3,4,
	15,3,5,
	24,3,4,
	7,15,8,
	21,15,8,
	4,23,3,
	15,23,3,
	24,23,3,
	255,

	//cabinets
	10,12,0,
	18,12,2,
	1,20,1,
	27,20,3,
	255,

	//desks
	2,13,
	25,13,
	10,21,
	17,21,

	255,

	//coffee machine
	11,4,
	255,

	//clock

	255,

	//door

	255,

//level 7
	40,(450/5),4,//calls, delay between calls, number of topics

	//player
	10,10,	

	//NPC
	255,

	//platforms
	0,7,30,
	0,15,30,
	0,23,30,
	255,

	//ladders
	12,3,4,
	2,7,8,
	16,7,8,
	26,7,8,
	5,15,8,
	23,15,8,
	12,23,3,
	255,

	//cabinets
	8,12,0,
	20,12,2,
	8,20,1,
	20,20,3,
	255,

	//desks
	6,5,
	20,5,
	1,21,
	26,21,
	255,

	//coffee machine
	15,20,
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
