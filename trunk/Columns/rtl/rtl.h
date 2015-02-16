#ifndef RTL_INCLUDED
#define RTL_INCLUDED 1

extern unsigned char ram_tiles[];
extern struct BgRestoreStruct ram_tiles_restore[];
extern unsigned char free_tile_index;

#include "rtl.inc"

#include "rtl_defines.h"
#include "rtl_helpers.h"
#include "rtl_colorchange.h"
#include "rtl_foreground.h"
#include "rtl_text.h"
#include "rtl_sprites.h"

void rtl_PreVsync(){
//#ifdef RTL_PREVSYNC_SPRITE_BLIT
//	rtl_ProcessSprites(0);
//#endif
}

void rtl_PostVsync(){
	rtl_ProcessSprites(0);
#ifdef RTL_FOREGROUND
	rtl_ForegroundBlit();
#endif

//#ifdef RTL_POSTVSYNC_SPRITE_BLIT
	rtl_ProcessSprites(1);
//#endif
}


void rtl_Init(){
//#ifdef RTL_BIND_PREVSYNC
	SetUserPreVsyncCallback(&rtl_PreVsync);
//#endif
//#ifdef RTL_BIND_POSTVSYNC
	SetUserPostVsyncCallback(&rtl_PostVsync);
//#endif
}

#endif
