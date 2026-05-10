#include "settings.h"

int resolution_apply(const char *mode) {
    return settings_set("resolution", mode ? mode : "1024x768");
}
