module sign_ext #(
  parameter int INSTR_WIDTH = 32,
  parameter int DATA_WIDTH = 32
) (
  input  logic [INSTR_WIDTH-1:0] Instr,
  input  logic [2:0]             ImmSrc,
  output logic [DATA_WIDTH-1:0]  ImmExt
);

  localparam logic [2:0] IMM_I = 3'b000;
  localparam logic [2:0] IMM_S = 3'b001;
  localparam logic [2:0] IMM_B = 3'b010;
  localparam logic [2:0] IMM_U = 3'b011;
  localparam logic [2:0] IMM_J = 3'b100;

  // The control unit decodes the opcode separately.
  logic [6:0] unused_opcode;
  assign unused_opcode = Instr[6:0];

  always_comb begin
    unique case (ImmSrc)
      IMM_I: ImmExt = DATA_WIDTH'($signed(Instr[31:20]));
      IMM_S: ImmExt = DATA_WIDTH'($signed({Instr[31:25], Instr[11:7]}));
      // B and J encode signed byte offsets with an implicit zero in bit 0.
      IMM_B:
          ImmExt = DATA_WIDTH'($signed({Instr[31], Instr[7], Instr[30:25], Instr[11:8], 1'b0}));
      IMM_U: ImmExt = DATA_WIDTH'($signed({Instr[31:12], 12'b0}));
      IMM_J:
          ImmExt = DATA_WIDTH'($signed({Instr[31], Instr[19:12], Instr[20], Instr[30:21], 1'b0}));
      default: ImmExt = '0;
    endcase
  end

endmodule
