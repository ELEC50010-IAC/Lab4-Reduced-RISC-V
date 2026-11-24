.text
.globl main
main:
    li t1, 10           # t1 = 10
    li t2, 12           # t2 = 12
    or a0, t1, t2       # a0 = t1 | t2 (= 14)
    bne a0, zero, finish

finish:     # expected result is 14
    bne a0, zero, finish    # loop forever
