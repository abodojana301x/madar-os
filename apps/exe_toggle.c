#include "settings.h"

int exe_toggle_safe_mode(int enabled) {
    return settings_set("exe", enabled ? "safe" : "off");
}
