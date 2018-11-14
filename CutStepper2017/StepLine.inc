InitSteperInt:
	ldi	r16,$40
	mov	CMP_PeriodH,r16
	outi	TCCR1A,0b00000000	; No PWMs
	outi	TCCR1B,0b00000001	; CK/1
	outi	TIMSK, 1<<OCIE1A	; OC1A interrupt enabled
	ret

StepInt:
	PrepareX	; Prepare STEP signals
	PrepareY
	in	SSREG,SREG ; Save SREG
	add	TimerCMPL,CMP_PeriodL	; Update OCR 
	adc	TimerCMPH,CMP_PeriodH
;----- ƒл€ обхода глюка протеуса ----
;	inc	TimerCMPH
;	inc	TimerCMPL
;	out	OCR1AH,TimerCMPH
;	out	OCR1AL,TimerCMPL
;	dec	TimerCMPH
;	dec	TimerCMPL
;------------------------------------
	out	OCR1AH,TimerCMPH
	out	OCR1AL,TimerCMPL
	push	r16
	push	r17
	push	r18
	push	r19
	push	r24
	push	r25

	sbrs	AFlags,afNewDir
	rjmp	st_9
	sbrc	AFlags,afXForward
	ForwardX
	sbrs	AFlags,afXForward
	BackwardX
	sbrc	AFlags,afYForward
	ForwardY
	sbrs	AFlags,afYForward
	BackwardY
	clt
	bld	AFlags,afNewDir
st_9:
	ldd	r16,Y+yMasterStCntL
	ldd	r17,Y+yMasterStCntH
	cp	r16,ZeroReg
	cpc	r17,ZeroReg
	breq	cnp_14		; ignore rest if no steps left

	ldd	r18,Y+yMasterStDivCnt
	dec	r18		; MasterDivCnt and SlaveDivCnt are used
	brne	st_1		;  for smoothing move at slave axis

	ldd	r24,Y+yMasterPosL ; StepCounter for mesuring stop distance and so on
	ldd	r25,Y+yMasterPosH
	subi_w	r25,r24,-1
	std	Y+yMasterPosL,r24
	std	Y+yMasterPosL,r25
;--- Debug ---
;	ldi	r25,1
;	add	r8,r25
;	adc	r9,ZeroReg
;-------------
	sbrs	AFlags,afXMaster ; If MasterDivCnt counted
	StepY
	sbrc	AFlags,afXMaster ; step master axis
	StepX
	sbrc	AFlags,afMoveLong
	rjmp	st_10
	subi_w	r17,r16,1
st_10:	std	Y+yMasterStCntL,r16 ; decrement master axis steps
	std	Y+yMasterStCntH,r17
st_6:	ldi	r18,4		    ; restart MasterDivCnt
st_1:	std	Y+yMasterStDivCnt,r18

	ldd	r16,Y+ySlaveStDivL ; Check and step slave axis
	ldd	r17,Y+ySlaveStDivH
	ldd	r18,Y+ySlaveStSubL
	sub	r16,r18
	ldd	r18,Y+ySlaveStSubH
	sbc	r17,r18
	brcc	st_3
st_2:	ldd	r18,Y+ySlaveStBaseL
	add	r16,r18
	ldd	r18,Y+ySlaveStBaseH
	adc	r17,r18
	ldd	r18,Y+ySlaveStDivCnt
	dec	r18
	brne	st_4
;--- Debug ---
;	ldi	r25,1
;	add	r10,r25
;	adc	r11,ZeroReg
;-------------
	sbrs	AFlags,afXMaster
	StepX
	sbrc	AFlags,afXMaster
	StepY
	ldi	r18,4
st_4:	std	Y+ySlaveStDivCnt,r18
st_3:	std	Y+ySlaveStDivL,r16
	std	Y+ySlaveStDivH,r17
st_8:
 ;--- Calc next period ---
	ldd	r16,Y+yMasterStCntL	; Check end of line
	ldd	r17,Y+yMasterStCntH
	cp	r16,YH
	cpc	r17,YH
	breq	cnp_14
	rjmp	cnp_12
cnp_14:					; Find and prepare next branch (line)
	ldd	r16,Y+yDataCnt
	subi	r16,1
	brcc	cnp_13		; If no new branches
	ldi_w	r25,r24,$4000		; stop drives
	rjmp	cnp_7
cnp_13:	std	Y+yDataCnt,r16	; Else extract new branch data
	ldd	r16,Y+yDataTail
	push	XL
	mov	XL,r16
	ldi	XH,2
	sbrc	XL,4
	ldi	XH,3
	andi	XL,$0F
	swap	XL
	inc	r16
	andi	r16,31	; 32 enties in queue
	std	Y+yDataTail,r16
	ld	r17,X+
	ld	r16,X+
	std	Y+yMasterStCntL,r16 ; Steps at master axis 2 (2)
	std	Y+yMasterStCntH,r17
	std	Y+ySlaveStBaseL,r16   ; = base for linediv algorithm
	std	Y+ySlaveStBaseH,r17
	subi_w	r17,r16,1
	std	Y+ySlaveStDivL,r16    ; = initial value of remainder
	std	Y+ySlaveStDivH,r17
	ld	r17,X+		   ; Steps at slave axis 2 (4)
	ld	r16,X+
	std	Y+ySlaveStSubL,r16  ;  = divider for linediv algorithm
	std	Y+ySlaveStSubH,r17
	ld	CMP_PeriodH,X+	  ; initial period 2 (6)
	ld	CMP_PeriodL,X+
	ld	r17,X+		  ; cruise period  2 (8)
	ld	r16,X+
	std	Y+yMidPeriodL,r16
	std	Y+yMidPeriodH,r17
	ld	r17,X+		  ; end period  2 (10)
	ld	r16,X+
	std	Y+yEndPeriodL,r16
	std	Y+yEndPeriodH,r17
	ld	r17,X+		  ; breaking distance 2 (12)
	ld	r16,X+
	std	Y+yAXChangeL,r16
	std	Y+yAXChangeH,r17
	ld	r16,X+		  ; flags 1 (13)
	pop	XL
	ldi	XH,1
	bst	r16,3
	bld	AFlags,afMoveLong
	bst	r16,4
	bld	AFlags,afStopOnHS
	bst	r16,0
	bld	AFlags,afXForward
	bst	r16,1
	bld	AFlags,afYForward
	set
	bld	AFlags,afNewDir
	bst	r16,2
	bld	AFlags,afXMaster   ; Delay counter dt=0.003с
	ldi_w	r17,r16,48000
	std	Y+yDeltaTimeL,r16
	std	Y+yDeltaTimeH,r17
	std	Y+yCMP_PeriodT,ZeroReg  ; Init period tail as zero
	ldi	r16,4
	std	Y+yMasterStDivCnt,r16 ; Init microstep dividers
	std	Y+ySlaveStDivCnt,r16
;--- Debug ---
;	mov	r8,ZeroReg
;	mov	r9,ZeroReg
;	mov	r10,ZeroReg
;	mov	r11,ZeroReg
;-------------
	rjmp	cnp_11		  ; Exit
					; Continue line
cnp_12:	ldd	r18,Y+yDeltaTimeL	; Counter dt=0.003с
	ldd	r19,Y+yDeltaTimeH	; (period of step calculation)
	sub	r18,CMP_PeriodL
	sbc	r19,CMP_PeriodH
	brcs	cnp_1
	std	Y+yDeltaTimeL,r18
	std	Y+yDeltaTimeH,r19
	rjmp	cnp_11		; Exit proc
cnp_1:	subi_w	r19,r18,-48000	; 0.003с (333√ц)
	std	Y+yDeltaTimeL,r18
	std	Y+yDeltaTimeH,r19

	ldd	r18,Y+yAXChangeL 	; Select target period
	ldd	r19,Y+yAXChangeH
	cp	r16,r18
	cpc	r17,r19
	brsh	cnp_2
	ldd	r24,Y+yEndPeriodL	; Deacceleration stage
	ldd	r25,Y+yEndPeriodH
	rjmp	cnp_3
cnp_2:	ldd	r24,Y+yMidPeriodL	; Acceleration stage
	ldd	r25,Y+yMidPeriodH
cnp_3:	cp	r24,CMP_PeriodL		; Compare
	cpc	r25,CMP_PeriodH
	breq	cnp_10		; if equal -> Clear period tail and exit
	set
	brsh	cnp_4
	clt				;  T=1 -> increase, T=0 - decrease;
cnp_4:
	push	r0
	push	r1
	mul	CMP_PeriodH,CMP_PeriodH	; Calculate dT (first - T^2/256)
	movw	r19:r18,r1:r0
	mul	CMP_PeriodL,CMP_PeriodH
	mov	r17,r0
	add	r18,r1
	adc	r19,ZeroReg
	add	r17,r0
	adc	r18,r1
	adc	r19,ZeroReg
	mul	CMP_PeriodL,CMP_PeriodL
	add	r17,r1
	adc	r18,YH
	adc	r19,YH
	lsr16	r19,r18
	ror	r17
	lsr16	r19,r18	; T^2/256/4 - deacceleration
	ror	r17
	brts	cnp_5
	lsr16	r19,r18	; T^2/256/8 - acceleration
	ror	r17
cnp_5:	pop	r1
	pop	r0

	ldd	r16,Y+yCMP_PeriodT
	brtc	cnp_6
	add	r16,r17		; Increase period (deacceleration)
	adc	CMP_PeriodL,r18
	adc	CMP_PeriodH,r19
	brcs	cnp_7
	cp	CMP_PeriodL,r24
	cpc	CMP_PeriodH,r25
	rjmp	cnp_9
cnp_6:
	sub	r16,r17		; Decrease period (acceleration)
	sbc	CMP_PeriodL,r18
	sbc	CMP_PeriodH,r19
	brcs	cnp_7
	cp	r24,CMP_PeriodL
	cpc	r25,CMP_PeriodH
cnp_9:	brlo	cnp_8
cnp_7:	movw	CMP_PeriodH:CMP_PeriodL,r25:r24
cnp_10:	ldi	r16,0
	cpi	r25,$F0		; Stop after break complete
	brsh	cnp_15
cnp_8:	std	Y+yCMP_PeriodT,r16

cnp_11:	pop	r25
	pop	r24
	pop	r19
	pop	r18
	pop	r17
	pop	r16
	out	SREG,SSREG
	reti
cnp_15:
	std	Y+yMasterStCntL,ZeroReg
	std	Y+yMasterStCntH,ZeroReg
	std	Y+yDataCnt,ZeroReg
	std	Y+yDataTail,ZeroReg
	std	Y+yDataHead,ZeroReg
	set
	bld	AFlags,afFullStop
	ldi_w	r25,r24,$4000		; stop drives
	rjmp	cnp_7

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

PushSegFromPROM:
	ldd	r16,Y+yDataCnt
	cpi	r16,8
	brlo	ppr_1
	ret
ppr_1:	ldd	r16,Y+yDataHead
	mov	XL,r16
	inc	r16
	andi	r16,7
	std	Y+yDataHead,r16
	swap	XL
	ldi	r18,6
ppr_2:	lpm	r16,Z+
	lpm	r17,Z+
	st	X+,r17
	st	X+,r16
	loop	r18,ppr_2
	lpm	r16,Z
	st	X,r16
	cli
	ldd	r16,Y+yDataCnt
	inc	r16
	std	Y+yDataCnt,r16
	sei
	ret	
	

SEG1:	.dw	1476,586,23073,6153,23073,41, 0b111

