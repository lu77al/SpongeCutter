/* --- Atmega8 TQFP-32 ---
	PORT_B
12 ICP1		PORTB,0	- 
13 IC1A		PORTB,1	- PWM (OC1A)
14 SS^/OC1B	PORTB,2	- 
15 MOSI/OC2	PORTB,3	- 
16 MISO		PORTB,4	- 
17 SCK		PORTB,5	- 
07 XTAL1/TOSC1	PORTB,6	- __XTAL1__
08 XTAL2/TOSC2	PORTB,7	- __XTAL2__
	PORT_C
23 ADC0		PORTC,0	- 
24 ADC1		PORTC,1	- 
25 ADC2		PORTC,2	- Iin
26 ADC3		PORTC,3	- Uout
27 ADC4/SDA	PORTC,4	- 
28 ADC5/SCL	PORTC,5	- 
29 ^RESET	PORTC,6	- __^RESET__
	PORT_D
30 RXD		PORTD,0	- RXD
31 TXD		PORTD,1	- TXD
32 INT0		PORTD,2	- DataDir
01 INT1		PORTD,3	- String shorted
02 XCK/T0	PORTD,4	- String broken
09 T1		PORTD,5	- 
10 AIN0		PORTD,6	- LED
11 AIN1		PORTD,7	- Fans
*/

; --- Initial port state ---
.macro	InitPorts
	outi	DDRB, 0b000010
	outi	PORTB,0b111101
	outi	DDRC, 0b000000
	outi	PORTC,0b110011
	outi	DDRD, 0b11000100
	outi	PORTD,0b00100000
.endm

;--- <UART SETTINGS> ---
#define	UART_DIR_PORT	PORTD,2	 ; <- UartDir
#define	UART_DIR_DDR	 DDRD,2
.equ	CPU_FREQ      = 8000000  ; <- FCPU
.equ	UART_RATE     = 19200	 ; <- BAUD RATE
.equ	UBRRB_V       = (CPU_FREQ/(16*UART_RATE)-1)
.equ	DevAddr       = $35
.equ	TactFreq      = 7812
.equ	UARTMesDelay  = 4*10*TactFreq/UART_RATE ; 4 Bytes
.equ	UARTByteDelay = 2*10*TactFreq/UART_RATE ; 2 Bytes
.equ	UARTStartByte = $14
.equ	RXBufLen      = 32
;--- </UART SETTINGS> ---

#define	LED	PORTD,6
#define	FAN	PORTD,7 


