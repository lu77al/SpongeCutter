InitUart:
	outi	UCSRA,0
	outi	UCSRB,0b00011000	; ��������� ����� � ��������
	outi	UCSRC,0b10000110	; 8-���
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

; ���������� USART � �������� ������
.macro	tskUart
	ldd	XL,Y+yTXPointer
	ldd	r17,Y+yTXEnd
	cp	XL,r17
	breq	uart_1
	sbis	UCSRA,UDRE ; ++ ��������� � ������ ��������
	rjmp	RetUart  ; ����� ���� ����� �������� �����
#ifdef	UART_DIR_PORT
	sbi	UART_DIR_PORT  ; ����������� ���������� �� ��������
#endif
	ld	r16,X+
; String emulator!!!
;	subi	r17,2
; String emulator!!!
	cp	XL,r17
	brne	uart_2
	ldd	r16,Y+yRXTXCHS
uart_2:	out	UDR,r16   ; ��������� ���������� ���� � �����
	std	Y+yTXPointer,XL
	ldd	r17,Y+yRXTXCHS
	add	r17,r16
	std	Y+yRXTXCHS,r17
	sbi	UCSRA,TXC ; ��������� ����� ��� ����������� �������� �� �����
	rjmp	RetUart
uart_1:
#ifdef	UART_DIR_PORT
	sbic	UCSRA,TXC ; ++ ��������� � ������ ������
	cbi	UART_DIR_PORT  ;  ����������� ���������� �� ����� ����� �������� �������� �����
#endif
	sbic	UCSRA,RXC  ; ����� �� ����������� ���� ��� ��������� �����
	rjmp	ReadUart   ; ���������� �������� ����
.endm

; -1 00 01 02 03 .. NN
; ST LN DN SR D0 .. CS
ReadUart:
	in	r16,UDR
	ldd	XL,Y+yRXPointer
	cpi	XL,RXBuf-1
	breq	rx_1
	cpi	XL,RXBuf
	breq	rx_2
	lds	r17,XSEG+RXBuf	; ++ ������ ���� 01...
	cp	XL,r17		; �������� �� ���������� �������
	breq	rx_3
rx_8:	st	X+,r16		; ����������� ���������� ����� � ������
	ldd	r17,Y+yRXTXCHS
	add	r17,r16
	std	Y+yRXTXCHS,r17
rx_5:	std	Y+yRXPointer,XL
rx_4:	std	Y+yRXTimer,ZeroReg
	rjmp	RetUart
rx_1:	cpi	r16,UARTStartByte ; ++ ������ ����� -1
	brne	rx_4		;  �������� �� ���������� �����-����
;	ldd	r16,Y+yRXTimer
;	cpi	r16,UARTMesDelay
;	brlo	rx_4		;  �������� �� ����������� �������� ����� ��������
	stdi	yRXTXCHS,UARTStartByte-(RXBuf+3)
	inc	XL
	rjmp	rx_5
rx_2:	cpi	r16,1		; ++ ������ ����� 00 (�����)
	brlo	rx_6		;  �������� �� ����������� �����
	cpi	r16,RXBufLen-2
	brlo	rx_7		;  �������� �� ������������ �����
rx_6:	ldi	XL,RXBuf-1	; ����� ������ ��� ��������� �����
	rjmp	rx_5
rx_7:	subi	r16,-(RXBuf+3)	;  ������������ ������ ����� �������
	rjmp	rx_8
rx_3:		; ++ ������� ��� ������� ++
rx_9:	ldd	r17,Y+yRXTXCHS ; �������� ����������� �����
	cp	r16,r17
	brne	rx_6	; ����� ������ ��� �������� �����
	stdi	yRXTimer,UARTMesDelay+1	; ������ ����� � �������� ����� �������
		; ++ ��������� ����������� �������
	stdi	yTXPointer,TXBuf+4
	ldi	XL,RXBuf+1
	ld	r16,X+	; ����� ����������
	ld	r17,X+	; ����� �����������
	std	Y+yRXPointer,XL
	ldd	r18,Y+yMyAddr ; *** ����� ����������� ������
	cp	r16,r18
	breq	MyCMDProcess
	andi	r18,$F0
	cp	r16,r18
	breq	MyCMDProcess
				; �� ������� ����������� �����������
rx_10:	stdi	yTXPointer,TXBuf
	std	Y+yTXEnd,r16
	stdi	yRXPointer,RXBuf-1
	rjmp	RetUart
RetCMDProc:
	ldd	XL,Y+yTXPointer	; �������������� ������ ��� ������� ������
	cpi	XL,TXBuf+4
	breq	rx_10
	rcall	ComplUartMes
	rjmp	RetUart

ComplUartMes:
	mov	r20,XL
	subi	r20,TXBuf+4
	inc	XL
; String emulator!!!
;	ldi	r25,$FF
;	st	X+,r25
;	st	X+,ZeroReg
		
;	subi	XL,-2
; String emulator!!!
	std	Y+yTXEnd,XL
	ldi	XL,TXBuf
	std	Y+yTXPointer,XL
	ldi	r18,UARTStartByte
	st	X+,r18
	st	X+,r20
	st	X+,r16
	st	X+,r17
	stdi	yRXPointer,RXBuf-1
	std	Y+yRXTXCHS,ZeroReg
	ret

MyCMDProcess:
	stdi	yNoUartCnt,30

	ldd	XL,Y+yRXPointer
cm_S2:
	ld	r16,X+
	std	Y+yRXPointer,XL

	cpi	r16,$00		; *** StateRequest ***
	brne	cm0_0
	inc	XL	; (1 extra byte for string driver)
	std	Y+yRXPointer,XL
	rjmp	RepStatus
cm0_0:
	cpi	r16,$01		; *** Push line in the buf ***
	brne	cm1_0
	EnableSteppers
	ldd	r16,Y+yDataCnt	; Check space in queue
	cpi	r16,32
	brsh	cm1_1
	ldd	r16,Y+yDataHead ; Move queue head
	mov	ZL,r16		; prepare pointer
	inc	r16
	andi	r16,31
	std	Y+yDataHead,r16
	ldi	ZH,2
	sbrc	ZL,4
	ldi	ZH,3
	andi	ZL,$0F
	swap	ZL
	mov	r17,ZL		; prepare limit
	subi	r17,-13
	cli
	ldd	r16,Y+yDataCnt	; increase data counter
	inc	r16
	std	Y+yDataCnt,r16
cm1_2:	ld	r16,X+		; push main data of line
	st	Z+,r16
	cp	ZL,r17
	brlo	cm1_2
	sei
	inc	XL
cm1_3:	std	Y+yRXPointer,XL
	rjmp	RepStatus
cm1_1:	subi	XL,-14	; (1 extra byte for string driver)
	rjmp	cm1_3
cm1_0:

; --- Line in queue ---
; 00 - MStH     - Steps at master axis
; 01 - MStL
; 02 - SStH     - Steps at slave axis
; 03 - SStL
; 04 - StartTH  - Initial period
; 05 - StartTL
; 06 - CruiseTH - Cruise period
; 07 - CruiseTL
; 08 - EndTH    - End period
; 09 - EndTL
; 0A - BrLenH   - Breaking distance
; 0B - BrLenL
; 0C - Flags
;	0 - XDIR
;	1 - YDIR
;	2 - XMaster
;	3 - MoveLong (don't decrement master steps)
;	4 - StopOnHS (stop on limit switch - hall sensor)

	cpi	r16,$03		; *** Stop immediate ***
	brne	cm3_0
	inc	XL	; (1 extra byte for string driver)
	std	Y+yRXPointer,XL
	cli
	std	Y+yMasterStCntL,ZeroReg
	std	Y+yMasterStCntH,ZeroReg
	std	Y+yDataCnt,ZeroReg
	std	Y+yDataHead,ZeroReg
	std	Y+yDataTail,ZeroReg
	clt
	bld	AFlags,afMoveLong
	sei
	rjmp	RepStatus
cm3_0:
	cpi	r16,$04		; *** Stop smooth ***
	brne	cm4_0
	inc	XL	; (1 extra byte for string driver)
	std	Y+yRXPointer,XL
	ldi_w	r17,r16,$F000
	ldi	r18,$FF
	cli
	std	Y+yAXChangeL,r18
	std	Y+yAXChangeH,r18
	std	Y+yEndPeriodL,r16
	std	Y+yEndPeriodH,r17
	std	Y+yDataCnt,ZeroReg
	std	Y+yDataHead,ZeroReg
	std	Y+yDataTail,ZeroReg
	clt
	bld	AFlags,afMoveLong
	sei
	rjmp	RepStatus
cm4_0:


	cpi	r16,$70 ; *** ������ ������� ������ ***
	brne	cm70_0
	ld	ZH,X+	; ZH[2..0]:ZL - ��������� �� ������ ����
	ld	ZL,X+	; ������� 5 ��� ZH - ����� �������
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
	cpi	r16,$71 ; *** ������ ������� ������ ***
	brne	cm71_0
	ld	ZH,X+	; ZH[2..0]:ZL - ��������� �� ������ ����
	ld	ZL,X+	; ������� 5 ��� ZH - ����� �������
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
	cpi	r16,$72 ; *** ��������� ����� ������ �������� ***
	brne	cm72_0
	ld	ZH,X+	; ZH:ZL - ��������� �� ����
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

	cpi	r16,$7F		; *** ������� � ���������� ***
	brne	cm7F_0
	rjmp	BootLoader
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

RepStatus:
	ldd	XL,Y+yTXPointer
	ldi	r16,$80
	st	X+,r16
	cli
	ldd	r16,Y+yDataCnt
	ldd	r17,Y+yMasterStCntL
	ldd	r18,Y+yMasterStCntH
	sei
	or	r18,r17
	or	r18,r16
	breq	rs_1
	ori	r16,1<<7
rs_1:	st	X+,r16
	std	Y+yTXPointer,XL
	rjmp	cm_S4


