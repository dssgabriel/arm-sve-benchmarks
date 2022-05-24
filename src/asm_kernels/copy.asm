	.text
	.global asm_copy
	.type asm_copy, %function

	aPtr	.req x0
	bPtr	.req x1
	size	.req x2

asm_copy:
	mov 	x3, xzr
	cntd	x4
	whilelo	p0.d, x3, size
.loop:
    ld1d	z0.d, p0/z, [bPtr, x3, lsl #3]
    st1d 	z0.d, p0, [aPtr, x3, lsl #3]
	add 	x3, x3, x4
.latch:
	whilelo p0.d, x3, size
	b.mi	.loop
	ret
