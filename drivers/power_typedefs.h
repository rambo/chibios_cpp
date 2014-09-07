#ifndef POWER_TYPEDEFS_H
#define POWER_TYPEDEFS_H
#include "ch.h"
#include "hal.h"

/* this complains about error: unknown type name 'ioportid_t' but hal.h should get that defined
typedef struct power_domain_struct
{
    ioportid_t port;
    uint8_t pin; // no port is going to have more than 255 pins...
    bool (*pre_enable)(void);
    bool (*post_enable)(void);
    bool (*pre_disable)(void);
    bool (*post_disable)(void);
} power_domain_t;
*/
/**
 * Trying to define the struct in the .c
typedef struct power_domain_struct power_domain_t;
*/

typedef struct power_domain_struct
{
    void* port;
    uint8_t pin; // no port is going to have more than 255 pins...
    bool (*pre_enable)(void);
    bool (*post_enable)(void);
    bool (*pre_disable)(void);
    bool (*post_disable)(void);
} power_domain_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif