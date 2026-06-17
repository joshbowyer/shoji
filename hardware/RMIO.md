# SmartLife Pocket — RMIO Expansion Connector

> The standardized expansion connector for the Pocket Companion. Designed for runtime-mutable pin config via `luckfox-config` (no kernel rebuild for most things).
> Status: v0 (2026-06-17). Pin map is a working proposal — verify against the actual Lyra Zero W pinout and module needs before committing.

## What is RMIO

**RMIO = "Raspberry-Module-IO"** (working name, open to better suggestions). It's a 40-pin, 2-row, 2.54mm-pitch connector on the Pocket's expansion bay. Pin map is documented and stable. Configuration is overlay-driven (device tree overlays) so modules can be added/swapped without recompiling the kernel.

Why this matters:
- **Snap-on modules** (LoRa, IR, thumb keyboard, sensors, etc.) need a stable pin contract
- **Community contributions** need a documented standard, not a custom wiring job per module
- **3D-printed enclosures** for modules need to know the connector's physical spec
- **Recovery from misconfig** is easy — edit `/etc/luckfox.cfg`, reboot

The connector is **physically compatible with Raspberry Pi HATs** (40-pin, 2×20, 2.54mm) so a community member can prototype with off-the-shelf Pi HATs and adapt them. **Electrically it's Lyra-specific** — the pin functions we expose are the ones the Lyra can actually deliver, with a custom pin map.

## Physical spec

- **Connector:** 2×20 pin header, 2.54mm pitch, female on the Pocket side (modules plug in)
- **Mounting:** M2.5 standoffs at standard Pi HAT positions
- **Module PCB size:** 65mm × 30mm typical (Pi HAT-compatible), can be smaller
- **Module retention:** 4× M2.5 screws (standard Pi HAT mounting)
- **3D-print integration:** snap-on / screw-on module cases that mate with the Pocket's expansion bay

## Pin map (working proposal)

Standard 40-pin layout, top-down view of the connector on the Pocket. Pin 1 is marked.

```
                          Pocket Expansion Connector
                          (top view, looking at the device)

  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
  | 3V3 | GP5 | GP6 | GP7 | GP8 | GP9 | GND | GP10| GP11| GP12|
  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
  |  5V | I2C1| I2C1| GND | UART| UART| GND | SPI1| SPI1| SPI1|
  |     | SDA | SCL |     | TX  | RX  |     | MOSI| MISO| SCLK|
  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
  | 3V3 | I2C2| I2C2| GND | SPI2| SPI2| SPI2| SPI2| GND  | GP13|
  |     | SDA | SCL |     | CS0 | CS1 | MOSI | MISO |     |     |
  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
  | GP14| GND | GP15| GP16| GP17| GP18| GND | GP19| GP20| GP21|
  | PWM0|     | PWM1| ADC0| ADC1|     |     |     | IRQ | IRQ |
  +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
```

**Pin function summary:**

| Function | Pins | Notes |
|---|---|---|
| **3.3V power** | 2 pins (1, 23) | 3.3V regulated, max ~500mA combined |
| **5V power** | 1 pin (11) | 5V from USB-C input, max ~1A |
| **GND** | 5 pins (7, 14, 18, 25, 30) | Distributed for clean returns |
| **I²C bus 1** | 2 pins (12, 13) | 400kHz default; for OLED, sensors, fuel gauge |
| **I²C bus 2** | 2 pins (24, 25) | 400kHz default; for second sensor cluster |
| **SPI bus 1** | 3 pins (15, 16, 17) | For high-speed peripherals; e-ink uses this by default |
| **SPI bus 2** | 4 pins (26, 27, 28, 29) | 2× chip-select capable; for LoRa, second display, etc. |
| **UART** | 2 pins (14, 15) | 3.3V TTL; for GPS, serial modules, debug |
| **GPIO** | 8 pins (3, 4, 5, 6, 8, 9, 10, 22) | General purpose, multi-function |
| **PWM** | 2 pins (31, 33) | For IR blaster LED, dimming, motor control |
| **ADC** | 2 pins (34, 35) | 0–3.3V analog input; for sensors |
| **IRQ** | 2 pins (39, 40) | Interrupt-capable GPIOs; for wake, mesh events |

**Total: 40 pins** — 8 power, 12 data (I²C/SPI/UART), 12 GPIO/PWM/ADC/IRQ, 8 general GPIO.

This is a **proposal** and the actual pinout depends on:
1. The Lyra Zero W's available GPIO pins (need to map against the actual SoC pinout)
2. The Lyra Zero W's MIPI DSI / SPI / I²C bus allocation (which buses are free vs used by on-board peripherals)
3. Any pin conflicts with the e-ink main display and OLED strip (which use SPI1 and I²C1 respectively)

The next step is to map this against the Lyra's actual pinout, which is the **first hardware spike** when we get there.

## RMIO overlay (proposal for `luckfox-config`)

When a community member attaches a module, they drop a config snippet into `/etc/luckfox.cfg` to enable the relevant pins. Example for a hypothetical LoRa module on SPI2, CS0, with an IRQ on pin 39:

```ini
# /etc/luckfox.cfg — SmartLife RMIO LoRa module
# Requires reboot to apply (SPI display-class device)

[rmio_lora]
compatible = "smartlife,rmio-lora"
spibus = 2
cs = 0
irq_pin = 39
reset_pin = 5
```

A `smartlife-rmio-overlay.dtbo` ships with the OS image and provides the base RMIO pin definitions. Module-specific snippets are layered on top.

## Module catalog (planned, not built)

| Module | Buses | Notes |
|---|---|---|
| **(empty / dummy)** | — | Placeholder, shows the connector |
| **OLED strip (2.4" SH1106)** | I²C1 | Actually built into the Pocket; not a module, but uses the same overlay pattern |
| **E-ink (4.2" Waveshare)** | SPI1 | Built into the Pocket; primary display |
| **LoRa (SX126x)** | SPI2, IRQ | Off-grid mesh; Meshtastic / MeshCore / Reticulum |
| **IR blaster + receiver** | PWM0, GP14 | Universal remote |
| **Thumb QWERTY keyboard** | I²C1 (matrix scanner) | For Pocket without on-screen keyboard |
| **Camera (CSI)** | CSI (Lyra native) | Optional; Lyra Zero W has CSI |
| **Sensor pack (IMU + temp + light)** | I²C2 | Optional; 9-DOF IMU, BME280, etc. |
| **Fuel gauge + battery management** | I²C1 | For non-Lyra battery setups |
| **GPS** | UART, IRQ | Location |

Each module gets:
- A 3D-printable case (snap-on or screw-on, mates with the Pocket's expansion bay)
- A KiCad reference design
- A `luckfox-config` snippet for `/etc/luckfox.cfg`
- A short doc on the module's wiki page

## What this doc locks in (when confirmed)

- 40-pin RMIO connector as the SmartLife Pocket expansion standard
- Raspberry Pi HAT-physical-compatible (2×20, 2.54mm)
- Overlay-driven configuration via `luckfox-config` + `/etc/luckfox.cfg`
- Module catalog above as the planned set
- Community contribution path: drop a config snippet + KiCad files + 3D-print files

## Open questions / next spikes

- **Map the RMIO pin map against the actual Lyra Zero W pinout** — first hardware spike
- **Allocate SPI/I²C buses** — confirm SPI1 is free for e-ink, I²C1 is free for OLED, etc.
- **Power budget** — 3.3V/5V pins max current; document for module designers
- **Boot-time overlay loading** — does `luckfox-config` reliably load the RMIO overlay at boot, or do we need a custom systemd service?
- **Hot-plug** — most peripherals need reboot. Can we design a subset that supports hot-swap? (Likely no for displays/cameras, yes for I²C sensors if we're careful.)

## See also

- `research/lyra-platform-tooling.md` — `luckfox-config` and dynamic device tree
- `research/lyra-os-landscape.md` — OS options
- `software/STACK-DECISION.md` — Phosh + phoc on Lyra
- `software/DEV-WORKFLOW.md` — dev loop
