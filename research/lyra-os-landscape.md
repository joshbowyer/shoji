# Lyra Zero W — OS Landscape (June 2026)

> Survey of OS options for the Pocket's Phase 1 build. **Promoted to mPWRD-OS as the recommended base** (Armbian + Meshtastic, Debian 13 Trixie, maintained by vidplace7 who is also the Armbian board maintainer for Lyra Zero W and the Meshtastic Linux lead).
> Status: living doc. Re-check before any flash.

## TL;DR — what to use for Phase 1

**Recommendation: mPWRD-OS as the base for our `smartlife-os`.** It's:
- Armbian Trixie under the hood (upstream Debian, upstream Mesa, upstream Linux kernel — minimal Luckfox-specific code)
- Built with Armbian's `userpatches` framework (the right pattern for our own custom build)
- Explicitly supports **Luckfox Lyra Zero W** and **Lyra Ultra W** (alpha status, actively maintained)
- Pre-installs `meshtasticd` + the Meshtastic CLI + `contact` TUI (we want Meshtastic for the Pocket's mesh radio story anyway)
- Maintained by **vidplace7** — `@armbian Board Maintainer` and `@meshtastic Lead: Linux Packaging/Distribution`. We can ask them hard questions about Lyra.

**Our `smartlife-os` would be a `userpatches` overlay on mPWRD-OS** that adds:
- `phosh phoc squeekboard phosh-mobile-settings` (apt)
- The e-ink display profile daemon
- The OLED strip renderer
- The RMIO pin map + `/etc/luckfox.cfg` snippets
- Our home screen widgets, settings subpages, apps

This is a much cleaner path than building from scratch and avoids the Luckfox SDK where possible (which Josh has documented as painful).

**Fallback (if mPWRD-OS doesn't work out):** stock Armbian Trixie for Lyra Zero W + our own `smartlife-os` userpatches.

## The options, re-ranked

### 1. mPWRD-OS — RECOMMENDED ⭐

- **Source:** `github.com/mPWRD-OS/mPWRD-OS`
- **Tagline:** "Armbian + Meshtastic == mPWRD-OS"
- **Base:** Debian 13 Trixie (built with Armbian's `userpatches` framework)
- **Maintained by:** vidplace7 (Austin, Atlanta GA) — `@armbian Board Maintainer` and `@meshtastic Lead: Linux Packaging/Distribution`
- **Pre-installed:** `meshtasticd`, meshtastic CLI, `contact` TUI, `mpwrd-menu`, BLE WiFi provisioning, nymea-networkmanager
- **Lyra Zero W support:** ✅ explicit, alpha status
- **Build pattern:** `git clone armbian/build && cd build && git clone mPWRD-OS userpatches && ./compile.sh <board>`
- **Our path:** same pattern, with our own `userpatches` overlay (adds Phosh + e-ink + OLED + SmartLife)
- **Pros:** least Luckfox-specific code, Armbian-ecosystem alignment, the right build pattern for our `smartlife-os`, maintainer is also the Armbian Lyra board maintainer (so they can answer our hardware questions)
- **Cons:** alpha status for Lyra Zero W (we may be early), Meshtastic is pre-installed (we want this anyway, so neutral)

**Direct lines to community:**
- Repo: `github.com/mPWRD-OS/mPWRD-OS`
- Wiki: Board Support, Provisioning, Rockchip Flashing
- Maintainer: vidplace7 on GitHub, `@armbian` and `@meshtastic` orgs
- Companion repo: `mPWRD-OS/mpwrd-menu` (the TUI manager)

### 2. Stock Armbian Trixie / Resolute (fallback)

- **Source:** `armbian.com/boards/luckfox-lyra-zero-w`
- **Base:** Debian 12/13
- **Maintained by:** Armbian community
- **Phosh path:** `apt install phosh phoc squeekboard phosh-mobile-settings`
- **Pros:** cleanest Phosh story (same distro, same repos), large community, well-tested Linux stack
- **Cons:** Known boot bug (Nov 2025) — community image missing `misc` partition. Workaround exists or wait for fix.
- **Risk:** Mali GPU on RK3506B may need Mesa config tweaks for phoc. First-spike territory.

### 3. platima/SBC-Images Ubuntu 22.04

- **Source:** `github.com/platima/SBC-Images`
- **Base:** Ubuntu 22.04 (LTS)
- **Maintained by:** platima
- **Phosh path:** Phosh is in Ubuntu 22.04 repos but not all pieces; may need backports
- **Pros:** well-tested per their own labeling, has TESTING build with `luckfox-config` preinstalled
- **Cons:** Ubuntu 22.04 base = older packages, less ideal than mPWRD-OS

### 4. markbirss/rk3506-ubuntu

- **Source:** `github.com/markbirss/rk3506-ubuntu`
- **Base:** Ubuntu 24.04.x
- **Maintained by:** markbirss (one-maintainer project)
- **Pros:** Ubuntu 24.04 (newer), supports many RK3506 boards, has a WIP branch for ST7305 300×400 RCLD e-ink display on Lyra
- **Cons:** one-maintainer, less battle-tested than mPWRD-OS / Armbian
- **Why we care:** the e-ink work is directly relevant. Worth watching/borrowing from.

### 5. Official Luckfox Ubuntu 22.04 + Buildroot

- **Source:** `wiki.luckfox.com/Luckfox-Lyra-Zero-W/`
- **Base:** Ubuntu 22.04 or Buildroot
- **Maintained by:** Luckfox (the manufacturer)
- **Phosh path:** add Phosh yourself
- **Pros:** most "official"
- **Cons:** uses Luckfox SDK (which Josh has documented as painful), focused on stock Luckfox use case

## Mali GPU story (the Spike 1 unknown)

- **PinePhone:** Mali-400 MP2 (postmarketOS wiki confirms)
- **Panfrost** is the open-source Mali driver in **mainline Mesa** — supports Mali 400, T600, G310, G610
- **postmarketOS + Phosh on PinePhone** = proven Mali + Phosh path via Panfrost
- **The Lyra Zero W's RK3506B GPU is not clearly documented in public sources.** Most likely variant is **Mali-G31** (entry-level Mali, Panfrost-supported). If so, we're fine. If it's something exotic, that's the unknown.
- **Spike 1 (when we get to it):** flash mPWRD-OS, run `glxinfo | grep OpenGL` and `glmark2`, see what GPU it actually reports, see if Phoc's Wayland session can render.

## Community resources to follow

- **vidplace7** — mPWRD-OS maintainer, Armbian Lyra board maintainer, Meshtastic Linux lead. Direct contact.
- **mPWRD-OS org** — `github.com/mPWRD-OS`
- **markbirss/MeshCalc** — `github.com/markbirss/MeshCalc` — Meshtastic on Clockwork PicoCalc, powered by Luckfox Lyra running Ubuntu. Direct precedent for the form factor.
- **wehooper4** — `github.com/wehooper4/Meshtastic-Hardware` — designs the Lyra Ultra Hat and other Meshtastic hardware. Has Lyra knowledge, contributes to mPWRD-OS ecosystem.

## Lineage — where this all came from

This project is grounded in a small set of people who actually know the Lyra:

- **Josh (you)** — co-creator of **Femtofox / Foxbuntu** (the Luckfox Pico Mini Linux image, the ancestor to Lyra-family Linux work); did the **initial kernel work for the Lyra family**; has the Lyra pin map in your notes
- **vidplace7** — built on Josh's initial Lyra kernel work, became the **Armbian board maintainer for Lyra**, founded **mPWRD-OS**
- **wehooper4** — designs Meshtastic hardware including the Lyra Ultra Hat, contributes to the mPWRD-OS ecosystem, has Lyra knowledge

**Femtofox is the ancestor project** (pre-Lyra, Pico Mini era). Per Josh: "pretty old, the Pico Mini is very limited." We are NOT watching Femtofox as a current reference; we are building on the **Lyra family** that came after it, using mPWRD-OS as the modern base. Femtofox is valuable as historical context (it proves the "small Linux device with custom PCB + mesh radio" pattern works) but the hardware is too constrained for our needs.

**The Lyra knowledge is concentrated between Josh and vidplace7, with wehooper4 as a third.** When we need hardware answers, the path is:
- **Pin map, kernel work, deep hardware** → Josh (you have the notes)
- **mPWRD-OS, Armbian build, Meshtastic packaging, dynamic DT exposure** → vidplace7
- **Custom hardware designs (HATs, modules)** → wehooper4 has direct precedent

## The "minimize Luckfox pain" principle

Josh's experience reverse-engineering the Luckfox Pico + Lyra families:
- Luckfox SDK is "bad"
- `luckfox-config` is "clunky and picky"
- Had to reverse engineer most of it

**Implication for our work:**
- Use Armbian (upstream Linux kernel, upstream Mesa, upstream Debian packages) wherever possible
- Use mPWRD-OS as our base (it already does this)
- Treat Luckfox-specific code (SDK, `luckfox-config`) as **escape hatches**, not first-choice tools
- For e-ink + OLED, prefer **mainline Linux drivers** (Waveshare SPI e-ink and SH1106/SSD1309 I²C OLED are well-supported)
- For RMIO pin config, use **dynamic device tree overlays via `configfs`** first; `luckfox-config` only when necessary
- For the SDK kernel rebuild escape hatch: only when the above don't work

## The Phosh install path (on mPWRD-OS)

```bash
# After flashing mPWRD-OS to Lyra Zero W
sudo apt update
sudo apt install phosh phoc squeekboard phosh-mobile-settings gnome-control-center

# Disable any login manager (if present)
sudo systemctl disable gdm3 lightdm

# Configure phoc to use the e-ink as primary, OLED as secondary
# (phoc config goes in /etc/phoc.ini or similar)

# Set up auto-launch: getty on tty1 → user login → phoc + phosh
# (custom systemd service)
```

## First spike (when we get to it)

Per Josh, **the first spike is minimal: just install mPWRD-OS on the Lyra Zero W, dump `glxinfo`, see what GPU it actually reports. No screen, no display wiring yet — just confirm the Mali story.**

1. Flash mPWRD-OS latest release to Lyra Zero W
2. Boot, SSH in
3. `apt install glxinfo mesa-utils`
4. `glxinfo | grep OpenGL` — **confirm Mali variant and Panfrost support** ← the spike
5. (Optional second pass) `glmark2` to confirm a real Wayland rendering path works

**Why this order matters:** if Mali/Panfrost doesn't work, no amount of e-ink panel bring-up will help. So we get the GPU story right first, then layer displays on top.

**Pre-spike question for vidplace7** (Josh to ask): does mPWRD-OS expose dynamic device tree via `configfs` (like raspi-config) so we can do RMIO pin config without image/kernel rebuild, or do we need to fork the build for that? This determines whether our `smartlife-os` `userpatches` overlay is enough or if we need a deeper fork.

## What this doc locks in (when confirmed)

- OS base: **mPWRD-OS** (with fallback to stock Armbian Trixie)
- Build pattern: Armbian `userpatches` overlay
- Maintainer contact: vidplace7
- Pin map: Josh has the notes, will reference when needed
- Mali GPU on Lyra Zero W is **Spike 1** (glxinfo, no screen)
- Dynamic DT exposure: ask vid
- "Minimize Luckfox pain" is a core principle

## See also

- `research/lyra-platform-tooling.md` — pin config + device tree (with the "minimize Luckfox pain" framing)
- `hardware/RMIO.md` — expansion connector + pin map
- `software/DEV-WORKFLOW.md` — dev loop
- `software/STACK-DECISION.md` — Phosh + phoc + squeekboard as the UI stack
