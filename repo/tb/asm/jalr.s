.text
.globl main
main:
    jal t0, here            # t0 = address of here (0xBFC00004)
here:
    jalr ra, 12(t0)         # jump to here + 12 = target, ra = 0xBFC00008
    li a0, 2                # only reached if jalr didn't jump
    j finish
target:
    mv a0, ra               # a0 = return address (= 0xBFC00008)

finish:     # expected result is 0xBFC00008
    bne a0, zero, finish    # loop forever
