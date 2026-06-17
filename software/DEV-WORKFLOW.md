# SmartLife System — Development Workflow

> How we build, test, and ship SmartLife software. The dev loop is designed so 90% of the work happens on the laptop with no Lyra-specific hardware, with periodic validation on Pinephone and Lyra.
> Status: v0 (2026-06-17). Lean workflow, no calendar.

## The 3-tier dev loop

```
   ┌─────────────────────────────────────────────────────┐
   │  TIER 1 — Laptop (Debian + GNOME3)                  │
   │  "Where you write the code"                         │
   │                                                     │
   │  • Build GTK / libadwaita apps in your usual env    │
   │  • Run them in GNOME3 — they work as regular apps  │
   │  • Iterate fast: save → reload → tweak              │
   │  • No Luckfox-specific code needed yet              │
   └──────────────────────┬──────────────────────────────┘
                          │  deploy .deb / .flatpak
                          ▼
   ┌─────────────────────────────────────────────────────┐
   │  TIER 2 — Pinephone (Mobian / Phosh)                │
   │  "Where you validate Phosh behavior"                │
   │                                                     │
   │  • Install on Pinephone (you have 3)                │
   │  • Test Phosh-specific widgets, multi-touch,        │
   │    mobile form factor, lockscreen, app drawer       │
   │  • Cross-test on 2–3 form factors (Pinephone        │
   │    variants) to validate adaptive design            │
   │  • The Pinephone is a real Phosh device — same      │
   │    stack we'll run on Lyra                          │
   └──────────────────────┬──────────────────────────────┘
                          │  ssh / scp the package
                          ▼
   ┌─────────────────────────────────────────────────────┐
   │  TIER 3 — Lyra Zero W (Phosh)                       │
   │  "Where you validate SmartLife-specific behavior"   │
   │                                                     │
   │  • e-ink refresh profile                            │
   │  • OLED strip renderer                              │
   │  • RMIO pin config via luckfox-config               │
   │  • Power / sleep / M0 wake behavior                 │
   │  • Lyra-specific drivers (if any)                   │
   │                                                     │
   │  • Only SmartLife-specific code is new here         │
   │  • Most of the app is portable GTK / libadwaita     │
   └─────────────────────────────────────────────────────┘
```

## What this means in practice

### Tier 1: Laptop (Debian + GNOME3) — daily work

You can start writing SmartLife apps today on your existing laptop. The stack:

- **Language:** Python (with PyGObject) or C (with GTK directly). Most Phosh apps are GTK; pick whichever you're more comfortable with.
- **UI toolkit:** GTK4 + libadwaita (the GNOME mobile-style widgets)
- **Build:** Meson + Ninja (standard for GTK apps)
- **Test:** Run the app from your IDE / terminal — it shows up as a regular GNOME app
- **Version control:** git (local for now, GitHub later)

A "hello world" SmartLife-style app is a `Adw.ApplicationWindow` with an `Adw.HeaderBar`, a `Gtk.Box` for content, and a `Adw.NavigationView` for the split-pane pattern. You can write this in 30 minutes on your laptop and it will look correct on GNOME3 today, and will look correct on Phosh on Pinephone and Lyra tomorrow.

### Tier 2: Pinephone (Phosh) — weekly check-in

When you want to validate Phosh-specific behavior:

1. Build the app on your laptop: `meson setup build && ninja -C build`
2. Package as a `.deb` (Debian packaging tools) or just `scp` the binary
3. Install on Pinephone: `sudo dpkg -i <package>.deb` or copy into `~/.local/bin/`
4. Launch from the app drawer, or via `phosh`'s "Run command" (super + space, then type)
5. Test: touch behavior, mobile form factor, status bar interaction, lockscreen, app lifecycle (backgrounding, notifications)
6. Screenshot / log any issues

You have 3 Pinephones, so you can:
- A/B test variants (different layouts, different colors)
- Cross-validate on different screen sizes (Pinephone has 5.95" 1440×720, Pro has 6", etc.)
- Use one as a "reference device" that always runs the latest stable, others for experiments

The Pinephone is a real Phosh device — same stack we'll run on Lyra. So this is high-fidelity testing.

### Tier 3: Lyra Zero W (Phosh) — when SmartLife-specific code is ready

When you have:
- The e-ink display profile working
- The OLED strip renderer working
- The RMIO expansion connector configured
- The Phosh autostart (no display manager) working

Then you deploy to the Lyra. This is where the SmartLife-specific behavior gets validated: refresh modes, OLED on/off, power management, RMIO modules, etc.

But the app code itself (the GTK / libadwaita widgets) is unchanged from what you wrote on the laptop. Only the platform-specific glue (display config, pin config, power management) is Lyra-specific.

## Building a SmartLife app: the recipe

```python
# smartlife-notes/app.py — example
# This is the structure of a Phosh app. It will run unchanged on
# GNOME3, Pinephone, and Lyra.

import gi
gi.require_version('Gtk', '4.0')
gi.require_version('Adw', '1')

from gi.repository import Gtk, Adw, GLib

class SmartLifeNotesWindow(Adw.ApplicationWindow):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.set_title("Notes")
        self.set_default_size(360, 640)

        # Header bar (Phosh-style: minimal, content-first)
        header = Adw.HeaderBar()
        self.set_titlebar(header)

        # Main content: a navigation view (master/detail)
        # Adapts from split-pane on tablets to stacked on phones
        nav = Adw.NavigationView()
        header.set_title_widget(nav)

        # ... content here ...

        self.set_content(nav)

class SmartLifeNotesApp(Adw.Application):
    def __init__(self):
        super().__init__(application_id="org.smartlife.notes",
                         flags=Gio.ApplicationFlags.FLAGS_NONE)

    def do_activate(self):
        win = SmartLifeNotesWindow(application=self)
        win.present()

if __name__ == "__main__":
    import sys
    app = SmartLifeNotesApp()
    app.run(sys.argv)
```

This app will:
- Run as a regular window in GNOME3
- Run in the app drawer on Pinephone (Phosh) — same code
- Run in the app drawer on Lyra (Phosh) — same code

Only when you want **SmartLife-specific behavior** (e-ink refresh on save, OLED confirmation flash, RMIO module status) do you add Lyra-specific code. And that code goes in a separate layer (e.g., `smartlife_platform.py` with stubs that do nothing on Pinephone, real implementations on Lyra).

## What this doc locks in

- 3-tier dev loop: laptop → Pinephone → Lyra
- 90% of the work happens on the laptop, no Luckfox-specific tooling needed
- Pinephones are the high-fidelity cross-test for Phosh behavior
- Lyra-specific code is the thin platform layer; app code is portable GTK / libadwaita
- No build calendar; work on the laptop whenever, validate on Pinephone when you have one handy, deploy to Lyra when SmartLife-specific code is ready

## Open questions / next spikes

- **Pinephone Phosh version** — what does your Mobian install look like? Anything we'd want to align with (or diverge from) for SmartLife?
- **Packaging** — `.deb` per app, or Flatpak, or a single `smartlife-os` image that includes everything? Affects the dev loop.
- **Theme / Adwaita skin** — our design tokens map to libadwaita, but we should define a custom `Adw.StyleManager` palette for SmartLife (calm paper-feel colors, monospace-leaning for terminal-y feel)
- **Test framework** — for GTK apps, how do we want to write tests? (Dogtail, LDTP, manual, screenshot diff?)

## See also

- `software/STACK-DECISION.md` — Phosh + phoc + squeekboard as the UI stack
- `research/lyra-os-landscape.md` — OS options
- `research/lyra-platform-tooling.md` — `luckfox-config` and dynamic device tree
- `hardware/RMIO.md` — expansion connector
- `ui/DESIGN-SYSTEM.md` — the SmartLife HIG
