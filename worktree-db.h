#ifndef WORKTREE_DB_H
#define WORKTREE_DB_H

#include <sqlite3.h>
#include "string-list.h"

extern sqlite3 *worktree_db;

void open_worktree_db(sqlite3 **db);

void close_worktree_db(sqlite3 *db);

void register_worktree_in_db(sqlite3 *db, const char *worktree);

void for_each_worktree_in_db
  (sqlite3 *db, const char *glob, int invert_glob,
   void (*cb)(const char *worktree, const char **cmd),
   const char **cmd);

#endif // WORKTREE_DB_H
