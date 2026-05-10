/*
 * Layer: 7 (User Applications) - Multi-language shell
 * File: shell.c
 * Purpose: Tiny POSIX-like REPL with Linux, simple English, and Arabic command aliases.
 */

#include "shell.h"
#include "kprintf.h"
#include "string.h"
#include "app.h"
#include "serial.h"

static char cwd[MAX_PATH_LEN] = "/";
static char ps1[32] = "madar$ ";
static char cmd_buf[MAX_CMD_LEN];
static lang_t current_lang = LANG_LINUX;

int sys_exec_app(const char* path, char* const argv[], char* const envp[]) {
    (void)argv;
    (void)envp;
    kprintf("exec: %s not found (user-space loader pending)\n", path ? path : "(null)");
    return -1;
}

static int shell_readline(char *buf, int max_len) {
    int i = 0;
    if (!buf || max_len <= 1) return -1;
    while (i < max_len - 1) {
        while (!serial_received()) { }
        char c = serial_read();
        if (c == '\r' || c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

static void safe_copy(char *dst, const char *src, int max_len) {
    if (!dst || max_len <= 0) return;
    if (!src) src = "";
    strncpy(dst, src, (size_t)max_len - 1);
    dst[max_len - 1] = '\0';
}

static void normalize_command(const char *input, char *out, int out_len) {
    safe_copy(out, input, out_len);
    current_lang = detect_lang(input);

    if (current_lang == LANG_ENGLISH) {
        if (strncmp(input, "show files", 10) == 0) {
            const char *path = strstr(input, " in ");
            if (path) {
                safe_copy(out, "ls ", out_len);
                strncat(out, path + 4, (size_t)out_len - strlen(out) - 1);
            } else {
                safe_copy(out, "ls", out_len);
            }
        } else if (strncmp(input, "print ", 6) == 0) {
            safe_copy(out, "echo ", out_len);
            strncat(out, input + 6, (size_t)out_len - strlen(out) - 1);
        }
    } else if (current_lang == LANG_ARABIC) {
        if (strncmp(input, "اعرض ملفات", strlen("اعرض ملفات")) == 0) {
            const char *path = strrchr(input, ' ');
            safe_copy(out, "ls", out_len);
            if (path && *(path + 1)) {
                strncat(out, " ", (size_t)out_len - strlen(out) - 1);
                strncat(out, path + 1, (size_t)out_len - strlen(out) - 1);
            }
        } else if (strncmp(input, "اطبع ", strlen("اطبع ")) == 0) {
            safe_copy(out, "echo ", out_len);
            strncat(out, input + strlen("اطبع "), (size_t)out_len - strlen(out) - 1);
        }
    }
}

int shell_parse_line(char *line, shell_ast_t *ast) {
    if (!line || !ast) return -1;
    memset(ast, 0, sizeof(shell_ast_t));
    safe_copy(ast->command, line, MAX_CMD_LEN);

    char *p = ast->command;
    while (*p && ast->argc < MAX_ARGS - 1) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        if (*p == '&') {
            ast->background = 1;
            *p++ = '\0';
            continue;
        }
        if (*p == '|') ast->has_pipe = 1;
        if (*p == '>' || *p == '<') ast->has_redirect = 1;
        if (*p == '|' || *p == '>' || *p == '<') {
            p++;
            continue;
        }
        ast->argv[ast->argc++] = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '|' && *p != '>' && *p != '<' && *p != '&') p++;
        if (*p) *p++ = '\0';
    }
    ast->argv[ast->argc] = NULL;
    return ast->argc;
}

static int execute_ast(shell_ast_t *ast) {
    if (!ast || !ast->argv[0]) return 0;
    if (ast->has_pipe) kprintf("shell: pipe parsed (serial demo executes left command)\n");
    if (ast->has_redirect) kprintf("shell: redirection parsed (VFS sink pending)\n");
    if (ast->background) kprintf("shell: background job accepted\n");

    if (strcmp(ast->argv[0], "ls") == 0 || strcmp(ast->argv[0], "اعرض") == 0) {
        const char *path = NULL;
        for (int i = 1; i < ast->argc; i++) {
            if (ast->argv[i][0] != '-') {
                path = ast->argv[i];
                break;
            }
        }
        return builtin_ls(path);
    }
    if (strcmp(ast->argv[0], "cd") == 0) return builtin_cd(ast->argv[1]);
    if (strcmp(ast->argv[0], "pwd") == 0) { kprintf("%s\n", cwd); return 0; }
    if (strcmp(ast->argv[0], "echo") == 0) return builtin_echo(ast->argv);
    if (strcmp(ast->argv[0], "help") == 0) return builtin_help();
    return sys_exec_app(ast->argv[0], ast->argv, NULL);
}

int shell_execute_line(const char *line) {
    char normalized[MAX_CMD_LEN];
    shell_ast_t ast;
    normalize_command(line, normalized, MAX_CMD_LEN);
    if (shell_parse_line(normalized, &ast) <= 0) return 0;
    return execute_ast(&ast);
}

void shell_run(void) {
    kprintf("Madar MultiLang Shell v0.1\n");
    while (1) {
        serial_puts(ps1);
        if (shell_readline(cmd_buf, MAX_CMD_LEN) <= 0) continue;
        shell_execute_line(cmd_buf);
    }
}

int shell_selftest(void) {
    kprintf("Testing MultiLang Shell...\n");
    if (shell_execute_line("ls -l /home | grep .txt > out.txt") != 0) return -1;
    if (shell_execute_line("show files in /home") != 0) return -1;
    if (shell_execute_line("اعرض ملفات /home") != 0) return -1;
    kprintf("MultiLang Shell PASSED\n");
    return 0;
}

void shell_init(void) {
    safe_copy(cwd, "/", MAX_PATH_LEN);
    safe_copy(ps1, "user@madar:/$ ", sizeof(ps1));
    kprintf("Shell initialized\n");
}
