EERead:	sbic	EECR,EEWE
	rjmp	EERead
	out	EEARL,r24
	out	EEARH,r25
	sbi	EECR,0
	in	r16,EEDR
	subi_w	r25,r24,-1
	ret

EEWrite:
	push	r17
EEW_2:	sbic	EECR,EEWE
	rjmp	EEW_2
	out	EEARL,r24
	out	EEARH,r25
	subi_w	r25,r24,-1
	sbi	EECR,0
	in	r17,EEDR
	cp	r17,r16
	breq	eew_1
	out	EEDR,r16
	cli
	sbi	EECR,EEMWE
	sbi	EECR,EEWE
	sei
eew_1:	pop	r17
	ret

; Memory map
; $00 - маркер $A5
; $01 - CurCutNum
; $02 - Speed
; $03 - StringCurrent
; $04 - Flags
; $05 - MinCnt
; $06 - Minutes
; $07 - HoursL
; $08 - HoursH
; $09 - StringsL
; $0A - StringsH
; $0B - Sheets
; $0C - Sheets100L
; $0D - Sheets100H
; $0E - CheckSum
ReadMemory:
	ldi_w	r25,r24,0
	rcall	EERead	; Маркер $A5
	cpi	r16,$A5
	brne	rm_1
	mov	r20,r16
	rcall	EERead	; CurCutNum
	add	r20,r16
	cpi	r16,20
	brlo	rm_2
	ldi	r16,0
rm_2:	std	Y+yCurCutNum,r16
	rcall	EERead	; Speed
	add	r20,r16
	cpi	r16,10
	brsh	rm_3
	ldi	r16,10
rm_3:	cpi	r16,80
	brlo	rm_4
	ldi	r16,80
rm_4:	std	Y+ySpeed,r16
	rcall	EERead	; StringCurrent
	add	r20,r16
	cpi	r16,0
	breq	rm_6
	cpi	r16,5
	brsh	rm_5
	ldi	r16,5
rm_5:	cpi	r16,100
	brlo	rm_6
	ldi	r16,100
rm_6:	std	Y+yHeatRate,r16
	rcall	EERead	; Falgs
	add	r20,r16
	bst	r16,0
	bld	AFlags,afReverseY
	ldi_w	ZH,ZL,MinCnt
rm_7:	rcall	EERead
	add	r20,r16
	st	Z,r16
	inc	ZL
	cpi	ZL,Sheets100H
	brlo	rm_7
	rcall	EERead
	cp	r16,r20
	breq	rm_8
rm_1:	rcall	DefaultSetup
	rcall	SaveMemory
rm_8:	ldi_w	r17,r16,$FFF6
	ldi	r18,9
	ldd	r19,Y+ySpeed
	ldi_w	r21,r20,0
	rcall	Div24
	std	Y+ySpeedKoefL,r16
	std	Y+ySpeedKoefH,r17
	ret

DefaultSetup:
	clt
	bld	AFlags,afReverseY
	stdi	ySpeed,60
	stdi	yHeatRate,50
	std	Y+yCurCutNum,YH
	ldi_w	ZH,ZL,MinCnt
dst_1:	st	Z+,YH
	cpi	ZL,Sheets100H
	brlo	dst_1
	ret

SaveMemory:
	ldi_w	r25,r24,0
	ldi	r16,$A5
	mov	r20,r16
	rcall	EEWrite
	ldd	r16,Y+yCurCutNum
	add	r20,r16
	rcall	EEWrite
	ldd	r16,Y+ySpeed
	add	r20,r16
	rcall	EEWrite
	ldd	r16,Y+yHeatRate
	add	r20,r16
	rcall	EEWrite
	ldi	r16,0
	bst	AFlags,afReverseY
	bld	r16,0
	add	r20,r16
	rcall	EEWrite
	ldi_w	ZH,ZL,MinCnt
svm_1:	ld	r16,Z+
	add	r20,r16
	rcall	EEWrite
	cpi	ZL,Sheets100H
	brlo	svm_1
	mov	r16,r20
	rcall	EEWrite
	ret
