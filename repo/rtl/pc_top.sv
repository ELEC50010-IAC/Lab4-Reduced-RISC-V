module pc_top #(
    parameter WIDTH = 32
)(
    input logic clk,
    input logic rst,
    input logic [1:0]PCSrc,
    input logic [WIDTH-1:0] ImmExt,    // branch PC offset value
    /* verilator lint_off UNUSED */
    input logic [WIDTH-1:0] ALUResult,   // jalr target (clear bit 0)
    /* verilator lint_on UNUSED */
    output logic [WIDTH-1:0] PC,
    output logic [WIDTH-1:0] PCPlus4,    // needed by the result mux (jal/jalr write-back)
    output logic [WIDTH-1:0] PCTarget   // PC + ImmExt, needed by auipc if not done in the ALU
);

logic [WIDTH-1:0] PCNext;


assign PCPlus4 = PC + 4;
assign PCTarget = PC + ImmExt;

always_ff @(posedge clk, posedge rst) begin
    if(rst) begin
        PC <= 32'hBFC00000;     // reset PC to the start of the instruction memory
    end
    else begin
        PC <= PCNext;
    end
end


mux4 #(
    .DATA_WIDTH(WIDTH)
) PC_mux (
    .in0(PCPlus4),
    .in1(PCTarget),
    .in2({ALUResult[WIDTH-1:1], 1'b0}),   // jalr target, clear bit 0
    .in3(PCPlus4),
    .sel(PCSrc),
    .out(PCNext)
);

endmodule
