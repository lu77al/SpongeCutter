;-- Init ports state --
	InitPorts

;-- ���� � ��������� --
	outi	SPH,high(RAMEND)
	outi	SPL,low(RAMEND)
	ldi_w	YH,YL,$60
	ldi_w	XH,XL,$100

;-- ��������� ������ � $100 �� $1ff
	ldi_w	ZH,ZL,$60
ini_1:	st	Z+,YH
	cpi	ZH,2
	brlo	ini_1

	ldi_w	YH,YL,YBUF
	ldi_w	XH,XL,XSEG

	clr	AFlags

	rcall	InitTact

	rcall	InitUart

	rcall	InitSteperInt

	sei


