# Contributing to gig

Thank you for your interest in contributing to `gig`. To maintain code quality and project consistency, please adhere to the following guidelines.

## Code of Conduct
By participating in this project, you agree to maintain a professional and respectful environment.

## Technical Standards
- **Language:** C11 (POSIX compliant).
- **Style:** Follow the existing indentation and naming conventions (snake_case for functions and variables).
- **Modularity:** Ensure new functionality is placed in the appropriate module (`parser/`, `layout/`, `pager/`, etc.).
- **Objectivity:** Avoid hyperbolic language in documentation and code comments. Use technical, objective descriptions (e.g., "72-character viewport boundary" instead of "Golden Ratio").

## Development Workflow
1.  **Fork and Clone:** Create a local copy for your changes.
2.  **Branching:** Use descriptive branch names (e.g., `fix/parser-error`, `feat/new-trigger`).
3.  **Testing:** Verify your changes against `test.gg` and create new test cases if adding features.
4.  **Linting:** Ensure your code compiles without warnings (`-Wall -Wextra`).

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
1.  Ensure your code builds and passes all tests.
2.  Update the relevant documentation in `docs/` if your change introduces new behavior.
3.  Provide a clear description of the changes in the PR.
4.  Reference any related issues (e.g., "Closes #123").
