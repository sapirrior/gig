# GIG Syntax Specification

The **General Interactive Guide (GIG)** syntax is a line-oriented format designed for technical documentation. It combines the readability of Markdown with the structural rigor of a manual.

## 1. Document Anatomy
A `.gg` file is split into two distinct zones by a mandatory triple-dash (`---`) separator.

### The Metadata Zone
Located at the very top of the file. Metadata keys start with `@`, followed by a space and a double-quoted value.
- **Required Keys:** `@name`, `@section`.
- **Supported Keys:** `@version`, `@author`, `@updated`.

```gig
@name    "example"
@section "1"
@version "1.0.0"
---
```

### The Content Zone
Everything below `---` is the body of the guide. It uses **First-Character Triggers** to define blocks.

---

## 2. Block Triggers

| Trigger | Semantic Meaning | Rendering Behavior |
| :--- | :--- | :--- |
| `# ` | **Header** | Bold Cream, 0 indent, adds vertical spacing. |
| `## ` | **Subheader** | Bold Sand, partial indent, adds vertical spacing. |
| `. ` | **Body Text** | Standard paragraph, 10% indent, auto-wraps. |
| `! ` | **Literal** | Sage Green, 15% indent, **no wrapping**, smart truncation. |
| `* ` | **Bullet** | Grey bullet point, 15% indent, wrapping support. |
| `- ` | **Definition** | `Term : Desc` format. Term is bold, Desc wraps at 15%. |
| `| ` | **Table** | Wrapped Unicode grid. First row is Header (Cream BG). |
| `$ ` | **Shell** | Command example, Sage Green, 15% indent. |
| `> ` | **Quote** | Olive Green, 15% indent, italicized. |
| ` ` | **Continuation** | Lines starting with a space append to the previous block. |

### The Continuation Rule
To create multi-line paragraphs or blocks without repeating triggers, start subsequent lines with at least one space.
```gig
. This is a very long paragraph that spans multiple
 lines in the source file but will be treated as
 a single semantic block by the engine.
```

---

## 3. Inline Semantic Styling
The engine automatically "senses" tokens within text and styles them accordingly.

| Pattern | Style | Purpose |
| :--- | :--- | :--- |
| `**text**` | **Bold** | Strong emphasis. |
| `` `cmd` `` | Sage Green | Command names or binaries. |
| `<var>` | Terracotta | Placeholders or required arguments. |
| `[opt]` | Grey | Optional flags or parameters. |
| `{link(1)}` | Slate Blue | Cross-reference (underlined). |
| `--flag` | Sage Green | Automatically detects hyphenated flags. |
| `"str"` | Terracotta | Double-quoted strings. |

---

## 4. Responsive Tables
Tables in GIG are dynamic and responsive. You don't need to align them in the source; the engine handles it.
```gig
| Command | Effect
| `ls -a` | Lists all files, including hidden ones.
| `rm -rf` | **DANGER**: Recursively removes files without asking.
```
- The engine calculates column weights based on the longest words.
- If the terminal is narrow, text inside cells will wrap automatically.

## 5. Best Practices
1. **Vertical Rhythm:** Use `# ` for major sections (NAME, SYNOPSIS, DESCRIPTION) and `## ` for grouping related items (Navigation, Formatting).
2. **Semantic Accuracy:** Use `$ ` for commands and `! ` for raw data or code blocks.
3. **Keep it Clean:** Avoid excessive inline styling; the staircase layout already provides high readability.
