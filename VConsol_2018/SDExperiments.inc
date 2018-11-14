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
	cbi	SD_xCS
	stdi	yCMDRep,$FF
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
ccm_2:	std	Y+yCMDRep,r16
	cp	r17,r16
	breq	ccm_4
	loop	r19,ccm_5
	set
	bld	AFlags,afCardError
ccm_4:	sbi	SD_xCS
	ret

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
	ldd	r16,Y+yCMDRep
	std	Y+yCMD0Rep,r16
	sbrc	AFlags,afCardError
	rjmp	isd_2
	DelayDMS 100
	ldi	XL,CardBuf
	ldi	r16,1
	st	X+,r16
	rcall	CardCmd
	ldd	r16,Y+yCMDRep
	std	Y+yCMD1Rep,r16
	sbrc	AFlags,afCardError
	rjmp	isd_2
	pop	r16
	outi	SPCR,0b01010000	; Enable SPI_MASTER, Fr = F_CPU/4 = 2000000
 	outi	SPSR,0b00000000
	ret
isd_2:	pop	r16
	loop	r16,isd_3
	ret

.equ	yCMDRep		= $20
.equ	yCMD0Rep	= $21
.equ	yCMD1Rep	= $22



