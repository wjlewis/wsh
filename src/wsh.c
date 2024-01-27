#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "lexer.h"
#include "parser.h"
#include "alloc.h"
#include "run.h"

#define BUFSIZE 512
static char line_buffer[BUFSIZE];

char *read_line(void) {
        int pos = 0;
        int c;

        while (pos < BUFSIZE) {
                c = getchar();
                if (c == EOF || c == '\n') {
                        line_buffer[pos] = '\0';
                        break;
                } else {
                        line_buffer[pos] = c;
                }
                pos++;
        }

        return line_buffer;
}

void loop(void) {
        char *line;
        while (1) {
                printf("wsh$ ");
                line = read_line();
                if (strlen(line) == 0) {
                        continue;
                }

                struct Lexer l = Lexer_new(line);
                struct Cmd *cmd = parse_cmd(&l);

                if (cmd != NULL) {
                        run(cmd);
                }

                reset_arena();
        }
}

int main(void) {
        loop();
        return EXIT_SUCCESS;
}
