.text
.globl main
main:
    li t1, 1            # t1 = 1 
    li t2, -1           # t2 = -1, which unsigned is 0xFFFFFFFF (the largest number) 
    sltu a0, t1, t2     # unsigned: 1 < 0xFFFFFFFF (= 1)
    bne a0, zero, finish

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
