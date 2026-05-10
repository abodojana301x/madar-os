#include "seh_bridge.h"
#include <kprintf.h>
#include <string.h>

static compat_exception_handler_t g_handler = 0;
static compat_exception_record_t g_last_exception;

void seh_bridge_init(void)
{
    g_handler = 0;
    memset(&g_last_exception, 0, sizeof(g_last_exception));
}

uint32_t seh_map_cpu_exception(uint32_t cpu_vector, uint64_t addr)
{
    switch (cpu_vector) {
        case 0:  return COMPAT_EX_DIVIDE_BY_ZERO;
        case 1:  return COMPAT_EX_BREAKPOINT;
        case 4:  return COMPAT_EX_INTEGER_OVERFLOW;
        case 6:  return COMPAT_EX_ILLEGAL_INSTRUCTION;
        case 11: return COMPAT_EX_DATATYPE_MISALIGNMENT;
        case 12: return COMPAT_EX_STACK_OVERFLOW;
        case 13: return COMPAT_EX_PRIV_INSTRUCTION;
        case 14: return (addr != 0) ? COMPAT_EX_ACCESS_VIOLATION : COMPAT_EX_PAGE_FAULT;
        default: return COMPAT_EX_ILLEGAL_INSTRUCTION;
    }
}

void seh_register_handler(compat_exception_handler_t handler)
{
    g_handler = handler;
}

int seh_raise(uint32_t code, uint64_t addr)
{
    kprintf("[compat-seh] exception=0x%llx addr=0x%llx (%s)\n",
            (uint64_t)code, addr, seh_code_name(code));

    g_last_exception.code = code;
    g_last_exception.address = addr;
    g_last_exception.flags = 0;
    g_last_exception.handled = 0;

    if (g_handler) {
        g_handler(code, addr);
        g_last_exception.handled = 1;
        return 0;
    }
    return -1;
}

int seh_raise_with_record(compat_exception_record_t *rec)
{
    if (!rec) return -1;
    return seh_raise(rec->code, rec->address);
}

const char *seh_code_name(uint32_t code)
{
    switch (code) {
        case COMPAT_EX_DIVIDE_BY_ZERO:        return "DIVIDE_BY_ZERO";
        case COMPAT_EX_ACCESS_VIOLATION:      return "ACCESS_VIOLATION";
        case COMPAT_EX_ILLEGAL_INSTRUCTION:   return "ILLEGAL_INSTRUCTION";
        case COMPAT_EX_STACK_OVERFLOW:        return "STACK_OVERFLOW";
        case COMPAT_EX_INTEGER_OVERFLOW:      return "INTEGER_OVERFLOW";
        case COMPAT_EX_PRIV_INSTRUCTION:      return "PRIV_INSTRUCTION";
        case COMPAT_EX_BREAKPOINT:            return "BREAKPOINT";
        case COMPAT_EX_DATATYPE_MISALIGNMENT: return "DATATYPE_MISALIGNMENT";
        default:                              return "UNKNOWN";
    }
}

void seh_print_last(void)
{
    kprintf("[compat-seh] last exception: code=0x%llx addr=0x%llx flags=%u handled=%u\n",
            (uint64_t)g_last_exception.code, g_last_exception.address,
            g_last_exception.flags, g_last_exception.handled);
}
