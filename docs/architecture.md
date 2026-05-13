# System Architecture: Modular Implementation

The `gig` engine is designed with a focus on modularity and maintainability. Written in C11/POSIX, it provides a platform for terminal documentation.

## 1. Modular Organization (SoC)
The system is divided into logical sub-systems:

### The Parser (`src/parser/`)
Translates `.gg` files into a document object.
- `lexer.c`: Trigger identification.
- `metadata.c`: Validation of guide identity.
- `doc.c`: Management of the document object.

### The Layout Engine (`src/layout/`)
Translates the document into a formatted grid.
- `builder.c`: Implements viewport and indentation rules.
- `table.c`: Rendering system for wrapped Unicode tables.
- `inline.c`: Processes semantic tokens (bold, commands, links).
- `wrap.c`: Handles ANSI-safe word wrapping.

### The Pager Renderer (`src/pager/`)
An interactive TUI application for viewing guides.
- `render.c`: Drawing logic for the viewport and status bar.
- `input.c`: Keyboard handling and interaction.
- `buffer.c`: Memory-buffered, flicker-free rendering.

### The CLI Locator (`src/cli/`)
Manages guide discovery and search path resolution.
- `locator.c`: Implements the POSIX-standard section search order.

## 2. Advanced Table Engine
One of `gig`'s most powerful features is its table engine. Unlike many terminal viewers that truncate content, `gig` supports **internal warping**. When content is too long for a column, it wraps *inside* the cell, while maintaining perfect vertical alignment with a high-fidelity Unicode grid.

## 3. Build & Maintenance
The project uses a standard `Makefile` with automatic dependency tracking. This ensures that any change to a shared header or module correctly triggers a re-build of only the affected components, keeping development cycles fast and reliable.
