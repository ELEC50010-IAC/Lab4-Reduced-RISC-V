.text
.globl main
main:
    addi t1, zero, 0xFF   # t1 = 255
    addi t2, zero, 0x19   # t2 = 25 which is the delay between state changes
mloop:
    addi a0, zero, 0x0    # set a0 to 0 initially
    jal ra, delay
iloop:
    add a0, a0, a0
    addi a0, a0, 0x1     # transition a0 to the next state a0 = 2*a0 + 1
    jal ra, delay
    bne a0, t1, iloop
    J       mloop        # JUMP back to main loop to reset state
delay:
    mv t3, t2
    delay_loop:
        addi t3, t3, -1
        bne t3, zero, delay_loop
    jalr x0, 0(ra)
   