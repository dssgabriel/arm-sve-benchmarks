    .text
    .global assembly_gaxpy
    .type assembly_gaxpy, %function

    a       .req d0
    x_ptr   .req x0
    y_ptr   .req x1
    len     .req x2

assembly_gaxpy:
    cbz     len, .end
    mov     x3, #0
    cntd    x4
    mov     z0.d, a
    whilelo p0.d, xzr, len
    ptrue   p1.b
.loop:
    ld1d    z2.d, p0/z, [y_ptr, x3, lsl #3]
    ld1d    z1.d, p0/z, [x_ptr, x3, lsl #3]
    fmad    z1.d, p1/m, z0.d, z2.d
    st1d    z1.d, p0, [y_ptr, x3, lsl #3]
    add     x3, x3, x4
    whilelo p0.d, x3, len
    b.ne    .loop
.end:
    ret
