// Checks instruction reads, byte order and address changes in the instruction memory
#include "base_testbench.h"

#include <cstdint>
#include <fstream>

class InstructionMemoryTestbench : public BaseTestbench
{
protected:
    // Begin each test at the first instruction address
    void initializeInputs() override
    {
        top->A = 0;
    }

    // Run ./compile.sh asm/program.S from repo/tb before running this test
        // Write a known image so the test does not depend on whichever program ran last
    void SetUp() override
    {
        std::ofstream image("program.hex");
        for (unsigned int i = 0; i < 32; ++i)
        {
            image << std::hex << i << "\n";   // the byte stored at address i
        }
        image.close();
        BaseTestbench::SetUp();               // creates the model, which loads program.hex
    }

    // A missing image can stop setup before the simulated memory is created
    void TearDown() override
    {
        if (top)
        {
            BaseTestbench::TearDown();
        }
    }

    // Memory reads update when the address changes, without a clock edge
    void expectRead(std::uint32_t address, std::uint32_t expected)
    {
        SCOPED_TRACE(address);
        top->A = address;
        top->eval();
        EXPECT_EQ(top->RD, expected);
    }
};

// Every word in the known image must be read back correctly
TEST_F(InstructionMemoryTestbench, ReadsKnownInstructionWords)
{
    expectRead(0, 0x03020100u);
    expectRead(4, 0x07060504u);
    expectRead(8, 0x0B0A0908u);
    expectRead(12, 0x0F0E0D0Cu);
    expectRead(16, 0x13121110u);
    expectRead(20, 0x17161514u);
    expectRead(24, 0x1B1A1918u);
    expectRead(28, 0x1F1E1D1Cu);
}

// Distinct bytes reveal whether a word has been assembled in the wrong order
TEST_F(InstructionMemoryTestbench, AssemblesDistinctBytesInLittleEndianOrder)
{
    // Bytes 14 15 16 17 at addresses 20..23 must assemble as 0x17161514
    expectRead(20, 0x14u | (0x15u << 8) | (0x16u << 16) | (0x17u << 24));
}

// A new address must produce its word without any clock activity
TEST_F(InstructionMemoryTestbench, AddressChangesUpdateOutputWithoutAClock)
{
    expectRead(0, 0x03020100u);
    expectRead(12, 0x0F0E0D0Cu);
    expectRead(24, 0x1B1A1918u);
    expectRead(4, 0x07060504u);
}

// Reads must depend on the current address, not the order of earlier reads
TEST_F(InstructionMemoryTestbench, SupportsNonSequentialReadsAndRevisits)
{
    expectRead(20, 0x17161514u);
    expectRead(4, 0x07060504u);
    expectRead(12, 0x0F0E0D0Cu);
    expectRead(0, 0x03020100u);
    expectRead(20, 0x17161514u);
    expectRead(24, 0x1B1A1918u);
    expectRead(4, 0x07060504u);
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
