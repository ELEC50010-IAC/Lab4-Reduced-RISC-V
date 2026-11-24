.text
.globl main
main:
    li t1, -64           # t1 = -64
    srai a0, t1, 2       # a0 = t1 >>> 2 (= 0xFFFFFFF0)
    bne a0, zero, finish

finish:     # expected result is 0xFFFFFFF0
    bne a0, zero, finish    # loop forever
