/*
 *  Verifies the register file: synchronous write, asynchronous reads,
 *  x0 hardwired to zero, and the a0 output.
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

    EXPECT_EQ(top->RD1, 10);        // the write should have happened this time
}

TEST_F(RegfileTestbench, X0StaysZeroTest)
{
    top->A3 = 0;
    top->WD3 = 10;
    top->WE3 = 1;

    runSimulation(1);

    top->A1 = 0;
    top->eval();
    EXPECT_EQ(top->RD1, 0);

}

TEST_F(RegfileTestbench, A0OutputTest)
{
    top->A3 = 10;
    top->WD3 = (top->a0 > 0) ? 0 : 1;
    top->WE3 = 1;
    
    runSimulation(1);

    EXPECT_EQ(top->WD3, top->a0);

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

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}