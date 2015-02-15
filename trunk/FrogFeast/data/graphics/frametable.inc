const uint8_t FrameTable[] PROGMEM = {
0,
0,1,//frog 1 sitting left
6,7,

0,
2,3,//frog 1 jumping left
8,9,

0,
4,5,//frog 1 swimming left
10,11,

0,//frog 1 blink sitting left
43,1,
6,7,

0,//frog 1 blink jumping left
44,3,
8,9,

0,//frog 1 blink swimming left
45,5,
10,11,

0,//frog 1 dizzy frame 1 LEFT
50,51,
56,57,

0,//frog 1 dizzy frame 2 LEFT
52,53,
56,57,

0,//frog 1 dizzy frame 3 LEFT
54,55,
56,57,

0,//frog 1 turn frame 1 LEFT
8,8,//these tiles were eliminated and all graphic offsets fixed when turn animation went to 1 frame(to save space, and looks better)
8,8,

0,//frog 1 turn frame 2 LEFT
62,63,
68,69,

SPRITE_FLIP_X,//frog 1 turn frame 3 LEFT
8,8,//frame removed
8,8,



0,//frog 1 spin frame 1(bottom down nose left) LEFT
58,59,
64,65,

0,//frog 1 spin frame 2(bottom left nose up) LEFT
60,61,
66,67,

SPRITE_FLIP_X|SPRITE_FLIP_Y,//frog 1 spin frame 3(bottom up nose right) LEFT
65,64,
59,58,


SPRITE_FLIP_X|SPRITE_FLIP_Y,//frog 1 spin frame 4(bottom right nose down) LEFT
67,66,
61,60,

//frame 16
SPRITE_FLIP_X,
1,0,//frog 1 sitting right
7,6,

SPRITE_FLIP_X,
3,2,//frog 1 jumping right
9,8,

SPRITE_FLIP_X,
5,4,//frog 1 swimming right
11,10,

SPRITE_FLIP_X,
1,43,//frog 1 blink sitting right
7,6,

SPRITE_FLIP_X,
3,44,//frog 1 blink jumping right
9,8,

SPRITE_FLIP_X,
5,45,//frog 1 blink swimming right
11,10,

SPRITE_FLIP_X,//frog 1 dizzy frame 1 RIGHT
51,50,
57,56,

SPRITE_FLIP_X,//frog 1 dizzy frame 2 RIGHT
53,52,
57,56,

SPRITE_FLIP_X,//frog 1 dizzy frame 3 RIGHT
55,54,
57,56,

SPRITE_FLIP_X,//frog 1 turn frame 1 RIGHT
63,62,
71,70,

SPRITE_FLIP_X,//frog 1 turn frame 2 RIGHT
63,62,
69,68,

0,//frog 1 turn frame 3 RIGHT
62,63,
70,71,



SPRITE_FLIP_X,//frog 1 spin frame 1(bottom down nose left) RIGHT
59,58,
65,64,

SPRITE_FLIP_X,//frog 1 spin frame 2(bottom left nose up) RIGHT
61,60,
67,66,

SPRITE_FLIP_Y,//frog 1 spin frame 3(bottom up nose right) RIGHT
64,65,
58,59,


SPRITE_FLIP_Y,//frog 1 spin frame 4(bottom right nose down) RIGHT
66,67,
60,61,



0,
12,13,//frog 2 sitting left
18,19,

0,
14,15,//frog 2 jumping left
20,21,

0,
16,17,//frog 2 swimming left
22,23,

0,
46,13,//frog 2 blink sitting left
18,19,

0,
47,15,//frog 2 blink jumping left
20,21,

0,
48,17,//frog 2 blink swimming left
22,23,

0,//frog 2 dizzy frame 1 LEFT
70,71,
76,77,

0,//frog 2 dizzy frame 2 LEFT
72,73,
76,77,

0,//frog 2 dizzy frame 3 LEFT
74,75,
76,77,

0,//frog 2 turn frame 1 LEFT
8,8,//frame was removed
8,8,

0,//frog 2 turn frame 2 LEFT
82,83,
88,89,

SPRITE_FLIP_X,//frog 2 turn frame 3 LEFT
8,8,//frame was removed
8,8,


0,//frog 2 spin frame 1(bottom down nose left) LEFT
78,79,
84,85,

0,//frog 2 spin frame 2(bottom left nose up) LEFT
80,81,
86,87,

SPRITE_FLIP_X|SPRITE_FLIP_Y,//frog 2 spin frame 3(bottom up nose right) LEFT
85,84,
79,78,


SPRITE_FLIP_X|SPRITE_FLIP_Y,//frog 2 spin frame 4(bottom right nose down) LEFT
87,86,
81,80,


SPRITE_FLIP_X,
13,12,//frog 2 sitting right
19,18,

SPRITE_FLIP_X,
15,14,//frog 2 jumping right
21,20,

SPRITE_FLIP_X,
17,16,//frog 2 swimming right
23,22,

SPRITE_FLIP_X,
13,46,//frog 2 blink sitting right
19,18,

SPRITE_FLIP_X,
15,47,//frog 2 blink jumping right
21,20,

SPRITE_FLIP_X,
17,48,//frog 2 blink swimming right
23,22,

SPRITE_FLIP_X,//frog 2 dizzy frame 1 RIGHT
71,70,//74,75,
77,76,//80,81,

SPRITE_FLIP_X,//frog 2 dizzy frame 2 RIGHT
73,72,//76,77,
77,76,//80,81,

SPRITE_FLIP_X,//frog 2 dizzy frame 3 RIGHT
75,74,//78,79,
77,76,//80,81,

SPRITE_FLIP_X,//frog 2 turn frame 1 RIGHT
87,86,//frame removed
95,94,

SPRITE_FLIP_X,//frog 2 turn frame 2 RIGHT
83,82,
89,88,

0,//frog 2 turn frame 3 RIGHT
87,86,//frame removed
95,94,


SPRITE_FLIP_X,//frog 2 spin frame 1(bottom down nose left) RIGHT
79,78,
85,84,

SPRITE_FLIP_X,//frog 2 spin frame 2(bottom left nose up) RIGHT
81,80,
87,86,

SPRITE_FLIP_Y,//frog 2 spin frame 3(bottom up nose right) RIGHT
84,85,
78,79,


SPRITE_FLIP_Y,//frog 2 spin frame 4(bottom right nose down) RIGHT
86,87,
80,81,


//frame 64
0,
255,24,//frog tongue frame 1 left
255,255,

0,
255,25,//frog tongue frame 2 left
255,255,

0,
255,26,//frog tongue frame 3 left
255,255,

0,
27,28,//frog tongue frame 4 left
255,255,

SPRITE_FLIP_X,
24,255,//frog tongue frame 1 right
255,255,

SPRITE_FLIP_X,
25,255,//frog tongue frame 2 right
255,255,

SPRITE_FLIP_X,
26,255,//frog tongue frame 3 right
255,255,

SPRITE_FLIP_X,
28,27,//frog tongue frame 4 right
255,255,

//Frame 72
0,//water ripple 1 left half
29,30,
35,36,

0,//water ripple 2 left half
32,33,
38,39,

0,
31,255,
37,255,

0,
34,255,
40,255,

};