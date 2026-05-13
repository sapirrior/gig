# Installation & Environment Setup

`gig` is written in C11/POSIX and can be compiled on Unix-like systems.

## 1. Prerequisites
A C compiler (such as `clang` or `gcc`) and `make` are required.

## 2. Building the Engine
Clone the repository and run the following command in the root directory:

```bash
make
```

This will create a `build/gig` binary. You can move this binary to your `/usr/local/bin` or any directory in your `$PATH`.

## 3. Creating Your First Guide
1. Create a file named `hello.gg`.
2. Add the following content:

```gig
@name    "hello"
@section "1"
---
# NAME
. hello - my first gig guide

# DESCRIPTION
. This is a simple guide to test the engine.

| Column A | Column B
| Data 1   | Data 2
```

3. View it directly using the `view` command:

```bash
./build/gig view hello.gg
```

## 4. Setting Up the Search Path
By default, `gig` looks for guides in:
- Standard Linux: `/usr/share/gig`
- Termux: `/data/data/com.termux/files/usr/share/gig`

Guides must be placed in a subdirectory corresponding to their section (e.g., `.../gig/1/ls.gg`). You can verify your current search path by running:

```bash
./build/gig path
```

## 5. Environment Integration
The engine automatically exports `$GIGPATH` to your environment when running. You can use this for scripts or to quickly navigate to the source of your guides.

We recommend adding the following alias to your `.bashrc` or `.zshrc`:
```bash
alias gig='/path/to/your/build/gig'
```
