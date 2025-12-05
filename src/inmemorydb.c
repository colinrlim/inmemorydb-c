#include "inmemorydb.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INMEMORYDB_ACTIVE_TRANSACTION_REQUIRED      \
        if (!db->transaction.keys) return -1;       \

#define INMEMORYDB_ACTIVE_TRANSACTION_NOT_ALLOWED   \
        if (db->transaction.keys) return -1;        \

/* --- MACRO PARENTS (THE ABSTRACTIONS) --- */

// 1. Accessors: Define HOW to access a member
#define M_DOT(parent, member) parent.member     // for structs  (db->transaction.keys)
#define M_CAT(parent, member) parent##member    // for vars     (new_keys)

// 2. Base Operations: Define WHAT to do (agnostic of access style)
//    AC = Accessor Macro (M_DOT or M_CAT)

// Transfer all fields (pointers + size + capacity)
// Requires variables to follow naming convention (e.g. new_size, new_capacity)
#define _SET_FIELDS(target, src, AC_T, AC_S)                \
    AC_T(target, hash_entries) = AC_S(src, hash_entries);   \
    AC_T(target, keys)         = AC_S(src, keys);           \
    AC_T(target, values)       = AC_S(src, values);         \
    AC_T(target, capacity)     = AC_S(src, capacity);       \
    AC_T(target, size)         = AC_S(src, size);           \

#define _ALLOC_KV(target, cap, AC)                              \
    AC(target, keys)    = calloc((cap), sizeof(const char*));   \
    AC(target, values)  = malloc((cap) * sizeof(int));          \

#define _FREE_KV(target, AC)    \
    free(AC(target, keys));     \
    free(AC(target, values));   \

#define _NULL_KV(target, AC)    \
    AC(target, keys)    = NULL; \
    AC(target, values)  = NULL; \

#define _ALLOC_HKV(target, cap, AC)                             \
    AC(target, hash_entries) = malloc((cap) * sizeof(size_t));  \
    _ALLOC_KV(target, cap, AC)                                  \

#define _FREE_HKV(target, AC)       \
    free(AC(target, hash_entries)); \
    _FREE_KV(target, AC)            \

#define _RESIZE_KV(target, AC, err_code)                                                \
    do {                                                                                \
        size_t _nc              = AC(target, capacity) * 2;                             \
        const char** _nk        = realloc(AC(target, keys), _nc * sizeof(const char*)); \
        if (!_nk) return err_code;                                                      \
        AC(target, keys)        = _nk;                                                  \
        int* _nv                = realloc(AC(target, values), _nc * sizeof(int));       \
        if (!_nv) return err_code;                                                      \
        AC(target, values)      = _nv;                                                  \
        AC(target, capacity)    = _nc;                                                  \
    } while(0)                                                                          \


/* --- MACRO CHILDREN (THE IMPLEMENTATION) --- */

// Updates db->committed from variables (e.g. new_keys, new_size)
#define INMEMORYDB_TABLE_UPDATE(t, v)   _SET_FIELDS(t, v, M_DOT, M_CAT)

// Transaction Helpers (Struct Access, Keys/Values Only)
#define INMEMORYDB_TXN_ALLOC(t, c)  _ALLOC_KV(t, c, M_DOT)
#define INMEMORYDB_TXN_FREE(t)      _FREE_KV(t, M_DOT)
#define INMEMORYDB_TXN_NULL(t)      _NULL_KV(t, M_DOT)
#define INMEMORYDB_TXN_RESIZE(t)    _RESIZE_KV(t, M_DOT, -2)

// Table/Committed Helpers (Struct Access, Includes Hash)
#define INMEMORYDB_TABLE_FREE(t)    _FREE_HKV(t, M_DOT)

// Variable Helpers (Prefix Access, Includes Hash)
#define INMEMORYDB_VAR_ALLOC(p, c)  _ALLOC_HKV(p, c, M_CAT)

/* ------------------------------------------- */

static inline void __attribute__((always_inline)) inmemorydb_end_transaction(
        inmemorydb* const restrict db) {
    INMEMORYDB_TXN_FREE(db->transaction);
    INMEMORYDB_TXN_NULL(db->transaction);
    db->transaction.size        = 0;
    db->transaction.capacity    = 0;
}

static inline size_t __attribute__((always_inline)) inmemorydb_hash(
        const char* key) { // djb2 hash function
    size_t hash = 5381;
    while (*key)
        hash = ((hash << 5) + hash) + *key++; // hash * 33 + c
    return hash;
}

// Returns 1 if a NEW key was inserted, 0 if an EXISTING key was updated
static inline size_t __attribute__((always_inline)) inmemorydb_hash_insert(
        size_t* hash_entries, const char** keys, int* values, size_t capacity,
        const char* key, int val)   {
    size_t hash     = inmemorydb_hash(key);
    size_t index    = hash % capacity;

    while (keys[index] != NULL) {
        if (strcmp(keys[index], key) == 0) {
            values[index]       = val;
            hash_entries[index] = hash;
            return 0; // UPDATE
        }
        index = (index + 1) % capacity;
    }

    keys[index]         = key;
    values[index]       = val;
    hash_entries[index] = hash;
    return 1; // INSERT
}




const int* inmemorydb_get(const inmemorydb* db, const char* key) { // TODO
    if (!db->committed.keys || db->committed.capacity == 0) return NULL;

    size_t hash         = inmemorydb_hash(key);
    size_t index        = hash % db->committed.capacity;
    size_t start_index  = index;

    // Hash collision
    while (db->committed.keys[index] != NULL) {
        if (db->committed.hash_entries[index] == hash
                && strcmp(db->committed.keys[index], key) == 0)
            return &db->committed.values[index];
        index = (index + 1) % db->committed.capacity;
        if (index == start_index) break; 
    }
    return NULL;
}

int inmemorydb_put(inmemorydb* db, const char* key, int val) {
    INMEMORYDB_ACTIVE_TRANSACTION_REQUIRED
    
    if (db->transaction.size == db->transaction.capacity)
        INMEMORYDB_TXN_RESIZE(db->transaction);

    db->transaction.keys[db->transaction.size]      = key;
    db->transaction.values[db->transaction.size]    = val;
    db->transaction.size++;
    return 0;
}

int inmemorydb_begin_transaction(inmemorydb* db) {
    INMEMORYDB_ACTIVE_TRANSACTION_NOT_ALLOWED
    db->transaction.size        = 0;
    db->transaction.capacity    = 8; 
    
    INMEMORYDB_TXN_ALLOC(db->transaction, db->transaction.capacity);
    
    if (!db->transaction.keys) return -2;
    return 0;
}

int inmemorydb_commit(inmemorydb* db) { // TODO
    INMEMORYDB_ACTIVE_TRANSACTION_REQUIRED
    size_t required_size    = db->committed.size + db->transaction.size;
    size_t new_capacity     = !db->committed.capacity ? 16 : db->committed.capacity;
    
    while (new_capacity * 0.75 < required_size)
        new_capacity *= 2;

    if (new_capacity > db->committed.capacity) {
        size_t          new_size            = 0;
        size_t*         new_hash_entries;
        const char**    new_keys;
        int*            new_values;

        INMEMORYDB_VAR_ALLOC(new_, new_capacity);
        if (!new_keys) return -1;

        // Rebuild hash table
        for (size_t i = 0; i < db->committed.capacity; i++) {
            if (!db->committed.keys || !db->committed.keys[i]) continue;
            new_size += inmemorydb_hash_insert(
                    new_hash_entries, new_keys, new_values, new_capacity, 
                    db->committed.keys[i], db->committed.values[i]);
        }
        // Commit transaction
        for (size_t i = 0; i < db->transaction.size; i++)
            new_size += inmemorydb_hash_insert(
                    new_hash_entries, new_keys, new_values, new_capacity, 
                    db->transaction.keys[i], db->transaction.values[i]);

        if (db->committed.keys)
            INMEMORYDB_TABLE_FREE(db->committed); 
        INMEMORYDB_TABLE_UPDATE(db->committed, new_);
    } else {
        size_t new_size = db->committed.size;
        for (size_t i = 0; i < db->transaction.size; i++)
            new_size += inmemorydb_hash_insert(
                    db->committed.hash_entries, db->committed.keys, db->committed.values, 
                    db->committed.capacity, db->transaction.keys[i], db->transaction.values[i]);
        db->committed.size = new_size;
    }

    inmemorydb_end_transaction(db);
    return 0;
}

int inmemorydb_rollback(inmemorydb* db) {
    INMEMORYDB_ACTIVE_TRANSACTION_REQUIRED
    inmemorydb_end_transaction(db);
    return 0;
}