.text
.globl main
main:
    li t1, -1               # t1 = 0xFFFFFFFF (largest unsigned)
    li t2, 1                # t2 = 1
    bltu t1, t2, wrong      # unsigned: 0xFFFFFFFF < 1 is false, must not jump
    bltu t2, t1, right      # unsigned: 1 < 0xFFFFFFFF is true, must jump
    li a0, 2                # only reached if second bltu didn't jump
    j finish
wrong:
    li a0, 3                # only reached if first bltu jumped
    j finish
right:
    li a0, 1                # correct path

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
