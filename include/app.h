#ifndef APP_H
#define APP_H

#include "types.h"

// Layer 7 App syscalls
int sys_exec_app(const char* path, char* const argv[], char* const envp[]);
int sys_get_config(const char* key, char* value, size_t size);
int sys_http_request(const char* url, char* response, size_t max_len);
int sys_list_processes(char* buf, size_t size);

// Shell builtins prototypes (for shell.c)
int builtin_ls(const char* path);
int builtin_cd(const char* path);
int builtin_echo(char** args);
int builtin_help(void);

#endif
