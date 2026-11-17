module control_unit
(
    input logic [6:0] op,
    // The reduced opcode-only decoder retains these interface inputs
    /* verilator lint_off UNUSEDSIGNAL */
    input logic [2:0] funct3,
    input logic funct7,
    /* verilator lint_on UNUSEDSIGNAL */
    input logic EQ,
    output logic RegWrite,
    output logic ALUSrc,
    output logic [1:0] ImmSrc,
    output logic PCSrc,
    output logic [2:0] ALUCtrl
);

    logic [1:0] ALUOp;
    logic Branch;

    always_comb begin
        Branch = 0;
        ALUOp = 2'b00;
        RegWrite = 0;
        ALUSrc = 0;
        ImmSrc = 2'b00;
        PCSrc = 0;
        case(op)
            7'b0010011:
            begin //addi 
                RegWrite = 1;
                ALUSrc = 1;
                ImmSrc = 2'b00;
                PCSrc = 0;
                ALUOp = 2'b00;
            end
            7'b1100011: //bne
            begin

                RegWrite = 0;
                ALUSrc = 0;
                ImmSrc = 2'b10;
                Branch = 1;
                PCSrc = Branch & ~EQ;
                ALUOp = 2'b01;
            end
            default: ;
        endcase
    end
    always_comb begin
        ALUCtrl = 3'b000; // default
        case(ALUOp)
            2'b00: ALUCtrl = 3'b000;
            2'b01: ALUCtrl = 3'b001;
            default: ;
        endcase
    end





endmodule




