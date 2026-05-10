#include "settings.h"
#include "string.h"

int app_config_load_defaults(void) {
    settings_set("resolution", "1024x768");
    settings_set("dhcp", "on");
    settings_set("language", "auto");
    settings_set("exe", "safe");
    return 0;
}
