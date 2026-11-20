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
        4'b0101: ALUResult = {{(DATA_WIDTH-1){1'b0}}, $signed(SrcA) < $signed(SrcB)};
        4'b0110: ALUResult = {{(DATA_WIDTH-1){1'b0}}, SrcA < SrcB};
        4'b0111: ALUResult = SrcA << SrcB[4:0];
        4'b1000: ALUResult = SrcA >> SrcB[4:0];
        4'b1001: ALUResult = $signed(SrcA) >>> SrcB[4:0];
        default: ALUResult = 0;
        endcase
    end
endmodule

