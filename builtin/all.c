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
int invert_glob = 0;
const char *color = GIT_COLOR_NORMAL;
const char *glob = "*";

void run_in_worktree(const char* worktree, const char** argv) {
  color_fprintf_ln(stdout, color, "%s", worktree);
  if (!argv || !*argv) return;
  struct child_process cmd = CHILD_PROCESS_INIT;
  cmd.dir = worktree;
  cmd.git_cmd = git_cmd;
  strvec_pushv(&cmd.args, argv);
  run_command(&cmd);
}

int cmd_all(int argc, const char **argv, const char *prefix, struct repository *repo)
{
  const char * const usage[] = {
    N_("git all [<options>] [--] <command>..."),
    NULL
  };
  const struct option options[] = {
    OPT_STRING_F('g', "glob", &glob, N_("glob"), N_("filter worktrees by glob"), PARSE_OPT_NONEG),
    OPT_BOOL('i', "invert-glob", &invert_glob, N_("skip by glob instead")),
    OPT_BOOL('\0', "git", &git_cmd, N_("treat as git subcommand")),
    OPT_END(),
  };

  parse_options(argc, argv, prefix, options, usage, 0);
  repo_config(repo, git_default_config, NULL);
  if (want_color(GIT_COLOR_AUTO)) color = GIT_COLOR_BOLD;
  open_worktree_db(&worktree_db);
  for_each_worktree_in_db(worktree_db, glob, invert_glob, run_in_worktree, argv);
  sqlite3_close(worktree_db);
  
  return 0;
}
