; blink led attatched to digital pin #13

	.device "primo"

	lda #13
	sta DIGID
	lda #DIGWCFG_OUTPUT
	sta DIGWCFG

l1:	lda #DIGVAL_LOW
	sta DIGVAL
	jsr pause
	lda #DIGVAL_HIGH
	sta DIGVAL
	jsr pause
	jmp l1

pause:
	ldy #20
l3:	ldx #0
l2: nop
	dex
	bne l2
	dey
	bne l3
	rts