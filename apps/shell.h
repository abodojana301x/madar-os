#ifndef SHELL_H
#define SHELL_H

#include "types.h"

#define MAX_CMD_LEN 256
#define MAX_ARGS 32
#define MAX_PATH_LEN 128

typedef enum { LANG_LINUX, LANG_ENGLISH, LANG_ARABIC } lang_t;

typedef struct {
    char command[MAX_CMD_LEN];
    char *argv[MAX_ARGS];
    int argc;
    int background;
    int has_pipe;
    int has_redirect;
} shell_ast_t;

void shell_init(void);
void shell_run(void);
int shell_execute_line(const char *line);
int shell_selftest(void);
lang_t detect_lang(const char* input);
int shell_parse_line(char *line, shell_ast_t *ast);

#endif
