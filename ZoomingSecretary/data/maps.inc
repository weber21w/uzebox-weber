const uint8_t lvl_data[] PROGMEM = {

//level 1

	0,10,(250/5),//tile set, calls, delay between calls

	//player
	15,18,	

	//NPC
	0,0,255,
	255,

	//platforms
	5,11,22,
	0,19,32,
	255,
	
	//ladders
	10,11,8,
	20,11,8,
	255,

	//cabinets
	6,16,0,
	24,16,1,
	255,

	//desks
	6,9,
	23,9,
	255,

	//coffee machine
	255,

	//clock
	15,5,
	255,

	//door
	14,8,
	255,

//level 2
	1,15,(250/5),//tile set, calls, delay between calls
	
	//player
	2,14,	

	//NPC
	28,12,1,//chief
	255,

	//platforms
	0,7,13,
	19,7,13,
	0,15,32,
	0,23,32,
	255,

	//ladders
	10,7,8,
	20,7,8,
	3,15,8,
	15,15,8,
	27,15,8,
	255,

	//cabinets
	8,20,0,
	22,20,1,
	255,

	//desks
	23,5,
	6,13,
	23,13,
	255,

	//coffee machine
	6,3,
	255,

	//clock
	15,8,
	255,

	//door
	255,

//level 3
	2,20,(250/5),//tileset, calls, delay between calls

	//player
	4,6,	

	//NPC
	27,4,2,
	255,

	//platforms
	0,7,32,
	0,15,32,
	0,23,32,
	255,

	//ladders
	2,7,8,
	28,7,8,
	8,15,8,
	22,15,8,
	255,

	//cabinets
	5,12,0,
	25,12,1,
	15,20,2,
	255,

	//desks
	14,13,
	3,21,
	27,21,
	255,

	//coffee machine
	14,3,
	255,

	//clock
	255,

	//door
	255,

//level 4
	3,25,(250/5),//tile set, calls, delay between calls

	//player
	3,6,	

	//NPC
	19,12,3,
	255,

	//platforms
	0,7,13,
	19,7,13,
	0,15,32,
	0,23,32,
	255,

	//ladders
	10,3,4,
	20,3,4,
	3,7,8,
	27,7,8,
	6,15,8,
	24,15,8,
	10,23,3,
	20,23,3,
	255,

	//cabinets
	3,20,0,
	15,20,1,
	27,20,2,
	255,

	//desks
	6,5,
	23,5,
	255,

	//coffee machine
	14,11,
	255,

	//clock
	255,

	//door
	255,

//level bonus
	4,1,(0/5),//tile set, calls, delay between calls

	//player
	15,14,	

	//NPC
	11,10,7,
	255,

	//platforms
	7,7,18,
	3,15,26,
	0,23,32,
	255,

	//ladders
	12,7,8,
	18,7,8,
	4,15,8,
	26,15,8,
	255,

	//cabinets
	255,

	//desks
	8,5,
	21,5,
	8,13,
	21,13,
	8,21,
	21,21,
	255,

	//coffee machine
	255,

	//clock
	255,

	//door
	255,

//level 5
	0,30,(275/5),//tile set, calls, delay between calls

	//player
	2,14,	

	//NPC
	28,12,4,
	255,

	//platforms
	5,8,22,
	0,15,32,
	0,23,32,
	255,

	//ladders
	15,3,5,
	6,15,8,
	24,15,8,
	15,23,3,
	255,

	//cabinets
	6,5,1,
	24,5,3,
	2,20,0,
	28,20,2,
	255,

	//desks
	9,13,
	20,13,
	19,21,
	255,

	//coffee machine
	10,19,
	255,

	//clock
	255,

	//door
	255,

//level 6
	1,35,(400/5),//tile set, calls, delay between calls

	//player
	7,14,	

	//NPC
	22,12,5,//box guy
	255,

	//platforms
	0,7,7,
	10,8,12,
	25,7,7,
	0,15,13,
	19,15,13,
	0,23,32,
	255,

	//ladders
	5,3,4,
	16,3,5,
	25,3,4,
	8,15,8,
	22,15,8,
	5,23,3,
	16,23,3,
	25,23,3,
	255,

	//cabinets
	11,12,0,
	19,12,2,
	2,20,1,
	28,20,3,
	255,

	//desks
	3,13,
	26,13,
	11,21,
	18,21,
	255,

	//coffee machine
	12,4,
	255,

	//clock
	255,

	//door
	255,

//level 7
	2,40,(450/5),//calls, delay between calls

	//player
	14,14,	

	//NPC
	27,20,6,
	255,

	//platforms
	0,7,32,
	0,15,32,
	0,23,32,
	255,

	//ladders
	13,3,4,
	3,7,8,
	17,7,8,
	27,7,8,
	6,15,8,
	24,15,8,
	13,23,3,
	255,

	//cabinets
	9,12,0,
	21,12,2,
	9,20,1,
	21,20,3,
	255,

	//desks
	7,5,
	21,5,
	2,21,
	27,21,
	255,

	//coffee machine
	16,19,
	255,

	//clock
	255,

	//door
	255,
};


#include "metasprite_map.inc"
#include "animations.inc"
