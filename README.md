# VillageSQL Nanoid Extension

Port of [Nano ID](https://github.com/ai/nanoid) to VillageSQL. Generates short, URL-safe, uniformly random identifiers — the default replacement for UUIDs when 21 characters beats 36.

## Functions

### nanoid

```sql
vsql_nanoid.nanoid() -> TEXT
```

Returns a 21-character random ID using the default alphabet (`A-Za-z0-9_-`), providing ~126 bits of entropy.

```sql
SELECT vsql_nanoid.nanoid();             -- 'V1StGXR8_Z5jdHi6B-myT'
SELECT vsql_nanoid.nanoid();             -- 'k2L9xPqR_3nYmH7wJ-tQa'
```

### nanoid_with

```sql
vsql_nanoid.nanoid_with(size INT [, alphabet TEXT]) -> TEXT
```

Returns a random ID with custom length and optionally a custom alphabet.

```sql
SELECT vsql_nanoid.nanoid_with(10);                  -- 'IRFa-VaY2b'
SELECT vsql_nanoid.nanoid_with(8, '0123456789');      -- '48293017'
SELECT vsql_nanoid.nanoid_with(6, 'ABCDEF');          -- 'BCAFDE'
```

## Usage at INSERT time

```sql
CREATE TABLE docs (id TEXT PRIMARY KEY, body TEXT);

INSERT INTO docs (id, body) VALUES (vsql_nanoid.nanoid(), 'hello');
INSERT INTO docs (id, body) VALUES (vsql_nanoid.nanoid(), 'world');
```

Note: `DEFAULT (nanoid())` is not supported because MySQL disallows
non-deterministic functions in column defaults.

## Installation

```sql
INSTALL EXTENSION vsql_nanoid;
```

## Building from source

```bash
export VillageSQL_BUILD_DIR=/path/to/villagesql/build
./build.sh
```

Requires OpenSSL for cryptographically secure random number generation.

## License

GPLv2 — see [LICENSE](LICENSE) for details.
