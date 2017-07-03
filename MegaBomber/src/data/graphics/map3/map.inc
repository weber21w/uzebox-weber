#define F 0  //FLOOR
#define B BLOCK_START  //BARRIER(EXPLODABLE)
#define W WALL_START  //WALL
#define P 254//PLAYER START

#define FLOOR F
#define BARRIER B
#define WALL W
#define PLAYERSTART P
//Mine cart defines
#define V SPECIAL_START
#define H SPECIAL_START+4
#define C1 SPECIAL_START+8
#define C2 SPECIAL_START+12
#define C3 SPECIAL_START+16
#define CT SPECIAL_START+20


//Instances greater than W, indicate chance of appearance(as a block) where W+1 would be 1/8 chance W+8 would be 8/8 chance

const char GameMaps[] PROGMEM = {
P,F,F,F,F,B,B,B,B,B,B,F,P,
F,W,B,W,F,W,B,W,B,W,B,W,F,
B,B,B,B,CT,H,H,H,C1,B,B,B,B,
B,W,B,W,B,W,B,W,V,W,B,W,B,
B,B,B,B,B,F,B,F,V,B,B,B,B,
B,W,B,W,B,W,B,W,V,W,B,W,B,
B,B,B,B,F,F,B,F,V,B,B,B,B,
B,W,B,W,V,W,B,W,V,W,B,W,B,
B,B,B,B,C3,H,H,H,C2,B,B,B,B,
F,W,B,W,B,W,B,W,B,W,B,W,F,
P,F,B,B,B,B,B,B,B,B,B,F,P,


/*
P,F,B,B,B,B,B,B,B,B,B,F,P,
F,W,B,W,B,W,B,W,B,W,B,W,F,
B,B,B,B,CT,H,H,H,C1,B,B,B,B,
B,W,B,W,B,W,B,W,V,W,B,W,B,
B,B,B,B,B,F,B,F,V,B,B,B,B,
B,W,B,W,B,W,B,W,V,W,B,W,B,
B,B,B,B,F,F,B,F,V,B,B,B,B,
B,W,B,W,V,W,B,W,V,W,B,W,B,
B,B,B,B,C3,H,H,H,C2,B,B,B,B,
F,W,B,W,B,W,B,W,B,W,B,W,F,
P,F,B,B,B,B,B,B,B,B,B,F,P,
*/
};


#define NUMLEVELS 8

const char MapPreviews[] PROGMEM = {

};

const char IcePos[] PROGMEM =
{
22,6,//6,4,
22,6,
14,12,
6,18,
22,20,
};

const char IglooPos[] PROGMEM = 
{
4,8,
20,12,
};

#define IG SPECIAL_START+8
const char IglooMap[] PROGMEM = 
{
IG+0 ,IG+1 ,IG+16,IG+17,IG+3 ,IG+4 ,
IG+2 ,IG+14,IG+24,IG+25,IG+15,IG+5 ,
IG+34,IG+18,IG+26,IG+27,IG+20,IG+35,
IG+19,IG+30,IG+28,IG+29,IG+31,IG+21,
IG+6 ,IG+7 ,IG+22,IG+23,IG+10,IG+11,
IG+8 ,IG+9 ,IG+32,IG+33,IG+12,IG+13,

};