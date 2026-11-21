module data_mem #(

    ADDRESS_WIDTH = 17
)( 
    input logic clk,
    input logic WE,
    input logic [31:0] A,
    input logic [31:0] WD,
    input logic [2:0] funct3,
    output logic [31:0] RD
);
    logic [7:0] mem [2**ADDRESS_WIDTH-1:0]; // each element contains 8 bytes;
    logic [ADDRESS_WIDTH-1:0] a;
    assign a = A[ADDRESS_WIDTH-1:0];

initial begin
    $display("Loading Data memory.");
    $readmemh("../rtl/program.hex", mem);
end

    always_comb begin
        case (funct3)
        3'b000: RD = {{24{mem[a][7]}}, mem[a]};
        3'b001: RD = {{16{mem[a+1][7]}},mem[a+1],mem[a]};
        3'b010: RD = {mem[a+3],mem[a+2],mem[a+1],mem[a]};
        3'b100: RD = {24'b0, mem[a]};
        3'b101: RD = {16'b0, mem[a+1], mem[a]};
        default: RD = 32'b0; //avoid latch
        endcase
    end

    always_ff @(posedge clk) begin
        if (WE) begin
            case (funct3)
                3'b000: mem[a] <= WD[7:0];
                3'b001: begin
                    mem[a+1] <= WD[15:8];
                    mem[a] <= WD[7:0];
                end
                3'b010: begin
                    mem[a+3] <= WD[31:24];
                    mem[a+2] <= WD[23:16];
                    mem[a+1] <= WD[15:8];
                    mem[a] <= WD[7:0];          
                end
                default: ;
            endcase
        end
    end

endmodule

