#include "inmemorydb.h"

const int* inmemorydb_get(const inmemorydb* db, const char* key);
int inmemorydb_put(inmemorydb* db, const char* key, int val);
int inmemorydb_begin_transaction(inmemorydb* db);
int inmemorydb_commit(inmemorydb* db);
int inmemorydb_rollback(inmemorydb* db);