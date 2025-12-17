/**
 * @file tpl_os_stubs.c
 *
 * @section desc File description
 *
 * Stub implementations of Trampoline OS API functions
 * These are minimal stubs to allow linking. In a real system,
 * these would be provided by the Trampoline RTOS kernel.
 */

#include <stddef.h>
#include "tpl_os.h"

/* Global variables */
static TaskStateType current_task_state[3] = {SUSPENDED, SUSPENDED, SUSPENDED};
static EventMaskType task_events[3] = {0, 0, 0};
static volatile uint8_t resource_locked[1] = {0};

/* OS Control */
void StartOS(AppModeType mode)
{
    (void)mode;
    
    /* Call startup hook */
    StartupHook();
    
    /* In a real implementation, this would start the scheduler */
    /* For now, we return to main() to run a simple loop */
    /* The infinite loop is now in main() */
}

void ShutdownOS(StatusType error)
{
    ShutdownHook(error);
    while(1); /* Halt */
}

/* Task Management */
StatusType ActivateTask(TaskType TaskID)
{
    if (TaskID >= 3) {
        return E_OS_ID;
    }
    
    current_task_state[TaskID] = READY;
    return E_OK;
}

StatusType TerminateTask(void)
{
    /* In a real system, this would switch to the next task */
    return E_OK;
}

StatusType ChainTask(TaskType TaskID)
{
    (void)TaskID;
    return E_OK;
}

StatusType Schedule(void)
{
    return E_OK;
}

StatusType GetTaskID(TaskRefType TaskID)
{
    if (TaskID == NULL) {
        return E_OS_VALUE;
    }
    
    *TaskID = 0; /* Stub: return first task */
    return E_OK;
}

StatusType GetTaskState(TaskType TaskID, TaskStateRefType State)
{
    if (TaskID >= 3) {
        return E_OS_ID;
    }
    
    if (State == NULL) {
        return E_OS_VALUE;
    }
    
    *State = current_task_state[TaskID];
    return E_OK;
}

/* Event Management */
StatusType SetEvent(TaskType TaskID, EventMaskType Mask)
{
    if (TaskID >= 3) {
        return E_OS_ID;
    }
    
    task_events[TaskID] |= Mask;
    return E_OK;
}

StatusType ClearEvent(EventMaskType Mask)
{
    /* Stub: clear events for current task (assume task 0) */
    task_events[0] &= ~Mask;
    return E_OK;
}

StatusType GetEvent(TaskType TaskID, EventMaskRefType Event)
{
    if (TaskID >= 3) {
        return E_OS_ID;
    }
    
    if (Event == NULL) {
        return E_OS_VALUE;
    }
    
    *Event = task_events[TaskID];
    return E_OK;
}

StatusType WaitEvent(EventMaskType Mask)
{
    (void)Mask;
    /* In a real system, this would block until the event is set */
    return E_OK;
}

/* Resource Management */
StatusType GetResource(ResourceType ResID)
{
    if (ResID >= 1) {
        return E_OS_ID;
    }
    
    if (resource_locked[ResID]) {
        return E_OS_ACCESS;
    }
    
    resource_locked[ResID] = 1;
    return E_OK;
}

StatusType ReleaseResource(ResourceType ResID)
{
    if (ResID >= 1) {
        return E_OS_ID;
    }
    
    if (!resource_locked[ResID]) {
        return E_OS_NOFUNC;
    }
    
    resource_locked[ResID] = 0;
    return E_OK;
}

/* Alarm Management */
StatusType GetAlarmBase(AlarmType AlarmID, void* Info)
{
    (void)AlarmID;
    (void)Info;
    return E_OK;
}

StatusType GetAlarm(AlarmType AlarmID, TickRefType Tick)
{
    (void)AlarmID;
    (void)Tick;
    return E_OK;
}

StatusType SetRelAlarm(AlarmType AlarmID, TickType increment, TickType cycle)
{
    (void)AlarmID;
    (void)increment;
    (void)cycle;
    return E_OK;
}

StatusType SetAbsAlarm(AlarmType AlarmID, TickType start, TickType cycle)
{
    (void)AlarmID;
    (void)start;
    (void)cycle;
    return E_OK;
}

StatusType CancelAlarm(AlarmType AlarmID)
{
    (void)AlarmID;
    return E_OK;
}
