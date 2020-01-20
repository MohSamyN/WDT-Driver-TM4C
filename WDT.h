#ifndef WDT_H_
#define WDT_H_
#include <stdint.h>
#include "WDT_Cfg.h"

typedef enum {DISABLED, ENABLED} EnableSelect;
typedef enum {MASKABLE, NON_MASKABLE} InterruptType;

typedef struct
{
    /* Selecting WDT0 or WDT1 module */
    uint8_t WDTID;
    /* Loading the time in milliseconds (WDT0 max: 268000ms - WDT1 max: 1072000ms) */
    uint32_t LoadTimeMS;
    /* Selecting whether the interrupt is maskable or non-maskable */
    InterruptType IntType;
    /* Selecting whether the reset is disabled or enabled */
    EnableSelect Reset;
    /* Selecting whether to stop watchdog when debug modes */
} WDT_CfgType;

typedef enum {WDT_OK, WDT_NOK} WDT_CheckType;

extern const WDT_CfgType WDT_ConfigParameters[WDT_GROUPS_NUMBER];


WDT_CheckType WDT_Init(void);
WDT_CheckType WDT_Start(uint8_t ModuleID);
WDT_CheckType WDT_Stop(uint8_t ModuleID);


#endif /* WDT_H_ */
