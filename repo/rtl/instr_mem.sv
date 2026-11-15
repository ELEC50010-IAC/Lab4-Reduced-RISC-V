module instr_mem # (
    parameter ADDRESS_WIDTH = 10,   // memory holds 2**ADDRESS_WIDTH bytes
)
(
    input logic [31:0] addr,
    output logic [31:0] dout
);

logic [7:0] rom_array [2**ADDRESS_WIDTH-1:0];

initial begin
    $display("Loading instruction memory.");
    $readmemh("../rtl/program.hex", rom_array);
end

assign dout = {rom_array[addr+3], rom_array[addr+2], rom_array[addr+1], rom_array[addr]};


endmodule
