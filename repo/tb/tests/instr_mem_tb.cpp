// Checks instruction reads, byte order and address changes in the instruction memory
#include "base_testbench.h"

#include <array>
#include <cstdint>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <string>

class InstructionMemoryTestbench : public BaseTestbench
{
protected:
    // Begin each test at the first instruction address
    void initializeInputs() override
    {
        top->addr = 0;
    }

    // Prepare known memory contents without losing an existing program image
    void SetUp() override
    {
        // Preserve the group's program image when running from repo/tb
        struct stat imageInfo {};
        const int status = stat(imagePath.c_str(), &imageInfo);
        ASSERT_TRUE(status == 0 || errno == ENOENT) << std::strerror(errno);
        hadImage = status == 0;
        if (hadImage)
        {
            std::ifstream original(imagePath, std::ios::binary);
            ASSERT_TRUE(original.is_open());
            savedImage.assign(std::istreambuf_iterator<char>(original),
                              std::istreambuf_iterator<char>());
            ASSERT_FALSE(original.bad());
        }

        std::array<unsigned char, 1024> bytes{};
        // Each instruction is stored least significant byte first
        const unsigned char program[] = {
            0x13, 0x03, 0xf0, 0x0f, // 0ff00313
            0x13, 0x05, 0x00, 0x00, // 00000513
            0x93, 0x05, 0x00, 0x00, // 00000593
            0x13, 0x85, 0x05, 0x00, // 00058513
            0x93, 0x85, 0x15, 0x00, // 00158593
            0xe3, 0x9c, 0x65, 0xfe, // fe659ce3
            0xe3, 0x18, 0x03, 0xfe  // fe0318e3
        };
        for (unsigned int i = 0; i < sizeof(program); ++i)
        {
            bytes[i] = program[i];
        }
        // Distinct byte patterns expose reversed or overlapping byte assembly
        bytes[512] = 0xd3;
        bytes[513] = 0xc2;
        bytes[514] = 0xb1;
        bytes[515] = 0xa0;
        bytes[1020] = 0xef;
        bytes[1021] = 0xcd;
        bytes[1022] = 0xab;
        bytes[1023] = 0x89;

        imageChanged = true;
        std::ofstream image(imagePath, std::ios::binary | std::ios::trunc);
        ASSERT_TRUE(image.is_open());
        // Write every byte so reads never depend on an earlier test or program
        for (unsigned char byte : bytes)
        {
            image << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(byte) << '\n';
        }
        image.close();
        ASSERT_FALSE(image.fail());

        // The complete image exists before construction or the first evaluation
        BaseTestbench::SetUp();
    }

    // Restore the original image, or remove the temporary image if none existed
    void TearDown() override
    {
        // Setup may have failed before a simulated memory was created
        if (top)
        {
            BaseTestbench::TearDown();
        }
        if (imageChanged)
        {
            if (hadImage)
            {
                std::ofstream restored(imagePath, std::ios::binary | std::ios::trunc);
                ASSERT_TRUE(restored.is_open());
                restored << savedImage;
                restored.close();
                EXPECT_FALSE(restored.fail());
            }
            else
            {
                EXPECT_EQ(std::remove(imagePath.c_str()), 0) << std::strerror(errno);
            }
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

private:
    const std::string imagePath = "../rtl/program.hex";
    bool hadImage = false;
    bool imageChanged = false;
    std::string savedImage;
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
    expectRead(512, 0xa0b1c2d3u);
    expectRead(1020, 0x89abcdefu);
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
    expectRead(512, 0xa0b1c2d3u);
    expectRead(0, 0x0ff00313u);
    expectRead(20, 0xfe659ce3u);
    expectRead(1020, 0x89abcdefu);
    expectRead(4, 0x00000513u);
}

// The final four bytes must remain readable as one complete instruction
TEST_F(InstructionMemoryTestbench, FetchesLastCompleteWordInMemory)
{
    expectRead(1016, 0u);
    expectRead(1020, 0x89abcdefu);
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
