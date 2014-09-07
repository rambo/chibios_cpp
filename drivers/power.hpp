#ifndef POWER_HPP
#define POWER_HPP
#include "drivers/power_typedefs.h"

class powermanager
{
    protected:
        uint8_t reservations[BOARD_NUM_POWER_DOMAINS]; // Keyed by the power domain number
        power_domain_t domains[];

    public:
        powermanager(power_domain_t domains_list[]);
        bool request(BOARD_POWER_DOMAIN domain);
        bool release(BOARD_POWER_DOMAIN domain);
        bool all_released(void);

}

#endif