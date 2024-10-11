/*
 * "git all" builtin command
 */

#include "builtin.h"
#include "config.h"
#include "color.h"
#include "strbuf.h"
#include "strvec.h"
#include "parse-options.h"
#include "worktree-db.h"
#include "run-command.h"

int git_cmd = 1;

void run_in_worktree(const char* worktree, const char** argv) {
  struct child_process cmd = CHILD_PROCESS_INIT;
  cmd.dir = worktree;
  cmd.git_cmd = git_cmd;
  strvec_pushv(&cmd.args, argv);
  color_fprintf_ln(stdout, GIT_COLOR_BOLD, "%s: %s", N_("worktree"), worktree);
  run_command(&cmd);
  printf("\n\n");
}

int cmd_all(int argc, const char **argv, const char *prefix)
{

  const char *glob = "*";
  int invert_glob = 0;
  const char * const usage[] = {
    N_("git all [<options>] [--] <command>..."),
    NULL
  };
  const struct option options[] = {
    OPT_STRING_F('g', "glob", &glob, N_("glob"), N_("filter worktrees by glob"),
                 PARSE_OPT_NONEG),
    OPT_BOOL('i', "invert-glob", &invert_glob, N_("skip by glob instead")),
    OPT_BOOL('\0', "git", &git_cmd, N_("treat as git subcommand")),
    OPT_END(),
  };

  parse_options(argc, argv, prefix, options, usage, 0);
  git_config(git_default_config, NULL);
  open_worktree_db(&worktree_db);
  for_each_worktree_in_db(worktree_db, glob, invert_glob, run_in_worktree, argv);
  sqlite3_close(worktree_db);
  
  return 0;
}
