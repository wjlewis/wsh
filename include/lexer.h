#ifndef __LEXER_H
#define __LEXER_H

enum TokenType {
        TOK_PIPE,
        TOK_RCHEVRON,
        TOK_LCHEVRON,
        TOK_LPAREN,
        TOK_RPAREN,
        TOK_IDENT,
        TOK_ERROR,
        TOK_END
};

struct Token {
        enum TokenType type;
        char *start;
        int len;
};

/*
 * Our lexer "streams" tokens from the source string without allocating.
 * Peeking the next token is supported by `buffer`, which may be NULL, and
 * `peeked`, which indicates if this is the case.
 */
struct Lexer {
        char *pos;
        struct Token buffer;
        int peeked;
};

struct Lexer Lexer_new(char *source);

struct Token Lexer_peek(struct Lexer *);

struct Token Lexer_pop(struct Lexer *);

/*
 * Returns the text of this particular token, allocating the string in the
 * arena. Used to construct exec commands and their argument arrays.
 */
char *Token_text(struct Token);

int Token_text_cmp(struct Token, char *text);

#endif
