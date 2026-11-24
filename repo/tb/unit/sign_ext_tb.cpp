/*
 *  Review tests for Ahsan's sign extend: each immediate type (I, S, B, U, J),
 *  positive and negative values, the limits, and the bits B and J types scramble.
 */

#include "base_testbench.h"

unsigned int ticks = 0;

class SignExtTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->Instr = 0;
        top->ImmSrc = 0;
    }
};

// I-type: 12-bit immediate in the top bits, sign-extended 
TEST_F(SignExtTestbench, ITypeTest)
{
    top->ImmSrc = 0;

    // addi x1, x0, 5
    top->Instr = 0x00500093u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 5u);

    // addi x1, x0, -1
    top->Instr = 0xFFF00093u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0xFFFFFFFFu);

    // largest: +2047
    top->Instr = 0x7FF00093u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0x000007FFu);

    // smallest: -2048
    top->Instr = 0x80000093u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0xFFFFF800u);
}

// S-type (stores): immediate split in two parts
TEST_F(SignExtTestbench, STypeTest)
{
    top->ImmSrc = 1;

    // sw x2, 8(x1)
    top->Instr = 0x0020A423u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 8u);

    // sw x2, -4(x1)
    top->Instr = 0xFE20AE23u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0xFFFFFFFCu);
}

// B-type (branches): scrambled bits, bit 0 always 0
TEST_F(SignExtTestbench, BTypeTest)
{
    top->ImmSrc = 2;

    // beq x1, x2, +8
    top->Instr = 0x00208463u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 8u);

    // beq x1, x2, -4
    top->Instr = 0xFE208EE3u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0xFFFFFFFCu);

    // offset 2048 (only imm bit 11 set)
    top->Instr = 0x000000E3u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0x00000800u);
}

// U-type (lui/auipc): top 20 bits, bottom 12 are 0
TEST_F(SignExtTestbench, UTypeTest)
{
    top->ImmSrc = 3;

    // lui x1, 0x12345
    top->Instr = 0x123450B7u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0x12345000u);

    // lui x1, 0xFFFFF
    top->Instr = 0xFFFFF0B7u;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0xFFFFF000u);
}

// J-type (jal): scrambled bits, bit 0 always 0
TEST_F(SignExtTestbench, JTypeTest)
{
    top->ImmSrc = 4;

    // jal x1, +8
    top->Instr = 0x008000EFu;
    top->eval();
    EXPECT_EQ(top->ImmExt, 8u);

    // jal x1, -4
    top->Instr = 0xFFDFF0EFu;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0xFFFFFFFCu);

    // offset 2048 (only imm bit 11 set)
    top->Instr = 0x001000EFu;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0x00000800u);

    // offset 4096 (only imm bit 12 set)
    top->Instr = 0x000010EFu;
    top->eval();
    EXPECT_EQ(top->ImmExt, 0x00001000u);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
