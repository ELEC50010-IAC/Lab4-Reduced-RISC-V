/*
 *  Verifies the branch unit: beq, bne, blt, bge (signed), bltu, bgeu (unsigned),
 *  and that unused funct3 codes never take a branch.
 */

#include "base_testbench.h"

unsigned int ticks = 0;

class BranchUnitTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->RD1 = 0;
        top->RD2 = 0;
        top->funct3 = 0;
    }
};

TEST_F(BranchUnitTestbench, BeqTest)
{
    // equal values, branch taken
    top->RD1 = 5;
    top->RD2 = 5;
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);

    // different values, branch not taken
    top->RD2 = 6;
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);
}

TEST_F(BranchUnitTestbench, BneTest)
{
    // different values, branch taken
    top->RD1 = 5;
    top->RD2 = 6;
    top->funct3 = 0b001;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);

    // equal values, branch not taken
    top->RD2 = 5;
    top->funct3 = 0b001;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);
}

TEST_F(BranchUnitTestbench, BltTest)
{
    // 1 < 2, branch taken
    top->RD1 = 1;
    top->RD2 = 2;
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);

    // 2 < 1 is false, branch not taken
    top->RD1 = 2;
    top->RD2 = 1;
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);

    // equal values are not less than, branch not taken
    top->RD1 = 5;
    top->RD2 = 5;
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);

    // signed: 0xFFFFFFFF is -1, and -1 < 1 so branch taken
    top->RD1 = 0xFFFFFFFF;
    top->RD2 = 1;
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);
}

TEST_F(BranchUnitTestbench, BgeTest)
{
    // 2 >= 1, branch taken
    top->RD1 = 2;
    top->RD2 = 1;
    top->funct3 = 0b101;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);

    // equal values count as >= so branch taken
    top->RD1 = 5;
    top->RD2 = 5;
    top->funct3 = 0b101;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);

    // signed: -1 >= 1 is false, branch not taken
    top->RD1 = 0xFFFFFFFF;
    top->RD2 = 1;
    top->funct3 = 0b101;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);
}

TEST_F(BranchUnitTestbench, BltuTest)
{
    // 1 < 2, branch taken
    top->RD1 = 1;
    top->RD2 = 2;
    top->funct3 = 0b110;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);

    // equal values are not less than so branch not taken
    top->RD1 = 5;
    top->RD2 = 5;
    top->funct3 = 0b110;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);

    // unsigned: 0xFFFFFFFF is the largest value, not less than 1 so branch not taken
    top->RD1 = 0xFFFFFFFF;
    top->RD2 = 1;
    top->funct3 = 0b110;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);
}

TEST_F(BranchUnitTestbench, BgeuTest)
{
    // 1 >= 2 is false, branch not taken
    top->RD1 = 1;
    top->RD2 = 2;
    top->funct3 = 0b111;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);

    // equal values count as >= so branch taken
    top->RD1 = 5;
    top->RD2 = 5;
    top->funct3 = 0b111;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);

    // unsigned: 0xFFFFFFFF >= 1 so branch taken
    top->RD1 = 0xFFFFFFFF;
    top->RD2 = 1;
    top->funct3 = 0b111;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 1u);
}

TEST_F(BranchUnitTestbench, UnusedFunct3Test)
{
    // 010 is not a branch, so never taken even with equal values
    top->RD1 = 5;
    top->RD2 = 5;
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);

    // 011 is not a branch either
    top->RD1 = 5;
    top->RD2 = 5;
    top->funct3 = 0b011;
    top->eval();
    EXPECT_EQ(top->BranchTaken, 0u);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}