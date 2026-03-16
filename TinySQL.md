# TinySQL – In-Memory SQL Database

**Language:** C++

---

## Overview

Build a minimal SQL database engine that runs entirely in memory. It should accept queries through a command-line REPL, support a handful of SQL statements, and store tables using STL containers. Optionally, persist data to disk between sessions.

This project tests your ability to parse structured input, design data structures for tabular storage, manage memory correctly, and write well-organized C++ code.


## What You're Building

A REPL-based application called `tinysql` that:

1. **Parses SQL statements** — Tokenizes and interprets a small subset of SQL: `CREATE TABLE`, `INSERT INTO`, `SELECT`, `DELETE FROM`, and `DROP TABLE`.
2. **Stores data in memory** — Tables live as STL containers (e.g., `std::vector` of rows, `std::unordered_map` for table lookup). Supports `INTEGER` and `TEXT` column types.
3. **Prints query results** — `SELECT` output is formatted as an aligned, readable table in the terminal.
4. **Persists to disk** *(stretch goal)* — On exit, serializes tables to a file. On startup, loads them back.


## Requirements

### Core (must-have)

- `CREATE TABLE <name> (<col> <type>, ...)` — creates a new table with the given columns. Types are `INTEGER` and `TEXT`.
- `INSERT INTO <name> VALUES (...)` — inserts a row. Validate column count and types.
- `SELECT <cols|*> FROM <name>` — prints matching columns for all rows. Support `*` and named columns.
- `SELECT ... WHERE <col> = <value>` — basic equality filtering on a single column.
- `DELETE FROM <name> WHERE <col> = <value>` — removes matching rows.
- `DROP TABLE <name>` — removes a table entirely.
- `.tables` — lists all existing tables (a meta-command, not SQL).
- `.quit` — exits the REPL.

### Extended (pick at least one)

- **ORDER BY:** `SELECT ... ORDER BY <col> ASC|DESC`.
- **UPDATE:** `UPDATE <name> SET <col> = <value> WHERE <col> = <value>`.
- **Persistence:** Save all tables to a binary or CSV file on `.save`, reload on startup with `.load <file>`.
- **Multi-condition WHERE:** Support `AND`/`OR` in WHERE clauses.
- **Aggregate functions:** `COUNT(*)`, `SUM(<col>)`, `AVG(<col>)`.


## Repo Structure

```
tiny_sql/
├── README.md
├── .gitignore
├── CMakeLists.txt
├── src/
│   ├── main.cpp          # REPL loop, entry point
│   ├── tokenizer.h/.cpp  # Breaks input string into tokens
│   ├── parser.h/.cpp     # Converts tokens into a structured command
│   ├── engine.h/.cpp     # Executes commands against the in-memory store
│   ├── table.h/.cpp      # Table and Row data structures
│   └── printer.h/.cpp    # Formats and prints SELECT results
├── tests/
│   ├── test_tokenizer.cpp
│   ├── test_parser.cpp
│   └── test_engine.cpp
└── docs/
    └── design.md          # Your architecture notes and decisions
```


## Technical Guidance

### Data model

A reasonable starting point:

```cpp
using Value = std::variant<int, std::string>;

struct Column {
    std::string name;
    enum Type { INTEGER, TEXT } type;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
    std::vector<std::vector<Value>> rows;
};
```

Store all tables in a `std::unordered_map<std::string, Table>`. This gives you O(1) table lookup by name.

### Parsing approach

Don't try to build a full SQL parser. A practical approach for this scope:

1. **Tokenizer** — Split the input on whitespace, parentheses, commas, and quotes. Produce a `std::vector<Token>` where each token has a type (keyword, identifier, literal, symbol) and a string value.
2. **Parser** — Walk the token list. The first one or two tokens tell you the statement type (`CREATE TABLE`, `INSERT INTO`, `SELECT`, `DELETE FROM`, `DROP TABLE`). Branch into a dedicated parse function for each type that extracts the relevant fields into a command struct.

Keep parsing and execution completely separate. The parser produces a command object; the engine receives it and acts on the data.

### Error handling

Be deliberate about what happens on bad input. The REPL should never crash. At minimum, handle: unknown table names, column count mismatches on INSERT, type mismatches (inserting text into an INTEGER column), unknown columns in SELECT/WHERE, and malformed syntax. Print a clear error message and return to the prompt.

### Testing with GoogleTest

Use [GoogleTest](https://google.github.io/googletest/) for unit tests. Test each layer independently:

- **Tokenizer tests** — Given a string like `"SELECT * FROM users WHERE id = 5"`, assert the correct token sequence.
- **Parser tests** — Given a token sequence, assert the correct command struct is produced.
- **Engine tests** — Create a table programmatically, insert rows, run a SELECT command, and assert the returned rows.


## What We're Looking For

- **Separation of tokenizing, parsing, and execution** — these should be distinct modules with clear interfaces between them.
- **Correct use of STL containers** — choose the right container for the job and use iterators / algorithms where appropriate.
- **Memory safety** — no leaks, no undefined behavior. Run your tests under AddressSanitizer (`-fsanitize=address`).
- **Robust error handling** — bad queries produce helpful messages, not segfaults.
- **Readable code** — consistent style, meaningful names, header/source separation, brief comments where the "why" isn't obvious.
- **Working tests** — at least tokenizer, parser, and core engine operations should be tested.
- **A README that helps someone build and run your project** — build instructions, usage examples, supported query syntax.


## Reference & Learning

| Topic | Resource |
|---|---|
| C++ standard library | [cppreference.com](https://en.cppreference.com/w/) · [Containers reference](https://en.cppreference.com/w/cpp/container.html) |
| `std::variant` | [cppreference — std::variant](https://en.cppreference.com/w/cpp/utility/variant) |
| Unit testing | [GoogleTest User's Guide](https://google.github.io/googletest/) · [GoogleTest GitHub](https://github.com/google/googletest) |
| Build system | [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) |
| SQL for reference | [SQLite SQL syntax docs](https://www.sqlite.org/lang.html) |
| Inspiration | [Codecrafters — Build Your Own SQLite](https://codecrafters.io/sqlite) (paid, but the concept is worth exploring) |
| C++ video series | [The Cherno — C++ Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98fqE45g3jZA_hLGUrD4bo6_) |
