#ifndef INMEMORYDB_H
#define INMEMORYDB_H

#include <stddef.h>

typedef struct inmemorydb_state {
    const char**    keys;
    int*            values;
    size_t          size;
    size_t          capacity;
} inmemorydb_state;

typedef struct inmemorydb {
    struct {
        const char**    keys;
        int*            values;
        size_t          size;
        size_t          capacity;
    } transaction;

    struct {
        size_t*         hash_entries;
        const char**    keys;
        int*            values;
        size_t          size;
        size_t          capacity;
    } committed;
} inmemorydb;

/**
 * Retrieves the value associated with the specified key.
 * 
 * Returns a pointer to the committed value for the key, or NULL if the key does not exist.
 * Uncommitted changes within an active transaction are not visible to this function.
 * Can be called whether or not a transaction is in progress.
 * The returned pointer must not be used to modify the value.
 * 
 * @param db  Pointer to the database instance
 * @param key The key to retrieve
 * @return    Pointer to the value associated with the key, or NULL if not found
 */
const int* inmemorydb_get(const inmemorydb* db, const char* key);

/**
 * Sets the value associated with the specified key.
 * 
 * If the key exists, updates its value. If the key does not exist, creates it.
 * Changes are not committed to the main state until commit() is called.
 * Returns an error code if called when no transaction is in progress.
 * 
 * @param db  Pointer to the database instance
 * @param key The key to set
 * @param val The value to associate with the key
 * @return    0 on success, non-zero error code on failure
 * 
 * @note Internally, multiple put calls to the same key within
 *       the same transaction are not consolidated.
 */
int inmemorydb_put(inmemorydb* db, const char* key, int val);

/**
 * Starts a new transaction.
 * 
 * Only one transaction may be active at a time. All subsequent put() operations
 * will be part of this transaction until either commit() or rollback() is called.
 * Returns an error code if a transaction is already in progress.
 * 
 * @param db Pointer to the database instance
 * @return   0 on success, non-zero error code on failure
 */
int inmemorydb_begin_transaction(inmemorydb* db);

/**
 * Commits the active transaction.
 * 
 * Applies all changes made during the transaction to the main database state,
 * making them visible to future get() calls. Ends the current transaction.
 * Returns an error code if no transaction is in progress.
 * 
 * @param db Pointer to the database instance
 * @return   0 on success, non-zero error code on failure
 */
int inmemorydb_commit(inmemorydb* db);

/**
 * Rolls back the active transaction.
 * 
 * Discards all changes made during the transaction, reverting the database
 * to its state before begin_transaction() was called. Ends the current transaction.
 * Returns an error code if no transaction is in progress.
 * 
 * @param db Pointer to the database instance
 * @return   0 on success, non-zero error code on failure
 */
int inmemorydb_rollback(inmemorydb* db);

#endif // INMEMORYDB_H