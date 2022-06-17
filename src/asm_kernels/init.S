    .text
    .global assembly_init
    .type assembly_init, %function

    k       .req d0
    x_ptr   .req x0
    len     .req x1

assembly_init:
    cbz     len, .end
    mov     x3, xzr
    cntd    x4
    whilelo p0.d, x3, len
    mov     z0.d, k
.loop:
    st1d    z0.d, p0, [x_ptr, x3, lsl #3]
    add     x3, x3, x4
    whilelo p0.d, x3, len
    b.mi    .loop
.end:
	ret
