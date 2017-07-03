const char FrameTable[] PROGMEM = {
0,0,
1,1,
2,2,
1,

3,4,
5,6,
7,8,
0,

9,10,
11,12,
13,14,
0,

3,4,
5,6,
7,8,
0,

0,0,
1,1,
2,2,
1,

4,3,
6,5,
8,7,
2,

10,9,
12,11,
14,13,
2,

4,3,
6,5,
8,7,
2,


////DOWN

15,15,
16,16,
17,17,
1,

18,19,
20,21,
22,23,
0,

24,25,
26,27,
28,29,
0,

18,19,
20,21,
22,23,
0,

15,15,
16,16,
17,17,
1,

19,18,
21,20,
23,22,
2,

25,24,
27,26,
29,28,
2,

19,18,
21,20,
23,22,
2,


//////LEFT

31,30,
33,32,
35,34,
2,

37,36,
39,38,
41,40,
2,

43,42,
45,44,
47,46,
2,

37,36,
39,38,
41,40,
2,

31,30,
33,32,
35,34,
2,

49,48,
51,50,
53,52,
2,

55,54,
57,56,
59,58,
2,

49,48,
51,50,
53,52,
2,

///////RIGHT

30,31,
32,33,
34,35,
0,

36,37,
38,39,
40,41,
0,

42,43,
44,45,
46,47,
0,

36,37,
38,39,
40,41,
0,

30,31,
32,33,
34,35,
0,

48,49,
50,51,
52,53,
0,

54,55,
56,57,
58,59,
0,

48,49,
50,51,
52,53,
0,

/////////////////////STUNNED
#define STUNNEDSTARTFRAME 32
#define SS FIRSTSTUNNEDSPRITE//119

SS+0,SS+0,
SS+1,SS+1,
SS+2,SS+2,
1,

SS+3,SS+4,
SS+5,SS+6,
SS+7,SS+8,
0,

SS+9,SS+10,
SS+11,SS+12,
SS+13,SS+14,
0,

SS+3,SS+4,
SS+5,SS+6,
SS+7,SS+8,
0,

SS+0,SS+0,
SS+1,SS+1,
SS+2,SS+2,
1,

SS+4,SS+3,
SS+6,SS+5,
SS+8,SS+7,
2,

SS+10,SS+9,
SS+12,SS+11,
SS+14,SS+13,
2,

SS+4,SS+3,
SS+6,SS+5,
SS+8,SS+7,
2,


#define VICTORYSTARTFRAME STUNNEDSTARTFRAME+8
#define VS FIRSTVICTORYSPRITE

VS+0,VS+0,
VS+1,VS+1,
VS+2,VS+2,
1,

VS+3,VS+3,
VS+4,VS+4,
VS+5,VS+5,
1,

#define IDLESTARTFRAME VICTORYSTARTFRAME+2
#define IS FIRSTIDLESPRITE//134

IS+0,IS+0,//0 look down
IS+1,IS+1,
IS+2,IS+2,
1,

IS+0,IS+0,//1 foot back
IS+1,IS+1,
IS+3,IS+2,
1,

IS+0,IS+0,//2 foot forward
IS+1,IS+1,
IS+4,IS+2,
1,


IS+0,IS+0,//3 blink
IS+5,IS+5,
IS+2,IS+2,
1,

IS+6,IS+6,//4 look up
IS+7,IS+7,
IS+2,IS+2,
1,

IS+6,IS+6,//5 arms out
IS+8,IS+8,
IS+2,IS+2,
1,

IS+10,IS+10,//6 arms up
IS+11,IS+11,
IS+2,IS+2,
1,

#define DEATHSTARTFRAME IDLESTARTFRAME+7
#define DT FIRSTDEATHSPRITE

DT+0,DT+0,
DT+1,DT+1,
DT+2,DT+2,
1,

DT+3,DT+3,
DT+4,DT+4,
DT+5,DT+5,
1,

DT+6,DT+6,
DT+7,DT+7,
DT+8,DT+8,
1,

DT+9,DT+9,
DT+10,DT+10,
DT+11,DT+11,
1,



#define BOMBSTARTFRAME DEATHSTARTFRAME+4
#define BM FIRSTBOMBSPRITE

255,255,
BM+0,BM+1,
BM+2,BM+3,
0,

255,255,
BM+4,BM+5,
BM+6,BM+7,
0,

255,255,
BM+8,BM+9,
BM+10,BM+11,
0,

#define SHADOWSTARTFRAME BOMBSTARTFRAME+3
255,255,
255,255,
SHADOW_SPRITE,SHADOW_SPRITE,
1,

#define FIRSTSPECIALFRAME SHADOWSTARTFRAME+1

#if MAP == 0
#include "map0/specialframes.inc"
#elif MAP == 1
#include "map1/specialframes.inc"
#elif MAP == 2
#include "map2/specialframes.inc"
#elif MAP == 3
#include "map3/specialframes.inc"
#elif MAP == 4
#include "map4/specialframes.inc"
#endif

};

const char MirrorTable[] PROGMEM = {
0,0,
0,0,  //0
0,0,

0,1,
0,1,  //1
0,1,

1,1,
1,1,  //2
1,1,

0,0,
1,0,  //3
1,0,

0,0,
1,1,  //4
0,0,

0,0,
0,1,  //5
1,1,

0,0,
0,1,  //6
0,0,

0,0,
0,0,
0,1,  //7

1,1,
1,1,
0,1,  //8
}; 

const char FireSequence[] PROGMEM = {0,1,2,1,0,0,};

const char StaticFrameTable[] PROGMEM = {
1,
};

#define NUMIDLEFRAMES 52

const char IdleFrames[NUMIDLEFRAMES] PROGMEM = {
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+1,
IDLESTARTFRAME+1,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+2,
IDLESTARTFRAME+2,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+1,
IDLESTARTFRAME+1,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+2,
IDLESTARTFRAME+2,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+1,
IDLESTARTFRAME+1,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+2,
IDLESTARTFRAME+2,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
//END FOOT SWING
IDLESTARTFRAME+3,
IDLESTARTFRAME+3,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+3,
IDLESTARTFRAME+3,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
IDLESTARTFRAME+3,
IDLESTARTFRAME+0,
IDLESTARTFRAME+3,
IDLESTARTFRAME+0,
IDLESTARTFRAME+0,
//END BLINK
IDLESTARTFRAME+4,
IDLESTARTFRAME+4,
IDLESTARTFRAME+5,
IDLESTARTFRAME+6,
IDLESTARTFRAME+5,
IDLESTARTFRAME+6,
IDLESTARTFRAME+5,
IDLESTARTFRAME+6,
IDLESTARTFRAME+5,
IDLESTARTFRAME+6,
IDLESTARTFRAME+5,
IDLESTARTFRAME+6,
IDLESTARTFRAME+5,





};
