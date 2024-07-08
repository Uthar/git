#ifndef WORKTREE_DB_H
#define WORKTREE_DB_H

#include <sqlite3.h>
#include "string-list.h"

extern sqlite3 *worktree_db;

void open_worktree_db(sqlite3 **db);

void close_worktree_db(sqlite3 *db);

void register_worktree_in_db(sqlite3 *db, const char *worktree);

void all_worktrees_from_db(sqlite3 *db, struct string_list *out);

#endif // WORKTREE_DB_H
