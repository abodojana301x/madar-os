#ifndef PANIC_H
#define PANIC_H

#include <types.h>

void panic(const char *file, int line, const char *message);

#define KPANIC(message) panic(__FILE__, __LINE__, (message))
#define KASSERT(expr, message) \
    do { \
        if (!(expr)) { \
            panic(__FILE__, __LINE__, (message)); \
        } \
    } while (0)

#endif
