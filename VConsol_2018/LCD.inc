LCD_PushByte:	; r16 -> Send 8 bits via 4-bit bus (DB4..DB7 - PC0..PC3)
	ldi	r17,0b011111
	out	DDRC,r17	; PC0..PC3 - outputs
	in	r17,PORTC
	andi	r17,$F0		; PORTC mask (save high bits)
	mov	r18,r16
	swap	r18
	andi	r18,$0F
	sbi	LCD_E		; E /
	or	r18,r17
	out	PORTC,r18	; Push high bits
	rjmp	PC+1
	cbi	LCD_E		; E \
	rjmp	PC+1
	mov	r18,r16
lpb_1:	andi	r18,$0F
	sbi	LCD_E		; E /
	or	r18,r17
	out	PORTC,r18	; Push low bits
	rjmp	PC+1
	cbi	LCD_E		; E \
	ldi	r17,0b010000
	out	DDRC,r17	; PC0..PC3 - inputs
	ret

LCD_PushHalfByte: ; r16 -> Send 4 high bits via 4-bit bus (DB4..DB7 - PC0..PC3)
	ldi	r17,0b011111
	out	DDRC,r17	; PC0..PC3 - outputs
	in	r17,PORTC
	andi	r17,$F0		; PORTC mask (save high bits)
	mov	r18,r16
	swap	r18
	rjmp	lpb_1

/*
.macro	WAIT_01MS
	ldi	r17,high(@0 * 200)
	ldi	r16,low(@0 * 200)
	rcall	Wait16
.endm

.macro	LCD_HALF_BYTE
	ldi	r16,@0
	rcall	LCD_PushHalfByte
	ldi	r17,high(@1 * 200 + 1)
	ldi	r16,low(@1 * 200 + 1)
	rcall	Wait16
.endm

.macro	LCD_BYTE
	ldi	r16,@0
	rcall	LCD_PushByte
	ldi	r17,high(@1 * 200 + 1)
	ldi	r16,low(@1 * 200 + 1)
	rcall	Wait16
.endm


Wait16:	subi_w	r17,r16,1
	brne	Wait16
	ret

InitLCD:
	cbi	LCD_RS
	LCD_HALF_BYTE	0b00110000, 1
	LCD_HALF_BYTE	0b00110000, 1
	LCD_BYTE	0b00100000, 1
	LCD_BYTE	0b00101000, 1
	LCD_BYTE	0b00001000, 1
	LCD_BYTE	0b00000001, 20
	LCD_BYTE	0b00000010, 1
	LCD_BYTE	0b00001100, 1
	ret
*/


InitLCD:
	cbi	LCD_RS
	ldi_w	ZH,ZL,2*LCDInit
	ldi	r19,10
LCD_1:	lpm	r16,Z+
	cpi	r19,7
	brlo	LCD_2
	rcall	LCD_PushHalfByte
	ldi	r17,$23
	rjmp	LCD_3
LCD_2:	rcall	LCD_PushByte
	ldi	r17,$10
LCD_3:	subi	r16,1
	sbci	r17,0
	brne	LCD_3
	loop	r19,LCD_1
	sbi	LCD_RS
	ldi	r19,8
LCD_5:	lpm	r16,Z+
	rcall	LCD_PushByte
	ldi	r16,200
LCD_4:	loop	r16,LCD_4
	loop	r19,LCD_5
	stdi	yLCDPointer,LCDBuf+20
	stdi	yLCDCnt,0b1111
	ret

.macro	PUSH_LCD
	ldd	XL,Y+yLCDPointer
	cpi	XL,LCDBuf+20
	brsh	skLCD		
	ld	r16,X+		
	std	Y+yLCDPointer,XL
	rcall	LCD_PushByte	
skLCD:
.endm

LCDInit:
	.db	0b00110000, 0b00110000,	0b00110000, 0b00100000,	0b00101000, 0b00001000,	0b00000001, 0b00000110
	.db	0b00001100, 0b01000000
	.db	6, 9, 9, 6,  0, 0, 0, 0
