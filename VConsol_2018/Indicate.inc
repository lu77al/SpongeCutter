Indicate:
	ldd	r16,Y+yLCDCnt	 ; 120.2 √ˆ
	inc	r16
	andi	r16,0b11111
	std	Y+yLCDCnt,r16
	mov	r17,r16
	andi	r17,0b00111
	cpi	r17,0b00100
	brne	ind_0
	rjmp	Menu

ind_0:	cpi	r16,4
	brlo	ind_1
	ret
ind_1:

// Line ADDR CMD
	cbi	LCD_RS
	ldi_w	ZH,ZL,LineAddr*2
	add	ZL,r16
	adc	ZH,YH
	lpm	r16,Z
	rcall	LCD_PushByte
	sbi	LCD_RS


// Fill line from screen template (addr = 80*CurScr + 20*CurLine
	ldi	XL,LCDBuf
	std	Y+yLCDPointer,XL
	ldd	r16,Y+yLCDCnt
	ldi	r17,20
	mul	r16,r17
	movw	ZH:ZL,r1:r0
	ldd	r16,Y+yCurScreen
	ldi	r17,80
	mul	r16,r17
	add	ZL,r0
	adc	ZH,r1
	subi	ZL,Low(-ScreenCont*2)
	sbci	ZH,High(-ScreenCont*2)
	ldi	XL,LCDBuf
ind_2:	lpm	r16,Z+
	st	X+,r16
	cpi	XL,LCDBuf+20
	brlo	ind_2

// --- Preload R25=CurScreen / R24=CurLine / Exit if CurLine > 3 / Correct CurScreen
	ldd	r25,Y+yCurScreen
	ldd	r24,Y+yLCDCnt
	cpi	r25,10
	brlo	ind_10
	ldi	r25,0
	std	Y+yCurScreen,r25
ind_10:	cpi	r24,4
	brlo	ind_11
	ret
ind_11:

;	|123-ƒ¬»√¿“b (#)-Ã≈Õﬁ| Screen0
;	|4 6 (*)-–≈«¿ >Õ¿    |
;	|789 (0)-À»—“>>—“¿–“ |
;	|(5)-Õ¿√–≈¬->¬€ Àﬁ◊≈Õ|
;        0123456789ABCDEF0123
	cpi	r25,0
	brne	ind0_1
	cpi	r24,3
	brne	ind0_2	// Line 3 -> show string state
	ldi	r16,$C2
	sbrc	AFlags,afNoStringRep // String broken
	rjmp	ind0_3
	ldi	r16,$C0
	sbrc	AFlags,afHeating  // On / Off
	ldi	r16,$C1
ind0_3:	rjmp	ExtMes
ind0_2:	ret
ind0_1:

;	|(1)-–”◊ÕŒ≈ ”œ–.     | Screen1
;	|(2)-—“¿–“ –≈« » N00 |
;	|(3)-¬€¡Œ– –¿— –Œﬂ   |
;	|(4)-œ¿–¿Ã (5)-‘Œ–Ã¿“|
;        0123456789ABCDEF0123
	cpi	r25,1
	brne	ind1_0
	cpi	r24,1	// Line 1 ->  show current cut
	brne	ind1_2
	ldd	r16,Y+yCurCutNum
	ldi	XL,LCDBuf+17
	rjmp	Print2
ind1_2:	ret
ind1_0:

;	| œ¿–¿Ã≈“–€  (#)-Ã≈Õﬁ| Screen2
;	|<123>“Œ  —“–”Õ€ 023%|
;	|<456>— Œ–Œ—“b 23ÏÏ/Ò|
;	|<7>-ƒ¬»√¿“b À»—“    |
;        0123456789ABCDEF0123
	cpi	r25,2
	brne	ind2_0
	cpi	r24,1	// Line 1 -> show heat rate
	brne	ind2_1
	ldd	r16,Y+yHeatRate
	ldi	r17,0
	subi_w	r17,r16,-1000
	PrintAt	14,3
	ret
ind2_1:	cpi	r24,2  // Line 2 -> show speed
	brne	ind2_2
	ldd	r16,Y+ySpeed
	ldi	XL,LCDBuf+14
	rjmp	Print2
ind2_2:	cpi	r24,3  // Line 3 -> show object to move
	brne	ind2_3
	ldi	r16,$C3 // "À»—“"
	sbrc	AFlags,afReverseY
	ldi	r16,$C4 // "–≈«¿ "
	rjmp	ExtMes
ind2_3:
	ret
ind2_0:

;	|   ¬€¡Œ– –¿— –Œﬂ    | Screen3
;	|<1-3>-¬€¡Œ– (#)-Ã≈Õﬁ|
;	|                    |
;	|                    |
;        0123456789ABCDEF0123
	cpi	r25,3
	brne	ind3_1
	cpi	r24,2 // Line2 -> description (line 0)
	brne	ind3_2
	ldi	ZL,DescrBuf
ind3_5:	ldi	ZH,1
	ldi	XL,LCDBuf
ind3_3:	ld	r16,Z+
	st	X+,r16
	cpi	XL,LCDBuf+20
	brlo	ind3_3
	ret
ind3_2:	cpi	r24,3 // Line2 -> description (line 1)
	brne	ind3_4
	ldi	ZL,DescrBuf+20
	rjmp	ind3_5
ind3_4:	ret
ind3_1:

;	|   –≈« ¿ œ–≈–¬¿Õ¿   | Screen4
;	|                    |
;	|                    |
;	|                    |
;        0123456789ABCDEF0123
	cpi	r25,4
	brne	ind4_1
	cpi	r24,2	// Line 2 -> interrupt reason
	brne	ind4_2
	ldd	r16,Y+yErrorCode
	subi	r16,-$15
	rjmp	ExtMes
ind4_2:	ret
ind4_1:

;	|                    | Screen5
;	|   —»ÀŒ¬ŒÈ ÃŒƒ”Àb   |
;	|    Õ≈ Œ“¬≈◊¿≈“     |
;	|                    |
	cpi	r25,5
	brne	ind5_1
	cpi	r24,1	// Line 1 -> reason
	brne	ind5_2
	sbrc	AFlags,afNoStepperRep
	ret
	sbrs	AFlags,afNoStringRep
ind5_2:	ret
	ldi	r16,$39
	rjmp	ExtMes
ind5_1:

;	|                    | Screen6
;	|                    |
;	|  Œ¡–¿¡Œ“¿ÕŒ 000%   |
;	|Àﬁ¡¿ﬂ  ÕŒœ ¿-—“Œœ!!!|
;        0123456789ABCDEF0123
	cpi	r25,6
	breq	ind6_0
	rjmp	ind6_1
ind6_0:	cpi	r24,0	// Line 0 -> description (line 0)
	brne	ind6_2
/*
	ldi	XL,LCDBuf
	ldd	r16,Y+ySegInBuf
	rcall	Print2

	inc	XL
	ldi	r16,'r'
	sbrc	AFLags,afNoStepperRep
	ldi	r16,'R'
	st	X+,r16
	ret

*/
	ldi	ZL,DescrBuf
	rjmp	ind3_5
ind6_2:	cpi	r24,1	// Line 1 -> description (line 1)
	brne	ind6_3
	ldi	ZL,DescrBuf+20
	rjmp	ind3_5
ind6_3:	cpi	r24,2	// Line 2 -> progress
	brne	ind6_4
	ldd	r16,Y+yInitCutCnt // Initial countdown
	cpi	r16,0
	breq	ind6_8
	ldi	r16,$18
	rcall	ExtMes
	ldd	r16,Y+yInitCutCnt 
	swap	r16
	andi	r16,$0F
	inc	r16
	ldi	r17,$20
	st	X+,r17
	rjmp	Print2
ind6_8:	ldd	r16,Y+ySegCntB0	// 
	ldd	r17,Y+ySegCntB1
	ldd	r18,Y+ySegCntB2
	ldd	r19,Y+ySegCntB0
	ldd	r20,Y+ySegCntB1
	ldd	r21,Y+ySegCntB2
	ldd	r22,Y+ySegLastB0
	ldd	r23,Y+ySegLastB1
	ldd	r24,Y+ySegLastB2
	sub	r16,r22
	sbc	r17,r23
	sbc	r18,r24
	ldd	r22,Y+ySegInBuf
	sub	r16,r22
	sbc	r17,YH
	sbc	r18,YH
	brcc	ind6_5
	ldi	r16,0
	rjmp	ind6_6
ind6_5:	tst	r21
	breq	ind6_7
	lsr16	r21,r20
	ror	r19
	lsr16	r18,r17
	ror	r16
	rjmp	ind6_5
ind6_7:	ldi	r25,100
	mul	r25,r16
	movw	r23:r22,r1:r0
	ldi	r24,0
	mul	r25,r17
	add	r23,r0
	adc	r24,r1
	movw	r17:r16,r23:r22
	mov	r18,r24
	rcall	Div24
ind6_6:	std	Y+yProgress,r16
	ldi	r17,0
	subi_w	r17,r16,-1000
	PrintAt	13,3
	ret

ind6_4:

	cpi	r24,3	// Line 3 -> heating
	brne	ind6_9

	ldd	r16,Y+yHeatRate
	ldi	r17,0
	subi_w	r17,r16,-1000
	PrintAt	11,3
ind6_9:

	ret
ind6_1:

;	|     —“¿“»—“» ¿     | Screen7
;	|   ¬–≈Ãﬂ      :     |
;	|  À»—“Œ¬            |
;	|   —“–”Õ            |
	cpi	r25,7
	brne	ind7_1
	cpi	r24,1		// Line 1 -> Time
	brne	ind7_2
	lds	r16,HoursL
	lds	r17,HoursH
	PrintAt	9,5
	inc	XL
	lds	r16,Minutes
	rjmp	Print2
ind7_2:
	cpi	r24,2		// Line 2 -> Sessions (sheets)
	brne	ind7_3
	lds	r16,Sheets100L
	lds	r17,Sheets100H
	PrintAt	9,5
	lds	r16,Sheets
	rjmp	Print2
ind7_3:
	cpi	r24,3		// Line 3 -> Strings torn
	brne	ind7_4
	lds	r16,StringsL
	lds	r17,StringsH
	PrintAt	9,5
	ret
ind7_4:
	ret
ind7_1:

	ret


LineAddr: .db $80+$00,$80+$40,$80+$14,$80+$54

ScreenCont:
;	|123-ƒ¬»√¿“b (#)-Ã≈Õﬁ| Screen0
;	|4 6     (*)-–≈«¿  Õ¿|
;	|789          —“¿–“  |
;	|(5)-Õ¿√–≈¬->¬€ Àﬁ◊≈Õ|
;        0123456789ABCDEF0123
.db	$31,$32,$33,$2D,$E0,$42,$A5,$A1,$41,$54,$62,$20,$28,$23,$29,$2D,$4D,$45,$48,$B0
.db	$34,$20,$36,$20,$20,$20,$20,$20,$28,$2A,$29,$2D,$50,$45,$A4,$41,$4B,$20,$48,$41
.db	$37,$38,$39,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$43,$54,$41,$50,$54,$20,$20
.db	$28,$35,$29,$2D,$48,$41,$A1,$50,$45,$42,$2D,$3E,$42,$AE,$4B,$A7,$B0,$AB,$45,$48
;	|(1)-–”◊ÕŒ≈ ”œ–.     | Screen1
;	|(2)-—“¿–“ –≈« » N00 |
;	|(3)-¬€¡Œ– –¿— –Œﬂ   |
;	|(4)-œ¿–¿Ã (5)-‘Œ–Ã¿“|
.db	$28,$31,$29,$2D,$50,$A9,$AB,$48,$4F,$45,$20,$A9,$A8,$50,$2E,$20,$20,$20,$20,$20
.db	$28,$32,$29,$2D,$43,$54,$41,$50,$54,$20,$50,$45,$A4,$4B,$A5,$20,$4E,$30,$30,$20
.db	$28,$33,$29,$2D,$42,$AE,$A0,$4F,$50,$20,$50,$41,$43,$4B,$50,$4F,$B1,$20,$20,$20
.db	$28,$34,$29,$2D,$A8,$41,$50,$41,$4D,$20,$28,$35,$29,$2D,$AA,$4F,$50,$4D,$41,$54
;	| œ¿–¿Ã≈“–€  (#)-Ã≈Õﬁ| Screen2
;	|<123>Õ¿√–≈¬   000%  |
;	|<456>— Œ–Œ—“b 23ÏÏ/Ò|
;	|<7>-ƒ¬»√¿“b À»—“    |
;        0123456789ABCDEF0123
.db	$20,$A8,$41,$50,$41,$4D,$45,$54,$50,$AE,$20,$20,$28,$23,$29,$2D,$4D,$45,$48,$B0
.db	$3C,$31,$32,$33,$3E,$48,$41,$A1,$50,$45,$42,$20,$20,$20,$30,$30,$30,$25,$20,$20
.db	$3C,$34,$35,$36,$3E,$43,$4B,$4F,$50,$4F,$43,$54,$62,$20,$32,$33,$BC,$BC,$2F,$63
.db	$3C,$37,$3E,$2D,$E0,$42,$A5,$A1,$41,$54,$62,$20,$A7,$A5,$43,$54,$20,$20,$20,$20
;	|   ¬€¡Œ– –¿— –Œﬂ    | Screen3
;	|<1-3>-¬€¡Œ– (#)-Ã≈Õﬁ|
;	|                    |
;	|                    |
.db	$20,$20,$20,$42,$AE,$A0,$4F,$50,$20,$50,$41,$43,$4B,$50,$4F,$B1,$20,$20,$20,$20
.db	$3C,$31,$2D,$33,$3E,$2D,$42,$AE,$A0,$4F,$50,$20,$28,$23,$29,$2D,$4D,$45,$48,$B0
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
;	|   –≈« ¿ œ–≈–¬¿Õ¿   | Screen4
;	|                    |
;	|                    |
;	|                    |
.db	$20,$20,$20,$50,$45,$A4,$4B,$41,$20,$A8,$50,$45,$50,$42,$41,$48,$41,$20,$20,$20
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
;	|   ÃŒƒ”À‹ Õ¿ ¿À¿    | Screen5
;	|   —»ÀŒ¬ŒÈ ÃŒƒ”Àb   |
;	|    Õ≈ Œ“¬≈◊¿≈“     |
;	|                    |
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$20,$43,$A5,$A7,$4F,$42,$4F,$A6,$20,$4D,$4F,$E0,$A9,$A7,$62,$20,$20,$20
.db	$20,$20,$20,$20,$48,$45,$20,$4F,$54,$42,$45,$AB,$41,$45,$54,$20,$20,$20,$20,$20
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
;	|                    | Screen6
;	|                    |
;	|  Œ¡–¿¡Œ“¿ÕŒ 000%   |
;	|Àﬁ¡¿ﬂ  ÕŒœ ¿-—“Œœ!!!|

;	|                    | Screen6
;	|                    |
;	| Õ¿√–≈¬ —“–”Õ€ 12   |
;	|<*< Õ¿√–≈¬ 000% >#> |
;        0123456789ABCDEF0123
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$4F,$A0,$50,$41,$A0,$4F,$54,$41,$48,$4F,$20,$30,$30,$30,$25,$20,$20,$20
.db	$3C,$2A,$3C,$20,$48,$41,$A1,$50,$45,$42,$20,$30,$30,$30,$25,$20,$3E,$23,$3E,$20
;	|     —“¿“»—“» ¿     | Screen7
;	|   ¬–≈Ãﬂ      :     |
;	|  À»—“Œ¬            |
;	|   —“–”Õ            |
.db	$20,$20,$20,$20,$20,$43,$54,$41,$54,$A5,$43,$54,$A5,$4B,$41,$20,$20,$20,$20,$20
.db	$20,$20,$20,$42,$50,$45,$4D,$B1,$20,$20,$20,$20,$20,$20,$3A,$20,$20,$20,$20,$20
.db	$20,$20,$A7,$A5,$43,$54,$4F,$42,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$20,$43,$54,$50,$A9,$48,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
;	|                    | Screen8
;	|   Œÿ»¡ ¿ –¿¡Œ“€    |
;	|  —  ¿–“ŒÈ œ¿Ãﬂ“»   |
;	|                    |
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
.db	$20,$20,$20,$4F,$AC,$A5,$A0,$4B,$41,$20,$50,$41,$A0,$4F,$54,$AE,$20,$20,$20,$20
.db	$20,$20,$43,$20,$4B,$41,$50,$54,$4F,$A6,$20,$A8,$41,$4D,$B1,$54,$A5,$20,$20,$20
.db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20
;	|  ‘Œ–Ã¿“»–Œ¬¿Õ»≈    | Screen9
;	|    ¿–“€ œ¿Ãﬂ“»     |
;	|     (#)-Ã≈Õﬁ       |
;	| (1,2,3,8,*)-‘Œ–Ã¿“ |
.db	$20,$20,$AA,$4F,$50,$4D,$41,$54,$A5,$50,$4F,$42,$41,$48,$A5,$45,$20,$20,$20,$20
.db	$20,$20,$20,$4B,$41,$50,$54,$AE,$20,$A8,$41,$4D,$B1,$54,$A5,$20,$20,$20,$20,$20
.db	$20,$20,$20,$20,$20,$28,$23,$29,$2D,$4D,$45,$48,$B0,$20,$20,$20,$20,$20,$20,$20
.db	$20,$28,$31,$2C,$32,$2C,$33,$2C,$38,$2C,$2A,$29,$2D,$AA,$4F,$50,$4D,$41,$54,$20


Messages:
.db	$42,$AE,$4B,$A7,$B0,$AB,$45,$48		; ¬€ Àﬁ◊≈Õ - 0
.db	0,  $42,$4B,$A7,$B0,$AB,$45,$48,$20,0	; ¬ Àﬁ◊≈Õ  - 1
.db	$4F,$A0,$50,$AE,$42,$20,$20,$20		; Œ¡–€¬    - 2
.db	0,  $A7,$A5,$43,$54,$20,$20,$20,$20,0	; À»—“ - 3
.db	$50,$45,$A4,$41,$4B,$20,$20,$20         ; –≈«¿  - 4
.db	0,  $A8,$50,$45,$50,$42,$41,$48,$41,$20
.db	$4F,$A8,$45,$50,$41,$54,$4F,$50,$4F,$4D  ; œ–≈–¬¿Õ¿ Œœ≈–¿“Œ–ŒÃ - 5
.db	0,  $4F,$AC,$A5,$A0,$4B,$41,$20,$AB,$54
.db	$45,$48,$A5,$B1,$20,$4B,$41,$50,$54,$AE	; Œÿ»¡ ¿ ◊“≈Õ»ﬂ  ¿–“€ - 6
.db	0,  $20,$20,$20,$4F,$A0,$50,$AE,$42,$20
.db	$43,$54,$50,$A9,$48,$AE			;    Œ¡–€¬ —“–”Õ€ - 7
.db	0,  $48,$41,$A1,$50,$45,$42,$20,$43,$54,$50,$A9,$48,$AE ;    Õ¿√–≈¬ —“–”Õ€ - 8
.db	0,  $4D,$4F,$E0,$A9,$A7,$62,$20,$48,$41,$4B,$41,$A7,$41,$20, 0  ; ÃŒƒ”À‹ Õ¿ ¿À¿  - 9
.db	$20,$20,$A4,$41,$4D,$AE,$4B,$41,$48,$A5,$45,$20,$43,$54
.db	$50,$A9,$48,$AE, 0, 0  ; «¿Ã€ ¿Õ»≈ —“–”Õ€  - 10


;******
; Message
;******
ExtMes:	mov	XL,r16
	swap	XL
	andi	XL,$0f
	andi	r16,$0f
	subi	XL,-LCDBuf
Mes:	ldi_w	ZH,ZL,Messages*2
msg_3:	subi	r16,1
	brcs	msg_1
msg_2:	lpm	r0,Z+
	tst	r0
	brne	msg_2
	rjmp	msg_3
msg_1:	lpm	r16,Z+
	tst	r16
	breq	msg_4
	cpi	r16,$80
	brlo	msg_6
	cpi	r16,$A0
	brsh	msg_6
	subi	r16,$80
	add	XL,r16
	rjmp	msg_1
msg_6:	st	X+,r16
	rjmp	msg_1
msg_4:	ret


;*********************************
; Print 2 digits with leading zero
; r16 = Value; X -> address
;*********************************
Print2:	ldi	r17,$2F
p2_1:	inc	r17
	subi	r16,10
	brcc	p2_1
	st	X+,r17
	subi	r16,-(10+$30)
	st	X+,r16
	ret

;*********************************
; Print number (0..65535)
;  r18 = Digits-1; r16,r17 = Value; X -> address
;*********************************
B2Dec:	ldi_w	ZH,ZL,Razr*2
	mov	r14,r18
	ldi	r18,4
	ldi	r19,$20
b2d_5:	lpm	r15,Z+
	lpm	r0,Z+
	andi	r19,$30
b2d_2:	sub	r16,r15
	sbc	r17,r0
	brcs	b2d_1
	ori	r19,$30
	inc	r19
	rjmp	b2d_2
b2d_1:	add	r16,r15
	adc	r17,r0
	cp	r14,r18
	brlo	b2d_3
	st	X+,r19
b2d_3:	dec	r18
	brne	b2d_5
	subi	r16,-$30
	st	X+,r16
	ret

Razr:
	.db	low(10000), high(10000)
	.db	low(1000), high(1000)
	.db	low(100), high(100)
	.db	low(10), high(10)

;*****
; Print decimal point
;*****
Point:
	subi	XL,2
	ld	r16,X
	cpi	r16,$20
	brne	pnt_1
	ldi	r16,$30
pnt_1:	st	X+,r16
	ld	r16,X
	ldi	r17,$2E
	st	X+,r17
	st	X+,r16
	ret

DPoint:
	subi	XL,3
	ld	r16,X
	cpi	r16,$20
	brne	dpnt_1
	ldi	r16,$30
dpnt_1:	st	X+,r16
	ld	r16,X
	cpi	r16,$20
	brne	dpnt_2
	ldi	r16,$30
dpnt_2:	ldi	r17,$2E
	st	X+,r17
	ld	r17,X
	st	X+,r16
	st	X+,r17
	ret
