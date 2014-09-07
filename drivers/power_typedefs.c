#include "hal.h"
#include "drivers/power_typedefs.h"

struct power_domain_struct
{
    void* port;
    int pin;
    void (*pre_enable)(void);
    void (*post_enable)(void);
    void (*pre_disable)(void);
    void (*post_disable)(void);
};
