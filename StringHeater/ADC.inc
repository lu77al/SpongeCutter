; ��������� �������� ����� ������� ������� StartADC
;  1 - ������������ ADCDummy ������ ��������� ������
;  2 - ����������� ADCSamplesQnt �������� ���������
;  3 - ����� ����������� � ������ (��������� SaveADC)
;  3 - ������������������� �����
;  4 - ���� �� ��� ������ ���������� ������ � 1
;  5 - ��������������� ���� afADCComplete � ��������� ���������������
; � ��������� SaveADC ����� �������������� ��������������
; �������������� ���������� (r23 �������� ����� ������, r17:r16 - �����

.equ	Ref256  = 0b11000000
.equ	RefAVCC = 0b01000000
.equ	RefExt  = 0b00000000

.equ	ADCSamplesQnt	= 16
.equ	ADCDummy	= 8
.equ	ADCChQnt	= 2

; CH#0 - ADC0 (PC0) -  U
.equ	adcU = 0
.equ	ADMUX0 = Ref256 + 3
; CH#1 - ADC1 (PC1) -  I
.equ	adcI = 1
.equ	ADMUX1 = Ref256 + 2


.macro	GetADC
	lds	r16,$100+ADCBuf+@0*2
	lds	r17,$100+ADCBuf+@0*2+1
.endm

.macro	GetADC_
	lds	@1,$100+ADCBuf+@2*2
	lds	@0,$100+ADCBuf+@2*2+1
.endm

ADCChannels:
.db		ADMUX0, ADMUX1

InitADC:
	outi	ADCSR,0b10000110	; Enable ADC, CK/64
	outi	SFIOR,0
	stdi	yADCCurCh,ADCChQnt-1
	stdi	yADCSamCnt,$FF
	ldi_w	ZH,ZL,2*ADCChannels+(ADCChQnt-1)
	lpm	r16,Z
	out	ADMUX,r16
	sbi	ADCSRA,ADSC
	clt
	bld	AFlags,afADCComplete
	ret

.macro	tskADC
	ldd	r16,Y+yADCSamCnt
	cpi	r16,$FF
	breq	RetADC
	sbis	ADCSR,6
	rjmp	CheckADCProc
.endm

.macro	StartADC
	sbi	ADCSRA,ADSC
	stdi	yADCSamCnt,ADCSamplesQnt+ADCDummy-1
.endm

CheckADCProc:
	in	r24,ADCL	 ; ��������� ��������� ���������
	in	r25,ADCH
	ldd	r23,Y+yADCCurCh	 ; ������ ��������������� ������
	ldd	r22,Y+yADCSamCnt ; ������� ��������� ������
	subi	r22,1
	brcc	adc_1	; ���� ��� ��������� ������ ��������� ������� �����
	ldi	r22,ADCSamplesQnt+ADCDummy-1
	ldd	r16,Y+yADCCurCh	 ; ������ ��������������� ������
	subi	r16,1
	brcc	adc_2	; ���� ���������� ��� ������
	ldi	r16,ADCChQnt-1 ;  ������ � ����������
	set
	bld	AFlags,afADCComplete ; � ��������� ���� ���������� ���������
	ldi	r22,$FF
adc_2:	std	Y+yADCCurCh,r16
	ldi_w	ZH,ZL,2*ADCChannels
	add	ZL,r16
	adc	ZH,ZeroReg
	lpm	r16,Z
	out	ADMUX,r16
adc_1:	sbi	ADCSRA,ADSC	; ����� ���������
	std	Y+yADCSamCnt,r22
	ldd	r16,Y+yADCSumL	; ��������� ���������
	ldd	r17,Y+yADCSumH
	add	r16,r24
	adc	r17,r25
	cpi	r22,ADCSamplesQnt+ADCDummy-1 ; ���� ����� ������������
	brlo	adc_3
	std	Y+yADCSumL,ZeroReg ; �������� �����
	std	Y+yADCSumH,ZeroReg
	rjmp	SaveADC		; ��������� �������� � ����� �����������
adc_3:	cpi	r22,ADCSamplesQnt-1 ; ���� �������������� �� ������
	brsh	adc_4
	std	Y+yADCSumL,r16 ; ��������� ������������� �����
	std	Y+yADCSumH,r17
adc_4:	rjmp	RetADC

SaveADC:
	ldi	XL,ADCBuf
	add	XL,r23
	add	XL,r23
	lsl16	r17,r16
	lsl16	r17,r16
	st	X+,r16
	st	X+,r17
	rjmp	RetADC

;	cpi	r23,0
;	brne	
;	lds	r18,AdjustUabL
;	lds	r19,AdjustUabH
;	lsl16	r17,r16
;	rcall	Mul_16
;	rjmp	adja_2
;adja_1:	cpi	r23,1
;	brne	adja_2
;	lds	r18,AdjustUinL
;	lds	r19,AdjustUinH
;	lsl16	r17,r16
;	rcall	Mul_16
;adja_2:	ldi	r18,0
;	lsl16	r17,r16
;	rol	r18
;	lsl16	r17,r16
;	rol	r18
;	lsl16	r17,r16
;	rol	r18
;	tst	r18
;	breq	adja_3
;	ldi_w	r17,r16,$FFFF
;adja_3:	st	X+,r16
;	st	X+,r17
;	ret

