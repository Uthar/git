#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include "path.h"
#include "string-list.h"
#include "worktree-db.h"

// na podstawie openbsd, ale omija pathname-name, tzn. wymaga kończącego '/'
void mkdirs(char* path) {
  for(char *dir = path;;) {
    dir += strspn(dir, "/");
    dir += strcspn(dir, "/");
    if (*dir == '\0') break;
    *dir = '\0';
    mkdir(path,0700);
    *dir = '/';
  }
}

sqlite3 *worktree_db = NULL;
char *db_path = NULL;

void open_worktree_db(sqlite3 **db) {
  struct stat st;
  if (!db_path) db_path = xdg_state_home("git.db");
  if (!db_path) return;
  if (stat(db_path,&st)) mkdirs(db_path);
  sqlite3_open(db_path, db);
  sqlite3_exec(*db,
    "create table if not exists worktrees("
    "path unique not null,"
    "created default (unixepoch('now','subsec'))"
    ");",
    NULL, NULL, NULL
  );
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

void for_each_worktree_in_db
  (sqlite3 *db, const char * glob, int invert_glob,
   void (*cb)(const char* worktree, const char **cmd),
   const char **cmd) {
  sqlite3_stmt *stmt;
  struct stat st;
  const char *sql;
  if (invert_glob)
    sql = "select * from worktrees where path not glob ?;";
  else
    sql = "select * from worktrees where path glob ?;";
  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, glob, -1, SQLITE_STATIC);
  while (SQLITE_ROW == sqlite3_step(stmt)) {
    char *worktree = (char *)sqlite3_column_blob(stmt, 0);
    if (!stat(worktree, &st))
      cb(worktree, cmd);
    else
      delete_worktree_from_db(db, worktree);
  }
  sqlite3_finalize(stmt);  
}
