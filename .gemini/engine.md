# GIG Engine: Technical Deep Dive

This document provides low-level technical details for AI models to understand the internal mechanics of the `gig` implementation.

## 1. Memory Management & Document Model
The `gig_doc_t` structure is the central repository for a parsed guide.

### Linked Block Architecture
Instead of a flat array, `gig` uses a linked list of `gig_block_t`. This allows for O(1) appending during parsing and simplifies the insertion of logical breaks.
- `type`: `GIG_BLOCK_HEADER`, `GIG_BLOCK_TABLE`, etc.
- `content`: Raw UTF-8 string with triggers stripped.
- `term`: Specific to `GIG_BLOCK_DEF`.

### Metadata Storage
Metadata is stored as a dynamic array of `gig_meta_t` (key-value pairs) inside `gig_doc_t`. Key lookups are O(N) but given the small count (usually < 10), this is negligible.

## 2. The Parser State Machine (`parser/`)
The parser is a single-pass, line-oriented state machine.
- **Trigger Detection:** Managed by `lexer.c` using prefix matching.
- **Continuation Logic:** A line starting with a space character is automatically appended to the `last_block` content, allowing for multi-line paragraphs without repeating triggers.
- **Strict Validation:** Fails immediately if metadata is missing or if triggers are malformed.

## 3. Layout & Visual Constraints (`layout/`)
The layout engine transforms semantic blocks into a `gig_layout_t`, which is essentially an array of pre-rendered, colorized, and wrapped lines.

### Viewport & Indentation
- **Boundary:** Golden Responsive Staircase. Dynamically scales content width to 80% of the terminal with balanced margins.
- **Staircase Logic:** Uses proportional, percentage-based indentation for visual hierarchy:
    - Content Area: 80% of terminal.
    - Body Indent: 10% of content width.
    - Deep Block Indent (Bullets/Literals/Quotes): 15% of content width.
- **ANSI-Aware Wrapping:** `gig_wrap_text` in `wrap.c` calculates visual width by skipping `\x1b[` sequences. It tracks the current ANSI state and resets/restores it at line boundaries to prevent "style bleeding."

### The Table Engine (`table.c`)
The table system uses a multi-pass heuristic for column allocation.
- **Fluidity Scoring:** Distinguishes between 'fixed' label columns and 'fluid' content columns by analyzing word density and length.
- **Priority Allocation:** Satisfies fixed columns first (up to a cap) and distributes the bulk of the width to fluid columns.
- **Responsive Stacking:** On very narrow terminals (< 40 chars), automatically stacks 2-column tables into a vertical bulleted definition format.

### Advanced Wrapping (`wrap.c`)
A UTF-8 aware, semantic-aware wrapping module.
- **Tiered Breakpoints:** Prioritizes spaces (P3), punctuation (P2), and technical separators (P1, e.g., `/`, `_`, `.`) to ensure logical breaks for paths and URLs.
- **Bracket Awareness:** Implements "break-before" logic for opening brackets to maintain parameter grouping.
- **ANSI Persistence:** Tracks ANSI escape sequences across line breaks with a 1024-byte state buffer to prevent style bleeding.

## 4. TUI & Rendering (`pager/`)
The pager is a `termios` application using a custom memory buffer.

### Flicker-Free Buffering (`buffer.c`)
- Uses a growable byte buffer to collect all ANSI escapes and text for a single frame.
- A single `write()` call is issued per frame to ensure smooth transitions and no flickering during high-speed scrolling.

### Event Handling
- **SIGWINCH:** Triggers a full rebuild of the `gig_layout_t` using the new terminal column count as a constraint, making the application fully responsive.
- **Search:** Case-insensitive `strcasestr` search. Matches are highlighted using `GIG_CLR_SEARCH` (Orange BG).
