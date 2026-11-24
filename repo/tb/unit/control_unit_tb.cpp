/*
 *  Review tests for Ahsan's control unit: checks the control signals for every
 *  instruction type, including taken and not-taken branches, jal, jalr, lui and auipc.
 */

#include "base_testbench.h"

unsigned int ticks = 0;

class ControlUnitTestbench : public BaseTestbench
{
protected:
    void initializeInputs() override
    {
        top->op = 0;
        top->funct3 = 0;
        top->funct7b5 = 0;
        top->BranchTaken = 0;
    }
};

// R-type: add and sub share an opcode and funct3, only funct7b5 tells them apart
TEST_F(ControlUnitTestbench, AddSubTest)
{
    // add: writes a register, both ALU inputs from registers
    top->op = 0b0110011;
    top->funct3 = 0b000;
    top->funct7b5 = 0;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);

    // sub: same switches, funct7b5 = 1 makes the ALU subtract
    top->funct7b5 = 1;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 1u);
}

// other R-type ops: funct3 picks the ALU code, funct7b5 splits srl/sra
TEST_F(ControlUnitTestbench, RTypeAluControlTest)
{
    top->op = 0b0110011;

    // sll
    top->funct3 = 0b001;
    top->funct7b5 = 0;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0111u);

    // slt (signed)
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0101u);

    // sltu (unsigned)
    top->funct3 = 0b011;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0110u);

    // xor
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0100u);

    // srl: funct7b5 = 0
    top->funct3 = 0b101;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b1000u);

    // sra: funct7b5 = 1 makes the shift arithmetic
    top->funct3 = 0b101;
    top->funct7b5 = 1;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b1001u);

    // or
    top->funct3 = 0b110;
    top->funct7b5 = 0;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0011u);

    // and
    top->funct3 = 0b111;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0010u);
}

// I-type: same ALU codes as R-type, second input is the immediate
TEST_F(ControlUnitTestbench, ITypeAluTest)
{
    top->op = 0b0010011;

    // addi
    top->funct3 = 0b000;
    top->funct7b5 = 0;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0000u);

    // slli
    top->funct3 = 0b001;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0111u);

    // slti (signed)
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0101u);

    // sltiu (unsigned)
    top->funct3 = 0b011;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0110u);

    // xori
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0100u);

    // srli: funct7b5 = 0
    top->funct3 = 0b101;
    top->funct7b5 = 0;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b1000u);

    // srai: funct7b5 = 1 makes the shift arithmetic
    top->funct7b5 = 1;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b1001u);

    // ori
    top->funct3 = 0b110;
    top->funct7b5 = 0;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0011u);

    // andi
    top->funct3 = 0b111;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0010u);
}

// addi with a negative immediate must still add (bit 30 is part of the immediate)
TEST_F(ControlUnitTestbench, AddiNegativeImmediateStillAddsTest)
{
    // e.g. addi x1, x2, -1 sets funct7b5
    top->op = 0b0010011;
    top->funct3 = 0b000;
    top->funct7b5 = 1;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0b0000u);
}

// loads: ALU adds register + immediate for the address, result comes from memory (ResultSrc = 1)
TEST_F(ControlUnitTestbench, LoadTest)
{
    top->op = 0b0000011;

    // lb
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);

    // lh
    top->funct3 = 0b001;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);

    // lw
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);

    // lbu (unsigned)
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);

    // lhu (unsigned)
    top->funct3 = 0b101;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);
}

// stores: write to memory (MemWrite = 1) and never to a register, S-type immediate
TEST_F(ControlUnitTestbench, StoreTest)
{
    top->op = 0b0100011;

    // sb
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 1u);
    EXPECT_EQ(top->MemWrite, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);

    // sh
    top->funct3 = 0b001;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 1u);
    EXPECT_EQ(top->MemWrite, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);

    // sw
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 1u);
    EXPECT_EQ(top->MemWrite, 1u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);
}

// taken branch: jump to target (PCSrc = 1), no register or memory write
TEST_F(ControlUnitTestbench, BranchTakenTest)
{
    top->op = 0b1100011;
    top->BranchTaken = 1;

    // beq
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 1u);

    // bne
    top->funct3 = 0b001;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 1u);

    // blt
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 1u);

    // bge
    top->funct3 = 0b101;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 1u);

    // bltu (unsigned)
    top->funct3 = 0b110;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 1u);

    // bgeu (unsigned)
    top->funct3 = 0b111;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 1u);

    // funct3 = 010 is not a branch, no jump even with BranchTaken = 1
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
}

// branch not taken: carry on to next instruction (PCSrc = 0)
TEST_F(ControlUnitTestbench, BranchNotTakenTest)
{
    top->op = 0b1100011;
    top->BranchTaken = 0;

    // beq
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);

    // bne
    top->funct3 = 0b001;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);

    // blt
    top->funct3 = 0b100;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);

    // bge
    top->funct3 = 0b101;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);

    // bltu (unsigned)
    top->funct3 = 0b110;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);

    // bgeu (unsigned)
    top->funct3 = 0b111;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->ALUSrc, 0u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 2u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
}

// jal: always jumps (PCSrc = 1) and saves PC + 4, BranchTaken is ignored
TEST_F(ControlUnitTestbench, JalTest)
{
    top->op = 0b1101111;

    // BranchTaken = 0 must not stop the jump
    top->BranchTaken = 0;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 4u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 2u);
    EXPECT_EQ(top->PCSrc, 1u);

    // BranchTaken = 1 changes nothing
    top->BranchTaken = 1;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 4u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 2u);
    EXPECT_EQ(top->PCSrc, 1u);
}

// jalr: jumps to register + immediate from the ALU (PCSrc = 2), saves PC + 4
TEST_F(ControlUnitTestbench, JalrTest)
{
    top->op = 0b1100111;

    // jalr (funct3 must be 000)
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 0u);
    EXPECT_EQ(top->ImmSrc, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 2u);
    EXPECT_EQ(top->PCSrc, 2u);
    EXPECT_EQ(top->ALUControl, 0u);
}

// lui: writes the U-type immediate straight into the register (ResultSrc = 3)
TEST_F(ControlUnitTestbench, LuiTest)
{
    // lui
    top->op = 0b0110111;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ImmSrc, 3u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 3u);
    EXPECT_EQ(top->PCSrc, 0u);
}

// auipc: ALU adds the PC (ALUSrcA = 1) and the U-type immediate 
TEST_F(ControlUnitTestbench, AuipcTest)
{
    // auipc
    top->op = 0b0010111;
    top->eval();
    EXPECT_EQ(top->RegWrite, 1u);
    EXPECT_EQ(top->ALUSrc, 1u);
    EXPECT_EQ(top->ALUSrcA, 1u);
    EXPECT_EQ(top->ImmSrc, 3u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->ResultSrc, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
    EXPECT_EQ(top->ALUControl, 0u);
}

// BranchTaken only matters for branches, other instructions never jump 
TEST_F(ControlUnitTestbench, NonBranchIgnoresBranchTakenTest)
{
    top->BranchTaken = 1;

    // add
    top->op = 0b0110011;
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->PCSrc, 0u);

    // addi
    top->op = 0b0010011;
    top->funct3 = 0b000;
    top->eval();
    EXPECT_EQ(top->PCSrc, 0u);

    // lw
    top->op = 0b0000011;
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->PCSrc, 0u);

    // sw
    top->op = 0b0100011;
    top->funct3 = 0b010;
    top->eval();
    EXPECT_EQ(top->PCSrc, 0u);
}

// unknown opcode does nothing: no register write, no memory write, no jump
TEST_F(ControlUnitTestbench, UnknownOpcodeTest)
{
    // all zeros is not a valid instruction
    top->op = 0b0000000;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);

    // all ones is not valid either
    top->op = 0b1111111;
    top->eval();
    EXPECT_EQ(top->RegWrite, 0u);
    EXPECT_EQ(top->MemWrite, 0u);
    EXPECT_EQ(top->PCSrc, 0u);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


