# Block A single-cycle control

Design draft for B's module tests and D's integration. The table covers the 37 instructions in Team16's RV32I sprint. The official reference program exercises these eight instructions — `addi`, `add`, `bne`, `lui`, `lbu`, `sb`, `jal` and `jalr`. [Team interfaces](https://github.com/rchak07/Team16/blob/55602c92f6278fbb9d17aa294e97743a162066c6/docs/plan/interfaces.md) · [Reference disassembly](https://github.com/ELEC50010-IAC/Project_Brief/blob/main/tb/reference/pdf.asm) · [RV32I specification](https://docs.riscv.org/reference/isa/v20260120/unpriv/rv32.html)

## Control table

Opcodes are hexadecimal. `ALUSrc` chooses RD2 (0) or the immediate (1). Write enables below assume a legal encoding. The register file discards writes to x0. For a signal whose value is unused, drive its default.

| Instruction family | Opcode | ImmSrc | ALUSrc | ALU action | RegWrite | MemWrite | ResultSrc | PCSrc |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Register ALU | 0x33 | I (unused) | 0 | Decode funct3/bit 30 | 1 | 0 | 00 | 00 |
| Immediate ALU | 0x13 | I | 1 | Decode funct3, plus bit 30 for right shifts | 1 | 0 | 00 | 00 |
| Load | 0x03 | I | 1 | Add base and offset | 1 | 0 | 01 | 00 |
| Store | 0x23 | S | 1 | Add base and offset | 0 | 1 | 00 | 00 |
| Conditional branch | 0x63 | B | 0 | B compares RD1 and RD2 separately | 0 | 0 | 00 | 01 if BranchTaken, else 00 |
| JAL | 0x6F | J | 0 | Unused | 1 | 0 | 10 | 01 |
| JALR | 0x67 | I | 1 | Add base and offset | 1 | 0 | 10 | 10 |
| LUI | 0x37 | U | 0 (unused) | Write ImmExt | 1 | 0 | 11 | 00 |
| AUIPC | 0x17 | U | 1 | Add the instruction PC and ImmExt | 1 | 0 | 00 | 00 |

- `ImmSrc` selects I=000, S=001, B=010, U=011 and J=100. B and J offsets already include their low zero bit. U places the upper 20 instruction bits over twelve zeros.
- `ResultSrc` selects ALUResult=00, ReadData=01 and PCPlus4=10. `PCSrc` selects PCPlus4=00, PC+ImmExt=01 and the JALR target=10. C clears bit zero of the JALR target. Both jumps write PC+4 to rd.

## Function fields

| funct3 | Register ALU | Immediate ALU | Load | Store | Branch taken when |
| --- | --- | --- | --- | --- | --- |
| 000 | ADD / SUB | ADDI | LB | SB | BEQ, equal |
| 001 | SLL | SLLI | LH | SH | BNE, unequal |
| 010 | SLT | SLTI | LW | SW | Reserved |
| 011 | SLTU | SLTIU | Reserved | Reserved | Reserved |
| 100 | XOR | XORI | LBU | Reserved | BLT, signed less |
| 101 | SRL / SRA | SRLI / SRAI | LHU | Reserved | BGE, signed greater or equal |
| 110 | OR | ORI | Reserved | Reserved | BLTU, unsigned less |
| 111 | AND | ANDI | Reserved | Reserved | BGEU, unsigned greater or equal |

The team's 4-bit ALU codes in decimal are ADD=0, SUB=1, AND=2, OR=3, XOR=4, SLT=5, SLTU=6, SLL=7, SRL=8 and SRA=9. Bit 30 distinguishes the two operations in each paired cell above. For non-shift immediate arithmetic, bit 30 is ordinary immediate data. RV32 shifts use the low five bits of their shift operand. SLTIU sign-extends its immediate before comparing unsigned. D's data memory receives funct3 directly for load and store width and signedness.

## Wiring for integration

1. **Branch comparison.** A and B agreed on a single `BranchTaken` bit. B compares the two register values using `funct3`. The controller consults the bit only for the six legal conditional branch encodings. Jumps select their target independently.
2. **Upper-immediate write-back.** This local draft implements option A pending team confirmation. Connect `ImmExt` to result-mux input 3 for LUI. For AUIPC, `ALUSrcA=1` selects the PC of the executing instruction and `ALUSrc=1` selects the U-immediate. All other instructions drive `ALUSrcA=0` to select RD1. D owns these top-level connections.

**Default behaviour for this coursework design.** For an unsupported opcode or a reserved function combination visible at the decoder inputs, select PC+4 and disable register and memory writes. A deterministic idle selection is I, RD2, ADD and result 00. JALR accepts funct3=000. The decoder receives only bit 30 of funct7, so several reserved or extension encodings look identical to supported RV32I instructions. Strict rejection of those encodings would require more input bits. Trap handling is outside this design.
