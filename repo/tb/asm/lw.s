.text
.globl main
main:
    li t0, 0x100            # t0 = 0x100 (data memory address)
    li t1, 0x12345678       # t1 = 0x12345678 (all 4 bytes different)
    sw t1, 0(t0)            # mem[0x100] = t1
    lw a0, 0(t0)            # a0 = mem[0x100] (= 0x12345678)
    bne a0, zero, finish

finish:     # expected result is 0x12345678
    bne a0, zero, finish    # loop forever
