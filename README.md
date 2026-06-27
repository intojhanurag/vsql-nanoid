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
SELECT vsql_nanoid.nanoid();             -- 'k2L9xPqR_3nYmH7wJ-tQ'
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

## Usage as DEFAULT

```sql
CREATE TABLE docs (
    id TEXT PRIMARY KEY DEFAULT (vsql_nanoid.nanoid()),
    body TEXT
);

INSERT INTO docs (body) VALUES ('hello');
-- id is auto-generated: 'V1StGXR8_Z5jdHi6B-myT'
```

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
