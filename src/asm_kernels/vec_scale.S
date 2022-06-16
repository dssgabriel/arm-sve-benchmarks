    .text
    .global assembly_vec_scale
    .type assembly_vec_scale, %function

    x_ptr   .req x0
    k       .req x1
    len     .req x2

assembly_vec_scale:
    cbz     len, .end
    mov	    x3, xzr
    dup     z0.d, k
    cntd    x4
    whilelo p0.d, x3, len
.loop:
    ld1d    z1.d, p0/z, [x_ptr, x3, lsl #3]
    fmul    z1.d, z1.d, z0.d
    st1d    z1.d, p0, [x_ptr, x3, lsl #3]
    add     x3, x3, x4
    whilelo p0.d, x3, len
    b.mi    .loop
.end:
    ret
