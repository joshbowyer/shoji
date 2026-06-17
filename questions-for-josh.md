# Questions for Josh

> Pulled from `grok-brainstorm.md` §9 plus a few new ones surfaced during scaffolding.
> These are the decisions that gate real progress. **Docs-only scaffolding is unblocked; hardware buys and code commits are not.**

## From the brainstorm (§9)

1. **Working name.** "SmartLife System" (your preference) or stick with Grok's "EchoSystem"? It affects branding, repo names, and any external-facing copy. (Recommend: SmartLife System — more descriptive, less collision-prone.) — **defaulting to SmartLife System unless Josh says otherwise.**

2. ~~**First prototype target.** Pocket Companion, Home Base, or Commander?~~ — **RESOLVED 2026-06-17: Pocket.** UX first, software next, hardware later.

3. ~~**Hardware-first or software-first?**~~ — **RESOLVED 2026-06-17: Software + UX first. Hardware deferred.** (See also: the design system in `ui/DESIGN-SYSTEM.md` answers the e-ink question with a hybrid display recommendation.)

4. **Time budget.** How many hours per week can you realistically put in? This decides whether we aim for a 4-week demo, an 8-week demo, or treat this as background tinkering.

5. **The friend.** Is the friend available to help with hardware/3D-printing/enclosure design? If yes, what is their role and bandwidth? (A friend with a 3D printer changes the enclosure timeline dramatically.) — **deferred until hardware phase.**

## New questions surfaced

6. **Home Base hardware target.** Lock in a specific candidate now (e.g., Beelink EQ12 N100, 16GB) or wait until we have the multi-agent spike results? A specific box makes the first sprint concrete; waiting is more honest. — **deferred until hardware phase.**

7. **Foxbuntu fork strategy.** Do we maintain a true upstream fork, a downstream layer on top of upstream images, or just consume Foxbuntu as-is and add a separate `smartlife-os` layer? Forking has maintenance cost; downstream is faster but less integrated.

8. **Pocket radio priorities.** Which add-on radio is highest priority for you — **LoRa** (off-grid mesh), **IR blaster** (smart remote), or **BT/WiFi only** (skip add-ons for v1)? This shapes the first snap-on module we design. — **deferred until hardware phase.**

9. **Mesh stack preference.** Any prior preference among Meshtastic, MeshCore, Reticulum? (Defaulting to Meshtastic for community size unless you say otherwise.) — **deferred until hardware phase.**

10. **AI personality / fine-tuning direction.** Is the "personal AI" angle (Pocket as a private PDA with a personal LoRA) the headline feature for you, or is the smart-home-orchestration angle the headline? This affects which agents get tuned first.

11. **Demo audience.** When we do a first demo, who is it for — yourself, the friend, the Foxbuntu community, a future Kickstarter audience? This changes polish vs. depth tradeoffs.

12. **Sync boundary.** Should Syncthing-style sync be cross-device (Pocket ↔ Home Base ↔ Commander) and/or Pocket ↔ laptop? Where does the user's "source of truth" notes/docs repository live?

13. **Update / OTA story.** How do we ship OS/agent updates to Pocket and Commander safely? (Read-only rootfs + A/B partitions, or just `apt upgrade` over WireGuard and hope?) This is a v1 question if we want to avoid bricking real devices.

14. **Power budget for Pocket.** Is weeks-of-standby via M0 deep sleep a hard requirement, or is "charge nightly" acceptable for v1? Hard requirement means we have to design the M0 firmware path now; nightly-charge lets us defer it.

15. **Licensing.** Any preference on OS license (GPL-3 vs Apache-2.0 vs MIT) and whether the agent configs / fine-tunes are open or proprietary? Affects the repo layout decision.

16. ~~**Display strategy: hybrid (e-ink + OLED strip) confirmed?**~~ — **RESOLVED 2026-06-17: hybrid confirmed.** Refined to **centered OLED island** (Dynamic Island pattern) in `ui/DEVICE-MOCKUP.md` — not full-width Touch Bar, because width-matching is a non-issue when the OLED is centered. See mockup §1 and §3 for rationale.

17. **Pocket industrial design priority.** When we do get to hardware, do you want a **cleaner, more polished** industrial design (slower, may require injection molding) or a **pragmatic 3D-print-first** design (faster, less polished, easier to iterate)? Affects enclosure timeline and BOM.

18. **OLED island size + interface.** With the centered-island recommendation, pick a target OLED size: **2.0"** (smallest, most common, lowest power), **2.4"** (recommended default — a bit more room for token streams / waveforms), or **3.2"+** (rare, expensive, color options exist). And: **mono** (default, cheap) vs **color** (status colors help, more $). The mockup in `ui/DEVICE-MOCKUP.md` was designed for 2.0–2.4" mono.

19. **OLED placement on Pocket.** Bottom (recommended — glanceable when device is held low / in pocket), top (phone-style status bar), or both ends. Affects the device's center of mass and how the user holds it.

20. ~~**Hero render.** Want me to use `image_generate` to produce one or two polished rendered mockups of the recommended layout (e-ink + centered OLED island) with realistic UI? The ASCII mockups in `ui/DEVICE-MOCKUP.md` are for structure; a render is for visual polish.~~ — **DONE 2026-06-17: 2 hero renders in `ui/heroes/`** (AI chat state, idle state).

21. ~~**OS base for Phase 1.**~~ — **RESOLVED 2026-06-17: mPWRD-OS** (Armbian Trixie + Meshtastic, Lyra Zero W supported, maintained by vidplace7). Our `smartlife-os` is an Armbian `userpatches` overlay on mPWRD-OS that adds Phosh + e-ink + OLED + SmartLife. Fallback: stock Armbian Trixie. Full analysis in `research/lyra-os-landscape.md`.

22. ~~**mpwrdOS / vid7 / OpenSourceCountry — sources?**~~ — **RESOLVED 2026-06-17:**
- **mPWRD-OS** is real, lives at `github.com/mPWRD-OS/mPWRD-OS` (Armbian + Meshtastic, Debian 13 Trixie, Lyra Zero W supported, alpha)
- **vidplace7** is the maintainer (`github.com/vidplace7`, Austin/Atlanta) — also `@armbian Board Maintainer` and `@meshtastic Lead: Linux Packaging/Distribution`
- **OSC** keeps repos private, dropped per Josh's note
- Worth watching: **Femtofox** (`github.com/femtofox/femtofox`) — Luckfox + custom PCB + LoRa, a community "Pocket" project

---

**Status as of 2026-06-17 (PIVOT + NAMED):**

- ✅ Working name: **SmartLife System** (umbrella, the three-device ecosystem) + **Shoji** (the e-ink DE, the Pocket's shell)
- ✅ Repo: `github.com/joshbowyer/shoji` (created, SSH-pushed, main branch live)
- ✅ First prototype: Pocket
- ✅ Display strategy: hybrid (e-ink + 2.4" centered OLED island)
- ✅ **UI shell: Shoji** — custom e-ink-first shell (replaces Phosh shell). Phoc + squeekboard + libadwaita + Wayland + PAM kept as infrastructure. Full design in `ui/EINK-DE-SHELL.md`.
- ✅ Phase 1 hardware: Lyra Zero W, Waveshare 4.2" e-ink SPI, Waveshare 2.4" SH1106 OLED I²C
- ✅ RMIO expansion connector spec'd
- ✅ Pin config priority: mainline > `configfs` > `luckfox-config` > SDK
- ✅ Dev workflow: laptop → Pinephone → Lyra
- ✅ OS base: mPWRD-OS (Armbian Trixie + Meshtastic)
- ✅ Lineage captured: Josh (Femtofox/Foxbuntu co-creator, initial Lyra kernel work, pin map) + vidplace7 (mPWRD-OS, Armbian Lyra maintainer) + wehooper4 (Meshtastic hardware)
- ⏳ ~~Q4 (time budget)~~ — **closed: no calendar, adhd-fueled side project**
- ⏳ ~~Q20 (hero render)~~ — **done: 2 hero renders in `ui/heroes/`**
- ⏳ ~~Q21 (OS base)~~ — **resolved: mPWRD-OS**
- ⏳ ~~Q22 (sources)~~ — **resolved: mPWRD-OS / vidplace7 / Femtofox**
- ⏳ ~~Q (Phosh vs custom shell)~~ — **RESOLVED 2026-06-17: custom e-ink-first shell (Option B)**
- ⏳ Still open: Q7 (Foxbuntu fork strategy), Q10 (AI personality angle), Q11 (demo audience), Q12 (sync boundary), Q13 (OTA), Q14 (power budget), Q15 (licensing), Q17 (industrial design), Q19 (OLED placement) — all **paused until e-ink DE works**
- 🆕 Active work:
  - **Tier 1 (next):** browser-based e-ink simulator (HTML/CSS/JS)
  - **Tier 2:** custom shell in libadwaita, tested on Pinephone
  - **Tier 3:** real e-ink on Pi + Waveshare 4.2" B/W HAT (~$50)
  - **Tier 4:** Lyra Zero W deployment
- 🆕 Action items (not my questions, just to-dos):
  - **Josh to ask vidplace7 (paused until Tier 1–3):** does mPWRD-OS expose dynamic device tree via `configfs` (raspi-config style) or do we need to fork the build for RMIO pin config?
  - **Spike 1 (paused until Tier 3):** flash mPWRD-OS to Lyra Zero W, dump `glxinfo`, confirm Mali + Panfrost.

**Most-recent deliverables:** `ui/EINK-DE-SHELL.md` (custom shell design, the active goal), `software/STACK-DECISION.md` (pivot to Option B), `TODO.md` (restructured: e-ink DE is the only active track). Everything else (AI, HA, mesh, security) paused.
