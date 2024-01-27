#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "alloc.h"

struct Lexer Lexer_new(char *source) {
        struct Lexer l;
        l.pos = source;
        l.peeked = 0;
        return l;
}

char *Token_text(struct Token t) {
        char *str = arena_alloc(sizeof(char) * (t.len + 1));
        strncpy(str, t.start, t.len);
        str[t.len + 1] = '\0';
        return str;
}

int Token_text_cmp(struct Token t, char *text) {
        return t.len == (int) strlen(text) && strncmp(t.start, text, t.len) == 0;
}

static void skip_trivia(struct Lexer *l) {
        while (1) {
                int c = *(l->pos);
                if (c == ' ' || c == '\t') {
                        l->pos++;
                } else {
                        break;
                }
        }
}

static int is_ident_char(char c) {
        return 33 <= c && c <= 126 && c != '(' && c != ')' && c != '<' && c != '>' && c != '|';
}

struct Token read_next(struct Lexer *l) {
        struct Token t;

        skip_trivia(l);

        char *start_pos = l->pos;
        int c = *start_pos;
        if (c == '\0') {
                t.type = TOK_END;
                t.start = start_pos;
                t.len = 0;
                return t;
        }

        (l->pos)++;
        enum TokenType type;
        switch (c) {
                case '|':
                        type = TOK_PIPE;
                        break;
                case '>':
                        type = TOK_RCHEVRON;
                        break;
                case '<':
                        type = TOK_LCHEVRON;
                        break;
                case '(':
                        type = TOK_LPAREN;
                        break;
                case ')':
                        type = TOK_RPAREN;
                        break;
                default:
                        while (is_ident_char(*(l->pos))) {
                                (l->pos)++;
                        }
                        type = TOK_IDENT;
                        break;
        }

        t.type = type;
        t.start = start_pos;
        t.len = l->pos - start_pos;

        return t;
}

struct Token Lexer_peek(struct Lexer *l) {
        if (l->peeked) {
                return l->buffer;
        } else {
                struct Token next = read_next(l);
                l->buffer = next;
                l->peeked = 1;
                return next;
        }
}

struct Token Lexer_pop(struct Lexer *l) {
        if (l->peeked) {
                l->peeked = 0;
                return l->buffer;
        } else {
                return read_next(l);
        }
}
