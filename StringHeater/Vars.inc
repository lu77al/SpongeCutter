.def	SSREG	= r2
.def	AFlags	= r3
	.equ	afADCComplete   = 0
	.equ	afStringShorted = 1
	.equ	afStringBroken  = 2

;.def	TimerCMPL = r4
;.def	TimerCMPH = r5
;.def	CMP_PeriodL = r6
;.def	CMP_PeriodH = r7

.def	BFlags	= r8
;	.equ	bfWaitForUartRep = 2	

#define	ZeroReg	YH

.equ	XSEG = $100
; --- Массивы ($100..$1FF)
.equ	RXBuf =		$00
.equ	TXBuf =		$20
.equ	ADCBuf	=	$40

;.equ	TimeBuf = $03
;.equ	ADCBuf	= $20
;.equ	InputBuf= $30
;.equ	DSReadBuf = $40
;.equ	DSResBuf  = $45 ; TL-TH-ER

.equ	YBUF = $60
; --- Переменные ($60..$9F)
.equ	yMainCntL	= $00
.equ	yMainCntM	= $01
.equ	yMainCntH	= $02
.equ	yRXPointer	= $03
.equ	yTXPointer	= $04
.equ	yTXEnd		= $05
.equ	yRXTimer	= $06
.equ	yRXTXCHS	= $07
.equ	yMyAddr		= $08
.equ	yADCCurCh	= $09
.equ	yADCSamCnt	= $0A
.equ	yADCSumL	= $0B
.equ	yADCSumH	= $0C

.equ	yPWML		= $10
.equ	yPWMH		= $11
.equ	yIzeroL		= $12
.equ	yIzeroH		= $13
.equ	yDestUL		= $14
.equ	yDestUH		= $15
.equ	yDestIL		= $16
.equ	yDestIH		= $17
.equ	yHeating	= $18
.equ	yNoRequestCnt	= $19
.equ	yFanBeforeCnt	= $1A
.equ	yFanAfterCnt	= $1B

.equ	yVoltage	= $1E
.equ	yMaxCur		= $1F










