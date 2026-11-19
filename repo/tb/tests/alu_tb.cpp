/*
 *  Verifies the results of the mux, exits with a 0 on success.
 */

#include "base_testbench.h"

Vdut *top;
VerilatedVcdC *tfp;
unsigned int ticks = 0;

class AluTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->operand1 = 0;
        top->operand2 = 0;
        top->alu_opc = 0;
    }
};

TEST_F(AluTestbench, AddCorrectly)
{
    top->operand1 = 5;
    top->operand2 = 6;
    top->alu_opc = 0;

    top->eval();

    EXPECT_EQ(top->alu_out, 11);
}

TEST_F(AluTestbench, AddOverflowWrapAround)
{
    top->operand1 = 1;
    top->operand2 = 0xFFFFFFFF;
    top->alu_opc = 0;

    top->eval();

    EXPECT_EQ(top->alu_out, 0 );
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
