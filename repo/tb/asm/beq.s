.text
.globl main
main:
    li t1, 5                # t1 = 5
    li t2, 5                # t2 = 5
    li t3, 6                # t3 = 6
    beq t1, t3, wrong       # 5 != 6, must not jump
    beq t1, t2, right       # 5 == 5, must jump
    li a0, 2                # only reached if second beq didn't jump
    j finish
wrong:
    li a0, 3                # only reached if first beq jumped
    j finish
right:
    li a0, 1                # correct path

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
