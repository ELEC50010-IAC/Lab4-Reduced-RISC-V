module top #(
    parameter DATA_WIDTH = 32
) (
    input   logic clk,
    input   logic rst,
    output  logic [DATA_WIDTH-1:0] a0    
);

// Program Counter and Instruction
logic [31:0] PC;
logic [31:0] instr;

// Control Signals
logic PCsrc;
logic RegWrite;
logic ALUsrc;
logic [2:0] ImmSrc;
logic [3:0] ALUControl;
logic EQ;

// Datapath
logic [31:0]  ImmOp;
logic [31:0] RD1, RD2, regOp2, ALUout;

// Instruction Fields
logic [4:0] rs1, rs2, rd;
assign rs1 = instr[19:15];
assign rs2 = instr[24:20];
assign rd = instr[11:7];

pc_top #(.WIDTH(32)) pc(
    .clk(clk),
    .rst(rst),
    .PCsrc(PCsrc),
    .ImmOp(ImmOp),
    .PC(PC)
);

instr_mem #(.ADDRESS_WIDTH(10)) imem(
    .addr(PC),
    .dout(instr)
);

control_unit control(
    .RegWrite(RegWrite),
    .ALUControl(ALUControl),
    .ALUSrc(ALUsrc),
    .ImmSrc(ImmSrc),
    .PCSrc(PCsrc),
    .Zero(EQ),
    .op(instr[6:0]),
    .funct3(instr[14:12]),
    .funct7b5(instr[30])
);

sign_ext signext(
    .Instr(instr),
    .ImmSrc(ImmSrc),
    .ImmExt(ImmOp)
);

regfile #(.DATA_WIDTH(32), .ADDR_WIDTH(5)) rf(
    .AD1(rs1),
    .AD2(rs2),
    .AD3(rd),
    .WD3(ALUout),
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
    .ALUControl(ALUControl),
    .ALUResult(ALUout),
    .Zero(EQ)
);
    
endmodule
