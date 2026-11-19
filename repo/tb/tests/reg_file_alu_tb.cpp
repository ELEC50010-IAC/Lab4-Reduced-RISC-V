/*
 *  Verifies the results of the mux, exits with a 0 on success.
 */

#include "base_testbench.h"

Vdut *top;
VerilatedVcdC *tfp;
unsigned int ticks = 0;

class RegFileAluTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->clk = 0;
        top->wen = 0;
        top->alu_src =0;
        top->r_ad1 = 0;
        top->r_ad2 = 0;
        top->w_ad = 0;
        top->imm_op = 0;
        top->alu_opc = 0;
        top->eval();
        // output: eq
    }

    void tick()
    { 
        top->clk = 0; top->eval();
        top->clk = 1;top->eval();
    }

    void writeImm(int rd, uint32_t value){
        top->r_ad1 = 0;
        top->alu_src = 1;//immediate operand
        top->imm_op = value;//set value given to imm
        top->alu_opc = 0; //add in opc 
        top->w_ad = rd;
        top->wen = 1;//write enable
        tick();
        top->wen = 0; //disable write 
    }

    void writeReg(int rd, int rs1, int rs2, int opc){
        top->r_ad1 =rs1;
        top->r_ad2 =rs2;
        top->alu_src =0;//reg + reg
        top->imm_op = 0;
        top->alu_opc = 0;
        top-> w_ad = rd;
        top->wen = 1;
        tick();
        top->wen = 0;

    }

    bool regEquals(int rs, uint32_t value){
        top->wen = 0;
        top->r_ad1 = rs;
        top->alu_src = 1;
        top->imm_op = value;
        top->eval();
        return top->eq;
    }
};


TEST_F(RegFileAluTestbench,WriteRead)
{
    writeImm(1, 5);
    EXPECT_EQ(regEquals(1, 5), true);
}


TEST_F(RegFileAluTestbench, X0AlwaysZero)
{
    writeImm(0, 7);              // try to write to x0
    EXPECT_TRUE(regEquals(0, 0));
}

TEST_F(RegFileAluTestbench, DisableWen)
{
    writeImm(2, 3);
    top->r_ad1 = 0;
    top->alu_src = 1;
    top->imm_op = 9;
    top->w_ad = 2;
    top->wen = 0;              
    tick();
    
    EXPECT_TRUE(regEquals(2, 3)); 
}

TEST_F(RegFileAluTestbench, AddTwoRegisters)
{
    writeImm(1, 5);
    writeImm(2, 3);
    writeReg(3, 1, 2, 0);    
    EXPECT_TRUE(regEquals(3, 8));
}

TEST_F(RegFileAluTestbench, EqComparesTwoRegisters)
{
    writeImm(1, 5);
    writeImm(2, 5);
    writeImm(3, 8);
    top->alu_src = 0;            
    top->r_ad1 = 1;
    top->r_ad2 = 2;
    top->eval();
    EXPECT_EQ(top->eq, 1);     
    top->r_ad2 = 3;
    top->eval();
    EXPECT_EQ(top->eq, 0);       
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
