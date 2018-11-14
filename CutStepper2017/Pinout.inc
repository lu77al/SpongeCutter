; --- Atmega8 16 ÌÃö ---
;		 _____________     _____________
;		 |	      |___|	       |
;		-| 1  Reset 	   ADC5 PC5 28 |- LS_3
;	    RXD	-| 2  PD0 RXD	   ADC4 PC4 27 |- LS_4
;	    TXD	-| 3  PD1 TXD	   ADC3 PC3 26 |- LS_5
;      UART_DIR	-| 4  PD2 INT0	A  ADC2 PC2 25 |- LS_6
;	   LS_2	-| 5  PD3 INT1	t  ADC1 PC1 24 |- XY_ENABLE
;	   LS_1	-| 6  PD4 T0	m  ADC0 PC0 23 |- Y_DIR
;		-| 7  VCC	e      AGND 22 |-
;		-| 8  GND	g      AREF 21 |-
;		-| 9  XTAL1	a      AVCC 20 |-
;		-| 10 XTAL2	    SCK	PB5 19 |- KEY_3
;	  KEY_2	-| 11 PD5 T1	8  MISO	PB4 18 |- KEY_4
;	  KEY_1	-| 12 PD6 AIN0+	   MOSI	PB3 17 |- KEY_5
;	    LED	-| 13 PD7 AIN1-	    ^SS	PB2 16 |- X_STEP
;	  X_DIR	-| 14 PB0 ICP	    0C1	PB1 15 |- Y_STEP
;		 |_____________________________|

.macro	InitPorts
	outi	PORTB,0b000001
	outi	DDRB, 0b111111
	outi	PORTC,0b000011
	outi	DDRC, 0b000011
	outi	PORTD,0b00000000
	outi	DDRD, 0b11100100
.endm

;--- <UART SETTINGS> ---
#define	UART_DIR_PORT	PORTD,2	 ; <- UartDir
#define	UART_DIR_DDR	DDRD,2
.equ	CPU_FREQ      = 16000000 ; <- FCPU
.equ	UART_RATE     = 19200	 ; <- BAUD RATE
.equ	UBRRB_V       = (CPU_FREQ/(16*UART_RATE)-1)
.equ	DevAddr       = $4A
.equ	TactFreq      = 7812
.equ	UARTMesDelay  = 4*10*TactFreq/UART_RATE ; 4 Bytes
.equ	UARTByteDelay = 2*10*TactFreq/UART_RATE ; 2 Bytes
.equ	UARTStartByte = $14
.equ	RXBufLen      = 32
;--- </UART SETTINGS> ---

;--- <STEPPERS PIN> ---
#define	XY_Enable	PORTC,1
#define	X_DIR		PORTB,0
#define	X_STEP		PORTB,2
#define	Y_DIR		PORTC,0
#define	Y_STEP		PORTB,1
#define EnableSteppers	cbi XY_Enable
#define DisableSteppers	sbi XY_Enable
#define ForwardX	sbi X_DIR
#define BackwardX	cbi X_DIR
#define	PrepareX	sbi X_STEP
#define	StepX		cbi X_STEP
#define ForwardY	sbi Y_DIR
#define BackwardY	cbi Y_DIR
#define	PrepareY	sbi Y_STEP
#define	StepY		cbi Y_STEP
;--- </STEPPERS PIN> ---
#define	LED_PIN		PORTD,7

