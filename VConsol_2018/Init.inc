;-- Init ports state --
	InitPorts

;-- Стек и указатели --
	outi	SPH,high(RAMEND)
	outi	SPL,low(RAMEND)
	ldi_w	YH,YL,$60
	ldi_w	XH,XL,$100

;-- Очиститка памяти с $100 по $1ff
	ldi_w	ZH,ZL,$60
ini_1:	st	Z+,YH
	cpi	ZH,2
	brlo	ini_1

	ldi_w	YH,YL,YBUF
	ldi_w	XH,XL,XSEG

	ldi	r18,2
	ldi_w	r17,r16,0
ini_2:	subi	r16,1
	sbci	r17,0
	sbci	r18,0
	brne	ini_2

	outi	SPCR,0b00000000	; Disable SPI
 	outi	SPSR,0b00000000

	clr	AFlags
	clr	BFlags

	rcall	InitTact

	rcall	InitUart

	rcall	InitLCD

	stdi	yKeyCode,$FF

	rcall	ReadMemory

	sei

;	rcall	PrepMove

