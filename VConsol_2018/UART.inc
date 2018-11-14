InitUart:
	outi	UCSRA,0
	outi	UCSRB,0b00011000	; Разрешить прием и передачу
	outi	UCSRC,0b10000110	; 8-бит
	outi	UBRRH,High(UBRRB_V)
	outi	UBRRL,Low(UBRRB_V)
	stdi	yTXPointer,TXBuf
	std	Y+yTXEnd,r16
	stdi	yRXPointer,RXBuf-1
	std	Y+yRXTimer,ZeroReg
#ifdef	UART_DIR_PORT
	cbi	UART_DIR_PORT
	sbi	UART_DIR_DDR
#endif
	stdi	yMyAddr,DevAddr
	std	Y+yRXTXCHS,ZeroReg
	ret
/*
.macro	macRXTimer
	ldd	r16,Y+yRXTimer
	inc	r16
	breq	rxtm_1
	std	Y+yRXTimer,r16
	cpi	r16,UARTByteDelay
	brne	rxtm_1
	stdi	yRXPointer,RXBuf-1	
rxtm_1:
.endm
*/

; Обработчик USART в основном потоке
.macro	tskUart
	ldd	XL,Y+yTXPointer
	ldd	r17,Y+yTXEnd
	cp	XL,r17
	breq	uart_1
	sbis	UCSRA,UDRE ; ++ Обработка в режиме передачи
	rjmp	RetUart  ; выйти если буфер передачи занят
#ifdef	UART_DIR_PORT
	sbi	UART_DIR_PORT  ; переключить микросхему на передачу
#endif
	ld	r16,X+
	cp	XL,r17
	brne	uart_2
	ldd	r16,Y+yRXTXCHS
uart_2:	out	UDR,r16   ; отправить очереждной байт в буфер
	std	Y+yTXPointer,XL
	ldd	r17,Y+yRXTXCHS
	add	r17,r16
	std	Y+yRXTXCHS,r17
	sbi	UCSRA,TXC ; обнуление флага для корректного перехода на прием
	rjmp	RetUart
uart_1:
#ifdef	UART_DIR_PORT
	sbic	UCSRA,TXC ; ++ Обработка в режиме приема
	cbi	UART_DIR_PORT  ;  переключить микросхему на прием после передачи крайнего байта
#endif
	sbic	UCSRA,RXC  ; выйти из обработчика если нет принятого байта
	rjmp	ReadUart   ; обработать принятый байт
.endm

; -1 00 01 02 03 .. NN
; ST LN DN SR D0 .. CS
ReadUart:
	in	r16,UDR
	ldd	r17,Y+yHeaterRepCnt
	subi	r17,1
	brcs	rx_11
	std	Y+yRXTimer,ZeroReg
	std	Y+yHeaterRepCnt,r17
	cpi	r17,0
	brne	rx_12

	sbrs	AFLags,afCutting
	rjmp	rx_14
	ldd	r18,Y+yInitCutCnt
	cpi	r18,0
	brne	rx_14
	set
	bld	BFlags,bfReqCutter
	ldi	r18,5
	std	Y+yCutReqCnt,r18
rx_14:


	ldd	r17,Y+yHeaterByte
	com	r17
	cp	r16,r17
	brne	rx_13

	sts	NoHeaterRepCnt,ZeroReg
	clt
	bld	AFlags,afNoStringRep

	ldd	r16,Y+yHeaterByte
	andi	r16,0b11
	breq	rx_13

	ldd	r17,Y+yCurScreen
	cpi	r17,4
	breq	rx_13

	ldi	r25,5
	sbrs	r16,1
	rjmp	str_1
	ldi	r25,2
	lds	r16,StringsL
	lds	r17,StringsH
	subi_w	r17,r16,-1
	brcc	str_1
	sts	StringsL,r16
	sts	StringsH,r17
str_1:	
	std	Y+yErrorCode,r25
	stdi	yCurScreen,4
	stdi	yNextCommand,cmStopSmooth
	rcall	StopCutting

rx_13:	rjmp	RetUart
rx_12:	std	Y+yHeaterByte,r16
	rjmp	RetUart

rx_11:	ldd	XL,Y+yRXPointer
	cpi	XL,RXBuf-1
	breq	rx_1
	cpi	XL,RXBuf
	breq	rx_2
	lds	r17,XSEG+RXBuf	; ++ чтение байт 01...
	cp	XL,r17		; проверка на завершение посылки
	breq	rx_3
rx_8:	st	X+,r16		; сохраненить очередного байта в буфере
	ldd	r17,Y+yRXTXCHS
	add	r17,r16
	std	Y+yRXTXCHS,r17
rx_5:	std	Y+yRXPointer,XL
rx_4:	std	Y+yRXTimer,ZeroReg
	rjmp	RetUart
rx_1:	cpi	r16,UARTStartByte ; ++ чтение байта -1
	brne	rx_4		;  проверка на корректный страт-байт
	ldd	r16,Y+yRXTimer
	cpi	r16,UARTMesDelay
	brlo	rx_4		;  проверка на минимальную задержку перед посылкой
	stdi	yRXTXCHS,UARTStartByte-(RXBuf+3)
	inc	XL
	rjmp	rx_5
rx_2:	cpi	r16,1		; ++ чтение байта 00 (длина)
	brlo	rx_6		;  проверка на минимальную длину
	cpi	r16,RXBufLen-2
	brlo	rx_7		;  проверка на максимальную длину
rx_6:	ldi	XL,RXBuf-1	; Сброс буфера при ошибочной длине
	rjmp	rx_5
rx_7:	subi	r16,-(RXBuf+3)	;  Формирование адреса конца посылки
	rjmp	rx_8
rx_3:		; ++ Принята вся посылка ++
rx_9:	ldd	r17,Y+yRXTXCHS ; проверка контрольной суммы
	cp	r16,r17
	brne	rx_6	; сброс буфера при неверной сумме
	stdi	yRXTimer,UARTMesDelay+1	; Таймер готов к принятию новой посылки
		; ++ Обработка проверенной посылки
	stdi	yTXPointer,TXBuf+4
	ldi	XL,RXBuf+1
	ld	r16,X+	; Адрес получателя
	ld	r17,X+	; Адрес отправителя
	std	Y+yRXPointer,XL
	ldd	r18,Y+yMyAddr ; *** Выбор обработчика команд
	cp	r16,r18
	breq	MyCMDProcess
	andi	r18,$F0
	cp	r16,r18
	breq	MyCMDProcess
				; Не найдено подходящего обработчика
rx_10:	stdi	yTXPointer,TXBuf
	std	Y+yTXEnd,r16
	stdi	yRXPointer,RXBuf-1
	rjmp	RetUart
RetCMDProc:
	ldd	XL,Y+yTXPointer	; Доформирование ответа при наличии данных
	cpi	XL,TXBuf+4
	breq	rx_10
	rcall	ComplUartMes
	rjmp	RetUart

ComplUartMes:
	mov	r20,XL
	subi	r20,TXBuf+4
	inc	XL
	std	Y+yTXEnd,XL
	ldi	XL,TXBuf
	std	Y+yTXPointer,XL
	ldi	r18,UARTStartByte
	st	X+,r18
	st	X+,r20
	st	X+,r16
	st	X+,r17
	stdi	yRXPointer,RXBuf-1
	std	Y+yHeaterRepCnt,ZeroReg	
	std	Y+yRXTXCHS,ZeroReg
	ret

MyCMDProcess:
	ldd	XL,Y+yRXPointer
cm_S2:
	ld	r16,X+
	std	Y+yRXPointer,XL

	cpi	r16,$00		; *** StateRequest ***
	brne	cm0_0
	ldd	XL,Y+yTXPointer
	ldi	r16,$80
	st	X+,r16
	st	X+,AFlags
	std	Y+yTXPointer,XL
	rjmp	cm_S4
cm0_0:

	cpi	r16,$01		; *** Init SD ***
	brne	cm1_0
	rcall	InitCard
	outi	SPCR,0b00000000	; Disable SPI
 	outi	SPSR,0b00000000
	ldd	XL,Y+yTXPointer
	ldi	r16,$81
	st	X+,r16
	st	X+,AFlags
	std	Y+yTXPointer,XL
	rjmp	cm_S4
cm1_0:

	cpi	r16,$02		; *** Format SD ***
	brne	cm2_0
	rcall	FormatCard
	outi	SPCR,0b00000000	; Disable SPI
 	outi	SPSR,0b00000000
	ldd	XL,Y+yTXPointer
	ldi	r16,$82
	st	X+,r16
	st	X+,AFlags
	std	Y+yTXPointer,XL
	rjmp	cm_S4
cm2_0:

	cpi	r16,$80		; *** Stepper responce ***
	brne	cm80_0
	ld	r16,X+
	std	Y+yRXPointer,XL
	std	Y+yStepperRep,r16
	andi	r16,$3F
	std	Y+ySegInBuf,r16
	std	Y+yNoStepRepCnt,ZeroReg
	clt
	bld	AFlags,afNoStepperRep
/*
	sbrs	AFLags,afCutting
	rjmp	cm_S4
	ldd	r16,Y+yInitCutCnt
	cpi	r16,0
	brne	rx_141
	set
	bld	BFlags,bfReqCutter
	stdi	yCutReqCnt,5
rx_141:
*/
	stdi	yHeaterRepCnt,2
	rjmp	cm_S4
cm80_0:

	cpi	r16,$70 ; *** Запрос участка памяти ***
	brne	cm70_0
	ld	ZH,X+	; ZH[2..0]:ZL - указатель на первый байт
	ld	ZL,X+	; старшие 5 бит ZH - длина участка
	std	Y+yRXPointer,XL
	ldd	XL,Y+yTXPointer
	ldi	r17,$70+$80
	st	X+,r17
	mov	r25,ZH
	andi	ZH,0b111
	lsr	r25
	lsr	r25
	lsr	r25
	breq	cm70_1
cm70_2:	ld	r16,Z+
	st	X+,r16
	loop	r25,cm70_2
cm70_1:	std	Y+yTXPointer,XL
	rjmp	cm_S4
cm70_0:
	cpi	r16,$71 ; *** Запись участка памяти ***
	brne	cm71_0
	ld	ZH,X+	; ZH[2..0]:ZL - указатель на первый байт
	ld	ZL,X+	; старшие 5 бит ZH - длина участка
	mov	r25,ZH
	andi	ZH,0b111
	lsr	r25
	lsr	r25
	lsr	r25
	breq	cm71_1
cm71_2:	ld	r16,X+
	st	Z+,r16
	loop	r25,cm71_2
cm71_1:	std	Y+yRXPointer,XL
	rjmp	cm_S4
cm71_0:
	cpi	r16,$72 ; *** Изменение байта памяти побитово ***
	brne	cm72_0
	ld	ZH,X+	; ZH:ZL - указатель на байт
	ld	ZL,X+
	ld	r16,X+	; AND_Mask
	ld	r17,X+	; OR_Mask
	ld	r18,X+	; XOR_Mask
	std	Y+yRXPointer,XL
	cli
	ld	r19,Z
	and	r19,r16
	or	r19,r17
	eor	r19,r18
	st	Z,r19
	sei
	rjmp	cm_S4
cm72_0:

	cpi	r16,$7F		; *** Переход к загрузчику ***
	brne	cm7F_0
	rjmp	StartBoot
cm7F_0:

	lds	XL,XSEG+RXBuf
	std	Y+yRXPointer,XL
cm_S4:
	lds	r16,XSEG+RXBuf
	ldd	XL,Y+yRXPointer
	cp	XL,r16
	brsh	cm_S3
	rjmp	cm_S2
cm_S3:	lds	r16,XSEG+RXBuf+2
	ldd	r17,Y+yMyAddr
	rjmp	RetCMDProc


StartBoot:
	outi	SPCR,0b00000000	; Disable SPI
 	outi	SPSR,0b00000000
	cli
	InitPorts
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
bin__1:	st	Z+,YH
	cpi	ZH,2
	brlo	bin__1

	rjmp	Boot_continue
	

