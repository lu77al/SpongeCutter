Menu:
	ldd	r25,Y+yCurScreen
	ldd	r24,Y+yKeyCode
	ldd	r22,Y+yButStateL
	ldd	r23,Y+yButStateH
	ldd	r20,Y+yButRepL
	ldd	r21,Y+yButRepH

	ldd	r16,Y+yBakScreen	; ŒÊË‰‡ÌËÂ ÓÚÔÛÒÍ‡ÌËˇ ÍÌÓÔÓÍ ÔË ÔÂÂıÓ‰‡ı ˝Í‡ÌÓ‚
	cp	r16,r25
	breq	mnu_2
	mov	r16,r23
	andi	r16,0b11101111	; Œ˜ËÒÚÍ‡ ÍÌÓÔÍË "Í‡ÚÓ˜Í‡"
	or	r16,r22
	breq	mnu_2
	rjmp	mnu_1
mnu_2:	std	Y+yBakScreen,r25

;	|123-ƒ¬»√¿“b (#)-Ã≈Õﬁ| Screen0
;	|4 6 (*)-–≈«¿ >Õ¿    |
;	|789 (0)-À»—“>>—“¿–“ |
;	|(5)-Õ¿√–≈¬->¬€ Àﬁ◊≈Õ|
	cpi	r25,0
	breq	mn0_0
	rjmp	mn0_1
mn0_0:
	sbrs	BFlags,bfSearchStart
	rjmp	mn0_40
	cpi	r24,$FF
	breq	mn0_41
	stdi	yNextCommand,cmStopSmooth
	clt
	bld	BFlags,bfSearchStart
mn0_41:	rjmp	mnu_1
mn0_40:

	sbrs	AFlags,afHandMoving
	rjmp	mn0_23
	eor	r22,r23
	ldd	r16,Y+yKBRDHash
	cp	r16,r22
	breq	mn0_22
mn0_26:	stdi	yNextCommand,cmStopSmooth
	clt
	bld	AFlags,afHandMoving
	rjmp	mnu_1
mn0_23:
	mov	r18,r22
	andi	r18,0b11011110
	mov	r19,r23
	andi	r19,0b00000011
	or	r18,r19
	ldd	r16,Y+yStepperRep
	cpi	r16,0
	breq	mn0_25
	cpi	r18,0
	breq	mn0_26
	rjmp	mn0_20
mn0_25:
	cpi	r18,0
	breq	mn0_20
	ldd	r20,Y+yLastKeyCode
	cpi	r20,1
	brlo	mn0_20
	cpi	r20,5
	breq	mn0_20
	brlo	mn0_21
	dec	r20
	cpi	r20,9
	brsh	mn0_20
mn0_21:	ldd	r16,Y+yNextCommand
	cpi	r16,0
	brne	mn0_22
	std	Y+yNextCommand,r20
	set
	bld	AFlags,afHandMoving
	eor	r22,r23
	std	Y+yKBRDHash,r22
mn0_22:	rjmp	mnu_1
mn0_20:
	cpi	r24,10
	brne	mn0_30
	ldd	r16,Y+yStepperRep
	ldd	r17,Y+yNextCommand
	or	r16,r17
	brne	mn0_31
	stdi	yNextCommand,cmSearchStart
	set
	bld	BFlags,bfSearchStart
	stdi	ySearchCnt,15
mn0_31:	rjmp	mnu_1
mn0_30:

	cpi	r24,5
	brne	mn0_50
	ldi	r16,1<<afHeating
	eor	AFlags,r16
	rjmp	mnu_1
mn0_50:
	cpi	r24,11
	brne	mn0_2
	clt
	bld	AFlags,afHeating
	stdi	yNextCommand,cmStopSmooth
mn0_11:	stdi	yCurScreen,1
mn0_4:	rjmp	mnu_1
mn0_2:
	rjmp	mnu_1
mn0_1:



;	|(1)-–”◊ÕŒ≈ ”œ–.     | Screen1
;	|(2)-—“¿–“ –≈« » N00 |
;	|(3)-¬€¡Œ– –¿— –Œﬂ   |
;	|(4)-œ¿–¿Ã≈“–€       |
	cpi	r25,1
	brne	mn1_1

	cpi	r24,$FF
	breq	mn1_6
	ldi_w	ZH,ZL,Password*2
	ldd	r16,Y+yPassCnt
	andi	r16,$0F
	add	ZL,r16
	adc	ZH,YH
	lpm	r17,Z
	cp	r17,r24
	breq	mn1_7
	std	Y+yPassCnt,YH
	rjmp	mn1_8
mn1_7:	cpi	r16,9
	brlo	mn1_9
	ldi	r25,7
	rjmp	mn1_3
mn1_9:	inc	r16
	ori	r16,$F0
	std	Y+yPassCnt,r16
	rjmp	mnu_1

mn1_8:	cpi	r24,1
	brne	mn1_2
	ldi	r25,0
	rjmp	mn1_3
mn1_2:	cpi	r24,2
	brne	mn1_4
	rcall	StartCutting
	ldi	r25,6
	sbrc	AFlags,afCardError
	ldi	r25,8
	rjmp	mn1_3
mn1_4:	cpi	r24,3
	brne	mn1_5
	ldi	r23,0
	rcall	ReadCutting
	ldi	r25,3
	sbrc	AFlags,afCardError
	ldi	r25,8
	rjmp	mn1_3
mn1_5:	cpi	r24,4
	brne	mn1_10
	ldi	r25,2
mn1_3:	std	Y+yCurScreen,r25
	rjmp	mnu_1
mn1_10:	cpi	r24,5
	brne	mn1_6
	ldi	r25,9
	rjmp	mn1_3
mn1_6:	rjmp	mnu_1
mn1_1:

;	| œ¿–¿Ã≈“–€  (#)-Ã≈Õﬁ| Screen2
;	|<123>“Œ  —“–”Õ€ 023%|
;	|<456>— Œ–Œ—“b 23ÏÏ/Ò|
;	|<7>-ƒ¬»√¿“b À»—“    |
	cpi	r25,2
	brne	mn2_1

	cpi	r24,11
	brne	mn2_2
	ldd	r16,Y+ySpeed
	ldi_w	r17,r16,$FFF6
	ldi	r18,9
	ldd	r19,Y+ySpeed
	ldi_w	r21,r20,0
	rcall	Div24
	std	Y+ySpeedKoefL,r16
	std	Y+ySpeedKoefH,r17
	rcall	SaveMemory
	rjmp	mn0_11
mn2_2:
	ldd	r16,Y+yHeatRate
	cpi	r24,1
	brne	mn2_3
	subi	r16,1
	brcs	mn2_14
	cpi	r16,5
	brsh	mn2_5
mn2_14:	ldi	r16,0
	rjmp	mn2_4
mn2_3:	cpi	r24,3
	brne	mn2_4
	inc	r16
	cpi	r16,5
	brsh	mn2_5
	ldi	r16,5
mn2_5:	cpi	r16,100
	brlo	mn2_4
	ldi	r16,100
mn2_4:	cpi	r24,2
	brne	mn2_7
	ldi	r16,50
mn2_7:	std	Y+yHeatRate,r16

	ldd	r16,Y+ySpeed
	cpi	r24,4
	brne	mn2_8
	dec	r16
mn2_8:	cpi	r24,6
	brne	mn2_9
	inc	r16
mn2_9:	cpi	r24,5
	brne	mn2_10
	ldi	r16,60
mn2_10:	cpi	r16,10
	brsh	mn2_11
	ldi	r16,10
mn2_11:	cpi	r16,80
	brlo	mn2_12
	ldi	r16,80
mn2_12:	std	Y+ySpeed,r16

	cpi	r24,7
	brne	mn2_13
	ldi	r16,1<<afReverseY
	eor	AFlags,r16
mn2_13:

	rjmp	mnu_1
mn2_1:

;	|   ¬€¡Œ– –¿— –Œﬂ    | Screen3
;	|<1-3>-¬€¡Œ– (#)-Ã≈Õﬁ|
;	|                    |
;	|                    |
	cpi	r25,3
	brne	mn3_1

	cpi	r24,11
	brne	mn3_5
	rcall	SaveMemory
	rjmp	mn0_11
mn3_5:
	ldd	r16,Y+yCurCutNum
	cpi	r24,1
	brne	mn3_2
	ldi	r23,1
	subi	r16,1
	brcc	mn3_3
	ldi	r16,19
	rjmp	mn3_3
mn3_2:
	cpi	r24,3
	brne	mn3_4
	ldi	r23,0
	inc	r16
	cpi	r16,20
	brlo	mn3_3
	ldi	r16,0
mn3_3:	std	Y+yCurCutNum,r16
	rcall	ReadCutting
	ldi	r25,8
	sbrc	AFlags,afCardError
	rjmp	mn1_3
mn3_4:
	rjmp	mnu_1
mn3_1:

;	|                    | Screen6
;	|                    |
;	|  Œ¡–¿¡Œ“¿ÕŒ 000%   |
;	|Àﬁ¡¿ﬂ  ÕŒœ ¿-—“Œœ!!!|
	cpi	r25,6
	brne	mn6_1

	ldd	r16,Y+yHeatRate
	cpi	r24,10
	brne	mn6_5
	subi	r16,1
	brcc	mn6_6
	ldi	r16,0
mn6_6:	std	Y+yHeatRate,r16
	rjmp	mn6_7
mn6_5:	cpi	r24,11
	brne	mn6_8
	inc	r16
	cpi	r16,101
	brlo	mn6_6
	ldi	r16,100
	rjmp	mn6_6
mn6_8:
	cpi	r24,$FF
	ldi	r16,0
	brne	mn6_2
;	rjmp	mn6_3	; «‡·ÎÓÍËÓ‚‡Ì‡ ÒÚÛÌ‡
mn6_7:
	rjmp	mnu_1

;	ldd	r16,Y+yInitCutCnt
;	cpi	r16,90
;	brsh	mn6_3
;	sbrs	AFlags,afNoStringRep
;mn6_3:	rjmp	mnu_1
;	stdi	yFinishSoundCnt,$F1
;	lds	r16,StringsL
;	lds	r17,StringsH
;	subi_w	r17,r16,-1
;	brcc	mn6_4
;	sts	StringsL,r16
;	sts	StringsH,r17
;mn6_4:	ldi	r16,2

mn6_2:	std	Y+yErrorCode,r16
	stdi	yCurScreen,4
	stdi	yNextCommand,cmStopSmooth
	rcall	StopCutting
	rjmp	mnu_1
mn6_1:

;	|     —“¿“»—“» ¿     | Screen7
;	|   ¬–≈Ãﬂ      :     |
;	|  À»—“Œ¬            |
;	|   —“–”Õ            |
	cpi	r25,7
	brne	mn7_1

	cpi	r24,11
	brne	mn7_4
	rjmp	mn0_11
mn7_4:
	andi	r22,0b1110
	cpi	r22,0b1110
	brne	mn7_2
	ldi_w	ZH,ZL,MinCnt
mn7_3:	st	Z+,ZeroReg
	cpi	ZL,Sheets100H
	brlo	mn7_3
	rcall	SaveMemory
mn7_2:
	rjmp	mnu_1
mn7_1:

;	|  ‘Œ–Ã¿“»–Œ¬¿Õ»≈    | Screen9
;	|    ¿–“€ œ¿Ãﬂ“»     |
;	|     (#)-Ã≈Õﬁ       |
;	| (1,2,3,8,*)-‘Œ–Ã¿“ |
	cpi	r25,9
	brne	mn9_1

	cpi	r24,11
	brne	mn9_2
	rjmp	mn0_11
mn9_2:
	cpi	r24,$FF
	breq	mn9_3
	ldi_w	ZH,ZL,FormatPass*2
	ldd	r16,Y+yPassCnt
	andi	r16,$0F
	add	ZL,r16
	adc	ZH,YH
	lpm	r17,Z
	cp	r17,r24
	breq	mn9_4
	std	Y+yPassCnt,YH
	rjmp	mn9_3
mn9_4:	cpi	r16,4
	brlo	mn9_5
	rcall	FormatCard
	sbrs	AFlags,afCardError
	rjmp	mn0_11
	ldi	r25,8
	rjmp	mn1_3
mn9_5:	inc	r16
	ori	r16,$F0
	std	Y+yPassCnt,r16
	rjmp	mnu_1
mn9_3:
	rjmp	mnu_1
mn9_1:

	cpi	r24,$FF
	breq	mnu_1
	stdi	yCurScreen,1


mnu_1:	stdi	yKeyCode,$FF
	std	Y+yButPressL,YH	
	std	Y+yButPressH,YH	
	std	Y+yButRepL,YH	
	std	Y+yButRepH,YH	
	ret


Password:
.db	0,5,0,3,0,3,6,7,1,6

FormatPass:
.db	1,2,3,8,10,0
