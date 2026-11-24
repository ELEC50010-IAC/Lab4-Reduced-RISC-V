.text
.globl main
main:
    auipc a0, 1             # a0 = PC + (1 << 12) = 0xBFC00000 + 0x1000 (= 0xBFC01000)
    bne a0, zero, finish

finish:     # expected result is 0xBFC01000
    bne a0, zero, finish    # loop forever
