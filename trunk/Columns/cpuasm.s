#include <avr/io.h>

.global CpuBufferWellAsm
.global CpuScoreAndGravityAsm
.global CpuEvaluateAsm
.global cpubuffer

.section .bss
.align 1
	cpubuffer:	.space (16*6)



.section .text
CpuBufferWellAsm:
mov ZL,r24;well pointer
mov ZH,r25
ldi XL,lo8(cpubuffer);cpu buffer
ldi XH,hi8(cpubuffer)
.rept 6*16
	ld r16,Z+
	st X+,r16
.endr
ret



CpuCheckLossAsm:
	ldi XL,lo8(cpubuffer);cpu buffer
	ldi XH,hi8(cpubuffer)
.rept 3*6
	ld r16,X+
	cpi r16,0
	brne CPULOSS
.endr
	clr r24
	ret
CPULOSS:
	ldi r24,1
	ret


