// Check the program counter's reset, clock timing and address calculations
#include "base_testbench.h"

#include <cstdint>

class PcTestbench : public BaseTestbench
{
protected:
    // Begin with a low clock and the normal next-instruction path selected
    void initializeInputs() override
    {
        top->clk = 0;
        top->rst = 0;
        top->PCsrc = 0;
        top->ImmOp = 0;
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
    ASSERT_EQ(top->PC, 8u);

    // Reset while the clock remains low
    top->rst = 1;
    top->eval();
    EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    top->eval();
    EXPECT_EQ(top->PC, 0u);

    top->clk = 1;
    top->eval();
    ASSERT_EQ(top->PC, 4u);

    // Reset while the clock remains high
    top->rst = 1;
    top->eval();
    EXPECT_EQ(top->PC, 0u);
    top->rst = 0;
    top->eval();
    EXPECT_EQ(top->PC, 0u);
}

// While reset is asserted, neither a branch nor a normal step may advance the PC
TEST_F(PcTestbench, ResetHasPriorityOverBothPcPaths)
{
    clockCycle();
    ASSERT_EQ(top->PC, 4u);

    top->PCsrc = 1;
    top->ImmOp = 24;
    top->rst = 1;
    top->eval();
    EXPECT_EQ(top->PC, 0u);
    clockCycle();
    EXPECT_EQ(top->PC, 0u);

    top->PCsrc = 0;
    clockCycle();
    EXPECT_EQ(top->PC, 0u);

    top->rst = 0;
    top->eval();
    EXPECT_EQ(top->PC, 0u);
    clockCycle();
    EXPECT_EQ(top->PC, 4u);
}

// The normal path advances by four bytes regardless of the immediate input
TEST_F(PcTestbench, SequentialPathAddsFourAndIgnoresImmediate)
{
    const std::uint32_t immediates[] = {0u, 12u, 0xfffffffcu, 0x12345678u};
    const std::uint32_t expected[] = {4u, 8u, 12u, 16u};

    top->PCsrc = 0;
    for (unsigned int i = 0; i < 4; ++i)
    {
        SCOPED_TRACE(i);
        top->ImmOp = immediates[i];
        clockCycle();
        EXPECT_EQ(top->PC, expected[i]);
    }
}

// Branch offsets are added to the current PC in bytes, including backwards and zero steps
TEST_F(PcTestbench, BranchesUsePositiveNegativeAndZeroByteOffsets)
{
    top->PCsrc = 1;
    top->ImmOp = 16;
    clockCycle();
    EXPECT_EQ(top->PC, 16u);

    top->ImmOp = 12;
    clockCycle();
    EXPECT_EQ(top->PC, 28u);

    top->ImmOp = 0xfffffff8u; // Minus eight bytes
    clockCycle();
    EXPECT_EQ(top->PC, 20u);

    top->ImmOp = 0;
    clockCycle();
    EXPECT_EQ(top->PC, 20u);

    top->ImmOp = 4;
    clockCycle();
    EXPECT_EQ(top->PC, 24u);
}

// Changing branch inputs or lowering the clock must not update the stored address
TEST_F(PcTestbench, HoldsWithoutRisingEdgeIncludingFallingEdge)
{
    clockCycle();
    ASSERT_EQ(top->PC, 4u);

    top->PCsrc = 1;
    top->ImmOp = 12;
    top->eval();
    EXPECT_EQ(top->PC, 4u);
    top->eval();
    EXPECT_EQ(top->PC, 4u);

    top->clk = 1;
    top->eval();
    ASSERT_EQ(top->PC, 16u);

    top->ImmOp = 0xfffffffcu;
    top->eval();
    EXPECT_EQ(top->PC, 16u);
    top->PCsrc = 0;
    top->eval();
    EXPECT_EQ(top->PC, 16u);

    top->clk = 0;
    top->eval();
    EXPECT_EQ(top->PC, 16u);
    top->clk = 1;
    top->eval();
    EXPECT_EQ(top->PC, 20u);
}

// Address arithmetic wraps around when it passes either end of the 32-bit range
TEST_F(PcTestbench, ArithmeticWrapsAtThirtyTwoBits)
{
    top->PCsrc = 1;
    top->ImmOp = 0xfffffffcu; // Zero minus four wraps to the top of the address space
    clockCycle();
    EXPECT_EQ(top->PC, 0xfffffffcu);

    top->PCsrc = 0;
    clockCycle();
    EXPECT_EQ(top->PC, 0u);

    top->PCsrc = 1;
    top->ImmOp = 0xfffffffcu;
    clockCycle();
    EXPECT_EQ(top->PC, 0xfffffffcu);
    top->ImmOp = 8;
    clockCycle();
    EXPECT_EQ(top->PC, 4u);
}

// Pass command-line options to the simulator and run the GoogleTest cases
int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
