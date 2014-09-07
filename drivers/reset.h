#ifndef RESET_H
#define RESET_H
/**
 * Needed by enter_bootloader
 */
extern uint32_t __ram_end__;
#define SYMVAL(sym) (uint32_t)(((uint8_t *)&(sym)) - ((uint8_t *)0))

#ifdef __cplusplus
extern "C" {
#endif

void reset(void); // This is basically wrapper for NVIC_SystemReset();
void enter_bootloader(void); // This implements the trick from http://forum.chibios.org/phpbb/viewtopic.php?p=14781#p14781, requires swreset.patch

void cmd_reset(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_flash(BaseSequentialStream *chp, int argc, char *argv[]);

#ifdef __cplusplus
}
#endif


#endif