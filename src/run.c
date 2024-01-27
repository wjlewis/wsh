#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "parser.h"
#include "run.h"

static void run_cmd(struct Cmd *);

static void run_pipe_cmd(struct PipeCmd *cmd) {
        int p[2];
        pipe(p);

        if (fork() == 0) {
                close(1);
                dup(p[1]);
                close(p[0]);
                close(p[1]);
                run_cmd(cmd->lhs);
        }

        if (fork() == 0) {
                close(0);
                dup(p[0]);
                close(p[0]);
                close(p[1]);
                run_cmd(cmd->rhs);
        }

        close(p[0]);
        close(p[1]);
        wait(0);
        wait(0);
}

static void run_redir_cmd(struct RedirCmd *cmd) {
        switch (cmd->type) {
                case REDIR_OUT:
                        close(1);
                        open(cmd->file_name, O_CREAT | O_WRONLY, 0777);
                        run_cmd(cmd->cmd);
                        break;
                case REDIR_IN:
                        close(0);
                        open(cmd->file_name, O_RDONLY);
                        run_cmd(cmd->cmd);
                        break;
        }
}

static void run_builtin_cmd(struct BuiltinCmd *cmd) {
        switch (cmd->type) {
                case BUILTIN_CD:
                        chdir(cmd->args.argv[0]);
                        return;
                default:
                        return;
        }
}

static void run_exec_cmd(struct ExecCmd *cmd) {
        struct Args args = cmd->args;
        char **argv = args.argv;
        execv(argv[0], argv);
        fprintf(stderr, "run %s failed\n", argv[0]);
}

static void run_cmd(struct Cmd *cmd) {
        switch (cmd->type) {
                case CMD_PIPE:
                        run_pipe_cmd(cmd->data.pipe);
                        break;
                case CMD_REDIR:
                        run_redir_cmd(cmd->data.redir);
                        break;
                case CMD_BUILTIN:
                        run_builtin_cmd(cmd->data.builtin);
                        break;
                case CMD_EXEC:
                        run_exec_cmd(cmd->data.cmd);
                        break;
        }
        exit(0);
}

void run(struct Cmd *cmd) {
        if (cmd->type == CMD_BUILTIN) {
                run_builtin_cmd(cmd->data.builtin);
        } else {
                if (fork() == 0) {
                        run_cmd(cmd);
                }
                wait(0);
        }
}
