# gig: General Interactive Guide Engine

[![Language: C11](https://img.shields.io/badge/Language-C11-blue.svg)](https://en.wikipedia.org/wiki/C11_(C_standard_revision))
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

`gig` is a C11/POSIX engine for viewing GIG-formatted documentation. It provides a semantically structured and interactive interface for terminal-based guides.

## ✨ Features

- **Fluid Golden Layout:** Content scales to 80% of terminal width with a balanced responsive staircase indentation.
- **Organic Palette:** A professional color scheme (Cream, Sage, Terracotta) with dedicated high-visibility highlights for errors and warnings.
- **Embedded Help:** Instant, colorful usage and navigation guides built directly into the binary.
- **Table Engine:** Hardened Unicode grid system with mixed alignment (centered headers), weighted elasticity for width distribution, and dynamic internal cell wrapping.
- **Literal Lines:** Support for unformatted, non-wrapping technical blocks with smart truncation and horizontal overflow indicators.
- **Interactive Pager:** Navigation features including case-insensitive search and SIGWINCH-responsive layout recalculation with zero-flicker buffering.
- **Auto-Sync:** Native `sync` command with atomic swaps and permissions verification to keep your documentation libraries updated from official sources.
- **Modular Design:** An architecture organized into distinct functional sub-systems.

## 🚀 Quick Start

### Build
Requires a C compiler (`clang` or `gcc`) and `make`.
```bash
make
```

### Sync Documentation
```bash
./build/gig sync
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
