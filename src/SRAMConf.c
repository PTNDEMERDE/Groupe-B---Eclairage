/*
 * SRAMConf.c
 *
 * This file provides storage definitions and simple helpers for the
 * application's state values that are persisted into an external
 * 23LC1024 SPI SRAM device. Many of the state variables are declared
 * `extern` in `SRAMConf.h`; this translation unit allocates the storage
 * and sets initial defaults.
 *
 * The macros like `LAMP1WRITE` and `LAMP2_PWM_VALUE_WRITE` are expected
 * to be defined elsewhere (headers) and perform the actual SRAM write
 * operations using the `SRAM23LC1024` SPI interface.
 */

#include "SRAM23LC1024.h"
#include "SPI.h"
#include "SRAMConf.h"

/*
 * Persistent state variables (allocated here):
 * - LAMPx_State: boolean flag for each lamp physical on/off state
 * - LAMP2_PWM_State: boolean indicating whether Lamp2 uses PWM dimming
 * - LAMP_All_PWM_State: global indicator if all lamps are in PWM mode
 * - RTC_State: whether RTC-based automatic control is enabled
 * - LAMP2_PWM_Value: current manual PWM setting for Lamp2 (0..100 percent)
 * - LAMP2_PWM_Auto_State: whether automatic PWM mode for Lamp2 is enabled
 *
 * These are initialized to safe defaults (FALSE/0). Values are written
 * to the external SRAM using the macros below when `SRAM_Save_All()` is
 * called.
 */
unsigned char LAMP1_State           = FALSE; /* Lamp1 physical state (0=off,1=on) */
unsigned char LAMP2_State           = FALSE; /* Lamp2 physical state (0=off,1=on) */
unsigned char LAMP3_State           = FALSE; /* Lamp3 physical state (0=off,1=on) */
unsigned char LAMP4_State           = FALSE; /* Lamp4 physical state (0=off,1=on) */
unsigned char LAMP2_PWM_State       = FALSE; /* Lamp2 using PWM dimming flag */
unsigned char LAMP_All_PWM_State    = FALSE; /* Global PWM mode flag for all lamps */
unsigned char RTC_State             = FALSE; /* RTC automatic control enabled */
unsigned char LAMP2_PWM_Value       = 0;     /* Lamp2 PWM percent (0..100) */
unsigned char LAMP2_PWM_Auto_State  = FALSE; /* Lamp2 auto PWM enabled flag */

/*
 * SRAM_Save_All()
 *
 * Write the current in-memory lamp states and PWM settings to the
 * external SRAM so they persist across resets or power cycles. The
 * actual write operations are performed via macros (e.g. LAMP1WRITE)
 * which wrap the SRAM driver calls and the correct SRAM addresses.
 *
 * This function performs a full-state save; callers should ensure the
 * variables contain the desired values before calling it.
 */
void SRAM_Save_All(void){

    /* Write each lamp state and related flags into the external SRAM */
    LAMP1WRITE;
    LAMP2WRITE;
    LAMP3WRITE;
    LAMP4WRITE;
    LAMP2_PWM_WRITE;
    LAMP_All_PWM_WRITE;
    RTCWRITE;
    LAMP2_PWM_VALUE_WRITE;
    LAMP2_PWM_AUTO_WRITE;

}

/* End of SRAMConf.c */

