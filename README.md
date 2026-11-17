# EmbeddedLinuxLab

Embedded Linux projects built to practice C++ applications, Yocto image creation, systemd services, and deployment on real hardware.

## EdgeStatusAgent

EdgeStatusAgent is a C++17 program for Raspberry Pi 3. It reads system uptime, available memory, CPU temperature, and Wi-Fi interface state from Linux virtual files and prints one JSON object per snapshot.

The program can print one snapshot with `--once` or continue at a chosen interval with `--interval N`. The Yocto image starts it as a systemd service and sends its output to the journal.

```text
EmbeddedLinuxLab/
└── EdgeStatusAgent/
    ├── CMakeLists.txt
    ├── src/
    ├── sample-*.txt
    └── yocto/meta-edge/
```

### Build and run with sample data

From the repository root:

```bash
cmake -S EdgeStatusAgent -B build/edge-status-agent
cmake --build build/edge-status-agent
cd EdgeStatusAgent
../build/edge-status-agent/EdgeStatusAgent --once
```

Example output from the sample files:

```json
{"uptime_seconds":123.45,"mem_available_kb":233228,"cpu_temperature_c":44.008,"wifi_operstate_up":false}
```

On Raspberry Pi, the systemd service passes `/proc` and `/sys` paths to the program instead of sample files.

### Yocto integration

[`meta-edge`](EdgeStatusAgent/yocto/meta-edge/README) contains recipes for the application, Wi-Fi configuration, SSH access, and an `edge-image` for `raspberrypi3-64`. The layer targets the Yocto `wrynose` series.

The image was built and booted on Raspberry Pi 3. The agent runs under systemd and writes snapshots to the journal. Local Wi-Fi credentials and SSH public keys are prepared before building the image and are excluded from Git.

## Scope

This is a small learning project focused on the path from C++ source code to a running Embedded Linux image. Additional Linux projects may be added to this repository.
