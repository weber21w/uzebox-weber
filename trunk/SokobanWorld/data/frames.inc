const char FrameTable[] PROGMEM = {
//BLOCK
0,1,
2,3,
0,

//UP
4,5,
6,7,
0,

5,4,
7,6,
1,

//UP PUSHING
8,9,
10,11,
0,

9,8,
11,10,
1,

//DOWN
12,13,
14,15,
0,

13,12,
15,14,
1,

//DOWN PUSHING
16,17,
18,19,
0,

17,16,
19,18,
1,

//LEFT
20,21,
22,23,
0,

24,25,
26,27,
0,

//LEFT PUSHING
28,29,
30,31,
0,

32,33,
34,35,
0,

//RIGHT
21,20,
23,22,
1,

25,24,
27,26,
1,

//RIGHT PUSHING
29,28,
31,30,
1,

33,32,
35,34,
1,

#define SPRITESPERTILESET 36
};

const char MirrorTable[] PROGMEM = {
0,0,
0,0,

1,1,
1,1,
}; 
