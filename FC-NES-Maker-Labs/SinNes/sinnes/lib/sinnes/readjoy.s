;
; trbbadboy, 2010-08-31
;
;
;void readjoy(void)

	.export		_readjoy
	.include	"nes.inc"
	
.proc	_readjoy
		lda		#$00
		sta		JOY_VAL
		lda		#$01		;��ʼ��
		sta		APU_PAD1
		lda		#$00
		sta		APU_PAD1
		ldx		#$08		;��ʼ��
read:	asl		JOY_VAL
		lda		APU_PAD1
		and		#$01
		beq		undown
		inc		JOY_VAL
undown:	dex
		bne		read
		lda		JOY_VAL
		rts

.endproc