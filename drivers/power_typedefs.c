#include "ch.h"
#include "hal.h"
#include "drivers/power_typedefs.h"

/*
struct power_domain_struct
{
    ioportid_t port;
    uint8_t pin; // no port is going to have more than 255 pins...
    bool (*pre_enable)(void);
    bool (*post_enable)(void);
    bool (*pre_disable)(void);
    bool (*post_disable)(void);
};
*/