/*
 *  Per-instruction tests: a small program per RV32I instruction which runs on the whole CPU.
 *  Each program leaves its result in a0 which the test checks.
 */

#include "cpu_testbench.h"

#define CYCLES 10000

// add: 12 + 30 = 42
TEST_F(CpuTestbench, AddTest)
{
    setupTest("add");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 42u);
}

// sub: 10 - 3 = 7
TEST_F(CpuTestbench, SubTest)
{
    setupTest("sub");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 7u);
}

// and: 12 & 10 = 8
TEST_F(CpuTestbench, AndTest)
{
    setupTest("and");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 8u);
}

// or: 10 | 12 = 14
TEST_F(CpuTestbench, OrTest)
{
    setupTest("or");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 14u);
}

// xor: 10 ^ 12 = 6
TEST_F(CpuTestbench, XorTest)
{
    setupTest("xor");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 6u);
}

// sll: 1 << 4 = 16
TEST_F(CpuTestbench, SllTest)
{
    setupTest("sll");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 16u);
}

// srl: 64 >> 2 = 16
TEST_F(CpuTestbench, SrlTest)
{
    setupTest("srl");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 16u);
}

// sra: -64 >>> 2 = -16 (0xFFFFFFF0), sign kept
TEST_F(CpuTestbench, SraTest)
{
    setupTest("sra");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 0xFFFFFFF0u);
}

// slt: signed, -1 < 1 = 1
TEST_F(CpuTestbench, SltTest)
{
    setupTest("slt");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 1u);
}

// sltu: unsigned, -1 is 0xFFFFFFFF (the largest number) so 1 < 0xFFFFFFFF = 1
TEST_F(CpuTestbench, SltuTest)
{
    setupTest("sltu");
    initSimulation();
    runSimulation(CYCLES);
    EXPECT_EQ(top_->a0, 1u);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
