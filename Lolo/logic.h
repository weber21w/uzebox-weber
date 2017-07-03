bool IsSolidForEnemyMove(){//includes grass as solid, enemy will always move if he gets unegged on grass anyways
	return false;
}

uint8_t GetGridTileType(uint8_t off){//11*11 bitpacked
	//what is the first bit we need?
	return lvl.grid[(off/8)]&(1<<(off%8));

}

void SetGridTileType(uint8_t off){//11*11 bitpacked
	lvl.grid[(off/8)] |= (1<<(off%8));
}


void ZeroState(){
	for(uint8_t i=0;i<MAX_ENEMIES;i++){
		for(uint8_t j=0;j<sizeof(enemy[i].packedbits);j++)
			enemy[i].packedbits[j] = 0;
		set_enemy_type(i,DUMMY);
	}

	px		=
	py		=
	poffset	=
	pstate	=
	pframe	=
	pftime	=
	pshots	=
//	pblink	=
	panim		=	0;
}


inline uint8_t LEVELHEARTSCOLLECTED(){
	return 0;
}


inline uint8_t GETNUMHEARTS(){
	return 0;
}

inline void SETNUMHEARTS(){

}

inline bool GETDEMOPLAYBACKFLAG(){
	return false;
}

inline void SETLOLOMOVED(){
	lvl.grid[75] |= 0b00100000;
}

inline bool GETLOLOMOVED(){
	return lvl.grid[75] & 0b00100000;
}

uint8_t prng(uint8_t val){
	/* taps: 16 14 13 11; feedback polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xB400u);
 	return (uint8_t)(lfsr%val);
}


void VsyncCallBack(){
	oldpadstate = padstate;//these are 8 bit variables, but all we care about is UP,DOWN,LEFT,RIGHT,B,START
	padstate = ReadJoypad(0);//&0xFF
	global_frame++;
}


void LoadLevel(uint8_t level){
/*	ZeroState();
	LoloClearVram(RAM_TILES_COUNT+BLACK_TILE);
	FATFS fs;
	FRESULT res;
	pf_mount(NULL);
	res = pf_mount(&fs);
	res |= pf_open("lolo1.lvl");
if(res)
	DebugCrash(0,1,0);
//level = 1;
	WORD sdoff = 0+(level*((11*11)+2));
	uint8_t freeenemy = 0;
WORD br;

//	sdoff = 0;
	pf_lseek(sdoff);
	pf_read((BYTE *)ram_tiles,((11*11)+2),&br);//load level data
	if(br != (11*11)+2)
		DebugCrash(0,2,0);

//for(uint8_t i=0;i<22;i++)
//ram_tiles[i] = MAPARROWDOWN;


	for(uint8_t i=0;i<(11*11);i++){
		lvl.grid[i] = ram_tiles[i];
		switch(lvl.grid[i]){
			case MAPBRICK:
			case MAPTREE:
			case MAPROCK:
			case MAPWATER:
			case MAPWATERUP:
			case MAPWATERDOWN:
			case MAPWATERLEFT:
			case MAPWATERRIGHT:
			continue;
			break;//these numbers stay the same
			
			case MAPLOLOSTART:
				lvl.grid[i] = BRICK;
				px = i%11;
				py = i/11;
			break;

			case MAPARROWUP:
			case MAPARROWDOWN:
			case MAPARROWLEFT:
			case MAPARROWRIGHT:
				lvl.grid[i] -= MAPARROWUP;
				lvl.grid[i] += ARROWUP;
			continue;

			case MAPWARP0:
			case MAPWARP1:
			case MAPWARP2:
			case MAPWARP3:
			case MAPWARP4:
			case MAPWARP5:
			case MAPWARP6:
			case MAPWARP7:
//				lvl.grid[i] -= MAPWARP0;
//				lvl.grid[i] += WARP0;
			continue;

			default://must be an enemy
			InitializeEnemy(i,freeenemy++);//maps are responsible for safe bounds
			lvl.grid[i] = BRICK;
			break;
		}
	}

	lvl.dooroff = (ram_tiles[(11*11)+0]<<4);
	lvl.dooroff |= ram_tiles[(11*11)+1];

DrawSidePanel();
//DrawFrame(4,4,12,12,1);
lvl.dooroff = 0b01100000;
DrawBorder(0);
//for(uint8_t i=0;i<11*11;i++){SetLevelGrid(i,WATER);}
//SetLevelGrid((11*5),TREE);

DrawLevel();
*/
}


inline bool GetSolidBit(uint8_t x, uint8_t y){
	uint8_t t = ((y*22)+x)/8;//which byte?
	uint8_t t2 = ((y*22)+x)%8;//which bit?
	t = lvl.grid[t];
	t = t & (128>>t2);
	return t;
}

inline void SetSolidBit(uint8_t x, uint8_t y){
	uint8_t t = ((y*22)+x)/8;
	uint8_t t2 = ((y*22)+x)%8;

	t = lvl.grid[t];
	//TODO
	t = t & (128>>t2);
}

inline bool GetSolidForEnemyShotBit(uint8_t x, uint8_t y){
	return true;
}

inline void SetSolidForEnemyShotBit(uint8_t x, uint8_t y){

}


inline void UpdateLevel(){
	for(uint8_t i=0;i<11*11;i++){
//		if(TileUpdateFuncs[] != NULL)
//			TileUpdateFuncs[](i);
	}
}

inline void UpdateEnemies(){
	for(uint8_t i=0;i<MAX_ENEMIES;i++){
		uint8_t s = get_enemy_state(i);
		if(s & EGGED)
			if(!EggThink(i,s))//did it finish being an egg and need more thinking this frame?
				continue;
		uint8_t t = get_enemy_type(i);
		switch(t){
			case DUMMY:							break;
			case SNAKEY:	SnakeyThink(i);		break;
			case SKULL:		SkullThink(i);		break;
			case ALMA:		AlmaThink(i);		break;
			case ROCKY:		RockyThink(i);		break;
			case LEEPER:	LeeperThink(i);		break;
			case DONMEDUSA:	DonMedusaThink(i);	break;
			case GOL:		GolThink(i);		break;
			case MOBY:		MobyThink(i);		break;
			case MEDUSA:	MedusaThink(i);		break;
			case EXPLOSION:	ExplosionThink(i);	break;

			default:
			DebugCrash(16,t,0);
			break;
		};
		
	}
}

void RestoreGrid11(uint8_t goff){

}

void GrabHeart(){
	TriggerFx(0,255,true);
	uint8_t goff = (px>>1)+((py>>1)*11);
	lvl.grid[goff] = BRICK;
	RestoreGrid11(goff);
	SETNUMHEARTS(GETNUMHEARTS()-1);
}



void Setuppb(uint8_t x, uint8_t y){
	pb_x = x;pb_y=y;pb_offset=0;//setup the sprite based block
	RestoreTile(x+0,y+0);//restore tiles underneath emerald framer
	RestoreTile(x+1,y+0);
	RestoreTile(x+0,y+1);
	RestoreTile(x+1,y+1);
}

void UpdateLolo(){
#define FIRSTEFTILE 179
return;
	if(pstate & MOVING){
		if(poffset)
			goto LOLOMOVEBOTTOM;

		uint8_t s0,s1,e0,e1,v0,v1,g0,g1,f0,f1,a0,vsource;
		vsource = ((py+FIRSTMAPY)*VRAM_TILES_H)+(px+FIRSTMAPX);
		
		//check to see if either of the 2 squares we are moving into is blocked
		if(pstate & UP){
			s0 = GetSolidBit(px+0,py-1);s1 = GetSolidBit(px+1,py-1);
			e0 = GetSolidBit(px+0,py-3);e1 = GetSolidBit(px+1,py-3);
			v0 = vram[vsource-(VRAM_TILES_H*1)+0]-RAM_TILES_COUNT;v1 = vram[vsource-(VRAM_TILES_H*1)+1]-RAM_TILES_COUNT;
			g0 = lvl.grid[(((py-2)>>1)*11)+((px+0)>>1)];g1 = lvl.grid[(((py-2)>>1)*11)+((px+1)>>1)];
			f0 = FIRSTEFTILE+2;f1 = FIRSTEFTILE+3;
			a0 = ARROWDOWN;
			Setuppb(px+0,py-2);
		}
		else if(pstate & DOWN){
			s0 = GetSolidBit(px+0,py+2);s1 = GetSolidBit(px+1,py+2);
			e0 = GetSolidBit(px+0,py+4);e1 = GetSolidBit(px+1,py+4);
			v0 = vram[vsource+(VRAM_TILES_H*2)+0]-RAM_TILES_COUNT;v1 = vram[vsource+(VRAM_TILES_H*2)+1]-RAM_TILES_COUNT;
			g0 = lvl.grid[(((py+3)>>1)*11)+((px+0)>>1)];g1 = lvl.grid[(((py+3)>>1)*11)+((px+1)>>1)];
			f0 = FIRSTEFTILE+0;f1 = FIRSTEFTILE+1;
			a0 = ARROWUP;
			Setuppb(px+0,py+2);
		}
		else if(pstate & LEFT){
			s0 = GetSolidBit(px-1,py+0);s1 = GetSolidBit(px-1,py+1);
			e0 = GetSolidBit(px-3,py+0);e1 = GetSolidBit(px-3,py+1);
			v0 = vram[vsource+(VRAM_TILES_H*0)-1]-RAM_TILES_COUNT;v1 = vram[vsource+(VRAM_TILES_H*1)-1]-RAM_TILES_COUNT;
			g0 = lvl.grid[(((py+0)>>1)*11)+((px-2)>>1)];g1 = lvl.grid[(((py+1)>>1)*11)+((px-2)>>1)];
			f0 = FIRSTEFTILE+1;f1 = FIRSTEFTILE+3;
			a0 = ARROWRIGHT;
			Setuppb(px-2,py+0);
		}
		else{//RIGHT
			s0 = GetSolidBit(px+2,py+0);s1 = GetSolidBit(px+1,py+1);
			e0 = GetSolidBit(px+4,py+0);e1 = GetSolidBit(px+1,py+1);
			v0 = vram[vsource+(VRAM_TILES_H*0)+2]-RAM_TILES_COUNT;v1 = vram[vsource-(VRAM_TILES_H*1)+2]-RAM_TILES_COUNT;
			g0 = lvl.grid[(((py+0)>>1)*11)+((px+2)>>1)];g1 = lvl.grid[(((py+1)>>1)*11)+((px+2)>>1)];
			f0 = FIRSTEFTILE+0;f1 = FIRSTEFTILE+2;
			a0 = ARROWLEFT;
			Setuppb(px+2,py+0);
		}

		if(g0 == a0 || g1 == a0)//can't move into opposite direction arrow
			goto LOLOMOVEFAIL;	
		if(!s0 && !s1)//not blocked
			goto LOLOMOVEBOTTOM;
		if((s0 && !s1) || (!s0 && s1))//only 1 solid, no way to move
			goto LOLOMOVEFAIL;
		if(v0 != f0 || v1 != f1)//blocked but not an emerald framer or not aligned with just one, move fails
			goto LOLOMOVEFAIL;
		//if we are here we are pushing against an emerald framer. see if something beyond it is blocking
		if(e0 || e1){
			pb_offset = 255;//cancel pb that we setup at beginning
			goto LOLOMOVEFAIL;
		}

LOLOMOVEBOTTOM:
		pblink = 128+prng(128);
		if(!pftime){
			if(++pframe > 5)
				pframe = 0;
		}else
			pftime--;

		if(++poffset > 7){//already moving see if we are done
			poffset = 0;
			if(pstate & UP)
				py--;
			else if(pstate & DOWN)
				py++;
			else if(pstate & LEFT)
				px--;
			else
				px++;
		}

	}else{//not moving, check blinking
		if(global_frame & 2){
			pblink--;
			if(pblink == 255)
				pblink = 128+prng(128);
		}
	}

LOLOMOVEFAIL:
	return;
}

uint8_t DetermineSandGrassMapTile(uint8_t x, uint8_t y, uint8_t t){
	uint8_t fkey = 0;//basically see how many connections there are U,D,L,R and pick the right frame
	if(y < 1 || t == lvl.grid[((y-1)*11)+x])
		fkey |= 1;
	if(y > 9 || t == lvl.grid[((y+1)*11)+x])
		fkey |= 2;
	if(x < 1 || t == lvl.grid[(y*11)+(x-1)])
		fkey |= 4;
	if(x > 9 || t == lvl.grid[(y*11)+(x+1)])
		fkey |= 8;
	t = pgm_read_byte(&SandGrassFrameKeys[fkey]);

//	if(lvl.grid[off-1] == SAND)
//		t += 14;//offset past grass frames to sand which are in same order
	return t;
}

void RestoreTile(uint8_t x, uint8_t y){
	uint8_t gx,gy,ox,oy,t;
	bool shadow=false;
	gx = x>>1;gy = y>>1;
	ox = x&1;oy = y&1;//calculate offset into tile frame
	t = lvl.grid[(gy*11)+gx];

	if(gx == 0 && t < SAND)//go to shadow version of frame
		shadow = true;

		
	if(t == SAND || t == GRASS)//determine which frames to draw based on surronding grass or sand
		t = DetermineSandGrassMapTile(gx,gy,t);
	else{
		t = pgm_read_byte(&MapFrameByType[t]);
		if(shadow)
			t++;
	}
	uint16_t toff = (t*4)+(y<<1)+x;//get offset into tile frame
	t = pgm_read_byte(&TileFrames[toff]);
	SetTile(x+FIRSTMAPX,y+FIRSTMAPY,t);
}

void RestoreMapTile(uint8_t x, uint8_t y){
	RestoreTile(x+0,y+0);
	RestoreTile(x+1,y+0);
	RestoreTile(x+0,y+1);
	RestoreTile(x+1,y+1);
}

void OpenChest(){
	TriggerFx(0,255,true);
	for(uint8_t i=0;i<11*11;i++){
		if(lvl.grid[i] == CHEST){
			lvl.grid[i] = CHESTOPEN;
			DrawMapTile(FIRSTMAPX+((i%11)<<1),FIRSTMAPX+((i/11)<<1),38);
			return;
		}
	}
}

void CaptureChest(){
	ExplodeEnemies();
	TriggerFx(SFX_EXPLOSION,255,1);
}	


bool UsePower(){
	if(poffset)
		return false;

	uint8_t powertouse,t;
	for(uint8_t i=0;i<3;i++){
		if((lvl.powerpanel[i]&3) != POWERNONE)
			powertouse = i;
		else
			powertouse = 255;
	}
	if(powertouse == 255 || LEVELHEARTSCOLLECTED() < ((lvl.powerpanel[powertouse] & 0b11111100)>>2))//enough hearts to use?
		return false;
	
	t = powertouse;	
	powertouse = lvl.powerpanel[powertouse] & 3;
	uint8_t effectlocation = lvl.grid[((py>>1)*11)+(px>>1)];

	if(pstate & (UP|DOWN)){
		if((py&1) ||//offset, cant use power
		(pstate & UP && py < 1) ||//don't try powers out of bounds
		(pstate & DOWN && py > 20))
			return false;

		if(pstate & UP)
			effectlocation -= 11;
		else
			effectlocation += 11;		

	}else{//left or right
		if((px&1) ||//offset, cant use power
		(pstate & LEFT && px < 1) ||//don't try powers out of bounds
		(!(pstate & LEFT) && px > 20))
			return false;

		if(pstate & LEFT)
			effectlocation--;
		else
			effectlocation++;
	}

	if(powertouse == POWERBRIDGE){
		if(lvl.grid[effectlocation] >= WATER && lvl.grid[effectlocation] <= WATERRIGHT){
			powertouse = (pstate &(UP|DOWN))?YBRIDGE:XBRIDGE;
			lvl.grid[effectlocation] = powertouse;
			//RestoreGrid(
		}else
			return false;
	}else if(powertouse == POWERHAMMER){
		if(lvl.grid[effectlocation] == ROCK)
			lvl.grid[effectlocation] = BRICK;
		else
			return false;
	}else{//power arrow
		powertouse = lvl.grid[effectlocation];
		if(powertouse == ARROWUP)
			powertouse = ARROWRIGHT;
		else if(powertouse == ARROWDOWN)
			powertouse = ARROWLEFT;
		else if(powertouse == ARROWLEFT)
			powertouse = ARROWUP;
		else if(powertouse == ARROWRIGHT)
			powertouse = ARROWDOWN;
		else
			return false;

	}
	TriggerFx(1,192,true);
	return true;
}

void OpenDoor(){
	pshots = 0;//per lolo 2
	TriggerFx(0,255,true);
	for(uint8_t i=0;i<MAX_ENEMIES;i++){//turn all active enemies into explosions
		//TODO turn all respawning enemies into dummies and clear any respawn markers
//		if(enemy[i].type == DUMMY)
//			continue;
//		enemy[i].type = ENEMYEXPLODE;
		set_enemy_ftime(i,6);
		set_enemy_frame(i,0);
		//RestoreMapTile(get_enemy_x(i),get_enemy_y(i));
	}
}

const uint8_t LoloDeathAnim[] PROGMEM = {//as per lolo 3, this does not start until a projecttile reaches lolo, he stays in specific death frame until then
// 0 = shocked frame 1 = tongue down 2 = tonge up 3 = X eyes
0,1,2,1,2,1,2,1,2,1,2,1,2,1,2,3,
};

const uint8_t LoloDeathAnimFtime[] PROGMEM = {
30,3,4, 4,4,4,4,4,4,4,4,4,4,4,4,40,
};

void StartLevelSong(){
	if(!(music & MUSICNOTRANDOM)){
		music = 1 | (prng(NUMSONGS)<<7);//save b0, it is actually kernel sound_enable
	}

	StartSong(LoloMusic[(music>>1)&0x0F]);//remember: music is actually #define sound_on so always keep b0 set!
}



void LoloDie(){//per lolo 3
	StopSong();
	//if a projectile is coming for lolo, we don't start until its done.


	StartSong(LoloDieSong);
	uint8_t waitticks,frame,fpos = 0;
	for(uint8_t i=0;i<16;i++){
		waitticks = pgm_read_byte(&LoloDeathAnimFtime[fpos]);
		frame = pgm_read_byte(&LoloDeathAnim[fpos++]);
		DrawSpriteFrame((px+FIRSTMAPX)*8,(py+FIRSTMAPY)*8,frame,0);
		WaitVsync(waitticks);
	}
	//after lolo stays in x eyes for 40 ticks then screen starts to go black
	//screen fades to black in 12 ticks with 3 levels of darkness each lasting 4 ticks
	FadeOut(1,true);
	//screen stays black for 51 frames
	WaitVsync(51);
	FadeIn(1,false);//screen fades in 12 ticks
}

void LoloGiveup(){//per Lolo 3
	//30 frames before it starts fading
	//StartSong(GiveUpSong);
	WaitVsync(30);
	//12 frames of fading (3 actual levels of fade)
	FadeOut(1,true);
	//51 frame before you see the first fading in frame(all sprites have been drawn) 12 frames of fading in (3 actual levels of fade)
	WaitVsync(51);
	FadeIn(1,true);
	
	//StartStageSong();//main song restart on the first full bright frame


}


