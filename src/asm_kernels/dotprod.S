    .text
    .global assembly_dotprod
    .type assembly_dotprod, %function

    x_ptr   .req x0
    y_ptr   .req x1
    d       .req x2
    len     .req x3

assembly_dotprod:
    fmov    d0, xzr
    cbz     len, .end
    dup     z0.d, #0
    mov     x4, xzr
    cntd    x5
    whilelo p0.d, x4, len
.loop:
    ld1d    z1.d, p0/z, [x_ptr, x4, lsl #3]
    ld1d    z2.d, p0/z, [y_ptr, x4, lsl #3]
    fmla    z0.d, p0/m, z1.d, z2.d
    add     x4, x4, x5
    whilelo p0.d, x4, len
    b.mi    .loop
    
    ptrue   p1.d
    faddv   d0, p1, z0.d
.end:
    str     d0, [d]
    ret
