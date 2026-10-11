# Embedded Device Controller & Automated Validation Platform

C11 Linux user-space DC power-source simulator with automated validation.
Developed on Ubuntu / WSL2 using GCC, CMake, CTest and Python.

## Behavior

- READY -> RUNNING -> READY state machine; faults latch until reset.
- Voltage setpoint: 1–24 V, initialized to 12 V; configurable only in READY.
- Load model: Vout = max(0, Vtarget - Iload * 0.05); Pout = Vout * Iload.
- Regulation: +/-2%; failures keep the device running.
- OVP >25 V, OCP >5 A, OPP >100 W; protection disables simulated output.
- OCP/OPP recovery requires a safe prospective load check while in FAULT.
- Triggering measurements remain as diagnostic snapshots after shutdown.

## Build and test

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel
./build-debug/device_sim
ctest --test-dir build-debug --output-on-failure
DEVICE_BUILD_DIR="$PWD/build-debug" .venv/bin/python -m pytest -q
```

Python setup: `python3 -m venv .venv`, then
`.venv/bin/python -m pip install -r requirements-dev.txt`.
The original setup records installed versions in `requirements-dev.lock.txt`.

Use VS Code connected to WSL and install the recommended extensions.
F5 uses the separate `build` Debug configuration in `.vscode/launch.json`.

## Layout

- `firmware/src/device.c`: device state and protection policy.
- `firmware/src/power_model.c`: stateless steady-state model.
- `tests/test_device.c`, `tests/test_voltage.c`: state and voltage tests.
- `tests/test_power_model.c`: 14 independently registered cases.
- `tests/test_sim.py`: executable output integration check.
- `docs/week1_validation.md`: verification results and evidence locations.
- `docs/test_spec.md`: tested scenarios, assertions and known coverage gaps.

## Scope

No physical hardware, switching/PWM dynamics, transient response, UART or SCPI.
`output_enabled` is a simulated control flag; it is not a physical disconnect.
OVP injection reset assumes its external cause has been removed.
Week 2: binary framing, CRC, ring buffer and command parser.
