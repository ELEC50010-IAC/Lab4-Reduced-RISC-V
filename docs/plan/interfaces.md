# Module interfaces: draft for lab session, Thu 19 Nov

Only signals that **cross module boundaries** are listed. How a module works inside is up to its owner.
Names follow Harris & Harris (CamelCase), as the lab README asks.
Status: **keep** = unchanged · **changed** = renamed or widened · **new** = doesn't exist yet.

---

## 1. Modules by package

### A · Control unit + sign extend

**`control_unit.sv`** (changed)

| Port | Dir | Width | Status | Notes |
|---|---|---|---|---|
| `op` | in | 7 | keep | `instr[6:0]` |
| `funct3` | in | 3 | keep | `instr[14:12]`, now actually used |
| `funct7b5` | in | 1 | changed | `instr[30]`, selects add/sub, srl/sra (currently `funct7`) |
| *branch input* | in | ? | **decide** | see section 3 |
| `RegWrite` | out | 1 | keep | |
| `ImmSrc` | out | 3 | changed | was 2 bits |
| `ALUSrcA` | out | 1 | **decide** | only if `auipc` goes through the ALU (see section 3) |
| `ALUSrc` | out | 1 | keep | 0 = RD2, 1 = ImmExt |
| `ALUControl` | out | 4 | changed | was `ALUCtrl`, 3 bits |
| `MemWrite` | out | 1 | new | |
| `ResultSrc` | out | 2 | new | |
| `PCSrc` | out | 2 | changed | was 1 bit |

**`sign_ext.sv`** (changed)

| Port | Dir | Width | Status | Notes |
|---|---|---|---|---|
| `Instr` | in | 32 | keep | |
| `ImmSrc` | in | 3 | changed | adds U and J |
| `ImmExt` | out | 32 | changed | was `ImmOp` |

### B · ALU + register file + muxes

**`alu.sv`** (changed)

| Port | Dir | Width | Status | Notes |
|---|---|---|---|---|
| `SrcA` | in | 32 | changed | was `ALUop1` |
| `SrcB` | in | 32 | changed | was `ALUop2` |
| `ALUControl` | in | 4 | changed | was `ALUctrl`, 3 bits |
| `ALUResult` | out | 32 | changed | was `ALUout` |
| `Zero` | out | 1 | changed | was `EQ`; any extra flags: see section 3 |

**`regfile.sv`** (keep, but the address ports are renamed)

| Port | Dir | Width | Status | Notes |
|---|---|---|---|---|
| `clk` | in | 1 | keep | |
| `A1`, `A2`, `A3` | in | 5 | changed | were `AD1`–`AD3` |
| `WD3` | in | 32 | keep | now driven by `Result` |
| `WE3` | in | 1 | keep | ← `RegWrite` |
| `RD1`, `RD2` | out | 32 | keep | |
| `a0` | out | 32 | keep | debug/output for testbench and Vbuddy |

**`mux.sv`** (keep): 2-input. **`mux4.sv`** (new): 4-input, needed for `ResultSrc` and `PCSrc`.

### C · PC + instruction memory + programs

**`pc_top.sv`** (changed)

| Port | Dir | Width | Status | Notes |
|---|---|---|---|---|
| `clk`, `rst` | in | 1 | keep | reset value becomes `0xBFC00000` |
| `PCSrc` | in | 2 | changed | was `PCsrc`, 1 bit |
| `ImmExt` | in | 32 | changed | was `ImmOp` |
| `ALUResult` | in | 32 | new | `jalr` target (clear bit 0) |
| `PC` | out | 32 | keep | |
| `PCPlus4` | out | 32 | new | needed by the result mux (`jal`/`jalr` write-back) |
| `PCTarget` | out | 32 | new | `PC + ImmExt`, needed by `auipc` if not done in the ALU |

**`instr_mem.sv`** (changed): 4 KB at `0xBFC00000`–`0xBFC00FFF` (currently 1 KB at address 0). Ports `A` in 32, `RD` out 32.

C also owns: F1 in RV32I assembly, the `pdf.s` data file and its output check, and the build wrapper around `tb/compile.sh` and `tb/doit.sh`.

*(B also owns the per-instruction test programs: one small asm test per RV32I instruction, run through `top.sv`.)*

### D · Data memory + result mux + top level

**`data_mem.sv`** (new)

| Port | Dir | Width | Notes |
|---|---|---|---|
| `clk` | in | 1 | |
| `WE` | in | 1 | ← `MemWrite` |
| `A` | in | 32 | ← `ALUResult`; range `0x00000000`–`0x0001FFFF` |
| `WD` | in | 32 | ← `RD2` |
| `funct3` | in | 3 | `Instr[14:12]`; data memory decodes size (byte/half/word) and signedness itself |
| `RD` | out | 32 | → result mux |

**`top.sv`** (changed): ports `clk`, `rst`, `a0` (keep). `a0[7:0]` drives the 8 Vbuddy LEDs for F1; the testbench passes it to Vbuddy. **Decide**: F1 trigger input? D also owns `verify.cpp`.

---

## 2. Control signal encodings (proposed)

| Signal | Code → meaning |
|---|---|
| `ImmSrc` [2:0] | `000` I · `001` S · `010` B · `011` U · `100` J (first three match current code) |
| `ALUControl` [3:0] | `0000` add · `0001` sub · `0010` and · `0011` or · `0100` xor · `0101` slt · `0110` sltu · `0111` sll · `1000` srl · `1001` sra (`add` matches current code) |
| `ResultSrc` [1:0] | `00` ALUResult · `01` ReadData (loads) · `10` PCPlus4 (`jal`/`jalr`) · `11` reserved for `lui`/`auipc` (see section 3) |
| `PCSrc` [1:0] | `00` PCPlus4 · `01` PCTarget (taken branch, `jal`) · `10` ALUResult (`jalr`) |
| `ALUSrc` | `0` RD2 · `1` ImmExt |

---

## 3. Decide at the session

| # | Question | Who decides |
|---|---|---|
| 1 | **Branch decision:** ALU outputs flags (Zero, Negative, Carry, oVerflow) and Control uses them with `funct3`, *or* a comparator outputs one `BranchTaken` signal? | A + B |
| 2 | ~~Does data memory receive `funct3`?~~ **Decided: yes.** One 3-bit signal to carry through the pipeline, and it keeps the work out of A's package. | D |
| 3 | **`lui` / `auipc` path:** (a) `lui` passes ImmExt through the result mux, and `auipc` uses a new `ALUSrcA` mux (PC into the ALU); *or* (b) `auipc` reuses C's `PCTarget` through the result mux, and `lui` passes SrcB through the ALU. | A (with B, C, D) |
| 4 | Does F1 need a trigger input on `top.sv` (Vbuddy button, as in Lab 3)? | C + D |
| 5 | Where does the `pdf.s` data get loaded in data memory (check the brief and `pdf.s`)? | C + D |

---

## 4. Rename list (existing code)

Each owner renames the signals in their own files as part of their sprint work. D fixes `top.sv` while integrating each module as it lands (from Fri 20 Nov).

| Current | Agreed | Files |
|---|---|---|
| `PCsrc` | `PCSrc` | `top.sv`, `pc_top.sv` |
| `ALUctrl` / `ALUCtrl` | `ALUControl` | `top.sv`, `alu.sv`, `control_unit.sv` |
| `ALUsrc` | `ALUSrc` | `top.sv` |
| `ImmOp` | `ImmExt` | `top.sv`, `sign_ext.sv`, `pc_top.sv` |
| `ALUop1` / `ALUop2` | `SrcA` / `SrcB` | `alu.sv` |
| `ALUout` | `ALUResult` | `top.sv`, `alu.sv` |
| `EQ` | `Zero` | `top.sv`, `alu.sv`, `control_unit.sv` |
| `regOp2` | `SrcB` | `top.sv` |
| `AD1`–`AD3` | `A1`–`A3` | `regfile.sv` |
