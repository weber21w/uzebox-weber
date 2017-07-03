#define F 0  //FLOOR
#define B 4  //BARRIER(EXPLODABLE)
#define W 28  //WALL
#define P 254//PLAYER START

#define FLOOR F
#define BARRIER B
#define WALL W
#define PLAYERSTART P

//Instances greater than W, indicate chance of appearance(as a block) where W+1 would be 1/8 chance W+8 would be 8/8 chance

const char GameMaps[] PROGMEM = {


P,F,B,B,B,B,B,B,B,B,B,F,P,
F,W,B,W,B,W,B,W,B,W,B,W,F,
B,B,B,B,B,B,B,B,B,B,B,B,B,
B,W,B,W,B,W,B,W,B,W,B,W,B,
B,B,B,B,B,F,F,F,B,B,B,B,B,
B,W,B,W,B,W,F,W,B,W,B,W,B,
B,B,B,B,B,F,F,F,B,B,B,B,B,
B,W,B,W,B,W,B,W,B,W,B,W,B,
B,B,B,B,B,B,B,B,B,B,B,B,B,
F,W,B,W,B,W,B,W,B,W,B,W,F,
P,F,B,B,B,B,B,B,B,B,B,F,P,

};


#define NUMLEVELS 8

const char MapPreviews[] PROGMEM = {

};

const char IcePos[] PROGMEM =
{
6,4,
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

const char IglooMap[] PROGMEM = 
{
1,

};

const char OcclusionLayer[] PROGMEM = {//don't set sprites if contained in this area
1,//NUM OCCLUSION AREAS
6,8,2,3,//x,y,w,h

255//END
};