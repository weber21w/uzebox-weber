const char BorderFrames[] PROGMEM ={
//////TOP
    0,  1,  2,  2,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  5,  2,  2,  6,  7,
   8, 9, 10, 10, 11, 12, 13, 14, 14, 14, 14, 15, 13, 14, 14, 14, 14, 15, 13, 14, 14, 14, 14, 15, 16, 17, 10, 10, 18, 19,

//////BOTTOM - NO LONGER USED!!!!!!!!!!!
 /*  50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,50, 51,*/
//////LEFT

    0, 20,  
    8, 22,  
   24, 25,
   28, 29,  
    0, 20,  
    8, 22,  
   24, 25,
   28, 29,  
    0, 20,  
    8, 22,  
    0, 25,
    8, 29,  
    0, 20,  
    8, 22,  
   24, 25,
   28, 29,  
    0, 20,  
    8, 22,  
   24, 25,
   28, 29,  
    0, 20,  
    8, 22,  
  




//////RIGHT
 21,  7,
 23, 19,
 26, 27,
 30, 31,
 
 21,  7,
 23, 19,
 21,  7,
 23, 19,

 21,  7,
 23, 19,
 26, 27,
 30, 31,

 21,  7,
 23, 19,
 21,  7,
 23, 19,

 21,  7,
 23, 19,
 26, 27,
 30, 31,
 21,  7,
 23, 19,

255,

};


const char BorderTable[] PROGMEM ={//1 for each level...
0, //START FRAME(IS ABSOLUTE OFFSET INTO FRAME TABLE)
1, //NUM FRAMES(FRAMES MUST BE SEQUENTIAL)
};

const char BorderFrameTimes[] PROGMEM ={
30,30,

};