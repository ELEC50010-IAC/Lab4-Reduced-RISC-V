module top #(
    parameter int DATA_WIDTH = 32
) (
    input  logic                  clk,
    input  logic                  rst,
    output logic [DATA_WIDTH-1:0] a0
);

    // ---------------- wires between the blocks ----------------
    logic [DATA_WIDTH-1:0] pc, pc_next;       // program counter
    logic [DATA_WIDTH-1:0] instr;             // current instruction
    logic [DATA_WIDTH-1:0] imm_ext;           // sign-extended immediate
    logic                  pc_src, reg_write, alu_src, imm_src;
    logic [2:0]            alu_ctrl;
    logic                  eq;                // rs1 == rs2 (used by bne)

    /* verilator lint_off UNUSEDSIGNAL */
    logic [DATA_WIDTH-1:0] pc_plus4, pc_target;   // not needed in Lab 4; used later (jal, auipc)
    /* verilator lint_on UNUSEDSIGNAL */

    // ---------------- PC (pc_reg + next_pc) ----------------
    pc_reg #(.XLEN(DATA_WIDTH)) pc_register (
        .clk(clk), .rst(rst), .pc_next(pc_next), .pc(pc)
    );

    next_pc #(.XLEN(DATA_WIDTH)) next_pc_logic (
        .pc_src   ({1'b0, pc_src}),   // Lab 4: only PC+4 (00) or branch (01)
        .pc       (pc),
        .imm_ext  (imm_ext),
        .alu_res  ('0),               // jalr not used in Lab 4
        .pc_plus4 (pc_plus4),
        .pc_target(pc_target),
        .pc_next  (pc_next)
    );

    // ---------------- fetch ----------------
    instr_mem imem (
        .addr(pc), .dout(instr)
    );

    // ---------------- decode ----------------
    ctrl_unit control (
        .op       (instr[6:0]),
        .funct3   (instr[14:12]),
        .zero     (eq),
        .pc_src   (pc_src),
        .reg_write(reg_write),
        .alu_src  (alu_src),
        .imm_src  (imm_src),
        .alu_ctrl (alu_ctrl)
    );

    sign_extend sext (
        .din(instr), .immsrc(imm_src), .immOp(imm_ext)
    );

    // ---------------- execute + register file ----------------
    harshitTop datapath (
        .clk     (clk),
        .rs1     (instr[19:15]),
        .rs2     (instr[24:20]),
        .rd      (instr[11:7]),
        .RegWrite(reg_write),
        .ALUsrc  (alu_src),
        .ALUctrl (alu_ctrl),
        .ImmOp   (imm_ext),
        .a0      (a0),
        .EQ      (eq)
    );

endmodule
