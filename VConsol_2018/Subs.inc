;*****
; 16*16=16
;*****
Mul_16:
	push	r20
	push	r21
	push	r22
	mul	r16,r18
	mov	r20,r1
	ldi	r21,0
	ldi	r22,0
	mul	r16,r19
	add	r20,r0
	adc	r21,r1
	adc	r22,YH
	mul	r17,r18
	add	r20,r0
	adc	r21,r1
	adc	r22,YH
	mul	r17,r19
	add	r21,r0
	adc	r22,r1
	mov	r16,r21
	mov	r17,r22
	pop	r22
	pop	r21
	pop	r20
	ret

;***************************
; Divide Result = Val1/Val2
;***************************
Div:	clr	r14		;clear remainder Low byte
	sub	r15,r15		;clear remainder High byte and carry
	ldi	r20,17		;init loop counter
d16u_1:	rol	r16		;shift left dividend
	rol	r17
	dec	r20		;decrement counter
	brne	d16u_2		;if done
	ret			;    return
d16u_2:	rol	r14		;shift dividend into remainder
	rol	r15
	sub	r14,r18		;remainder = remainder - divisor
	sbc	r15,r19
	brcc	d16u_3		;if result negative
	add	r14,r18		;    restore remainder
	adc	r15,r19
	clc			;    clear carry to be shifted into result
	rjmp	d16u_1		;else
d16u_3:	sec			;    set carry to be shifted into result
	rjmp	d16u_1

;------ 24bit division -------

.def	remL=r13
.def	remM=r14
.def	remH=r15

.def	ddrL=r16
.def	ddrM=r17
.def	ddrH=r18

.def	dvL=r19
.def	dvM=r20
.def	dvH=r21

.def	divcnt=r22

;***** Code

Div24:	clr	remL
	clr	remM
	sub	remH,remH
	ldi	divcnt,25

div_3:	rol	ddrL
	rol	ddrM
	rol	ddrH
	dec	divcnt
	brne	div_1
	ret
div_1:	rol	remL
	rol	remM
	rol	remH
	sub	remL,dvL
	sbc	remM,dvM
	sbc	remH,dvH
	brcc	div_2
	add	remL,dvL
	adc	remM,dvM
	adc	remH,dvH
	clc
	rjmp	div_3
div_2:	sec
	rjmp	div_3

.undef	remL
.undef	remM
.undef	remH
.undef	ddrL
.undef	ddrM
.undef	ddrH
.undef	dvL
.undef	dvM
.undef	dvH
.undef	divcnt


;***** Subroutine Register Variables

.def	drem8u	=r15		;remainder
.def	ddres8u	=r16		;result
//.def	dd8u	=r16		;dividend
.def	dv8u	=r17		;divisor
.def	dcnt8u	=r18		;loop counter

;***** Code

div8:	sub	drem8u,drem8u	;clear remainder and carry
	ldi	dcnt8u,9	;init loop counter
d8u_1:	rol	ddres8u		;shift left dividend
	dec	dcnt8u		;decrement counter
	brne	d8u_2		;if done
	com	ddres8u
	ret			;    return
d8u_2:	rol	drem8u		;shift dividend into remainder
	sub	drem8u,dv8u	;remainder = remainder - divisor
	brcc	d8u_1		;if result negative
	add	drem8u,dv8u	;    restore remainder
	rjmp	d8u_1

.undef	drem8u
.undef	ddres8u
.undef	dv8u
.undef	dcnt8u


;******************
; Copy PROM -> SRAM
;******************
CopPS:	lpm	r0,Z+
	st	X+,r0
	loop	r16,CopPS
	ret

