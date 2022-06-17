    .text
    .global assembly_vec_sum
    .type assembly_vec_sum, %function

    x_ptr   .req x0
    y_ptr   .req x1
    len     .req x2

assembly_vec_sum:
    cbz     len, .end
    mov     x3, xzr
    cntd    x4
    whilelo p0.d, x3, len
.loop:
    ld1d    z0.d, p0/z, [x_ptr, x3, lsl #3]
    ld1d    z1.d, p0/z, [y_ptr, x3, lsl #3]
    fadd    z0.d, z0.d, z1.d
    st1d    z0.d, p0, [x_ptr, x3, lsl #3]
    add     x3, x3, x4
    whilelo p0.d, x3, len
    b.mi    .loop
.end:
    ret
