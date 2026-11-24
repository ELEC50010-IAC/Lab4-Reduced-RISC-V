module top #(
    parameter DATA_WIDTH = 32
) (
    input   logic clk,
    input   logic rst,
    /* verilator lint_off UNUSED */
    input   logic trigger,
    /* verilator lint_on UNUSED */
    output  logic [DATA_WIDTH-1:0] a0   
);

// Program Counter and Instruction
logic [31:0] PC;
logic [31:0] instr;

// Control Signals
logic [1:0] PCSrc;
logic RegWrite;
logic BranchTaken;
logic ALUSrc;
logic [2:0] ImmSrc;
logic [3:0] ALUControl;

// Datapath
logic [31:0]  ImmExt;
logic ALUSrcA;
logic [31:0] RD1, RD2, SrcB, ALUResult, ReadData, Result, PCPlus4, SrcA;

logic [1:0] ResultSrc;
logic       MemWrite;

// Instruction Fields
logic [4:0] rs1, rs2, rd;
assign rs1 = instr[19:15];
assign rs2 = instr[24:20];
assign rd = instr[11:7];

pc_top #(.WIDTH(32)) pc_reg(
    .clk(clk),
    .rst(rst),
    .PCSrc(PCSrc),
    .ImmExt(ImmExt),
    .PC(PC),
    .PCPlus4(PCPlus4),
    .ALUResult(ALUResult)
);

instr_mem #() imem(
    .A(PC),
    .RD(instr)
);

control_unit control(
    .RegWrite(RegWrite),
    .ALUControl(ALUControl),
    .ALUSrc(ALUSrc),
    .ImmSrc(ImmSrc),
    .PCSrc(PCSrc),
    .ALUSrcA(ALUSrcA),
    .MemWrite(MemWrite),
    .ResultSrc(ResultSrc),
    .BranchTaken(BranchTaken),
    .op(instr[6:0]),
    .funct3(instr[14:12]),
    .funct7b5(instr[30])
);

sign_ext signext(
    .Instr(instr),
    .ImmSrc(ImmSrc),
    .ImmExt(ImmExt)
);

regfile #(.DATA_WIDTH(32), .ADDR_WIDTH(5)) rf(
    .A1(rs1),
    .A2(rs2),
    .A3(rd),
    .WD3(Result),
    .WE3(RegWrite),
    .clk(clk),
    .RD1(RD1),
    .RD2(RD2),
    .a0(a0)
);

mux #(.DATA_WIDTH(32)) alu_src_mux(
    .in0(RD2),
    .in1(ImmExt),
    .sel(ALUSrc),
    .out(SrcB)
);

alu #(.DATA_WIDTH(32)) alu_unit(
    .SrcA(SrcA),
    .SrcB(SrcB),
    .ALUControl(ALUControl),
    .ALUResult(ALUResult)
);
    



branch_unit branch(
    .RD1(RD1),
    .RD2(RD2),
    .funct3(instr[14:12]),
    .BranchTaken(BranchTaken)
);

mux #(.DATA_WIDTH(32)) alu_srca_mux(
    .in0(RD1),          // normal instructions
    .in1(PC),          // auipc
    .sel(ALUSrcA),
    .out(SrcA)
);

data_mem data_mem_unit(
    .clk(clk),
    .funct3(instr[14:12]),
    .WE(MemWrite),
    .RD(ReadData),
    .A(ALUResult),
    .WD(RD2)
);

mux4 result_mux(
    .in0(ALUResult),
    .in1(ReadData),
    .in2(PCPlus4),
    .in3(ImmExt),          // lui
    .sel(ResultSrc),
    .out(Result)
);

endmodule
