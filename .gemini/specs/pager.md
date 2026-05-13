# GIG Pager Design Specifications

The `gig` engine pager is designed to provide a structured viewport for terminal documentation.

## 1. Color Palette
| Element | ANSI (256) | Hex (Approx) | Name |
| :--- | :--- | :--- | :--- |
| **Headers** | `230` | #FFF9E3 | Cream |
| **Subheaders** | `223` | #FFDAB9 | Sand |
| **Actions** | `108` | #87AF87 | Sage |
| **Values** | `173` | #D7875F | Terracotta |
| **Literal** | `108` | #87AF87 | Sage |
| **Links** | `109` | #87AFAF | Slate Blue |
| **Status BG** | `95` | #875F5F | Clay |

## 2. Layout & Viewport
- **Viewport Boundary:** Golden Responsive Staircase (Fluid 80% width).
- **Indentation Model:**
    - Level 0: Section Headers (`# `)
    - Level Text (10%): Body Text, Tables, Definitions
    - Level Deep (15%): Examples, Quotes, Bullets, Literal Blocks


## 3. The Status Bar
- **Location:** Bottom row.
- **Style:** Inverted Deep Clay (Bold White text).
- **Format:** ` GIG /query [PROGRESS%] ━╸─── `
- **Interactive:** Updates instantly during scroll and search.

## 4. Navigation (Less-style)
- `j / k` : Scroll line by line.
- `d / u` : Scroll half-page.
- `f / b` : Scroll full-page.
- `g / G` : Jump to Start / End.
- `/` : Case-insensitive text search.
- `n / N` : Cycle search matches.
- `q` : Instant exit.
