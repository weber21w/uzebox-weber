const char pallet[] PROGMEM= {
0xFE,//DUMMY

#define BOMBER_PALLET_ENTRIES 8

#define SHADOW_PALLET 0//only needs 0x01
#define BLOCK_PALLET 3//only needs 0xFF

#define WHITE_PALLET 1

0x01,//outline
0xAC,//helmet shading
0x5F,//light foot and hand color
0xFF,//light helmet color
0x8A,//dark stomach color
0x77,//face/skin color
0xDC,//light stomach color
0x0F,//visor outline

#define BLACK_PALLET WHITE_PALLET+8

0x01,//outline-
0x49,//helmet shading-
0x5F,//light foot and hand color-
0x5B,//light helmet color-
0x49,//dark stomach color-
0x77,//face/skin color-
0x52,//light stomach color-
0x0F,//visor outline-

#define GREEN_PALLET BLACK_PALLET+8

0x01,//outline-
0x7B,//helmet shading-
0x5F,//light foot and hand color-
0x7B,//light helmet color-
0x61,//dark stomach color-
0xB7,//face/skin color-
0x73,//light stomach color-
0x0F,//visor outline-

#define RED_PALLET GREEN_PALLET+8

0x01,//outline-
0x0C,//helmet shading-
0xFF,//light foot and hand color-
0x0F,//light helmet color-
0x0B,//dark stomach color-
0xAF,//face/skin color-
0x17,//light stomach color-
0xDB,//visor outline-

#define POISON_PALLET 4*8

0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,
0x00,

#define DEATH_PALLET POISON_PALLET+8

0x01,
0x0C,
0x0F,
0x27,
0x0E,
0x2F,

#define BOMB_PALLET DEATH_PALLET+6

0x01,
0x49,
0x92,
0xA3,
0xED,
0xFF,
0x1E,
0x2F,
0x66,
0x5D,

};