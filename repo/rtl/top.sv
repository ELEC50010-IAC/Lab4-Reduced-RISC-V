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
logic PCsrc;
logic RegWrite;
logic ALUsrc;
logic [1:0] ImmSrc;
logic [2:0] ALUctrl;
logic EQ;

// Datapath
logic [31:0]  ImmOp;
logic [31:0] RD1, RD2, regOp2, ALUout, ReadData, Result, PCPlus4, PCTarget;

logic [1:0] ResultSrc;
logic       MemWrite;
assign ResultSrc = 2'b00;  // TEMP until A's control unit drives it
assign MemWrite  = 1'b0;   // TEMP: no stores yet

// Instruction Fields
logic [4:0] rs1, rs2, rd;
assign rs1 = instr[19:15];
assign rs2 = instr[24:20];
assign rd = instr[11:7];

pc_top #(.WIDTH(32)) pc_reg(
    .clk(clk),
    .rst(rst),
    .PCSrc({1'b0, PCsrc}),   // TEMP until A's 2-bit PCSrc),
    .ImmExt(ImmOp),
    .PC(PC),
    .PCPlus4(PCPlus4),
    .PCTarget(PCTarget),
    .ALUResult(ALUout)
);

instr_mem #() imem(
    .A(PC),
    .RD(instr)
);

control_unit control(
    .RegWrite(RegWrite),
    .ALUCtrl(ALUctrl),
    .ALUSrc(ALUsrc),
    .ImmSrc(ImmSrc),
    .PCSrc(PCsrc),
    .EQ(EQ),
    .op(instr[6:0]), // op
    .funct3(instr[14:12]),
    .funct7(instr[30])
);

sign_ext signext(
    .Instr(instr),
    .ImmSrc(ImmSrc),
    .ImmOp(ImmOp)
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
    .in1(ImmOp),
    .sel(ALUsrc),
    .out(regOp2)
);

alu #(.DATA_WIDTH(32)) alu_unit(
    .SrcA(RD1),
    .SrcB(regOp2),
    .ALUControl({1'b0, ALUctrl}),
    .ALUResult(ALUout),
    .Zero(EQ)
);

data_mem#() data_mem_unit(
    .clk(clk),
    .funct3(instr[14:12]),
    .WE(MemWrite),
    .RD(ReadData),
    .A(ALUout),
    .WD(RD2)


);

mux4 result_mux(
    .in0(ALUout),
    .in1(ReadData),
    .in2(PCPlus4),
    .in3(32'b0),
    .sel(ResultSrc),
    .out(Result)
);
    
endmodule
