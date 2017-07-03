inline void SnakeyThink(uint8_t id);
inline void MedusaThink(uint8_t id);
inline void DonMedusaThink(uint8_t id);
inline void SkullThink(uint8_t id);
inline void AlmaThink(uint8_t id);
inline void RockyThink(uint8_t id);
inline void GolThink(uint8_t id);
inline void LeeperThink(uint8_t id);
inline void MobyThink(uint8_t id);
inline uint8_t EggThink(uint8_t id, uint8_t s);
inline uint8_t WaterEggThink(uint8_t id, uint8_t s);
inline void InitializeEnemy(uint8_t i, uint8_t id);


//enemy bitmap functions-enemy data is compressed, actual variables are packed into a buffer(less than 8 bits each). see lolodef.h

inline uint8_t get_enemy_x(uint8_t id){
	return (enemy[id].packedbits[0] & 0b00011111)>>0;
}

inline void set_enemy_x(uint8_t id,uint8_t val){
	enemy[id].packedbits[0] &= 0b11100000;//keep bits used for type
	enemy[id].packedbits[0] |= val;//no safety, dont a num greater than 31
}

inline uint8_t get_enemy_y(uint8_t id){
	return (enemy[id].packedbits[1] & 0b00011111)>>0;
}

inline void set_enemy_y(uint8_t id,uint8_t val){
	enemy[id].packedbits[1] &= 0b11100000;//keep bits used for offset
	enemy[id].packedbits[1] |= val;//no safety, dont a num greater than 31
}

inline uint8_t get_enemy_state(uint8_t id){
	return (enemy[id].packedbits[2] & 0b11111111)>>0;
}

inline void set_enemy_state(uint8_t id,uint8_t val){
	enemy[id].packedbits[2]  = (val & 0b11111111);
}

inline uint8_t get_enemy_frame(uint8_t id){
	return (enemy[id].packedbits[3] & 0b11000000)>>6;
}

inline void set_enemy_frame(uint8_t id,uint8_t val){
	enemy[id].packedbits[3] &= 0b00111111;//keep bits used for ftime/proj
	enemy[id].packedbits[3] |= (val<<6);//no safety, dont a num greater than 3
}

inline uint8_t get_enemy_ftime(uint8_t id){
	return (enemy[id].packedbits[3] & 0b00001111)>>0;
}

inline void set_enemy_ftime(uint8_t id,uint8_t val){
	enemy[id].packedbits[3] &= 0b11110000;//keep bits used for frame/proj
	enemy[id].packedbits[3] |= (val<<0);//no safety, dont a num greater than 15
}

inline uint8_t get_enemy_offset(uint8_t id){
	return (enemy[id].packedbits[1] & 0b11100000)>>5;
}

inline void set_enemy_offset(uint8_t id,uint8_t val){
	enemy[id].packedbits[1] &= 0b00011111;//keep bits used for y
	enemy[id].packedbits[1] |= (val<<5);//no safety, dont a num greater than 7
}

inline uint8_t get_enemy_type(uint8_t id){
	return (enemy[id].packedbits[0] & 0b11100000)>>5;
}

inline void set_enemy_type(uint8_t id,uint8_t val){
	enemy[id].packedbits[0] &= 0b00011111;//keep bits used for x
	enemy[id].packedbits[0] |= (val<<5);//no safety, dont a num greater than 7
}

inline uint8_t get_game_state(){
	return sound_enabled;
}

inline void set_game_state(uint8_t state){
	sound_enabled = state|1;//keep sound playing
}

inline void set_game_state_flag(uint8_t flags){
	sound_enabled |= flags;
}

inline void InitializeEnemy(uint8_t i, uint8_t id){
	uint8_t t = lvl.grid[i];
//	uint8_t x = (i%11)<<2;
//	uint8_t y = (i/11)<<2;
//	uint8_t state;
	uint8_t dir;
//	enemy[id].origin = i;

	lvl.grid[i] = 0;
	if(t >= MAPSNAKEY && t <= MAPMEDUSA){
		//no change
	}else{
		t -= MAPDONUP;
		dir = t%4;
		if(!dir)
			dir = 1;
		t >>= 2;//get type by getting rid of UDLR
		t += 2;//snakey and medusa are 0 and 1
//		enemy[id].type = t;
		set_enemy_state(id,dir);
	}
}



void MedusaKill(uint8_t id, uint8_t dir){
	
}


inline void EnemyInit(uint8_t id){

}





inline void SnakeyThink(uint8_t id){
	uint8_t ex,ey,estate,frame,ftime;
	char difx,dify;
	ex = get_enemy_x(id);
	ey = get_enemy_y(id);
	estate = get_enemy_state(id);
	frame = get_enemy_frame(id);
	ftime = get_enemy_ftime(id);
	//when all the hearts are gone Snakey goes berzerk!
	//16 ticks for frames 0 and frame 3, frame 1 and 2 are 8 ticks
	if(false){//snakeyfreakingout){
		if(!ftime){//either just went berzerk or needs to change frame
			if(estate & LEFT){
				if(frame < 1){//time to go right
					frame++;
					estate ^= LEFT;
				}else
					frame--;
			}else{
				if(frame > 2){//time to go left
					frame--;
					estate |= LEFT;
				}
			}

			ftime = 8;
			if(frame == 0 || frame == 3)
				ftime <<= 1;//16 ticks for edge frames
		}else//not time to change frames, check next tick
			ftime--;

		return;
	}

	difx = px-ex;
	dify = py-ey;

	if(py <= ey){
		dify = ey-py;
		if(px <= ex){//UL quadrant
			difx = ex-px;
			if(dify > difx)
				frame = 1;
			else
				frame = 0;
		}else{//UR quadrant
			difx = px-ex;
			if(dify > difx)
				frame = 2;
			else
				frame = 3;
			
		}
	}
	else{
		dify = py-ey;
		if(px <= ex){//LL quadrant
			difx = ex-px;
			if(dify > difx)
				frame = 1;
			else
				frame = 0;
		}else{//LR quadrant
			difx = px-ex;
			if(dify > difx)
				frame = 2;
			else
				frame = 3;
		}
	}
}

void Medusakill(uint8_t id, uint8_t dir){//handle medusa and don medusa killing lolo
	StopSong();
	TriggerFx(0,255,true);

	uint8_t projframe;
	if(get_enemy_type(id) == MEDUSA){
		pframe = 4;//set lolo to stone frame
		projframe = 0+0;
	}else{//don medusa
		pframe = 5;//set lolo to hit frame
		projframe = 0+2;
	}
		
}

uint8_t MedusaCanHit(uint8_t x, uint8_t y){//Used by Medusa and Don Medusa, only called when exactly aligned with Lolo on an axis
	char offset = 0;
	if(px == x){//lolo is above or below
		offset = y;
		if(py < y){//lolo is above
			while(offset < py){
				if(GetSolidForEnemyShotBit(x,offset--) || GetSolidForEnemyShotBit(x+1,offset--))
					return 0;//can't hit lolo 
			}
			return 1;//can hit lolo up
		}else{//lolo is below
			while(offset > py){
				if(GetSolidForEnemyShotBit(x,offset++) || GetSolidForEnemyShotBit(x+1,offset++))
					return 0;//can't hit lolo 
			}
			return 2;//can hit lolo down
		}
	}else{//lolo is left or right
		offset = x;
		if(px < x){//lolo is left
			while(offset < px){
				if(GetSolidForEnemyShotBit(offset--,y) || GetSolidForEnemyShotBit(offset--,y+1))
					return 0;//can't hit lolo 
			}
			return 3;//can hit lolo left
		}else{//lolo is right
			while(offset > px){
				if(GetSolidForEnemyShotBit(offset++,y) || GetSolidForEnemyShotBit(offset++,y+1))
					return 0;//can't hit lolo 
			}
			return 4;//can hit lolo right
		}
	}
}

inline void MedusaThink(uint8_t id){
	uint8_t ex = get_enemy_x(id);
	uint8_t ey = get_enemy_y(id);
	char difx = px - ex;
	char dify = py - ey;

	if( (difx < 2 && difx > -2) ||
		(dify < 2 && dify > -2)){//medusa is inline with lolo
		set_enemy_frame(id,1);//alarmed frame
		uint8_t t = MedusaCanHit(ex,ey);
		if(t){
			MedusaKill(id,--t);//u,d,l,r
			return;
		}
	}else{//can't see lolo
		set_enemy_frame(id,0);
	}
	DrawMapTile(ex,ey,5+get_enemy_frame(id));
}

inline void DonMedusaThink(uint8_t id){
	//try to move...
	MedusaThink(id);
}

inline void SkullThink(uint8_t id){
	if(lvl.heartsleft)
		return;//not awake yet


}

inline void AlmaThink(uint8_t id){

}

inline void RockyThink(uint8_t id){

}

inline void GolThink(uint8_t id){
	if(lvl.heartsleft)
		return;//not awake yet
//	uint8_t s = get_enemy_state(id);
}

inline void LeeperThink(uint8_t id){
	uint8_t s = get_enemy_state(id);
	if(s & SPECIAL0){//already sleeping, spends 64 ticks in each frame, all leepers frames are synched
		
		return;
	}//56/16/16/16/8/16
	//  0/ 1/ 0/ 2/3
}

inline void MobyThink(uint8_t id){//tile limits make this impractical..only 11 levels ever used him(lolo3 only)
	return;
}

inline uint8_t EggThink(uint8_t id, uint8_t s){//returns 1 if logic should continue immediately this frame(just got unegged?)
	if(s & SPECIAL0)//actually a water egg
		return WaterEggThink(id,s);
	return 0;
}

inline uint8_t WaterEggThink(uint8_t id, uint8_t s){
	return 0;
}

inline uint8_t FlyingEggThink(uint8_t id, uint8_t s){
	return 0;
}

inline void ExplosionThink(uint8_t id){
	uint8_t f = get_enemy_frame(id);
	uint8_t ft = get_enemy_ftime(id);
	if(!ft){
		if(++f > 8){//done exploding
			set_enemy_type(id,DUMMY);
			return;
		}
		set_enemy_frame(id,f);
		set_enemy_ftime(id,4);
	}else
		set_enemy_ftime(id,ft-1);
}

inline void ExplodeEnemies(){
	for(uint8_t i=0;i<MAX_ENEMIES;i++){
		set_enemy_type(i,EXPLOSION);
		set_enemy_frame(i,0);
		set_enemy_ftime(i,0);
	}
}
