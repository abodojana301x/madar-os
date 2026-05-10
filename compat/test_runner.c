#include "test_runner.h"
#include "compatd.h"
#include <kprintf.h>

int compat_run_all_tests(void)
{
    kprintf("🧪 Layer5 test runner start\n");
    int rc = compatd_layer5_selftest();
    if (rc == 0) {
        kprintf("✅ Layer5 tests PASSED\n");
    } else {
        kprintf("❌ Layer5 tests FAILED\n");
    }
    return rc;
}
