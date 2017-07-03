#define RC RAM_TILES_COUNT
const uint8_t TileFrames[] PROGMEM = {
0+RC,0+RC,//0-standard brick
0+RC,0+RC,

1+RC,0+RC,//standard brick shadowed
1+RC,0+RC,

2+RC,3+RC,//standard tree
4+RC,5+RC,

2+RC,3+RC,//standard tree shadowed
6+RC,5+RC,

7+RC,8+RC,//standard rock
9+RC,10+RC,

11+RC,8+RC,//standard rock shadowed
12+RC,10+RC,

33+RC,33+RC,//water frame 0
33+RC,33+RC,

38+RC,33+RC,//water frame 0 shadowed
38+RC,33+RC,

33+RC,33+RC,//water up frame 0
33+RC,33+RC,

38+RC,33+RC,//water up frame 0 shadowed
38+RC,33+RC,

33+RC,33+RC,//water down frame 0
33+RC,33+RC,

38+RC,33+RC,//water down frame 0 shadowed
38+RC,33+RC,

33+RC,33+RC,//water left frame 0
33+RC,33+RC,

38+RC,33+RC,//water left frame 0 shadowed
38+RC,33+RC,

33+RC,33+RC,//water right frame 0
33+RC,33+RC,

38+RC,33+RC,//water right frame 0 shadowed
38+RC,33+RC,

75+RC,75+RC,//sand 0
75+RC,75+RC,

71+RC,75+RC,//sand 0 shadowed
71+RC,75+RC,

64+RC,64+RC,//grass 0
64+RC,64+RC,

70+RC,64+RC,//grass 0 shadowed
70+RC,64+RC,

201+RC,202+RC,//arrow up
207+RC,208+RC,

201+RC,202+RC,//arrow up shadowed(same)
207+RC,208+RC,

205+RC,206+RC,//arrow down
203+RC,204+RC,

205+RC,206+RC,//arrow down shadowed(same)
203+RC,204+RC,

201+RC,206+RC,//arrow left
203+RC,208+RC,

201+RC,206+RC,//arrow left shadowed(same)
203+RC,208+RC,

205+RC,202+RC,//arrow right
207+RC,204+RC,

205+RC,202+RC,//arrow right shadowed(same)
207+RC,204+RC,

55+RC,55+RC,//x bridge
57+RC,57+RC,

55+RC,55+RC,//x bridge shadowed(same)
57+RC,57+RC,

59+RC,60+RC,//y bridge
59+RC,60+RC,

59+RC,60+RC,//y bridge shadowed(same)
59+RC,60+RC,

51+RC,51+RC,//weak bridge
51+RC,51+RC,

51+RC,51+RC,//weak bridge shadowed(same)
51+RC,51+RC,

52+RC,54+RC,//weaker bridge
54+RC,52+RC,

52+RC,54+RC,//weaker bridge shadowed(same)
54+RC,52+RC,

43+RC,44+RC,//chest
45+RC,46+RC,

43+RC,44+RC,//chest shadowed(same)
45+RC,46+RC,

47+RC,48+RC,//chest open
49+RC,50+RC,

47+RC,48+RC,//chest open shadowed(same)
49+RC,50+RC,

169+RC,170+RC,//heart framer
171+RC,172+RC,

169+RC,170+RC,//heart framer shadowed(same)
171+RC,172+RC,

173+RC,174+RC,//shot framer
171+RC,172+RC,

173+RC,174+RC,//shot framer shadowed(same)
171+RC,172+RC,

//doesn't stick to 11x11 grid, handled separately
179+RC,180+RC,//emerald framer
181+RC,182+RC,

179+RC,180+RC,//emerald framer shadowed(same)
181+RC,182+RC,

55+RC,55+RC,//x player bridge
57+RC,57+RC,

55+RC,55+RC,//x player bridge shadowed(same)
57+RC,57+RC,

59+RC,60+RC,//y player bridge
59+RC,60+RC,

59+RC,60+RC,//y player bridge shadowed(same)
59+RC,60+RC,

197+RC,197+RC,//x player bridge burned
199+RC,199+RC,

197+RC,197+RC,//x player bridge burned shadowed(same)
199+RC,199+RC,

198+RC,200+RC,//y player bridge burned
198+RC,200+RC,

198+RC,200+RC,//y player bridge burned shadowed(same)
198+RC,200+RC,

17+RC,18+RC,//standard door closed
19+RC,20+RC,

13+RC,14+RC,//standard door open
15+RC,16+RC,






161+RC,162+RC,//power panel blank
163+RC,164+RC,

201+RC,202+RC,//power panel arrow
207+RC,208+RC,

59+RC,60+RC,//power panel bridge
59+RC,60+RC,

165+RC,166+RC,//power panel hammer
167+RC,168+RC,




24+RC,21+RC,//60-border standard top left corner
21+RC,25+RC,

24+RC,21+RC,//border standard top right corner
21+RC,25+RC,

21+RC,25+RC,//border standard bottom left corner
25+RC,24+RC,

21+RC,25+RC,//border standard bottom right corner
25+RC,24+RC,

26+RC,28+RC,//border standard shadow top field
29+RC,31+RC,

27+RC,28+RC,//border standard top field
30+RC,31+RC,

22+RC,23+RC,//border bottom tiles(must be 4, not handled with DrawMap2
22+RC,23+RC,

25+RC,21+RC,//border standard sides
21+RC,25+RC,
};

const uint8_t SandGrassFrameKeys[] PROGMEM = {
78+15,78+11,78+13,78+9,78+14,78+8,78+5,78+12,78+12,78+10,78+7,78+14, 78+6,78+13,78+1,78+0,
//  0,    1,    2,   3,    4,   5,   6,    7,    8,    9,  10,   11,   12,   13,  14,  15,
};



#define NUMWATERFRAMES 6

const uint8_t WaterAnimTable[] PROGMEM = {
0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47
};

const uint8_t WaterFrameTimes[] PROGMEM = {
56,16,16,16,8,16,
};
/*
#define BRICK			0
#define TREE			1
#define ROCK			2
#define WATER			3
#define WATERUP			4
#define WATERDOWN		5
#define WATERLEFT		6
#define WATERRIGHT		7
#define SAND			8
#define GRASS			9
#define UPARROW			10
#define DOWNARROW		11
#define LEFTARROW		12
#define RIGHTARROW		13
#define XBRIDGE			14
#define YBRIDGE			15
#define WEAKBRIDGE		16
#define WEAKERBRIDGE	17
#define CHEST			18
#define CHESTOPEN		19
#define HEARTFRAMER		20
#define SHOTFRAMER		21
#define EMERALDFRAMER	22
*/
const uint8_t MapFrameByType[] PROGMEM = {
30,//BRICK
32,//TREE
34,//ROCK
0,//WATER
6,//WATER UP
12,//WATER DOWN
18,//WATER LEFT
24,//WATER RIGHT
45,//SAND
46,//GRASS
255,//UP ARROW
42,//DOWN ARROW
43,//LEFT ARROW
48,//RIGHT ARROW
47,//X BRIDGE
46,//Y BRIDGE
47,//WEAKBRIDGE
48,//WEAKER BRIDGE
49,//CHEST
50,//CHEST OPEN
50,//HEART FRAMER
50,//SHOT FRAMER
50,//EMERALD FRAMER
};