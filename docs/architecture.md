# System Architecture: Modular Implementation

The `gig` engine is designed with a focus on modularity, performance, and ANSI-compliant terminal rendering. It follows a strict separation of concerns (SoC) to ensure that the document parsing, layout calculation, and interactive display are independent and robust.

## 1. High-Level Data Flow

```text
[ .gg File ] ───▶ ( Parser ) ───▶ [ gig_doc_t ]
                                       │
                                       ▼
[ Viewport ] ◀─── ( Pager )  ◀─── ( Layout )
( Redraws )        ( TUI )       ( Staircase )
```

## 2. Component Breakdown

### A. The Parser (`src/parser/`)
The parser is a single-pass state machine that converts raw text into a structured document model.
- **Lexer (`lexer.c`):** Identifies "First-Character Triggers" at the start of each line.
- **Metadata (`metadata.c`):** Processes the header zone (above `---`), enforcing a whitelist of keys (`@name`, `@section`, etc.) and validating syntax.
- **Document Model (`doc.c`):** Manages `gig_doc_t`, which stores metadata in a dynamic array and content as a linked list of `gig_block_t` nodes.

### B. The Layout Engine (`src/layout/`)
The layout engine is responsible for transforming semantic blocks into a character-precise terminal representation.
- **Builder (`builder.c`):** Implements the **Golden Responsive Staircase**. It calculates container widths and proportional indents based on the current terminal size.
- **Table Engine (`table.c`):** A renderer that calculates column weights using a heuristic fluidity score, performs internal cell wrapping, and draws Unicode borders.
- **Inline Processor (`inline.c`):** A regex-like scanner that injects ANSI color escape sequences into text based on semantic markers (e.g., `**bold**`, `-f`).
- **Advanced Wrapper (`wrap.c`):** A UTF-8 aware, semantic wrapping algorithm. It prioritizes breakpoints (spaces, punctuation, paths) and uses "break-before" logic for brackets while preserving ANSI state.

### C. The Pager Renderer (`src/pager/`)
The pager provides the interactive interface, managing terminal state and user input.
- **TUI Renderer (`render.c`):** Orchestrates the drawing of the header, the scrollable layout area, and the dynamic status bar.
- **Input Handler (`input.c`):** Manages non-blocking keyboard reads and sequence decoding (e.g., arrow keys, PageUp/Down).
- **Buffer Management (`buffer.c`):** Implements a growable memory buffer to batch all ANSI commands into a single `write()` syscall, eliminating flicker.

### D. CLI Utilities (`src/cli/`)
Support modules for the command-line interface.
- **Locator (`locator.c`):** Resolves guide names into absolute file paths using the Tiered POSIX Search Order.
- **Sync Service (`sync.c`):** Manages documentation updates. It implements a robust synchronization logic using `curl` and `tar` with an **Atomic Swap** mechanism: new data is prepared in a temporary workspace and swapped with the existing library only when fully validated, ensuring zero-downtime and data integrity.

## 3. Core Data Structures

### `gig_doc_t` (The Document)
Stores the entire guide in memory.
- `metadata`: Key-value pairs.
- `blocks`: Linked list of semantic content blocks.
- `error`: Error state for validation failures.

### `gig_layout_t` (The Formatted View)
The output of the layout engine, ready for rendering.
- `lines`: An array of pre-colorized, pre-wrapped UTF-8 strings.
- `count`: Total number of lines in the formatted document.

## 4. Search Path Strategy
The engine implements a **Tiered POSIX Search Order** to resolve guide names when no section is specified.
- **Resolution Order:** `1`, `n`, `l`, `8`, `3`, `2`, `5`, `4`, `9`, `6`, `7`.
- **Transparency:** The absolute search path is exported as `$GIGPATH` to the environment.

## 5. Signal Handling & Responsiveness
The `gig` engine is fully responsive. It intercepts the `SIGWINCH` (Window Change) signal. When the terminal is resized:
1. The Pager catches the signal and queries the new dimensions via `ioctl`.
2. The Layout Engine is re-invoked with the new width to rebuild the `gig_layout_t`.
3. The Pager re-centers the scroll position and issues a full redraw.

## 6. Build System
The project is built using a dependency-aware `Makefile`.
- **`MMD -MP` Flags:** Generate header dependency files (`.d`) automatically.
- **Standard:** C11 with POSIX.1-2008 extensions.
