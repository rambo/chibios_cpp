/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <time.h>

#include "ch.h"
#include "ch.hpp"
#include "hal.h"
#include "test.h"

#include "shell.h"
#include "drivers/debug.h"
#include "chprintf.h"
#include "power.h"
#include "drivers/usb_serial.h"
#include "drivers/reset.h"

using namespace chibios_rt;



/**
 * Backup domain data
 */
#define BACKUP_CONFIG_VERSION (0xdeadbeef + 0) // Increment the + part each time the config struct changes
struct backup_domain_data_t
{
    char apn[50];
    char pin[10];
    uint32_t config_version;
}; struct backup_domain_data_t * const backup_domain_data = (struct backup_domain_data_t *)BKPSRAM_BASE;
static bool backup_domain_data_is_sane(void)
{
    return backup_domain_data->config_version == BACKUP_CONFIG_VERSION;
}



/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/



static void cmd_bkp(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc < 2)
    {
        chprintf(chp, "Usage: bkp set member value | bkp get member\r\n");
        return;
    }
    if (0 == strcmp(argv[0], "get"))
    {
        if (!backup_domain_data_is_sane())
        {
            chprintf(chp, "Backup data config version %x does not match expected %x\r\n", backup_domain_data->config_version, BACKUP_CONFIG_VERSION);
            return;
        }
        if (0 == strcmp(argv[1], "apn"))
        {
            chprintf(chp, "backup_domain_data->%s=%s\r\n", argv[1], backup_domain_data->apn);
        }
        else if (0 == strcmp(argv[1], "pin"))
        {
            chprintf(chp, "backup_domain_data->%s=%s\r\n", argv[1], backup_domain_data->pin);
        }
        else
        {
            chprintf(chp, "Unsupported member %s\r\n", argv[1]);
        }
    }
    else if (0 == strcmp(argv[0], "set"))
    {
        if (0 == strcmp(argv[1], "apn"))
        {
            strncpy(backup_domain_data->apn, argv[2], sizeof(backup_domain_data->apn));
            // Set the signature (maybe someday it's a checksum)
            backup_domain_data->config_version = BACKUP_CONFIG_VERSION;
        }
        else if (0 == strcmp(argv[1], "pin"))
        {
            strncpy(backup_domain_data->pin, argv[2], sizeof(backup_domain_data->pin));
            // Set the signature (maybe someday it's a checksum)
            backup_domain_data->config_version = BACKUP_CONFIG_VERSION;
        }
        else
        {
            chprintf(chp, "Unsupported member %s\r\n", argv[1]);
        }
    }
    else
    {
        chprintf(chp, "Unsupported subcommand %s\r\n", argv[0]);
    }
    
}


static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
    size_t n, size;

    (void)argv;
    if (argc > 0)
    {
        chprintf(chp, "Usage: mem\r\n");
        return;
    }
    n = chHeapStatus(NULL, &size);
    chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
    chprintf(chp, "heap fragments   : %u\r\n", n);
    chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
    static const char *states[] = {THD_STATE_NAMES};
    Thread *tp;

    (void)argv;
    if (argc > 0)
    {
        chprintf(chp, "Usage: threads\r\n");
        return;
    }
    chprintf(chp, "    addr    stack prio refs     state time\r\n");
    tp = chRegFirstThread();
    do
    {
        chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
                 (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
                 (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
                 states[tp->p_state], (uint32_t)tp->p_time);
        tp = chRegNextThread(tp);
    } while (tp != NULL);
#ifdef CORTEX_ENABLE_WFI_IDLE
    chprintf(chp, "CORTEX_ENABLE_WFI_IDLE=%d\r\n", CORTEX_ENABLE_WFI_IDLE);
#endif
#ifdef ENABLE_WFI_IDLE
    chprintf(chp, "ENABLE_WFI_IDLE=%d\r\n", ENABLE_WFI_IDLE);
#endif
}

#define TEST_WA_SIZE    THD_WA_SIZE(256)
static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[])
{
    Thread *tp;

    (void)argv;
    if (argc > 0)
    {
        chprintf(chp, "Usage: test\r\n");
        return;
    }
    tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(), TestThread, chp);
    if (tp == NULL)
    {
        chprintf(chp, "out of memory\r\n");
        return;
    }
    chThdWait(tp);
}

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
static const ShellCommand commands[] =
{
    {"mem", cmd_mem},
    {"threads", cmd_threads},
    {"test", cmd_test},
    {"stop", cmd_stop},
    {"standby", cmd_standby},
    {"bkp", cmd_bkp},
    {"reset", cmd_reset},
    {"flash", cmd_flash},
    {NULL, NULL}
};

static const ShellConfig shell_cfg1 =
{
    (BaseSequentialStream *)&SDU,
    commands
};

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */

class blinker_thd : public BaseStaticThread<128>
{
    protected:
        virtual msg_t main(void)
        {
            systime_t time;
            setName("blinker");
            while (true)
            {
                time = SDU.config->usbp->state == USB_ACTIVE ? 250 : 500;
                palClearPad(GPIOB, GPIOB_LED1);
                sleep(time);
                palSetPad(GPIOB, GPIOB_LED1);
                sleep(time);
            }
            // TODO: How to get rid of the compiler warning ? the demos do not return anything from their blinkers either...        
        }

    public:
        // Empty constructor
        blinker_thd(void) : BaseStaticThread<128>() { }
};

static blinker_thd blinky;


/*
 * Application entry point.
 */
int main(void)
{
    Thread *shelltp = NULL;

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    System::init();

    /*
     * Initializes a serial-over-USB CDC driver.
     */
    usb_serial_init();


    /*
     * Shell manager initialization.
     */
    shellInit();

    /*
     * Creates the blinker thread.
     */
    blinky.start(NORMALPRIO);
    palClearPad(GPIOB, GPIOB_LED2);


    /*
     * Normal main() thread activity, in this demo it does nothing except
     * sleeping in a loop and check the USB state.
     */
    while (TRUE) {
        if (!shelltp && (SDU.config->usbp->state == USB_ACTIVE)) {
            palSetPad(GPIOB, GPIOB_LED2);
            shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
        } else if (chThdTerminated(shelltp)) {
            chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
            shelltp = NULL;           /* Triggers spawning of a new shell.        */
            palClearPad(GPIOB, GPIOB_LED2);
        }
        BaseThread::sleep(MS2ST(500));
    }
    
    return 0;
}
