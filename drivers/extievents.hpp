#ifndef EXTIEVENTS_HPP
#define EXTIEVENTS_HPP
#include "ch.h"
#include "ch.hpp"
#include "hal.h"

typedef enum {
  EXTINT_EDGE_RISING  = 0x1,
  EXTINT_EDGE_FALLING = 0x2,
  EXTINT_EDGE_BOTH    = 0x3
} extint_edge_t;


class extimanager_class
{
    protected:
        EXTConfig extcfg;
        uint32_t get_ext_from_port(ioportid_t port);
        void set_callback(ioportid_t port, uint8_t pin,  extint_edge_t edgemode, extcallback_t cb);
        void _extint_cb(EXTDriver *extp, expchannel_t channel);
 
    public:
        void enable_signal(ioportid_t port, uint8_t pin, extint_edge_t edgemode);
};

class extimanager_thd : public chibios_rt::BaseStaticThread<128>
{
    protected:
        virtual msg_t main(void);

    public:
        // Instance of the extimanager for enabling signals etc
        extimanager_class manager;
        // Constructor
        extimanager_thd();
};

extern extimanager_thd extimanager;
extern chibios_rt::EvtSource extievent;


#endif