#ifndef COMPAT_WIN32_STUBS_H
#define COMPAT_WIN32_STUBS_H

#include <types.h>

typedef uint32_t DWORD;
typedef uint64_t HANDLE;
typedef int BOOL;
typedef uint32_t UINT;
typedef uint16_t WORD;
typedef uint32_t LONG;
typedef uint64_t ULONG_PTR;
typedef int64_t LONGLONG;

#define TRUE 1
#define FALSE 0
#define INVALID_HANDLE_VALUE ((HANDLE)(uint64_t)-1)

/* Standard handle IDs */
#define STD_INPUT_HANDLE  ((uint32_t)-10)
#define STD_OUTPUT_HANDLE ((uint32_t)-11)
#define STD_ERROR_HANDLE  ((uint32_t)-12)

/* File access */
#define GENERIC_READ  (0x80000000U)
#define GENERIC_WRITE (0x40000000U)

/* File creation disposition */
#define CREATE_NEW        1
#define CREATE_ALWAYS     2
#define OPEN_EXISTING     3
#define OPEN_ALWAYS       4
#define TRUNCATE_EXISTING 5

/* Memory allocation */
#define MEM_COMMIT    0x00001000
#define MEM_RESERVE   0x00002000
#define MEM_RELEASE   0x00008000
#define MEM_FREE      0x00010000

/* Memory protection */
#define PAGE_NOACCESS          0x01
#define PAGE_READONLY          0x02
#define PAGE_READWRITE         0x04
#define PAGE_WRITECOPY         0x08
#define PAGE_EXECUTE           0x10
#define PAGE_EXECUTE_READ      0x20
#define PAGE_EXECUTE_READWRITE 0x40

/* Wait return values */
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT  0x102
#define WAIT_FAILED   0xFFFFFFFF
#define INFINITE      0xFFFFFFFF

/* Error codes */
#define ERROR_SUCCESS          0
#define ERROR_FILE_NOT_FOUND   2
#define ERROR_ACCESS_DENIED    5
#define ERROR_INVALID_HANDLE   6
#define ERROR_NOT_ENOUGH_MEM   8
#define ERROR_INVALID_PARAMETER 87

/* Last error per-thread */
DWORD w32_GetLastError(void);
void w32_SetLastError(DWORD err);

/* Process and thread */
HANDLE w32_GetCurrentProcess(void);
HANDLE w32_GetCurrentThread(void);
DWORD w32_GetCurrentProcessId(void);
DWORD w32_GetCurrentThreadId(void);
void w32_ExitProcess(uint32_t code);
void w32_ExitThread(uint32_t code);
HANDLE w32_CreateThread(void *sec, uint64_t stack, void (*entry)(void*), void *param, uint32_t flags, uint32_t *tid);
BOOL w32_TerminateThread(HANDLE h, DWORD code);
DWORD w32_SuspendThread(HANDLE h);
DWORD w32_ResumeThread(HANDLE h);
BOOL w32_GetExitCodeThread(HANDLE h, DWORD *code);

/* File I/O */
HANDLE w32_CreateFileA(const char *path, uint32_t access, uint32_t share, void *sec, uint32_t creation, uint32_t attrs, HANDLE tmpl);
BOOL w32_ReadFile(HANDLE h, void *buf, uint32_t len, uint32_t *read_out, void *ov);
BOOL w32_WriteFile(HANDLE h, const void *buf, uint32_t len, uint32_t *written_out, void *ov);
BOOL w32_CloseHandle(HANDLE h);
BOOL w32_DeleteFileA(const char *path);
BOOL w32_MoveFileA(const char *old, const char *new_);
BOOL w32_CopyFileA(const char *src, const char *dst, BOOL fail_if_exists);
DWORD w32_GetFileSize(HANDLE h, DWORD *high);
BOOL w32_SetFilePointer(HANDLE h, int32_t dist, int32_t *high, uint32_t method);

/* Console */
HANDLE w32_GetStdHandle(uint32_t std_handle);
BOOL w32_SetStdHandle(uint32_t std_handle, HANDLE h);
BOOL w32_WriteConsoleA(HANDLE h, const void *buf, uint32_t len, uint32_t *written_out, void *reserved);
BOOL w32_ReadConsoleA(HANDLE h, void *buf, uint32_t len, uint32_t *read_out, void *reserved);
BOOL w32_AllocConsole(void);
BOOL w32_FreeConsole(void);

/* Memory management */
void *w32_VirtualAlloc(void *addr, uint64_t size, uint32_t type, uint32_t protect);
BOOL w32_VirtualFree(void *addr, uint64_t size, uint32_t type);
BOOL w32_VirtualProtect(void *addr, uint64_t size, uint32_t new_prot, uint32_t *old_prot);
void *w32_HeapAlloc(HANDLE heap, uint32_t flags, uint64_t size);
BOOL w32_HeapFree(HANDLE heap, uint32_t flags, void *mem);
HANDLE w32_GetProcessHeap(void);
BOOL w32_HeapSetInformation(void *handle, void *type, void *info, size_t len);

/* Module */
HANDLE w32_GetModuleHandleA(const char *name);
HANDLE w32_GetModuleHandleW(const uint16_t *name);
BOOL w32_FreeLibrary(HANDLE mod);

/* Synchronization */
DWORD w32_WaitForSingleObject(HANDLE h, DWORD timeout_ms);
DWORD w32_WaitForMultipleObjects(uint32_t count, HANDLE *handles, BOOL wait_all, DWORD timeout_ms);
HANDLE w32_CreateMutexA(void *attr, BOOL owned, const char *name);
BOOL w32_ReleaseMutex(HANDLE h);
HANDLE w32_CreateEventA(void *attr, BOOL manual, BOOL initial, const char *name);
BOOL w32_SetEvent(HANDLE h);
BOOL w32_ResetEvent(HANDLE h);
HANDLE w32_CreateSemaphoreA(void *attr, LONG initial, LONG max, const char *name);
BOOL w32_ReleaseSemaphore(HANDLE h, LONG release, LONG *prev);

/* Time */
void w32_Sleep(uint32_t ms);
DWORD w32_GetTickCount(void);
DWORD w32_GetTickCount64(void);
BOOL w32_QueryPerformanceCounter(uint64_t *counter);
BOOL w32_QueryPerformanceFrequency(uint64_t *freq);
void w32_GetSystemTime(void *sys_time);

/* Environment */
DWORD w32_GetEnvironmentVariableA(const char *name, char *buf, DWORD size);
BOOL w32_SetEnvironmentVariableA(const char *name, const char *value);
DWORD w32_GetCommandLineA(char *buf, DWORD size);

/* Error handling */
void w32_RtlMoveMemory(void *dst, const void *src, uint64_t len);
void w32_RtlFillMemory(void *dst, uint64_t len, uint8_t val);
void w32_RtlZeroMemory(void *dst, uint64_t len);

/* String */
int w32_lstrcmpA(const char *a, const char *b);
int w32_lstrcmpiA(const char *a, const char *b);
int w32_lstrlenA(const char *s);
char *w32_lstrcpyA(char *dst, const char *src);
char *w32_lstrcatA(char *dst, const char *src);

/* Utility */
int w32_MessageBoxA(void *wnd, const char *text, const char *caption, uint32_t type);
void w32_OutputDebugStringA(const char *msg);
uint32_t w32_GetLastError(void);
void w32_SetLastError(uint32_t err);
BOOL w32_DebugBreak(void);

/* Heap API set */
HANDLE w32_HeapCreate(uint32_t options, uint64_t initial, uint64_t max);
BOOL w32_HeapDestroy(HANDLE h);
void *w32_HeapReAlloc(HANDLE h, uint32_t flags, void *mem, uint64_t size);
uint64_t w32_HeapSize(HANDLE h, uint32_t flags, const void *mem);
BOOL w32_HeapValidate(HANDLE h, uint32_t flags, const void *mem);

/* Initialization */
void win32_stubs_init(void);

#endif
