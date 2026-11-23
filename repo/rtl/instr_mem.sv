module instr_mem # (
    parameter ADDRESS_WIDTH = 12   // memory is 4kB from 0xBFC00000 to 0xBFC00FFF hence 12 bits are needed
)
(
    /* verilator lint_off UNUSED */
    input logic [31:0] A,
    /* verilator lint_on UNUSED */
    output logic [31:0] RD
);

logic [7:0] rom_array [2**ADDRESS_WIDTH-1:0];

initial begin
    $display("Loading instruction memory.");
    $readmemh("program.hex", rom_array);
end


logic [ADDRESS_WIDTH-1:0] a;
assign a = A[ADDRESS_WIDTH-1:0];
assign RD = {rom_array[a+12'd3], rom_array[a+12'd2], rom_array[a+12'd1], rom_array[a]};


endmodule
