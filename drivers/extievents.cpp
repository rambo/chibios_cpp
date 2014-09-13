#include "ch.h"
#include "ch.hpp"
#include "hal.h"
#include "drivers/extievents.hpp"

static chibios_rt::BinarySemaphore exticonfig_semaphore = chibios_rt::BinarySemaphore(false);
#define LOCK exticonfig_semaphore.wait();
#define UNLOCK exticonfig_semaphore.signal();

// A lot of this is shamelessly ripped from Kulve

/***********
 * Manager
 ***********/

uint32_t extimanager_class::get_ext_from_port(ioportid_t port)
{
#define GPIO2EXT(a)                             \
  if (port == a) return EXT_MODE_##a
  GPIO2EXT(GPIOA);
  GPIO2EXT(GPIOB);
  GPIO2EXT(GPIOC);
#ifdef GPIOD
  GPIO2EXT(GPIOD);
#endif
#ifdef GPIOE
  GPIO2EXT(GPIOE);
#endif
#ifdef GPIOF
  GPIO2EXT(GPIOF);
#endif
#ifdef GPIOG
  GPIO2EXT(GPIOG);
#endif
#ifdef GPIOH
  GPIO2EXT(GPIOH);
#endif
#ifdef GPIOI
  GPIO2EXT(GPIOI);
#endif
#undef GPIO2EXT

  chDbgAssert(0, "extimanager_class::get_ext_from_port #1", "Invalid port");

  return 0;
}

void extimanager_class::_extint_cb(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    chDbgAssert(channel < EXT_MAX_CHANNELS, "extimanager_class::_extint_cb#1", "Channel number too large");
    
    // TODO: Post a signal...

}


void extimanager_class::enable_signal(ioportid_t port, uint8_t pin, extint_edge_t edgemode)
{
    set_callback(port, pin, mode, _extint_cb);
}



void extimanager_class::set_callback(ioportid_t port, uint8_t pin, extint_edge_t mode, extcallback_t cb)
{
    uint8_t need_start = 1;
    uint8_t i;
    uint32_t ext_mode;
    EXTChannelConfig cfg;

    LOCK;

    chDbgAssert(pin < EXT_MAX_CHANNELS , "extimanager_class::set_callback#1", "EXT pin number outside range");
    chDbgAssert(extcfg.channels[pin].cb == NULL, "extimanager_class::set_callback#2", "EXT pin already registered");
    chDbgAssert(   mode == EXT_CH_MODE_RISING_EDGE
                || mode == EXT_CH_MODE_FALLING_EDGE
                || mode == EXT_CH_MODE_BOTH_EDGES, "extimanager_class::set_callback#3", "Invalid edge mode");

    for (i = 0; i < EXT_MAX_CHANNELS; ++i)
    {
        if (extcfg.channels[pin].cb != NULL)
        {
            need_start = 0;
            break;
        }
    }

    ext_mode = (mode | EXT_CH_MODE_AUTOSTART | get_ext_from_port(port));

    cfg.cb = cb;
    cfg.mode = ext_mode;

    if (need_start)
    {
        extStart(&EXTD1, &extcfg);
    }

    extSetChannelMode(&EXTD1, pin, &cfg);

    UNLOCK;
}




/**********
 * Thread
 **********/

/**
 * Constructor
 */
msg_t extimanager_thd::extimanager_thd()
{
    // Create a manager instance
    manager = extimanager_class();
}

/**
 * Thread entry point
 */
virtual msg_t extimanager_thd::main(void)
{
    while (true)
    {
        // Nop
    }
}
