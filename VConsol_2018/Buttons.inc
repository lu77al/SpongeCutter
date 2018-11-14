Buttons:
	cbi	KB_C0_LEV ; Подать землю на столбец [147*]
	sbi	KB_C0_DIR
	ldi	r17,$FF
	ldi	r25,5
bt_1:	loop	r25,PC-1
	in	r25,PINC  ; Прочитать состояние порта
	cbi	KB_C0_DIR ; Снять землю со столбца [147*]
	sbi	KB_C0_LEV
	cbi	KB_C1_LEV ; Подать землю на столбец [2580]
	sbi	KB_C1_DIR
	bst	r25,KB_R0
	bld	r16,1
	bst	r25,KB_R1
	bld	r16,4
	bst	r25,KB_R2
	bld	r16,7
	bst	r25,KB_R3
	bld	r17,2
	in	r25,PINC  ; Прочитать состояние порта
	cbi	KB_C1_DIR ; Снять землю со столбца [2580]
	sbi	KB_C1_LEV
	cbi	KB_C2_LEV ; Подать землю на столбец [369#]
	sbi	KB_C2_DIR
	bst	r25,KB_R0
	bld	r16,2
	bst	r25,KB_R1
	bld	r16,5
	bst	r25,KB_R2
	bld	r17,0
	bst	r25,KB_R3
	bld	r16,0
	in	r25,PINC ; Прочитать состояние порта
	cbi	KB_C2_DIR ; Снять землю со столбеца [369#]
	sbi	KB_C2_LEV
	bst	r25,KB_R0
	bld	r16,3
	bst	r25,KB_R1
	bld	r16,6
	bst	r25,KB_R2
	bld	r17,1
	bst	r25,KB_R3
	bld	r17,3
	sbis	CARD_INSERTED
	andi	r17,$FF - (1<<4)
	com	r17
	com	r16	; В r17,r16 - текущее состояние кнопок

	ldd	r18,Y+yBakButStateL ; Фильтр дребезга
	ldd	r19,Y+yBakButStateH
	std	Y+yBakButStateL,r16
	std	Y+yBakButStateH,r17
	ldd	r20,Y+yBakButCnt
	cp	r16,r18
	cpc	r17,r19
	breq	but_1
	ldi	r20,4
but_1:	subi	r20,1
	brcs	but_2
	std	Y+yBakButCnt,r20
	ldd	r16,Y+yBakButStateL
	ldd	r17,Y+yBakButStateH
but_2:
	ldd	r18,Y+yButStateL
	ldd	r19,Y+yButStateH
	std	Y+yButStateL,r16 ; Сохраняется текущее состояние кнопок
	std	Y+yButStateH,r17
	com	r18
	com	r19
	and	r18,r16		; r19,r18 - маска свеженажатых кнопок
	and	r19,r17
	andi	r17,0b11101111	; Очистка кнопки "карточка" для отключения повторений
	ldd	r20,Y+yButPressL
	ldd	r21,Y+yButPressH
	or	r20,r18
	or	r21,r19
	std	Y+yButPressL,r20 ; Маска нажатий
	std	Y+yButPressH,r21
	ldd	r20,Y+yButRepL
	ldd	r21,Y+yButRepH
	or	r20,r18
	or	r21,r19
	std	Y+yButRepL,r20
	std	Y+yButRepH,r21
	mov	r20,r18
	or	r20,r19		; Вычисление кода свеженажатой кнопки
	breq	but_7
	ldi	r20,13
but_6:	subi	r20,1
	brcs	but_7
	lsl16	r19,r18
	sbrs	r19,5
	rjmp	but_6
	std	Y+yLastKeyCode,r20
	ldd	r21,Y+yKeyCode
	cpi	r21,$FF
	brne	but_5
	std	Y+yKeyCode,r20
but_5:	ldi	r20,3
	std	Y+ySoundCnt,r20
	std	Y+yFinishSoundCnt,ZeroReg
	ldi	r21,64
	ldi	r20,90
	rjmp	but_4
but_7:	mov	r20,r16
	or	r20,r17
	breq	but_3		; Формирование повторных нажатий при удержании
	ldd	r20,Y+yButCnt
	ldd	r21,Y+yButCntStart
	dec	r20
	brne	but_4
	ldd	r18,Y+yButRepL
	ldd	r19,Y+yButRepH
	or	r18,r16
	or	r19,r17
	std	Y+yButRepL,r18
	std	Y+yButRepH,r19
	ldd	r18,Y+yKeyCode
	cpi	r18,$FF
	brne	but_8
	ldd	r18,Y+yLastKeyCode
	std	Y+yKeyCode,r18
but_8:	ldi	r20,32
	lsr	r21
	add	r20,r21
but_4:	std	Y+yButCnt,r20
	std	Y+yButCntStart,r21
but_3:
	ldd	r16,Y+ySoundCnt
	subi	r16,1
	brcs	but_9
	std	Y+ySoundCnt,r16
	sbi	BUZER_PIN
	ret
but_9:	cbi	BUZER_PIN
	ret
