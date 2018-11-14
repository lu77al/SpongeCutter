.def	SSREG	= r2
.def	AFlags	= r3
	.equ	afCardError	= 0
	.equ	afNoStringRep	= 1
	.equ	afHeating	= 2
	.equ	afReverseY	= 3
	.equ	afHandMoving	= 4
	.equ	afCutting	= 5
	.equ	afItemNoEmpty	= 6
	.equ	afNoStepperRep	= 7


.def	TimerCMPL = r4
.def	TimerCMPH = r5
.def	CMP_PeriodL = r6
.def	CMP_PeriodH = r7

.def	BFlags	= r8
	.equ	bfReqCutter	 = 0
	.equ	bfSearchStart	 = 1
;	.equ	bfWaitForUartRep = 2	



#define	ZeroReg	YH

.equ	XSEG = $100
; --- Массивы ($100..$1FF)
.equ	RXBuf =		$00
.equ	TXBuf =		$10
.equ	CardBuf	=	$30
.equ	DescrBuf =	$37
.equ	LCDBuf =	$60


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
.equ	yCurScreen	= $09
.equ	yBakScreen	= $0A
.equ	yStepperRep	= $0B
.equ	yKBRDHash	= $0C

.equ	yLCDPointer	= $10
.equ	yLCDCnt		= $11
.equ	yBakButStateL	= $12
.equ	yBakButStateH	= $13
.equ	yBakButCnt	= $14
.equ	yButStateL	= $15
.equ	yButStateH	= $16
.equ	yButPressL	= $17
.equ	yButPressH	= $18
.equ	yButRepL	= $19
.equ	yButRepH	= $1A
.equ	yButCnt		= $1B
.equ	yButCntStart	= $1C
.equ	yKeyCode	= $1D
.equ	yLastKeyCode	= $1E
.equ	ySoundCnt	= $1F
.equ	yFinishSoundCnt	= $20
.equ	yCurCutNum	= $21
.equ	yHeatRate	= $22
.equ	ySpeed		= $23
.equ	yErrorCode	= $24
.equ	ySegLastB0	= $25
.equ	ySegLastB1	= $26
.equ	ySegLastB2	= $27
.equ	ySegCntB0	= $28
.equ	ySegCntB1	= $29
.equ	ySegCntB2	= $2A
.equ	yProgress	= $2B
.equ	yInitCutCnt	= $2C
.equ	ySegInBuf	= $2D
.equ	yNextCommand	= $2E
.equ	yPassCnt	= $2F
.equ	ySpeedKoefL	= $30
.equ	ySpeedKoefH	= $31
.equ	yBlockCntL	= $32
.equ	yBlockCntH	= $33
.equ	ySDAddrB0	= $34
.equ	ySDAddrB1	= $35
.equ	ySDAddrB2	= $36
.equ	yCutByteCntB0	= $37
.equ	yCutByteCntB1	= $38
.equ	yCutByteCntB2	= $39
.equ	yNoStepRepCnt	= $3A
.equ	yCutReqCnt	= $3B
.equ	yHeaterRepCnt	= $3C
.equ	ySearchCnt	= $3D
.equ	yHeatingCnt	= $3E
.equ	yHeaterByte	= $3F


.equ	MinCnt		= $A0
.equ	Minutes		= $A1
.equ	HoursL		= $A2
.equ	HoursH		= $A3
.equ	StringsL	= $A4
.equ	StringsH	= $A5
.equ	Sheets		= $A6
.equ	Sheets100L	= $A7
.equ	Sheets100H	= $A8
.equ	NoHeaterRepCnt	= $A9

