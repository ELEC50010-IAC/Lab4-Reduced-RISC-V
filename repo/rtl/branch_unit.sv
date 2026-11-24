// Decides whether a branch is taken, from the two register values and funct3
module branch_unit #(
    DATA_WIDTH = 32
)(
    input logic [DATA_WIDTH-1:0] RD1,
    input logic [DATA_WIDTH-1:0] RD2,
    input logic [2:0] funct3,
    output logic BranchTaken
);
    always_comb begin
        case(funct3)
        3'b000: BranchTaken = (RD1 == RD2);
        3'b001: BranchTaken = (RD1 != RD2);
        3'b100: BranchTaken = ($signed(RD1) < $signed(RD2));
        3'b101: BranchTaken = ($signed(RD1) >= $signed(RD2));
        3'b110: BranchTaken = (RD1 < RD2);
        3'b111: BranchTaken = (RD1 >= RD2);
        default: BranchTaken = 1'b0; 
        endcase
    end
endmodule
