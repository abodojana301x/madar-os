#include "win32_stubs.h"
#include "api_log.h"
#include "pe_threads.h"
#include "sync_mapper.h"
#include <kprintf.h>
#include <heap.h>
#include <string.h>
#include <timer.h>
#include "../services/vfs.h"

extern uint64_t sys_open(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_read(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_write(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
extern uint64_t sys_close(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

static DWORD g_last_error = 0;

static void log_api(const char *name, int ok)
{
    compat_api_log_call(name, ok ? 0 : -1);
}

void win32_stubs_init(void)
{
    compat_api_log_reset();
    g_last_error = 0;
}

/* ---- Error handling ---- */
DWORD w32_GetLastError(void) { return g_last_error; }
void w32_SetLastError(DWORD err) { g_last_error = err; }

/* ---- Process ---- */
HANDLE w32_GetCurrentProcess(void)
{
    log_api("GetCurrentProcess", 1);
    return (HANDLE)0xFFFF0001ULL;
}

HANDLE w32_GetCurrentThread(void)
{
    log_api("GetCurrentThread", 1);
    return (HANDLE)0xFFFF0002ULL;
}

DWORD w32_GetCurrentProcessId(void)
{
    log_api("GetCurrentProcessId", 1);
    return 1;
}

DWORD w32_GetCurrentThreadId(void)
{
    log_api("GetCurrentThreadId", 1);
    return 1;
}

void w32_ExitProcess(uint32_t code)
{
    log_api("ExitProcess", 1);
    kprintf("[compat] ExitProcess(%u)\n", code);
}

void w32_ExitThread(uint32_t code)
{
    log_api("ExitThread", 1);
    kprintf("[compat] ExitThread(%u)\n", code);
}

HANDLE w32_CreateThread(void *sec, uint64_t stack, void (*entry)(void*), void *param, uint32_t flags, uint32_t *tid)
{
    (void)sec; (void)stack; (void)flags;
    uint32_t tid_val = 0;
    int rc = compat_create_thread(entry, param, &tid_val);
    if (tid) *tid = tid_val;
    if (rc != 0) { g_last_error = ERROR_NOT_ENOUGH_MEM; }
    log_api("CreateThread", rc == 0);
    return (rc == 0) ? (HANDLE)tid_val : INVALID_HANDLE_VALUE;
}

BOOL w32_TerminateThread(HANDLE h, DWORD code)
{
    (void)h; (void)code;
    log_api("TerminateThread", 1);
    return TRUE;
}

DWORD w32_SuspendThread(HANDLE h)
{
    (void)h;
    log_api("SuspendThread", 1);
    return 0;
}

DWORD w32_ResumeThread(HANDLE h)
{
    (void)h;
    log_api("ResumeThread", 1);
    return 0;
}

BOOL w32_GetExitCodeThread(HANDLE h, DWORD *code)
{
    (void)h;
    if (code) *code = 0;
    log_api("GetExitCodeThread", 1);
    return TRUE;
}

/* ---- File I/O ---- */
HANDLE w32_CreateFileA(const char *path, uint32_t access, uint32_t share, void *sec, uint32_t creation, uint32_t attrs, HANDLE tmpl)
{
    (void)share; (void)sec; (void)creation; (void)attrs; (void)tmpl;
    uint64_t flags = VFS_O_RDONLY;
    if (access & GENERIC_WRITE) flags = VFS_O_WRONLY;
    if ((access & (GENERIC_READ | GENERIC_WRITE)) == (GENERIC_READ | GENERIC_WRITE)) flags = VFS_O_RDWR;
    uint64_t fd = sys_open((uint64_t)(uintptr_t)path, flags, 0, 0, 0, 0);
    if (fd == (uint64_t)-1) g_last_error = ERROR_FILE_NOT_FOUND;
    log_api("CreateFileA", fd != (uint64_t)-1);
    return (fd == (uint64_t)-1) ? INVALID_HANDLE_VALUE : (HANDLE)fd;
}

BOOL w32_ReadFile(HANDLE h, void *buf, uint32_t len, uint32_t *read_out, void *ov)
{
    (void)ov;
    uint64_t n = sys_read((uint64_t)h, (uint64_t)(uintptr_t)buf, len, 0, 0, 0);
    if (read_out) *read_out = (n == (uint64_t)-1) ? 0 : (uint32_t)n;
    if (n == (uint64_t)-1) g_last_error = ERROR_ACCESS_DENIED;
    log_api("ReadFile", n != (uint64_t)-1);
    return (n == (uint64_t)-1) ? FALSE : TRUE;
}

BOOL w32_WriteFile(HANDLE h, const void *buf, uint32_t len, uint32_t *written_out, void *ov)
{
    (void)ov;
    uint64_t n = sys_write((uint64_t)h, (uint64_t)(uintptr_t)buf, len, 0, 0, 0);
    if (written_out) *written_out = (n == (uint64_t)-1) ? 0 : (uint32_t)n;
    if (n == (uint64_t)-1) g_last_error = ERROR_ACCESS_DENIED;
    log_api("WriteFile", n != (uint64_t)-1);
    return (n == (uint64_t)-1) ? FALSE : TRUE;
}

BOOL w32_CloseHandle(HANDLE h)
{
    if (h == INVALID_HANDLE_VALUE || h == 0xFFFF0001ULL || h == 0xFFFF0002ULL) {
        log_api("CloseHandle", 1);
        return TRUE;
    }
    uint64_t rc = sys_close((uint64_t)h, 0, 0, 0, 0, 0);
    if (rc == (uint64_t)-1) g_last_error = ERROR_INVALID_HANDLE;
    log_api("CloseHandle", rc != (uint64_t)-1);
    return (rc == (uint64_t)-1) ? FALSE : TRUE;
}

BOOL w32_DeleteFileA(const char *path)
{
    (void)path;
    log_api("DeleteFileA", 1);
    return TRUE;
}

BOOL w32_MoveFileA(const char *old, const char *new_)
{
    (void)old; (void)new_;
    log_api("MoveFileA", 1);
    return TRUE;
}

BOOL w32_CopyFileA(const char *src, const char *dst, BOOL fail_if_exists)
{
    (void)src; (void)dst; (void)fail_if_exists;
    log_api("CopyFileA", 1);
    return TRUE;
}

DWORD w32_GetFileSize(HANDLE h, DWORD *high)
{
    (void)h; (void)high;
    log_api("GetFileSize", 1);
    return 0;
}

BOOL w32_SetFilePointer(HANDLE h, int32_t dist, int32_t *high, uint32_t method)
{
    (void)h; (void)dist; (void)high; (void)method;
    log_api("SetFilePointer", 1);
    return TRUE;
}

/* ---- Console ---- */
HANDLE w32_GetStdHandle(uint32_t std_handle)
{
    log_api("GetStdHandle", 1);
    return (HANDLE)(0x1000 + std_handle);
}

BOOL w32_SetStdHandle(uint32_t std_handle, HANDLE h)
{
    (void)std_handle; (void)h;
    log_api("SetStdHandle", 1);
    return TRUE;
}

BOOL w32_WriteConsoleA(HANDLE h, const void *buf, uint32_t len, uint32_t *written_out, void *reserved)
{
    (void)h; (void)reserved;
    const char *cbuf = (const char *)buf;
    if (!cbuf) { g_last_error = ERROR_INVALID_PARAMETER; log_api("WriteConsoleA", 0); return FALSE; }
    for (uint32_t i = 0; i < len && cbuf[i]; i++) kprintf("%c", cbuf[i]);
    if (written_out) *written_out = len;
    log_api("WriteConsoleA", 1);
    return TRUE;
}

BOOL w32_ReadConsoleA(HANDLE h, void *buf, uint32_t len, uint32_t *read_out, void *reserved)
{
    (void)h; (void)buf; (void)len; (void)reserved;
    if (read_out) *read_out = 0;
    log_api("ReadConsoleA", 1);
    return TRUE;
}

BOOL w32_AllocConsole(void)
{
    log_api("AllocConsole", 1);
    return TRUE;
}

BOOL w32_FreeConsole(void)
{
    log_api("FreeConsole", 1);
    return TRUE;
}

/* ---- Memory ---- */
void *w32_VirtualAlloc(void *addr, uint64_t size, uint32_t type, uint32_t protect)
{
    (void)addr; (void)type; (void)protect;
    if (size == 0) { g_last_error = ERROR_INVALID_PARAMETER; log_api("VirtualAlloc", 0); return NULL; }
    void *p = kmalloc((size_t)size);
    if (!p) { g_last_error = ERROR_NOT_ENOUGH_MEM; log_api("VirtualAlloc", 0); return NULL; }
    memset(p, 0, (size_t)size);
    log_api("VirtualAlloc", 1);
    return p;
}

BOOL w32_VirtualFree(void *addr, uint64_t size, uint32_t type)
{
    (void)size; (void)type;
    if (!addr) { g_last_error = ERROR_INVALID_PARAMETER; log_api("VirtualFree", 0); return FALSE; }
    kfree(addr);
    log_api("VirtualFree", 1);
    return TRUE;
}

BOOL w32_VirtualProtect(void *addr, uint64_t size, uint32_t new_prot, uint32_t *old_prot)
{
    (void)addr; (void)size; (void)new_prot;
    if (old_prot) *old_prot = PAGE_READWRITE;
    log_api("VirtualProtect", 1);
    return TRUE;
}

void *w32_HeapAlloc(HANDLE heap, uint32_t flags, uint64_t size)
{
    (void)heap; (void)flags;
    void *p = kmalloc((size_t)size);
    if (!p) { g_last_error = ERROR_NOT_ENOUGH_MEM; log_api("HeapAlloc", 0); return NULL; }
    if (flags & 8) memset(p, 0, (size_t)size);
    log_api("HeapAlloc", 1);
    return p;
}

BOOL w32_HeapFree(HANDLE heap, uint32_t flags, void *mem)
{
    (void)heap; (void)flags;
    if (!mem) { g_last_error = ERROR_INVALID_PARAMETER; log_api("HeapFree", 0); return FALSE; }
    kfree(mem);
    log_api("HeapFree", 1);
    return TRUE;
}

HANDLE w32_GetProcessHeap(void)
{
    log_api("GetProcessHeap", 1);
    return (HANDLE)1;
}

BOOL w32_HeapSetInformation(void *handle, void *type, void *info, size_t len)
{
    (void)handle; (void)type; (void)info; (void)len;
    log_api("HeapSetInformation", 1);
    return TRUE;
}

/* ---- Module ---- */
HANDLE w32_GetModuleHandleA(const char *name)
{
    (void)name;
    log_api("GetModuleHandleA", 1);
    return (HANDLE)1;
}

HANDLE w32_GetModuleHandleW(const uint16_t *name)
{
    (void)name;
    log_api("GetModuleHandleW", 1);
    return (HANDLE)1;
}

BOOL w32_FreeLibrary(HANDLE mod)
{
    (void)mod;
    log_api("FreeLibrary", 1);
    return TRUE;
}

/* ---- Synchronization ---- */
DWORD w32_WaitForSingleObject(HANDLE h, DWORD timeout_ms)
{
    int rc = compat_wait_for_object((uint32_t)h, timeout_ms);
    log_api("WaitForSingleObject", rc == 0);
    return (rc == 0) ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
}

DWORD w32_WaitForMultipleObjects(uint32_t count, HANDLE *handles, BOOL wait_all, DWORD timeout_ms)
{
    (void)count; (void)handles; (void)wait_all; (void)timeout_ms;
    log_api("WaitForMultipleObjects", 1);
    return WAIT_OBJECT_0;
}

HANDLE w32_CreateMutexA(void *attr, BOOL owned, const char *name)
{
    (void)attr; (void)owned; (void)name;
    log_api("CreateMutexA", 1);
    return (HANDLE)0x2001;
}

BOOL w32_ReleaseMutex(HANDLE h)
{
    (void)h;
    log_api("ReleaseMutex", 1);
    return TRUE;
}

HANDLE w32_CreateEventA(void *attr, BOOL manual, BOOL initial, const char *name)
{
    (void)attr; (void)manual; (void)initial; (void)name;
    log_api("CreateEventA", 1);
    return (HANDLE)0x3001;
}

BOOL w32_SetEvent(HANDLE h)
{
    (void)h;
    log_api("SetEvent", 1);
    return TRUE;
}

BOOL w32_ResetEvent(HANDLE h)
{
    (void)h;
    log_api("ResetEvent", 1);
    return TRUE;
}

HANDLE w32_CreateSemaphoreA(void *attr, LONG initial, LONG max, const char *name)
{
    (void)attr; (void)initial; (void)max; (void)name;
    log_api("CreateSemaphoreA", 1);
    return (HANDLE)0x4001;
}

BOOL w32_ReleaseSemaphore(HANDLE h, LONG release, LONG *prev)
{
    (void)h; (void)release;
    if (prev) *prev = 0;
    log_api("ReleaseSemaphore", 1);
    return TRUE;
}

/* ---- Time ---- */
void w32_Sleep(uint32_t ms)
{
    uint64_t start = timer_get_ticks();
    uint64_t ticks_needed = (ms / 10U) + 1U;
    uint64_t guard = (uint64_t)ms * 20000ULL + 50000ULL;
    while ((timer_get_ticks() - start) < ticks_needed) {
        if (guard-- == 0) break;
        __asm__ volatile("pause");
    }
    log_api("Sleep", 1);
}

DWORD w32_GetTickCount(void)
{
    log_api("GetTickCount", 1);
    return (DWORD)(timer_get_ticks() * 10);
}

DWORD w32_GetTickCount64(void)
{
    log_api("GetTickCount64", 1);
    return (DWORD)(timer_get_ticks() * 10);
}

BOOL w32_QueryPerformanceCounter(uint64_t *counter)
{
    if (counter) *counter = timer_get_ticks();
    log_api("QueryPerformanceCounter", 1);
    return TRUE;
}

BOOL w32_QueryPerformanceFrequency(uint64_t *freq)
{
    if (freq) *freq = 100;
    log_api("QueryPerformanceFrequency", 1);
    return TRUE;
}

void w32_GetSystemTime(void *sys_time)
{
    (void)sys_time;
    log_api("GetSystemTime", 1);
}

/* ---- Environment ---- */
DWORD w32_GetEnvironmentVariableA(const char *name, char *buf, DWORD size)
{
    (void)name;
    if (buf && size > 0) buf[0] = '\0';
    log_api("GetEnvironmentVariableA", 1);
    return 0;
}

BOOL w32_SetEnvironmentVariableA(const char *name, const char *value)
{
    (void)name; (void)value;
    log_api("SetEnvironmentVariableA", 1);
    return TRUE;
}

DWORD w32_GetCommandLineA(char *buf, DWORD size)
{
    if (buf && size > 0) buf[0] = '\0';
    log_api("GetCommandLineA", 1);
    return 0;
}

/* ---- Memory move/fill ---- */
void w32_RtlMoveMemory(void *dst, const void *src, uint64_t len)
{
    memcpy(dst, src, (size_t)len);
    log_api("RtlMoveMemory", 1);
}

void w32_RtlFillMemory(void *dst, uint64_t len, uint8_t val)
{
    memset(dst, val, (size_t)len);
    log_api("RtlFillMemory", 1);
}

void w32_RtlZeroMemory(void *dst, uint64_t len)
{
    memset(dst, 0, (size_t)len);
    log_api("RtlZeroMemory", 1);
}

/* ---- String ---- */
int w32_lstrcmpA(const char *a, const char *b)
{
    log_api("lstrcmpA", 1);
    return strcmp(a, b);
}

int w32_lstrcmpiA(const char *a, const char *b)
{
    log_api("lstrcmpiA", 1);
    return strcmp(a, b);
}

int w32_lstrlenA(const char *s)
{
    log_api("lstrlenA", 1);
    return s ? (int)strlen(s) : 0;
}

char *w32_lstrcpyA(char *dst, const char *src)
{
    log_api("lstrcpyA", 1);
    if (dst && src) strncpy(dst, src, 4096);
    return dst;
}

char *w32_lstrcatA(char *dst, const char *src)
{
    log_api("lstrcatA", 1);
    if (dst && src) strncat(dst, src, 4096);
    return dst;
}

/* ---- Utility ---- */
int w32_MessageBoxA(void *wnd, const char *text, const char *caption, uint32_t type)
{
    (void)wnd; (void)type;
    kprintf("[compat-msgbox] %s: %s\n", caption ? caption : "Message", text ? text : "");
    log_api("MessageBoxA", 1);
    return 1;
}

void w32_OutputDebugStringA(const char *msg)
{
    if (msg) kprintf("[compat-dbg] %s\n", msg);
    log_api("OutputDebugStringA", 1);
}

BOOL w32_DebugBreak(void)
{
    log_api("DebugBreak", 1);
    kprintf("[compat] DebugBreak called\n");
    return TRUE;
}

/* ---- Heap API set ---- */
HANDLE w32_HeapCreate(uint32_t options, uint64_t initial, uint64_t max)
{
    (void)options; (void)initial; (void)max;
    log_api("HeapCreate", 1);
    return (HANDLE)0x5001;
}

BOOL w32_HeapDestroy(HANDLE h)
{
    (void)h;
    log_api("HeapDestroy", 1);
    return TRUE;
}

void *w32_HeapReAlloc(HANDLE h, uint32_t flags, void *mem, uint64_t size)
{
    (void)h; (void)flags; (void)mem; (void)size;
    log_api("HeapReAlloc", 1);
    return mem;
}

uint64_t w32_HeapSize(HANDLE h, uint32_t flags, const void *mem)
{
    (void)h; (void)flags; (void)mem;
    log_api("HeapSize", 1);
    return 0;
}

BOOL w32_HeapValidate(HANDLE h, uint32_t flags, const void *mem)
{
    (void)h; (void)flags; (void)mem;
    log_api("HeapValidate", 1);
    return TRUE;
}
