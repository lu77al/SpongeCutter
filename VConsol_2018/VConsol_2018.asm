;--- Template with TACT, UART, ADC ---
.include "m8def.inc"
.include "Macro.inc"
.include "Pinout.inc"
.include "Vars.inc"

rjmp	RESET		; Reset
reti			; INT0
reti			; INT1
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

.include "LCD.inc"
.include "Cutter.inc"
.include "UART.inc"
.include "Tact.inc"
.include "SDCard.inc"
.include "Buttons.inc"
.include "Indicate.inc"
.include "Subs.inc"
.include "Menu.inc"
.include "Memory.inc"


RESET:

.include "Init.inc"

;	ldi_w	ZH,ZL,2*SEG1
;	rcall	PushSegFromPROM
;	ldi_w	ZH,ZL,2*SEG1
;	rcall	PushSegFromPROM

Main:
	sbrc	BFlags,bfReqCutter
	rcall	RequestCutter

	tskTact
RetTact:
	tskUart
RetUart:

	rjmp	Main


#define	UART_DIR_PORT	PORTD,2	; <- UartDir
#define	UART_DIR_DDR	 DDRD,2
;********** BootLoader V0.0 **********
.ORG	SECONDBOOTSTART
BootLoader:
	cli
;-- Порты --
	InitPorts
#ifdef	UART_DIR_PORT
	cbi	UART_DIR_PORT
	sbi	UART_DIR_DDR
#endif
;--- Стек и указатели ---
	outi	SPH,high(RAMEND)
	outi	SPL,low(RAMEND)
	ldi	XH,1
	ldi_w	YH,YL,$60
;--- Остановить таймеры ---
	out	TCCR1A,YH
	out	TCCR1B,YH
	out	TCCR0,YH
	out	TCCR2,YH
;-- Очиститка памяти с $100 по $1ff
	ldi_w	YH,YL,$60
	ldi_w	XH,XL,$100

	ldi_w	ZH,ZL,$60
bini_1:	st	Z+,YH
	cpi	ZH,2
	brlo	bini_1
;--- Инициализация UART ---
	out	UCSRA,YH
	outi	UCSRB,(1<<RXEN) | (1<<TXEN)	; Разрешить прием и передачу
	outi	UCSRC,0b10000110	; 8-бит
	outi	UBRRH,High(UBRRB_V)
	outi	UBRRL,Low(UBRRB_V)
Boot_continue:


