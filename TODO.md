# SmartLife System — TODO

> Conventions: `[ ]` pending, `[x]` done, `[?]` needs Josh decision, `[~]` in progress.
> **2026-06-17 PIVOT: e-ink DE is the only active goal. Everything else (AI, HA, mesh, security) is paused until a working e-ink DE exists.**

## 0. Decisions status (updated 2026-06-17)

- [x] Working name: **SmartLife System**
- [x] First prototype: **Pocket** Companion
- [x] Work order: **UX → software → hardware**
- [x] Display strategy: **hybrid** (e-ink main + 2.4" centered OLED island)
- [x] OLED specs: **2.4" SH1106 / SSD1309, I²C, mono** for Phase 1
- [x] UI shell: **Custom e-ink-first shell** (replaces Phosh shell). Phosh *infrastructure* (phoc, squeekboard, libadwaita) kept. See `ui/EINK-DE-SHELL.md`.
- [x] Display manager: **none** — boot directly to custom shell via systemd
- [x] Phase 1 board: **Luckfox Lyra Zero W** (Josh has one on hand)
- [x] Phase 1 e-ink: **Waveshare 4.2" B/W SPI (400×300)**; Phase 2 upgrade is E Ink 4.01" MIPI DSI
- [x] OS base: **mPWRD-OS** (Armbian Trixie + Meshtastic), our `smartlife-os` = `userpatches` overlay
- [x] "Minimize Luckfox pain" principle: mainline drivers > `configfs` > `luckfox-config` > SDK
- [x] Lineage: Josh (initial Lyra kernel, Femtofox/Foxbuntu co-creator, pin map) + vidplace7 (Armbian Lyra, mPWRD-OS) + wehooper4 (Meshtastic hardware)

**Paused until e-ink DE works:** AI stack, HA integration, mesh stack comparison, security, OTA, sync, power management (M0 firmware), all stretch items.

## 1. E-ink DE — THE ACTIVE SPRINT

**Everything in this section is the only thing we work on until the DE works.**

### Tier 1: Browser-based e-ink simulator (next concrete deliverable)

A web page that simulates the e-ink surface + OLED island + refresh modes. Lets us iterate the design language and shell UX every time Josh feels like working on it. No hardware, no flashing, no SSH.

- [ ] Decide on tech stack: vanilla HTML/CSS/JS vs small framework (Lit, Preact, Svelte)
- [ ] E-ink surface canvas: paper-like rendering, ghosting simulation, refresh modes
- [ ] OLED strip canvas: dim by default, "wake" on events, mock live content
- [ ] Centered island layout (matches `ui/DEVICE-MOCKUP.md` Option A)
- [ ] Multiple "screens" demonstrating the shell states: lockscreen, home, app grid, app switcher, settings, AI chat
- [ ] Touch/click interaction
- [ ] Refresh mode controls: full / partial / spot, with visible distinction
- [ ] Deploy as a single HTML file Josh can open locally

### Tier 2: Custom shell on Pinephone

When Tier 1 demonstrates the design, build the actual shell in GTK4 + libadwaita, test on Pinephone (Josh has 3).

- [ ] Shell app skeleton (libadwaita single window)
- [ ] Custom lockscreen (full-refresh, no animation)
- [ ] Pocket home (widget grid: clock, weather, calendar, HA glance, AI prompt, app row)
- [ ] App grid launcher
- [ ] App switcher (card list, no live previews)
- [ ] Custom notification banner (text only)
- [ ] Custom settings (subpages for AI/sync, mesh, power, connectivity, display, about)
- [ ] Status bar migrated to OLED strip (mocked on Pinephone; e-ink main shows the rest)
- [ ] PAM auth wiring
- [ ] squeekboard integration
- [ ] Multi-output config in phoc (e-ink main + OLED island, mocked on Pinephone)

### Tier 3: Real e-ink on Pi + Waveshare HAT

When the shell design is solid, get real e-ink hardware. ~$50.

- [ ] Buy: Pi Zero 2 W + Waveshare 4.2" B/W e-ink HAT
- [ ] Flash Pi OS / Armbian to Pi
- [ ] Bring up the e-ink panel (kernel driver, SPI, partial refresh)
- [ ] Port the custom shell to the e-ink + OLED layout
- [ ] Display profile daemon: e-ink refresh modes (full/partial/spot), OLED on/off, low-power state machine
- [ ] Test ghosting mitigation
- [ ] Test all shell states on real e-ink

### Tier 4: Lyra Zero W (final target)

- [ ] Flash mPWRD-OS to Lyra Zero W (ask vid about dynamic DT exposure first)
- [ ] `glxinfo | grep OpenGL` — confirm Mali + Panfrost
- [ ] Wire e-ink (SPI) and OLED (I²C) to Lyra per RMIO pin map
- [ ] Port the custom shell to Lyra
- [ ] Port the display profile daemon to Lyra
- [ ] M0 deep-sleep wake sources (button, alarm, mesh event)
- [ ] Battery management

## 2. Paused tracks (resume after Tier 1–3 land)

### AI / agents (paused)

- [ ] Pocket tiny-model selection (Phi-3 mini vs Gemma-2B vs TinyLlama)
- [ ] Voice loop (Whisper tiny + Piper TTS + Vosk)
- [ ] AI reply renderer (token stream → e-ink vs OLED)
- [ ] Home Base multi-agent stack (OpenClaw + LangGraph + Ollama)
- [ ] Offload heuristic (when does Pocket use local vs Home Base)
- [ ] Agent taxonomy (Home Control / Personal Assistant / Security / Mesh / Vision)

### Home Assistant integration (paused)

- [ ] Transport: native HA WebSocket API vs MQTT bridge
- [ ] HA companion on Pocket
- [ ] "Glance" entity schema for the home widget

### Security (paused)

- [ ] VLAN isolation plan for Home Base
- [ ] WireGuard for remote access
- [ ] Hardened OS baseline
- [ ] Threat model doc
- [ ] Secrets management

### Mesh / comms (paused)

- [ ] Compare Meshtastic vs MeshCore vs Reticulum
- [ ] Define mesh message schema
- [ ] LoRa bring-up on a companion MCU

### Hardware validation (paused)

- [ ] Confirm 7–10" MIPI DSI e-ink panel SKU for Commander
- [ ] RMIO pin map against actual Lyra Zero W pinout (Josh has notes)
- [ ] BOMs for Pocket, Home Base, Commander
- [ ] 3D-print enclosure family
- [ ] Snap-on module designs

### Cross-cutting (paused)

- [ ] Repo layout decision (monorepo vs split repos)
- [ ] Licensing decision
- [ ] Wiki / docs site candidate

## 3. Stretch (post-DE)

- [ ] Solar trickle-charge reference design for Pocket
- [ ] Optional camera + vision model (MobileNet) on Pocket
- [ ] Premium fine-tunes / "personalities" marketplace shape
- [ ] Rugged / weather-resistant Pocket enclosure variant
- [ ] Standalone Commander vs "big Pocket" decision once panel test is done

---

## Top priority right now (one thing)

**Build the browser-based e-ink simulator (Tier 1).** It's the next concrete deliverable, costs $0, lets us iterate the design language and shell UX every time you feel like it. Once it shows the shell states convincingly, we move to Tier 2 (custom shell on Pinephone).
