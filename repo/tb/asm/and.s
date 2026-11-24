.text
.globl main
main:
    li t1, 12           # t1 = 12
    li t2, 10           # t2 = 10
    and a0, t1, t2      # a0 = t1 & t2 (= 8)
    bne a0, zero, finish

finish:     # expected result is 8
    bne a0, zero, finish    # loop forever
