# Test Plan

## How to run
    cd repo/tb
    ./doit.sh                      # all tests
    ./doit.sh tests/alu_tb.cpp     # one test file


| File | Module | Status |
|---|---|---|
| mux_tb.cpp | mux | passing |
| alu_tb.cpp | alu | passing |
| regfile_tb.cpp | regfile | passing |
| instr_mem_tb.cpp | instr_mem | TODO |
| pc_top_tb.cpp | pc_top | TODO |
| (sign-extension unit, name TBC) | sign-extension | TODO |
| (control unit, name TBC) | control | TODO |
| verify.cpp | top (whole CPU) | TODO |

## Evidence
TODO: passing output and a GTKWave screenshot of a0.

