module control_unit (
  input  logic [6:0] op,
  input  logic [2:0] funct3,
  input  logic       funct7b5,
  input  logic       BranchTaken,
  output logic       RegWrite,
  output logic       ALUSrc,
  output logic       ALUSrcA,
  output logic [2:0] ImmSrc,
  output logic       MemWrite,
  output logic [1:0] ResultSrc,
  output logic [1:0] PCSrc,
  output logic [3:0] ALUControl
);
  localparam logic [6:0] OP_REG    = 7'b0110011;
  localparam logic [6:0] OP_IMM    = 7'b0010011;
  localparam logic [6:0] OP_LOAD   = 7'b0000011;
  localparam logic [6:0] OP_STORE  = 7'b0100011;
  localparam logic [6:0] OP_BRANCH = 7'b1100011;
  localparam logic [6:0] OP_JAL    = 7'b1101111;
  localparam logic [6:0] OP_JALR   = 7'b1100111;
  localparam logic [6:0] OP_LUI    = 7'b0110111;
  localparam logic [6:0] OP_AUIPC  = 7'b0010111;

  localparam logic [2:0] IMM_I = 3'd0, IMM_S = 3'd1, IMM_B = 3'd2,
                         IMM_U = 3'd3, IMM_J = 3'd4;
  localparam logic [1:0] RESULT_ALU = 2'd0, RESULT_MEM = 2'd1,
                         RESULT_PC4 = 2'd2, RESULT_IMM = 2'd3;
  localparam logic [1:0] PC_NEXT = 2'd0, PC_TARGET = 2'd1, PC_JALR = 2'd2;

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
    ALUSrcA = 1'b0;
    ImmSrc = IMM_I;
    MemWrite = 1'b0;
    ResultSrc = RESULT_ALU;
    PCSrc = PC_NEXT;
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
      OP_LOAD: begin
        if (funct3 inside {3'b000, 3'b001, 3'b010, 3'b100, 3'b101}) begin
          ALUSrc = 1'b1;
          RegWrite = 1'b1;
          ResultSrc = RESULT_MEM;
        end
      end
      OP_STORE: begin
        if (funct3 inside {3'b000, 3'b001, 3'b010}) begin
          ImmSrc = IMM_S;
          ALUSrc = 1'b1;
          MemWrite = 1'b1;
        end
      end
      OP_BRANCH: begin
        if (funct3 inside {3'b000, 3'b001, 3'b100, 3'b101, 3'b110, 3'b111}) begin
          ImmSrc = IMM_B;
          ALUControl = ALU_SUB;
          PCSrc = BranchTaken ? PC_TARGET : PC_NEXT;
        end
      end
      OP_JAL: begin
        ImmSrc = IMM_J;
        RegWrite = 1'b1;
        ResultSrc = RESULT_PC4;
        PCSrc = PC_TARGET;
      end
      OP_JALR: begin
        if (funct3 == 3'b000) begin
          ALUSrc = 1'b1;
          RegWrite = 1'b1;
          ResultSrc = RESULT_PC4;
          PCSrc = PC_JALR;
        end
      end
      OP_LUI: begin
        ImmSrc = IMM_U;
        RegWrite = 1'b1;
        ResultSrc = RESULT_IMM;
      end
      OP_AUIPC: begin
        ImmSrc = IMM_U;
        RegWrite = 1'b1;
        ALUSrcA = 1'b1;  // Select the PC of this instruction.
        ALUSrc = 1'b1;
      end
      default: ;
    endcase
  end
endmodule
