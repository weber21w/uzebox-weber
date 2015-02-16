inline void CalculateMusicTempo(){
	if(++tickssincetempochange < 4)
	return;
	tickssincetempochange = 0;
	
	if((state[0] & (SCORING|APPLYGRAVITY|EXPLODING)) && (state[1] & (SCORING|APPLYGRAVITY|EXPLODING)))
	return;
	uint16_t mt = (wellfullness[0] > wellfullness[1]) ? wellfullness[0]:wellfullness[1];


	if(mt < 6*5)
	mt = 0;
	else{
		mt -= 6*5;
		mt *= 4;//5 points for every jewel above 5*6
	}
	
	if(mt > MAXTEMPO)
	mt = MAXTEMPO;
	
	if(musicspeed < mt)
	musicspeed++;
	else if(musicspeed > mt)
	musicspeed--;
	if(musicspeed > mt+10)
	musicspeed -= 10;//respond quickly to chain action

	SetSongTempo((uint32_t)((musicspeed*musicspeed*2)/32));
}




void TriggerPCM(uint8_t i,uint8_t speed, uint8_t vol){//TODO SCORING HIGHER PRIORITY THAN DROP
	uint8_t t = pgm_read_byte(&PCMPRIORITY[i-FIRSTPCMSOUND]);
	if(pcm_priority > t && pcm_timeleft)//a higher priority sound is playing
		return;

	pcm_priority = t;
	pcm_timeleft = pgm_read_byte(&PCMTIMES[i-FIRSTPCMSOUND]);

	if(speed == 255)
		speed = pgm_read_byte(&PCMSPEEDS[i-FIRSTPCMSOUND]);
	TriggerNote(4,i,speed,vol);
}

inline void DoSong(){
	if(musictrack != level){
		if(level == 0)
		StartSong(Song1);
		else
		StartSong(Song0);
		musictrack = level;
	}
}