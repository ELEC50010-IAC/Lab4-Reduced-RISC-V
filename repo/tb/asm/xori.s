.text
.globl main
main:
    li t1, 10           # t1 = 10
    xori a0, t1, -1     # a0 = t1 ^ (-1) (= 0xFFFFFFF5)
    bne a0, zero, finish

finish:     # expected result is 0xFFFFFFF5
    bne a0, zero, finish    # loop forever
