/*
 *  Uzebox Kernel - Mode 3
 *  Copyright (C) 2009  Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

;***************************************************
; Video Mode 3: 28x28 (224x224 pixels) using 8x8 tiles
; with overlay & sprites X flipping.
;
; If the SCROLLING build parameter=0, the scrolling 
; code is removed and the screen resolution 
; increase to 30*28 tiles.
; 
;***************************************************	

.global vram
.global ram_tiles
.global ram_tiles_restore
.global sprites
.global overlay_vram
.global Screen
.global SetSpritesTileTable
.global CopyTileToRam
.global SetSpritesTileBank
.global SetTile
.global ClearVram
.global SetFontTilesIndex
.global SetTileTable
.global SetTile
.global BlitSprite
.global SetFont

;Screen Sections Struct offsets
#define scrollX				0
#define scrollY				1
#define sectionHeight		2
#define vramBaseAdressLo	3
#define vramBaseAdressHi	4
#define tileTableAdressLo	5
#define tileTableAdressHi	6
#define wrapLine			7
#define flags				8
#define scrollXcoarse		9
#define scrollXfine			10		
#define vramRenderAdressLo	11
#define vramRenderAdressHi	12
#define vramWrapAdressLo	13
#define vramWrapAdressHi	14

;Sprites Struct offsets
#define sprPosX  0
#define sprPosY  1
#define sprTileIndex_lo 2
#define sprTileIndex_hi 3
#define sprFlags 4

#define SPRITE_FLIP_X_BIT 0


.section .bss
	.align 5
	vram: 	  				.space VRAM_SIZE ;MUST be aligned to 32 bytes
	
	#if SCROLLING == 1
		overlay_vram:		.space VRAM_TILES_H*OVERLAY_LINES
	#endif

	.align 1
	ram_tiles:				.space RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH
	;ram_tiles_restore:  	.space RAM_TILES_COUNT*3 ;vram addr|Tile

	;sprites_tile_banks: 	.space 8
	;vram_linear_buf:		.space 30
	tile_table_lo:			.byte 1
	tile_table_hi:			.byte 1
	;font_tile_index:		.byte 1 

.section .text



	;***************************************************
	; Mode 3 with NO scrolling
	;***************************************************	
	sub_video_mode3:

		;wait cycles to align with next hsync
		ldi r16,6
		dec r16
		brne .-4		
		;rjmp .

		;HACKED TO ELIMINATE THIS UNEEDED RAM FOR COLUMNS
		;Set ramtiles indexes in VRAM 
		
		;ldi ZL,lo8(ram_tiles_restore);
		;ldi ZH,hi8(ram_tiles_restore);

		;ldi YL,lo8(vram)
		;ldi YH,hi8(vram)

		;lds r18,free_tile_index
		lpm
		lpm

		clr r16
	upd_loop:	
		ldd XL,Z+0
		ldd XH,Z+1
	
		add XL,YL
		adc XH,YH

		ld r17,X	;currbgtile
		rjmp .;std Z+2,r17
		

		cp r16,r18
		brsh noov
		mov r17,r16
	noov:
		rjmp .;st X,r17
	
		adiw ZL,3 ;sizeof(ram_tiles_restore)

		inc r16
		cpi r16,RAM_TILES_COUNT
		brlo upd_loop ;23



		ldi r16,63-RAM_TILES_COUNT ;222*7 
	wait_loop:
	
		ldi r17,6
		dec r17
		brne .-4

		dec r16
		brne wait_loop

	


		ldi YL,lo8(vram)
		ldi YH,hi8(vram)

		ldi r16,SCREEN_TILES_V*TILE_HEIGHT; total scanlines to draw (28*8)
		mov r10,r16
		clr r22
		ldi r23,TILE_WIDTH ;tile width in pixels





	next_text_line:	
		rcall hsync_pulse

		WAIT r19,245 - AUDIO_OUT_HSYNC_CYCLES + CENTER_ADJUSTMENT

		;***draw line***
		call render_tile_line

		ldi r19,7 + 4 - CENTER_ADJUSTMENT	
		dec r19			
		brne .-4
	
		;rjmp .

		dec r10
		breq text_frame_end
	
		inc r22
		lpm ;3 nop

		cpi r22,TILE_HEIGHT ;last char line? 1
		breq next_text_row 
	
		;wait to align with next_tile_row instructions (+1 cycle for the breq)
		lpm ;3 nop
		lpm ;3 nop
		lpm ;3 nop
		nop
		rjmp next_text_line	

	next_text_row:
		clr r22		;current char line			;1	

		clr r0
		ldi r19,VRAM_TILES_H
		add YL,r19
		adc YH,r0

		lpm
		nop

		rjmp next_text_line

	text_frame_end:

		ldi r19,5
		dec r19			
		brne .-4
		rjmp .

		rcall hsync_pulse ;145
	
		clr r1
;		call RestoreBackground

		;set vsync flag if beginning of next frame
		ldi ZL,1
		sts vsync_flag,ZL

		;clear any pending timer int
		ldi ZL,(1<<OCF1A)
		sts _SFR_MEM_ADDR(TIFR1),ZL



		clr r1


		ret



	;*************************************************
	; RENDER TILE LINE
	;
	; r22     = Y offset in tiles
	; r23 	  = tile width in bytes
	; Y       = VRAM adress to draw from (must not be modified)
	;
	; Can destroy: r0,r1,r2,r3,r4,r5,r6,r7,r13,r16,r17,r18,r19,r20,r21,Z
	; 
	; cycles  = 1495
	;*************************************************
	render_tile_line:

		;load first tile and determine if its a ROM or RAM tile

		movw XL,YL

		mul r22,r23

		nop

		lds r16,tile_table_lo 
		lds r17,tile_table_hi
		subi r16,lo8(RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH)
		sbci r17,hi8(RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH)

		add r16,r0
		adc r17,r1
		movw r2,r16			;rom tiles

		ldi r16,lo8(ram_tiles)
		ldi r17,hi8(ram_tiles)
		add r16,r0
		adc r17,r1
		movw r4,r16			;ram tiles

		ldi r19,TILE_HEIGHT*TILE_WIDTH
		ldi r17,SCREEN_TILES_H

	    ld r18,X+     	;load next tile # from VRAM
		cpi r18,RAM_TILES_COUNT
		in r6,_SFR_IO_ADDR(SREG)	;save the carry flag
		bst r6,SREG_C

		mul r18,r19 	;tile*width*height
		movw r20,r2		;rom tiles
		brtc .+2
		movw r20,r4		;ram tiles

	    add r0,r20    ;add title table address +row offset
	    adc r1,r21

		movw ZL,r0

		brts ramloop
	

	romloop:
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 1
	    ld r18,X+     ;load next tile # from VRAM


	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 2
		mul r18,r19 ;tile*width*height


	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 3
		cpi r18,RAM_TILES_COUNT		;is tile in RAM or ROM? (RAM tiles have indexes<RAM_TILES_COUNT)
		in r6,_SFR_IO_ADDR(SREG)	;save the carry flag


	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
		brsh .+2		;skip in next tile is in ROM	
		movw r20,r4 	;load RAM title table address +row offset	
   
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 5
	    bst r6,SREG_C	;store carry state in T flag for later branch
		add r0,r20		;add title table address +row offset lsb
    
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 6
		adc r1,r21		;add title table address +row offset msb
		dec r17			;decrement tiles to draw on line

   
	    lpm r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 7   
	    lpm r16,Z+

		breq end	
	    movw ZL,r0   	;copy next tile adress

	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
	    brtc romloop
	
		rjmp .

	ramloop:

	    ld r16,Z+
	    out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 1
	    ld r18,X+     ;load next tile # from VRAM

	    ld r16,Z+ 
		nop   
		out _SFR_IO_ADDR(DATA_PORT),r16 		;pixel 2
		mul r18,r19 ;tile*width*height


	    ld r16,Z+
		nop
		out _SFR_IO_ADDR(DATA_PORT),r16         ;pixel 3
		cpi r18,RAM_TILES_COUNT
		in r6,_SFR_IO_ADDR(SREG)	;save the carry flag
		bst r6,SREG_C
   

	    ld r16,Z+
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 4
		brts .+2 
		movw r20,r2 	;ROM title table address +row offset	
   
   
	    ld r16,Z+
	    add r0,r20    ;add title table address +row offset
		out _SFR_IO_ADDR(DATA_PORT),r16       ;pixel 5
	    adc r1,r21
		rjmp .
    
		ld r16,Z+		
		out _SFR_IO_ADDR(DATA_PORT),r16       ;pixel 6
		nop
		rjmp .  

	    ld r16,Z+	
		out _SFR_IO_ADDR(DATA_PORT),r16      ;pixel 7   
	    ld r16,Z+

	    dec r17
	    breq end
	
		movw ZL,r0
		out _SFR_IO_ADDR(DATA_PORT),r16        ;pixel 8   
	
	    brtc romloop
		rjmp ramloop
	
	end:
		out _SFR_IO_ADDR(DATA_PORT),r16  	;pixel 8
		clr r16	
		lpm	
		nop
		out _SFR_IO_ADDR(DATA_PORT),r16        

		;wait
		ldi r16,5
		dec r16
		brne .-4

	
		ret



;***********************************
; SET TILE 8bit mode
; C-callable
; r24=ROM tile index
; r22=RAM tile index
;************************************
CopyTileToRam:
/*
	src=tile_table_lo+((bt&0x7f)*64);
	dest=ram_tiles+(free_tile_index*TILE_HEIGHT*TILE_WIDTH);

	ram_tiles_restore[free_tile_index].addr=ramPtr;//(by*VRAM_TILES_H)+bx+x;
	ram_tiles_restore[free_tile_index].tileIndex=bt;

	for(j=0;j<64;j++){
		px=pgm_read_byte(src++);
		*dest++=px;
	}
*/

	ldi r18,TILE_HEIGHT*TILE_WIDTH

	;compute source adress
	lds ZL,tile_table_lo
	lds ZH,tile_table_hi
	;andi r24,0x7f
	subi r24,RAM_TILES_COUNT
	mul r24,r18
	add ZL,r0
	adc ZH,r1

	;compute destination adress
	ldi XL,lo8(ram_tiles)
	ldi XH,hi8(ram_tiles)
	mul r22,r18
	add XL,r0
	adc XH,r1

	clr r0
	;copy data (fastest possible)
.rept TILE_HEIGHT*TILE_WIDTH
	lpm r0,Z+	
	st X+,r0
.endr


	clr r1
	ret







;***********************************
; CLEAR VRAM 8bit
; Fill the screen with the specified tile
; C-callable
;************************************
.section .text.ClearVram
ClearVram:
	//init vram		
	ldi r30,lo8(VRAM_SIZE)
	ldi r31,hi8(VRAM_SIZE)

	ldi XL,lo8(vram)
	ldi XH,hi8(vram)

	ldi r22,RAM_TILES_COUNT

fill_vram_loop:
	st X+,r22
	sbiw r30,1
	brne fill_vram_loop

	clr r1

	ret


;***********************************
; Define the tile data source
; C-callable
; r25:r24=pointer to tiles data
;************************************
.section .text.SetTileTable
SetTileTable:
	sts tile_table_lo,r24
	sts tile_table_hi,r25	
	ret
