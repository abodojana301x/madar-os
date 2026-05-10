#include "exception_mapper.h"
#include "seh_bridge.h"
#include <kprintf.h>

uint32_t compat_exception_from_vector(uint32_t vector, uint64_t addr)
{
    return seh_map_cpu_exception(vector, addr);
}

void compat_exception_handler_dispatch(compat_cpu_exception_t *ctx)
{
    if (!ctx) return;
    uint32_t seh_code = seh_map_cpu_exception(ctx->cpu_vector, ctx->address);
    kprintf("[compat-ex] CPU vector=%u addr=0x%llx cr2=0x%llx => SEH code=0x%llx\n",
            ctx->cpu_vector, ctx->address, ctx->cr2_value, (uint64_t)seh_code);
    seh_raise(seh_code, ctx->address);
}
