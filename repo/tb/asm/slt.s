.text
.globl main
main:
    li t1, -1           # t1 = -1 
    li t2, 1            # t2 = 1 
    slt a0, t1, t2      # a0 = t1 < t2 (= 1)
    bne a0, zero, finish

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
