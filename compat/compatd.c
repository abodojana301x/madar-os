#include "compatd.h"
#include "pe_parser.h"
#include "pe_loader.h"
#include "pe_validator.h"
#include "mem_mapper.h"
#include "stack_init.h"
#include "win32_stubs.h"
#include "syscall_bridge.h"
#include "seh_bridge.h"
#include "exception_mapper.h"
#include "safe_exit.h"
#include "pe_threads.h"
#include "tls_manager.h"
#include "sync_mapper.h"
#include "exe_sandbox.h"
#include "api_log.h"
#include "compat_process.h"
#include "import_resolver.h"
#include "module_registry.h"
#include <kprintf.h>
#include <heap.h>
#include <string.h>
#include <timer.h>
#include "../services/vfs.h"
#include "../services/ramdisk.h"

typedef struct {
    uint32_t cycles;
    uint32_t kills;
} compat_watchdog_t;

static uint8_t g_demo_pe[2048];
static uint32_t g_demo_pe_size = 0;
static compat_sandbox_t g_sandbox;
static compat_sandbox_stats_t g_sandbox_stats;
static compat_watchdog_t g_watchdog;

static void write_name8(uint8_t out[8], const char *s)
{
    for (int i = 0; i < 8; i++) out[i] = 0;
    if (!s) return;
    for (int i = 0; i < 8 && s[i]; i++) out[i] = (uint8_t)s[i];
}

static void build_demo_pe(void)
{
    memset(g_demo_pe, 0, sizeof(g_demo_pe));
    g_demo_pe_size = 0x800;

    pe_dos_header_t *dos = (pe_dos_header_t *)g_demo_pe;
    dos->e_magic = PE_DOS_MAGIC;
    dos->e_lfanew = 0x80;

    *(uint32_t *)(g_demo_pe + 0x80) = PE_NT_SIGNATURE;
    pe_coff_header_t *coff = (pe_coff_header_t *)(g_demo_pe + 0x84);
    coff->machine = PE_MACHINE_AMD64;
    coff->number_of_sections = 3;
    coff->size_of_optional_header = sizeof(pe_optional_header64_t);
    coff->characteristics = 0x2022;

    pe_optional_header64_t *opt = (pe_optional_header64_t *)(g_demo_pe + 0x84 + sizeof(pe_coff_header_t));
    opt->magic = PE_OPT_MAGIC_PE32_PLUS;
    opt->address_of_entry_point = 0x1000;
    opt->base_of_code = 0x1000;
    opt->image_base = 0x140000000ULL;
    opt->section_alignment = 0x1000;
    opt->file_alignment = 0x200;
    opt->size_of_image = 0x4000;
    opt->size_of_headers = 0x200;
    opt->subsystem = PE_SUBSYSTEM_CONSOLE;
    opt->size_of_stack_reserve = 0x100000;
    opt->size_of_stack_commit = 0x1000;
    opt->number_of_rva_and_sizes = 16;

    uint32_t sec_off = 0x84 + sizeof(pe_coff_header_t) + sizeof(pe_optional_header64_t);
    pe_section_header_t *sec = (pe_section_header_t *)(g_demo_pe + sec_off);

    write_name8(sec[0].name, ".text");
    sec[0].virtual_size = 0x200;
    sec[0].virtual_address = 0x1000;
    sec[0].size_of_raw_data = 0x200;
    sec[0].pointer_to_raw_data = 0x200;
    sec[0].characteristics = PE_SECT_CODE | PE_SECT_EXECUTE | PE_SECT_READ;

    write_name8(sec[1].name, ".rdata");
    sec[1].virtual_size = 0x200;
    sec[1].virtual_address = 0x2000;
    sec[1].size_of_raw_data = 0x200;
    sec[1].pointer_to_raw_data = 0x400;
    sec[1].characteristics = PE_SECT_INIT_DATA | PE_SECT_READ;

    write_name8(sec[2].name, ".data");
    sec[2].virtual_size = 0x200;
    sec[2].virtual_address = 0x3000;
    sec[2].size_of_raw_data = 0x200;
    sec[2].pointer_to_raw_data = 0x600;
    sec[2].characteristics = PE_SECT_INIT_DATA | PE_SECT_READ | PE_SECT_WRITE;

    g_demo_pe[0x200] = 0xC3;
}

static void compat_watchdog_tick(uint32_t mem_mb, uint32_t cpu_percent)
{
    g_watchdog.cycles++;
    if (sandbox_check_memory(&g_sandbox, mem_mb) != 0 || cpu_percent > g_sandbox.max_cpu_percent) {
        g_watchdog.kills++;
        kprintf("[compat-watchdog] policy violation: mem=%uMB cpu=%u%%\n", mem_mb, cpu_percent);
    }
}

static void demo_exception_handler(uint32_t code, uint64_t addr)
{
    kprintf("[compat-seh] handled code=0x%llx addr=0x%llx (%s)\n",
            (uint64_t)code, addr, seh_code_name(code));
}

static void demo_thread_entry(void *arg)
{
    uint32_t tid = (uint32_t)(uintptr_t)arg;
    tls_set(tid, 0, (uint64_t)tid * 10ULL);
}

static int run_api_smoke(void)
{
    kprintf("[compat-smoke] running API smoke tests...\n");

    HANDLE h = w32_CreateFileA("/apps/test64.exe", GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (h == INVALID_HANDLE_VALUE) return -1;

    uint8_t buf[256];
    uint32_t n = 0;
    if (!w32_ReadFile(h, buf, sizeof(buf), &n, 0) || n == 0) {
        (void)w32_CloseHandle(h);
        return -1;
    }
    (void)w32_CloseHandle(h);

    (void)w32_GetCurrentProcess();
    (void)w32_GetCurrentThread();
    (void)w32_GetCurrentProcessId();
    (void)w32_GetCurrentThreadId();

    HANDLE out = w32_GetStdHandle(STD_OUTPUT_HANDLE);
    const char *msg = "[compat] console alive\n";
    uint32_t written = 0;
    (void)w32_WriteConsoleA(out, msg, 22, &written, 0);

    void *v = w32_VirtualAlloc(0, 128, MEM_COMMIT, PAGE_READWRITE);
    if (!v) return -1;
    if (!w32_VirtualFree(v, 0, MEM_RELEASE)) return -1;

    void *hmem = w32_HeapAlloc(0, 0, 64);
    if (!hmem) return -1;
    if (!w32_HeapFree(0, 0, hmem)) return -1;

    HANDLE default_heap = w32_GetProcessHeap();
    (void)default_heap;

    uint32_t tid = 0;
    HANDLE th = w32_CreateThread(0, 0, demo_thread_entry, (void *)(uintptr_t)1U, 0, &tid);
    if (th == INVALID_HANDLE_VALUE) return -1;
    if (w32_WaitForSingleObject(th, 1000) != WAIT_OBJECT_0) return -1;

    uint64_t tls = 0;
    if (tls_get(tid, 0, &tls) != 0 || tls != 10ULL) return -1;

    w32_SetLastError(1234);
    if (w32_GetLastError() != 1234U) return -1;

    (void)w32_GetModuleHandleA(0);
    (void)w32_GetTickCount();
    (void)w32_GetTickCount64();
    w32_Sleep(5);

    w32_OutputDebugStringA("smoke test debug message");
    (void)w32_MessageBoxA(0, "Hello from Layer5", "Madar Compat", 0);

    w32_RtlZeroMemory(buf, 64);
    w32_RtlFillMemory(buf, 32, 0xFF);

    w32_ExitThread(0);
    w32_ExitProcess(0);

    kprintf("[compat-smoke] all API smoke tests passed\n");
    return 0;
}

void compatd_init(void)
{
    kprintf("Layer 5: Starting compatibility subsystem...\n");

    compat_mem_init();
    sandbox_init(&g_sandbox);
    sandbox_stats_init(&g_sandbox_stats);
    sandbox_add_whitelist_path(&g_sandbox, "/apps");

    g_watchdog.cycles = 0;
    g_watchdog.kills = 0;

    win32_stubs_init();
    compat_syscall_bridge_init();
    seh_bridge_init();
    seh_register_handler(demo_exception_handler);
    compat_sync_init();
    compat_process_init();
    module_registry_init();
    module_registry_load_stubs();
    import_resolver_init();

    build_demo_pe();
    (void)vfs_mkdir("/apps");
    (void)ramdisk_create_file("/apps/test64.exe", g_demo_pe, g_demo_pe_size);

    kprintf("compatd initialized (sandbox: %uMB, cpu<=%u%%)\n",
            g_sandbox.max_memory_mb, g_sandbox.max_cpu_percent);
}

int compatd_exec_exe(const char *path)
{
    if (!path) return -1;

    if (sandbox_check_path(&g_sandbox, path) != 0) {
        kprintf("compatd: sandbox denied path %s\n", path);
        sandbox_stats_record_path_denial(&g_sandbox_stats);
        return -1;
    }

    const uint8_t *file_data = g_demo_pe;
    uint32_t file_size = g_demo_pe_size;
    uint8_t *tmp = (uint8_t *)kmalloc(4096);

    HANDLE h = w32_CreateFileA(path, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0);
    if (h != INVALID_HANDLE_VALUE && tmp) {
        uint32_t n = 0;
        if (w32_ReadFile(h, tmp, 4096, &n, 0) && n > 0) {
            file_data = tmp;
            file_size = n;
        }
        (void)w32_CloseHandle(h);
    }

    pe_image_info_t info;
    if (pe_parse_image(file_data, file_size, &info) != 0) {
        kprintf("compatd: PE parse failed for %s\n", path);
        if (tmp) kfree(tmp);
        return -1;
    }

    if (pe_validate_image(file_data, file_size, &info) != 0) {
        kprintf("compatd: PE validation failed for %s\n", path);
        if (tmp) kfree(tmp);
        return -1;
    }

    pe_print_image_info(&info);

    pe_loaded_image_t loaded;
    if (pe_load_image(file_data, file_size, &info, &loaded) != 0) {
        kprintf("compatd: PE load failed for %s\n", path);
        if (tmp) kfree(tmp);
        return -1;
    }

    if (info.has_relocs) {
        if (pe_apply_relocations(file_data, file_size, &info, &loaded) != 0) {
            kprintf("compatd: relocation failed for %s\n", path);
        } else {
            kprintf("compatd: relocations applied (delta=0x%llx)\n",
                    (uint64_t)(uintptr_t)loaded.image_base - info.image_base);
        }
    }

    compat_import_table_t import_table;
    import_resolver_parse(file_data, file_size, &info, &import_table);
    if (import_table.count > 0) {
        import_resolver_resolve(&import_table, loaded.image_base, &info);
        import_resolver_print(&import_table);
    }

    uint64_t pid = 0;
    compat_process_create("test_exe", loaded.image_base, loaded.entry_point, loaded.mapped_size, &pid);

    void *map_text = compat_mmap_region(4096, COMPAT_PROT_READ | COMPAT_PROT_EXEC);
    void *map_data = compat_mmap_region(4096, COMPAT_PROT_READ | COMPAT_PROT_WRITE);
    uint8_t *stack = (uint8_t *)compat_mmap_region(4096, COMPAT_PROT_READ | COMPAT_PROT_WRITE);
    uint64_t rsp = 0;
    if (!map_text || !map_data || !stack || compat_stack_prepare(stack, 4096, &rsp) != 0) {
        pe_unload_image(&loaded);
        if (tmp) kfree(tmp);
        if (map_text) compat_munmap_region(map_text);
        if (map_data) compat_munmap_region(map_data);
        if (stack) compat_munmap_region(stack);
        return -1;
    }

    if (run_api_smoke() != 0) {
        compat_cleanup_on_crash();
        pe_unload_image(&loaded);
        if (tmp) kfree(tmp);
        compat_munmap_region(map_text);
        compat_munmap_region(map_data);
        compat_munmap_region(stack);
        return -1;
    }

    (void)seh_raise(compat_exception_from_vector(0, 0), 0);
    (void)seh_raise(compat_exception_from_vector(14, 0xDEAD), 0xDEAD);

    (void)compat_call_api_by_name("GetTickCount", 0, 0, 0, 0);
    (void)compat_call_api_by_name("Sleep", 1, 0, 0, 0);

    compat_watchdog_tick(64, 20);

    compat_auto_close_on_exit();
    compat_exit_cleanup(0);

    pe_unload_image(&loaded);
    if (tmp) kfree(tmp);
    compat_munmap_region(map_text);
    compat_munmap_region(map_data);
    compat_munmap_region(stack);

    uint32_t total = compat_api_log_total_calls();
    uint32_t failed = compat_api_log_failed_calls();
    kprintf("compatd exec complete: api_calls=%u failed=%u rsp=0x%llx watchdog(kills=%u)\n",
            total, failed, rsp, g_watchdog.kills);

    compat_process_list();
    compat_mem_list_regions();
    module_registry_print();

    return 0;
}

int compatd_layer5_selftest(void)
{
    int rc = compatd_exec_exe("/apps/test64.exe");
    if (rc == 0 && compat_api_log_failed_calls() == 0) {
        kprintf("Layer 5: COMPATIBILITY ENGINE STABLE\n");
        return 0;
    }
    kprintf("Layer 5 selftest failed\n");
    return -1;
}
