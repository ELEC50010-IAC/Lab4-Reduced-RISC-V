/*
 *  Verifies the ALU: add, sub, and, or, xor, the Zero flag,
 *  and that unused ALUControl codes give 0.
 */

#include "base_testbench.h"

unsigned int ticks = 0;

class AluTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->SrcA = 0;
        top->SrcB = 0;
        top->ALUControl = 0;
        
    }
};

TEST_F(AluTestbench, AddTest)
{
    top->SrcA = 3;
    top->SrcB = 4;
    top->ALUControl = 0;

    top->eval();

    EXPECT_EQ(top->ALUResult, 7u);
}

TEST_F(AluTestbench, WrapAroundTest)
{
    top->SrcA = 0xFFFFFFFF;   // all ones: the largest 32-bit value
    top->SrcB = 1;
    top->ALUControl = 0;

    top->eval();

    EXPECT_EQ(top->ALUResult, 0u); // 0xFFFFFFFF + 1 wraps round to 0
}

TEST_F(AluTestbench, SubTest)
{
    top->ALUControl = 0b0001; // sub

    // Case 1: simple subtraction
    top->SrcA = 7;
    top->SrcB = 4;
    top->eval();
    EXPECT_EQ(top->ALUResult, 3u);

    // Case 2: negative result wraps round. 3 - 4 = -1 stored as all ones
    top->SrcA = 3;
    top->SrcB = 4;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0xFFFFFFFFu);
}

TEST_F(AluTestbench, AndTest)
{
    top->ALUControl = 0b0010; // and

    // Case 1: a result bit is 1 only when both input bits are 1
    top->SrcA = 0b1100;
    top->SrcB = 0b1010;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0b1000u);

    // Case 2: and with 0 clears every bit
    top->SrcA = 0x12345678;
    top->SrcB = 0;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0u);
}

TEST_F(AluTestbench, OrTest)
{
    top->ALUControl = 0b0011; // or

    // Case 1: a result bit is 1 if either or both input bits are 1
    top->SrcA = 0b1100;
    top->SrcB = 0b1010;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0b1110u);

    // Case 2: or with 0 leaves input unchanged
    top->SrcA = 0x12345678;
    top->SrcB = 0;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0x12345678u);
}

TEST_F(AluTestbench, XorTest)
{
    top->ALUControl = 0b0100; // xor

    // Case 1: a result bit is 1 when the two input bits are different
    top->SrcA = 0b1100;
    top->SrcB = 0b1010;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0b0110u);

    // Case 2: an input xor itself clears every bit
    top->SrcA = 0x12345678;
    top->SrcB = 0x12345678;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0u);
}

TEST_F(AluTestbench, ZeroTest)
{
    top->ALUControl = 0;

    // Case 1: identical operands
    top->SrcA = 0xFFFFFFFF;
    top->SrcB = 0xFFFFFFFF;
    top->eval();
    EXPECT_EQ(top->Zero, 1);

    // Case 2: make SrcB differ from SrcA in exactly one bit
    top->SrcB = 0xFFFFFFFE;
    top->eval();
    EXPECT_EQ(top->Zero, 0);
}

TEST_F(AluTestbench, UnusedCodeGivesZeroTest)
{
    // 1111 isn't an agreed operation (only 0000 to 1001 used),
    // so the ALU should fall through to default and give 0
    top->ALUControl = 0b1111;

    // non-zero inputs: if default didn't work, would see 7 instead of 0 
    top->SrcA = 3;
    top->SrcB = 4;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0u);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}