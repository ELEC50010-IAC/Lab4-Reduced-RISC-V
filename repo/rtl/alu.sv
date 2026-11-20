module alu #(
    DATA_WIDTH = 32
)(
    input logic [DATA_WIDTH-1:0] SrcA,
    input logic [DATA_WIDTH-1:0] SrcB,
    input logic [3:0] ALUControl,
    output logic [DATA_WIDTH-1:0] ALUResult,
    output logic Zero
);
    assign Zero = (SrcA == SrcB);

    always_comb begin
        case(ALUControl)
        4'b0000: ALUResult = SrcA + SrcB;
        4'b0001: ALUResult = SrcA - SrcB;
        4'b0010: ALUResult = SrcA & SrcB;
        4'b0011: ALUResult = SrcA | SrcB;
        4'b0100: ALUResult = SrcA ^ SrcB;
        default: ALUResult = 0;
        endcase
    end
endmodule

