;--- Template with TACT, UART, ADC ---
.include "m8def.inc"
.include "Macro.inc"
.include "Pinout.inc"
.include "Vars.inc"

rjmp	RESET		; Reset
reti			; INT0
rjmp	ShortStr	; INT1
reti			; TIMER2 COMP
reti			; TIMER2 OVF
reti			; TIMER1 CAPT
reti			; TIMER1 COMPA
reti			; TIMER1 COMPB
reti			; TIMER1 OVF
reti			; TIMER0 OVF
reti			; SPI,STC
reti			; UART, RX
reti			; UART, UDRE
reti			; UART, TX
reti			; ADC
reti			; EE_RDY
reti			; ANA_COMP
reti			; TWI
reti			; SPM_RDY

ShortStr:
	out	TCCR1A, ZeroReg
	out	TCCR1B, ZeroReg
	cbi	PORTB,1
	in	SSREG,SREG
	set
	bld	AFlags,afStringShorted
	out	SREG,SSREG
	out	MCUCR,ZeroReg
	out	GICR, ZeroReg
	outi	GIFR, 0b10000000
	cbi	PORTB,1
	reti

.include "ADC.inc"
.include "UART.inc"
.include "Tact.inc"
.include "Subs.inc"
.include "Stabilize.inc"

RESET:

.include "Init.inc"

Main:
	tskTact
RetTact:
	tskUart
RetUart:
	tskADC
RetADC:

	sbrc	AFlags,afADCComplete
	rcall	Stabilize

	rjmp	Main

InitPWM:
	outi	MCUCR,0b00001000
	outi	GIFR, 0b10000000
	outi	GICR, 0b10000000
	cbi	PORTB,1
	ldi_w	r17,r16,400
	out	ICR1H,r17
	out	ICR1L,r16
	ldi_w	r17,r16,400
	out	OCR1AH,r17
	out	OCR1AL,r16
	outi	TCCR1A, 1*(1<<COM1A1) | 1*(1<<COM1A0) | 1*(1<<WGM11) | 0*(1<<WGM10)
	outi	TCCR1B, 1*(1<<WGM13) | 1*(1<<WGM12) | 0*(1<<CS12) | 0*(1<<CS11) | 1*(1<<CS10)
	ret

DeInitPWM:
	out	MCUCR,ZeroReg
	out	GICR, ZeroReg
	outi	GIFR, 0b10000000
	ldi_w	r17,r16,400
	ldi_w	r17,r16,400
	out	OCR1AH,r17
	out	OCR1AL,r16
	out	TCCR1A, ZeroReg
	out	TCCR1B, ZeroReg
	cbi	PORTB,1
	ret


;********** BootLoader V0.0 **********
.ORG	SECONDBOOTSTART
BootLoader:

