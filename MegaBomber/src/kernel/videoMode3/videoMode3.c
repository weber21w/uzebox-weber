	#include <stdbool.h>
	#include <avr/io.h>
	#include <stdlib.h>
	#include <avr/pgmspace.h>
	#include <avr/interrupt.h>

    #include "../data/graphics/map1/tileoffsets.inc"
	#include "uzebox.h"
	

	extern unsigned char overlay_vram[];
	extern unsigned char ram_tiles[];
	extern struct SpriteStruct sprites[];
	extern unsigned char *sprites_tiletable_lo;
	extern unsigned char *tile_table_lo;
	extern struct BgRestoreStruct ram_tiles_restore[];
	extern unsigned char pallet[] PROGMEM;


	extern void CopyTileToRam(unsigned char romTile,unsigned char ramTile);
	extern void BlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy);
    extern const char MapTiles[];
	extern void MaskRamTile(unsigned char r, unsigned char t);
	extern unsigned char ram_tiles_reserved;
	extern unsigned char tick_tracker;

	unsigned char free_tile_index;

	void RestoreBackground(){
		unsigned char i,j;
		unsigned int a;
		for(i=0;i<free_tile_index;i++){			
			a=ram_tiles_restore[i].addr;
			j=ram_tiles_restore[i].tileIndex;
			vram[a]=j;
			//vram[ram_tiles_restore[i].addr]=ram_tiles_restore[i].tileIndex;
		}	
	}

extern unsigned char GameSprites;

void PalletBlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy){

          u8 *src=&GameSprites+(sprites[spriteNo].tileIndex*(8*TILE_WIDTH));
          u8 *dest=ram_tiles+(ramTileNo*TILE_HEIGHT*TILE_WIDTH);

          u8 flags=sprites[spriteNo].flags;
          u8 y=xy>>8,x=xy&0xff,dy=dxdy>>8,dx=dxdy&0xff;
          u8 xdiffSrc,xdiffDest,ydiff,x2,y2,px;
		  u8 palletoff = (flags & 0b11111100)>>2;

          if(x==0){
             dest+=dx;
             xdiffDest=dx;
             xdiffSrc=dx;
             if(flags&SPRITE_FLIP_X){
                src+=(TILE_WIDTH-1);
                xdiffSrc=16-dx;
             }
          }else{
             xdiffDest=(TILE_WIDTH-dx);

             if((flags&SPRITE_FLIP_X)==0){
                xdiffSrc=xdiffDest;
                src+=xdiffDest;
             }else{
                xdiffSrc=TILE_WIDTH+dx;
                src+=dx;
                src-=1;
             }
          }

          if(y==0){
             dest+=(dy*TILE_WIDTH);
             ydiff=dy;
          }else{
             src+=((8-dy)*TILE_WIDTH);
             ydiff=(8-dy);
          }

          for(y2=ydiff;y2<TILE_HEIGHT;y2++){
             for(x2=xdiffDest;x2<TILE_WIDTH;x2++){

                if(flags&SPRITE_FLIP_X){

                   px=pgm_read_byte(src--);
                   if(px){//px!=TRANSLUCENT_COLOR){
                      *dest=pgm_read_byte(&pallet[px+palletoff]);
                   }
                   dest++;

                }else{
                   px=pgm_read_byte(src++);
                   if(px){//(px!=TRANSLUCENT_COLOR){
                      *dest=pgm_read_byte(&pallet[px+palletoff]);//*dest=px;
                   }
                   dest++;
                }
             }
             src+=xdiffSrc;
             dest+=xdiffDest;
          }
       }


void SlowBlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy){

          u8 *src=&GameSprites+(sprites[spriteNo].tileIndex*(8*TILE_WIDTH));
          u8 *dest=ram_tiles+(ramTileNo*TILE_HEIGHT*TILE_WIDTH);

          u8 flags=sprites[spriteNo].flags;
          u8 y=xy>>8,x=xy&0xff,dy=dxdy>>8,dx=dxdy&0xff;
          u8 xdiffSrc,xdiffDest,ydiff,x2,y2,px;

          if(x==0){
             dest+=dx;
             xdiffDest=dx;
             xdiffSrc=dx;
             if(flags&SPRITE_FLIP_X){
                src+=(TILE_WIDTH-1);
                xdiffSrc=16-dx;
             }
          }else{
             xdiffDest=(TILE_WIDTH-dx);

             if((flags&SPRITE_FLIP_X)==0){
                xdiffSrc=xdiffDest;
                src+=xdiffDest;
             }else{
                xdiffSrc=TILE_WIDTH+dx;
                src+=dx;
                src-=1;
             }
          }

          if(y==0){
             dest+=(dy*TILE_WIDTH);
             ydiff=dy;
          }else{
             src+=((8-dy)*TILE_WIDTH);
             ydiff=(8-dy);
          }

          for(y2=ydiff;y2<TILE_HEIGHT;y2++){
             for(x2=xdiffDest;x2<TILE_WIDTH;x2++){

                if(flags&SPRITE_FLIP_X){

                   px=pgm_read_byte(src--);
                   if(px!=TRANSLUCENT_COLOR){
                      *dest=px;
                   }
                   dest++;

                }else{
                   px=pgm_read_byte(src++);
                   if(px!=TRANSLUCENT_COLOR){
                      *dest=px;
                   }
                   dest++;
                }
             }
             src+=xdiffSrc;
             dest+=xdiffDest;
          }
       }




	void ProcessSprites(){
	
		unsigned char i,bx,by,dx,dy,bt,x,y,tx=1,ty=1,wx,wy;
		unsigned int ramPtr,ssx,ssy;
		unsigned char tile=0;

		free_tile_index=ram_tiles_reserved;	
	
		for(i=0;i<MAX_SPRITES;i++){
			bx=sprites[i].x;

			if(bx<(SCREEN_TILES_H*TILE_WIDTH) && sprites[i].y < SCREEN_TILES_V*TILE_HEIGHT){
				//get tile's screen section offsets
					ssx=sprites[i].x;
					ssy=sprites[i].y;

				tx=1;
				ty=1;

				//get the BG tiles that are overlapped by the sprite
				bx=ssx>>3;
				dx=ssx&0x7;
				if(dx>0) tx++;
		
				by=ssy/TILE_HEIGHT;
				dy=ssy%TILE_HEIGHT;
				if(dy>0) ty++;			

				for(y=0;y<ty;y++){

					for(x=0;x<tx;x++){
						wy=by+y;
						wx=bx+x;

						ramPtr=(wy*VRAM_TILES_H)+wx;
						bt=vram[ramPtr];
#if SPECIAL_FOREGROUND_START != 255	
						if(bt >= SPECIAL_OCCLUSION_START+RAM_TILES_COUNT && bt <= SPECIAL_OCCLUSION_END+RAM_TILES_COUNT)
						   continue; 					
#endif
						if( (bt>=RAM_TILES_COUNT)  && (free_tile_index < RAM_TILES_COUNT)) 
					{
                            //if(sprites[i].flags & 0b00000010)
							  // SetSpritesTileTable(GameSprites);
                            //else
						//	   SetSpritesTileTable(GameSprites);
							//tile is mapped to flash. Copy it to next free RAM tile.
							//if no ram free ignore tile
							ram_tiles_restore[free_tile_index].addr=ramPtr;
							ram_tiles_restore[free_tile_index].tileIndex=bt;
													
							CopyTileToRam(bt,free_tile_index);

							vram[ramPtr]=free_tile_index;
							tile = bt-RAM_TILES_COUNT;//added
							bt=free_tile_index++;
						}
				
						if(bt<RAM_TILES_COUNT){
	
	PalletBlitSprite(i,bt,(y<<8)+x,(dy<<8)+dx);
	if(tile >= SPECIAL_FOREGROUND_START && tile <= SPECIAL_FOREGROUND_END)
       MaskRamTile(bt,tile);
//	BlitSprite(i,bt,x,y,dx,dy);
						}


					}//end for X
				}//end for Y
	
			}//	if(bx<(SCREEN_TILES_H*TILE_WIDTH))		
		}

		//restore BG tiles
		RestoreBackground();

	}


	//Callback invoked by UzeboxCore.Initialize()
	void InitializeVideoMode(){

		//disable sprites
		for(int i=0;i<MAX_SPRITES;i++){
			sprites[i].x=(SCREEN_TILES_H*TILE_WIDTH);		
		}

	}

	//Callback invoked during hsync
	void VideoModeVsync(){
		if(!(tick_tracker & 1))//TICK_ENDED))
		   tick_tracker |= 2;//TICK_MISSED;
		ProcessFading();
		ProcessSprites();
	}

