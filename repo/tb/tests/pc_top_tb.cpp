// Check the program counter's reset, clock timing and address calculations
#include "base_testbench.h"

#include <cstdint>

class PcTestbench : public BaseTestbench
{
protected:
    // Begin with a low clock and the normal next-instruction path selected
    const std::uint32_t RESET = 0xBFC00000u;
    void initializeInputs() override
    {
        top->clk = 0;
        top->rst = 0;
        top->PCSrc = 0;
        top->ImmExt = 0;
        top->ALUResult = 0;
    }

    // Give each test a known PC of zero by asserting and releasing reset
    void SetUp() override
    {
        BaseTestbench::SetUp();
        top->eval();
        top->rst = 1;
        top->eval();
        top->rst = 0;
        top->eval();
    }

    // Evaluate low, high and low so the DUT sees one rising edge and one falling edge
    void clockCycle()
    {
        top->clk = 0;
        top->eval();
        top->clk = 1;
        top->eval();
        top->clk = 0;
        top->eval();
    }
};

// Reset must clear the PC immediately without waiting for a clock edge
TEST_F(PcTestbench, AsynchronousResetBetweenClockEdges)
{
    clockCycle();
    clockCycle();
    ASSERT_EQ(top->PC, RESET + 8u);

    // Reset while the clock remains low
    top->rst = 1;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 0u);
    top->rst = 0;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 0u);

    top->clk = 1;
    top->eval();
    ASSERT_EQ(top->PC, RESET + 4u);

    // Reset while the clock remains high
    top->rst = 1;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 0u);
    top->rst = 0;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 0u);
}

// While reset is asserted, neither a branch nor a normal step may advance the PC
TEST_F(PcTestbench, ResetHasPriorityOverBothPcPaths)
{
    clockCycle();
    ASSERT_EQ(top->PC, RESET + 4u);

    top->PCSrc = 1;
    top->ImmExt = 24;
    top->rst = 1;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 0u);
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 0u);

    top->PCSrc = 0;
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 0u);

    top->rst = 0;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 0u);
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 4u);
}

// The normal path advances by four bytes regardless of the immediate input
TEST_F(PcTestbench, SequentialPathAddsFourAndIgnoresImmediate)
{
    const std::uint32_t immediates[] = {0u, 12u, 0xfffffffcu, 0x12345678u};
    const std::uint32_t expected[] = {4u, 8u, 12u, 16u};

    top->PCSrc = 0;
    for (unsigned int i = 0; i < 4; ++i)
    {
        SCOPED_TRACE(i);
        top->ImmExt = immediates[i];
        clockCycle();
        EXPECT_EQ(top->PC, RESET + expected[i]);
    }
}

// Branch offsets are added to the current PC in bytes, including backwards and zero steps
TEST_F(PcTestbench, BranchesUsePositiveNegativeAndZeroByteOffsets)
{
    top->PCSrc = 1;
    top->ImmExt = 16;
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 16u);

    top->ImmExt = 12;
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 28u);

    top->ImmExt = 0xfffffff8u; // Minus eight bytes
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 20u);

    top->ImmExt = 0;
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 20u);

    top->ImmExt = 4;
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 24u);
}

// Changing branch inputs or lowering the clock must not update the stored address
TEST_F(PcTestbench, HoldsWithoutRisingEdgeIncludingFallingEdge)
{
    clockCycle();
    ASSERT_EQ(top->PC, RESET + 4u);

    top->PCSrc = 1;
    top->ImmExt = 12;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 4u);
    top->eval();
    EXPECT_EQ(top->PC, RESET + 4u);

    top->clk = 1;
    top->eval();
    ASSERT_EQ(top->PC, RESET + 16u);

    top->ImmExt = 0xfffffffcu;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 16u);
    top->PCSrc = 0;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 16u);

    top->clk = 0;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 16u);
    top->clk = 1;
    top->eval();
    EXPECT_EQ(top->PC, RESET + 20u);
}

// Address arithmetic wraps around when it passes either end of the 32-bit range
TEST_F(PcTestbench, ArithmeticWrapsAtThirtyTwoBits)
{
    top->PCSrc = 1;
    top->ImmExt = 0x40400000u;  // RESET + this overflows past the top
    clockCycle();
    EXPECT_EQ(top->PC, 0x00000000u);    // where did it land?

    top->ImmExt = 0xfffffffcu;  // minus four from there
    clockCycle();
    EXPECT_EQ(top->PC, 0xfffffffcu);    // wraps backwards to...?

    top->PCSrc = 0;             // normal step, PC + 4
    clockCycle();
    EXPECT_EQ(top->PC, 0x00000000u);    // wraps forwards to...?
}


// jalr: PC jumps to the ALU result with bit 0 cleared
TEST_F(PcTestbench, JalrJumpsToAluResultWithBitZeroCleared)
{
    top->PCSrc = 2;
    top->ALUResult = 0x00000101u;   // odd address
    clockCycle();
    EXPECT_EQ(top->PC, 0x00000100u);
}

// PCSrc = 3 is unused and must behave like a normal step
TEST_F(PcTestbench, UnusedPcSrcThreeStepsByFour)
{
    top->PCSrc = 3;
    top->ImmExt = 0x100u;           // should be ignored
    clockCycle();
    EXPECT_EQ(top->PC, RESET + 4u);
}

// PCPlus4 and PCTarget are combinational: they update with no clock edge
TEST_F(PcTestbench, PcPlus4AndPcTargetUpdateWithoutClock)
{
    top->ImmExt = 0x100u;
    top->eval();                    // no clock
    EXPECT_EQ(top->PC, RESET);        // PC must not have moved
    EXPECT_EQ(top->PCPlus4, RESET + 4u);
    EXPECT_EQ(top->PCTarget, RESET + top->ImmExt);
}



// Pass command-line options to the simulator and run the GoogleTest cases
int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
