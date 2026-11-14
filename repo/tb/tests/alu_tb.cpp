/*
 *  Verifies the ALU: addition (ALUctrl = 0) and the EQ flag.
 */

#include "base_testbench.h"

unsigned int ticks = 0;

class AluTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->ALUop1 = 0;
        top->ALUop2 = 0;
        top->ALUctrl = 0;
        
    }
};

TEST_F(AluTestbench, AddTest)
{
    const int DATA_WIDTH = 32;
    for (int i = 0; i < 32; i++)
    top->ALUop1 = 3;
    top->ALUop2 = 4;
    top->ALUctrl = 0;

    top->eval();

    EXPECT_EQ(top->ALUout, 7u);
}

TEST_F(AluTestbench, WrapAroundTest)
{
    top->ALUop1 = 0xFFFFFFFF;   // all ones: the largest 32-bit value
    top->ALUop2 = 1;
    top->ALUctrl = 0;

    top->eval();

    EXPECT_EQ(top->ALUout, 0u); // 0xFFFFFFFF + 1 wraps round to 0
}

TEST_F(AluTestbench, EqTest)
{
    top->ALUctrl = 0;

    // Case 1: identical operands
    top->ALUop1 = 0xFFFFFFFF;
    top->ALUop2 = 0xFFFFFFFF;
    top->eval();
    EXPECT_EQ(top->EQ, 1);

    // Case 2: make ALUop2 differ from ALUop1 in exactly one bit
    top->ALUop2 = 0xFFFFFFFE;
    top->eval();
    EXPECT_EQ(top->EQ, 0);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}