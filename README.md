# In-Memory Transactional Database

A simple in-memory key-value database implementation in C with transaction support.

## Features

- Key-value storage (string keys, integer values)
- Transaction support with commit/rollback functionality
- ACID-compliant transaction isolation

## Building and Running

### Prerequisites

- GCC or any C compiler
- Make (optional, for using the provided Makefile)

### Compilation

Using GCC directly:
```bash
gcc -o inmemorydb_test inmemorydb.c main.c -Wall -Wextra
```

Or using the provided Makefile:
```bash
make
```

### Running

```bash
./inmemorydb_test
```

The test program will run through all the examples from the assignment specification and display the results.

### Build Scripts

For convenience, platform-specific build scripts are provided:
- **Linux**: `./run.sh` - Cleans, builds, and runs the test program
- **Windows**: `run.bat` - Cleans, builds, and runs the test program

## Usage Example

```c
inmemorydb db;

// Get returns NULL for non-existent keys
const int* val = inmemorydb_get(&db, "A");  // Returns NULL

// Put requires an active transaction
inmemorydb_put(&db, "A", 5);  // Returns error code

// Start a transaction
inmemorydb_begin_transaction(&db);

// Make changes within the transaction
inmemorydb_put(&db, "A", 5);

// Changes are not visible until committed
inmemorydb_get(&db, "A");  // Returns NULL

// Commit the transaction
inmemorydb_commit(&db);

// Now changes are visible
inmemorydb_get(&db, "A");  // Returns pointer to 5
```

## Assignment Improvement Suggestions

TODO