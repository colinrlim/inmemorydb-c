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

This assignment could be enhanced in several ways for future offerings.

First, the specification should clarify how errors should be handled in languages without exceptions (return codes, error objects, etc.), and the desired interface specifications should be more detailed in general. Since I used C, I decided to substitute exceptions for return error codes as convention in production C code. In practice, this meant that I had to deviate from the example interface and have most of the methods return "int" instead of "void". Alongside the issue with exceptions, in the context of the "get" method, C cannot return either an int value or a NULL value with a single variable type.

Second, adding support for transaction history with the ability to roll back to any previous committed state (similar to Git versioning) would make it more challenging and realistic. My data structure for transactions was actually made with the Git "diff" as inspiration.

Finally, it would be great if a specific data structure time complexity was required. Even if many students will opt to use a prebuilt hash table, a specified time complexity would make the assignment more interesting for those who choose to roll their own. Personally, I very nearly decided to just use an unsorted array instead of a hash table.