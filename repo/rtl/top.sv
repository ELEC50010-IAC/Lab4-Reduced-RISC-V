module top #(
    DATA_WIDTH = 32
) (
    input   logic clk,
    input   logic rst,
    output  logic [DATA_WIDTH-1:0] a0    
);
    assign a0 = 32'd5;
    logic [DATA_WIDTH-1:0] pc_plus4, pc_target;

    pc_reg pc_register (
        .clk(clk), .rst(rst), .pc_next(pc_next), .pc(pc)
    );

    next_pc next_pc_logic (
        .pc_src(pc_src), .pc(pc), .imm_ext(imm_ext), .alu_res(alu_res),
        .pc_plus4(pc_plus4), .pc_target(pc_target), .pc_next(pc_next)
    );
endmodule
