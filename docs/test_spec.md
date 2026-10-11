# Week 1 test specification

All C test checks use CHECK, which is independent of NDEBUG. Each failed check
returns a nonzero process status. Tests run against the actual core library.
Build directories are separate for Debug, Release and sanitizers.

## Registered CTest cases

- DeviceStateTest: initialization (READY, 12 V), start, repeated start rejection,
  stop, repeated stop rejection, null pointers, INIT/FAULT operation rejection.
- VoltageTest: valid/range-invalid setpoints, RUNNING setpoint rejection,
  nominal/tolerance-failing voltage samples, OVP, fault-reset cleanup, nulls,
  NaN/infinity, minimum/maximum setpoints, negative samples, regulation failure
  without fault, 25 V not OVP, FAULT API rejection and restart. OVP disables output.
- PowerModel_TC1: 12 V, 0 A -> 12 V and 0 W.
- PowerModel_TC2: 12 V, 3 A -> 11.85 V, 35.55 W and controller PASS.
- PowerModel_TC3: 12 V, 5 A -> 11.75 V; no OCP, REGULATION FAIL, still RUNNING.
- PowerModel_TC4: 12 V, 5.1 A -> OCP, FAULT and disabled output.
- PowerModel_TC5: 24 V, 4 A -> 23.8 V, 95.2 W; model POWER_OK.
- PowerModel_TC6: 24 V, 4.5 A -> 106.9875 W; model OPP.
- PowerModel_TC7: negative current rejected; model result unchanged, controller
  remains RUNNING.
- PowerModel_TC8: NaN/infinity voltage or current rejected.
- PowerModel_TC9: null configuration/result/device and representative invalid
  configuration values rejected.
- PowerModel_TC10: OCP reset rejected until safe load; FAULT remains latched during
  safe recheck, snapshot preserved, invalid recheck revokes permission; reset and
  restart succeed. Output remains disabled until START.
- PowerModel_TC11: OPP reset rejected until 4 A safe load at 24 V; safe recheck
  does not resume output, reset and restart succeed.
- PowerModel_TC12: OCP and OPP flags both latch; OCP return-code priority; reset
  blocked and output disabled.
- PowerModel_TC13: exactly 5 A and 100 W allowed; >100 W triggers OPP.
- PowerModel_TC14: voltage clamps to zero for large drop; overflow rejected.

Power-model float checks use absolute error <1e-4. Python checks displayed
numbers with a 0.011 tolerance because the demo prints two decimal places.
Python verifies all six demo statuses and the protection-snapshot notice.

## State transitions and output policy

- Initialization from any state -> READY; output disabled. Initialization is a
  full object reset, not a guarded recovery command.
- READY + START -> RUNNING; output enabled.
- RUNNING + STOP -> READY; output disabled; reported measurements zeroed.
- RUNNING + regulation failure -> RUNNING; output remains enabled.
- RUNNING + OVP/OCP/OPP -> FAULT; output disabled; trigger snapshot retained.
- FAULT + START/STOP/setpoint change -> rejected; stays FAULT, output disabled.
- FAULT + load recheck -> stays FAULT, output disabled; safe load allows a later
  OCP/OPP reset. Invalid recheck removes safe-load confirmation.
- FAULT + unsafe RESET -> rejected; stays FAULT, output disabled.
- FAULT + permitted RESET -> READY; output disabled; flags/measurements cleared.
- READY + START after reset -> RUNNING; output enabled.

## Limits of the evidence

The tests cover these scenarios, not all floating-point values or all sequences.
Exact +/-2% tolerance edges are not exhaustively tested. Configuration validation
uses representative cases. There is no physical shutdown circuit, real sensor
input, fault-source freshness check or OVP-clearance measurement.
Fault data is a pre-protection snapshot, not evidence of continuing output.
There is no simulated PWM, switching, transient response or communication stack.
