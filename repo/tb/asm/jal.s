.text
.globl main
main:
    jal ra, target          # jump to target, ra = 0xBFC00000 + 4
    li a0, 2                # only reached if jal didn't jump
    j finish
target:
    mv a0, ra               # a0 = return address (= 0xBFC00004)

finish:     # expected result is 0xBFC00004
    bne a0, zero, finish    # loop forever
