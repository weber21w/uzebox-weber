#ifndef __UZEBOX_H_
#define __UZEBOX_H_

	#include <stdbool.h>
	#include "defines.h"
	#include "kernel.h"
	
	//Include functions & var specific to the video mode used
	#ifdef VMODE_C_PROTOTYPES
		#include VMODE_C_PROTOTYPES
	#endif

	/*
	 * Video Engine structures & functions
	 */
	extern void FadeIn(unsigned char speed,bool blocking);
	extern void FadeOut(unsigned char speed,bool blocking);

	extern void SetSpritesOptions(unsigned char params);
	extern void SetSpritesTileTable(const char *data);

	extern void ClearVram(void);
	extern void SetTile(char x,char y, unsigned int tileId);
	extern void SetFontTilesIndex(unsigned char index);
	extern void SetFontTable(const char *data);
	extern void SetTileTable(const char *data);
	extern void DrawMap2(unsigned char x,unsigned char y,const char *map); //draw a map in video mode 2


	extern void WaitVsync(int count);
	extern void ClearVsyncFlag(void);
	extern unsigned char GetVsyncFlag(void);
	



	extern unsigned int ReadJoypad(unsigned char joypadNo);
	void ReadControllers(); //use only if CONTROLLERS_READ_MASTER=1






	/*
	 * Sound Engine functions
	 */	
	extern void SetMasterVolume(unsigned char vol);		//global player volume
	extern void TriggerNote(unsigned char channel,unsigned char patch,unsigned char note,unsigned char volume);
	extern void TriggerFx(unsigned char patch,unsigned char volume, bool retrig); //uses a simple voice stealing algorithm
	extern void StopSong();
	extern void StartSong(const char *midiSong);
	extern void ResumeSong();
	extern void InitMusicPlayer(const struct PatchStruct *patchPointersParam);

	/*
	 * Misc functions
	 */
	extern void WaitUs(unsigned int microseconds);
	extern void SoftReset(void);

	extern void SetUserPreVsyncCallback(VsyncCallBackFunc);
	extern void SetUserPostVsyncCallback(VsyncCallBackFunc);

#endif
