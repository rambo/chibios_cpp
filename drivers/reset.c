#include "ch.h"
#include "chprintf.h"
#include "reset.h"
#include "hal.h"


/**
 * SW-reset of the board
 */
void reset(void)
{
    NVIC_SystemReset();
}

void cmd_reset(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    (void)argc;
    chprintf(chp, "Resetting, bye!\r\n");
    chThdSleepMilliseconds(100);
    reset();
}

/**
 * Tag end-of-ram with a magic word (checked by the reset handler) and reboot
 */
void enter_bootloader(void)
{
    *((unsigned long *)(SYMVAL(__ram_end__) - 4)) = 0xDEADBEEF; // set magic flag => reset handler will jump into boot loader
    NVIC_SystemReset();
}

void cmd_flash(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    (void)argc;
    chprintf(chp, "Going to bootloader, bye!\r\n");
    chThdSleepMilliseconds(100);
    enter_bootloader();
}

