module alu #(
    DATA_WIDTH = 32
)(
    input logic [DATA_WIDTH-1:0] SrcA,
    input logic [DATA_WIDTH-1:0] SrcB,
    input logic [2:0] ALUControl,
    output logic [DATA_WIDTH-1:0] ALUResult,
    output logic Zero
);
    assign Zero = (SrcA == SrcB);

    always_comb begin
        case(ALUControl)
        3'b000: ALUResult = SrcA + SrcB;
        default: ALUResult = 0;
        endcase
    end
endmodule

