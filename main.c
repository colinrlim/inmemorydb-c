#include <stdio.h>
#include "inmemorydb.h"

int main() {
    inmemorydb db = {0};
    const int* val;
    int err;
    
    printf("=== In-Memory Transactional Database Test ===\n\n");
    
    // Test 1: Get on non-existent key
    printf("Test 1: get(\"A\") on non-existent key\n");
    val = inmemorydb_get(&db, "A");
    printf("Result: %s\n", val == NULL ? "NULL (expected)" : "NOT NULL (unexpected)");
    printf("\n");
    
    // Test 2: Put without transaction (should error)
    printf("Test 2: put(\"A\", 5) without transaction\n");
    err = inmemorydb_put(&db, "A", 5);
    printf("Result: %s\n", err != 0 ? "Error (expected)" : "Success (unexpected)");
    printf("\n");
    
    // Test 3: Begin transaction
    printf("Test 3: begin_transaction()\n");
    err = inmemorydb_begin_transaction(&db);
    printf("Result: %s\n", err == 0 ? "Success" : "Error");
    printf("\n");
    
    // Test 4: Put within transaction
    printf("Test 4: put(\"A\", 5) within transaction\n");
    err = inmemorydb_put(&db, "A", 5);
    printf("Result: %s\n", err == 0 ? "Success" : "Error");
    printf("\n");
    
    // Test 5: Get should return NULL (not committed yet)
    printf("Test 5: get(\"A\") before commit\n");
    val = inmemorydb_get(&db, "A");
    printf("Result: %s\n", val == NULL ? "NULL (expected)" : "NOT NULL (unexpected)");
    printf("\n");
    
    // Test 6: Update A's value within transaction
    printf("Test 6: put(\"A\", 6) within same transaction\n");
    err = inmemorydb_put(&db, "A", 6);
    printf("Result: %s\n", err == 0 ? "Success" : "Error");
    printf("\n");
    
    // Test 7: Commit transaction
    printf("Test 7: commit()\n");
    err = inmemorydb_commit(&db);
    printf("Result: %s\n", err == 0 ? "Success" : "Error");
    printf("\n");
    
    // Test 8: Get should return 6 (last committed value)
    printf("Test 8: get(\"A\") after commit\n");
    val = inmemorydb_get(&db, "A");
    if (val != NULL) {
        printf("Result: %d (expected 6)\n", *val);
    } else {
        printf("Result: NULL (unexpected)\n");
    }
    printf("\n");
    
    // Test 9: Commit without transaction (should error)
    printf("Test 9: commit() without transaction\n");
    err = inmemorydb_commit(&db);
    printf("Result: %s\n", err != 0 ? "Error (expected)" : "Success (unexpected)");
    printf("\n");
    
    // Test 10: Rollback without transaction (should error)
    printf("Test 10: rollback() without transaction\n");
    err = inmemorydb_rollback(&db);
    printf("Result: %s\n", err != 0 ? "Error (expected)" : "Success (unexpected)");
    printf("\n");
    
    // Test 11: Get on non-existent key B
    printf("Test 11: get(\"B\") on non-existent key\n");
    val = inmemorydb_get(&db, "B");
    printf("Result: %s\n", val == NULL ? "NULL (expected)" : "NOT NULL (unexpected)");
    printf("\n");
    
    // Test 12: Begin new transaction
    printf("Test 12: begin_transaction()\n");
    err = inmemorydb_begin_transaction(&db);
    printf("Result: %s\n", err == 0 ? "Success" : "Error");
    printf("\n");
    
    // Test 13: Put B within transaction
    printf("Test 13: put(\"B\", 10) within transaction\n");
    err = inmemorydb_put(&db, "B", 10);
    printf("Result: %s\n", err == 0 ? "Success" : "Error");
    printf("\n");
    
    // Test 14: Rollback transaction
    printf("Test 14: rollback()\n");
    err = inmemorydb_rollback(&db);
    printf("Result: %s\n", err == 0 ? "Success" : "Error");
    printf("\n");
    
    // Test 15: Get B should return NULL (rolled back)
    printf("Test 15: get(\"B\") after rollback\n");
    val = inmemorydb_get(&db, "B");
    printf("Result: %s\n", val == NULL ? "NULL (expected)" : "NOT NULL (unexpected)");
    printf("\n");
    
    printf("=== All tests complete ===\n");

    inmemorydb_destroy(&db);
    
    return 0;
}