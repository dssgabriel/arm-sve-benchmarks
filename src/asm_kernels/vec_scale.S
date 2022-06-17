    .text
    .global assembly_vec_scale
    .type assembly_vec_scale, %function

    k       .req d0
    x_ptr   .req x0
    len     .req x1

assembly_vec_scale:
    cbz     len, .end
    mov     z0.d, k
    mov	    x2, xzr
    cntd    x3
    whilelo p0.d, x2, len
.loop:
    ld1d    z1.d, p0/z, [x_ptr, x2, lsl #3]
    fmul    z1.d, z1.d, z0.d
    st1d    z1.d, p0, [x_ptr, x2, lsl #3]
    add     x2, x2, x3
    whilelo p0.d, x2, len
    b.mi    .loop
.end:
    ret
