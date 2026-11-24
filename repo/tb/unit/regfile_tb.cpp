/*
 *  Verifies the register file: synchronous write, asynchronous reads, all 31 writable registers,
 *  read-before-write in the same cycle, x0 hardwired to zero and a0 showing only x10. 
 */

#include "testbench.h"

unsigned int ticks = 0;

class RegfileTestbench : public Testbench
{
protected:
    void initializeInputs() override
    {
        top->A1 = 0;
        top->A2 = 0;
        top->A3 = 0;
        top->WD3 = 0;
        top->WE3 = 0;
        top->clk = 0;
    }
};

TEST_F(RegfileTestbench, WriteThenReadTest)
{
    top->A3 = 5;
    top->WD3 = 10;
    top->WE3 = 1;

    runSimulation(1);          // one cycle: the write happens at the rising edge

    top->A1 = 5;
    top->eval();               // reads are asynchronous, so no clock needed

    EXPECT_EQ(top->RD1, 10);
}

TEST_F(RegfileTestbench, WriteDisabledTest)
{
    top->A1 = 5;
    top->A3 = 5;
    top->WD3 = 10;
    top->WE3 = 0;
    top->eval();
    auto before = top->RD1;

    runSimulation(1);                      // clock edge with writing disabled

    EXPECT_EQ(top->RD1, before);           // nothing should have been written

    top->WE3 = 1;                  // control: now enable the write
    runSimulation(1);

    EXPECT_EQ(top->RD1, 10u);        // the write should have happened this time
}

TEST_F(RegfileTestbench, X0StaysZeroTest)
{
    top->A3 = 0;
    top->WD3 = 0xFFFFEEEE;
    top->WE3 = 1;

    runSimulation(1);

    top->A1 = 0;
    top->A2 = 0;
    top->eval();
    EXPECT_EQ(top->RD1, 0u);
    EXPECT_EQ(top->RD2, 0u);

}

TEST_F(RegfileTestbench, A0OutputTest)
{
    top->A3 = 10;
    top->WD3 = 0xFFFFEEEE;
    top->WE3 = 1;
    
    runSimulation(1);

    EXPECT_EQ(top->a0, 0xFFFFEEEEu);

}


TEST_F(RegfileTestbench, TwoReadPortsTest)
{
    top->WE3 = 1;
    top->A3 = 5;
    top->WD3 = 10;
    runSimulation(1);

    top->A3 = 6;
    top->WD3 = 11;
    runSimulation(1);

    top->A1 = 5;
    top->A2 = 6;
    top->eval();
    EXPECT_EQ(top->RD1, 10);
    EXPECT_EQ(top->RD2, 11);

    // now swap the addresses: each port should follow its own address
    top->A1 = 6;
    top->A2 = 5;
    top->eval();
    EXPECT_EQ(top->RD1, 11);
    EXPECT_EQ(top->RD2, 10);
}

// registers x1 to x31 can be written and keep their own value
TEST_F(RegfileTestbench, AllRegistersWritableTest)
{
    // 0xA5 in the top bits checks all 32 bits stored
    top->WE3 = 1;
    for(unsigned int i = 1; i < 32; i++)
    {
        top->A3 = i;
        top->WD3 = 0xA5000000 + i;
        runSimulation(1);
    }

    // read back only after all the writes so two registers sharing storage would show up
    top->WE3 = 0;
    for(unsigned int i = 1; i < 32; i++)
    {
        top->A1 = i;
        top->eval();
        EXPECT_EQ(top->RD1, 0xA5000000 + i) << "register " << i;
    }

    // x0 must still read 0
    top->A1 = 0;
    top->eval();
    EXPECT_EQ(top->RD1, 0u);
}

// a read in the same cycle as a write sees old value until clock edge
TEST_F(RegfileTestbench, WriteAndReadSameCycleTest)
{
    top->A3 = 5;
    top->WD3 = 10;
    top->WE3 = 1;
    runSimulation(1);

    // write of 20 set up but no clock edge, so x5 still reads 10
    top->A1 = 5;
    top->WD3 = 20;
    top->eval();
    EXPECT_EQ(top->RD1, 10u);

    runSimulation(1);
    EXPECT_EQ(top->RD1, 20u);
}

// a0 shows x10 and only x10
TEST_F(RegfileTestbench, A0OnlyShowsX10Test)
{
    top->A3 = 10;
    top->WD3 = 0x12345678;
    top->WE3 = 1;
    runSimulation(1);
    EXPECT_EQ(top->a0, 0x12345678u);

    // writing a different register must not change a0
    top->A3 = 11;
    top->WD3 = 0xFFFFFFFF;
    runSimulation(1);
    EXPECT_EQ(top->a0, 0x12345678u);
}

// check if all the register values at the start are initialised to 0
TEST_F(RegfileTestbench, AllZeroAtStart)
{
    for(unsigned int i = 0; i < 32; i++){
        top->A1 = i;
        top->A2 = i;
        top->eval();
        EXPECT_EQ(top->RD1, 0u);
        EXPECT_EQ(top->RD2, 0u);
    }
}

// writing the same register twice: the second value replaces the first
TEST_F(RegfileTestbench, Overwrite)
{
    top->WE3 = 1;
    top->A3 = 1;
    top->A1 = 1;
    
    top->WD3 = 10;
    runSimulation(1);
    EXPECT_EQ(top->RD1, 10u);  // first write

    top->WD3 = 5;
    runSimulation(1);
    EXPECT_EQ(top->RD1, 5u);  // second write replaces the first

}

// check a0 == RD1 when A1 = 10
TEST_F(RegfileTestbench, A0MatchesRD1Test)
{
    top->A3 = 10;
    top->WD3 = 0xFFFFEEEE;
    top->WE3 = 1;
    runSimulation(1);

    top->A1 = 10;
    top->eval();

    EXPECT_EQ(top->RD1, 0xFFFFEEEEu);
    EXPECT_EQ(top->a0, top->RD1);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}