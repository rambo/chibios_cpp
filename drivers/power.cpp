#include "ch.hpp"
#include "hal.h"
#include "drivers/power_typedefs.h"
#include "drivers/power.hpp"

static BinarySemaphore power_semaphore;
#define LOCK chBSemWait(&power_semaphore);
#define UNLOCK chBSemSignal(&power_semaphore);

// Constructor
powermanager::powermanager(power_domain_t domains_list[])
{
    // TODO: Make sure this is a pointer to the array (usually it automagically is)
    domains = domains_list;
    chBSemInit(&power_semaphore, true); // CHECKME: Are we allowed to init semaphores in at construction time ??
}

/**
 * Requests a power domain to be enabled
 *
 * First request for a domain enables it (if the domain configuration has pre/post callbacks defined those are going to be called)
 * subsequent ones just increment our internal counter
 *
 * @return bool true if everything is ok, false otherwise
 */
bool powermanager::request(BOARD_POWER_DOMAIN domain)
{
    chDbgAssert(reservations[domain] < 254, "powermanager::request #1", "Trying to request a domain for the 255th time, something must be wrong");
    LOCK;
    if (!reservations[domain])
    {
        if (domains[domain]->pre_enable)
        {
            if (!domains[domain]->pre_enable())
            {
                goto ERROR;
            }
        }
        palSetPad(domains[domain]->port, domains[domain]->pin);
        if (domains[domain]->post_enable)
        {
            if (!domains[domain]->post_enable())
            {
                goto ERROR;
            }
        }
    }
    reservations[domain]++;
    UNLOCK;
    return true;
ERROR:
    UNLOCK;
    return false;
}

/**
 * Requests a power domain to be released
 *
 * Last release (when counter goes to 0) will disable the domain (if the domain configuration has pre/post callbacks defined those are going to be called)
 * before that we just decrement our internal counter
 *
 * Trying to release unused domain will trigger assert.
 *
 * @return bool true if everything is ok, false otherwise
 */
bool powermanager::release(BOARD_POWER_DOMAIN domain)
{
    chDbgAssert(reservations[domain] >= 1, "powermanager::release #1", "Trying to release unused domain");
    LOCK;
    reservations[domain]--;
    if (!reservations[domain])
    {
        if (domains[domain]->pre_disable)
        {
            if (!domains[domain]->pre_disable())
            {
                goto ERROR;
            }
        }
        palClearPad(domains[domain]->port, domains[domain]->pin);
        if (domains[domain]->post_disable)
        {
            if (!domains[domain]->post_disable())
            {
                goto ERROR;
            }
        }
    }
    UNLOCK;
    return true;
ERROR:
    UNLOCK;
    return false;
}

bool powermanager::all_released(void)
{
    for (uint8_t i=0; i < BOARD_NUM_POWER_DOMAINS; i++)
    {
        if (reservations[i])
        {
            return false;
        }
    }
    return true;
}



