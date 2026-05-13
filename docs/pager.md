# Pager Design: Terminal Viewport Specifications

The `gig` pager is designed to provide a structured environment intended to reduce eye strain and improve information retention during documentation sessions.

## 1. Color Palette
`gig` uses a palette optimized for terminal visibility:

- **Cream:** Used for primary headers.
- **Sage:** Used for actions and commands.
- **Terracotta:** Used for variables and values.
- **Deep Clay:** Used for the status bar.

## 2. Layout & Viewport
Readability is influenced by line length. `gig` implements a **72-character viewport boundary** regardless of terminal width. This maintains a consistent scanning distance for the reader.

### The Indentation Model
To help navigate documents, a progressive indentation system is used:
- **Level 0 (Indent 0):** Major section headers.
- **Level 3 (Indent 3):** Sub-headers.
- **Level 7 (Indent 7):** The primary viewport for body text and tables.
- **Level 11 (Indent 11):** Examples and quotes.


## 3. Navigation Controls
The `gig` pager uses standard `less`-style keybindings for a familiar experience:

- **j / k:** Move down or up by one line.
- **d / u:** Jump half a page down or up.
- **f / b:** Flip a full page down or up.
- **g / G:** Instantly return to the start or skip to the end.
- **/**: Search for any term (case-insensitive).
- **n / N:** Cycle through search results.
- **q:** Exit the pager immediately.

## 4. Responsive Design
If you resize your terminal window, `gig` will intelligently re-calculate the layout. It listens for the `SIGWINCH` signal and instantly re-wraps text and re-centers the Golden Ratio plane, ensuring your documentation always looks perfect.
