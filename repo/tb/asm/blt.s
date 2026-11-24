.text
.globl main
main:
    li t1, -1               # t1 = -1 (0xFFFFFFFF)
    li t2, 1                # t2 = 1
    blt t2, t1, wrong       # signed: 1 < -1 is false, must not jump
    blt t1, t2, right       # signed: -1 < 1 is true, must jump
    li a0, 2                # only reached if second blt didn't jump
    j finish
wrong:
    li a0, 3                # only reached if first blt jumped
    j finish
right:
    li a0, 1                # correct path

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
