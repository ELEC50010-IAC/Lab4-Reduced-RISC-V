.text
.globl main
main:
    li t0, 0x100            # t0 = 0x100 (data memory address)
    li t1, 0x12345678       # t1 = 0x12345678
    sw t1, 0(t0)            # mem[0x100] = 0x12345678
    li t2, -1               # t2 = 0xFFFFFFFF
    sh t2, 0(t0)            # only the lowest 2 bytes become 0xFFFF
    lw a0, 0(t0)            # a0 = 0x1234FFFF
    bne a0, zero, finish

finish:     # expected result is 0x1234FFFF
    bne a0, zero, finish    # loop forever
