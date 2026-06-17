# Shoji — TODO

> Conventions: `[ ]` pending, `[x]` done, `[?]` needs decision, `[~]` in progress.
> Scope: only the Shoji e-ink DE. Everything else (SmartLife System components beyond Shoji) is tracked elsewhere.

## 0. Decisions status (2026-06-17)

- [x] Project name: **Shoji**
- [x] Repo: `github.com/joshbowyer/shoji` (Shoji-only, e-ink DE scope)
- [x] Design language: SmartLife HIG (`ui/DESIGN-SYSTEM.md`)
- [x] Display strategy: e-ink main + 2.4" centered OLED island
- [x] UI stack: custom shell on Phoc + squeekboard + libadwaita
- [x] Tier 1: browser-based e-ink simulator

## 1. Custom Shoji shell (the only active goal)

### Tier 1: Browser-based e-ink simulator ✅ DONE
- [x] HTML/CSS/JS, single file
- [x] E-ink surface with refresh modes (full / partial / spot)
- [x] Centered OLED island with live content
- [x] 9 shell states (lockscreen, home, app grid, AI chat × 4, voice, settings)
- [x] Design tokens from `ui/DESIGN-SYSTEM.md`
- [x] Auto-play for the full tour

### Tier 2: Custom shell in libadwaita — NEXT
- [ ] Hello-world libadwaita app (single window, Adw.NavigationView)
- [ ] Design tokens as CSS in a stylesheet
- [ ] "Fake e-ink surface" widget (Gtk.Box with paper-feel CSS, refresh animations)
- [ ] "Fake OLED island" widget (centered below e-ink)
- [ ] Build: `meson setup build && meson compile -C build`
- [ ] Runs on GNOME3 (laptop) — no hardware needed
- [ ] Shell skeleton: lockscreen, home, app grid, settings
- [ ] PAM auth wiring
- [ ] squeekboard integration (laptop: manual, Pinephone: real)

### Tier 3: Real e-ink on Pi + Waveshare HAT
- [ ] Buy: Pi Zero 2 W + Waveshare 4.2" B/W e-ink HAT (~$50 total)
- [ ] Flash Pi OS / Armbian
- [ ] Bring up e-ink panel (kernel driver, SPI, partial refresh)
- [ ] Port shell to e-ink + OLED layout
- [ ] Display profile daemon (full/partial/spot refresh, OLED on/off, low-power state machine)
- [ ] Test ghosting mitigation
- [ ] Test all shell states on real e-ink

### Tier 4: Lyra Zero W (final target)
- [ ] Flash mPWRD-OS to Lyra Zero W
- [ ] Wire e-ink (SPI) and OLED (I²C) to Lyra
- [ ] Port shell + display profile to Lyra
- [ ] M0 deep-sleep wake sources
- [ ] Battery management

## 2. Stretch (post-shell)

- [ ] Solar trickle-charge reference design
- [ ] Optional camera + vision model
- [ ] Premium fine-tunes / "personalities" marketplace shape
- [ ] Rugged / weather-resistant Pocket enclosure variant
- [ ] Standalone Commander vs "big Pocket" decision (when we get to Commander)
