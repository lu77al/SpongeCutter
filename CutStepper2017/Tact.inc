InitTact:
	outi	TCCR2,0b010	; CK/8
	ret

.macro	tskTact		;----- าเ๊๒ 7812.5 Hz -----
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
	cpi	r23,0b11100111	; 30.518 ร๖
	brne	sk_BR1

	ldd	r16,Y+yNoUartCnt
	subi	r16,1
	brcs	br1_4
	std	Y+yNoUartCnt,r16
	brne	br1_4
	ldi_w	r17,r16,$F000
	ldi	r18,$FF
	cli
	std	Y+yAXChangeL,r18
	std	Y+yAXChangeH,r18
	std	Y+yEndPeriodL,r16
	std	Y+yEndPeriodH,r17
	std	Y+yDataCnt,ZeroReg
	std	Y+yDataHead,ZeroReg
	std	Y+yDataTail,ZeroReg
	clt
	bld	AFlags,afMoveLong
	sei
br1_4:
	
	ldd	r16,Y+yDataCnt
	ldd	r17,Y+yMasterStCntL
	ldd	r18,Y+yMasterStCntH
	or	r16,r17
	or	r16,r18
	breq	br1_1
	sbrs	r24,2
	sbi	LED_PIN
	sbrc	r24,2
br1_3:	cbi	LED_PIN
	rjmp	br1_2
br1_1:	andi	r24,0b11111
	cpi	r24,1<<2
	brsh	br1_3
	sbi	LED_PIN
br1_2:
	rjmp	RetTact
sk_BR1:
;--- Br2 ---
	andi	r23,0b00000111
	cpi	r23,0b00000010	; 976.5625 ร๖
	brne	sk_BR2

	sbrs	AFlags,afStopOnHS
	rjmp	br2_1
	ldd	r16,Y+yDataCnt
	ldd	r17,Y+yMasterStCntL
	ldd	r18,Y+yMasterStCntH
	or	r18,r17
	or	r18,r16
	breq	br2_1
	sbis	PIND,4
	rjmp	br2_1
	cli
	std	Y+yMasterStCntL,ZeroReg
	std	Y+yMasterStCntH,ZeroReg
	std	Y+yDataCnt,ZeroReg
	std	Y+yDataHead,ZeroReg
	std	Y+yDataTail,ZeroReg
	clt
	bld	AFlags,afMoveLong
	sei
br2_1:
	rjmp	RetTact
sk_BR2:

	rjmp	RetTact
