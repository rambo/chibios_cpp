#ifndef POWER_HPP
#define POWER_HPP
#include "drivers/power_typedefs.h"

class powermanager_class
{
    protected:
        uint8_t reservations[BOARD_NUM_POWER_DOMAINS]; // Keyed by the power domain number

    public:
        powermanager_class();
        bool request(BOARD_POWER_DOMAIN_t domain);
        bool release(BOARD_POWER_DOMAIN_t domain);
        bool all_released(void);

};

void cmd_power_request(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_power_release(BaseSequentialStream *chp, int argc, char *argv[]);

extern powermanager_class powermanager;

#endif