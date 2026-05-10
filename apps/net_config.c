#include "settings.h"

int net_config_set_dhcp(int enabled) {
    return settings_set("dhcp", enabled ? "on" : "off");
}
