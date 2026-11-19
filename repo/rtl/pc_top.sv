module pc_top #(
    parameter WIDTH = 32
)(
    input logic clk,
    input logic rst,
    input logic PCsrc,
    input logic [WIDTH-1:0] ImmOp,    // branch PC offset value
    output logic [WIDTH-1:0] PC
);

logic [WIDTH-1:0] next_PC;
logic [WIDTH-1:0] inc_PC;
logic [WIDTH-1:0] branch_PC;

assign inc_PC = PC + 4;
assign branch_PC = PC + ImmOp;

always_ff @(posedge clk, posedge rst) begin
    if(rst) begin
        PC <= 32'b0;
    end
    else begin
        PC <= next_PC;
    end
end


mux #(
    .DATA_WIDTH(WIDTH)
) PC_mux (
    .in0(inc_PC),
    .in1(branch_PC),
    .sel(PCsrc),
    .out(next_PC)
);

endmodule
    