.text
.globl main
main:
    li t0, 0x100            # t0 = 0x100 (data memory address)
    li t1, -128             # t1 = -128, lowest byte is 0x80 (top bit set)
    sb t1, 0(t0)            # mem[0x100] = 0x80
    lb a0, 0(t0)            # a0 = 0x80 sign-extended (= 0xFFFFFF80)
    bne a0, zero, finish

finish:     # expected result is 0xFFFFFF80
    bne a0, zero, finish    # loop forever
