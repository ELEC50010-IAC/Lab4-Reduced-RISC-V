module control_unit (
  input  logic [6:0] op,
  input  logic [2:0] funct3,
  input  logic       funct7b5,
  input  logic       Zero,
  output logic       RegWrite,
  output logic       ALUSrc,
  output logic [2:0] ImmSrc,
  output logic       PCSrc,
  output logic [3:0] ALUControl
);
  localparam logic [6:0] OP_REG    = 7'b0110011;
  localparam logic [6:0] OP_IMM    = 7'b0010011;
  localparam logic [6:0] OP_BRANCH = 7'b1100011;

  localparam logic [3:0] ALU_ADD  = 4'd0;
  localparam logic [3:0] ALU_SUB  = 4'd1;
  localparam logic [3:0] ALU_AND  = 4'd2;
  localparam logic [3:0] ALU_OR   = 4'd3;
  localparam logic [3:0] ALU_XOR  = 4'd4;
  localparam logic [3:0] ALU_SLT  = 4'd5;
  localparam logic [3:0] ALU_SLTU = 4'd6;
  localparam logic [3:0] ALU_SLL  = 4'd7;
  localparam logic [3:0] ALU_SRL  = 4'd8;
  localparam logic [3:0] ALU_SRA  = 4'd9;

  logic valid_reg_funct, valid_imm_funct;
  // With bit 30 set, only SUB and SRA are valid register operations.
  assign valid_reg_funct = !funct7b5 || funct3 == 3'b000 || funct3 == 3'b101;
  // For SLLI the high immediate bits encode the operation, not shift data.
  assign valid_imm_funct = !funct7b5 || funct3 != 3'b001;

  always_comb begin
    RegWrite = 1'b0;
    ALUSrc = 1'b0;
    ImmSrc = 3'b000;
    PCSrc = 1'b0;
    ALUControl = ALU_ADD;

    unique case (op)
      OP_REG, OP_IMM: begin
        // For ordinary I-type instructions, bit 30 is part of the immediate.
        if ((op == OP_REG && valid_reg_funct) || (op == OP_IMM && valid_imm_funct)) begin
          RegWrite = 1'b1;
          ALUSrc = op == OP_IMM;
          unique case (funct3)
            3'b000: ALUControl = (op == OP_REG && funct7b5) ? ALU_SUB : ALU_ADD;
            3'b001: ALUControl = ALU_SLL;
            3'b010: ALUControl = ALU_SLT;
            3'b011: ALUControl = ALU_SLTU;
            3'b100: ALUControl = ALU_XOR;
            3'b101: ALUControl = funct7b5 ? ALU_SRA : ALU_SRL;
            3'b110: ALUControl = ALU_OR;
            3'b111: ALUControl = ALU_AND;
            default: RegWrite = 1'b0;
          endcase
        end
      end
      OP_BRANCH: begin
        ImmSrc = 3'b010;
        ALUControl = ALU_SUB;
        unique case (funct3)
          3'b000: PCSrc = Zero;
          3'b001: PCSrc = !Zero;
          default: PCSrc = 1'b0;
        endcase
      end
      default: ;
    endcase
  end
endmodule
