# TinySql

## Build Instructions
In order to build this project you just need to run these following commands:
1. `cmake build` <br> *(Run this in `root` path of the repository).* <br> &rarr; This will create two executables:
   1. `src/main` &rarr; for running main. REPL like interface.
   2. `tests/unit_tests` &rarr; for running unit tests.
2. `cd build && src/main` for running main.

## Syntax description
This project supports a subset of the sql language:
- `CREATE TABLE <name> (<col> <type>, ...)` — creates a new table with the given columns. Types are `INTEGER` and `TEXT`.
- `INSERT INTO <name> VALUES (...)` — inserts a row. Validate column count and types.
- `SELECT <cols|*> FROM <name>` — prints matching columns for all rows. Support `*` and named columns.
- `SELECT ... WHERE <col> = <value>` — supports both simple and nested conditions.
<br>
&rarr; ***For Example**: `SELECT * FROM t1 WHERE (a = 2 or b = 3) and c = "tal"`*

- `DELETE FROM <name> WHERE <col> = <value>` — removes matching rows.
- `DROP TABLE <name>` — removes a table entirely.

**Meta Commands:**
- `.tables` — lists all existing tables.
- `.quit` — exits the REPL + saves the current tables to `tables.db` in the build directory. <br> The next time you run the main executable, it will pickup from where you left off last time. 

### General Comments on syntax:
1. Keywords can be written both in lowercase and uppercase.
2. When writing a `WHERE` condition on a string value, the value must be enclosed in double quotes (e.g., `"word"`).
3. Only `=` operator is supported in where clauses.
4. To persist tables to disk, use `.quit` instead of closing the terminal.

## Usage Examples
*More examples can be found in the `tests/test_entire_project.cpp` file.*
```sql
CREATE TABLE t1 (a INTEGER, b INTEGER, c TEXT) -- CREATE TABLE Example.
INSERT INTO t1 VALUES (2, 3, "1") -- INSERT Example
INSERT INTO t1 VALUES (4, 5, "2")
INSERT INTO t1 VALUES (4, 5, "2.5")
INSERT INTO t1 VALUES (7, 8, "3")
SELECT a, b, c FROM t1
select a, b, c from t1 -- This is the same as the line above.
select * from t1 where (a = 2 OR a = 7) AND c = "1" -- Example WHERE clause.
```