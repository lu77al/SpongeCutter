.equ	cmNoCmd		= 0
.equ	cmUpLeft	= 1
.equ	cmUp		= 2
.equ	cmUpRight	= 3
.equ	cmLeft		= 4
.equ	cmRight		= 5
.equ	cmDownLeft	= 6
.equ	cmDown		= 7
.equ	cmDownRight	= 8
.equ	cmInitX		= 9
.equ	cmInitY		= 10
.equ	cmSearchStart	= 11
;.equ	cmHeatingOff	= 12
.equ	cmStopSmooth	= 13
.equ	cmStopImmediate	= 14


Cutter:
/*
	ldi	XL,TXBuf+4	; Start of UART cmd body
	ldd	r16,Y+yNextCommand
	std	Y+yNextCommand,ZeroReg
	cpi	r16,cmStopSmooth
	brne	cc_1
	ldi	r16,0
	rjmp	StartMove

cc_1:	ret
*/
	ldi	XL,TXBuf+4	; Start of UART cmd body

	sbrc	AFlags,afCutting
	rjmp	DoCutting

	ldd	r16,Y+yNextCommand
	std	Y+yNextCommand,ZeroReg
	cpi	r16,cmNoCmd
	breq	StateReq
	cpi	r16,cmDownRight+1
	brsh	cut_1
	ldd	r17,Y+yStepperRep
	cpi	r17,0
	brne	StopSmooth
	dec	r16
	rjmp	StartMove
cut_1:	cpi	r16,cmStopSmooth
	breq	StopSmooth
	cpi	r16,cmStopImmediate
	breq	StopImmediate
	cpi	r16,cmSearchStart
	breq	SearchStart

StateReq:
	ldd	r16,Y+yCurScreen
	cpi	r16,0
	breq	srq_1
	cpi	r16,6
	brne	StopSmooth
srq_1:	ldi	r16,$00
FinishShortCMD:
	st	X+,r16
FinishCMD:
	ldd	r16,Y+yHeatRate
	sbrs	AFLags,afHeating
	ldi	r16,0
	st	X+,r16
	ldi	r16,$4A
	ldd	r17,Y+yMyAddr
	rjmp	ComplUartMes

StopImmediate:
	ldi	r16,$03
	rjmp	FinishShortCMD

StopSmooth:
	ldi	r16,$04
	rjmp	FinishShortCMD



SearchStart:
	ldd	r16,Y+ySearchCnt
	ldi	r16,01
	st	X+,r16
	ldi	r16,$FF
	st	X+,r17 ; Master axis steps
	st	X+,r16
	st	X+,ZeroReg ; Slave axis steps
	st	X+,ZeroReg
	ldi_w	r25,r24,38000 ; Start/End period
	st	X+,r25
	st	X+,r24
	ldi_w	r17,r16,6610 ; Cruise speed (20 mm/s)
	st	X+,r17
	st	X+,r16
	st	X+,r25 ; End period
	st	X+,r24
	ldi_w	r17,r16,10 ; Brake length
	st	X+,r17
	st	X+,r16
	ldi	r16,0b11100	; Left,  Long, Stop on hall
	st	X+,r16

	rjmp	FinishCMD


; --- Line in queue ---
; 00 - MStH     - Steps at master axis
; 01 - MStL
; 02 - SStH     - Steps at slave axis
; 03 - SStL
; 04 - StartTH  - Initial period
; 05 - StartTL
; 06 - CruiseTH - Cruise period
; 07 - CruiseTL
; 08 - EndTH    - End period
; 09 - EndTL
; 0A - BrLenH   - Breaking distance
; 0B - BrLenL
; 0C - Flags
;	0 - XDIR
;	1 - YDIR
;	2 - XMaster
/*
SearchTab:
;	M_STEPS, Speed, BrDist, Flags
.dw	3026,  2204, 580, 0b00101	; Right, 100mm, 60 mm/s
.dw	$FFFF, 2204, 580, 0b11100	; Left,  Long,  60 mm/s (stop on hall)
.dw	908,   4407, 77,  0b00100	; Right, 30mm,  30 mm/s
*/



StartMove:
	ldi	r17,6
	mul	r16,r17
	ldi_w	ZH,ZL,MoveTab*2
	add	ZL,r0
	adc	ZH,r1

	ldi	r16,01
	st	X+,r16

	ldi	r16,$FF; Master axis steps
	st	X+,r16
	st	X+,r16
	lpm	r16,Z+	; Slave axis steps
	lpm	r17,Z+
	st	X+,r17
	st	X+,r16
	ldi_w	r25,r24,38000 ; Start/End period
	st	X+,r25
	st	X+,r24
; koef = 65535 / Speed * 8 = 
	ldi_w	r17,r16,$FFFF
	ldd	r18,Y+ySpeed
	ldi	r19,0
	rcall	Div
	lsl16	r17,r16
	lsl16	r17,r16
	lsl16	r17,r16
	lpm	r18,Z+	; Cruise period for 8 mm/s
	lpm	r19,Z+
	rcall	Mul_16
	st	X+,r17  ; Cruise period
	st	X+,r16
	st	X+,r25 ; End period
	st	X+,r24

	ldi	r19,0  ; Divider for speed calc (break len)
	mov	r18,r17
	ldi_w	r17,r16,$FFFF
	rcall	Div
	movw	r19:r18, r17:r16
	rcall	Mul_16		 ; Square
	ldi_w	r19,r18,24160	 ; koef
	rcall	Mul_16

	st	X+,r17	; BreakLen
	st	X+,r16

	lpm	r16,Z+	; Flags
	ldi	r17,0b010
	sbrs	AFlags,afReverseY
	eor	r16,r17
	st	X+,r16

	rjmp	FinishCMD



MoveTab: ; Slave axis steps; cruise period; flags
	.dw	26708,	23373,	0b01100	; 1 (B1) - UP/LEFT
	.dw	0,	40554,	0b01000	; 2 (B2) - UP
	.dw	26708,	23373,	0b01101	; 3 (B3) - UP/RIGHT
	.dw	0,	16527,	0b01100	; 4 (B4) - LEFT
	.dw	0,	16527,	0b01101	; 5 (B6) - RIGHT
	.dw	26708,	23373,	0b01110	; 6 (B7) - DOWN/LEFT
	.dw	0,	40554,	0b01010	; 7 (B8) - DOWN
	.dw	26708,	23373,	0b01111	; 8 (B9) - DOWN/RIGHT

DoCutting:
	std	Y+yNextCommand,YH
	ldd	r16,Y+yInitCutCnt ; Initial delay 
	subi	r16,1
	brcs	dc_1
	std	Y+yInitCutCnt,r16
	clt
	cpi	r16,140
	brsh	dc_2
	set	
dc_2:	bld	AFlags,afHeating
	rjmp	StateReq
dc_1:	sbrc	AFlags,afNoStepperRep	; Just req status if no response
dc_3:	rjmp	StateReq
	ldd	r19,Y+ySegInBuf
	cpi	r19,32
	brsh	dc_3			;   ... or segment buffer is full
	ldd	r16,Y+ySegLastB0
	ldd	r17,Y+ySegLastB1
	ldd	r18,Y+ySegLastB2		; Downcount segment counter
	subi_w	r17,r16,1
	sbci	r18,0
	brcs	dc_4			;   and go to finishing if all segs are pushed 
	std	Y+ySegLastB0,r16
	std	Y+ySegLastB1,r17
	std	Y+ySegLastB2,r18
	rcall	PushSeg			; Push next seg 
	sbrs	AFlags,afCardError
	ret
	stdi	yFinishSoundCnt,$F1	; card error processing
	stdi	yErrorCode,1
	stdi	yCurScreen,4
	ret
dc_4:		; Finishing 
	ldd	r16,Y+yStepperRep
	andi	r16,$8F
	brne	dc_3
	brne	dc_3
	rcall	StopCutting
	std	Y+yCurScreen,YH
	stdi	yFinishSoundCnt,$70
	ret

PushSeg:
	rcall	ReadNextItem
	sbrc	AFlags,afCardError
	ret
	mov	XL,ZL
	rjmp	FinishCMD

RequestCutter:
	clt
	bld	BFlags,bfReqCutter
; NoRequest counters
	set
	ldd	r16,Y+yNoStepRepCnt
	inc	r16
	cpi	r16,30
	brlo	rcut_1
	ldi	r16,30
	bld	AFlags,afNoStepperRep
rcut_1:	std	Y+yNoStepRepCnt,r16
	lds	r16,NoHeaterRepCnt
	inc	r16
	cpi	r16,30
	brlo	rcut_2
	ldi	r16,30
	bld	AFlags,afNoStringRep
rcut_2:	sts	NoHeaterRepCnt,r16
	ldd	r16,Y+yCurScreen
	cpi	r16,5
	breq	rcut_3
	sbrc	AFlags,afNoStringRep
	rjmp	rcut_4
	sbrs	AFlags,afNoStepperRep
	rjmp	rcut_3
rcut_4:	rcall	StopCutting
	stdi	yCurScreen,5
rcut_3:
;	ldd	r16,Y+yNoStepRepCnt
;	inc	r16
;	cpi	r16,30
;	brlo	rcut_2
;rcut_5:	set
;	bld	AFlags,afNoStepperRep
;	ldd	r16,Y+yCurScreen
;	cpi	r16,5
;	breq	rcut_3
;	rcall	StopCutting
;	stdi	yCurScreen,5
;rcut_3:	ldi	r16,255
;rcut_2:	std	Y+yNoStepRepCnt,r16
// Heater reply check
;	lds	r16,NoHeaterRepCnt
;;	inc	r16
;	cpi	r16,30
;	brlo	rcut_4
;	cpi	r16,5
;	breq	rcut_5
;	rcall	StopCutting
;	stdi	yCurScreen,5
;rcut_3:	ldi	r16,255
;rcut_2:	sts	Y+yNoStepRepCnt,r16
;
;
;rcut_4:

	rjmp	Cutter

;	sbrs	FlagReg,Cutting
;dc_1:	ret
;	ldd	r16,Y+NoStepRepCnt
;	cpi	r16,0
;	brne	dc_3
;	ldd	r16,Y+SegLastB0
;	ldd	r17,Y+SegLastB1
;	ldd	r18,Y+SegLastB2
;	ldd	r19,Y+SegInBuf
;	cpi	r19,6
;	brlo	dc_2	
;dc_3:	rjmp	ReqStatus
;dc_2:	subi_w	r17,r16,1
;	sbci	r18,0
;	brcs	dc_1
;	std	Y+SegLastB0,r16
;	std	Y+SegLastB1,r17
;	std	Y+SegLastB2,r18
;	rjmp	PushSeg



/*
Speed=8; StartP=38000; EndP=38000 
DIR		Master	Slave	CruiseP	Flags
<UP>		65500	0	40554	0b001	
<RIGHT>		0	65500	16527	0b111	
<UP/RIGHT>	65500	26708	23373	0b101	


	sbrc	AFlags,afCutting
	rjmp	DoCutting

	ldd	r16,Y+yInitCutCnt	// Stop steppers and turn of heating after StopCutting
	subi	r16,1
	brcs	cut_20
	std	Y+yInitCutCnt,r16
	ldi	r17,cmStopFree
	sbrs	r16,0
	ldi	r17,cmHeatingOff
	std	Y+yNextCommand,r17
cut_20:

	ldd	r16,Y+yNextCommand

	cpi	r16,cmNoCmd	// if No CMD ->just request status
	brne	cut_1
cut_3:	rjmp	ReqStatus
cut_1:
	cpi	r16,10	// Process move 
	brsh	cut_2
	cpi	r16,5
	breq	cut_3
	brlo	cut_4
	dec	r16
cut_4:	dec	r16
	lsl	r16
	lsl	r16
	ldi_w	ZH,ZL,StringBasedMove*2
	sbrs	FlagReg,ReverseY
	rjmp	cut_5
	ldi_w	ZH,ZL,SheetBasedMove*2
cut_5:	add	ZL,r16
	adc	ZH,YH
	ldi	XL,TXBuf+2
	lpm	r16,Z+
	st	X+,r16
	lpm	r16,Z+
	st	X+,r16
	ldi	r16,$0C
	st	X+,r16
	lpm	r16,Z+
	lpm	r17,Z+
	ldd	r18,Y+SpeedKoefL
	ldd	r19,Y+SpeedKoefH
	rcall	Mul_16
	st	X+,r17
	st	X+,r16
	ldi	r16,4
	std	Y+NextCommand,YH
	rjmp	FormMes
cut_2:
	cpi	r16,cmStopFree
	brne	cut_6
	std	Y+NextCommand,YH
	ldi	XL,TXBuf+2
	ldi	r16,1
	st	X+,r16
	ldi	r16,3
	rjmp	FormMes
cut_6:
	cpi	r16,cmHeatingOff
	brne	cut_7
cut_14:	std	Y+NextCommand,YH
	ldi	XL,TXBuf+2
	st	X+,YH
	ldi	r16,5
	rjmp	FormMes
cut_7:
	cpi	r16,cmHeatingOn
	brne	cut_8
	std	Y+NextCommand,YH
cut_12:	ldd	r16,Y+StringCurrent
	lsl	r16
	ldi	r17,196
	mul	r17,r16
	ldi	XL,TXBuf+2
	st	X+,r1
	ldi	r16,5
	rjmp	FormMes
cut_8:
	cpi	r16,cmInitX
	brne	cut_9
	ldi	r16,0
cut_11:	std	Y+NextCommand,YH
	ldi	XL,TXBuf+2
	st	X+,r16
	ldi	r16,2
	rjmp	FormMes
cut_9:
	cpi	r16,cmInitY
	brne	cut_10
	ldi	r16,1
	rjmp	cut_11
cut_10:

	std	Y+NextCommand,YH

	ret

StringBasedMove:
.dw	103+256*4,18413 ; 1
.dw	0+256*1,  32300 ; 2
.dw	103+256*5,18413 ; 3
.dw	0+256*6,  13020 ; 4
.dw	0+256*7,  13020 ; 6
.dw	103+256*6,18413 ; 7
.dw	0+256*3,  32300 ; 8
.dw	103+256*7,18413 ; 9


SheetBasedMove:
.dw	103+256*6,18413 ; 7
.dw	0+256*3,  32300 ; 8
.dw	103+256*7,18413 ; 9
.dw	0+256*6,  13020 ; 4
.dw	0+256*7,  13020 ; 6
.dw	103+256*4,18413 ; 1
.dw	0+256*1,  32300 ; 2
.dw	103+256*5,18413 ; 3

; Запрос статуса
; *** 01 - Запрос статуса ***
; A5-01-CS
ReqStatus:
	ldi	XL,TXBuf+2
	ldi	r16,1
	rjmp	FormMes	

PushSeg:
	rcall	ReadNextItem
	sbrc	FlagReg,CardError
	ret
	mov	XL,ZL
	ldi	r16,0
	rjmp	FormMes	

*/
/*
DoCutting:
	std	Y+NextCommand,YH
	ldd	r16,Y+InitCutCnt
	subi	r16,1
	brcs	dc_1
	std	Y+InitCutCnt,r16
	cpi	r16,140
	breq	dc_2
	rjmp	ReqStatus
dc_2:	rjmp	cut_12
dc_1:	sbrc	FlagReg,NoStepperRep
dc_3:	rjmp	ReqStatus
	ldd	r19,Y+SegInBuf
	cpi	r19,6
	brsh	dc_3
	ldd	r16,Y+SegLastB0
	ldd	r17,Y+SegLastB1
	ldd	r18,Y+SegLastB2
	subi_w	r17,r16,1
	sbci	r18,0
	brcs	dc_4
	std	Y+SegLastB0,r16
	std	Y+SegLastB1,r17
	std	Y+SegLastB2,r18
	rcall	PushSeg
	sbrs	FlagReg,CardError
	ret
	stdi	FinishSoundCnt,$F1
	stdi	ErrorCode,1
	stdi	CurScreen,4
	ret
dc_4:	ldd	r16,Y+SegInBuf
	ldd	r17,Y+PointsInSeg
	or	r16,r17
	brne	dc_3
	rcall	StopCutting
	std	Y+CurScreen,YH
	stdi	FinishSoundCnt,$70
	ret


;	sbrs	FlagReg,Cutting
;dc_1:	ret
;	ldd	r16,Y+NoStepRepCnt
;	cpi	r16,0
;	brne	dc_3
;	ldd	r16,Y+SegLastB0
;	ldd	r17,Y+SegLastB1
;	ldd	r18,Y+SegLastB2
;	ldd	r19,Y+SegInBuf
;	cpi	r19,6
;	brlo	dc_2	
;dc_3:	rjmp	ReqStatus
;dc_2:	subi_w	r17,r16,1
;	sbci	r18,0
;	brcs	dc_1
;	std	Y+SegLastB0,r16
;	std	Y+SegLastB1,r17
;	std	Y+SegLastB2,r18
;	rjmp	PushSeg

*/

