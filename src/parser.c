#include "lexer.h"
#include "parser.h"
#include "alloc.h"

static struct Args parse_args(struct Lexer *l) {
        // Start with enough space for 4 args, and reallocate (doubling the
        // allocation size) as necessary.
        int n_alloc = 4;
        char **argv = arena_alloc(sizeof(char *) * n_alloc);

        int argc = 0;

        while (Lexer_peek(l).type == TOK_IDENT) {
                argv[argc] = Token_text(Lexer_pop(l));
                argc++;
        
                if (argc == n_alloc) {
                        n_alloc *= 2;
                        argv = arena_realloc(argv, argc * sizeof(char *), n_alloc * sizeof(char *));
                }
        }

        // Add a null pointer to the end.
        if (argc == n_alloc) {
                argv = arena_realloc(argv, argc * sizeof(char *), (argc + 1) * sizeof(char *));
        }
        argv[argc] = 0;

        struct Args args;
        args.argc = argc;
        args.argv = argv;

        return args;
}

static struct Cmd *parse_atomic_cmd(struct Lexer *l) {
        struct Token peek = Lexer_peek(l);
        switch (peek.type) {
                case TOK_LPAREN: {
                        Lexer_pop(l);
                        struct Cmd *inner = parse_cmd(l);
                        if (Lexer_peek(l).type != TOK_RPAREN) {
                                return NULL;
                        }
                        Lexer_pop(l);
                        return inner;
                }
                case TOK_IDENT: {
                        if (Token_text_cmp(peek, "cd")) {
                                Lexer_pop(l);
                                struct Args args = parse_args(l);

                                struct BuiltinCmd *builtin = arena_alloc(sizeof(*builtin));
                                builtin->type = BUILTIN_CD;
                                builtin->args = args;
                                
                                struct Cmd *cmd = arena_alloc(sizeof(*cmd));
                                cmd->type = CMD_BUILTIN;
                                cmd->data.builtin = builtin;

                                return cmd;
                        } else {
                                struct Args args = parse_args(l);
                                struct ExecCmd *exec_cmd = arena_alloc(sizeof(*exec_cmd));
                                exec_cmd->args = args;
                        
                                struct Cmd *cmd = arena_alloc(sizeof(*cmd));
                                cmd->type = CMD_EXEC;
                                cmd->data.cmd = exec_cmd;
                        
                                return cmd;
                        }
                }
                default:
                        return NULL;
        }
}

static char *parse_filename(struct Lexer *l) {
        struct Token peek = Lexer_peek(l);
        if (peek.type == TOK_IDENT) {
                return Token_text(Lexer_pop(l));
        } else {
                return NULL;
        }
}

static struct Cmd *parse_redir_cmd(struct Lexer *l) {
        struct Cmd *cmd = parse_atomic_cmd(l);
        if (cmd == NULL) {
                return NULL;
        }

        struct Token peek = Lexer_peek(l);
        if (peek.type == TOK_LCHEVRON || peek.type == TOK_RCHEVRON) {
                Lexer_pop(l);
                char *file_name = parse_filename(l);
                if (file_name == NULL) {
                        return NULL;
                }

                struct RedirCmd *redir = arena_alloc(sizeof(*redir));
                redir->type = peek.type == TOK_LCHEVRON ? REDIR_IN : REDIR_OUT;
                redir->cmd = cmd;
                redir->file_name = file_name;

                struct Cmd *new_cmd = arena_alloc(sizeof(*new_cmd));
                new_cmd->type = CMD_REDIR;
                new_cmd->data.redir = redir;

                return new_cmd;
        }

        return cmd;
}

static struct Cmd *parse_pipe_cmd(struct Lexer *l) {
        struct Cmd *cmd = parse_redir_cmd(l);
        if (cmd == NULL) {
                return NULL;
        }

        while (Lexer_peek(l).type == TOK_PIPE) {
                Lexer_pop(l);
                struct Cmd *rhs = parse_redir_cmd(l);

                struct PipeCmd *pipe = arena_alloc(sizeof(*pipe));
                pipe->lhs = cmd;
                pipe->rhs = rhs;

                struct Cmd *new_cmd = arena_alloc(sizeof(*new_cmd));
                new_cmd->type = CMD_PIPE;
                new_cmd->data.pipe = pipe;

                cmd = new_cmd;
        }

        return cmd;
}

struct Cmd *parse_cmd(struct Lexer *l) {
        return parse_pipe_cmd(l);
}
