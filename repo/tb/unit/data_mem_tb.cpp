#include <cstdint>
#include <gtest/gtest.h>
#include "Vdut.h"

namespace {

constexpr std::uint32_t base = 0x10000;
constexpr unsigned byteAccess = 0;
constexpr unsigned halfAccess = 1;
constexpr unsigned wordAccess = 2;
constexpr unsigned unsignedByte = 4;
constexpr unsigned unsignedHalf = 5;

class DataMemTest : public ::testing::Test {
protected:
    Vdut dut;

    void SetUp() override {
        dut.clk = 0;
        dut.WE = 0;
        dut.A = base;
        dut.WD = 0;
        dut.funct3 = wordAccess;
        dut.eval();
    }

    void TearDown() override {
        dut.final();
    }

    void store(std::uint32_t address, unsigned width, std::uint32_t value) {
        dut.A = address;
        dut.funct3 = width;
        dut.WD = value;
        dut.WE = 1;
        dut.eval();
        dut.clk = 1;
        dut.eval();
        dut.clk = 0;
        dut.WE = 0;
        dut.eval();
    }

    std::uint32_t load(std::uint32_t address, unsigned width) {
        dut.A = address;
        dut.funct3 = width;
        dut.eval();
        return dut.RD;
    }
};

TEST_F(DataMemTest, ByteStorePreservesNeighboursAndLoadsRespectSignedness) {
    store(base, wordAccess, 0x44332211);
    store(base + 1, byteAccess, 0x12345680);

    EXPECT_EQ(load(base + 1, byteAccess), 0xffffff80u);
    EXPECT_EQ(load(base + 1, unsignedByte), 0x80u);
    EXPECT_EQ(load(base, byteAccess), 0x11u);
    EXPECT_EQ(load(base, wordAccess), 0x44338011u);
}

TEST_F(DataMemTest, HalfwordStorePreservesNeighboursAndLoadsRespectSignedness) {
    store(base, wordAccess, 0x44332211);
    store(base + 2, halfAccess, 0xaabb8001);

    EXPECT_EQ(load(base + 2, halfAccess), 0xffff8001u);
    EXPECT_EQ(load(base + 2, unsignedHalf), 0x8001u);
    EXPECT_EQ(load(base, halfAccess), 0x2211u);
    EXPECT_EQ(load(base, wordAccess), 0x80012211u);
}

TEST_F(DataMemTest, WordUsesFourConsecutiveBytesInLittleEndianOrder) {
    store(base, wordAccess, 0x89abcdef);

    EXPECT_EQ(load(base, unsignedByte), 0xefu);
    EXPECT_EQ(load(base + 1, unsignedByte), 0xcdu);
    EXPECT_EQ(load(base + 2, unsignedByte), 0xabu);
    EXPECT_EQ(load(base + 3, unsignedByte), 0x89u);
    EXPECT_EQ(load(base, wordAccess), 0x89abcdefu);
}

TEST_F(DataMemTest, OnlyAnEnabledRisingEdgeWrites) {
    store(base, wordAccess, 0x11223344);

    dut.WD = 0x55667788;
    dut.clk = 1;
    dut.eval();
    EXPECT_EQ(dut.RD, 0x11223344u);

    dut.WE = 1;
    dut.eval();
    EXPECT_EQ(dut.RD, 0x11223344u);
    dut.clk = 0;
    dut.eval();
    EXPECT_EQ(dut.RD, 0x11223344u);
    dut.clk = 1;
    dut.eval();
    EXPECT_EQ(dut.RD, 0x55667788u);
}

}  // namespace
