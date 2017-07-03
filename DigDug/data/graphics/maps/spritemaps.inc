const char sprite_frames[] PROGMEM = {

////////////////////Player//////////////////
////////////////////////////////////////////

#define NUM_FRAMES_PER_PLAYER_DIRECTION_STATE 13
#define FIRST_PLAYER_UP_FRAME 0
#define NUM_PLAYER_DEATH_FRAMES 5
#define FIRST_PLAYER_DEATH_FRAME 8
#define FIRST_ATTACK_FRAME 6
#define FIRST_HARPOON_SPRITE 166

///////////////////facing up, feet on right wall///////////////////

10,11,//0:walk up frame 0 feet on right wall
12,13,

10,14,//1:walk up frame 1 feet on right wall
12,15,

10,16,//2:dig up frame 0 feet on right wall
12,17,

10,18,//3:dig up frame 1 feet on right wall
12,19,

20,21,//4:pump up frame 0 feet on right wall
22,23,

24,25,//5:pump up frame 1 feet on right wall
26,27,

28,29,//6:shoot up feet on right wall
30,31,

54,55,//7:squished up frame 0 feet on right wall(skipped for normal death)
255,255,

56,57,//8:normal dying up frame 0 feet on right wall
58,59,

60,61,//9:normal dying up frame 1 feet on right wall
62,63,

64,65,//10:normal dying up frame 2 feet on right wall
66,67,

68,69,//11:normal dying up frame 3 feet on right wall
70,71,

72,73,//12:normal dying up frame 4 feet on right wall
74,75,

//////////////////facing up feet on left wall//////////////////////

11,10,//13:walk up frame 0 feet on left wall
13,12,

14,10,//14:walk up frame 1 feet on left wall
15,12,

16,10,//15:dig up frame 0 feet on left wall
17,12,

18,10,//16:dig up frame 1 feet on left wall
19,12,

21,20,//17:pump up frame 0 feet on left wall
23,22,

25,24,//18:pump up frame 1 feet on left wall
27,26,

29,28,//19:shoot up feet on left wall
31,30,

55,54,//20:squished up frame 0 feet on left wall(skipped for normal death)
255,255,

57,56,//21:normal dying up frame 0 feet on left wall
59,58,

61,60,//22:normal dying up frame 1 feet on left wall
63,62,

65,64,//23:normal dying up frame 2 feet on left wall
67,66,

69,68,//24:normal dying up frame 3 feet on left wall
71,70,

73,72,//25:normal dying up frame 4 feet on left wall
75,74,

#define FIRST_PLAYER_DOWN_FRAME 26

///////////////////facing down feet on right wall/////////////////////

12,13,//26:walk down frame 0 feet on right wall
10,11,

12,15,//27:walk down frame 1 feet on right wall
10,14,

12,17,//28:dig down frame 0 feet on right wall
10,16,

12,19,//29:dig down frame 1 feet on right wall
10,18,

22,23,//30:pump down frame 0 feet on right wall
20,21,

26,27,//31:pump down frame 1 feet on right wall
24,25,

30,31,//32:shoot down feet on right wall
28,29,

255,255,//33:squished down frame 0 feet on right wall(skipped for normal death)
54,55,

58,59,//34:normal dying down frame 0 feet on right wall
56,57,

62,63,//35:normal dying down frame 1 feet on right wall
60,61,

66,67,//36:normal dying down frame 2 feet on right wall
64,65,

70,71,//37:normal dying down frame 3 feet on right wall
68,69,

74,75,//38:normal dying down frame 4 feet on right wall
72,73,

//////////////////facing down feet on left wall////////////////////////

13,12,//39:walk down frame 0 feet on left wall
11,10,

15,12,//40:walk down frame 1 feet on left wall
14,10,

17,12,//41:dig down frame 0 feet on left wall
16,10,

19,12,//42:dig down frame 1 feet on left wall
18,10,

23,22,//43:pump down frame 0 feet on left wall
21,20,

27,26,//44:pump down frame 1 feet on left wall
25,24,

31,30,//45:shoot down feet on left wall
29,28,

255,255,//46:squished down frame 0 feet on left wall(skipped for normal death)
55,54,

59,58,//47:normal dying down frame 0 feet on left wall
57,56,

63,62,//48:normal dying down frame 1 feet on left wall
61,60,

67,66,//49:normal dying down frame 2 feet on left wall
65,64,

71,70,//50:normal dying down frame 3 feet on left wall
69,68,

75,74,//51:normal dying down frame 4 feet on left wall
73,72,

#define FIRST_PLAYER_LEFT_FRAME 52

//////////////////facing left/////////////////////////

32,33,//52:walk left frame 0
34,35,

32,33,//53:walk left frame 1
36,37,

32,33,//54:dig left frame 0
38,39,

32,33,//55:dig left frame 1
40,41,

42,43,//56:pump left frame 0
44,45,

46,47,//57:pump left frame 1
48,49,

50,51,//58:shoot left
52,53,

255,255,//59:squished left frame 0(skipped for normal death)
76,77,

78,79,//60:normal dying left frame 0
80,81,

82,83,//61:normal dying left frame 1
84,85,

86,87,//62:normal dying left frame 2
88,89,

90,91,//63:normal dying left frame 3
92,93,

94,95,//64:normal dying left frame 4
96,97,

#define FIRST_PLAYER_RIGHT_FRAME 65

//////////////////facing right////////////////////////

33,32,//65:walk right frame 0
35,34,

33,32,//66:walk right frame 1
37,36,

33,32,//67:dig right frame 0
39,38,

33,32,//68:dig right frame 1
41,40,

43,42,//69:pump right frame 0
45,44,

47,46,//70:pump right frame 1
49,48,

51,50,//71:shoot right
53,52,

255,255,//72:squished right frame 0(skipped for normal death)
77,76,

79,78,//73:normal dying right frame 0
81,80,

83,82,//74:normal dying right frame 1
85,84,

87,86,//75:normal dying right frame 2
89,88,

91,90,//76:normal dying right frame 3
93,92,

95,94,//77:normal dying right frame 4
97,96,


#define FIRST_ENEMY_FRAME	78
#define FIRST_POOKA_FRAME FIRST_ENEMY_FRAME

////////////////////////////////////////
/////////////Pooka//////////////////////

255,255,//78:squished left
98,99,

100,101,//79:walk left frame 0
102,103,

104,105,//80:walk left frame 1
106,107,

172,173,//81:inflated frame 0 left
174,175,

255,255,//82:squished right
99,98,

101,100,//83:walk right frame 0
103,102,

105,104,//84:walk right frame 1
107,106,

173,172,//85:inflated frame 0 right
175,174,

255,255,//86:ghost frame 0, needs vertical adjustment(TODO JUST USE 2 EXTRA SPRITES...)
128,129,

255,255,//87:ghost frame 1, needs vertical adjustment
130,131,

#define FIRST_FYGAR_FRAME	88

////////////////////////////////////////
////////////Fygar///////////////////////

255,255,//88:squished left
108,109,

110,111,//89:walk left frame 0
112,113,

114,115,//90:walk left frame 1
116,117,

114,115,//91:fire flash left
118,119,

203,204,//92:stunned frame 0 left 
205,206,

255,255,//93:squished right
109,108,

111,110,//94:walk right frame 0
113,112,

115,114,//95:walk right frame 1
117,116,

115,114,//96:fire flash right
117,116,

204,203,//97:stunned frame 0 right 
206,205,

120,121,//98:ghost frame 0
122,123,

124,125,//99:ghost frame 1
126,127,


#define FIRST_ROCK_FRAME	100

132,133,//100:rock normal
134,135,

136,137,//101:rock tilted
138,139,

140,141,//102:rock crumble frame 0
142,143,

255,255,//103:rock crumble frame 1
144,145,

#define FIRST_FYGAR_FLAME_FRAME		104

0,0,//104:flame frame 0 left, 1 of 1
0,0,

0,0,//105:flame frame 1 left, 1 of 2
0,0,

0,0,//106:flame frame 1 left, 2 of 2
0,0,

0,0,//107:flame frame 2 left, 1 of 3
0,0,

0,0,//108:flame frame 2 left, 2 of 3
0,0,

0,0,//109:flame frame 2 left, 3 of 3
0,0,

/////

0,0,//110:flame frame 0 right, 1 of 1
0,0,

0,0,//111:flame frame 1 right, 1 of 2
0,0,

0,0,//112:flame frame 1 right, 2 of 2
0,0,

0,0,//113:flame frame 2 right, 1 of 3
0,0,

0,0,//114:flame frame 2 right, 2 of 3
0,0,

0,0,//115:flame frame 2 right, 3 of 3
0,0,


#define FIRST_LARGE_SPRITE_FRAME	116

//////////////////////////////////////////
///////////////Large 3x3 frames///////////

///////////////Pooka//////////////////////

176,177,178,//116:inflated frame 0 left
179,180,181,
182,183,184,

185,186,187,//117:inflated frame 1 left
188,189,190,
191,192,193,

194,195,196,//118:inflated frame 2(popped) left
197,198,199,
200,201,202,

178,177,176,//119:inflated frame 0 right
181,180,179,
184,183,182,

187,186,185,//120:inflated frame 1 right
190,189,188,
193,192,191,

196,195,194,//121:inflated frame 2(popped) right
199,198,197,
202,201,200,

///////////////Fygar//////////////////////

207,208,209,//122:inflated frame 0 left
210,211,212,
213,214,215,

216,217,218,//123:inflated frame 1 left
219,220,221,
222,223,224,

225,226,227,//124:inflated frame 2(popped) left
228,229,230,
231,232,233,

209,208,207,//125:inflated frame 0 right
212,211,210,
215,214,213,

218,217,216,//126:infalted frame 1 right
221,220,219,
224,223,222,

227,226,225,//127:inflated frame 2(popped) right
230,229,228,
233,232,231,


};

const char sprite_flags[] PROGMEM = {

////////////////////up feet on right////////////////////

0,0,//frame 0
0,0,

0,0,//frame 1
0,0,

0,0,//frame 2
0,0,

0,0,//frame 3
0,0,

0,0,//frame 4
0,0,

0,0,//frame 5
0,0,

0,0,//frame 6
0,0,

0,0,//frame 7
0,0,

0,0,//frame 8
0,0,

0,0,//frame 9
0,0,

0,0,//frame 10
0,0,

0,0,//frame 11
0,0,

0,0,//frame 12
0,0,

////////////////////up feet on left/////////////////////

1,1,//frame 13
1,1,

1,1,//frame 14
1,1,

1,1,//frame 15
1,1,

1,1,//frame 16
1,1,

1,1,//frame 17
1,1,

1,1,//frame 18
1,1,

1,1,//frame 19
1,1,

1,1,//frame 20
1,1,

1,1,//frame 21
1,1,

1,1,//frame 22
1,1,

1,1,//frame 23
1,1,

1,1,//frame 24
1,1,

1,1,//frame 25
1,1,

////////////////////down///////////////////

2,2,//frame 26
2,2,

2,2,//frame 27
2,2,

2,2,//frame 28
2,2,

2,2,//frame 29
2,2,

2,2,//frame 30
2,2,

2,2,//frame 31
2,2,

2,2,//frame 32
2,2,

2,2,//frame 33
2,2,

2,2,//frame 34
2,2,

2,2,//frame 35
2,2,

2,2,//frame 36
2,2,

2,2,//frame 37
2,2,

2,2,//frame 38
2,2,

////////////////////down///////////////////

3,3,//frame 39
3,3,

3,3,//frame 40
3,3,

3,3,//frame 41
3,3,

3,3,//frame 42
3,3,

3,3,//frame 43
3,3,

3,3,//frame 44
3,3,

3,3,//frame 45
3,3,

3,3,//frame 46
3,3,

3,3,//frame 47
3,3,

3,3,//frame 48
3,3,

3,3,//frame 49
3,3,

3,3,//frame 50
3,3,

3,3,//frame 51
3,3,


///////////////////left////////////////////

0,0,//frame 52
0,0,

0,0,//frame 53
0,0,

0,0,//frame 54
0,0,

0,0,//frame 55
0,0,

0,0,//frame 56
0,0,

0,0,//frame 57
0,0,

0,0,//frame 58
0,0,

0,0,//frame 59
0,0,

0,0,//frame 60
0,0,

0,0,//frame 61
0,0,

0,0,//frame 62
0,0,

0,0,//frame 63
0,0,

0,0,//frame 64
0,0,

/////////////////right//////////////////////

1,1,//frame 65
1,1,

1,1,//frame 66
1,1,

1,1,//frame 67
1,1,

1,1,//frame 68
1,1,

1,1,//frame 69
1,1,

1,1,//frame 70
1,1,

1,1,//frame 71
1,1,

1,1,//frame 72
1,1,

1,1,//frame 73
1,1,

1,1,//frame 74
1,1,

1,1,//frame 75
1,1,

1,1,//frame 76
1,1,

1,1,//frame 77
1,1,

////////////////Pooka//////////////////////

0,0,//frame 78
0,0,

0,0,//frame 79
0,0,

0,0,//frame 80
0,0,

1,1,//frame 81
1,1,

1,1,//frame 82
1,1,

1,1,//frame 83
1,1,

0,0,//frame 84
0,0,

0,0,//frame 85
0,0,

0,0,//frame 86 ghost
0,0,

0,0,//frame 87 ghost
0,0,

////////////////Fygar//////////////////////////
0,0,//frame 88
0,0,

0,0,//frame 89
0,0,

0,0,//frame 90
0,0,

0,0,//frame 91
0,0,

1,1,//frame 92
1,1,

1,1,//frame 93
1,1,

1,1,//frame 94
1,1,

1,1,//frame 95
1,1,

0,0,//frame 96
0,0,

0,0,//frame 97 ghost
0,0,

0,0,//frame 98 ghost
0,0,

///////////////Rock///////////////////////////////

0,0,//frame 99
0,0,

0,0,//frame 100
0,0,

0,0,//frame 101
0,0,

0,0,//frame 102
0,0,

//////////////Fygar Flame//////////////////////////


/////////////Inflated frames 3x3///////////////////////


};
