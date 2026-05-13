# GIG Engine Architecture

`gig` is an engine for parsing and rendering General Interactive Guides. It provides a structured interface for documentation.

## Core Identity
- **Format:** GIG v1.2.x
- **File Extension:** `.gg`
- **Primary Goal:** Provide structured terminal documentation.

## Modular Components (SoC)

### 1. The Parser (`src/parser/`)
- **Lexer:** Trigger identification.
- **Metadata:** `@key "value"` validation.
- **Document Model:** Linked-list of semantic blocks.

### 2. The Layout Engine (`src/layout/`)
- **Viewport Boundary:** Centering content within a 72-character width.
- **Indentation Model:** 0 (Headers), 3 (Sub), 7 (Body/Tables), 11 (Deep/Quotes).
- **ANSI-Aware Wrapping:** Preserves formatting across line breaks.
- **Table Engine:** Scales columns and wraps content inside cells.


### 3. The Interactive Pager (`src/pager/`)
- **Buffered Rendering:** Uses a custom `gig_buffer_t` for flicker-free updates.
- **Input handling:** Dedicated module for `less`-style keybindings and responsive prompts.
- **Responsive Design:** Listens for `SIGWINCH` to instantly re-calculate layout on resize.

## Search Path Strategy
The engine uses a **Single Source of Truth** for resolving guide names:
- **Default Path:** `/usr/share/gig` (or Termux equivalent).
- **POSIX Order:** Searches sections in standard 1, n, l, 8, 3, 2, 5, 4, 9, 6, 7 order.
- **GIGPATH:** Automatically exported to the environment for transparency.
