.text
.globl main
main:
    li t1, 10           # t1 = 10
    addi a0, t1, -3     # a0 = t1 + (-3) (= 7)
    bne a0, zero, finish

finish:     # expected result is 7
    bne a0, zero, finish    # loop forever
