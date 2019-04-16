#include "stm32l4xx_hal.h"
#include "gpio.h"

#include "los_demo_debug.h"
#include "los_inspect_entry.h"

#include "los_typedef.h"
#include "los_queue.h"
#include "los_task.h"

static UINT32 g_uwTskHiID;    
static UINT32 g_uwTskLoID;   

#define TSK_PRIOR_HI 4  
#define TSK_PRIOR_LO 5  


static UINT32 Example_TaskHi_1(VOID)
{
    UINT32 uwRet = LOS_OK;

    dprintf("Enter TaskHi Handler.\r\n");

    /*延时5个Tick，延时后该任务会挂起，执行剩余任务中高优先级的任务(g_uwTskLoID任务)*/
    uwRet = LOS_TaskDelay(5);
    if (uwRet != LOS_OK)
    {
        dprintf("Delay Task Failed.\r\n");
        return LOS_NOK;
    }

    /*2个tick时间到了后，该任务恢复，继续执行*/
    dprintf("Enter TaskHi LOS_TaskDelay Done.\r\n");

    /*挂起自身任务*/
    uwRet = LOS_TaskSuspend(g_uwTskHiID);
    if (uwRet != LOS_OK)
    {
        dprintf("Suspend TaskHi Failed.\r\n");
        uwRet = LOS_InspectStatusSetByID(LOS_INSPECT_TASK,LOS_INSPECT_STU_ERROR);
        if (LOS_OK != uwRet)
        {
            dprintf("Set Inspect Status Err\n");
        }
        return LOS_NOK;
    }
    
    dprintf("TaskHi LOS_TaskResume Success.\r\n");
        
    uwRet = LOS_InspectStatusSetByID(LOS_INSPECT_TASK,LOS_INSPECT_STU_SUCCESS);
    if (LOS_OK != uwRet)
    {
        dprintf("Set Inspect Status Err\n");
    }
    
    /*删除任务*/
    if(LOS_OK != LOS_TaskDelete(g_uwTskHiID))
    {
        dprintf("TaskHi delete failed .\n");
        return LOS_NOK;
    }
    
    return LOS_OK;
}

/*低优先级任务入口函数*/
static UINT32 Example_TaskLo_1(VOID)
{
    UINT32 uwRet;

    dprintf("Enter TaskLo Handler.\r\n");

    /*延时10个Tick，延时后该任务会挂起，执行剩余任务中就高优先级的任务(背景任务)*/
    uwRet = LOS_TaskDelay(10);
    if (uwRet != LOS_OK)
    {
        dprintf("Delay TaskLo Failed.\r\n");
        return LOS_NOK;
    }

    dprintf("TaskHi LOS_TaskSuspend Success.\r\n");

    /*恢复被挂起的任务g_uwTskHiID*/
    uwRet = LOS_TaskResume(g_uwTskHiID);
    if (uwRet != LOS_OK)
    {
        dprintf("Resume TaskHi Failed.\r\n");
        uwRet = LOS_InspectStatusSetByID(LOS_INSPECT_TASK,LOS_INSPECT_STU_ERROR);
        if (LOS_OK != uwRet)  
        {
            dprintf("Set Inspect Status Err\n");
        }
        return LOS_NOK;
    }
    
    /*删除任务*/
    if(LOS_OK != LOS_TaskDelete(g_uwTskLoID))
    {
        dprintf("TaskLo delete failed .\n");
        
        return LOS_NOK;
    }
    
    return LOS_OK;  
}

UINT32 Example_Entry(VOID)
{
	UINT32 uwRet = LOS_OK;
	TSK_INIT_PARAM_S stInitParam = {0};
	
	dprintf("LOS_TaskLock() Success!\r\n");
	
	stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_TaskHi_1;
	stInitParam.usTaskPrio = TSK_PRIOR_HI;
	stInitParam.pcName = "Task1";
	stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
	uwRet = LOS_TaskCreate(&g_uwTskHiID, &stInitParam);
	if (uwRet != LOS_OK) {
		printf("Example_Task1 create Failed!\r\n");
		return LOS_NOK;
	}
	dprintf("Example_TaskHi create Success!\r\n");
	
	stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_TaskLo_1;
	stInitParam.usTaskPrio = TSK_PRIOR_LO;
	stInitParam.pcName = "Task2";
	stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
	uwRet = LOS_TaskCreate(&g_uwTskLoID, &stInitParam);
	if (uwRet != LOS_OK) {
		printf("Example_Task2 create Failed!\r\n");
		return LOS_NOK;
	}
	dprintf("Example_TaskLo create Success!\r\n");
	
    return uwRet;
}


