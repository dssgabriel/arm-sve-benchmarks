    .text
    .global assembly_gaxpy
    .type assembly_gaxpy, %function

    x_ptr   .req x0
    y_ptr   .req x1
    z_ptr   .req x2
    a       .req x3
    len     .req x4

assembly_gaxpy:
    cbz     len, .end
    mov     z0.d, d0
    mov     x5, xzr
    cntd    x6
    whilelo p0.d, x5, len
.loop:
    ld1d    z1.d, p0/z, [x_ptr, x5, lsl #3]
    ld1d    z2.d, p0/z, [y_ptr, x5, lsl #3]
    fmad    z1.d, p0/m, z0.d, z2.d
    st1d    z2.d, p0, [z_ptr, x5, lsl #3]
    add     x5, x5, x6
    whilelo p0.d, x5, len
    b.mi    .loop
.end:
    ret
