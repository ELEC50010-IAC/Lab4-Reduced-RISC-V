.text
.globl main
main:
    li t1, -1               # t1 = 0xFFFFFFFF (largest unsigned)
    li t2, 1                # t2 = 1
    bgeu t2, t1, wrong      # unsigned: 1 >= 0xFFFFFFFF is false, must not jump
    bgeu t1, t2, right      # unsigned: 0xFFFFFFFF >= 1 is true, must jump
    li a0, 2                # only reached if second bgeu didn't jump
    j finish
wrong:
    li a0, 3                # only reached if first bgeu jumped
    j finish
right:
    li a0, 1                # correct path

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
