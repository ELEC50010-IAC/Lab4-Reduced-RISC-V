.text
.globl main
main:
    li t0, 0x100            # t0 = 0x100 (data memory address)
    li t1, -2               # t1 = -2, lowest half is 0xFFFE (top bit set)
    sh t1, 0(t0)            # mem[0x100] = 0xFFFE
    lhu a0, 0(t0)           # a0 = 0xFFFE zero-extended (= 0xFFFE)
    bne a0, zero, finish

finish:     # expected result is 0xFFFE
    bne a0, zero, finish    # loop forever
