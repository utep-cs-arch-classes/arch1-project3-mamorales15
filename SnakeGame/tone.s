	.file	"tone.c"
	.text
	.extern CCRO
	.extern CCR1
	.extern P2SEL2
	.extern P2SEL
	.extern P2DIR
	.extern BIT6
	.extern BIT7
	
tone_set_period:
	MOV r12, &CCRO
	;;  Not sure how to do cycles >> 1
	RET

tone_init:
	CALL #timerAUpmode()
	MOV &BIT6, r4
	BIS &BIT7, r4
	BIC r4, r4
	AND r4, &P2SEL2
	BIC &BIT7, r5
	AND r5, &P2SEL
	BIS &BIT6, &P2SEL
	MOV &BIT6, &P@DIR
	RET
