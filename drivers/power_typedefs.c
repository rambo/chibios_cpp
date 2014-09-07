#include "ch.h"
#include "hal.h"
#include "drivers/power_typedefs.h"

struct power_domain_struct
{
    iopinid_t port;
    uint8_t pin; // no port is going to have more than 255 pins...
    void (*pre_enable)(void);
    void (*post_enable)(void);
    void (*pre_disable)(void);
    void (*post_disable)(void);
};
