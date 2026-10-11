# Week 1 — Device & Power Model Validation

## Project overview

Linux user-space embedded device simulator implemented in C11.
Features: state machine, setpoint validation, regulation checking, OVP, load and
power model, OCP/OPP, fault latching, safe-load recovery and simulated output gate.

## Environment

- UTC validation time: 2026-10-11T15:33:46Z
- OS: Linux-6.6.87.2-microsoft-standard-WSL2-x86_64-with-glibc2.39
- Compiler: gcc (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- CMake: cmake version 3.28.3
- Python: Python 3.12.3
- Valgrind: valgrind-3.22.0

## Model

Vout = max(0, Vtarget - Iload * Req); Pout = Vout * Iload.
Req = 0.05 ohm, current limit = 5 A, power limit = 100 W, voltage tolerance = +/-2%.
Protection thresholds use strict greater-than comparison. OVP is injected
externally; this passive-drop model does not create overvoltage.

## Verification results

- Debug CTest: PASS, 16/16.
- Release CTest: PASS, 16/16.
- Debug/Release/Sanitizer registered test names and arguments: identical.
- Release check audit: PASS; C tests use CHECK, independent of NDEBUG.
- Compilation database confirms Debug symbols, Release NDEBUG and sanitizer
  instrumentation on both core sources and all three C test executables.
- Deliberately failing CHECK compiled with -DNDEBUG: exited nonzero as expected.
- ASan/UBSan CTest: PASS, 16/16; UBSan configured to halt on error.
- Python integration tests: PASS in Debug and Release.
- Valgrind: PASS for DeviceStateTest, VoltageTest, all 14 PowerModel cases and demo.
- Fault recovery: covered by TC10/TC11, including rejected unsafe reset.
- Output shutdown: simulated gate disabled on OCP/OPP/OVP and stays disabled
  through safe-load recheck and reset; START enables it.

Evidence is in ignored `week1-validation.log`, `week1-evidence-i0B3Fo9J/` logs,
and each build directory's `Testing/Temporary/LastTest.log`.
Re-running the closeout creates a fresh evidence directory.
The source-level test specification is in `docs/test_spec.md`.

## Limitations and next milestone

Measurements and shutdown are simulated; no hardware, PWM/switching model,
transient response, UART or SCPI. OVP reset assumes its injected source has been
removed. Safe-load checks have no freshness timeout. This is scenario coverage,
not exhaustive proof. Next milestone: binary protocol, CRC, ring buffer and parser.

## Version

Intended release tag: v0.1.0. This report records validation only; successful Git
commit/tag creation is confirmed separately by the closeout script.
