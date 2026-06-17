# SmartLife System — Design System

> The SmartLife HIG. GNOME-inspired, e-ink-aware, power-aware, dual-display, mesh-aware, button-aware.
> **Status: v0 draft (2026-06-17).** Josh to review, push back, extend.

## 1. Foundations

### 1.1 Inspiration: GNOME, adapted

GNOME is the right reference for our aesthetic — content-first, hierarchy over decoration, no gratuitous motion, predictable, calm, keyboard-first. We adopt all of that. We add five SmartLife-specific extensions:

| GNOME assumes | SmartLife adds |
|---|---|
| 60Hz refresh | **Refresh-aware** components (full / partial / spot modes are first-class) |
| Battery doesn't matter | **Power-aware** state (charging / battery / sleep; OLED off by default) |
| One display | **Dual-display** composition (e-ink canvas + OLED strip) |
| Always online | **Connectivity-aware** state (online / local / mesh-only / offline) |
| Touch / mouse / keyboard | Touch + **physical buttons** + voice, all three working everywhere |

### 1.2 Design principles (the rules that don't change)

1. **Content first, chrome fades.** E-ink amplifies this: a clean black-on-white text surface is the default. No decorative borders, no shadows, no gradients.
2. **Hierarchy over decoration.** If something is important, it's bigger, bolder, or higher contrast — never fancier.
3. **No gratuitous motion.** Motion is functional, never decorative. If a transition is needed, it has a purpose (orientation, causality, status). Otherwise, nothing moves.
4. **Predictable.** The same gesture / button / voice command does the same thing everywhere. Surprises are bugs.
5. **Calm.** No neon, no urgency theater, no exclamation marks. The aesthetic is paper, not glass.
6. **One-handed.** The Pocket is held in one hand, often. Primary actions are in the thumb zone. Hardware buttons cover the most-used actions so you don't have to look.
7. **Read before write.** Most states of the system are read-mostly. The UI should be optimized for the act of glancing, not the act of doing.

### 1.3 Adaptive philosophy

Three layers, in order:

- **Responsive** — same component, reflows at different sizes. A card on 1.5" OLED is a single line; on 4" e-ink is a 4-line block; on 10" Commander is a 12-line block.
- **Adaptive** — the system detects capabilities and serves the right variant. An "AI response" widget on e-ink shows a "thinking…" indicator then a final reply; on the OLED strip, it streams tokens in real time.
- **Progressive** — starts simple, adds capability as available. A note-taking app is one screen in mesh-only mode; gains sync indicators, AI suggestions, and voice dictation as features come online.

## 2. Design tokens

Tokens are the only way the system scales. Every value in the design system is a token; components reference tokens, never raw values.

### 2.1 Color (v0)

```
--color-bg              paper white (#F8F8F4 on light, #0E0E0C on dark)
--color-surface         same as bg or 1 step lighter/darker
--color-ink             near-black (#1A1A1A on light, #E8E8E4 on dark)
--color-ink-muted       50% gray
--color-ink-faint       25% gray
--color-accent          single accent (default: cool blue #2A4D7A) — used sparingly
--color-positive        green (low-saturation)
--color-warning         amber
--color-negative        red
--color-link            slightly more saturated than accent
--color-divider         ink-faint
```

**Rules**: at most one accent per screen. Status colors used only for status. No gradients. No shadows.

**Dark mode is not a theme, it's the default for OLED-friendly reading at night.** Both modes are first-class.

### 2.2 Typography (v0)

E-ink loves high-contrast, large, sans-serif type. Avoid thin weights (ghosting kills them). Avoid italics (poorly rendered on most e-ink panels).

```
--type-display          32–48px, weight 700
--type-h1               24–28px, weight 700
--type-h2               20–22px, weight 600
--type-h3               18px, weight 600
--type-body             16px, weight 400, line-height 1.5
--type-body-large       18px, weight 400 (notes, long-form)
--type-caption          14px, weight 400, line-height 1.4
--type-mono             14–16px, weight 400 (code, IDs, logs)
```

**Rule of thumb**: body is 16px minimum. E-ink renders small text poorly and ghosting makes it worse. When in doubt, bigger.

### 2.3 Spacing (v0)

A 4px base, doubled for hierarchy. Touch targets at least 44px (Apple HIG-ish, applies even when touch isn't the only input).

```
--space-1               4px
--space-2               8px
--space-3               12px
--space-4               16px
--space-6               24px
--space-8               32px
--space-12              48px
--space-16              64px
```

### 2.4 Radii (v0)

E-ink sharpens poorly. Conservative radii only.

```
--radius-none           0
--radius-sm             2px (icons, badges)
--radius-md             4px (cards, inputs)
--radius-lg             8px (sheets, modals — used sparingly)
--radius-pill           999px (chips, status pills)
```

### 2.5 Motion (v0)

Motion is rare and deliberate. Three motion tokens cover almost everything:

```
--motion-none           0ms    default; no transition
--motion-quick          120ms  used only on OLED strip (high-refresh surface)
--motion-wipe           250ms  partial-refresh transition on e-ink
--motion-full           1000ms full-refresh boundary; the "page turn"
```

**Default is `--motion-none`. Motion must be justified in the design.** Reduce-motion mode disables everything except full-refresh boundaries (which are functional, not motion).

### 2.6 Iconography (v0)

- 1.5–2px stroke, geometric, no fill
- 24x24 default, 16x16 for inline, 32x32 for empty states
- No decorative icons in chrome; icons appear next to labels or not at all
- A small set of "system" icons: settings, search, voice, sync, mesh, battery, signal

## 3. Layout primitives

Layout primitives are the only things a screen is built from. They handle the adaptive sizing transparently.

| Primitive | Purpose | Behavior on small / large |
|---|---|---|
| **Container** | Bounded region with consistent padding | Stacks vertically when narrow |
| **Stack** | Vertical or horizontal flow with consistent gap | Horizontal stacks collapse to vertical under 4" |
| **Grid** | 1–4 columns of equal cards | 1 col under 4", 2 col 4–7", 3–4 col 7"+ |
| **Sheet** | Bottom-anchored overlay, half- or full-height | Becomes a full-screen modal under 4" |
| **Pane** | Two-pane master/detail | Stacks vertically under 7" |
| **NavBar** | Bottom navigation with 3–5 destinations | Adapts label visibility to available width |

Every primitive is **a slot, not a frame**: it accepts any content but provides consistent rhythm.

## 4. Components (v0 — names + intent, not pixels)

| Component | Purpose | E-ink variant | OLED variant |
|---|---|---|---|
| **Card** | Discrete block of content (note, entity, message) | Static; full refresh on change | (n/a — lives on e-ink) |
| **List** | Vertical collection of items | Partial refresh for new items; spot for status | (n/a) |
| **Button** | Tap target | Full refresh on press (1 flash), partial after | Hardware button = same action |
| **Input** | Single or multi-line text | Spot refresh on each character commit | (uses OLED for keystroke echo) |
| **StatusIndicator** | Battery, signal, mesh state | Spot refresh; idle when stable | Live tickers (network, mesh) |
| **AIResponse** | Model output (text + tokens) | Full refresh when complete; partial for "thinking…" | Streams tokens live |
| **VoiceIndicator** | Mic state, waveform, transcript | Idle = small static icon | Live waveform + transcript |
| **Sheet / Modal** | Task-focused overlay | Full refresh on open/close | n/a |
| **NavBar** | Bottom nav, 3–5 destinations | Partial refresh on selection | n/a |
| **Toast** | Transient confirmation | Full refresh with timeout | Slides in from bottom on OLED |
| **Keyboard** | Virtual keyboard | Per-key spot refresh | Keystroke echo on OLED |
| **EmptyState** | First-run / no-data | Full refresh; calm copy | n/a |

**The component library has two surface types — e-ink and OLED — and the same component can be both.** `AIResponse`, `VoiceIndicator`, `Keyboard`, and `StatusIndicator` are the most important to get right because they straddle the two surfaces.

## 5. Display routing (the heart of the system)

Most components live on **one** surface. A few are **paired** — they exist on both surfaces, with each surface doing what it's good at.

### Paired components (e-ink + OLED)

- **AI chat**: e-ink = committed messages; OLED strip = live token stream + "thinking…" indicator
- **Voice session**: e-ink = transcript as it stabilizes; OLED strip = waveform + listening state
- **Keyboard input**: e-ink = committed text; OLED strip = per-keystroke echo
- **System status**: e-ink = idle when stable; OLED strip = live tickers when something is happening

### E-ink only (default)

Everything else: cards, lists, settings, calendars, books, forms, modal dialogs, full content views.

### OLED only (rare)

Boot / shutdown progress, transient notifications that don't deserve an e-ink flash, real-time mesh packet counters when actively monitoring.

### Refreshing rules (concrete)

- **Full refresh** on: app launch, mode change, modal open, AI reply complete, settings change, screen unlock. **At most one full refresh per screen** — design the screen so subsequent updates can be partial.
- **Partial refresh** on: in-app updates (new list item, clock minute tick, weather refresh, entity state change). Bounded to the smallest region possible.
- **Spot refresh** on: typing echo, status badge, notification count, single-icon change.
- **OLED wake** on: AI generation start, voice session start, typing start, live data update above threshold, notification, system event. **Asleep by default.**

## 6. Input model

Three input modalities, all working everywhere, none blocking the others.

### 6.1 Touch

- Primary input for navigation, reading, soft actions
- Tap = activate; long-press = secondary; swipe = navigate (forward/back/up)
- Touch targets ≥ 44px

### 6.2 Physical buttons

Default Pocket button map (proposed; configurable later):

- **D-pad up / down / left / right**: navigate
- **Select (center)**: activate
- **Back**: navigate back
- **Home**: jump to home screen
- **Voice (side button, hold to talk)**: push-to-talk, the most reliable voice activation
- **Power**: wake / sleep / long-press = power menu

Buttons and touch are **siblings**, not alternatives. The same action should be reachable by both. Voice is the third sibling.

### 6.3 Voice

- Push-to-talk is the default (privacy + reliability on e-ink — no false wakeups, no listening indicator needed unless active)
- "Listening…" state is on the OLED strip (no need to wake the e-ink)
- Transcript is committed to e-ink in chunks, not streamed per-word (which would flash the e-ink)
- Wake-word support is **off by default**. When enabled, the OLED strip shows "wake word armed" as a tiny indicator.

## 7. State vocabulary

A small, fixed set of states the system can be in. Components react to these; the design system enforces them.

### 7.1 Power states

- **Charging** (mains connected, drawing)
- **Battery** (on battery, normal operation)
- **Low** (< 20%, dimmed OLED, reduced refresh)
- **Critical** (< 5%, e-ink only, OLED off, no AI offload)
- **Sleep** (M0 co-processor alive; full displays off; wake on button / alarm / mesh event)

### 7.2 Connectivity states

- **Online** (WiFi or wired, internet + Home Base reachable)
- **Local** (WiFi up, Home Base reachable, internet down)
- **Mesh-only** (no IP, mesh radio up; degraded AI, no Home Base)
- **Offline** (no comms; local-only AI, local-only data)

### 7.3 AI states

- **Idle** (no active generation)
- **Thinking** (generation in progress; shows on OLED strip)
- **Streaming** (tokens arriving; OLED strip is the surface)
- **Complete** (final reply committed to e-ink via full or partial refresh)
- **Error** (calm error state; no retry theater)

## 8. Accessibility

E-ink has unique accessibility considerations. Default-on, opt-out.

- **High contrast mode**: pure black on pure white (or pure white on pure black in dark mode)
- **Large text mode**: scale type tokens 1.25x or 1.5x
- **Reduce motion**: disables everything except full-refresh boundaries
- **Screen reader**: linear content order; live regions for AI streaming and voice

## 8.1 Theme policy

**Shoji is always light.** The e-ink paper feel is the brand, not a mode. The e-ink device is physically light (paper), the OLED is physically dark. The window chrome, header bars, and surrounding UI must match the design regardless of the user's system theme.

This is implemented via `adw_application_set_color_scheme(app, ADW_COLOR_SCHEME_FORCE_LIGHT)` in `src/main.c`. Per-app, doesn't affect the user's other apps.

**To follow the system color scheme instead:** comment out the line in `main.c`. The design tokens will then need a dark mode variant, which is a future design problem.
- **No-color-only signals**: status is conveyed by text + icon, not color alone
- **Focus order is explicit**, not "tab through the DOM"

## 9. Microcopy / voice style

The AI replies in a calm, brief, paper-like voice. This is part of the design system, not an afterthought.

- No emojis in default replies
- No exclamation marks
- Short sentences. Lists when appropriate.
- Errors say what happened and what to do, never apologize
- "Thinking…" is the only "spinner" copy
- Local AI is a tool, not a personality. Pocket is not a chatbot pretending to be a friend — it's a private, calm, capable butler

## 10. Concrete example: AI chat on Pocket

```
┌──────────────────────────────────────────┐ ← e-ink (4–6" main)
│                                          │
│  > summarize the meeting notes           │ ← user prompt (committed)
│                                          │
│  ─────────────────────────────────────   │
│                                          │
│  Meeting covered the v1 BOM,             │ ← AI reply (committed)
│  enclosure timeline, and panel           │   partial refresh as
│  sourcing. Action items:                 │   tokens complete
│  - confirm panel SKU                     │
│  - sketch enclosure language             │
│  - ping friend about print bed size      │
│                                          │
│  42s · local · no offload                │ ← meta line (caption)
│                                          │
│  [ Ask follow-up ]                       │ ← button (full refresh)
│                                          │
├──────────────────────────────────────────┤
│ ● ● ● ● 1m 58% battery  12:42 ☀ 22°     │ ← OLED strip (live, off
└──────────────────────────────────────────┘   when idle)
```

**During streaming**, the OLED strip is the only thing updating:

```
┌──────────────────────────────────────────┐
│  > summarize the meeting notes           │
│                                          │
│  thinking…                               │ ← e-ink "thinking…" label
│                                          │
│  (e-ink not flashing)                    │
│                                          │
├──────────────────────────────────────────┤
│ ∿∿∿∿∿∿∿∿∿▁▂▃▂▁▂▃▂▁∿∿∿▁▂∿  tokens: 87   │ ← OLED: waveform + token
└──────────────────────────────────────────┘   counter, real-time
```

When streaming completes, the e-ink does a single full refresh to show the committed reply, and the OLED strip returns to idle status.

## 11. Next artifacts to produce

In order, before writing any code:

1. **Full widget library** — every component on the list above, with both e-ink and OLED variants where applicable, with concrete refresh behavior
2. **First screen mockups** (ASCII or Figma):
   - Home (clock, weather, HA glance, AI prompt bar)
   - AI chat (paired e-ink + OLED)
   - Notes (read + edit)
   - HA glance (entity list)
   - Settings (sections, toggles)
   - Power / connectivity / AI state surfaces
3. **Theme tokens as concrete values** — the v0 in §2 is right but not final
4. **Input map** — concrete button bindings for the proposed Pocket button map
5. **Refresh policy** — the rules in §5 formalized into a reference table
6. **Microcopy pass** — voice style applied to a representative set of system messages

Once these are in good shape, the UI framework pick (Flutter embedded / web stack / TUI / Qt) becomes a real decision rather than a guess.
