.text
.globl main
main:
    li t1, 3           # t1 = 3
    slli a0, t1, 4     # a0 = t1 << 4 (= 48)
    bne a0, zero, finish

finish:     # expected result is 48
    bne a0, zero, finish    # loop forever
