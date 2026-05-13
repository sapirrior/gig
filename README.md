# gig: General Interactive Guide Engine

[![Language: C11](https://img.shields.io/badge/Language-C11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

`gig` is a C11/POSIX engine for viewing GIG-formatted documentation. It provides a semantically structured and interactive interface for terminal-based guides.

## ✨ Features

- **Viewport Configuration:** Content is rendered within a 72-character width boundary to optimize readability.
- **Organic Palette:** A color scheme (Cream, Sage, Terracotta) optimized for terminal visibility.
- **Table Engine:** Support for multi-line cell wrapping within a Unicode grid system.
- **Structural Hierarchy:** Implementation of a consistent indentation model (0-3-7-11 rule) for document navigation.
- **Interactive Pager:** Navigation features including case-insensitive search and SIGWINCH-responsive layout recalculation.
- **Modular Design:** An architecture organized into distinct functional sub-systems.

## 🚀 Quick Start

### Build
Requires a C compiler (`clang` or `gcc`) and `make`.
```bash
make
```

### View a Guide
```bash
./build/gig view test.gg
```

### Check System Path
```bash
./build/gig path
```

## ⌨️ Navigation

| Key | Action |
| :--- | :--- |
| `j` / `k` | Scroll Down / Up |
| `d` / `u` | Half-page Down / Up |
| `f` / `b` | Full-page Down / Up |
| `g` / `G` | Jump to Start / End |
| `/` | Search Text |
| `n` / `N` | Next / Previous Match |
| `q` | Exit |

## 📚 Documentation

Technical documentation:
- [Syntax Guide](docs/syntax.md): GIG file format specification.
- [Design Specifications](docs/pager.md): Viewport and color palette definitions.
- [Architecture](docs/architecture.md): System design and modular organization.
- [Installation](docs/installation.md): Environment setup and configuration.
- [Contributing](CONTRIBUTING.md): Guidelines for development.
