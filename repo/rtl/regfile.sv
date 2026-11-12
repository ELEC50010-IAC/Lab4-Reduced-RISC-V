module regfile #(
    DATA_WIDTH = 32,
    ADDR_WIDTH = 5
)(
    input logic [ADDR_WIDTH-1:0] AD1,
    input logic [ADDR_WIDTH-1:0] AD2,
    input logic [ADDR_WIDTH-1:0] AD3,
    input logic [DATA_WIDTH-1:0] WD3,
    input logic WE3,
    input logic clk,
    output logic [DATA_WIDTH-1:0] RD1,
    output logic [DATA_WIDTH-1:0] RD2,
    output logic [DATA_WIDTH-1:0] a0
);
    logic [DATA_WIDTH-1:0] registers [2**ADDR_WIDTH-1:0];
    assign RD1 = registers[AD1];
    assign RD2 = registers[AD2];
    assign a0 = registers[10];

    always_ff @(posedge clk) begin
        if(WE3 == 1 && AD3 != 0)
            registers[AD3] <= WD3;    
    end
endmodule