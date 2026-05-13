# Pager Design: Terminal Viewport Specifications

The `gig` pager is designed to provide a structured environment intended to reduce eye strain and improve information retention during documentation sessions.

## 1. Color Palette
`gig` uses a palette optimized for terminal visibility:

- **Cream:** Used for primary headers.
- **Sage:** Used for actions and commands.
- **Terracotta:** Used for variables and values.
- **Bold Terracotta:** Used for fatal errors.
- **Orange:** Used for warnings and hints.
- **Deep Clay:** Used for the status bar.

## 2. Layout & Viewport
Readability is influenced by line length. `gig` implements a **Golden Responsive Staircase** that scales the content width to 80% of the terminal while maintaining balanced margins.

### The Indentation Model
To help navigate documents, a fluid percentage-based indentation system is used:
- **Major Headers:** Indent 0.
- **Sub-headers:** Indent proportional to text level.
- **Body Text:** Indent calculated at 10% of content width.
- **Deep Content:** Indent calculated at 15% of content width (Bullets, Examples, Quotes, Literal Blocks).


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
