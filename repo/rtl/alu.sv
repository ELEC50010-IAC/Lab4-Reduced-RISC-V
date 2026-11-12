module alu #(
    DATA_WIDTH = 32
)(
    input logic [DATA_WIDTH-1:0] ALUop1,
    input logic [DATA_WIDTH-1:0] ALUop2,
    input logic [2:0] ALUctrl,
    output logic [DATA_WIDTH-1:0] ALUout,
    output logic EQ
);
    assign EQ = (ALUop1 == ALUop2);

    always_comb begin
        case(ALUctrl)
        3'b000: ALUout = ALUop1 + ALUop2;
        default: ALUout = 0;
        endcase
    end
endmodule

