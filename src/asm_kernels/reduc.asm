	.text
	.global asm_reduc
	.type asm_reduc, %function

	aPtr	.req x0
	outPtr	.req x1
	size	.req x2

asm_reduc:
	mov 	x3, xzr
	fmov	d0, xzr
	dup		z0.d, #0
	cntd	x4
	ptrue	p1.d
	whilelo	p0.d, x3, size
.loop:
	ld1d	z1.d, p0/z, [aPtr, x3, lsl #3]
	fadd 	z0.d, p0/m, z0.d, z1.d
	add 	x3, x3, x4
.latch:
	whilelo p0.d, x3, size
	b.mi	.loop
	faddv	d0, p1, z0.d
	str		d0, [outPtr]
	ret
