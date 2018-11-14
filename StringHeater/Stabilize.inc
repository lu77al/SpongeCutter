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

; Close key (set zero PWM) if DestV = 0
	ldd	r16,Y+yDestUL
	ldd	r17,Y+yDestUH
	or	r16,r17
	brne	st_1
	std	Y+yPWML,ZeroReg
	std	Y+yPWMH,ZeroReg
st_1:

; Delta U
	GetADC	adcU
	lsr16	r17,r16
	ldd	r18,Y+yDestUL
	ldd	r19,Y+yDestUH
	lsr16	r19,r18
	sub	r18,r16
	sbc	r19,r17		; Positive -> increase PWM
; Delta I
	GetADC	adcI
	lsr16	r17,r16
	ldd	r20,Y+yDestIL
	ldd	r21,Y+yDestIH
	lsr16	r21,r20
	sub	r16,r20
	sbc	r17,r21		; Positive -> increase PWM
; Choose minimal
	cp	r16,r18
	cpc	r17,r19
	brlt	st_2
	movw	r17:r16,r19:r18
st_2:
; If result is negative calculate module and set T
	bst	r17,7
	brtc	st_3
	com	r17
	com	r16
	subi_w	r17,r16,-1
st_3:
; If difference is too small assign 1
	cpi	r17,0
	brne	st_7
	cpi	r16,64
	brlo	st_7
	ldi	r17,1
st_7:
; Change PWM
	ldd	r24,Y+yPWML
	ldd	r25,Y+yPWMH
	cp	r24,ZeroReg
	cpc	r25,ZeroReg
	brne	st_6
	GetADC_	r19,r18,adcI
	std	Y+yIzeroL,r18
	std	Y+yIzeroH,r19
st_6:	brts	st_4
	add	r24,r17
	adc	r25,ZeroReg
	cpi_w	r25,r24,400,r17
	brlo	st_5
	ldi_w	r25,r24,400
	rjmp	st_5
st_4:
	sub	r24,r17
	sbc	r25,ZeroReg
	brcc	st_5
	ldi_w	r25,r24,0
st_5:	std	Y+yPWML,r24
	std	Y+yPWMH,r25
; Update timer settings
	ldd	r16,Y+yPWML
	ldd	r17,Y+yPWMH
	ldi_w	r19,r18,400
	sub	r18,r16
	sbc	r19,r17
	out	OCR1AH,r19
	out	OCR1AL,r18

	ret

