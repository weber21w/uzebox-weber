#define RTL_BIND_PREVSYNC 1
#define RTL_PREVSYNC_SPRITE_BLIT 1

#define RTL_SPRITE_LAYER 2//why do higher number corrupt tileindex? something in blit sprite???
//#define RTL_SPRITE_FLIPH 2
//#define RTL_SPRITE_ROT90 4
//#define RTL_SPRITE_IGNORE_BG 8

#ifndef RTL_MAX_SPRITES
#define RTL_MAX_SPRITES 32
#endif

#ifndef RTL_SPRITE_BPP
#define RTL_SPRITE_BPP 8
#endif

/*typedef struct{
	uint8_t x,y;
	uint8_t index,flags;
}rtl_Sprite;

rtl_Sprite rtl_sprites[RTL_MAX_SPRITES];
*/

	extern void CopyTileToRam(unsigned char romTile,unsigned char ramTile);
	extern void BlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy);
extern const char GameSprites[];
	extern const char pallet[] PROGMEM;

extern struct SpriteStruct sprites[];
void CompressedBlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy){

          u8 *src=&GameSprites+((sprites[spriteNo].tileIndex/2)*(8*TILE_WIDTH));
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

          if(!(sprites[spriteNo].tileIndex & 1)){

          if(flags&SPRITE_FLIP_X){
             for(y2=ydiff;y2<TILE_HEIGHT;y2++){
                for(x2=xdiffDest;x2<TILE_WIDTH;x2++){
                   px = pgm_read_byte(src--);
				   px = (px & 0b00001111);

				   if(px)
                      *dest=pgm_read_byte(&pallet[px+palletoff]);
                   

                   dest++;
                }
                src+=xdiffSrc;
                dest+=xdiffDest;
             }
		  }
		  else{
             for(y2=ydiff;y2<TILE_HEIGHT;y2++){
                for(x2=xdiffDest;x2<TILE_WIDTH;x2++){
                   px = pgm_read_byte(src++);
                   px = (px & 0b00001111);

                   if(px)
                      *dest=pgm_read_byte(&pallet[px+palletoff]);
                   
                   dest++;
                }
                src+=xdiffSrc;
                dest+=xdiffDest;
             }
		  }

		  }
		  else{
		  //sprites[spriteNo].tileIndex & 1
          if(flags&SPRITE_FLIP_X){
             for(y2=ydiff;y2<TILE_HEIGHT;y2++){
                for(x2=xdiffDest;x2<TILE_WIDTH;x2++){
                   px = pgm_read_byte(src--);
				   px = ((px & 0b11110000)>>4);

				   if(px)
                      *dest=pgm_read_byte(&pallet[px+palletoff]);
                   
                   dest++;
                }
                src+=xdiffSrc;
                dest+=xdiffDest;
             }
		  }
		  else{
             for(y2=ydiff;y2<TILE_HEIGHT;y2++){
                for(x2=xdiffDest;x2<TILE_WIDTH;x2++){
                   px = pgm_read_byte(src++);
                   px = ((px & 0b11110000)>>4);

                   if(px)
                      *dest=pgm_read_byte(&pallet[px+palletoff]);
                   
                   dest++;
                }
                src+=xdiffSrc;
                dest+=xdiffDest;
             }
		  }
		  
		  
		  }
}

void PalletBlitSprite(unsigned char spriteNo,unsigned char ramTileNo,unsigned int xy,unsigned int dxdy){
//sprites[spriteNo].tileIndex = 0;
          u8 *src=&GameSprites+(sprites[spriteNo].tileIndex*64);
          u8 *dest=ram_tiles+(ramTileNo*64);

          u8 flags=sprites[spriteNo].flags;
          u8 y=xy>>8,x=xy&0xff,dy=dxdy>>8,dx=dxdy&0xff;
          u8 xdiffSrc,xdiffDest,ydiff,x2,y2,px;
		  u8 palletoff = 0;//(flags & 0b11111100)>>2;

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



void rtl_ProcessSprites(bool layer){
	unsigned char i,bx,by,dx,dy,bt,x,y,tx=1,ty=1,wx,wy;
	unsigned int ramPtr,ssx,ssy;



if(!layer)
	free_tile_index=0;	



	for(i=0;i<MAX_SPRITES;i++){
		if(layer && !(sprites[i].flags & RTL_SPRITE_LAYER))//not to be drawn on this layer
			continue;

		bx=sprites[i].x;

		if(bx == (SCREEN_TILES_H*TILE_WIDTH))
			continue;
			//get tile's screen section offsets
				
		#if SCROLLING == 1
			ssx=sprites[i].x+Screen.scrollX;
			ssy=sprites[i].y+Screen.scrollY;
		#else
			ssx=sprites[i].x;
			ssy=sprites[i].y;
		#endif

		tx=1;
		ty=1;

		//get the BG tiles that are overlapped by the sprite
		bx=ssx>>3;
		dx=ssx&0x7;
		if(dx>0) tx++;

		//by=ssy>>3;
		//dy=ssy&0x7;
		by=ssy/TILE_HEIGHT;
		dy=ssy%TILE_HEIGHT;
		if(dy>0) ty++;			

		for(y=0;y<ty;y++){
			for(x=0;x<tx;x++){
				wy=by+y;
				wx=bx+x;

				if(wy>=(VRAM_TILES_V*2))
					wy-=(VRAM_TILES_V*2);
				else if(wy>=VRAM_TILES_V)
					wy-=VRAM_TILES_V;
					
				if(wx>=VRAM_TILES_H)wx-=VRAM_TILES_H; //should always be 32

				ramPtr=(wy*VRAM_TILES_H)+wx;
				bt=vram[ramPtr];						

				if( (bt>=RAM_TILES_COUNT)  && (free_tile_index < RAM_TILES_COUNT) ){
					//tile is mapped to flash. Copy it to next free RAM tile.
					//if no ram free ignore tile
					ram_tiles_restore[free_tile_index].addr=ramPtr;
					ram_tiles_restore[free_tile_index].tileIndex=bt;						
					CopyTileToRam(bt,free_tile_index);
					vram[ramPtr]=free_tile_index;
					bt=free_tile_index;
					free_tile_index++;										
				}

				if(bt<RAM_TILES_COUNT){		
#if RTL_SPRITE_BPP == 4
					PalletBlitSprite(i,bt,(y<<8)+x,(dy<<8)+dx);
#else	
			//		BlitSprite(i,bt,(y<<8)+x,(dy<<8)+dx);
#endif
				}
			}//end for X
		}//end for Y
	}
		//restore BG tiles
		if(layer)
			RestoreBackground();
}
