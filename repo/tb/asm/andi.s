.text
.globl main
main:
    li t1, -100             # t1 = -100
    andi a0, t1, -16        # a0 = 0xFFFFFF9C & 0xFFFFFFF0 (= 0xFFFFFF90)
    bne a0, zero, finish

finish:     # expected result is 0xFFFFFF90
    bne a0, zero, finish    # loop forever
