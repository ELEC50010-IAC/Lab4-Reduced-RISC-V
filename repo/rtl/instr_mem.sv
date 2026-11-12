module instr_mem # (
    parameter ADDRESS_WIDTH = 8,
              DATA_WIDTH = 32
)
(
    input logic [31:0] addr,
    output logic [DATA_WIDTH-1:0] dout
);

logic [DATA_WIDTH-1:0] rom_array [2**ADDRESS_WIDTH-1:0];

initial begin
    $display("Loading instruction memory.");
    $readmemh("instruction.hex", rom_array);
end

assign dout = rom_array[addr[ADDRESS_WIDTH+1:2]];  // bits 9:2 of addr give an 8 bit word index


endmodule
