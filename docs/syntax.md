# GIG Syntax: A Guide to Modern Manuals

Welcome to the **General Interactive Guide (GIG)** syntax specification. GIG was designed to bridge the gap between the simplicity of Markdown and the semantic power required for professional technical documentation.

## Core Philosophy
The GIG format is strictly line-oriented. By using unique "First-Character Triggers," the engine can parse a document efficiently while ensuring content remains readable in its raw form.

---

## 1. Document Structure
Every GIG file (`.gg`) is divided into two primary areas by a triple-dash (`---`) separator.

### Metadata Zone
At the top of your file, define your guide's identity using the `@` trigger. Both `@name` and `@section` are required for the guide to be valid.

```gig
@name    "example"
@section "1"
@author  "Your Name"
---
```

### Content Zone
Everything below the separator is the body of your guide.

---

## 2. Block Triggers
Each line in your guide must begin with a semantic trigger followed by a space.

| Trigger | Name | Purpose |
| :--- | :--- | :--- |
| `# ` | **Header** | Top-level section names (e.g., NAME, SYNOPSIS). |
| `## ` | **Subheader** | Groups within a section (e.g., Navigation Keys). |
| `. ` | **Body** | Standard paragraphs. Wraps based on the fluid viewport width. |
| `! ` | **Literal** | Technical lines that skip wrapping and formatting (e.g., code). |
| `| ` | **Table** | Creates a structured grid with automatic headers and wrapping. |
| `- ` | **Definition** | Formatted as `Term : Description` for flags and parameters. |
| `* ` | **Bullet** | Standard list items. |
| `$ ` | **Example** | Command-line examples, rendered in Sage Green. |
| `> ` | **Quote** | Notes, warnings, or helpful pro-tips. |
| ` ` | **Continuation** | Lines starting with a space append to the previous block. |

---

## 3. Inline Semantic Styling
GIG automatically senses and styles technical tokens to help your readers scan information faster:

- **Bold Emphasis:** Wrap text in `**double asterisks**`.
- **Commands:** Use `` `backticks` `` for executable commands or binary names.
- **Variables:** Use `<angle brackets>` for placeholders or arguments.
- **Options:** Use `[square brackets]` for optional components.
- **Links:** Use `{target(section)}` to create a cross-reference to another guide.
- **Flags:** Any word starting with a hyphen (e.g., `--verbose`) is automatically highlighted.

## 4. Writing Best Practices
To ensure your guides look beautiful in the `gig` viewer:
1. **Be Concise:** Use the "Staircase" hierarchy to keep your information structured.
2. **Use Tables:** Use the `| ` trigger for structured data; the engine will handle the alignment for you!
3. **Semantic Triggers:** Always use the correct trigger for the content type—don't use `> ` for standard body text!

Happy writing!
