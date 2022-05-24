    .text
    .global asm_vec_scale
    .type asm_vec_scale, %function

    aPtr    .req x0
    bPtr    .req x1
    cPtr    .req x2
    size    .req x3

asm_vec_scale:
    mov	    x4, xzr
    cntd    x5
    whilelo p0.d, x4, size
.loop:
    ld1d    z0.d, p0/z, [aPtr, x4, lsl #3]
    ld1d    z1.d, p0/z, [bPtr, x4, lsl #3]
    fmul    z0.d, z0.d, z1.d
    st1d    z0.d, p0, [cPtr, x4, lsl #3]
    add     x4, x4, x5
.latch:
    whilelo p0.d, x4, size
    b.mi    .loop
    ret
