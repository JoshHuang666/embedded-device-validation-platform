# Stage 3: steady-state load and power model

The controller uses 1–24 V, 5 A, 100 W, 0.05 ohm output resistance and 2%
voltage tolerance. The independent model takes a configuration and produces
measurements without changing controller state.

Vout = max(0, Vtarget - Iload * Req); Pout = Vout * Iload.
Negative voltage is clamped to zero. Nonfinite inputs, negative resistance or
current, nonpositive target/limits and calculation overflow are rejected.
Invalid calls leave the result unchanged. The device restricts target voltage
to 1–24 V; the standalone model accepts any positive finite target.

OCP is strictly above 5 A; OPP is strictly above 100 W. If both trigger,
both flags latch and OCP has return-code priority. Regulation failure leaves
the controller RUNNING. Fault measurements are snapshots before protection,
not ongoing physical output. The model never generates OVP by itself;
external voltage injection and Stage 2 voltage tests still exercise OVP.

## Recovery

An immediate reset after OCP/OPP returns DEV_ERR_UNSAFE. Supply a new load via
device_update() while in FAULT. This evaluates prospective output at the
unchanged target, preserves triggering measurements and keeps FAULT latched.
It returns DEV_OK when both protection limits are satisfied, DEV_ERR_UNSAFE
otherwise. DEV_OK here means safe to reset, not resumed operation or regulation
PASS. An invalid load in FAULT revokes the last safe-load confirmation.
device_reset_fault() can then return to READY. Call device_start() and
device_update() to resume. OVP-only reset retains Stage 2's assumption that
the injected fault source has already been removed.

No freshness timeout or physical fault-source verification is modeled.
Supply a new load check before reset if the simulated load changes.

## Verification

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/device_sim
ctest --test-dir build --output-on-failure
.venv/bin/python -m pytest -q
```

CTest has 16 cases: Stage 1 state test, Stage 2 voltage test, and 14 individually
reported power cases. TC01–08 cover the requested scenarios; TC09–14 cover
arguments/configuration, latched OCP/OPP recovery, simultaneous faults,
strict threshold equality and numerical overflow/clamping. Float comparisons
use absolute tolerance. Checks remain active in Release builds.
