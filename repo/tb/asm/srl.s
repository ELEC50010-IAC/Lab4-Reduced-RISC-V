.text
.globl main
main:
    li t1, 64           # t1 = 64 
    li t2, 2            # t2 = 2 
    srl a0, t1, t2      # a0 = t1 >> t2 (= 16)
    bne a0, zero, finish

finish:     # expected result is 16
    bne a0, zero, finish    # loop forever
