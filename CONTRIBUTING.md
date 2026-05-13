# Contributing to gig

Thank you for your interest in contributing to `gig`. To maintain code quality and project consistency, please adhere to the following guidelines.

## Code of Conduct
By participating in this project, you agree to maintain a professional and respectful environment.

## Technical Standards
- **Language:** C11 (POSIX compliant).
- **Standards:** Strictly adhere to POSIX.1-2008 and C11. Avoid non-portable extensions (e.g., `__attribute__((constructor))`) unless absolutely necessary and guarded by macros.
- **Style:** Follow the existing indentation (4 spaces) and naming conventions (`gig_<module>_<action>` for public APIs, `snake_case` for local functions and variables).
- **Modularity:** Adhere to the established separation of concerns. Do not leak pager-specific logic (like `termios` calls) into the parser or layout modules.
- **Objectivity:** Avoid hyperbolic language. Use technical, objective descriptions (e.g., "Golden Responsive Staircase" instead of "the best layout").

## Development Workflow
1.  **Fork and Clone:** Create a local copy for your changes.
2.  **Branching:** Use descriptive branch names (e.g., `fix/parser-error`, `feat/new-trigger`).
3.  **Local Testing:**
    *   Verify your changes against a variety of terminal sizes.
    *   Test with complex guides (e.g., nested formatting, large tables).
    *   Check for memory leaks using `valgrind` or similar tools if you modify the parser or layout logic.
4.  **Linting:** Ensure your code compiles without warnings (`-Wall -Wextra`). We treat warnings as errors.

## Commit Message Convention
We follow the [Conventional Commits](https://www.conventionalcommits.org/) specification. Each commit message must consist of a **type**, an optional **scope**, and a **description**.

### Types
- `feat`: A new feature for the user.
- `fix`: A bug fix.
- `docs`: Documentation changes only.
- `style`: Formatting, missing semi-colons, etc; no code change.
- `refactor`: Refactoring production code.
- `test`: Adding missing tests, refactoring tests; no production code change.
- `chore`: Updating build tasks, package manager configs, etc; no production code change.

### Format
```text
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Example:**
`feat(layout): implement multi-line cell wrapping for tables`

## Pull Request Process
1.  **Preparation:** Ensure your code builds and passes all manual tests.
2.  **Documentation:** Update the relevant documentation in `docs/` if your change introduces new behavior or syntax.
3.  **Description:** Provide a clear description of the changes in the PR. Include screenshots of visual changes if applicable.
4.  **Traceability:** Reference any related issues (e.g., "Closes #123").
5.  **Review:** Once a PR is submitted, it will undergo review. Be prepared to address feedback and iterate on your implementation.
