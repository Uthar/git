#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <libgen.h>
#include <sys/stat.h>
#include "worktree-db.h"
#include "path.h"
#include "string-list.h"

void mkdirp(char* dir) {
  struct string_list dirs;
  string_list_init_dup(&dirs);
  char *up = dir;
  while (strcmp(up, "/")) {
    string_list_append(&dirs, dir);
    up = dirname(up);
  }
  for (size_t n = dirs.nr; n; n--)
    mkdir(dirs.items[n].string,0777);
  string_list_clear(&dirs, 1);
}

sqlite3 *worktree_db;

void open_worktree_db(sqlite3 **db) {
  char *conf_dir = xdg_config_home("");
  char *git_db = xdg_config_home("git.db");
  mkdirp(conf_dir);
  free(conf_dir);
  sqlite3_open(git_db, db);
  free(git_db);
  sqlite3_exec(*db,
               "create table if not exists worktrees("
               "path unique not null,"
               "created default (unixepoch('now','subsec'))"
               ");",
               NULL, NULL, NULL);
  
}

void close_worktree_db(sqlite3 *db) {
  sqlite3_close(db);
}

void register_worktree_in_db(sqlite3 *db, const char *worktree) {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, "insert or ignore into worktrees(path) values (?);", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, worktree, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void delete_worktree_from_db(sqlite3 *db, const char *worktree) {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, "delete from worktrees where path = ?;", -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, worktree, -1, SQLITE_STATIC);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void all_worktrees_from_db(sqlite3 *db, struct string_list *out) {
  sqlite3_stmt *stmt;
  struct stat st;
  sqlite3_prepare_v2(db, "select * from worktrees;", -1, &stmt, NULL);
  while (SQLITE_ROW == sqlite3_step(stmt)) {
    char *worktree = (char *)sqlite3_column_blob(stmt, 0);
    if (!stat(worktree, &st))
      string_list_append(out, worktree);
    else
      delete_worktree_from_db(db, worktree);
      
  }
  sqlite3_finalize(stmt);
}
