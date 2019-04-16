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

    /*��ʱ5��Tick����ʱ�����������ִ��ʣ�������и����ȼ�������(g_uwTskLoID����)*/
    uwRet = LOS_TaskDelay(5);
    if (uwRet != LOS_OK)
    {
        dprintf("Delay Task Failed.\r\n");
        return LOS_NOK;
    }

    /*2��tickʱ�䵽�˺󣬸�����ָ�������ִ��*/
    dprintf("Enter TaskHi LOS_TaskDelay Done.\r\n");

    /*������������*/
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
    
    /*ɾ������*/
    if(LOS_OK != LOS_TaskDelete(g_uwTskHiID))
    {
        dprintf("TaskHi delete failed .\n");
        return LOS_NOK;
    }
    
    return LOS_OK;
}

/*�����ȼ�������ں���*/
static UINT32 Example_TaskLo_1(VOID)
{
    UINT32 uwRet;

    dprintf("Enter TaskLo Handler.\r\n");

    /*��ʱ10��Tick����ʱ�����������ִ��ʣ�������о͸����ȼ�������(��������)*/
    uwRet = LOS_TaskDelay(10);
    if (uwRet != LOS_OK)
    {
        dprintf("Delay TaskLo Failed.\r\n");
        return LOS_NOK;
    }

    dprintf("TaskHi LOS_TaskSuspend Success.\r\n");

    /*�ָ������������g_uwTskHiID*/
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
    
    /*ɾ������*/
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


