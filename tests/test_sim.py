import os
from pathlib import Path
import subprocess

def test_sim_output():
    root = Path(__file__).resolve().parents[1]
    build = Path(os.environ.get("DEVICE_BUILD_DIR", str(root / "build")))
    result = subprocess.run([str(build / "device_sim")], capture_output=True, text=True, check=True)
    lines = result.stdout.splitlines()
    assert len(lines) == 7
    expected = [
        (12, 0, 12, 0, "PASS"),
        (12, 3, 11.85, 35.55, "PASS"),
        (12, 5, 11.75, 58.75, "REGULATION FAIL"),
        (12, 5.1, 11.745, 59.8995, "FAULT (OCP)"),
        (24, 4, 23.8, 95.2, "PASS"),
        (24, 4.5, 23.775, 106.9875, "FAULT (OPP)"),
    ]
    import re
    for line, (target, current, voltage, power, status) in zip(lines[:6], expected):
        match = re.fullmatch(
            r"Target: ([0-9.]+) V, Load: ([0-9.]+) A, Vout: ([0-9.]+) V, Power: ([0-9.]+) W -> (.+)", line
        )
        assert match is not None
        for actual, wanted in zip(map(float, match.groups()[:4]), (target, current, voltage, power)):
            assert abs(actual - wanted) < 0.011
        assert match.group(5) == status
    assert lines[6] == "FAULT measurements above are pre-protection snapshots."
