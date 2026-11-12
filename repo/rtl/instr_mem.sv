module instr_mem (
    input logic [31:0] addr,
    output logic [31:0] dout
);

    logic [7:0] rom [255:0];        
    initial begin $readmemh("program.hex", rom);
    end
    assign dout <= {rom[addr[9:2]]+3, rom[addr[9:2]]+2, rom[addr[9:2]]+1, rom[addr[9:2]]};

endmodule