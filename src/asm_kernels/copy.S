    .text
    .global assembly_copy
    .type assembly_copy, %function
   
    x_ptr   .req x0
    y_ptr   .req x1
    len     .req x2

assembly_copy:
    cbz     len, .end
    mov     x3, xzr
    cntd    x4
    whilelo p0.d, x3, len
.loop:
    ld1d    z0.d, p0/z, [y_ptr, x3, lsl #3]
    st1d    z0.d, p0, [x_ptr, x3, lsl #3]
    add     x3, x3, x4
    whilelo p0.d, x3, len
    b.mi    .loop
.end:
    ret
