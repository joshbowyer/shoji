# SmartLife System

> **Status:** Brainstorming → early prototype. Source-of-truth: [`grok-brainstorm.md`](./grok-brainstorm.md).
> Working name: **SmartLife System** (Grok branded it "EchoSystem" in the brainstorm — see [`BRIEF.md`](./BRIEF.md)).
> **This repo: `shoji`** — the e-ink desktop environment (the Pocket's shell). The overall SmartLife System is the three-device ecosystem (Pocket + Home Base + Commander); Shoji is the user-facing DE that runs on the Pocket.

## What is this?

SmartLife System is a privacy-first, fully local AI smart-life platform built around three coordinated devices. Every model, every byte of data, every rule lives in your house — no cloud, no telemetry, no vendor lock-in. The goal is a modern offline PDA experience plus intelligent home/life orchestration that is resilient to outages, surveillance, and the coming wave of AI-driven attacks.

**Shoji** is the e-ink-first desktop environment for the Pocket Companion — a custom shell that replaces Phosh's mobile shell because Phosh's animation vocabulary is wrong for e-ink. Full design in [`ui/EINK-DE-SHELL.md`](./ui/EINK-DE-SHELL.md). The repo lives at `github.com/joshbowyer/shoji`.

The idea is to combine ultra-low-power SBCs (Luckfox Lyra, Pi Zero class) with a hardened always-on home hub, run small tuned models on the edge devices and larger quantized models on the hub, and orchestrate the whole thing with a multi-agent local stack. When you walk out of WiFi range, your pocket device keeps working on its own; when you walk back in, it quietly offloads the heavy stuff to the home hub. Optional mesh radios (LoRa) keep you reachable off-grid.

## Architecture — three devices, one system

### Pocket Companion
Ultra-portable daily brain. Built around the **Luckfox Lyra Zero W** (Pi Zero form factor, $15–30, WiFi 6 + BT 5.2, MIPI DSI). **4–6" e-ink main panel + a small OLED strip** for live feedback (AI token stream, voice waveform, typing echo). Capacitive touch + physical buttons, MEMS mic + small speaker, 1500–3000mAh battery with deep-sleep M0 co-processor (weeks/months standby). Snap-on modules via USB-C / GPIO: thumb QWERTY, LoRa, IR blaster, sensors, optional camera. Runs a minimal Foxbuntu image and a quantized tiny LLM (Phi-3 mini / Gemma-2B / TinyLlama) for offline notes, quick AI queries, voice, IR control, Home Assistant glances, and mesh comms.

### Home Base
The always-on brain. **NOT** a $3000 workstation — a $200–600 fanless mini-PC (Intel N100/N200 or Ryzen 5/7 class) sits behind the router on a VLAN with 16–32GB RAM and an SSD. Runs the full **Ollama** stack (7B–13B quantized), a multi-agent orchestration layer (**OpenClaw** for persistent agents + **LangGraph** for stateful workflows + Hephaestus-style phase-based subagents), and **Home Assistant**. Pocket/Commander offload complex tasks to it over encrypted local WiFi (WireGuard, MQTT, Syncthing). Hardened OS, no unnecessary egress, only required ports exposed.

### Commander
A stationary 7–10"+ MIPI DSI e-ink dashboard. Could be a Lyra-based build or a bigger Lyra Plus/Pro variant. **Pure e-ink** (no OLED — it's the always-glanceable device, e-ink is exactly right). Always-visible widgets for calendar, home status, weather, overnight AI briefing, controls. E-ink partial refresh keeps power draw negligible. Same modular 3D-printed case language as the Pocket, so the family looks cohesive on a shelf or mounted on a wall.

## Key differentiators

- **Privacy-first** — data, models, and rules live locally. No cloud dependency for any core function.
- **Offline by default** — works on a boat, in a cabin, during an outage, or in a Faraday bag.
- **Multi-agent local intelligence** — specialized tuned agents (Home Control, Personal Assistant, Security, Mesh, Vision) orchestrated on the Home Base, each with its own model size and skillset.
- **Modular hardware** — standardized GPIO expansion (RMIO-style, not just Grove), 3D-printable snap-together cases, KiCad templates for community breakouts.
- **E-ink-first UX** — battery-friendly, sunlight-readable, partial-refresh-aware custom UI on top of a Foxbuntu base. **Hybrid display** (e-ink canvas + small OLED strip) on Pocket; pure e-ink on Commander. See `ui/DISPLAY-STRATEGY.md`.
- **Resilient** — mesh radios (Meshtastic / MeshCore / Reticulum) keep comms alive when IP is gone. VLAN isolation + WireGuard keep the hub safe.

## Documents

- [`BRIEF.md`](./BRIEF.md) — one-page distilled concept (pitch, hardware table, stack, users)
- [`TODO.md`](./TODO.md) — first-sprint prototype priorities by track
- [`questions-for-josh.md`](./questions-for-josh.md) — open decisions blocking progress
- [`grok-brainstorm.md`](./grok-brainstorm.md) — full Grok conversation (do not edit)

## Subdirectories

- `hardware/` — specs, BOMs, GPIO pin maps, 3D-print files, panel test results
  - [`hardware/RMIO.md`](./hardware/RMIO.md) — RMIO expansion connector (40-pin, runtime-mutable via `luckfox-config`)
- `software/` — stack notes, agent configs, Foxbuntu fork notes, sync setup
  - [`software/STACK-DECISION.md`](./software/STACK-DECISION.md) — Phosh + phoc + squeekboard, Lyra Zero W, e-ink + OLED parts, boot flow
  - [`software/DEV-WORKFLOW.md`](./software/DEV-WORKFLOW.md) — laptop → Pinephone → Lyra dev loop
- `ui/` — design system, display strategy, e-ink refresh policy, screen mockups
  - [`ui/EINK-DE-SHELL.md`](./ui/EINK-DE-SHELL.md) — **the custom e-ink-first shell design** (the active goal)
  - [`ui/DESIGN-SYSTEM.md`](./ui/DESIGN-SYSTEM.md) — the SmartLife HIG, GNOME-inspired
  - [`ui/DISPLAY-STRATEGY.md`](./ui/DISPLAY-STRATEGY.md) — the e-ink + OLED hybrid decision
  - [`ui/DEVICE-MOCKUP.md`](./ui/DEVICE-MOCKUP.md) — visual concept: e-ink + centered OLED island, 9 UI states
  - `ui/heroes/` — rendered hero concepts (PNG/JPEG) showing the device in AI chat and idle states
- `research/` — sub-investigations (mesh stack comparison, tiny model benchmarks, NPU options)
  - [`research/lyra-os-landscape.md`](./research/lyra-os-landscape.md) — OS options for Lyra Zero W (mPWRD-OS recommended, Armbian fallback)
  - [`research/lyra-platform-tooling.md`](./research/lyra-platform-tooling.md) — pin config / device tree priority order: mainline > `configfs` > `luckfox-config` > SDK

## Status

**Shoji — the e-ink DE — is the only active goal.** Phosh's shell is the wrong fit for e-ink; we're building a custom e-ink-first shell on Phoc + squeekboard + libadwaita + Wayland + PAM. Full design in `ui/EINK-DE-SHELL.md`. Everything else (AI, HA, mesh, security) is paused until a working e-ink DE exists.

UI stack: **custom e-ink-first shell** on **Phoc + squeekboard + libadwaita**. OS base: **mPWRD-OS** (Armbian Trixie + Meshtastic, maintained by vidplace7) with our `smartlife-os` as a `userpatches` overlay. Display strategy: hybrid (e-ink main + 2.4" centered OLED island). Phase 1 hardware: Lyra Zero W (on hand), Waveshare 4.2" e-ink SPI, Waveshare 2.4" SH1106 OLED I²C. RMIO expansion connector spec'd. "Minimize Luckfox pain" principle: mainline drivers > `configfs` overlays > `luckfox-config` > SDK. Lineage: Josh (initial Lyra kernel, Femtofox/Foxbuntu co-creator, pin map) + vidplace7 (Armbian Lyra, mPWRD-OS) + wehooper4 (Meshtastic hardware). Testing tiers: **browser sim** (next deliverable) → Pinephone → Pi + Waveshare HAT → Lyra. The brainstorm with Grok is captured in `grok-brainstorm.md`. No calendar — work on it when you feel like it.
