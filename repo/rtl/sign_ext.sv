module sign_ext #(
    IMM_WIDTH  = 12,
    DATA_WIDTH = 32
)(
    input  logic [IMM_WIDTH-1:0]  Imm,
    output logic [DATA_WIDTH-1:0] ImmOp
);

    // upper bits = copies of Imm's top bit (DATA_WIDTH - IMM_WIDTH of them), then Imm
    assign ImmOp = {{(DATA_WIDTH - IMM_WIDTH){Imm[DATA_WIDTH - 1]}}, Imm };

endmodule