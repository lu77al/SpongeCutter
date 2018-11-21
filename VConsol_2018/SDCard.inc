.macro	MSwapSPI
	out	SPDR,r16
	sbis	SPSR,7
	rjmp	PC-1
	in	r16,SPDR
.endm

.macro	MSwapSPI_I
	ldi	r16,@0
	out	SPDR,r16
	sbis	SPSR,7
	rjmp	PC-1
	in	r16,SPDR
.endm

.macro	DelayDMS
	ldi	r16,LOW((CPU_FREQ/40000*@0)-1)
	ldi	r17,HIGH((CPU_FREQ/40000*@0)-1)
	rcall	Delay2B
.endm

.macro	DelayMS
	ldi	r16,LOW((CPU_FREQ/5000*@0)-1)
	ldi	r17,HIGH((CPU_FREQ/5000*@0)-1)
	ldi	r18,BYTE3((CPU_FREQ/5000*@0)-1)
	rcall	Delay3B
.endm

.macro	MReadByte
	ldi	r16,$FF
	out	SPDR,r16
	in	r16,SPSR
	sbrs	r16,7
	rjmp	PC-2
	in	r16,SPDR
	subi	r24,1
	sbci	r25,0
.endm

.macro	ClearReadReq
	rcall	ReadByte
	brpl	PC-1
.endm

Delay2B:
	subi_w	r17,r16,1
	brcc	Delay2B
	ret

Delay3B:
	subi_w	r17,r16,1
	sbci	r18,0
	brcc	Delay3B
	ret

ReadByte:
	MReadByte
	ret

SwapSPI:
	MSwapSPI
	ret


/* --- CardCmd (8 bytes)---
 *  - perfom 3 empty SPI exchange to empty card output queue
 *  - send 8 byte command from CardBuf
 *  - set bit 46 and bit 0 to 1
 *  - adds CRC7 for CMD0
 *  - waits for response (50 bytes)
 *  - repeat attempt 30 times
 *  - sets afCardError if there is no correct respose
 *  Command format (bits - value / description
 *   47     - fixed - "0"
 *   46     - fixed - "1"
 *   45..40 - CMD number
 *   39..8  - data (argument)	
 *   7..1   - CRC 7    - $95 with stop bit
 *   0      - stop bit - "0"
 * ------------------------ */
CardCmd: 
	ldi	r19,30
ccm_5:	ldi	r16,$FF	
	rcall	SwapSPI
	ldi	r16,$FF
	rcall	SwapSPI
	ldi	r16,$FF
	rcall	SwapSPI
	ldi	XL,CardBuf
	ldi	r17,0
	ld	r16,X+
	cpi	r16,0
	brne	ccm_6
	ldi	r17,1
ccm_6:	ori	r16,$40
	MSwapSPI
ccm_1:	ld	r16,X+
	MSwapSPI
	cpi	XL,CardBuf+5
	brlo	ccm_1
	MSwapSPI_I $95
	ldi	r18,50
ccm_3:	ldi	r16,$FF
	MSwapSPI
	cpi	r16,$FF
	brne	ccm_2
	loop	r18,ccm_3
ccm_2:	cp	r17,r16
	breq	ccm_4
	loop	r19,ccm_5
	set
	bld	AFlags,afCardError
ccm_4:	ret

/* --- Card initialization ---
 *  - 10 attempts
 *  - setup SPI 125 kHz
 *  - 12 empty SPI swaps $FF with xCS = 1
 *  - xCS = 0 (select)
 *  - CMD0 (reset)
 *  - CMD1 (init)
 *  - setup SPI 2MHz
 * --------------------------- */
InitCard:
	ldi	r16,10
isd_3:	push	r16
	DelayDMS 70
	outi	SPCR,0b01010010	; Enable SPI_MASTER, Fr = F_CPU/64 = 125000
 	outi	SPSR,0b00000000
	sbi	SD_xCS
	DelayDMS 30
	ldi	r17,12
isd_1:	ldi	r16,$FF
	rcall	SwapSPI
	loop	r17,isd_1
	DelayDMS 15
	cbi	SD_xCS
	DelayDMS 15
	clt
	bld	AFlags,afCardError
	ldi	XL,CardBuf
	st	X+,ZeroReg
	st	X+,ZeroReg
	st	X+,ZeroReg
	st	X+,ZeroReg
	st	X+,ZeroReg
	rcall	CardCmd
	sbrc	AFlags,afCardError
	rjmp	isd_2
	DelayDMS 100
	ldi	XL,CardBuf
	ldi	r16,1
	st	X+,r16
	rcall	CardCmd
	sbrc	AFlags,afCardError
	rjmp	isd_2
	pop	r16
	outi	SPCR,0b01010000	; Enable SPI_MASTER, Fr = F_CPU/4 = 2000000
 	outi	SPSR,0b00000000
	ret
isd_2:	pop	r16
	loop	r16,isd_3
	ret


SetBlock: ;  r25:r24 - длина
	ldi	XL,CardBuf
	ldi	r16,16
	st	X+,r16
	st	X+,YH
	st	X+,YH
	st	X+,r25
	st	X,r24
	rjmp	CardCmd

ReadCard: ; r23:r22:r21:r20 - адрес,  r25:r24 - длина
	rcall	SetBlock
	sbrc	AFlags,afCardError
	ret
ReadBlock:
	ldi	XL,CardBuf
	ldi	r16,17
	st	X+,r16
	st	X+,r23
	st	X+,r22
	st	X+,r21
	st	X,r20
	rcall	CardCmd
	sbrc	AFlags,afCardError
	ret
	ldi_w	r18,r17,5000
rcrd_2:	ldi	r16,$FF
	rcall	SwapSPI
	cpi	r16,$FE
	breq	rcrd_1
	subi_w	r18,r17,1
	brne	rcrd_2
	set
	bld	AFlags,afCardError
rcrd_1:	subi_w	r25,r24,-2
	ret

ReadHeader:
	clt
	bld	AFlags,afItemNoEmpty
	ldd	r16,Y+yCurCutNum
	ldi	r17,$28	
	mul	r17,r16
	movw	r23:r22,r1:r0
	ldi_w	r21,r20,0	; јдрес €чейки
	ldi_w	r25,r24,68	; ƒлина заголовка
	rcall	ReadCard
	sbrc	AFlags,afCardError
	ret
	ldi_w	ZH,ZL,2*Marker
	ldi	r19,0
	ldi	r18,12
rh_2:	rcall	ReadByte
	add	r19,r16
	lpm	r17,Z+
	cp	r17,r16
	brne	rh_1
	loop	r18,rh_2
	ldi	XL,DescrBuf
rh_3:	rcall	ReadByte
	add	r19,r16
	st	X+,r16
	cpi	XL,DescrBuf+40
	brlo	rh_3
	rcall	ReadByte
	add	r19,r16
	mov	r18,r16
	rcall	ReadByte
	add	r19,r16
	mov	r17,r16
	rcall	ReadByte
	add	r19,r16
	cpi	r16,2
	cpc	r17,YH
	cpc	r18,YH
	brlo	rh_1
	cpi	r16,$AD
	ldi	r20,$13
	cpc	r17,r20
	ldi	r20,$03
	cpc	r18,r20
	brsh	rh_1
	std	Y+ySegCntB0,r16
	std	Y+ySegCntB1,r17
	std	Y+ySegCntB2,r18
	ldi	r20,12
rh_4:	rcall	ReadByte
	add	r19,r16
	loop	r20,rh_4
	rcall	ReadByte
	cp	r16,r19
	brne	rh_1
	set
	bld	AFlags,afItemNoEmpty
rh_1:	ClearReadReq
	ret


StartWrite:
	ldi	XL,CardBuf
	ldi	r16,24
	st	X+,r16
	st	X+,r25
	st	X+,r24
	st	X+,r23
	st	X,r22
	rcall	CardCmd
	sbrc	AFlags,afCardError
	ret
	ldi	r16,$FF
	rcall	SwapSPI
	ldi	r16,$FF
	rcall	SwapSPI
	ldi	r16,$FE
	rcall	SwapSPI
	ret


FinishWrite:
	ldi_w	r17,r18,3000
fwr_2:	ldi	r16,$FF
	rcall	SwapSPI
	cpi	r16,$FF
	brne	fwr_1
	subi_w	r18,r17,1
	brne	fwr_2
fwr_1:	ldi_w	r17,r18,4000
fwr_4:	ldi	r16,$FF
	rcall	SwapSPI
	cpi	r16,$00
	brne	fwr_3
	subi_w	r18,r17,1
	brne	fwr_4
fwr_3:	ret

FormatCard:
	rcall	InitCard
	sbrc	AFlags,afCardError
	ret
	ldi_w	r25,r24,512
	rcall	SetBlock
	sbrc	AFlags,afCardError
	ret
	ldi_w	r25,r24,0
	ldi_w	r23,r22,0
	ldi	r21,20
fcd_2:	rcall	StartWrite
	sbrc	AFlags,afCardError
	ret
	ldi_w	ZH,ZL,EmptyHeader*2
	ldi_w	r18,r17,512
fcd_1:	lpm	r16,Z+
	MSwapSPI
	subi_w	r18,r17,1
	brne	fcd_1
	rcall	FinishWrite
	subi_w	r25,r24,-$28
	loop	r21,fcd_2
	ret

InitRead:
	sbrs	AFlags,AFCutting
	sbrs	AFlags,AFItemNoEmpty
	ret
	set
	bld	AFlags,afCutting
	ldd	r16,Y+ySegCntB0
	ldd	r17,Y+ySegCntB1
	ldd	r18,Y+ySegCntB2
	std	Y+ySegLastB0,r16
	std	Y+ySegLastB1,r17
	std	Y+ySegLastB2,r18
	ldi	r25,13
	mul	r16,r25
	movw	r21:r20,r1:r0
	ldi	r22,0
	mul	r17,r25
	add	r21,r0
	adc	r22,r1
	mul	r18,r25
	add	r22,r0
	ldi_w	r25,r24,444
	cp	r20,r24
	cpc	r21,r25
	cpc	r22,YH
	brsh	ird_1
	movw	r25:r24,r21:r20
ird_1:	sub	r20,r24
	sbc	r21,r25
	sbc	r22,YH
	std	Y+yCutByteCntB0,r20
	std	Y+yCutByteCntB1,r21
	std	Y+yCutByteCntB2,r22
	std	Y+yBlockCntL,r24
	std	Y+yBlockCntH,r25
	ldd	r16,Y+yCurCutNum
	ldi	r17,$28
	mul	r17,r16
	movw	r23:r22,r1:r0
	ldi_w	r21,r20,68	; јдрес €чейки
	std	Y+ySDAddrB0,r21
	std	Y+ySDAddrB1,r22
	std	Y+ySDAddrB2,r23
	rcall	ReadCard
	sbrc	AFlags,afCardError
	rjmp	StopCutting
	stdi	ySegInBuf,8
	ret

ReadNextItem:
	ldi	ZH,1
	ldi	ZL,TXBuf+4
	ldi	r16,$01
	st	Z+,r16
	ldi	r16,$58
	mov	r15,r16
rni_4:	ldd	r20,Y+yCutByteCntB0
	ldd	r21,Y+yCutByteCntB1
	ldd	r22,Y+yCutByteCntB2
	ldd	r24,Y+yBlockCntL
	ldd	r25,Y+yBlockCntH
rni_3:	subi_w	r25,r24,1
	brcc	rni_1
	subi_w	r22,r21,2
	brcc	rni_2
	movw	r25:r24,r21:r20
	ldi_w	r21,r22,0
	ldi	r22,0
	subi	r25,-2
	rjmp	rni_6
rni_2:	ldi_w	r25,r24,512
rni_6:	std	Y+yBlockCntL,r24
	std	Y+yBlockCntH,r25
	std	Y+yCutByteCntB0,r20
	std	Y+yCutByteCntB1,r21
	std	Y+yCutByteCntB2,r22
	ldd	r21,Y+ySDAddrB0
	ldd	r22,Y+ySDAddrB1
	ldd	r23,Y+ySDAddrB2
	subi_w	r22,r21,-2
	sbci	r23,$FF
	std	Y+ySDAddrB0,r21
	std	Y+ySDAddrB1,r22
	std	Y+ySDAddrB2,r23
	ldi	r20,0
	rcall	ReadCard
	sbrc	AFlags,afCardError
	rjmp	StopCutting
	rjmp	rni_4
rni_1:	MSwapSPI_I $FF
	add	r15,r16
	st	Z+,r16
	cpi	ZL,TXBuf+18
	brlo	rni_3
	dec	ZL
	sub	r15,r16
	mov	r17,r15
	eor	r17,r16
	andi	r17,0b11111000
	breq	rni_5
	set
	bld	AFlags,afCardError
	rjmp	StopCutting
rni_5:	andi	r16,0b111
	st	Z+,r16
	std	Y+yBlockCntL,r24
	std	Y+yBlockCntH,r25
	ret


StartCutting:
	std	Y+yProgress,ZeroReg
	rcall	InitCard
	sbrc	AFlags,afCardError
	ret
	rcall	ReadHeader
	set
	sbrs	AFlags,afItemNoEmpty
	bld	AFlags,afCardError
	sbrc	AFlags,afCardError
	ret
	rcall	InitRead
	sbrc	AFlags,afCardError
	ret
	stdi	yInitCutCnt,150
	ret

Marker:	.db	"@VALEROPILKA"
;Marker:	.db	"@VALErOPILKA"

ReadCutting:
	push	r23
	rcall	InitCard
	pop	r23
	sbrc	AFlags,afCardError
	ret
	ldi	r25,20
rct_2:	push	r25
	push	r23
	rcall	ReadHeader
	pop	r23
	pop	r25
	sbrs	AFlags,afCardError
	sbrc	AFlags,afItemNoEmpty
	ret
	cpi	r23,2
	breq	rct_4
	ldd	r16,Y+yCurCutNum
	cpi	r23,0
	breq	rct_3
	subi	r16,1
	brcc	rct_1
	ldi	r16,19
	rjmp	rct_1
rct_3:	inc	r16
	cpi	r16,20
	brlo	rct_1
	ldi	r16,0
rct_1:	std	Y+yCurCutNum,r16
	loop	r25,rct_2
rct_4:	set
	bld	AFlags,afCardError
	ret

StopCutting:
	sbrs	AFlags,afCutting
	ret
	clt
	bld	AFlags,afHeating	
	stdi	yInitCutCnt,5
	clt
	bld	AFlags,afCutting
	ldd	r24,Y+yBlockCntL
	ldd	r25,Y+yBlockCntH
	subi_w	r25,r24,-3
stc_1:	ldi	r16,$FF
	rcall	SwapSPI
	subi_w	r25,r24,1
	brne	stc_1
	ldd	r16,Y+yProgress
	cpi	r16,50
	brlo	stc_2
	lds	r16,Sheets
	lds	r17,Sheets100L
	lds	r18,Sheets100H
	inc	r16
	cpi	r16,100
	brlo	stc_3
	ldi	r16,0
	subi_w	r18,r17,-1
	brcc	stc_2
stc_3:	sts	Sheets,r16
	sts	Sheets100L,r17
	sts	Sheets100H,r18
	andi	r16,0b11
	brne	stc_2
	rjmp	SaveMemory
stc_2:	ret

EmptyHeader:
.db	$40,$56,$41,$4C,$45,$52,$4F,$50,$49,$4C,$4B,$41,$2A,$2A,$2A,$2A
.db	$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A
.db	$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A,$2A
.db	$2A,$2A,$2A,$2A,$00,$00,$00,$AA,$AA,$AA,$AA,$AA,$AA,$AA,$AA,$AA
.db	$AA,$AA,$AA,$02,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00
.db	$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00,$00



