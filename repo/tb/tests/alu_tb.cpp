/*
 *  Verifies the ALU: add, sub, and, or, xor, slt, sltu, sll, srl, sra and the Zero flag,
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

TEST_F(AluTestbench, SltTest)
{
    top->ALUControl = 0b0101; // slt

    // Case 1: 3 < 5 is true, so result is 1
    top->SrcA = 3;
    top->SrcB = 5;
    top->eval();
    EXPECT_EQ(top->ALUResult, 1u);

    // Case 2: 5 < 3 is false, so result is 0 
    top->SrcA = 5;
    top->SrcB = 3;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0u);

    // Case 3: signed, all ones is -1, -1 < 1 is true, so result is 1
    top->SrcA = 0xFFFFFFFF;
    top->SrcB = 1;
    top->eval();
    EXPECT_EQ(top->ALUResult, 1u);
}

TEST_F(AluTestbench, SltuTest)
{
    top->ALUControl = 0b0110; // sltu

    // Case 1: 3 < 5 is true, so result is 1
    top->SrcA = 3;
    top->SrcB = 5;
    top->eval();
    EXPECT_EQ(top->ALUResult, 1u);

    // Case 2: 5 < 3 is false, so result is 0 
    top->SrcA = 5;
    top->SrcB = 3;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0u);

    // Case 3: unsigned, all ones is 4,294,967,295, which isn't < 1, so result is 0
    top->SrcA = 0xFFFFFFFF;
    top->SrcB = 1;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0u);
}

TEST_F(AluTestbench, SllTest)
{
    top->ALUControl = 0b0111; // sll

    // Case 1: basic shift left, 0001 shifts twice to 0100
    top->SrcA = 0b0001;
    top->SrcB = 2;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0b0100u);

    // Case 2: top bit falls off the end (doesn't wrap around), bottom 1 moves to 2
    top->SrcA = 0x80000001;
    top->SrcB = 1;
    top->eval();
    EXPECT_EQ(top->ALUResult, 2u);

    // Case 3: only the bottom 5 bits of SrcB count, 100001 -> 00001, so shift by 1
    top->SrcA = 1;
    top->SrcB = 0b100001;
    top->eval();
    EXPECT_EQ(top->ALUResult, 2u);
}

TEST_F(AluTestbench, SrlTest)
{
    top->ALUControl = 0b1000; // srl

    // Case 1: basic shift right, 1000 shifts twice to 0010
    top->SrcA = 0b1000;
    top->SrcB = 2;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0b0010u);

    // Case 2: 0s come in from the left
    top->SrcA = 0xF0000000;
    top->SrcB = 4;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0x0F000000u);

    // Case 3: only the bottom 5 bits of SrcB count, 100001 -> 00001, so shift by 1
    top->SrcA = 0b1000;
    top->SrcB = 0b100001;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0b0100u);
}

TEST_F(AluTestbench, SraTest)
{
    top->ALUControl = 0b1001; // sra

    // Case 1: top bit is 1, so 1s come in (remains negative after shift)
    top->SrcA = 0xF0000000;
    top->SrcB = 4;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0xFF000000u);

    // Case 2: top bit is 0, so 0s come in (shift acts like srl)
    top->SrcA = 0x70000000;
    top->SrcB = 4;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0x07000000u);

    // Case 3: only the bottom 5 bits of SrcB count, 100100 -> 00100, so shift by 4
    top->SrcA = 0xF0000000;
    top->SrcB = 0b100100;
    top->eval();
    EXPECT_EQ(top->ALUResult, 0xFF000000u);
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