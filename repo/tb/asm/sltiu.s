.text
.globl main
main:
    li t1, 1              # t1 = 1
    sltiu a0, t1, -1      # unsigned: 1 < 0xFFFFFFFF (= 1)
    bne a0, zero, finish

finish:     # expected result is 1
    bne a0, zero, finish    # loop forever
