Stabilize:
	clt
	bld	AFlags,afADCComplete

; Calculate destination voltage (ADC 8bit)
; 160 = 5V -> 255 = 7.969V
	ldd	r16,Y+yVoltage
	swap	r16
	andi	r16,$0F
	ldi	r17,10
	mul	r16,r17
	ldd	r17,Y+yVoltage
	andi	r17,$0F
	add	r17,r0
	ldi	r16,0
	lsl16	r17,r16
	MulKoef16 52885
	lsl16	r17,r16
	std	Y+yDestUL,r16
	std	Y+yDestUH,r17

; Calculate destination current (ADC 8bit)
; ADC 0xFFFF -> 64A
	ldd	r16,Y+yMaxCur
	swap	r16
	andi	r16,$0F
	ldi	r17,10
	mul	r16,r17
	ldd	r16,Y+yMaxCur
	add	r16,r0
	ldi	r17,Low(975/10)
	mul	r16,r17
	movw	r21:r20,r1:r0
	ldi	r17,High(975/10)
	mul	r16,r17
	add	r21,r0
	ldd	r16,Y+yIzeroL
	ldd	r17,Y+yIzeroH
	sub	r16,r20
	sbc	r17,r21
	std	Y+yDestIL,r16
	std	Y+yDestIH,r17





Stabilize:

;.macro	GetADC
;	lds	r16,$100+ADCBuf+@0*2
;	lds	r17,$100+ADCBuf+@0*2+1
;.endm
	clt
	bld	AFlags,afADCComplete

; 160 = 5V -> 255 = 7.969V

	ldd	r16,Y+yVoltage
	swap	r16
	andi	r16,$0F
	ldi	r17,10
	mul	r16,r17
	ldd	r16,Y+yVoltage
	andi	r16,$0F
	add	r16,r0
	ldi	r17,0
	lsl16	r17,r16
	lsl16	r17,r16
	MulKoef16 52885
	cpi	r17,0
	breq	st_10
	ldi	r16,$ff
st_10:
	mov	r25,r16		
	cpi	r25,0
	brne	st_11
	ldi_w	r25,r24,0
	std	Y+yPWML,r24
	std	Y+yPWMH,r25
st_11:
	ldd	r16,Y+yMaxCur
	swap	r16
	andi	r16,$0F
	ldi	r17,10
	mul	r16,r17
	ldd	r16,Y+yMaxCur
	add	r16,r0
	ldi	r17,Low(975/10)
	mul	r16,r17
	movw	r21:r20,r1:r0
	ldi	r17,High(975/10)
	mul	r16,r17
	add	r21,r0


	GetADC	adcI
	ldd	r18,Y+yPWML
	ldd	r19,Y+yPWMH
	or	r18,r19
	brne	st_12
	std	Y+yIzeroL,r16
	std	Y+yIzeroH,r17
st_12:
	ldd	r18,Y+yIzeroL
	ldd	r19,Y+yIzeroH
	sub	r18,r16
	sbc	r19,r17
	brcc	st_13
	ldi_w	r19,r18,0
st_13:
	cp	r18,r20
	cpc	r19,r21
	brlo	st_14
	ldd	r24,Y+yPWML
	ldd	r25,Y+yPWMH
	ldi	r16,1
	rjmp	st_4
st_14:


;	ldi_w	r25,r24,100
;	rjmp	st_5

	lds	r16,$100+ADCBuf+2*adcU+1
	sub	r16,r25
	brcc	st_1
	set
	neg	r16
st_1:	cpi	r16,1
	brlo	st_2
	lsr	r16
	brne	st_3
	ldi	r16,1
st_3:
	ldd	r24,Y+yPWML
	ldd	r25,Y+yPWMH
	brtc	st_4
	add	r24,r16
	adc	r25,ZeroReg
	cpi_w	r25,r24,400,r17
	brlo	st_5
	ldi_w	r25,r24,400
	rjmp	st_5
st_4:
	sub	r24,r16
	sbc	r25,ZeroReg
	brcc	st_5
	ldi_w	r25,r24,0
st_5:	std	Y+yPWML,r24
	std	Y+yPWMH,r25
st_2:
	ldd	r16,Y+yPWML
	ldd	r17,Y+yPWMH
	ldi_w	r19,r18,400
	sub	r18,r16
	sbc	r19,r17
	out	OCR1AH,r19
	out	OCR1AL,r18

	ret

<<<12:26:43.97<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.1<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.6<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.11<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.16<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.20<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.27<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.31<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.36<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.41<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.45<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.50<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.55<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.61<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.66<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.70<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.75<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.80<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.84<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:44.89<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
<<<12:26:44.95<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10- 
<<<12:26:45.0<<<14-02-4A-30-00-00-90-14-02-30-4A-80-00-10-00-FF- 
