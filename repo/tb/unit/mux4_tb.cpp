#include <gtest/gtest.h>
#include "Vdut.h"

TEST(Mux4Test, EachSelectForwardsTheCorrespondingFullWord) {
    Vdut dut;
    dut.in0 = 0x84210001;
    dut.in1 = 0x42310002;
    dut.in2 = 0x21430004;
    dut.in3 = 0x18270008;

    dut.sel = 0;
    dut.eval();
    EXPECT_EQ(dut.out, 0x84210001u);
    dut.sel = 1;
    dut.eval();
    EXPECT_EQ(dut.out, 0x42310002u);
    dut.sel = 2;
    dut.eval();
    EXPECT_EQ(dut.out, 0x21430004u);
    dut.sel = 3;
    dut.eval();
    EXPECT_EQ(dut.out, 0x18270008u);

    dut.in3 = 0xdeadbeef;
    dut.eval();
    EXPECT_EQ(dut.out, 0xdeadbeefu);
    dut.final();
}
