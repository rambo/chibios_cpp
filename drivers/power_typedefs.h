#ifndef POWER_TYPEDEFS_H
#define POWER_TYPEDEFS_H
#include "ch.h"
#include "hal.h"

/* this complains about error: unknown type name 'iopinid_t' but hal.h should get that defined
typedef struct power_domain_struct
{
    iopinid_t port;
    int pin;
    void (*pre_enable)(void);
    void (*post_enable)(void);
    void (*pre_disable)(void);
    void (*post_disable)(void);
} power_domain_t;
*/
typedef struct power_domain_struct power_domain_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif