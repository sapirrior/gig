# Installation & Environment Setup

`gig` is designed for documentation retrieval on POSIX-compliant systems. It has zero external dependencies beyond a standard C library.

## 1. Prerequisites
- **Compiler:** `clang` (recommended) or `gcc`.
- **Standards:** Requires C11 and POSIX.1-2008 support.
- **Build Tool:** `make`.

## 2. Building the Engine
To compile the engine, run:
```bash
make
```
The binary will be generated at `build/gig`.

### Installation
To install `gig` system-wide:
```bash
sudo cp build/gig /usr/local/bin/
```

## 3. The Guide Search Path
`gig` uses a tiered search system to locate guides.

### Default Paths
- **Termux:** `/data/data/com.termux/files/usr/share/gig/`
- **Linux/BSD:** `/usr/share/gig/`

### Directory Structure
Guides must be placed in subdirectories named after their section:
```text
/usr/share/gig/
├── 1/           # User Commands
│   ├── gig.gg
│   └── ls.gg
├── 7/           # Miscellaneous
│   └── help.gg
└── n/           # New / Local
    └── test.gg
```

### Section Reference & Search Order
`gig` follows the standard manual section categorization for organizing guides:

- **1**: **User Commands** (Executable programs or shell commands)
- **2**: **System Calls** (Functions provided by the kernel)
- **3**: **Library Functions** (Functions within system libraries)
- **4**: **Special Files** (Device files usually found in `/dev`)
- **5**: **File Formats** (File formats and conventions, e.g., `.gg` specs)
- **6**: **Games** (Games and demonstrations)
- **7**: **Miscellaneous** (Overviews, specifications, and macro packages)
- **8**: **System Administration** (Commands for root/administrators)
- **9**: **Kernel Routines** (Non-standard kernel internal functions)
- **n**: **New / Pending** (Newly drafted or pending guides)
- **l**: **Local** (Project-specific or local documentation)

### Tiered Search Order
When a guide is requested without a specific section (e.g., `gig ls`), the engine performs a tiered search across sections in the following priority order to resolve the name:

`1` → `n` → `l` → `8` → `3` → `2` → `5` → `4` → `9` → `6` → `7`

## 4. Operational Modes

### A. Manual Mode (`gig [section] <name>`)
Searches the system paths for a guide. If no section is provided, it performs a **Tiered Search** (see above) to find the best match.
```bash
gig ls
gig 7 help
```

### B. View Mode (`gig view <file.gg>`)
Directly opens a local file without searching the system path. Useful for development.
```bash
gig view ./my_notes.gg
```

### C. Helper Commands
- `gig sync`: Synchronizes documentation from the official repository using a native atomic swap logic.
- `gig path`: Displays the current internal search path.
- `gig sections`: Shows the list of valid manual sections.
- `gig version`: Displays the current engine version and build standard.
- `gig help`: Displays the interactive colorful help guide.

## 5. Environment Integration
The engine exports a `$GIGPATH` environment variable to its sub-processes. 

### Recommended Alias
Add this to your `.bashrc`, `.zshrc`, or `.profile` for quick access:
```bash
alias gig='/usr/local/bin/gig'
```

## 6. Troubleshooting
- **Garbled Unicode:** Ensure your terminal emulator supports UTF-8 and has a font that includes Box-Drawing characters (e.g., Fira Code, JetBrains Mono).
- **No Colors:** `gig` uses ANSI 256-color escapes. If colors aren't showing, check your `$TERM` variable (e.g., `export TERM=xterm-256color`).
- **Layout Issues:** If the layout looks broken, ensure your terminal reports its size correctly (run `stty size`).
