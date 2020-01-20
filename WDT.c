#include <stdint.h>
#include "M4MemMap.h"
#include "WDT.h"
#include "WDT_Cfg.h"


typedef volatile uint32_t* const WDT_RegAddType;

#define MODULES_NUMBER          2u

#define WDT0_BASE_ADDRESS       0x40000000
#define WDT1_BASE_ADDRESS       0x40001000

static const uint32_t ModulesBaseAddressLUT[MODULES_NUMBER] =
{
    WDT0_BASE_ADDRESS,
    WDT1_BASE_ADDRESS
};

#define WDT_REG_ADDRESS(WDT_ID, REG_OFFSET)     (ModulesBaseAddressLUT[WDT_ID] + REG_OFFSET)

#define WDTLOAD(WDT_ID)                         *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0x000))
#define WDTVALUE(WDT_ID)                        *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0x004))
#define WDTCTL(WDT_ID)                          *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0x008))
#define WDTICR(WDT_ID)                          *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0x00C))
#define WDTRIS(WDT_ID)                          *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0x010))
#define WDTMIS(WDT_ID)                          *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0x014))
#define WDTTEST(WDT_ID)                         *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0x418))
#define WDTLOCK(WDT_ID)                         *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xC00))
#define WDTPeriphID4(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFD0))
#define WDTPeriphID5(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFD4))
#define WDTPeriphID6(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFD8))
#define WDTPeriphID7(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFDC))
#define WDTPeriphID0(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFE0))
#define WDTPeriphID1(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFE4))
#define WDTPeriphID2(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFE8))
#define WDTPeriphID3(WDT_ID)                    *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFEC))
#define WDTPCellID0(WDT_ID)                     *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFF0))
#define WDTPCellID1(WDT_ID)                     *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFF4))
#define WDTPCellID2(WDT_ID)                     *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFF8))
#define WDTPCellID3(WDT_ID)                     *((WDT_RegAddType)WDT_REG_ADDRESS(WDT_ID, 0xFFC))

static uint8_t WDT_ModuleState[MODULES_NUMBER] = {0};

WDT_CheckType WDT_Init(void)
{
    WDT_CheckType RetVar;
    const WDT_CfgType* CfgPtr;
    uint8_t ErrorFlag = 0;
    uint8_t LoopIndex;
    for(LoopIndex = 0; ((LoopIndex < WDT_GROUPS_NUMBER) && (ErrorFlag == 0)); LoopIndex++)
    {
        if(WDT_ConfigParameters[LoopIndex].WDTID < MODULES_NUMBER)
        {
            CfgPtr = &WDT_ConfigParameters[LoopIndex];

            /* Enabling the clock for the appropriate WDT module */
            RCGCWD_REG |= (1 << CfgPtr->WDTID);

            /* Loading the LoadTime value into the load register */
            WDTLOAD(CfgPtr->WDTID) = (CfgPtr->LoadTimeMS * 16000);

            /* Waiting for the WRC bit to be set (for WDT1 only) */
            if(CfgPtr->WDTID == 1)
            {
                while((WDTCTL(CfgPtr->WDTID) & 0x80000000) == 0);
            }
            else
            {
            }

            /* Enabling or disabling the reset */
            WDTCTL(CfgPtr->WDTID) |= (CfgPtr->Reset << 1);

            /* Selecting maskable or non-maskable interrupt */
            WDTCTL(CfgPtr->WDTID) |= (CfgPtr->IntType << 2);

            /* Waiting for the WRC bit to be set (for WDT1 only) */
            if(CfgPtr->WDTID == 1)
            {
                while((WDTCTL(CfgPtr->WDTID) & 0x80000000) == 0);
            }
            else
            {
            }

            WDT_ModuleState[LoopIndex] = 1;
            RetVar = WDT_OK;
        }
        else
        {
            ErrorFlag = 1;
            RetVar = WDT_NOK;
        }
    }
    return RetVar;
}

WDT_CheckType WDT_Start(uint8_t ModuleID)
{
    WDT_CheckType RetVar;
    const WDT_CfgType* CfgPtr;
    if(ModuleID < WDT_GROUPS_NUMBER)
    {
        if(WDT_ModuleState[ModuleID] == 1)
        {
            CfgPtr = &WDT_ConfigParameters[ModuleID];
            /* Enabling the watchdog timer */
            WDTCTL(CfgPtr->WDTID) |= 0x01;
            RetVar = WDT_OK;
        }
        else
        {
            RetVar = WDT_NOK;
        }
    }
    else
    {
        RetVar = WDT_NOK;
    }
    return RetVar;
}

WDT_CheckType WDT_Stop(uint8_t ModuleID)
{
    WDT_CheckType RetVar;
    const WDT_CfgType* CfgPtr;
    if(ModuleID < WDT_GROUPS_NUMBER)
    {
        if(WDT_ModuleState[ModuleID] == 1)
        {
            CfgPtr = &WDT_ConfigParameters[ModuleID];
            /* Disabling the watchdog timer */
            WDTCTL(CfgPtr->WDTID) &= ~(0x01);
            /* Reloading the timer with the same value */
            WDTLOAD(CfgPtr->WDTID) = (CfgPtr->LoadTimeMS * 16000);
            /* Waiting for the WRC bit to be set (for WDT1 only) */
            if(CfgPtr->WDTID == 1)
            {
                while((WDTCTL(CfgPtr->WDTID) & 0x80000000) == 0);
            }
            else
            {
            }
            RetVar = WDT_OK;
        }
        else
        {
            RetVar = WDT_NOK;
        }
    }
    else
    {
        RetVar = WDT_NOK;
    }
    return RetVar;
}
