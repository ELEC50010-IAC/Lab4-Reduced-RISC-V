.text
.globl main
main:
    li t0, 0x100            # t0 = 0x100 (data memory address)
    li t1, -2               # t1 = -2, lowest half is 0xFFFE (top bit set)
    sh t1, 0(t0)            # mem[0x100] = 0xFFFE
    lh a0, 0(t0)            # a0 = 0xFFFE sign-extended (= 0xFFFFFFFE)
    bne a0, zero, finish

finish:     # expected result is 0xFFFFFFFE
    bne a0, zero, finish    # loop forever
