inline void Engine(){
	if(Gui())
		return;
	Input();
	UpdateLevel();
	UpdateEnemies();
	UpdateLolo();
		
	Render();
	WaitVsync(1);
}
