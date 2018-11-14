.def	SSREG	= r2
.def	AFlags	= r3
	.equ	afXMaster   = 0
	.equ	afXForward  = 1
	.equ	afYForward  = 2
	.equ	afNewDir    = 3
	.equ	afFullStop  = 4
	.equ	afStopOnHS  = 5
	.equ	afMoveLong  = 6

.def	TimerCMPL = r4
.def	TimerCMPH = r5
.def	CMP_PeriodL = r6
.def	CMP_PeriodH = r7

#define	ZeroReg	YH

.equ	XSEG = $100
; --- Массивы ($100..$1FF)
.equ	RXBuf   = $00
.equ	TXBuf   = $80

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
.equ	yNoUartCnt	= $09

.equ	yMasterStCntL	= $10
.equ	yMasterStCntH	= $11
.equ	yMasterStDivCnt	= $12
.equ	ySlaveStDivL	= $13
.equ	ySlaveStDivH	= $14
.equ	ySlaveStSubL	= $15
.equ	ySlaveStSubH	= $16
.equ	ySlaveStBaseL	= $17
.equ	ySlaveStBaseH	= $18
.equ	ySlaveStDivCnt	= $19
.equ	yDeltaTimeL	= $1A
.equ	yDeltaTimeH	= $1B
.equ	yCMP_PeriodT	= $1C
.equ	yAXChangeL	= $1D
.equ	yAXChangeH	= $1E
.equ	yEndPeriodL	= $1F
.equ	yEndPeriodH	= $20
.equ	yMidPeriodL	= $21
.equ	yMidPeriodH	= $22
.equ	yDataTail	= $23
.equ	yDataHead	= $24
.equ	yDataCnt	= $25
.equ	yMasterPosL	= $26
.equ	yMasterPosH	= $27
