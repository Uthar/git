/*
 * "git all" builtin command
 */

#include "builtin.h"
#include "config.h"
#include "strbuf.h"
#include "worktree-db.h"

int cmd_all(int argc, const char **argv, const char *prefix)
{

  if (argc < 2) {
    printf("must provide command");
    return 1;
  }

  git_config(git_default_config, NULL);

  char *cmdfmt;
  struct string_list all;
  struct strbuf sb;
  strbuf_init(&sb, 0);
  strbuf_addstr(&sb, "git -C %s ");
  strbuf_join_argv(&sb, argc-1, argv+1, ' '); 
  cmdfmt = strbuf_detach(&sb, 0);
  open_worktree_db(&worktree_db);
  string_list_init_dup(&all);
  all_worktrees_from_db(worktree_db, &all);
  sqlite3_close(worktree_db);

  struct string_list_item *it;
  char *cmd;
  for_each_string_list_item(it, &all) {
    printf("\nworktree: %s\n====================\n", it->string);
    cmd = xstrfmt(cmdfmt, it->string);
    system(cmd);
    free(cmd);
  }

  free(cmdfmt);
  string_list_clear(&all, 1);

  return 0;
}
