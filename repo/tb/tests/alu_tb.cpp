/*
 *  Verifies the ALU: addition (ALUControl = 0) and the Zero flag.
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


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}