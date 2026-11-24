.text
.globl main
main:
    lui a0, 0x12345         # a0 = 0x12345 << 12 (= 0x12345000)
    bne a0, zero, finish

finish:     # expected result is 0x12345000
    bne a0, zero, finish    # loop forever
