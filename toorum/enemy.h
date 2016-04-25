/*
 Toorum's Quest II
 Copyright (c) 2013 Petri Hakkinen
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions: 

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/


#define MAX_ENEMIES	4

#define ENEMY_WYVERN	0
#define ENEMY_GHOST		1

typedef struct{
	uint8_t	x, y;
	uint8_t	oy;	// y origin for wyvern
	int8_t	dir;
	uint8_t	frame;
	uint8_t	walkPhase;
	uint8_t type;
}Enemy;

void initEnemies();
void updateEnemies();

Enemy	enemies[MAX_ENEMIES];
uint8_t	numEnemies;


void updateWyvern(Enemy* e);
void updateGhost(Enemy* e);

bool isObstacle(uint8_t x, uint8_t y) {
	uint8_t tile = sampleTile(x, y);
	return ((tile >= TILE_WALL && tile <= TILE_WALL+3) || (tile >= TILE_WALL_DARK && tile <= TILE_WALL_DARK+3) || (tile >= TILE_DOOR && tile <= TILE_DOOR+3));
}

bool isWalkable(uint8_t x, uint8_t y) {
	uint8_t tile = sampleTile(x, y);
	return ((tile >= TILE_WALL && tile <= TILE_WALL+3) || (tile >= TILE_WALL_DARK && tile <= TILE_WALL_DARK+3) || (tile >= TILE_LADDER && tile <= TILE_LADDER+3));
}

bool hitPlayer(Enemy* e) {

	return p.x + 9 >= e->x && p.y + 13 >= e->y && p.x + 3 <= e->x + 13 && p.y <= e->y +13;//fixed for 16x16 tiles
}


void initEnemy(uint8_t x, uint8_t y, uint8_t tile){
			
	if(numEnemies >= MAX_ENEMIES)//protect against bad user levels
		return;

	Enemy* e = &enemies[numEnemies++];
					
	uint8_t sprite;
	// use sprite index 1?
	if(tile == TILE_WYVERN_2ND) {
		tile = TILE_WYVERN;
		//	e->spr = 1;
	}else if(tile == TILE_GHOST_LEFT_2ND) {
		tile = TILE_GHOST_LEFT;
		//	e->spr = 1;
	}

	if(tile >= TILE_WYVERN && tile <= TILE_WYVERN_2ND)
		sprite = SPRITE_WYVERN;
	else
		sprite = SPRITE_GHOST;

	e->x = x * 8;//8;
	e->y = y * 8;//8;
	e->oy = e->y;
	e->frame = sprite;//tile;
	e->dir = (tile == TILE_GHOST_LEFT ? -1 : 1);
	e->walkPhase = numEnemies * 123*2;
	e->type = (sprite == SPRITE_WYVERN ? ENEMY_WYVERN : ENEMY_GHOST);
	SetMapTile(x, y, FIRST_MAP_TILE);

}


void updateEnemies(){

	if(p.gameover)
		return;

	for(uint8_t i=0;i<numEnemies;i++){

		Enemy* e = &enemies[i];

		if(e->type == ENEMY_WYVERN)
			updateWyvern(e);
		else
			updateGhost(e);		

		updateSprite((i*4)+4, e->frame, (e->dir == 1), e->x, e->y);//save 0-3 for player and expansion?

		if(hitPlayer(e))
			hurtPlayer();

	}
}

void updateWyvern(Enemy* e){

	if(e->walkPhase & 1){
		uint8_t o = (e->walkPhase>>1) & 127;//63;
		int8_t w = pgm_read_byte_near(sintab + o);

		e->y = e->oy + w;
	}

	//if(frame & 1)//fix for 60hz
		e->walkPhase++;
	
	e->frame = SPRITE_WYVERN + ((e->walkPhase/8) & 1);

}

void updateGhost(Enemy* e){

	if(e->walkPhase & 1){
		// turn around

		if(e->dir < 0) {
			if(e->x == 0 || isObstacle(e->x + 1, e->y + 7*2) || !isWalkable(e->x +1, e->y + 8*2))//-1,+7*2/ -1*2 +8*2
				e->dir = 1;
		} else {
			//if(isObstacle(e->x + 8, e->y + 7) || !isWalkable(e->x + 8, e->y + 8))
			if(isObstacle(e->x + 14, e->y + 14) || !isWalkable(e->x + 14, e->y + 16))//fixed for 16x16 tiles
				e->dir = -1;
		}
		e->x += e->dir;
	}

	if(!(frame & 1))
		e->walkPhase++;

	e->frame = SPRITE_GHOST+((e->walkPhase/6) & 1);

}

