# SmartLife System — Brief

> **Name note:** Grok branded this concept "EchoSystem" in the brainstorm. **Working name is SmartLife System** (per Josh's preference). Use "SmartLife System" externally; "EchoSystem" only appears in historical refs.
> Distilled from `grok-brainstorm.md`.

## Elevator pitch

A privacy-first, fully local AI smart-life ecosystem built around three coordinated devices — a pocket companion, a hardened home hub, and an e-ink command tablet — that runs a multi-agent AI stack on hardware you own, in your house, with no cloud dependency. Modern offline PDA plus intelligent home orchestration, resilient to outages, surveillance, and the coming wave of AI-driven attacks.

## Hardware overview

| | Pocket Companion | Home Base | Commander |
|---|---|---|---|
| **Role** | Ultra-portable daily brain, on-the-go control | Always-on hub, heavy compute, HA + agents | Stationary glanceable dashboard |
| **Form factor** | Credit-card-thick handheld, 3D-printable | Fanless mini-PC behind router | Tablet-shaped 7–10"+ e-ink, wall or desk |
| **Candidate board** | Luckfox Lyra Zero W (~$15–30) | Beelink N100/N200 (~$150–250) or Ryzen 5/7 mini (~$400–600) | Lyra-based build or larger Lyra Plus/Pro |
| **CPU/RAM** | Triple-core Cortex-A7 @1.2GHz + M0 co-proc, 512MB DDR3L | N100 4C/4T or Ryzen 5/7, 16–32GB DDR5 | Lyra-class A7, 512MB–few GB |
| **Storage** | 256MB SPI NAND + microSD | 256GB–1TB SSD | microSD / eMMC |
| **Display** | **4–6" MIPI DSI e-ink main + 1–2" OLED strip** (hybrid; OLED off by default) | None (headless server) | **7–10"+ MIPI DSI e-ink only** (pure e-ink — always-glanceable device) |
| **Radio** | WiFi 6 + BT 5.2/BLE; optional LoRa (SX126x SPI), IR, NFC | Wired Ethernet; WiFi AP optional | WiFi 6 + BT |
| **Power** | 1500–3000mAh LiPo/LiFePO4, deep-sleep M0, optional solar trickle | 24/7 mains, fanless, UPS optional | Larger battery or USB-C mains |
| **I/O** | USB-C OTG, GPIO banks, capacitive touch, physical buttons, MEMS mic + speaker, optional camera | Standard mini-PC I/O | Touch, GPIO for snap-on modules |
| **BOM (volume)** | $40–80 | $150–300 | $60–120 |
| **Retail target** | Kit $99, assembled $149–199 | n/a (self-built or sourced) | Kit $129–179 |

## Software / AI stack

**Base OS**
- Pocket & Commander: Foxbuntu (Ubuntu image for Luckfox) — Josh has contributor background here
- Home Base: Hardened minimal Ubuntu / Proxmox, AppArmor or SELinux, no unnecessary egress

**Edge models (Pocket & Commander)**
- Quantized tiny LLMs: Phi-3 mini, Gemma-2B, TinyLlama
- Voice: Whisper tiny + Piper TTS
- Vision: MobileNet / YOLO on optional camera modules
- Local embeddings + RAG over personal notes

**Hub models (Home Base)**
- **Ollama** for model serving (7B–13B quantized typical, up to 70B if hardware allows)
- Specialized per-agent model sizes

**Multi-agent orchestration (Home Base)**
- **OpenClaw** — persistent autonomous agents, skills, memory, heartbeat, local execution
- **LangGraph** — stateful workflows, checkpoints, HITL, branching, replay
- **Hephaestus-style** subagents for dynamic phase-based discovery
- Specialized agents: Home Control, Personal Assistant, Security/Monitoring, Mesh/Comms, Vision/Sensor
- Orchestrator routes tasks; agents can be tuned per task (LoRA)

**Connectivity & sync**
- Encrypted local sync: Syncthing or rsync + WireGuard
- Home Assistant: native companion or MQTT/WebSocket client
- Mesh: Meshtastic / MeshCore / Reticulum over LoRa (evaluation pending)

**UI**
- Custom e-ink-first frontend on top of Foxbuntu
- Partial-refresh-aware, dark/light themes, one-handed friendly
- Voice (Whisper tiny + Vosk), button shortcuts, swipe gestures
- Modes: Notebook, Dashboard, AI Chat, Controls

## Target users

- **Privacy enthusiasts** — want a smart home/AI life without cloud exposure
- **Makers / Foxbuntu community** — already in the SBC ecosystem, want a polished product to build on
- **Hikers / field users** — need off-grid comms, sensors, and AI assistance without cell service
- **Smart-home power users** — run Home Assistant, want a privacy-respecting control surface
- **Tinkerers / privacy-curious parents** — want a kid-safe / elderly-safe local AI companion

## Use cases (vignettes)

1. **Morning briefing on the Commander** — Home Base agents summarize overnight events (motion, energy, weather, calendar) and render a glanceable e-ink card before the user wakes.
2. **On-the-go control from the Pocket** — user pulls the Pocket out, asks "did I close the garage?"; tiny local model answers or offloads to Home Base over WiFi.
3. **Mesh alert in a blackout** — LoRa-equipped Pocket relays a "door opened" event from a remote sensor node to the Home Base when the LAN is down.
4. **Multi-agent handoff at home** — Pocket asks a complex RAG question; Home Base routes to the Personal Assistant agent, which pulls from indexed docs and replies; Pocket renders the answer on e-ink.
5. **Routine automation** — Security agent notices an anomaly, alerts Pocket via mesh if user is away, and the Home Control agent turns on lights and starts a recording.

## Open scope (defer to follow-on docs)

- Detailed hardware validation plan → `hardware/`
- Software stack spikes and notes → `software/`
- E-ink UI design language → `ui/`
  - `ui/DESIGN-SYSTEM.md` — the SmartLife HIG, GNOME-inspired
  - `ui/DISPLAY-STRATEGY.md` — the e-ink + OLED hybrid decision
- Sub-investigations (mesh comparison, tiny model benchmarks) → `research/`
- First-sprint todo list → `TODO.md`
- Blocking decisions → `questions-for-josh.md`
