# GIG AI Context: Master Index

This directory contains the deep technical specifications and architectural constraints of the `gig` project. It is intended for AI models to fully understand the codebase and its mandates.

## 1. Project Identity
- **Name:** `gig` (General Interactive Guide)
- **Language:** C11 (POSIX compliant)
- **Version:** Managed via `Makefile` (`GIG_VERSION`)

## 2. Technical Documentation
- **[Engine Deep Dive](engine.md):** Low-level details on memory, parsing, and layout algorithms.
- **[Architecture](specs/architecture.md):** High-level component organization and SoC.
- **[Syntax Specification](specs/syntax.md):** The GIG v1.2.x line-oriented format.
- **[Pager Specifications](specs/pager.md):** Viewport boundaries, color palettes, and navigation.

## 3. Core Mandates for AI
- **Strict Adherence:** All code must remain C11 and POSIX compliant.
- **No Hyperbole:** Documentation and comments must remain objective and technical.
- **Modular Growth:** New features must be integrated into the existing modular structure (`src/parser/`, `src/layout/`, etc.).
- **Visual Integrity:** Any changes to the layout must respect the 72-character viewport and the 0-3-7-11 indentation model.

## 4. Key Symbols
- `gig_doc_t`: Root document object.
- `gig_layout_t`: Formatted line array.
- `gig_render_table`: Core table wrapping logic.
- `gig_pager_run`: Main TUI event loop.
