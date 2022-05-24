	.text
	.global asm_dotprod
	.type asm_dotprod, %function

	aPtr	.req x0
	bPtr	.req x1
	outPtr  .req x2
	size 	.req x3

asm_dotprod:
	mov 	x4, xzr
	fmov	d0, xzr
	dup		z0.d, #0
	cntd	x5
	whilelo	p0.d, x4, size
.loop:
	ld1d	z1.d, p0/z, [aPtr, x4, lsl #3]
	ld1d	z2.d, p0/z, [bPtr, x4, lsl #3]
	fmla	z0.d, p0/m, z1.d, z2.d
	add 	x4, x4, x5
.latch:
	whilelo p0.d, x4, size
	b.mi	.loop
	ptrue	p0.d
	faddv	d0, p0, z0.d
	str 	d0, [outPtr]
	ret
