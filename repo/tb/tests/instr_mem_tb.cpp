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
        top->addr = 0;
    }

    // Run ./compile.sh asm/program.S from repo/tb before running this test
    void SetUp() override
    {
        // Check the real image before the first evaluation loads instruction memory
        std::ifstream image("../rtl/program.hex");
        ASSERT_TRUE(image.is_open())
            << "Run ./compile.sh asm/program.S from repo/tb first";
        for (unsigned int i = 0; i < 28; ++i)
        {
            unsigned int byte = 0;
            ASSERT_TRUE(static_cast<bool>(image >> std::hex >> byte))
                << "Missing or invalid program byte " << i;
            ASSERT_LE(byte, 0xffu) << "Invalid program byte " << i;
        }
        BaseTestbench::SetUp();
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
        top->addr = address;
        top->eval();
        EXPECT_EQ(top->dout, expected);
    }
};

// The stored program bytes must reproduce the expected 32-bit instructions
TEST_F(InstructionMemoryTestbench, ReadsKnownInstructionWords)
{
    expectRead(0, 0x0ff00313u);
    expectRead(4, 0x00000513u);
    expectRead(8, 0x00000593u);
    expectRead(12, 0x00058513u);
    expectRead(16, 0x00158593u);
    expectRead(20, 0xfe659ce3u);
    expectRead(24, 0xfe0318e3u);
}

// Distinct bytes reveal whether a word has been assembled in the wrong order
TEST_F(InstructionMemoryTestbench, AssemblesDistinctBytesInLittleEndianOrder)
{
    // At address 20, the assembler emits e3 9c 65 fe, least significant byte first
    expectRead(20, 0xe3u | (0x9cu << 8) | (0x65u << 16) | (0xfeu << 24));
}

// A new address must produce its instruction without any clock activity
TEST_F(InstructionMemoryTestbench, AddressChangesUpdateOutputWithoutAClock)
{
    expectRead(0, 0x0ff00313u);
    expectRead(12, 0x00058513u);
    expectRead(24, 0xfe0318e3u);
    expectRead(4, 0x00000513u);
}

// Reads must depend on the current address, not the order of earlier reads
TEST_F(InstructionMemoryTestbench, SupportsNonSequentialReadsAndRevisits)
{
    expectRead(20, 0xfe659ce3u);
    expectRead(4, 0x00000513u);
    expectRead(12, 0x00058513u);
    expectRead(0, 0x0ff00313u);
    expectRead(20, 0xfe659ce3u);
    expectRead(24, 0xfe0318e3u);
    expectRead(4, 0x00000513u);
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
