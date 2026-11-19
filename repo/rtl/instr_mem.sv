module instr_mem # (
    parameter ADDRESS_WIDTH = 10   // memory holds 2**ADDRESS_WIDTH bytes
)
(
    /* verilator lint_off UNUSED */
    input logic [31:0] addr,
    /* verilator lint_on UNUSED */
    output logic [31:0] dout
);

logic [7:0] rom_array [2**ADDRESS_WIDTH-1:0];

initial begin
    $display("Loading instruction memory.");
    $readmemh("../rtl/program.hex", rom_array);
end


logic [ADDRESS_WIDTH-1:0] a;
assign a = addr[ADDRESS_WIDTH-1:0];
assign dout = {rom_array[a+3], rom_array[a+2], rom_array[a+1], rom_array[a]};


endmodule
