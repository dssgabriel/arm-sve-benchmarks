	.text
	.global asm_init
	.type asm_init, %function

	aPtr	.req x0
	cnst	.req x1
	size	.req x2

asm_init:
	mov 	x3, xzr
	cntd	x4
	whilelo	p0.d, x3, cnst
	mov 	z0.d, d0
.loop:
	st1d 	z0.d, p0, [aPtr, x3, lsl #3]
	add 	x3, x3, x4
.latch:
	whilelo p0.d, x3, cnst
	b.mi	.loop
	ret
