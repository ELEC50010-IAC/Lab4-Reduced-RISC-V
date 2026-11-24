.text
.globl main
main:
    li t1, 10           # t1 = 10
    li t2, 3            # t2 = 3
    sub a0, t1, t2      # a0 = t1 - t2 (= 7)
    bne a0, zero, finish

finish:     # expected result is 7
    bne a0, zero, finish    # loop forever
