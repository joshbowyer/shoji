# SmartLife System — Dev Setup

> Concrete setup for building the custom e-ink shell. System packages, reference repos, the minimal "hello world" plan, the dev loop, and when to move to hardware.
> Status: v0 (2026-06-17). The companion to `software/DEV-WORKFLOW.md` (conceptual) — this is the *how*.

## What we're building

The custom e-ink-first shell (per `ui/EINK-DE-SHELL.md`) as a single **libadwaita app** that runs on your laptop in GNOME3 today, on Pinephone (real Phosh) tomorrow, and on Lyra Zero W (real e-ink + OLED) eventually.

The shell uses a **"fake e-ink surface" widget** for laptop dev — a styled container that mocks the real e-ink paper feel and refresh animations. The shell logic is real; the display renderer is swappable. When we move to real hardware, the fake widget is replaced with a real e-ink driver and the shell code doesn't change.

## Language: C + Meson (recommended)

| Language | Pros | Cons |
|---|---|---|
| **C + Meson** ✅ | The Phosh ecosystem's native language. Reference code is C. Native GTK4 + libadwaita. Fast, idiomatic. | Verbose. Slower to write. |
| Python + PyGObject | Faster to write, slower to iterate visually. Good for prototyping. | Slower at runtime (not a real issue for a shell). Less idiomatic in the Phosh world. |
| Rust + gtk-rs | Modern, type-safe, memory-safe. Cleaner code. | Steeper learning curve. Phosh ecosystem isn't there yet. |

**Recommendation: C + Meson** for the shell. The reference code is C, the patterns are C, and Phosh apps in C are easier to learn from and contribute to. The shell will be a single binary that runs as a Wayland client on GNOME3, Phosh, or phoc.

## System packages (Debian/Ubuntu on laptop)

```bash
sudo apt update
sudo apt install -y \
  build-essential \
  meson \
  ninja-build \
  pkg-config \
  libgtk-4-dev \
  libadwaita-1-dev \
  libglib2.0-dev \
  libsoup-3.0-dev \
  libgtk4-layer-shell-dev \
  desktop-file-utils \
  gettext \
  blueprint-compiler
```

That's the full GTK4 + libadwaita + Meson dev environment. Nothing exotic.

## Reference repos to clone (for learning, not for forking)

Clone these somewhere outside the smartlife-system project (e.g., `~/src/` or `~/reference/`) so you can read them without polluting the project:

```bash
mkdir -p ~/src/reference
cd ~/src/reference

# The shell we're replacing — read for architecture, not for code
git clone https://gitlab.gnome.org/World/Phosh/phosh.git

# The Wayland compositor — read for multi-output (e-ink + OLED strip)
git clone https://gitlab.gnome.org/World/Phosh/phoc.git

# Best example of a libadwaita app with subpages (NavigationView)
git clone https://gitlab.gnome.org/World/Phosh/phosh-mobile-settings.git

# Single-window Phosh app example
git clone https://gitlab.gnome.org/World/Phosh/calls.git

# On-screen keyboard — we'll consume, not modify
git clone https://gitlab.gnome.org/World/Phosh/squeekboard.git
```

For build templates, GNOME's `gnome-template` is the canonical GTK project scaffold. Or use `meson init` and customize.

## The minimal first thing

Don't try to build the whole shell. Build the **smallest thing that proves the dev loop works**:

**`hello.c`** — a single libadwaita app with:

- One `Adw.ApplicationWindow` (window)
- One `Adw.HeaderBar` (title bar)
- One `Adw.NavigationView` (the master/detail pattern)
- One `Adw.NavigationPage` (the home placeholder)
- A "fake e-ink surface" widget — a styled `Gtk.Box` with the paper-feel background, showing "SmartLife Pocket — Shell v0" in the design tokens
- A "fake OLED island" widget — a small centered `Gtk.Box` below the fake e-ink
- The SmartLife design tokens as CSS custom properties in a `.css` file

That's it. ~150 lines of C + ~50 lines of CSS. Should take 30-60 minutes from a template.

**Build commands:**

```bash
cd ~/src/smartlife-shell  # or wherever the project lives
meson setup build
meson compile -C build
./build/smartlife-shell
```

**Expected result:** A regular GNOME3 window with the SmartLife design language in action, including the fake e-ink surface and OLED island. Validates the build, the design system, and the dev loop in one shot.

## The dev loop after that

```bash
# Edit
$EDITOR src/smartlife-shell.c

# Rebuild (incremental, fast)
meson compile -C build

# Run (in dev environment with proper paths)
meson devenv -C build -e ./smartlife-shell

# Or just run the binary
./build/smartlife-shell
```

**Typical loop: edit → build → run → see → edit. ~5 seconds per iteration.** Most of the time is thinking, not compiling.

For iteration speed, you can also use GNOME Builder (the official GNOME IDE), which handles the build/run cycle for you with hot reload. Worth installing alongside the CLI toolchain:

```bash
sudo apt install gnome-builder
```

## The fake e-ink widget

The shell's display is a single `Gtk.Box` (the "device") containing two child widgets:

- **Fake e-ink surface** — a `Gtk.Box` with `background-color: var(--color-bg)`, no border-radius, large fixed size (360×540 css in the simulator, real size on real hardware). Renders whatever content the shell wants to show.
- **Fake OLED island** — a smaller `Gtk.Box` centered below the e-ink, `background-color: var(--oled-bg)`, with text in `var(--oled-fg)`. Renders the live status / token stream / waveform.

When we move to real e-ink (Tier 3, Pi + Waveshare HAT), the fake e-ink widget becomes a real e-ink renderer that talks to the SPI panel. The shell code that calls into the widget doesn't change — same API, different implementation.

The fake e-ink widget can also implement the refresh modes as CSS animations (full / partial / spot), as the browser simulator already does. That gives us a consistent visual model across the simulator and the shell during development.

## Reference patterns to learn

When you're ready to build the actual shell (lockscreen, home, app grid), these are the patterns to study:

| What we need to build | Reference in Phosh |
|---|---|
| Single-window libadwaita app | `calls` |
| Subpages (NavigationView) | `phosh-mobile-settings` |
| PAM lockscreen backend | `phosh/src/lockscreen-manager.c` |
| Background apps, status bar | `phosh/src/overview.c`, `phosh/src/status-icon.c` |
| Multi-output Wayland config | `phoc/src/` |
| On-screen keyboard integration | `squeekboard` (any Phosh app shows the pattern) |
| Touch + button input handling | `phosh/src/manager.c`, GTK event controllers |

Read the reference code, copy the patterns, don't copy the code. Our shell will look nothing like Phosh's shell because e-ink is a different medium, but the plumbing patterns are the same.

## When to move off the laptop

| Trigger | Move to |
|---|---|
| Hello world runs on laptop with the design tokens | **Build the lockscreen + home** on laptop |
| Lockscreen + home work on laptop | **Pinephone** — validates Phoc + squeekboard + libadwaita + PAM on real Phosh infra |
| Shell is feature-complete on Pinephone | **Pi + Waveshare 4.2" B/W HAT** (~$50) — real e-ink |
| Real e-ink works | **Lyra Zero W** — final target |

The laptop is home for the next several weeks. The Pinephone is a checkpoint (you have 3 — use one as a Phosh test device). The Pi and Lyra are downstream.

## Git workflow

This project is in git from day one. See `software/DEV-WORKFLOW.md` for the conceptual workflow; the practical git setup:

- **Repo:** `git init` in `/home/josh/workspace/smartlife-system/`
- **Remote:** GitHub (or wherever you prefer) — `git@github.com:joshbowyer/smartlife-system.git` or similar
- **Branch strategy:** `main` is stable, feature work in `feature/<name>` branches, PRs back to main
- **Commit style:** Conventional Commits (`feat: ...`, `fix: ...`, `docs: ...`, `chore: ...`)
- **What to commit:** all docs (markdown, simulator, heroes), the future C source, build configs (meson, etc.). Do NOT commit: `build/` (meson output), OS files (`.DS_Store`), editor backups (`*~`).

`.gitignore` is minimal for now — see `software/DEV-SETUP.md` for the template.

## Open questions

- **Where does the C source live in the project?** Options: `src/` at the project root, or `software/smartlife-shell/` as a subdir. My rec: `src/` at the root, mirroring GNOME conventions.
- **Single binary or split?** Single binary for the shell is fine for v1. Apps (Notes, Chat, etc.) come later as separate binaries.
- **CI?** Defer until we have something to test. When we do, GitHub Actions is the obvious choice (Linux runners, apt-based, cheap).
- **Pinephone dev workflow** — `scp` the binary, or set up a cross-compile, or use GNOME Builder's remote? Simplest is `scp` for v1.

## See also

- `ui/EINK-DE-SHELL.md` — the custom shell design (the active goal)
- `ui/DESIGN-SYSTEM.md` — the design tokens (the shell consumes them)
- `ui/DISPLAY-STRATEGY.md` — e-ink + OLED split
- `ui/DEVICE-MOCKUP.md` — visual concept
- `ui/simulator.html` — the browser-based e-ink simulator (the design's first living form)
- `software/DEV-WORKFLOW.md` — the conceptual 3-tier dev loop (laptop → Pinephone → Lyra)
- `software/STACK-DECISION.md` — Phosh infra + custom shell stack
- `README.md` — project status
