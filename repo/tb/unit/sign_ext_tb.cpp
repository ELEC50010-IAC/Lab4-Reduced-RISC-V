// Checks that I, S and B instruction immediates become the correct 32-bit values
#include "base_testbench.h"
#include <cstdint>

class SignExtTestbench : public BaseTestbench
{
protected:
    // Begin with a zero instruction and the I-format selector
    void initializeInputs() override
    {
        top->Instr = 0;
        top->ImmSrc = 0;
    }
};

// Build instruction encodings from signed values; expected results remain the
// original values, so the test does not decode the DUT output to check itself
static std::uint32_t encodeI(int value)
{
    // Keep the 12-bit pattern used to encode both positive and negative values
    const std::uint32_t immediate = static_cast<std::uint32_t>(value) & 0xfffu;
    return (immediate << 20) | (21u << 15) | (10u << 7) | 0x13u;
}

// S-format stores its immediate in two separate parts of the instruction
static std::uint32_t encodeS(int value)
{
    const std::uint32_t immediate = static_cast<std::uint32_t>(value) & 0xfffu;
    return ((immediate >> 5) << 25) | (7u << 20) | (21u << 15)
           | (2u << 12) | ((immediate & 31u) << 7) | 0x23u;
}

// Branch offsets scatter their bits and omit the always-zero lowest bit
static std::uint32_t encodeB(int byteOffset)
{
    const std::uint32_t immediate = static_cast<std::uint32_t>(byteOffset) & 0x1fffu;
    return (((immediate >> 12) & 1u) << 31)
           | (((immediate >> 5) & 63u) << 25)
           | (6u << 20) | (11u << 15) | (1u << 12)
           | (((immediate >> 1) & 15u) << 8)
           | (((immediate >> 11) & 1u) << 7) | 0x63u;
}

// I-format must preserve signed immediates, including the largest and smallest values
TEST_F(SignExtTestbench, IFormatPositiveZeroNegativeAndLimits)
{
    const int values[] = {0, 1, 255, 1024, 2047, -1, -17, -1024, -2048};
    top->ImmSrc = 0;
    for (int value : values)
    {
        SCOPED_TRACE(value);
        top->Instr = encodeI(value);
        top->eval();
        EXPECT_EQ(top->ImmOp, static_cast<std::uint32_t>(value));
    }
}

// Split S-format fields must be reassembled into the original signed value
TEST_F(SignExtTestbench, SFormatExistingExtractorContract)
{
    // The extractor supports S format even though the baseline CPU has no store
    const int values[] = {0, 1, 31, 32, 2047, -1, -32, -33, -2048};
    top->ImmSrc = 1;
    for (int value : values)
    {
        SCOPED_TRACE(value);
        top->Instr = encodeS(value);
        top->eval();
        EXPECT_EQ(top->ImmOp, static_cast<std::uint32_t>(value));
    }
}

// Branch immediates must become signed byte offsets without an extra shift
TEST_F(SignExtTestbench, BFormatProducesSignedByteOffsets)
{
    const int offsets[] = {0, 2, 8, 30, 32, 2048, 4094,
                           -2, -8, -16, -2048, -4096};
    top->ImmSrc = 2;
    for (int offset : offsets)
    {
        SCOPED_TRACE(offset);
        top->Instr = encodeB(offset);
        top->eval();
        EXPECT_EQ(top->ImmOp, static_cast<std::uint32_t>(offset));
    }
}

// Real program words and selector changes must give the expected immediate each time
TEST_F(SignExtTestbench, KnownProgramWordsAndSelectorChanges)
{
    top->ImmSrc = 0;
    top->Instr = 0x0ff00313u; // addi t1, zero, 255
    top->eval();
    EXPECT_EQ(top->ImmOp, 255u);

    top->ImmSrc = 2;
    top->Instr = 0xfe659ce3u; // bne a1, t1, -8
    top->eval();
    EXPECT_EQ(top->ImmOp, 0xfffffff8u);

    top->ImmSrc = 3;
    top->eval();
    EXPECT_EQ(top->ImmOp, 0u);

    top->ImmSrc = 0;
    top->Instr = 0x00000513u; // addi a0, zero, 0
    top->eval();
    EXPECT_EQ(top->ImmOp, 0u);
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
