module pc_top #(
    parameter WIDTH = 32
)(
    input logic clk,
    input logic rst,
    input logic PCsrc,
    input logic [WIDTH-1:0] ImmOp,    // branch PC offset value
    output logic [WIDTH-1:0] PC,
);

logic [WIDTH-1:0] next_PC;

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
)(
    .in0(PC + 4),
    .in1(PC + ImmOp),
    .sel(PCsrc),
    .out(next_PC)
);

endmodule
    