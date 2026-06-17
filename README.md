# Shoji

> The custom e-ink-first desktop environment for the SmartLife System Pocket Companion. A paper-thin shell designed for contemplative computing on dual-display (e-ink main canvas + 2.4" centered OLED island).
>
> Repo: `github.com/joshbowyer/shoji`

## What is Shoji?

Shoji (障子) is a privacy-first, fully local e-ink desktop environment. It replaces Phosh's mobile shell because Phosh's animation vocabulary is wrong for e-ink — no smooth scrolling, no slide animations, no fade transitions. Just a paper surface that gently refreshes when needed.

Live content — AI token streams, voice waveforms, typing echo, system status — lives on a small centered OLED island below the e-ink. The Dynamic Island pattern.

Shoji is one component of the **SmartLife System** (Pocket + Home Base + Commander). The Pocket is the e-ink device Shoji runs on. Home Base, Commander, the OS image, and other components live in their own repos or are tracked separately.

## Quick start

Open the browser-based e-ink simulator (no build, no deps):

```bash
xdg-open ui/simulator.html
```

For building the actual shell in C + libadwaita on your laptop:

```bash
# System packages (Debian/Ubuntu)
sudo apt install -y build-essential meson ninja-build pkg-config \
  libgtk-4-dev libadwaita-1-dev libglib2.0-dev libsoup-3.0-dev \
  libgtk4-layer-shell-dev desktop-file-utils gettext blueprint-compiler

# Build (from the repo root)
meson setup build
meson compile -C build
./build/shoji
```

Full setup details in [`software/DEV-SETUP.md`](./software/DEV-SETUP.md).

## Status

v0 design + hello-world app. The browser-based e-ink simulator is built (`ui/simulator.html`). The hello-world libadwaita app is the Tier 2.0 deliverable: a single Adw window with the fake e-ink surface + OLED island, design tokens applied. Build with `meson setup build && meson compile -C build && ./build/shoji`. Everything else is paused until a working shell exists.

See [`ui/SHELL.md`](./ui/SHELL.md) for the full shell design.

## Documentation

| File | What it covers |
|---|---|
| [`ui/SHELL.md`](./ui/SHELL.md) | The custom e-ink-first shell design (the heart of the project) |
| [`ui/DESIGN-SYSTEM.md`](./ui/DESIGN-SYSTEM.md) | Design tokens (SmartLife HIG) |
| [`ui/DISPLAY-STRATEGY.md`](./ui/DISPLAY-STRATEGY.md) | E-ink + OLED split, refresh modes, the Dynamic Island pattern |
| [`ui/DEVICE-MOCKUP.md`](./ui/DEVICE-MOCKUP.md) | 9 UI states for the device |
| [`ui/simulator.html`](./ui/simulator.html) | Browser-based e-ink simulator |
| [`ui/heroes/`](./ui/heroes/) | Rendered device concept images (AI chat + idle states) |
| [`software/STACK.md`](./software/STACK.md) | Phoc + squeekboard + libadwaita stack decision |
| [`software/DEV-SETUP.md`](./software/DEV-SETUP.md) | C + Meson setup for building Shoji |
| [`software/DEV-WORKFLOW.md`](./software/DEV-WORKFLOW.md) | Laptop → Pinephone → Pi → Lyra dev loop |
| [`TODO.md`](./TODO.md) | What's next |
| [`questions.md`](./questions.md) | Open decisions |

## The name

**Shoji** (障子) are paper sliding screens in traditional Japanese architecture. Translucent, paper-thin, calm, used to softly separate spaces. E-ink is the same idea: a paper surface that gently filters light and information. The metaphor is precise — a paper screen for contemplative computing.

Nautilus was considered first but rejected (GNOME file manager collision). The "paper, not shell" direction led to Shoji.

## Testing tiers

| Tier | What it tests | Cost | Status |
|---|---|---|---|
| 1. Browser-based e-ink simulator | Design language, shell UX, dual-display layout | $0 | **Done** (`ui/simulator.html`) |
| 2. Pinephone | Custom shell on real Phosh infrastructure (Phoc + squeekboard) | $0 (you have 3) | Next |
| 3. Pi + Waveshare 4.2" B/W e-ink HAT | Real e-ink refresh profile, ghosting mitigation | ~$50 | When design is solid |
| 4. Lyra Zero W + e-ink + OLED | Final Pocket target | $0 (you have it) | Last step |

## License

TBD — see `questions.md`.

## See also

- SmartLife System (broader ecosystem): separate repos / tracking
- mPWRD-OS (the base OS for the Lyra): `github.com/mPWRD-OS/mPWRD-OS`
- Phosh (the mobile shell we're replacing): `gitlab.gnome.org/World/Phosh/phosh`
