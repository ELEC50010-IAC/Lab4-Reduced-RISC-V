module sign_ext #(
    INSTR_WIDTH  = 32,
    DATA_WIDTH = 32
)(
    // Opcode and register fields are not part of an immediate
    /* verilator lint_off UNUSEDSIGNAL */
    input logic [INSTR_WIDTH-1:0] Instr,
    /* verilator lint_on UNUSEDSIGNAL */
    input logic [1:0] ImmSrc,
    output logic [DATA_WIDTH-1:0] ImmOp
);

    // upper bits = copies of Imm's top bit (DATA_WIDTH - IMM_WIDTH of them), then Imm
    always_comb begin
        case (ImmSrc)
            2'b00: ImmOp = {{20{Instr[31]}}, Instr[31:20]};
            2'b01: ImmOp = {{20{Instr[31]}}, Instr[31:25], Instr[11:7]};
            2'b10: ImmOp = {{20{Instr[31]}}, Instr[7], Instr[30:25], Instr[11:8], 1'b0};
            default: ImmOp = 32'b0;
        endcase
    end

endmodule
