.text
.globl main
main:
    li t1, -1            # t1 = -1
    slti a0, t1, 1       # a0 = t1 < 1 (= 1)
    bne a0, zero, finish

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
