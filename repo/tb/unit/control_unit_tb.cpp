// Checks the control signals for the reduced addi and bne instruction set
#include "base_testbench.h"

class ControlUnitTestbench : public BaseTestbench
{
protected:
    // Start each test with neutral inputs before selecting an instruction
    void initializeInputs() override
    {
        top->op = 0;
        top->funct3 = 0;
        top->funct7 = 0;
        top->EQ = 0;
    }

    // Check all five outputs for the instruction being tested
    void expectControls(unsigned regWrite, unsigned aluSrc, unsigned immSrc,
                        unsigned pcSrc, unsigned aluCtrl)
    {
        EXPECT_EQ(top->RegWrite, regWrite);
        EXPECT_EQ(top->ALUSrc, aluSrc);
        EXPECT_EQ(top->ImmSrc, immSrc);
        EXPECT_EQ(top->PCSrc, pcSrc);
        EXPECT_EQ(top->ALUCtrl, aluCtrl);
    }
};

// addi must select addition, use the immediate and enable register writing
TEST_F(ControlUnitTestbench, AddiUsesImmediateAndWritesRegister)
{
    top->op = 0x13;
    top->funct3 = 0;

    for (unsigned equal = 0; equal < 2; ++equal)
    {
        for (unsigned immediateBit = 0; immediateBit < 2; ++immediateBit)
        {
            SCOPED_TRACE(::testing::Message() << "EQ=" << equal
                                            << " instruction bit 30=" << immediateBit);
            top->EQ = equal;
            // This input is an immediate bit in addi, not an arithmetic selector
            top->funct7 = immediateBit;
            top->eval();
            expectControls(1, 1, 0, 0, 0);
        }
    }
}

// bne must branch only when the compared register values differ
TEST_F(ControlUnitTestbench, BneBranchesOnlyWhenOperandsDiffer)
{
    top->op = 0x63;
    top->funct3 = 1;

    for (unsigned offsetBit = 0; offsetBit < 2; ++offsetBit)
    {
        top->funct7 = offsetBit;
        top->EQ = 0;
        top->eval();
        expectControls(0, 0, 2, 1, 1);

        top->EQ = 1;
        top->eval();
        expectControls(0, 0, 2, 0, 1);
    }
}

// Changing instructions must replace the previous control decisions
TEST_F(ControlUnitTestbench, InstructionChangesDoNotRetainPreviousControls)
{
    top->op = 0x13;
    top->funct3 = 0;
    top->eval();
    expectControls(1, 1, 0, 0, 0);

    top->op = 0x63;
    top->funct3 = 1;
    top->EQ = 0;
    top->eval();
    expectControls(0, 0, 2, 1, 1);

    top->op = 0x13;
    top->funct3 = 0;
    top->eval();
    expectControls(1, 1, 0, 0, 0);
}

// Unknown opcodes must leave register writing and branching disabled
TEST_F(ControlUnitTestbench, UnsupportedOpcodesDoNotWriteOrBranch)
{
    // Unsupported instructions are not implemented; these check safe defaults
    const unsigned opcodes[] = {0x00, 0x03, 0x23, 0x33, 0x6f, 0x7f};
    for (unsigned opcode : opcodes)
    {
        top->op = opcode;
        top->funct3 = 0;
        top->eval();
        expectControls(0, 0, 0, 0, 0);
    }
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
