    .text
    .global assembly_init
    .type assembly_init, %function

    x_ptr   .req x0
    k       .req x1
    len     .req x2

assembly_init:
    cbz     k, .end
    mov     x3, xzr
    cntd    x4
    whilelo p0.d, x3, k
    mov     z0.d, d0
.loop:
    st1d    z0.d, p0, [x_ptr, x3, lsl #3]
    add     x3, x3, x4
    whilelo p0.d, x3, k
    b.mi    .loop
.end:
	ret
