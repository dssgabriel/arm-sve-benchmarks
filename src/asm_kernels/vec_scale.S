    .text
    .global assembly_vec_scale
    .type assembly_vec_scale, %function

    k       .req x0
    x_ptr   .req x1
    y_ptr   .req x2
    len     .req x3

assembly_vec_scale:
    cbz     len, .end
    mov	    x4, xzr
    dup     z0.d, k
    cntd    x5
    whilelo p0.d, x4, len
.loop:
    ld1d    z1.d, p0/z, [x_ptr, x4, lsl #3]
    fmul    z1.d, z1.d, z0.d
    st1d    z1.d, p0, [x_ptr, x4, lsl #3]
    add     x4, x4, x5
    whilelo p0.d, x4, len
    b.mi    .loop
.end:
    ret
