# Phase 2 interfaces: pipeline, cache, predictor

Status: **draft v0 for the Wed 25 Nov kick-off**. Becomes **v1** when agreed.
Only signals that **cross a package boundary** are listed. How a module works inside is up to its owner.
Names follow Harris & Harris with a stage suffix: `F` fetch · `D` decode · `E` execute · `M` memory · `W` write-back. The exact rule is in section 8.
Status column: **keep** = unchanged · **changed** = renamed or widened · **new** = does not exist yet · **removed**.

---

## 1. Stage registers (B)

Every register has an **enable** (`en`, holds its value when low) and a synchronous **clear** (`clr`, loads a bubble). A bubble has `RegWrite`, `MemWrite`, `MemRead`, `Branch`, `Jump` and `Jalr` all 0; the other fields do not matter.

| Register | Control | Data |
|---|---|---|
| **IF/ID** | none | `InstrD`[32] · `PCD`[32] · `PCPlus4D`[32] · `PredTakenD`[1] · `PredTargetD`[32] |
| **ID/EX** | `RegWriteE` · `ResultSrcE`[2] · `MemWriteE` · `MemReadE` · `ALUControlE`[4] · `ALUSrcE` · `ALUSrcAE` · `BranchE` · `JumpE` · `JalrE` | `RD1E` · `RD2E` · `ImmExtE` · `PCE` · `PCPlus4E` (32 each) · `Rs1E` · `Rs2E` · `RdE`[5] · `funct3E`[3] · `PredTakenE`[1] · `PredTargetE`[32] |
| **EX/MEM** | `RegWriteM` · `MemWriteM` · `MemReadM` | `ExecResultM`[32] · `WriteDataM`[32] · `RdM`[5] · `funct3M`[3] |
| **MEM/WB** | `RegWriteW` · `MemReadW` | `ExecResultW`[32] · `ReadDataW`[32] · `RdW`[5] |

`ALUSrcAE` is `ALUSrcA` in stage E (the `auipc` select), not "`ALUSrc`, `AE`". The same goes for `ALUSrcE`.

Enables and clears come from the hazard unit (section 3):

| Register | `en` | `clr` |
|---|---|---|
| PC (`pc_top`, section 4) | `~StallF` | none |
| IF/ID | `~StallD` | `FlushD` |
| ID/EX | `~StallE` | `FlushE` |
| EX/MEM | `~StallM` | none |
| MEM/WB | always 1 | `FlushW` |

`PredTakenD/E` and `PredTargetD/E` are **reserved from day one**. They are tied to 0 until the predictor is attached, so the baseline pipeline is "predict not taken".

**`ExecResult` mux (B, in EX; decide D2).** It picks `ALUResultE` / `PCPlus4E` / `ImmExtE` using `ResultSrcE` (`00` and `01` → ALU, `10` → PC+4, `11` → ImmExt). Loads and stores carry their address in it. Why: `jal`, `jalr`, `lui` and `auipc` results are then forwardable from MEM with no extra muxes, and the WB stage shrinks to `ResultW = MemReadW ? ReadDataW : ExecResultW`.

## 2. `control_unit` (A)

| Port | Dir | Status | Notes |
|---|---|---|---|
| `op`, `funct3`, `funct7b5` | in | keep | |
| `BranchTaken` | in | **removed** | the branch is now decided in EX |
| `PCSrc` | out | **removed** | built in EX from `Branch`/`Jump`/`Jalr` (section 4) |
| `RegWrite`, `ALUSrc`, `ALUSrcA`, `ImmSrc`, `MemWrite`, `ResultSrc`, `ALUControl` | out | keep | |
| `MemRead` | out | **new** | 1 for loads. Tells the cache a read is real, and drives load-use detection |
| `Branch` | out | **new** | 1 for the six conditional branches |
| `Jump` | out | **new** | 1 for `jal` |
| `Jalr` | out | **new** | 1 for `jalr` |

`branch_unit` (B) moves to EX and compares the **forwarded** operands. `regfile` (B) must return a value being written in the same cycle (`A3 == A1/A2` and `WE3` and `A3 != 0` → return `WD3`).

## 3. `hazard_unit` (A) and the forwarding muxes (B)

| Port | Dir | Width | Notes |
|---|---|---|---|
| `Rs1D`, `Rs2D` | in | 5 | from `InstrD` |
| `Rs1E`, `Rs2E`, `RdE` | in | 5 | from ID/EX |
| `MemReadE` | in | 1 | the instruction in EX is a load |
| `RdM`, `RegWriteM` | in | 5, 1 | from EX/MEM |
| `RdW`, `RegWriteW` | in | 5, 1 | from MEM/WB |
| `MispredictE` | in | 1 | from section 4 |
| `MemBusyM` | in | 1 | from the cache (section 5) |
| `ForwardAE`, `ForwardBE` | out | 2 | `00` use `RD1E`/`RD2E` · `01` use `ResultW` · `10` use `ExecResultM` |
| `StallF`, `StallD`, `StallE`, `StallM` | out | 1 | hold PC / IF/ID / ID/EX / EX/MEM |
| `FlushD`, `FlushE`, `FlushW` | out | 1 | clear IF/ID / ID/EX / MEM/WB |

**Forwarding muxes (B, in EX).** `ForwardAE` selects the source of `SrcAE`, `ForwardBE` the source of the forwarded `RD2E`. That forwarded value feeds the `ALUSrc` mux (`SrcBE`), `WriteDataE` (store data) and the branch unit.

**Rules**

- **Forward:** `RegWriteM && RdM != 0 && RdM == Rs1E` → `ForwardAE = 10`. Otherwise `RegWriteW && RdW != 0 && RdW == Rs1E` → `01`. Same for `Rs2E` → `ForwardBE`. MEM wins over WB.
- **Load-use:** `MemReadE && RdE != 0 && (RdE == Rs1D || RdE == Rs2D)` → `StallF = StallD = 1`, `FlushE = 1`. It may stall when the instruction in ID does not really read `rs2`. That costs speed, not correctness.
- **Mispredict:** `MispredictE` → `FlushD = FlushE = 1`.
- **Memory stall:** `MemBusyM` → `StallF = StallD = StallE = StallM = 1` and `FlushW = 1` (the instruction in MEM has not finished, so WB gets a bubble).
- **Priority: `MemBusyM` > `MispredictE` > load-use.** While `MemBusyM` is high no flush takes effect and the branch in EX waits with everything else. `MispredictE` cancels a load-use stall in the same cycle.

## 4. Mispredict and redirect (A, in EX) and `pc_top` (C, in IF)

**In EX**

```
PCTargetE    = PCE + ImmExtE                                       // adder in EX (B), moved here from pc_top
TakenE       = JumpE | JalrE | (BranchE & BranchTakenE)            // mispredict logic: A
TargetE      = JalrE ? {ALUResultE[31:1], 1'b0} : PCTargetE
MispredictE  = (TakenE != PredTakenE) | (TakenE & PredTakenE & (TargetE != PredTargetE))
RedirectPCE  = TakenE ? TargetE : PCPlus4E
```

An instruction that was predicted taken but is not a taken jump or branch (a table collision) is a mispredict and goes to `PCPlus4E`.

**In IF: `pc_top` (C)** holds the PC, the +4 adder and the next-PC mux. Next PC, highest priority first:

| Condition | next PC |
|---|---|
| `MispredictE` | `RedirectPCE` |
| `PredTakenF` | `PredTargetF` |
| otherwise | `PCPlus4F` |

| Port | Dir | Width | Status | Notes |
|---|---|---|---|---|
| `clk`, `rst` | in | 1 | keep | reset value `0xBFC00000` |
| `en` | in | 1 | **new** | the PC holds when 0. Driven by `~StallF` |
| `Mispredict` | in | 1 | **new** | `MispredictE` |
| `RedirectPC` | in | 32 | **new** | `RedirectPCE` |
| `PredTaken` | in | 1 | **new** | `PredTakenF` (tied to 0 until the predictor is attached) |
| `PredTarget` | in | 32 | **new** | `PredTargetF` |
| `PC` | out | 32 | keep | `PCF` |
| `PCPlus4` | out | 32 | keep | `PCPlus4F` |
| `PCSrc`, `ImmExt`, `ALUResult` | in | | **removed** | the next PC is chosen by the new inputs |
| `PCTarget` | out | | **removed** | `PCTargetE` is computed in EX |

## 5. Cache ↔ pipeline (D and C, used by A and B)

**`data_cache`** replaces `data_mem` in the MEM stage. `data_mem.sv` stays in the repo **unchanged** as the reference.

| Port | Dir | Width | Notes |
|---|---|---|---|
| `clk`, `rst` | in | 1 | `rst` invalidates every line |
| `MemRead` | in | 1 | `MemReadM` |
| `MemWrite` | in | 1 | `MemWriteM` |
| `A` | in | 32 | `ExecResultM`, range `0x00000000`–`0x0001FFFF` |
| `WD` | in | 32 | `WriteDataM` |
| `funct3` | in | 3 | `funct3M`, same codes as `data_mem` (byte/half/word, signed/unsigned) |
| `RD` | out | 32 | `ReadDataM` |
| `MemBusy` | out | 1 | the access in MEM has not finished |

**Behaviour**

- **Hit:** `RD` valid in the same cycle, `MemBusy = 0`. No slower than today's `data_mem`.
- **Miss:** `MemBusy = 1` until the line is in the cache. The access completes in the first cycle `MemBusy = 0`.
- A store takes effect at the clock edge that ends a cycle with `MemBusy = 0`.
- No `MemRead` and no `MemWrite` → never busy.
- `funct3` behaves exactly as in `data_mem`, so the cache and the reference give identical results.

**`main_mem`** (C): the slow memory behind the cache. Parameter `LATENCY` (cycles per line fill). Loads `data.hex` at `0x10000`, as `data_mem` does today. Its port list towards the cache is agreed between C and D on Wed 25 Nov and added here.

## 6. Predictor ↔ pipeline (C, used by A and B)

**`branch_predictor`**

| Port | Dir | Width | Notes |
|---|---|---|---|
| `clk`, `rst` | in | 1 | |
| `PCF` | in | 32 | fetch address |
| `PredTakenF` | out | 1 | |
| `PredTargetF` | out | 32 | |
| `UpdateEn` | in | 1 | `BranchE \| JumpE` (`jalr` is not predicted) |
| `UpdatePC` | in | 32 | `PCE` |
| `UpdateTaken` | in | 1 | `TakenE` |
| `UpdateTarget` | in | 32 | `PCTargetE` |

Parameter `ENTRIES` (default 16). Starting point: direct-mapped BTB (tag + target) with a 2-bit counter per entry; predict taken when the tag matches and the counter is in a taken state. **Stub first:** a module that outputs `PredTakenF = 0` lets the pipeline be finished and tested before the real predictor exists.

## 7. Decide at the kick-off

| # | Question | Default | Who decides |
|---|---|---|---|
| D1 | Where is the branch decided? **EX**: 2 bubbles per taken branch, simple. **ID**: 1 bubble, but needs forwarding into ID and a stall for load → branch | EX (the predictor removes the penalty later) | A + B |
| D2 | One `ExecResult` path (section 1)? | yes | A + B |
| D3 | WB → ID same-cycle read: write-through inside `regfile`, or write on the falling edge? | write-through | B |
| D4 | Cache geometry and policy | 2-way, 16-byte lines, 64 sets (2 KB), LRU, write-through, no-write-allocate. Make all of it parameters so the benchmark can sweep it | C + D |
| D5 | Stores: stall for the memory write, or a 1-entry write buffer? | stall (simplest, still correct) | C + D |
| D6 | `LATENCY` | 6 cycles per line fill, sweep 0 / 6 / 12 | C + D |
| D7 | Predictor | 16-entry BTB + 2-bit counters, `jalr` not predicted | C + A |
| D8 | Test cycle | A → D → B → C → A (A tests D's work, D tests B's, B tests C's, C tests A's) | everyone |
| D9 | Where does the size/sign handling for loads live? | inside the cache, keeping the `funct3` port, so cache and `data_mem` behave identically | C + D |
| D10 | Who reworks `pc_top`, and where does the `PCTarget` adder go? | `pc_top` rework → C (it owns the module and needs the next-PC mux for the predictor). The `PCTargetE` adder moves to EX with B's datapath | B + C |

## 8. Naming

Harris & Harris names. If this section disagrees with the textbook figure, the textbook wins, unless the name is in the "ours" row below.

| Layer | Rule | Example |
|---|---|---|
| Ports of Phase 1 modules (and `data_cache`, which replaces `data_mem`) | keep the port's own name. New ports on them are unsuffixed too | `ALUResult`, `MemRead`, `en`, `Mispredict` |
| Wires in `top.sv` and every diagram | take the suffix of the stage they are in | `ALUResultE`, `MemReadM`, `PCPlus4F` |
| New modules that span stages (`hazard_unit`, the stage registers, `branch_predictor`) | ports are named like the wires they connect to | `Rs1D`, `ForwardAE`, `PredTakenF` |

| Where the name comes from | Names |
|---|---|
| Textbook | `PCF`, `PCPlus4F`, `InstrD`, `RD1D`, `RD2D`, `Rs1D`, `Rs2D`, `RdD`, `ImmExtD`, `SrcAE`, `SrcBE`, `ALUResultE`, `WriteDataE`, `PCTargetE`, `ForwardAE`, `ForwardBE`, `StallF`, `StallD`, `FlushD`, `FlushE`, `ReadDataM`, `ResultW`, `RdW`, `RegWriteW` |
| **Ours** (the textbook has no equivalent) | `ExecResult*` (D2: replaces carrying `ALUResultM` and `PCPlus4M`), `MemRead*`, `MemBusy*`, `Mispredict*`, `RedirectPC*`, `TakenE`, `TargetE`, `BranchTaken*`, `Pred*`, `Update*`, `StallE`, `StallM`, `FlushW` |

**File names** are lower snake case, like the existing `instr_mem.sv` and `control_unit.sv`:

- New RTL: `hazard_unit.sv`, `data_cache.sv`, `main_mem.sv`, `branch_predictor.sv`, `if_id.sv`, `id_ex.sv`, `ex_mem.sv`, `mem_wb.sv`.
- Tests: `<module>_tb.cpp`. `doit.sh` builds `rtl/<module>.sv` from the test file name, so the two must match. Whole-CPU tests are `top-<name>_tb.cpp` and are built against `top`.
