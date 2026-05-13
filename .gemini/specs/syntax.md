# GIG Syntax Specification

The General Interactive Guide (GIG) format is designed for terminal manuals. It uses a line-oriented syntax.

## 1. File Structure
A `.gg` file is split into two zones by a `---` delimiter.

### Zone A: Identity (Metadata)
Format: `@key "value"`
- Every key must start with `@`.
- Values **must** be enclosed in double quotes `" "`.

Example:
```gig
@name    "ls"
@section "1"
@author  "Gemini CLI"
@updated "2026-05-13"
---
```

### Zone B: Body (Content)
Uses First-Character Triggers for O(1) block detection.

| Trigger | Meaning | Rule |
| :--- | :--- | :--- |
| `# ` | Heading | Primary section title (Indent 0). |
| `## ` | Sub-heading | Category or group title (Indent Variable). |
| `. ` | Body | Standard paragraph text (Indent 10%). |
| `! ` | Literal | Unformatted/Non-wrapping technical text (Indent 15%). |
| `| ` | Table Row | Forms a wrapped Unicode grid (Indent 10%). |
| `- ` | Definition | `Term : Description` (Split by first `:`) |
| `* ` | Bullet | Standard indented list item (Indent 15%). |
| `$ ` | Example | Command-line example (Indent 15%). |
| `> ` | Quote | Pro-tips or warnings (Indent 15%). |
| ` ` | Continuation | Appends to the previous block. |

---

## 2. Table Engine (Internal Warping)
Tables are contiguous blocks of `| ` triggers.
- **Auto-Header:** The first row is styled with a Warm Cream background.
- **Warping:** Content that exceeds column width wraps *inside* the cell.
- **Grid:** Rendered with Unicode Box-Drawing characters.

---

## 3. Inline Semantic Highlighting
The `gig` engine "senses" tokens automatically:
- **Bold:** `**text**` (Heavy Emphasis)
- **Commands:** `` `command` `` (Sage Green)
- **Arguments:** `<variable>` (Terracotta)
- **Optional:** `[optional]` (Grey)
- **Links:** `{target(section)}` (Slate Blue + Underline)
- **Constants:** `"literal"` (Lavender)
- **Flags:** `-f` or `--flag` (Sage Green)
