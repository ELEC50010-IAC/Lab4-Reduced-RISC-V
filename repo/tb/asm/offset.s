.text
.globl main
main:
    li t0, 0x100            # t0 = 0x100
    li t2, 0x108            # t2 = 0x108
    li t1, 90               # t1 = 90
    sw t1, -4(t2)           # store at 0x108 - 4 = 0x104
    lw a0, 4(t0)            # load from 0x100 + 4 = 0x104 (= 90)
    bne a0, zero, finish

finish:     # expected result is 90
    bne a0, zero, finish    # loop forever
