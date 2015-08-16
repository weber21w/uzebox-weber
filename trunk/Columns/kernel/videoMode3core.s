.global vram
.global ram_tiles
.global Screen

.global SetTile

.global SetTileTable



.section .bss
	.align 32
	vram: 	  				.space VRAM_SIZE ;MUST be aligned to 32 bytes

	.align 1
	;sprites:				.space SPRITE_STRUCT_SIZE*MAX_SPRITES
	ram_tiles:				.space (RAM_TILES_COUNT*TILE_HEIGHT*TILE_WIDTH)
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

		nop;clr r16
	upd_loop:	
		rjmp .;ldd XL,Z+0
		rjmp .;ldd XH,Z+1
	
		nop;add XL,YL
		nop;adc XH,YH

		rjmp.//ld r17,X	;currbgtile
		rjmp .;std Z+2,r17
		

		nop;cp r16,r18
		
		rjmp .;brsh noov
		;mov r17,r16
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
; Define the tile data source
; C-callable
; r25:r24=pointer to tiles data
;************************************
.section .text.SetTileTable
SetTileTable:
	sts tile_table_lo,r24
	sts tile_table_hi,r25	
	ret


