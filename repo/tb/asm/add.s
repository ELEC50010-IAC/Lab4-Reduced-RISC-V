.text
.globl main
main:
    li t1, 12           # t1 = 12
    li t2, 30           # t2 = 30
    add a0, t1, t2      # a0 = t1 + t2 (= 42)
    bne a0, zero, finish

finish:     # expected result is 42
    bne a0, zero, finish    # loop forever
