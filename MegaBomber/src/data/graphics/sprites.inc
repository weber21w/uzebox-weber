const char GameSprites[] PROGMEM = {
#include "bomber.inc"
#include "death.inc"
#include "bomb.inc"
#include "block.inc"
#if MAP == 0
#include "map0/specialsprites.inc"
#elif MAP == 1 
#include "map1/specialsprites.inc"
#elif MAP == 2 
#include "map2/specialsprites.inc"
#elif MAP == 3 
#include "map3/specialsprites.inc"
#elif MAP == 4
#include "map4/specialsprites.inc"
#endif
};
