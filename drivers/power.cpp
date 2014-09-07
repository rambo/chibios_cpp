#include "ch.hpp"
#include "hal.h"
#include "board.h"
#include "drivers/power_typedefs.h"
#include "drivers/power.hpp"
#include "chprintf.h"
#include <stdlib.h> // For atoi

static chibios_rt::BinarySemaphore power_semaphore = chibios_rt::BinarySemaphore(false);
#define LOCK power_semaphore.wait();
#define UNLOCK power_semaphore.signal();

// Constructor
powermanager_class::powermanager_class()
{
}

/**
 * Requests a power domain to be enabled
 *
 * First request for a domain enables it (if the domain configuration has pre/post callbacks defined those are going to be called)
 * subsequent ones just increment our internal counter
 *
 * @return bool true if everything is ok, false otherwise
 */
bool powermanager_class::request(BOARD_POWER_DOMAIN_t domain)
{
    chDbgAssert(reservations[domain] < 254, "powermanager_class::request #1", "Trying to request a domain for the 255th time, something must be wrong");
    LOCK;
    if (!reservations[domain])
    {
        if (BOARD_POWER_DOMAIN_CONFIG[domain].pre_enable)
        {
            if (!BOARD_POWER_DOMAIN_CONFIG[domain].pre_enable())
            {
                goto ERROR;
            }
        }
        palSetPad((ioportid_t)BOARD_POWER_DOMAIN_CONFIG[domain].port, BOARD_POWER_DOMAIN_CONFIG[domain].pin);
        if (BOARD_POWER_DOMAIN_CONFIG[domain].post_enable)
        {
            if (!BOARD_POWER_DOMAIN_CONFIG[domain].post_enable())
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
bool powermanager_class::release(BOARD_POWER_DOMAIN_t domain)
{
    chDbgAssert(reservations[domain] >= 1, "powermanager_class::release #1", "Trying to release unused domain");
    LOCK;
    if (reservations[domain] == 1)
    {
        if (BOARD_POWER_DOMAIN_CONFIG[domain].pre_disable)
        {
            if (!BOARD_POWER_DOMAIN_CONFIG[domain].pre_disable())
            {
                goto ERROR;
            }
        }
        palClearPad((ioportid_t)BOARD_POWER_DOMAIN_CONFIG[domain].port, BOARD_POWER_DOMAIN_CONFIG[domain].pin);
        if (BOARD_POWER_DOMAIN_CONFIG[domain].post_disable)
        {
            if (!BOARD_POWER_DOMAIN_CONFIG[domain].post_disable())
            {
                goto ERROR;
            }
        }
    }
    reservations[domain]--;
    UNLOCK;
    return true;
ERROR:
    UNLOCK;
    return false;
}

/**
 * Checks if given power domain is enabled
 *
 * NOTE: This method is mainly for testing and debugging, you need to use a power domain just request it and release when done...
 *
 * @return bool true if domain is enabled, false if disabled
 */
bool powermanager_class::status(BOARD_POWER_DOMAIN_t domain)
{
    return (bool)reservations[domain];
}

/**
 * Checks if all power domains are released (for example if we might want to go to standby mode...)
 *
 * @return bool true if all domains are fully released (false otherwise)
 */
bool powermanager_class::all_released(void)
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

/**
 * Shell command to test the power request
 *
 */
void cmd_power_request(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc < 1)
    {
        chprintf(chp, "Usage: power_request number_of_power_domain\r\n");
        return;
    }
    uint8_t domain = atoi(argv[0]);
    if (!powermanager.request((BOARD_POWER_DOMAIN_t)domain))
    {
        chprintf(chp, "FAILED: domain was %d\r\n", domain);
        return;
    }
    chprintf(chp, "OK: domain was %d\r\n", domain);
}

/**
 * Shell command to test the power release
 *
 */
void cmd_power_release(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc < 1)
    {
        chprintf(chp, "Usage: power_release number_of_power_domain\r\n");
        return;
    }
    uint8_t domain = atoi(argv[0]);
    if (!powermanager.release((BOARD_POWER_DOMAIN_t)domain))
    {
        chprintf(chp, "FAILED: domain was %d\r\n", domain);
        return;
    }
    chprintf(chp, "OK: domain was %d\r\n", domain);
    if (powermanager.status((BOARD_POWER_DOMAIN_t)domain))
    {
        chprintf(chp, "Domain %d is still enabled\r\n", domain);
    }
    if (powermanager.all_released())
    {
        chprintf(chp, "All domains released!\r\n");
    }
}

// Create an instance to use
powermanager_class powermanager = powermanager_class();
