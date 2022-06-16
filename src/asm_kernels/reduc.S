    .text
    .global assembly_reduc
    .type assembly_reduc, %function

    x_ptr   .req x0
    r       .req x1
    len     .req x2

assembly_reduc:
    fmov    d0, xzr
    cbz     len, .end
    dup     z0.d, #0
    mov     x3, xzr
    cntd    x4
    whilelo p0.d, x3, len
.loop:
    ld1d    z1.d, p0/z, [x_ptr, x3, lsl #3]
    fadd    z0.d, z0.d, z1.d
    add     x3, x3, x4
    whilelo p0.d, x3, len
    b.mi    .loop
    
    ptrue   p1.d
    faddv   d0, p1, z0.d
.end:
    str     d0, [r]
    ret
