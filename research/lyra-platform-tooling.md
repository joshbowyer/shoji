# Lyra Platform Tooling — `luckfox-config`, Device Tree, SDK Escape Hatch

> How to configure pins, attach peripherals, and ship device tree changes on the Luckfox Lyra family — without rebuilding the kernel for most things.
> Status: v0 (2026-06-17). Based on `wiki.luckfox.com/Luckfox-Pico-Pi/Luckfox-config/` and adjacent pages.

## TL;DR

**`luckfox-config` is a real, official utility from Luckfox that does dynamic device tree overlay configuration at runtime. It works like `raspi-config` for pin mux — you don't need to rebuild the kernel for most pin/peripheral changes.** Persistent config in `/etc/luckfox.cfg`. Most functions (GPIO, PWM, UART, I²C, SPI) take effect immediately. A few (FBTFT, RGB, touch, CSI) need a reboot. SDK kernel rebuild is the escape hatch for anything `luckfox-config` doesn't cover.

This is **huge for the RMIO expansion connector**: the SmartLife RMIO overlay can be defined once and activated at boot via `luckfox-config`, with no kernel rebuild.

## `luckfox-config` — what it is

- **Binary name:** `luckfox-config` (in the root filesystem)
- **Persistent config:** `/etc/luckfox.cfg` (human-editable, loaded at boot)
- **Backend:** Linux **dynamic device tree mechanism** + the `fdt` tool
- **UI:** TUI (text-mode menu) — arrow keys + enter; on ADB use number keys
- **Access:** serial console or SSH (not local console / display, since these are headless boards)
- **Maintained by:** Luckfox (official)

### What it can configure

| Peripheral | Status | Notes |
|---|---|---|
| GPIO | ✅ supported | Pin mode, direction, value via sysfs |
| PWM | ✅ supported | Multiple channels configurable |
| UART | ✅ supported | Multiple UARTs |
| I²C | ✅ supported | Multiple I²C buses |
| SPI | ✅ supported | Multiple SPI buses |
| FBTFT (display) | ⚠️ reboot required | Driver doesn't support hot-plug |
| RGB (display) | ⚠️ reboot required | Same |
| TouchScreen (Pico Ultra) | ⚠️ reboot required | Same |
| USB | ✅ supported | Host/device mode toggle |
| CSI (camera) | ⚠️ reboot required | Same |

For our use case (e-ink SPI display, OLED I²C, buttons GPIO, IR GPIO/PWM, mic/speaker I²S, optional LoRa SPI, optional CSI camera), most of this is in the **immediate-effect** category. Only the e-ink display and the OLED would need a reboot (FBTFT-class drivers).

### Usage flow

```bash
# Open the TUI
sudo luckfox-config

# Or, edit config directly (also valid)
sudo nano /etc/luckfox.cfg

# Apply changes (auto-runs at boot, or manually)
sudo luckfox-config load
```

When you change a setting:
- Most peripherals (GPIO/PWM/UART/I²C/SPI) → take effect immediately
- Displays / touch / CSI → need `sudo reboot`

### Caveat

The wiki explicitly says: *"the configuration of luckfox-config is based on the default device tree file. If the device tree has been adjusted during image compilation, some functions may not work properly."* So if we customize the dtb at SDK build time, we have to be careful that luckfox-config still works on top of it. Best practice: keep the base dtb stock, do all per-project pin config via `luckfox-config`.

## Device tree — when do we need to touch it

The **base device tree** (the dtb compiled into the kernel image) is what the board boots with. It defines:
- All on-board peripherals (WiFi, BT, MIPI DSI, SPI buses, I²C buses, etc.)
- Default pin mux for each pin
- Default state of each peripheral

You'd touch the base dtb only when:
- Adding a peripheral the kernel doesn't know about (e.g., a specific e-ink panel's SPI config)
- Changing default behavior (e.g., SPI bus speed, I²C bus speed)
- Adding a new device node (e.g., a custom OLED strip as a separate display)

For most of our work, **`/etc/luckfox.cfg` + luckfox-config dynamic overlays are enough** and the base dtb stays stock.

## SDK kernel rebuild — the escape hatch

When you DO need to touch the base device tree or kernel config:

1. **Get the SDK** for the Lyra family. Per `wiki.luckfox.com`, the Luckfox-Lyra-SDK is the right one (not the Pico SDK). It's primarily tested on Ubuntu 22.04 host.
2. **Build environment:** Ubuntu 22.04 x86_64 (per wiki)
3. **Edit device tree:** `.dts` files in the SDK's `kernel/arch/arm/boot/dts/rockchip/`
4. **menuconfig:** for kernel options
5. **Build:** per SDK instructions
6. **Flash:** the resulting image to SD card

Time cost: a fresh SDK build can take 1–4 hours depending on host. Incremental rebuilds (after the first) are much faster.

## RMIO expansion connector — what this means for us

The RMIO standard we define for SmartLife Pocket is **runtime-mutable, and the path we use should follow the priority order above**. The plan:

1. **Define the RMIO pin map** in `hardware/RMIO.md` (40-pin connector, 2.54mm pitch, Raspberry Pi HAT compatible)
2. **Ship a `smartlife-rmio-overlay.dtbo`** — a device tree overlay (built with the upstream Linux `configfs` overlay mechanism) that activates the standard pin functions for the RMIO connector (I²C × 2, SPI × 2, UART × 1, GPIO × 8, PWM × 2, ADC × 2, 3.3V + 5V + GND)
3. **Load the overlay on boot** via a small systemd service that writes to `/sys/kernel/config/device-tree/overlays/smartlife-rmio/status` (standard upstream Linux mechanism, no Luckfox tooling)
4. **Fall back to `luckfox-config` + `/etc/luckfox.cfg` snippets** only for peripherals the upstream path can't reach
5. **Document any module-specific pin requirements** as either additional DT overlay snippets or `luckfox-config` snippets

This means a community member can attach a new module to the RMIO connector and configure its pins by either:
- Adding to the upstream DT overlay (preferred, more powerful)
- Editing `/etc/luckfox.cfg` and rebooting (fallback, simpler)

**No kernel rebuild required in most cases.** No SDK. No Luckfox-specific tooling knowledge needed for the common case.

## What this doc locks in

- Priority order: mainline drivers > `configfs` DT overlays > `luckfox-config` > SDK kernel rebuild
- `luckfox-config` is a useful escape hatch, not the primary tool
- Base device tree stays stock; customization via overlays (upstream Linux path preferred)
- RMIO connector is overlay-driven, community-friendly, follows the upstream path where possible

## Open questions (follow-up spikes)

- **Does mPWRD-OS expose dynamic device tree via `configfs`?** (Josh to ask vidplace7.) If yes, we can do RMIO pin config in `smartlife-os` `userpatches` without forking the build. If no, we need a deeper fork or fall back to `luckfox-config`.
- **Lyra Zero W pin map** — Josh has the notes, will reference when needed. No need to ask the community; it's a Josh source.
- Is the e-ink SPI panel's driver in mainline Linux, or do we need a Luckfox-specific one? (Waveshare 4.2" B/W SPI is well-supported in mainline; should be fine.)
- Does the OLED I²C driver (SH1106 / SSD1309) need anything beyond the standard `i2c-dev` + a userspace library, or is there a kernel driver we should use?
- For the CSI camera (if we add one), is there a working V4L2 driver for the Lyra's CSI interface?
- How do we configure phoc to use the e-ink as primary and the OLED as secondary Wayland output? That's a phoc config question, not a device tree question.
- **What's the actual Mali GPU variant on the Lyra Zero W's RK3506B?** Most likely Mali-G31 (Panfrost-supported), but unconfirmed. **Spike 1** (glxinfo, no screen).

## See also

- `research/lyra-os-landscape.md` — OS options (mPWRD-OS ranked #1) + **lineage** (Josh → vid → mPWRD-OS)
- `hardware/RMIO.md` — RMIO pin map + module designs
- `software/DEV-WORKFLOW.md` — dev loop
