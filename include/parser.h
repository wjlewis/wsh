#ifndef __PARSER_H
#define __PARSER_H

#include "lexer.h"

struct Cmd;

struct PipeCmd {
        struct Cmd *lhs;
        struct Cmd *rhs;
};

enum RedirType {
        REDIR_OUT,
        REDIR_IN
};

struct RedirCmd {
        struct Cmd *cmd;
        char *file_name;
        enum RedirType type;
};

enum BuiltinType {
        BUILTIN_CD
};

struct Args {
        int argc;
        char **argv;
};

struct BuiltinCmd {
        enum BuiltinType type;
        struct Args args;
};

struct ExecCmd {
        struct Args args;
};

enum CmdType {
        CMD_PIPE,
        CMD_REDIR,
        CMD_BUILTIN,
        CMD_EXEC,
};

union CmdData {
        struct PipeCmd *pipe;
        struct RedirCmd *redir;
        struct BuiltinCmd *builtin;
        struct ExecCmd *cmd;
};

struct Cmd {
        enum CmdType type;
        union CmdData data;
};

struct Cmd *parse_cmd(struct Lexer *);

#endif
