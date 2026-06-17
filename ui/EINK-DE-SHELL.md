# SmartLife e-ink Desktop Environment — Shell Design

> The custom e-ink-first shell that replaces Phosh's mobile shell. Built on Phoc + libadwaita + squeekboard; provides the lockscreen, home, app launcher, notifications, and settings that Phosh's shell would have provided, but designed from day one for an e-ink + OLED island display.
> **Status: v0 design (2026-06-17).** Architectural pivot — Phosh's shell is the wrong fit for e-ink. This is the new Sprint 0.
> **Goal: a working e-ink DE, period. Everything else (AI stack, HA integration, mesh, security) is paused until this lands.**

## Why we're building a custom shell

Phosh — the mobile shell — is designed for 60Hz smooth-scrolling LED surfaces. Its animation vocabulary (lockscreen fade, app drawer slide, notification arrival, modal transitions) is baked into the shell, libadwaita, and most Phosh apps. E-ink is the opposite of every one of those: ~250ms–3000ms per refresh, no smooth motion, ghosting on partial updates, designed for static content.

The Dynamic Island design (e-ink main canvas + OLED strip for live feedback) works for *app content*. It does not solve the *shell animation* problem. The shell is what the user interacts with constantly — lockscreen every wake, app drawer every launch, notifications constantly — and the e-ink can't do shell transitions gracefully.

**Option B (locked in 2026-06-17):** keep Phosh's *infrastructure* (compositor, libs, keyboard, PAM, lifecycle, Wayland); replace Phosh's *shell* with a custom e-ink-first one.

**Community angle:** no true general-purpose e-ink desktop environment exists. reMarkable/Boox/Kobo are proprietary and reading-focused. InkBox and Plato are open but also reading-focused. Pinephone/Mobian/postmarketOS users are the natural allies (privacy-first, mobile Linux, battery-life pain). A real e-ink DE is a genuine open niche.

## What we keep from Phosh infrastructure

| Component | What it does | Why we keep it |
|---|---|---|
| **Phoc** | Wayland compositor | Renders surfaces, handles input, manages multi-output (e-ink + OLED). Standard Wayland. |
| **squeekboard** | On-screen keyboard | Part of the Phosh ecosystem, works with Wayland. Could be replaced but no reason to. |
| **Wayland** | Display protocol | Standard Linux. Lets us render the e-ink shell and the OLED strip as separate outputs. |
| **PAM** | Auth | Pluggable auth modules. We use `pam_unix` for password, optional fingerprint later. |
| **seatd** / logind | Session management | Standard Linux. |
| **systemd** | Service supervisor | Standard Linux. |
| **libadwaita** | Widget toolkit | GNOME design system. Our design language maps 1:1. Phosh shell is *built on* libadwaita; we can use the widgets without using the Phosh shell. |
| **GTK4** | Toolkit foundation | Same. |

## What we replace from Phosh shell

| Phosh shell component | Custom replacement |
|---|---|
| **Lockscreen** (Phosh built-in) | **Custom lockscreen** — full-refresh on wake, no fade, no slide, no animated unlock. Just a calm image with a "press button to unlock" affordance. |
| **Home screen** (Phosh overview) | **Pocket home** — a grid of widgets (clock, weather, HA glance, AI prompt, recent notes), not an app drawer. Calm, content-first. |
| **App drawer** (Phosh overview) | **App grid** — a simple list or icon grid of installed apps. Tapping launches the app. No slide-in animation; just full-refresh to the app. |
| **Status bar** (Phosh top bar) | **OLED strip** — moved entirely to the centered OLED island below the e-ink. Battery, signal, mesh state, AI state. |
| **Notifications** (Phosh panel) | **Text-only notifications** — appear as a banner at the top of the e-ink, full-refresh, no slide. Tapping opens the relevant app. |
| **Settings app** (phosh-mobile-settings) | **Custom SmartLife settings** — a single libadwaita app with subpages for AI & sync, mesh, power, accessibility. We don't ship phosh-mobile-settings; we ship our own. |
| **Quick settings** (Phosh overlay) | **OLED strip interactions** — tap-and-hold the OLED strip for power / connectivity / AI mode toggles. |
| **Overview / multitasking** (Phosh) | **App switcher** — simple card list, no live previews (e-ink can't do them), full-refresh between apps. |

## Custom shell architecture

```
┌─────────────────────────────────────────────────┐
│              e-ink surface (4–6")               │ ← Phoc output 1
│                                                 │
│   [ Notification banner — when present ]       │ ← top, full-refresh
│                                                 │
│   [ App content (lockscreen / home / app) ]    │ ← main area
│                                                 │
│                                                 │
│                                                 │
├─────────────────────────────────────────────────┤
│       [ OLED island (2.4" centered) ]           │ ← Phoc output 2
└─────────────────────────────────────────────────┘
```

The shell is a single GTK4 / libadwaita application that:
- Owns the lockscreen
- Owns the home screen
- Owns the app launcher
- Owns the notification surface
- Owns the app switcher
- Writes to the OLED strip via a separate Wayland surface

It runs as a single Wayland client. Other apps (Notes, Chat, Home Assistant, etc.) run as separate Wayland clients. Phoc composites them.

## The Pocket home (the heart of the shell)

The home screen is **not an app drawer**. It's a grid of widgets. Like a smartwatch face, but bigger and more capable. The user lands here on unlock and can read the state of their life at a glance.

```
┌─────────────────────────────────────────────────┐
│                                                 │
│   Tue  Jun 17                                  │
│   10:56 AM                                     │
│                                                 │
│   San Antonio  88°F  ☀ partly cloudy           │
│                                                 │
│   ───────────────────────────────────          │
│                                                 │
│   Up next                                       │
│   • 2:00 PM   Sprint planning                  │
│   • 4:30 PM   Pickup groceries                 │
│                                                 │
│   ───────────────────────────────────          │
│                                                 │
│   Home: 3 lights on, 1 door open               │
│                                                 │
│   ───────────────────────────────────          │
│                                                 │
│   [ Ask the butler ▸ ]                         │ ← AI prompt bar
│                                                 │
│                                                 │
│   [ Notes ] [ Chat ] [ Home ] [ Mesh ] [ … ]  │ ← app row
│                                                 │
├─────────────────────────────────────────────────┤
│       ┌─────────────────────┐                  │
│       │  78% ▂▃▄▅█  12:56  │                  │ ← OLED: idle
│       └─────────────────────┘                  │
└─────────────────────────────────────────────────┘
```

**Components of the home:**
- **Clock + date** (large, top)
- **Weather** (current + brief forecast)
- **Calendar glance** (next 2–3 events)
- **Home Assistant glance** (key entity states)
- **AI prompt bar** (tap to chat with the butler)
- **App row** (icons or labels for installed apps)
- **OLED strip** (battery, signal, time, mesh status)

Every section is a discrete widget. Each widget can update independently (partial refresh on the e-ink for its region). The home is itself a full-refresh at unlock, then partial-refreshes for widget updates.

## The custom lockscreen

- Full e-ink refresh on wake
- Large time, date, day
- "Press select to unlock" hint at bottom
- Optional: notification count badge
- No animation, no fade, no wallpaper transition
- Unlock = button press (or touch) → PAM auth (password, or fingerprint later) → home

## Custom notifications

- Text-only, no rich previews
- Appear as a banner at the top of the e-ink (full-refresh of the top region)
- Tapping opens the relevant app
- Auto-dismiss after N seconds, or on user action
- The OLED strip can show "new notification" indicator (a glyph or a count) for at-a-glance awareness

## App launcher

- Not a drawer, a **grid** (icons + labels)
- Configurable rows of apps
- Tapping launches the app via Phoc's app-launching mechanism
- Search bar at top (optional, fits our design language)
- No app categorization, no folders in v1 (keep it simple)

## App switcher

- Card list of recently-used apps
- Tap to switch
- No live previews (e-ink can't render them)
- Full-refresh between cards
- Inspired by Phosh's overview but stripped of motion

## Settings

- Custom libadwaita app, single window
- Subpages via `Adw.NavigationView`:
  - **AI & sync**: which model, offload policy, RAG index, voice
  - **Mesh**: radio on/off, channel, region, mesh peer list
  - **Power**: battery thresholds, sleep timeout, M0 deep-sleep policy, OLED auto-off
  - **Connectivity**: WiFi, BT, IP status, Home Base discovery
  - **Display**: refresh mode policy, OLED strip on/off, dark/light theme, accessibility
  - **About**: device info, OS version, build, updater
- Phosh-mobile-settings is NOT shipped; we replace it entirely

## What this means for the design system

The design system in `ui/DESIGN-SYSTEM.md` still applies. The custom shell is a consumer of the design system, not a replacement. The tokens, components, layout primitives, refresh policy, input model, state vocabulary, and accessibility checklist all carry over.

The additions:
- **Shell-specific component patterns** (the home widget grid, the custom lockscreen, the custom notification banner) get added to the component library
- **App switcher pattern** is new (no equivalent in the original design system)
- **OLED-strip-only interactions** (quick settings) are a new pattern

## Implementation sketch (for later)

- Language: Python (PyGObject) or C (GTK direct). Most Phosh apps are GTK. Pick whichever is more comfortable.
- UI: GTK4 + libadwaita
- Build: Meson + Ninja
- The shell is a single libadwaita application that holds the home / lockscreen / app grid
- App launching: spawn subprocesses for each app
- OLED strip: a separate Wayland surface, written to from the shell when state changes
- Display profile daemon: a separate service that owns the e-ink refresh policy and OLED on/off (the shell is a *consumer* of the display profile, not the owner)

## Testing tiers (concrete)

| Tier | What it tests | Hardware | Status |
|---|---|---|---|
| **1. Browser-based e-ink simulator** | Design language, shell UX, dual-display layout, refresh-mode behavior | None — HTML/CSS/JS | **Next concrete artifact to build** |
| **2. Pinephone** | Custom shell on real Phosh infrastructure (Phoc + squeekboard) | Pinephone (3 in hand) | When shell is built |
| **3. Pi + Waveshare 4.2" B/W e-ink HAT** | Real e-ink refresh profile, ghosting mitigation, OLED strip | Pi Zero 2 W + HAT (~$50) | When design is solid |
| **4. Lyra Zero W + e-ink + OLED** | Final Pocket target | Lyra Zero W (in hand) | Last step |

**Tier 1 is the immediate next deliverable.** A web page that simulates the e-ink surface + OLED island + refresh modes. Lets you iterate the design language and shell UX every time you feel like it. Build it in a session, iterate forever.

## What this doc locks in (when confirmed)

- Phosh's shell is replaced with a custom e-ink-first shell
- Phoc, squeekboard, libadwaita, PAM, Wayland, systemd are kept as infrastructure
- Shell is a single libadwaita app holding lockscreen / home / app grid / app switcher / settings
- OLED strip is a separate Wayland surface, owned by the display profile daemon
- "Pocket home" replaces app drawer as the home paradigm
- Browser-based e-ink simulator is the next concrete deliverable
- Pi + Waveshare 4.2" B/W e-ink HAT is the real e-ink test target (~$50)
- Everything else (AI, HA, mesh, security) is paused until the DE works

## See also

- `ui/DESIGN-SYSTEM.md` — design language (still applies)
- `ui/DISPLAY-STRATEGY.md` — e-ink + OLED split (still applies)
- `ui/DEVICE-MOCKUP.md` — visual concept for the device (still applies)
- `software/STACK-DECISION.md` — updated to reflect Option B
- `questions-for-josh.md` — open decisions
