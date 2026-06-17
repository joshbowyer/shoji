# SmartLife System — Display Strategy

> Decides: what kind of display(s) go on each device, and what lives on each.
> Status: draft v0 (2026-06-17). Awaits Josh's confirmation.

## TL;DR

| Device | Main display | Secondary | Refresh policy |
|---|---|---|---|
| **Pocket** | **E-ink** (4–6" MIPI DSI) | **Small OLED strip** (1–2") | E-ink is the canvas. OLED strip is the exception, off by default. |
| **Commander** | **E-ink** (7–10"+ MIPI DSI) | None | Pure e-ink. Partial refresh, full refresh on event boundaries. |
| **Home Base** | None (headless server) | n/a | n/a |

**Recommendation: hybrid (e-ink + small OLED strip) on Pocket, pure e-ink on Commander.** Not pure LCD, not pure e-ink.

## Why hybrid on Pocket

E-ink is excellent for what it's excellent for, and the wrong tool for several things Pocket is supposed to do. The brainstorm already proposed "e-ink or hybrid e-ink/LCD" — this doc picks hybrid and explains why.

### Where e-ink wins (Pocket should keep e-ink for these)

- **Static, glanceable content**: calendar, weather, HA status, notes, books, dashboards
- **Sunlight readability**: best in class — and Pocket is the device you take outside
- **Battery**: zero power when not refreshing — this is the whole reason it can sit in standby for weeks
- **Paper-like feel, easy on eyes, calm aesthetic**: part of the brand's identity, part of why you'd reach for Pocket over a phone

### Where e-ink fights the concept (these need the OLED strip)

| Use case | Why e-ink hurts | OLED strip fixes it |
|---|---|---|
| **AI streaming replies** (the killer) | Partial refresh 250–500ms/token feels brutal; full refresh 1–2s breaks the illusion of conversation | Stream tokens on the OLED strip at 30–60Hz; e-ink shows the final/canonical reply |
| **Voice feedback** (waveform, real-time transcript, "listening…") | E-ink can't show motion cleanly | Waveform + state on OLED; transcript committed to e-ink as it stabilizes |
| **Typing feedback** (virtual keyboard) | E-ink ghosting on each keypress is awful | Per-keypress flash on OLED; committed text on e-ink |
| **Live data** (sensor streams, mesh packet counters, network status ticking) | E-ink partial refresh has visible lag/ghost at high update rates | Tickers on OLED; events committed to e-ink |
| **Camera viewfinder** (if we add a camera) | E-ink is not a viewfinder | OLED becomes the viewfinder |
| **Smooth transitions / motion** that GNOME's adaptive vocabulary assumes | E-ink partial refresh is a "wipe" or "fade" at best, never smooth motion | OLED handles the transient motion; e-ink shows the destination state |

### Why not color e-ink (Gallery 3 / Kaleido 3)?

- **Gallery 3**: ~500ms–1s refresh, dimmer than mono, expensive, low color saturation
- **Kaleido 3**: faster but color is essentially a color filter overlay, so saturation is poor
- **For accent status (red/green/blue, a single highlight color)** OLED does it better, cheaper, and draws zero power when off

### Why not pure LCD on Pocket?

- Pocket becomes "a smartphone with a worse battery and worse sunlight readability"
- We lose the e-ink *soul* of the brand
- The hybrid keeps the e-ink on the device the user holds all day

## Display routing rules

The design system must encode **what lives on which display**. Default policy:

### Lives on the e-ink (main canvas)

- App bodies: notes, settings, lists, calendars, dashboards
- Read-mode content: books, long AI replies (after streaming completes), HA entity state
- Static widgets: clock, weather, today's calendar, system status
- Forms and text input (the actual text, not the per-keystroke feedback)
- Modal dialogs, alerts (rendered, not animated)

### Lives on the OLED strip (live feedback)

- AI token stream during generation
- Voice waveform + listening / thinking / speaking state
- Per-keypress flash / typing echo
- Live sensor / mesh / network tickers
- Notification arrival + count badge
- Boot / shutdown progress bars
- "Thinking…" / "Working…" spinners
- Camera viewfinder (if camera added)

### Lives on neither / ambient

- M0 deep-sleep state — both displays off, system wakes on physical button, scheduled alarm, or mesh event

## Refresh policy on e-ink

Three refresh modes, used deliberately:

| Mode | Latency | Use for | Cost |
|---|---|---|---|
| **Full** | ~1–2s, no ghosting | Mode changes, app launch, modal open, AI reply complete, settings change | Battery heavy per refresh, but no power to hold |
| **Partial** | ~250–500ms, some ghosting | In-app updates: new list item, clock minute tick, weather refresh, entity state change | Cheaper per refresh; needs periodic full refresh to clear ghosting |
| **Spot / zone** | ~150–300ms, small area | Typing echo (one character), notification count, status badge | Cheapest; ghosting is contained to the spot |

**Rule of thumb**: at most one full refresh per screen. App body uses partial; spot updates use spot. OLED strip handles everything continuous.

## Refresh policy on OLED

- **Off by default.** Wakes on: AI generation start, voice session start, typing start, live data update above threshold, notification, system event
- **30–60Hz** when active
- **No animations on e-ink** that aren't backed by full-refresh boundaries

## Physical placement (for when we get to hardware)

The OLED strip is part of the industrial design. Two options worth mocking:

1. **Bottom strip**: 1.5–2" wide horizontal band below the e-ink, all the time. Always visible.
2. **Top strip**: thin status bar at the top. Frees up vertical e-ink space.

**Recommendation: bottom strip** (4"–6" main + 1.5" bottom strip is a comfortable grip; top strip competes with status icons). This is a design question for the industrial design phase — flag for Q17.

## Commander (pure e-ink) is unaffected

Commander is the always-glanceable device. Pure e-ink is exactly right: no live feedback needed, partial refresh is sufficient, full refresh once per scene change. No OLED strip.

## Open questions (gating hardware)

- Q16 (above): confirm hybrid vs pure e-ink vs pure LCD
- OLED strip interface: I²C SSD1306 / SH1106 (mono) vs SPI small color OLED (SSD1351 / SH1122). Mono is cheaper and lower power; color helps with status colors. Default: **mono** for v1.
- OLED strip size: 0.91", 1.3", 1.54", 2.0" candidate SKUs

## What this doc locks in (when confirmed)

- Pocket has two displays; Commander has one
- E-ink is the canvas on both, never the live feedback surface
- OLED strip is the exception, not the rule — used for ~5% of the UX
- Refresh policy: full / partial / spot on e-ink; off-by-default on OLED
- Color e-ink is not a dependency
- Industrial design language: bottom strip (pending Q17)
