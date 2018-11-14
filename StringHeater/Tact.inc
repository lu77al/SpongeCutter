InitTact:
	outi	TCCR2,0b010	; CK/8
	ret

.macro	tskTact		;----- าเ๊๒ 3906.25 Hz -----
	in	r16,TIFR
	sbrc	r16,TOV2
	rjmp	Tact
.endm

Tact:	outi	TIFR,1<<TOV2

;**** RXTimer ****
	ldd	r16,Y+yRXTimer
	inc	r16
	breq	rxtm_1
	std	Y+yRXTimer,r16
	cpi	r16,UARTByteDelay
	brne	rxtm_1
	stdi	yRXPointer,RXBuf-1
rxtm_1:
;**** RXTimer ****


;-- MainCnt 24bit
	ldd	r23,Y+yMainCntL
	ldd	r24,Y+yMainCntM
	ldd	r25,Y+yMainCntH
	subi_w	r24,r23,-1
	sbci	r25,$FF
	std	Y+yMainCntL,r23
	std	Y+yMainCntM,r24
	std	Y+yMainCntH,r25





;--- Br1 ---
	cpi	r23,0b00111111	; 15.25
	breq	st_BR1
	rjmp	sk_BR1
st_BR1:
	StartADC

	ldd	r16,Y+yNoRequestCnt
	inc	r16
	cpi	r16,20
	brlo	br1_1
	std	Y+yHeating,ZeroReg
	ldi	r16,20
br1_1:	std	Y+yNoRequestCnt,r16

//	rcall	CalcVoltage


/*
	ldd	r18,Y+yPWMCnt
	ldi_w	r17,r16,0
	subi	r18,1
	brcs	sp_1
	std	Y+yPWMCnt,r18
	ldd	r16,Y+yPWML
	ldd	r17,Y+yPWMH
sp_1:	sbrs	r17,7
	cbi	FAN
	sbrc	r17,7
	sbi	FAN
	andi	r17,0x1
	ldi_w	r19,r18,400
	sub	r18,r16
	sbc	r19,r17
	out	OCR1AH,r19
	out	OCR1AL,r18
*/
	rjmp	RetTact
sk_BR1:
;------ 15.25 ร๖ ---
	cpi	r23,0b00000011
	brne	sk_BR2

	ldd	r16,Y+yHeating
	cpi	r16,0
	breq	ind_1
	sbrc	r24,2
	sbi	LED
	sbrs	r24,2
	cbi	LED
	rjmp	ind_2
ind_1:
	andi	r24,0b1111
	ldi	r17,4
	ldd	r16,Y+yNoRequestCnt
	cpi	r16,20
	brlo	ind_3
	ldi	r17,12
ind_3:	cp	r24,r17
	brlo	ind_4
	cbi	LED
	rjmp	ind_2
ind_4:	sbi	LED
ind_2:



	rjmp	RetTact
sk_BR2:


;------ Indication 120.2 ร๖ ---
	andi	r23,0b00011111
	cpi	r23,0b00000001
	brne	sk_Ind


	rjmp	RetTact
sk_Ind:


;------ Buttons 120.2 ร๖ ---
	andi	r23,0b00011111
	cpi	r23,0b00000010
	brne	sk_But

	rjmp	RetTact
sk_But:

	rjmp	RetTact
