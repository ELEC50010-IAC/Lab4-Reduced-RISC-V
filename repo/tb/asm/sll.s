.text
.globl main
main:
    li t1, 1            # t1 = 1 
    li t2, 4            # t2 = 4 
    sll a0, t1, t2      # a0 = t1 << t2 (= 16)
    bne a0, zero, finish

finish:     # expected result is 16
    bne a0, zero, finish    # loop forever
