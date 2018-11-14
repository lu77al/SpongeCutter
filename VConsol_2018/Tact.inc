InitTact:
	outi	TCCR2,0b010	; CK/8
	ret

.macro	tskTact		;----- “акт 3906.25 Hz -----
	in	r16,TIFR
	sbrc	r16,TOV2
	rjmp	Tact
.endm

Tact:	outi	TIFR,1<<TOV2

	PUSH_LCD	

;**** RXTimer ****
	ldd	r16,Y+yRXTimer
	inc	r16
	breq	rxtm_1
	std	Y+yRXTimer,r16
	cpi	r16,UARTByteDelay
	brne	rxtm_1
	stdi	yRXPointer,RXBuf-1
	std	Y+yHeaterRepCnt,ZeroReg	
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
	cpi	r23,0b00111111
	breq	st_BR1
	rjmp	sk_BR1
st_BR1:

	sbrs	AFlags,afCutting
	rjmp	ssn_1
	ldd	r16,Y+yInitCutCnt
	cpi	r16,28
	brlo	ssn_1
	cpi	r16,32
	brsh	ssn_2
	ldi	r16,140
	rjmp	ssn_3
ssn_2:	andi	r24,0b1111
	breq	ssn_4
	cpi	r24,0b0011
	brne	ssn_1
ssn_4:	ldi	r16,6
ssn_3:	std	Y+ySoundCnt,r16
ssn_1:

	ldd	r16,Y+yFinishSoundCnt
	bst	r16,7
	andi	r16,$7F
	subi	r16,1
	brcs	fs_1
	bld	r16,7
	std	Y+yFinishSoundCnt,r16
	andi	r16,0b1111
	brtc	fs_2
	andi	r16,0b111
fs_2:	brne	fs_1
	stdi	ySoundCnt,30
fs_1:

	sbrs	AFlags,afCutting
	rjmp	tim_1
	andi	r24,0b11
	brne	tim_1
	lds	r16,MinCnt
	dec	r16
	brne	tim_2
	lds	r16,Minutes
	inc	r16
	cpi	r16,60
	brlo	tim_3
	lds	r16,HoursL
	lds	r17,HoursH
	subi_w	r17,r16,-1
	brcc	tim_4
	sts	HoursL,r16
	sts	HoursH,r17
tim_4:	ldi	r16,0
tim_3:	sts	Minutes,r16
	ldi	r16,229
tim_2:	sts	MinCnt,r16
tim_1:

	ldd	r16,Y+yPassCnt
	subi	r16,$10
	brcc	psw_1
	ldi	r16,0
psw_1:	std	Y+yPassCnt,r16

	ldd	r16,Y+ySearchCnt
	subi	r16,1
	brcc	scnt_1
	sbrs	BFlags,bfSearchStart
	rjmp	scnt_2
	ldd	r16,Y+yStepperRep
	ldd	r17,Y+yNextCommand
	or	r16,r17
	brne	scnt_2
	clt
	bld	BFlags,bfSearchStart
	rjmp	scnt_2
scnt_1:	std	Y+ySearchCnt,r16
scnt_2:

	sbrs	AFlags,afHeating
	rjmp	sth_1
	ldd	r16,Y+yCurScreen
	cpi	r16,0
	breq	sth_2
	cpi	r16,6
	brne	sth_3
sth_2:	stdi	yHeatingCnt,6
	rjmp	sth_1
sth_3:	ldd	r16,Y+yHeatingCnt
	subi	r16,1
	brcc	sth_4
	ldi	r16,0
	clt
	bld	AFlags,afHeating
sth_4:	std	Y+yHeatingCnt,r16
sth_1:

	rjmp	RetTact
sk_BR1:
;------ ‘ормирование запросов на силовой модуль 120.2 √ц ---
	andi	r23,0b00011111
	cpi	r23,0b00000011
	brne	sk_Cutter

	ldd	r16,Y+yCutReqCnt	; 20 √ц
	subi	r16,1
	brcc	ctr_1
	set
	bld	BFlags,bfReqCutter
	ldi	r16,5
;	sbrs	AFlags,afCutting
;	ldi	r16,30
ctr_1:	std	Y+yCutReqCnt,r16

	rjmp	RetTact
sk_Cutter:


;------ Indication 120.2 √ц ---
	andi	r23,0b00011111
	cpi	r23,0b00000001
	brne	sk_Ind

	rcall	Indicate

	rjmp	RetTact
sk_Ind:


;------ Buttons 120.2 √ц ---
	andi	r23,0b00011111
	cpi	r23,0b00000010
	brne	sk_But

	rcall	Buttons

	rjmp	RetTact
sk_But:

	rjmp	RetTact
