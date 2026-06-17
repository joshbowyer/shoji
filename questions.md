# Shoji — Open Questions

> Decisions we need to make before Shoji is feature-complete. Most other SmartLife System questions (Home Base, Commander, OS image) are tracked outside this repo.

## Display

- **OLED placement on Pocket.** Bottom (recommended — glanceable when device is held low) vs top (phone-style status bar) vs both ends. Affects device center of mass and how the user holds it.
- **Industrial design direction** (when we get to hardware). 3D-print-first (faster, less polished, easier to iterate) vs injection-molding-first (slower, more polished, longer lead time)?

## Software

- **License.** GPL-3 vs Apache-2.0 vs MIT for the Shoji code, and whether the design tokens / config are open or proprietary.
- **Foxbuntu / mPWRD-OS fork strategy** (when relevant). True upstream fork vs downstream layer vs separate `shoji-os` layer.

## Architecture (for when we move to Pinephone, Tier 2)

- **PAM auth wiring** — which PAM modules for the lockscreen? `pam_unix` for password, optional `pam_fprintd` for fingerprint later?
- **squeekboard integration** — is the stock Phosh keyboard sufficient, or do we want a custom keymap for the Pocket form factor?
- **App packaging** — single `.deb` per app, or Flatpak, or a single `shoji-os` image that includes everything?

## Open architecture questions for when we move to Lyra (Tier 4)

- **Mali GPU on Lyra Zero W** — confirm Mali variant + Panfrost support. Spike 1: flash mPWRD-OS, `glxinfo | grep OpenGL`. (Not blocking Tier 1–3.)
- **Dynamic device tree in mPWRD-OS** — does it expose `configfs` overlays (raspi-config style) so we can do pin config without forking the build? (Ask vidplace7.)

## Action items (Josh, not me)

- **Ask vidplace7** (when we get to mPWRD-OS): does mPWRD-OS expose dynamic device tree via `configfs`? Or do we need to fork for the e-ink + OLED pin config?
- **Spike 1** (paused until Tier 3): flash mPWRD-OS to Lyra Zero W, dump `glxinfo`, confirm Mali + Panfrost.

## Resolved (2026-06-17)

- ✅ Project name: **Shoji** (Japanese paper sliding screens)
- ✅ Display strategy: e-ink main + 2.4" centered OLED island (Dynamic Island pattern)
- ✅ UI stack: custom e-ink-first shell on Phoc + squeekboard + libadwaita
- ✅ Testing tier 1: browser-based e-ink simulator
- ✅ Phosh shell rejected — animation vocabulary is wrong for e-ink
- ✅ No display manager — boot directly to custom shell via systemd (single-user)
- ✅ Testing order: laptop (browser sim) → laptop (libadwaita) → Pinephone → Pi + Waveshare HAT → Lyra
- ✅ First prototype device: Pocket (the e-ink one)
- ✅ "Pocket home" replaces app drawer (grid of widgets, not a phone home screen)
