    .text
    .global assembly_gaxpy
    .type assembly_gaxpy, %function

    a       .req x0
    x_ptr   .req x1
    y_ptr   .req x2
    z_ptr   .req x3
    len     .req x4

assembly_gaxpy:
    cbz     len, .end
    mov     z0.d, d0
    mov     x4, xzr
    cntd    x5
    whilelo p0.d, x4, len
.loop:
    ld1d    z1.d, p0/z, [x_ptr, x4, lsl #3]
    ld1d    z2.d, p0/z, [y_ptr, x4, lsl #3]
    fmad    z2.d, p0/m, z0.d, z1.d
    st1d    z2.d, p0, [z_ptr, x4, lsl #3]
    add     x4, x4, x5
    whilelo p0.d, x4, len
    b.mi    .loop
.end:
    ret
