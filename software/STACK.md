# Shoji — UI Stack Decision

> What runs the Pocket's UI. The base shell, the display profile, the hardware, the boot flow. This is the **Shoji** stack.
> **Status: v1 (2026-06-17).** **Major pivot: Phosh's shell is the wrong fit for e-ink. We keep Phosh's infrastructure (phoc, squeekboard, libadwaita, PAM, Wayland) and build a custom e-ink-first shell — Shoji.** Full design in `ui/SHELL.md`.

## TL;DR

| Layer | Decision | Why |
|---|---|---|
| **Wayland compositor** | **phoc** | Standard Wayland, supports multi-output (e-ink + OLED) |
| **On-screen keyboard** | **squeekboard** | Works with Wayland, part of the Phosh ecosystem |
| **Shell** | **Custom e-ink-first shell** (replaces Phosh shell) | Phosh's shell animations are wrong for e-ink; a custom shell designed for the medium is the right call. See `ui/SHELL.md`. |
| **Widget toolkit** | **GTK4 + libadwaita** | GNOME design system, our design language maps 1:1 |
| **Display manager** | **None** — boot directly to custom shell via systemd | Single-user pocket device doesn't need a greeter |
| **Lock screen** | **Custom** (replaces Phosh's built-in) | Full-refresh on wake, no fade, no slide, just calm image + button to unlock |
| **Settings app** | **Custom SmartLife settings** (replaces phosh-mobile-settings) | Subpages for AI & sync, mesh, power, connectivity, display, about |
| **Status bar** | **OLED strip** (replaces Phosh's top status bar) | Battery, signal, mesh state, AI state — on the centered OLED island |
| **OS base** | **mPWRD-OS** (Armbian Trixie + Meshtastic), `smartlife-os` = our `userpatches` overlay | Least Luckfox-specific code, right build pattern, vidplace7 maintains both mPWRD-OS and Armbian Lyra support |
| **Board** | **Luckfox Lyra Zero W** | Matches the brainstorm spec, $15–30, has SPI + MIPI DSI + WiFi 6 |
| **E-ink (Phase 1)** | **Waveshare 4.2" B/W SPI (400×300)** | Most common, easiest bring-up, mainline-supported |
| **E-ink (Phase 2 upgrade)** | **E Ink 4.01" HD MIPI DSI (480×800)** | Native to Lyra, cleaner industrial design |
| **OLED** | **Waveshare 2.4" SH1106 / SSD1309, I²C, mono** | 2.4" matches design system target, I²C simple, mono cheap and low-power |

## The "third one" Josh was trying to remember

In the Pinephone / Phosh world:

| Component | What it is | What it does |
|---|---|---|
| **phoc** | Wayland compositor | Renders surfaces, handles input, manages outputs (incl. multi-display) |
| **Phosh** | Mobile shell | Lockscreen, home, status bar, app drawer, notifications, overview, quick settings — **all one process** |
| **squeekboard** | On-screen keyboard | Separate process, launched by Phosh when a text field is focused |
| **calls** / **chatty** / **phosh-mobile-settings** | Apps | Phone, SMS, settings — separate apps in the Phosh ecosystem |

**The lockscreen is not a separate "third app"** — it's a mode of Phosh. Phosh handles lock + unlock via PAM.

For **multi-user** systems you need a greeter (`greetd` + `tuigreet` / `regreet`). For **single-user** pocket devices (us), skip the greeter entirely:

```
power on → kernel → systemd → phoc + phosh + squeekboard → lockscreen (PAM) → home
```

No lightdm, no gdm, no greeter. Phosh is the session.

## Why Phosh + phoc

1. **GNOME-aligned.** Our design system is GNOME-inspired (Adwaita tokens, libadwaita, content-first, calm, keyboard-first). Phosh is built on GTK + libadwaita. The design language we wrote down in `ui/DESIGN-SYSTEM.md` maps to Phosh's rendering stack for free. We get libadwaita widgets, Adwaita color schemes, the whole ecosystem.
2. **Mature.** Used in production on Pinephone, Librem 5, Mobian, postmarketOS. Not a research project.
3. **Modular and skinnable.** Phosh is designed to be themed, restyled, extended with custom widgets. We can skin it to our SmartLife HIG without forking.
4. **Wayland-native.** Critical: we have two displays (e-ink + OLED strip). Wayland's multi-output model lets us render the Phosh shell to the e-ink surface and route "live" content to the OLED surface as a separate output, all under one compositor session.
5. **Mobile conventions solved.** Lockscreen, app drawer, notifications, status bar, quick settings, virtual keyboard, settings app, multi-tasking overview — none of this has to be written from scratch.
6. **Active upstream.** Won't rot.

## Why not the alternatives

| Alternative | Why not |
|---|---|
| **Plasma Mobile + KWin** | Heavier, more KDE-coupled, same e-ink gap. No advantage for us. |
| **Custom wlroots-based shell from scratch** | Maximum flexibility, but we'd write lockscreen + app drawer + notifications + status bar + keyboard + settings. Phosh already has all of it. |
| **reMarkable-style e-ink native (Toltec / Inkbox / Plato)** | Designed for reading + notes, not a general mobile shell. We lose every mobile convention. **Worth borrowing their e-ink refresh techniques** but not their shell. |
| **Mainline GNOME on touch** | Assumes desktop, mouse, big screen. Wrong fit. |

## Fork vs downstream — why **no fork**

**Recommendation: consume Phosh as-is, add a SmartLife layer on top, contribute the e-ink + OLED bits upstream.**

- Phosh keeps getting better; we get those improvements for free.
- We contribute back the e-ink + OLED bits so the community benefits.
- We focus engineering on SmartLife-specific code, not on maintaining a shell.
- If divergence ever gets painful, vendor the specific files we need and patch locally. **Fork only as a last resort.**

This is the same pattern reMarkable uses with their custom Linux — but they don't want a phone-style shell, we do.

## What we add as the SmartLife layer

| Component | Purpose |
|---|---|
| **Display profile daemon** | e-ink refresh modes (full / partial / spot), OLED on/off policy, low-power state machine, refresh-on-wake |
| **Multi-output config in phoc** | e-ink = primary Wayland output, OLED strip = secondary output |
| **OLED strip renderer** | A small libadwaita-based surface that the AI reply / voice waveform / typing echo content writes to |
| **Home-screen widgets** | Clock, weather, HA glance, AI prompt bar (libadwaita-style, themed) |
| **Custom stylesheet / theme** | Adwaita tokens from `ui/DESIGN-SYSTEM.md`, applied to libadwaita |
| **Status bar move** | Phosh's top status bar moves to the OLED strip in our layout, freeing e-ink for content |
| **Custom lockscreen behavior** | Single-user, fast unlock, optional fingerprint later |
| **Our app set** | Notes, Chat, Home, Mesh — standard GTK/libadwaita apps |
| **SmartLife settings subpages** | AI & sync, mesh, power, accessibility — layered into phosh-mobile-settings |

## Phase 1 OS base (mPWRD-OS)

**mPWRD-OS as the base for our `smartlife-os`.** Built on Armbian Trixie, with `meshtasticd` pre-installed (we want Meshtastic for the Pocket's mesh radio story anyway). Maintained by **vidplace7**, who is also the Armbian board maintainer for Lyra Zero W and the Meshtastic Linux lead.

**Our `smartlife-os` is an Armbian `userpatches` overlay on mPWRD-OS** that adds:
- `phosh phoc squeekboard phosh-mobile-settings` (apt)
- The e-ink display profile daemon
- The OLED strip renderer
- The RMIO pin map + `/etc/luckfox.cfg` snippets (only as needed)
- Our home screen widgets, settings subpages, apps

Build pattern (from mPWRD-OS README):

```bash
git clone https://github.com/armbian/build.git
cd build
git clone https://github.com/SmartLife-System/smartlife-os userpatches  # our fork
./compile.sh luckfox-lyra-zero-w
```

**Fallback (if mPWRD-OS doesn't fit):** stock Armbian Trixie for Lyra Zero W + our own `smartlife-os` userpatches.

**Why mPWRD-OS over the alternatives:**
- Armbian-ecosystem alignment (upstream Linux kernel, upstream Mesa, upstream Debian packages)
- "Minimize Luckfox pain" principle — Luckfox SDK is bad per Josh's experience
- Right build pattern for our `smartlife-os` (Armbian `userpatches`)
- Active maintainer who can answer our hardware questions
- Lyra Zero W is in the supported boards list (alpha status)

See `research/lyra-os-landscape.md` for the full OS landscape and the fallback options.

## Phase 1 hardware (recommend)

### Board: Luckfox Lyra Zero W

- Triple-core ARM Cortex-A7 @1.2GHz + Cortex-M0 co-proc
- 512MB DDR3L RAM, 256MB SPI NAND + microSD
- Onboard WiFi 6 + BT 5.2/BLE
- MIPI DSI display support
- Ample GPIO, USB OTG/host
- $15–30 in volume
- Matches the brainstorm spec

### E-ink: Phase 1 (Waveshare 4.2" B/W SPI, 400×300)

- Most common e-ink panel in this class
- Easiest Linux bring-up (SPI, mainline kernel driver)
- Cheap (~$25–35)
- Tradeoff: SPI, not MIPI DSI — small bridge chip in industrial design
- **For Phase 1, the goal is to validate software/UX, not perfect display hardware.** SPI is fine.

### E-ink: Phase 2 upgrade (E Ink 4.01" HD MIPI DSI, 480×800)

- Native to Lyra's MIPI DSI output
- More phone-shaped resolution
- Cleaner industrial design (no bridge)
- Tradeoff: rarer, more expensive, panel bring-up is its own project

### OLED: Waveshare 2.4" SH1106 / SSD1309, I²C, mono

- 2.4" matches our design system target
- I²C = 2 wires, simple to wire to Lyra
- Mono = cheap, low power, sufficient for status pills + token stream + waveform
- Mainline Linux driver
- Inverted regions for alert states (works fine on mono)
- Color v2 deferred

### Other Phase 1 bits (carryover from brainstorm)

- 1500–3000mAh LiPo/LiFePO4 battery
- USB-C for charging / keyboard / OTG
- Capacitive touch + 4–5 physical buttons (D-pad, select, back, home, voice)
- MEMS mic + small speaker
- microSD for storage

## Boot flow (Phase 1)

```
power on
  ↓
U-Boot (Lyra)
  ↓
Linux kernel (Foxbuntu / SmartLife fork) with e-ink SPI + OLED I²C drivers
  ↓
systemd: phoc + phosh + squeekboard + smartlife-display-profile.service
  ↓
Phosh lockscreen (PAM auth, optional fingerprint)
  ↓
Phosh home → SmartLife home widgets (clock, weather, HA glance, AI prompt)
```

M0 co-processor handles wake sources (button, alarm, mesh event) when the rest of the system is in sleep. Both displays off. Weeks/months standby.

## What this doc locks in (when confirmed)

- Custom e-ink-first shell replaces Phosh shell
- phoc + squeekboard + libadwaita + Wayland + PAM kept as infrastructure
- No display manager, no greeter; boot directly to custom shell
- Custom display profile + multi-output (e-ink + OLED strip) on phoc
- OS base: mPWRD-OS (Armbian Trixie) + our `smartlife-os` userpatches
- "Minimize Luckfox pain" principle: mainline drivers > configfs overlays > `luckfox-config` > SDK
- Lyra Zero W as the Phase 1 board
- Waveshare 4.2" B/W SPI e-ink for Phase 1 (E Ink 4.01" MIPI DSI as Phase 2 upgrade)
- Waveshare 2.4" SH1106 I²C mono OLED
- SmartLife shell is GTK/libadwaita-native, themed with our design tokens
- Everything else (AI, HA, mesh, security) paused until the DE works
- Testing tiers: browser sim → Pinephone → Pi + Waveshare HAT → Lyra

## Open questions

- **Q7** — Foxbuntu fork strategy: fork, downstream, or separate layer? Now matters more.
- **Phase 1 vs Phase 2 e-ink timing** — when do we swap SPI → MIPI DSI? After software/UX validation, or earlier?
- **Phosh version** — latest stable, or pin a version compatible with our Lyra port?
- **Fingerprint sensor on Phase 1** or defer to Phase 2?
- **Other hardware add-ons** (LoRa, IR, BT keyboard) — when to integrate? Q8 in questions.md.
- **Pinephone community connection** — worth reaching out to Mobian / postmarketOS folks early for e-ink display profile work?

## See also

- `ui/SHELL.md` — **the custom e-ink-first shell design** (the heart of the project now)
- `ui/DESIGN-SYSTEM.md` — design language (still applies)
- `ui/DISPLAY-STRATEGY.md` — e-ink + OLED hybrid decision (still applies)
- `ui/DEVICE-MOCKUP.md` — visual concept for the device (still applies)
- `research/lyra-os-landscape.md` — OS options (mPWRD-OS ranked #1)
- `research/lyra-platform-tooling.md` — pin config + device tree
- `questions.md` — open decisions
