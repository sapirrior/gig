# Pager Design: Terminal Viewport Specifications

The `gig` pager is an interactive interface designed to provide a formatted reading experience in the terminal.

## 1. The Golden Responsive Staircase
`gig` does not use a fixed-width layout. Instead, it implements a responsive container model that ensures content is never too wide for the eye to track comfortably, nor too cramped on small screens.

### Viewport Breakpoints
The engine calculates `content_width` and `gutter` (margins) based on the terminal's column count:

- **Narrow (< 40 cols):** Content = Full width - 2. Minimal margins (1 char).
- **Medium (40-84 cols):** Content = Full width - 8. Fixed margins (4 chars).
- **Wide (> 84 cols):** **The Golden Ratio**. Content = 80% of terminal width. Gutter = Remaining 20% split equally.

### Proportional Indentation
Within the content area, a "Staircase" hierarchy is maintained:
- **Level 0 (Headers):** Indent = 0.
- **Level 1 (Paragraphs/Tables):** Indent = 10% of content width.
- **Level 2 (Blocks/Quotes):** Indent = 15% of content width.

## 2. Interactive Search
`gig` implements a case-insensitive search algorithm that respects ANSI styling.

- **Trigger:** Press `/` to enter search mode.
- **Highlighting:** Matches are rendered with a bright orange background (`GIG_CLR_SEARCH`).
- **ANSI-Awareness:** The search highlighter is "ANSI-transparent"—it can find text that spans multiple color blocks and will not break existing styles.
- **Navigation:**
    - `n`: Jump to the next match.
    - `N`: Jump to the previous match.

## 3. Responsive Redrawing
The pager is fully responsive to terminal resizing.
- **SIGWINCH:** When the terminal size changes, the pager immediately drains its input buffer, queries the new `rows` and `cols`, and re-invokes the layout builder.
- **Viewport Persistence:** The engine attempts to maintain the relative scroll percentage during a resize so you don't lose your place.

## 4. The Status Bar
The status bar at the bottom provides essential context:
- **Command/Search:** Shows the current mode or active search query.
- **Progress:** A percentage-based indicator of your position in the document.
- **Progress Bar:** A Unicode visual bar (`━╸───`) that grows as you scroll.
- **Responsive:** On narrow terminals, the bar and percentage automatically hide to prioritize the search query display.

## 5. Navigation Matrix

| Category | Keys | Behavior |
| :--- | :--- | :--- |
| **Scrolling** | `j` / `k` | Single line scroll. |
| **Leaping** | `d` / `u` | Half-page jump. |
| **Flying** | `f` / `b` | Full-page flip (Standard `less` behavior). |
| **Jumping** | `g` / `G` | Instant Start / End. |
| **Searching** | `/`, `n`, `N` | Text discovery and cycling. |
| **Exiting** | `q` | Clean exit, restores terminal state. |
