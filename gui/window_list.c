#include "wm_core.h"

void wm_compact_z(wm_window_t *arr, int n)
{
    int z = 0;
    for (int pass = 0; pass < n; pass++) {
        for (int i = 0; i < n; i++) {
            if (arr[i].used && arr[i].z_index == pass) {
                arr[i].z_index = z++;
            }
        }
    }
}
