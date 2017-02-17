const char sprite_frame_table[] PROGMEM = {
#define FRAME_LEFT_WALK 0
0,1,//walk 1
2,3,

#define FRAME_LEFT_WALK2 1
0,4,//walk 2
5,6,

#define FRAME_LEFT_CARRY 2
7,8,//walk 1 while carry
9,10,

#define FRAME_LEFT_CARRY2 3
11,12,//walk 2 while carry
13,14,

#define FRAME_LEFT_JUMP_FALL 4
15,16,//jump/fall
17,18,

#define FRAME_LEFT_VICTORY 5
19,20,//victory
21,22,

#define FRAME_LEFT_LIFT_LOW 6
27,28,
29,30,

#define FRAME_LEFT_LIFT_HIGH 7
31,32,
33,34,

//////facing right
#define FIRST_RIGHT_FRAME 8
#define FRAME_RIGHT_WALK FIRST_RIGHT_FRAME
1,0,//walk 1
3,2,

#define FRAME_RIGHT_WALK2 9
4,0,//walk 2
6,5,

#define FRAME_RIGHT_CARRY 10
8,7,//walk 1 while carry
10,9,

#define FRAME_RIGHT_CARRY2 11
12,11,//walk 2 while carry
14,13,

#define FRAME_RIGHT_JUMP_FALL 12
16,15,//jump/fall
18,17,

#define FRAME_RIGHT_VICTORY 13
20,19,//victory
22,21,

#define FRAME_RIGHT_LIFT_LOW 14
28,27,
30,29,

#define FRAME_RIGHT_LIFT_HIGH 15
32,31,
34,33,

//////
#define FRAME_ROCK 16
23,24,//rock
25,26,

/////
#define FIRST_ALPHA_SPRITE_FRAME 17
35,36,//'L'
37,38,

35,39,//'E'
37,40,

41,42,//'V'
43,44,

#define FIRST_NUMERAL_SPRITE_FRAME 20
45,46,//'0'
47,48,

49,50,//'1'
51,52,

53,54,//'2'
55,56,

57,58,//'3'
59,60,

61,62,//'4'
63,64,

65,66,//'5'
67,68,

69,70,//'6'
71,72,

73,74,//'7'
75,76,

77,78,//'8'
79,80,

81,82,//'9'
83,84,

#define FRAME_EDITOR_CURSOR 30
85,85,
85,85,

};


const uint8_t sprite_mirror_table[] PROGMEM = {
0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

//////RIGHT

1,1,
1,1,

1,1,
1,1,

1,1,
1,1,

1,1,
1,1,

1,1,
1,1,

1,1,
1,1,

1,1,
1,1,

1,1,
1,1,

/////ROCK

0,0,
0,0,

//alpha
0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

//numerals
0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

0,0,
0,0,

//editor cursor
0,1,
2,3,

};



const uint8_t tile_frame_table[] PROGMEM = {
0,0,//black
0,0,

#define TILE_FRAME_SKY 1
1,1,//sky
1,1,

#define TILE_FRAME_ROCK 2
2,3,//rock
4,5,

#define TILE_FRAME_DOOR_CLOSED 3
6,7,//door closed
8,9,

#define TILE_FRAME_DOOR_OPEN 4
10,11,//door open
12,13,

#define FIRST_DIRT_TILE_FRAME 5

14,15,//5-dirt full
16,17,

16,27,//6-dirt on left, sky on right, dirt above and below(left level border edge)
17,29,

15,29,//7-variation of above
15,27,

16,27,//8-variation of above
14,27,

26,17,//9-dirt on right, sky on left, dirt above and below(right level border edge)
28,15,

28,14,//10-variation of above
28,15,

28,16,//11-variation of above
26,17,

19,22,//12-dirt on left and right, dirt below, sky on top, normal flat grass stretch
14,16,

22,19,//13-variation of above
15,16,

22,22,//14-variation of above
16,14,


26,27,//15-vertical tower, sky pixes on each side, dirt above and below
28,29,

28,27,//16-variation of above
26,29,

26,29,//17-variation of above
26,27,

18,23,//18-top of vertical tower(as above), grass on top, dirt below, nothing on sides(sky pixels)
26,29,

18,23,//19-variation of above
28,29,

18,23,//20-variation of above
26,27,

15,30,//21-dirt meets left level border edge, flat grass run on right side
14,15,

16,30,//22-variation of above
15,17,

15,30,//23-variation of above
15,16,

31,16,//24-dirt meets right level border edge, flat grass run on left side
16,15,

31,15,//25-variation of above
14,16,

31,14,//26-variation of above
15,16,

19,23,//27-grass horizontal strip, ends on right, dirt to the left and below, right edge of a plateau
16,27,

22,23,//28-variation of above
16,29,

19,23,//29-variation of above
14,27,

18,19,//30-grass horizontal strip, ends on left, dirt to the right and below, left edge of a plateau
26,15,

18,22,//31-variation of above
28,14,

18,19,//32-variation of above
26,16,

16,30,//33-grass cliff on right, blending in the cliff with the left side wall
15,14,

15,30,//34-variation of above
14,16,

14,30,//35-variation of above
16,14,

16,27,//36-dirt above and to the left, jagged edge on bottom, sky pixels on right
36,41,

15,29,//37-variation of above
37,41,

14,27,//38-variation of above
40,41,

14,15,//39-dirt to the left, right, and above, jagged edge on bottom
34,35,

16,17,//40-variation of above
36,37,

15,17,//41-variation of above
37,34,

26,14,//42-dirt above and to the right, jagged edge on bottom, sky pixels on left
38,36,

28,15,//43-variation of above
38,34,

26,16,//44-variation of above
38,36,

26,27,//45-"icicle" dirt above, jagged bottom, sky edge on both sides
38,41,

28,29,//46-variation of above
38,41,

31,27,//47-right edge is sky pixels, left edge is blending in grass horizontal plane
16,29,

31,29,//48-variation of above
17,27,

18,19,//49-dirt on right, no where else, left side of mid-air island
42,36,

18,22,//50-variation of above
42,37,

22,23,//51-dirt on left, no where else, right side of mid-air island
39,43,

19,23,//52-variation of above
39,43,

22,19,//53-mid-air island center
35,36,

19,22,//54-variation of above
39,35,

18,23,//55-single piece mid-air island
42,43,

31,18,//56-dirt on left only, jagged bottom island meeting with wall
36,39,

31,16,//57-variation of above
39,35,

28,30,//58-grass on right, dirt below, sky on left, grass meeting up to side border edge
26,15,

31,16,//59-grass on left, dirt below, sky on right, grass meeting up to side border edge
16,15,

31,30,//60-base for vertical tower on top of grass
17,16,


15,30,//61-dirt meets left level border edge, flat grass run on right side, jagged on bottom
35,36,

31,16,//62-dirt meets right level border edge, flat grass run on left side, jagged on bottom
36,35,

31,27,//63-dirt above and to the left, jagged edge on bottom, sky pixels on right, blends grass from right
39,41,

31,27,//64-variation of above
36,41,

26,30,//65-dirt above and to the right, jagged edge on bottom, sky pixels on left
38,36,

28,30,//66-variation of above
38,34,

26,30,//67-sky on left, grass run from right blending, dirt above and below
28,16,

31,27,//68-sky on right, grass run from left blending, dirt above and below
16,29,

31,30,//69-dirt to the left, right, and above, jagged edge on bottom, blending grass from both sides
34,35,


};
