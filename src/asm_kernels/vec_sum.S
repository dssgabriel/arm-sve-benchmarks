    .text
    .global assembly_vec_sum
    .type assembly_vec_sum, %function

    x_ptr   .req x0
    y_ptr   .req x1
    z_ptr   .req x2
    len     .req x3

assembly_vec_sum:
    cbz     len, .end
    mov     x4, xzr
    cntd    x5
    whilelo p0.d, x4, len
.loop:
    ld1d    z0.d, p0/z, [x_ptr, x4, lsl #3]
    ld1d    z1.d, p0/z, [y_ptr, x4, lsl #3]
    fadd    z0.d, z0.d, z1.d
    st1d    z0.d, p0, [z_ptr, x4, lsl #3]
    add     x4, x4, x5
    whilelo p0.d, x4, len
    b.mi    .loop
.end:
    ret
