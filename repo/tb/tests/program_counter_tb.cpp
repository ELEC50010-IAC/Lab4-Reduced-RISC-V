/*
 *  Verifies the results of the mux, exits with a 0 on success.
 */

#include "base_testbench.h"

Vdut *top;
VerilatedVcdC *tfp;
unsigned int ticks = 0;

class ProgramCounterTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->clk = 0;
        top->rst = 0;
        top->pc_src = 0;
        top->imm_op = 0;
        // output: pc
    }

    void tick()
    { 
        top->clk = 0; top->eval();
        top->clk = 1;top->eval();
    }
};

TEST_F(ProgramCounterTestbench,ResetsCorrectly)
{
    top->rst = 1;//set rst = 0, after one clock cycle output should be 0
    tick();

    EXPECT_EQ(top->pc, 0);
}

TEST_F(ProgramCounterTestbench,PCIncrementsCorrectly)
{
    top->rst = 1;
    tick();
    top->rst = 0;
    tick();
    EXPECT_EQ(top->pc, 4);
    tick();
    EXPECT_EQ(top->pc, 8);
}

TEST_F(ProgramCounterTestbench,ImmediateIncrCorrectly)
{
    top->rst=1;
    tick();
    top->rst=0;
    top->pc_src=1;//decides that we do pc + imm_op instead of pc + 4
    top->imm_op=9;
    tick();
    EXPECT_EQ(top->pc,9);
}

int main(int argc, char **argv)
{
    top = new Vdut;
    tfp = new VerilatedVcdC;

    Verilated::traceEverOn(true);
    top->trace(tfp, 99);
    tfp->open("waveform.vcd");

    testing::InitGoogleTest(&argc, argv);
    auto res = RUN_ALL_TESTS();

    top->final();
    tfp->close();

    delete top;
    delete tfp;

    return res;
}
