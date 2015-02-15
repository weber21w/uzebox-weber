uint16_t ProcessAI(uint8_t i){//return 0;


//DrawNumber(Frogs[i].AIState,4,0,1);
//DrawNumber(Frogs[i].State,4,2,1);
//DrawNumber(JoyVal[i],4,4,1);
//	DrawNumber(0,8,2,1);	DrawNumber(0,7,2,1);
	switch (Frogs[i].AIState){

		case AI_STUNNED:
			return AIStun(i);
		case AI_SIT:
			return AISit(i);
		case AI_JUMP:
			return AIJump(i);
		case AI_CENTER:
			return AICenter(i);
		case AI_SWIM:
			return AISwim(i);
//		case AI_TONGUE:
//			return AITongue(i);
//		case AI_LAND:
//			return AILand(i);
//		case AI_EVADE:
//			return AIEvade(i);
	}
	return 0;
}

uint16_t AIStun(uint8_t i){
/*	if(!Frogs[i].State & STUNNED)
		Frogs[i].AIState = AI_SIT;
	else */
	Frogs[i].AITarget = 0;
	if((prng()&15) > 11)
		return BTN_B;
	else
		return 0;
}



uint16_t AISit(uint8_t i){
	Frogs[i].AITarget = 0;
	uint16_t rnd = (prng()%161);
	if(AIFlyInJumpRange(i) || AIFliesInKillZone(i)>2){//2){
			Frogs[i].AIState	= AI_JUMP;
			return 0;//JUMP_BUTTON;
	}
	
	if((Frogs[!i].State & STOMPING) && ((Frogs[i].x > Frogs[!i].x && Frogs[i].x-Frogs[!i].x < 16)||(Frogs[!i].x > Frogs[i].x && Frogs[!i].x-Frogs[i].x < 16))){//are we in danger of being stomped?
	//FFTriggerFx(1);
		//Frogs[i].AIState = AI_EVADE;
		uint8_t evade_chance = prng()&31;
		if(evade_chance == 5){//we will just jump out of the way, might take 1 second to react
			if(rnd < 37)//most of the time do a full fledged jump, sometimes just a very quick jump to evade quickly
				Frogs[i].AIState = AI_JUMP;
			if(Frogs[i].State & FACING)
				return (JUMP_BUTTON|BTN_RIGHT);
			else
				return (JUMP_BUTTON|BTN_LEFT);
		}else if(evade_chance < 6){//we will hop out of the way(totally inferior to just jumping, but for more life like actions)
			if(Frogs[!i].x < Frogs[i].x)//quickest to go right
				return BTN_RIGHT;
			else
				return BTN_LEFT;
		}
	}else if(rnd==93)
		Frogs[i].AIState = AI_CENTER;
	return 0;
}


uint16_t AIJump(uint8_t i){
	if(Frogs[i].State & STOMPING){
		return 0;
	}

	//see if the enemy is stomping and close on the x-axis. We might have jumped just to dodge the attack, let's counter-attack!
	if(Frogs[!i].State & STOMPING && Frogs[i].y < Frogs[!i].y){//he is stomping and we are above him!
		if((Frogs[i].x > Frogs[!i].x && (Frogs[i].x-Frogs[!i].x < 16)) || (Frogs[!i].x > Frogs[i].x && (Frogs[!i].x-Frogs[i].x < 16)))
			if((prng()&5) == 1)
				return BTN_DOWN;//he is vulnerable, attack!

	}

//	StartY	= Frogs[i].y - pgm_read_byte(&JumpWave[Frogs[i].JumpTime]) + 2;

	uint8_t x1,y1,rnd0,rnd1;
	uint16_t natural_direction;
	rnd0 = prng()&1;//7;
	rnd1 = prng()&1;//7;
	if((Frogs[i].State & FACING)){
		natural_direction = BTN_RIGHT;
		x1 = ((Frogs[i].x+16)-rnd0)+rnd1;
	}else{
		natural_direction = BTN_LEFT;
		x1 = ((Frogs[i].x-16)-rnd0)+rnd1;
	}
	//y1 = (Frogs[i].y- pgm_read_byte(&JumpWave[Frogs[i].JumpTime]))+2;
	y1 = ((Frogs[i].y+4)+1)-1;//rnd0)-rnd1;

	if(!(Frogs[i].State & TONGUEING)){
		for(uint8_t j=0;j<MAXFLIES;j++){
			if(!(Flies[j].State & ACTIVE) || Flies[j].RebirthTimer)
				continue;


			uint8_t w = (11+2)+(Cheats[i]*24);//pgm_read_byte(&TongueLength[Frogs[i].TongueTime >> 1])+(Cheats[i]*24);
			int16_t x1 = (int16_t)((Frogs[i].State & FACING)?(Frogs[i].x+16):(Frogs[i].x-w));
			if(	(Frogs[i].y+(6+2) < Flies[j].y) ||
				(Frogs[i].y+(4-2) > Flies[j].y+(8+2)) ||
				(x1 > Flies[j].x+(8+2)) ||
				(x1+w < Flies[j].x))
				continue;
		return (LICK_BUTTON|JUMP_BUTTON|natural_direction);
		}
	}
//see if we are in the kill zone and close to a fly that we should steer towards
		uint8_t steer_weight[2] = {0,0};//prng()&1,prng()&1};
		if(Frogs[i].y < KILLHEIGHT){
			for(uint8_t j=0;j<MAXFLIES;j++){
				if(!(Flies[j].State & ACTIVE) || Flies[j].RebirthTimer)
					continue;

				if(Frogs[i].State & FACING){//we are moving right, favor flies in front of us or very close behind(since we can't steer back much)
					if(Flies[j].x > (Frogs[i].x+8)){//fly is ahead of us
						if((Flies[j].x-Frogs[i].x) > 16+18)//too far forward to consider yet
							continue;
						steer_weight[1] += (Flies[j].x-Frogs[i].x);
					}else{//fly is behind us
						if(((Frogs[i].x+8)-Flies[j].x) > 5)//too far back, not realistic to swing back
							continue;
						steer_weight[0] += 0+((Frogs[i].x+8)-Flies[j].x);
					}
				}else{//we are moving left
					if(Flies[j].x < (Frogs[i].x+0)){//fly is ahead of us
						if((Frogs[i].x-Flies[j].x) > 18)//too far forward to consider yet
							continue;
						steer_weight[0] += (Frogs[i].x-Flies[j].x);
					}else{//fly is behind us
						if((Flies[j].x-Frogs[i].x) > 3)//too far back, not realistic to swing back
							continue;
						steer_weight[1] += 0+(Flies[j].x-Frogs[i].x);
					}
				}
				/*
				if(Flies[j].x > Frogs[i].x && (Flies[j].x-Frogs[i].x) > 12+(Frogs[i].State&FACING?22:0))//too far
					continue;
				if(Flies[j].x < Frogs[i].x && (Frogs[i].x-Flies[j].x) > 12+(Frogs[i].State&FACING?0:22))//too far
					continue;
				if(Flies[j].x < Frogs[i].x)
					steer_weight[0]++;
				else
					steer_weight[1]++;
			*/
			}
	//		DrawNumber(steer_weight[0],6,4,1);
	//		DrawNumber(steer_weight[1],22,4,1);
			if(steer_weight[1] > 6 && steer_weight[1] > steer_weight[0])
				return (JUMP_BUTTON|BTN_RIGHT);
			else if(steer_weight[0] > 6)
				return (JUMP_BUTTON|BTN_LEFT);
		}



	//see if we are jumping against the edge of the screen and we should do a double jump
	if(Frogs[i].State & JUMPING){//NOT falling
		if(Frogs[i].x < 2){
			if(Frogs[i].State & FACING){//we can jump, let go of jump if we were holding it, then press it again next frame
				if(OldJoyVal[i] & JUMP_BUTTON)
					return 0;
				return (JUMP_BUTTON|BTN_RIGHT);
			}else
				return BTN_RIGHT;

		}else if(Frogs[i].x > (SCREEN_TILES_H*TILE_WIDTH)-18){
			if(!(Frogs[i].State & FACING)){//we can jump, let go of jump if we were holding it, then press it again next frame
				if(OldJoyVal[i] & JUMP_BUTTON)
					return 0;
				return (JUMP_BUTTON|BTN_LEFT);
			}else
				return BTN_LEFT;

		}
	}
	//see if we should stomp and we aren't over the middle water(otherwise bot frequently stomps there and never hits, slowing him down)
	if(!AIFliesInKillZone(i)){//nothing else to do this jump, might as well try attacking
		if((Frogs[i].x < MIDDLELEFT || Frogs[i].x > MIDDLERIGHT) && (prng() < 3 || (Frogs[i].State & (FALLING|DROPPING))) && (Frogs[i].y < (Frogs[!i].y-15)) && !(Frogs[!i].State & (SWIMMING|STUNNED))){//we are falling and the enemy is not in an immune state
			if(Frogs[i].x > Frogs[!i].x){
				if(Frogs[i].x - Frogs[!i].x < 14)
					return BTN_DOWN;
			}else if(Frogs[!i].x - Frogs[i].x < 14)
				return BTN_DOWN;
		}
	}



	return JUMP_BUTTON|natural_direction;
}

uint16_t AICenter(uint8_t i){
//	DrawNumber(Frogs[i].AITarget,8,2,1);
	if(!Frogs[i].AITarget){//find a place we want to get to
		if(Frogs[i].x < (SCREEN_TILES_H*TILE_WIDTH)/2)//we are on left side
			Frogs[i].AITarget = (int16_t)(FROG1X-(prng()&7))+(prng()&31);
		else
			Frogs[i].AITarget = (int16_t)(FROG2X+(prng()&7))-(prng()&31);

	}

	if((Frogs[i].State & HOPPING))
		return 0;
	else if(AIFlyInJumpRange(i) > 1){
		Frogs[i].AIState	= AI_JUMP;
		Frogs[i].AITarget = 0;
		return 0;//JUMP_BUTTON;
	}else{
		uint8_t dist = prng()&5;
		uint8_t t;
		if((Frogs[i].AITarget < Frogs[i].x) && ((t = Frogs[i].x-Frogs[i].AITarget) > dist)){//need to move left
			if(t > 16){//faster to jump?
				if((OldJoyVal[i] & JUMP_BUTTON) && Frogs[i].y < FROGY-34)
					return 0;//let go of jump so we can jump next tick
				else if(!(Frogs[i].State & (FALLING|JUMPING)) && !(Frogs[i].State & FACING))//on ground and facing right direction?
					return (JUMP_BUTTON|BTN_LEFT);
				else
					return BTN_LEFT;
			}else
				return BTN_LEFT;
		}else if((Frogs[i].AITarget > Frogs[i].x) && ((t = Frogs[i].AITarget-Frogs[i].x) > dist)){//need to move right
			if(t > 16){//faster to jump?
				if(OldJoyVal[i] & JUMP_BUTTON && Frogs[i].y < FROGY-14)
					return 0;//let go of jump so we can jump next tick
				if(!(Frogs[i].State & (FALLING|JUMPING)) && (Frogs[i].State & FACING))//on ground and facing right direction?
					return (JUMP_BUTTON|BTN_RIGHT);
				else
					return BTN_RIGHT;
			}else
				return BTN_RIGHT;
		}
	}
	Frogs[i].AIState	= AI_SIT;
	Frogs[i].AITarget = 0;
	return AICheckFacing(i);
}

uint16_t AISwimTowardsPad(uint8_t i){
	Frogs[i].AITarget = 0;
	if(Frogs[i].x < LEFTEDGE)
		return BTN_RIGHT;
	else if(Frogs[i].x > MIDDLELEFT && Frogs[i].x < MIDDLERIGHT){
		if(Frogs[i].x < SCREENLEFT + (SCREENRIGHT - SCREENLEFT) / 2)
			return BTN_LEFT;
		else
			return BTN_RIGHT;
	}else if(Frogs[i].x > RIGHTEDGE)
		return BTN_LEFT;
	return 0;
}

uint16_t AISwim(uint8_t i){
	Frogs[i].AITarget = 0;
	if((Frogs[i].State & SWIMMING)){
		return AISwimTowardsPad(i);
	}else{
		uint8_t rnd	= prng()%100;
		if(rnd < 60)
			Frogs[i].AIState	= AI_SIT;
		else
			Frogs[i].AIState	= AI_CENTER;
	}
	return 0;
}
/*
uint16_t AITongue(uint8_t i){
	if(!(Frogs[i].State & TONGUEING)){
		if((Frogs[i].State & JUMPING))
			Frogs[i].AIState	= AI_JUMP;
		else
			Frogs[i].AIState	= AI_LAND;
	}
	return 0;
}

uint16_t AILand(uint8_t i){
	if((Frogs[i].State & SWIMMING)){
		Frogs[i].AIState	= AI_SWIM;
		return AISwimTowardsPad(i);
	}else{
		Frogs[i].AIState	= AI_SIT;
		return AICheckFacing(i);
	}
}
*/
uint8_t AIFlyInJumpRange(uint8_t i){
	uint8_t	Difference1;
	uint8_t	Difference2;
	uint8_t	Value;
	uint8_t NumFlies = 0;

	/* Check if in jumping range of fly (considering facing direction) */
	for(uint8_t j=0;j<MAXFLIES;j++){
		if(!(Flies[j].State & ACTIVE) || Flies[j].RebirthTimer)
			continue;
		/*if(Frogs[1].Score > Frogs[0].Score){
			Difference1	= Frogs[1].Score - Frogs[0].Score;
			if(Difference1 < 3)
				Value	= 2;
			else{
				if(Difference1 > 6)
					Difference1	=- 6;
				Value		= prng()%(Difference1 + 2);
			}
		}else*/
			Value		= 2;//prng()&3;

		Difference1	= Frogs[i].x - Flies[j].x;
		Difference2	= Flies[j].x - Frogs[i].x;

		if(!(Frogs[i].State & FACING)){
			if(!(Flies[j].State & FACING) && (Difference1 > JUMPAWAY) && (Difference1 < JUMPAWAY + Value))
				NumFlies++;
			else if((Flies[j].State & FACING) && (Difference1 > JUMPTOWARDS) && (Difference1 < JUMPTOWARDS + Value))
				NumFlies++;
		}else{
			if((Flies[j].State & FACING) && (Difference2 > JUMPAWAY) && (Difference2 < JUMPAWAY + Value))
				NumFlies++;
			else if(!(Flies[j].State & FACING) && (Difference2 > JUMPTOWARDS) && (Difference2 < JUMPTOWARDS + Value))
				NumFlies++;
		}
	}
	return	NumFlies;
}


uint8_t AIFliesInKillZone(uint8_t i){
	if((Frogs[i].State & FALLING) && (FROGY > KILLHEIGHT))//we wont be catching any more flies this jump
		return 0;
	uint8_t NumFlies = 0;
	uint8_t left_edge = KILLZONELEFT;
	uint8_t right_edge = KILLZONERIGHT;
	for(uint8_t j=0;j<MAXFLIES;j++){
		if(!(Flies[j].State & ACTIVE) || Flies[j].RebirthTimer)
			continue;
		if(Flies[j].x > left_edge && Flies[j].x < right_edge){
			if(Frogs[i].State & FACING){
				if(Frogs[i].x > Flies[j].x+4)//already past way the fly, forget it
					continue;
			}else if(Frogs[i].x+4 < Flies[j].x)//went past him
				continue;
			NumFlies++;
		}
	}
//	DrawNumber(NumFlies,3,6,1);
	return NumFlies;
}

uint16_t AICheckFacing(uint8_t i){
	if(!(Frogs[i].State & FACING) && Frogs[i].x < SCREENLEFT+(SCREENRIGHT - SCREENLEFT) / 2 && Frogs[i].x < LEFTEDGE +(MIDDLELEFT - LEFTEDGE) / 2)
		return BTN_RIGHT;
	else if((Frogs[i].State & FACING) && Frogs[i].x > SCREENLEFT +(SCREENRIGHT - SCREENLEFT) / 2 && Frogs[i].x > MIDDLERIGHT +(RIGHTEDGE - MIDDLERIGHT) / 2)
		return BTN_LEFT;
	return 0;
}


/*
uint16_t AIFaceCenter(uint8_t i){
	if(!(Frogs[i].State & FACING) && Frogs[i].x < SCREENLEFT + (SCREENRIGHT - SCREENLEFT) / 2)
		return BTN_RIGHT;
	else if((Frogs[i].State & FACING) && Frogs[i].x > SCREENLEFT + (SCREENRIGHT - SCREENLEFT) / 2)
		return BTN_LEFT;
	return 0;
}*/
/*
void DrawBestGeneration(){
	DDRC = 255;
	HideSprites();
	WaitVsync(1);
	for(uint16_t i=0;i<VRAM_SIZE;i++)
		vram[i] = RAM_TILES_COUNT+72;
	for(uint8_t i=0;i<NUMAIVARS;i++){//draw the current best
		DrawNumber(bestaivars[i],4+((i>19)*10),i%20,1);
	}
	DrawNumber(generation,20,22,0);
	DrawNumber(generation>>8,15,22,0);
	DrawNumber(bestscore,20,23,0);//initial best score was 106

	uint16_t currentscore = (uint16_t)(Frogs[0].Score+Frogs[1].Score);
//currentscore = bestscore+1;
	if(currentscore >= ((bestscore/2)+(bestscore/4))){//this is as good as anything, blend together
		for(uint8_t i=0;i<NUMAIVARS;i++){
			uint16_t temp = (uint16_t)(bestaivars[i]+aivars[i]);
			(uint16_t)(temp /= 2UL);//average it all
			bestaivars[i] = temp;
		}
		if(currentscore > bestscore)
			bestscore = currentscore;
	}
	
	for(uint8_t i=0;i<NUMAIVARS;i++)//setup next generation either to old good genes or equal or better new genes just calculated
		aivars[i] = bestaivars[i];

	WaitVsync(255);

	//modify a random gene, there are dummy vars to allow better prng() sequences to shine somewhat..maybe
	if(prng() < 128){
		uint8_t r = prng()%NUMAIVARS;
		aivars[r] += prng()%2;
		aivars[r] -= prng()%2; 

	}else if(prng() > 207)
		aivars[prng()%NUMAIVARS] = prng();
	else{
		uint8_t r = prng()%NUMAIVARS;
		uint8_t r2 = prng()%NUMAIVARS;
		uint8_t t = aivars[r];
		aivars[r] = aivars[r2];
		aivars[r2] = t;
	}
}
*/
