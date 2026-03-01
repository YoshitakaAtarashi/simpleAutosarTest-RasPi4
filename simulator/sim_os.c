/**
 * @file sim_os.c
 * @brief PC simulator implementation of the Trampoline AUTOSAR OS API
 *
 * Replaces app/tpl_os_stubs.c in the simulator build.
 *
 * Key difference from the hardware stubs:
 *   StartOS() runs a real time-based task scheduler that calls the
 *   AUTOSAR task functions at the periods configured in app/app.oil:
 *
 *     Task      Period   First fire
 *     --------  -------  ----------
 *     Task100ms   100 ms     10 ms
 *     TaskBlink   500 ms     50 ms
 *     TaskSerial 1000 ms    100 ms
 *     Task1000ms 1000 ms    100 ms
 *
 *   TaskProcess is event-driven (UART ISR) and is not scheduled here.
 *
 * Timing is provided by clock_gettime(CLOCK_MONOTONIC) on POSIX systems
 * and GetTickCount() on Windows.
 *
 * The simulation runs until the process is interrupted (Ctrl+C).
 * Override SIM_DURATION_SEC at compile time to add a finite timeout:
 *   gcc -DSIM_DURATION_SEC=30 ...
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "tpl_os.h"

/* ------------------------------------------------------------------ */
/*  Platform timing helpers                                            */
/* ------------------------------------------------------------------ */

#ifdef _WIN32
#  include <windows.h>
static DWORD sim_start_ticks;
static long sim_get_elapsed_ms(void)
{
    return (long)(GetTickCount() - sim_start_ticks);
}
static void sim_sleep_ms(long ms)
{
    if (ms > 0) Sleep((DWORD)ms);
}
#else
#  include <time.h>
#  include <unistd.h>
static struct timespec sim_start_time;
static long sim_get_elapsed_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (long)((now.tv_sec  - sim_start_time.tv_sec)  * 1000L +
                  (now.tv_nsec - sim_start_time.tv_nsec) / 1000000L);
}
static void sim_sleep_ms(long ms)
{
    if (ms > 0) usleep((unsigned int)(ms * 1000));
}
#endif

/* ------------------------------------------------------------------ */
/*  Forward declarations for AUTOSAR task functions (defined in       */
/*  app/main.c via the TASK() macro which expands to                  */
/*  void TaskName_func(void) )                                         */
/* ------------------------------------------------------------------ */

extern void TaskSerial_func(void);
extern void TaskBlink_func(void);
extern void Task100ms_func(void);
extern void Task1000ms_func(void);

/* ------------------------------------------------------------------ */
/*  Internal OS state                                                  */
/* ------------------------------------------------------------------ */

#define MAX_TASKS     5
#define MAX_RESOURCES 1

static TaskStateType task_state[MAX_TASKS];
static EventMaskType task_events[MAX_TASKS];
static volatile uint8_t resource_locked[MAX_RESOURCES];

/* ------------------------------------------------------------------ */
/*  OS Control                                                         */
/* ------------------------------------------------------------------ */

void StartOS(AppModeType mode)
{
    (void)mode;

    /* Initialise OS state */
    for (int i = 0; i < MAX_TASKS; i++) {
        task_state[i]  = SUSPENDED;
        task_events[i] = 0;
    }
    for (int i = 0; i < MAX_RESOURCES; i++) {
        resource_locked[i] = 0;
    }

    /* Call user startup hook (initialises output + prints banner) */
    StartupHook();

    /* ---- Simulator scheduler ---- */
    printf("\n[SIM] AUTOSAR task scheduler started\n");
    printf("[SIM] Press Ctrl+C to stop the simulation\n\n");
    fflush(stdout);

    /* Record simulation start time */
#ifdef _WIN32
    sim_start_ticks = GetTickCount();
#else
    clock_gettime(CLOCK_MONOTONIC, &sim_start_time);
#endif

    /* Alarm initial fire times (from app.oil ALARMTIME values) */
    long next_100ms   = 10;    /* Alarm100ms:   first at  10 ms */
    long next_blink   = 50;    /* AlarmBlink:   first at  50 ms */
    long next_serial  = 100;   /* AlarmSerial:  first at 100 ms */
    long next_1000ms  = 100;   /* Alarm1000ms:  first at 100 ms */

#ifdef SIM_DURATION_SEC
    long end_ms = (long)(SIM_DURATION_SEC) * 1000L;
#else
    long end_ms = -1L; /* run until interrupted */
#endif

    while (1) {
        long elapsed = sim_get_elapsed_ms();

#ifdef SIM_DURATION_SEC
        if (elapsed >= end_ms) {
            printf("\n[SIM] Simulation finished (%d seconds)\n",
                   SIM_DURATION_SEC);
            fflush(stdout);
            ShutdownOS(E_OK);
            return; /* ShutdownOS loops; should not reach here */
        }
#endif

        /* Task100ms – 100 ms cycle */
        if (elapsed >= next_100ms) {
            next_100ms += 100;
            Task100ms_func();
        }

        /* TaskBlink – 500 ms cycle */
        if (elapsed >= next_blink) {
            next_blink += 500;
            TaskBlink_func();
        }

        /* TaskSerial – 1000 ms cycle */
        if (elapsed >= next_serial) {
            next_serial += 1000;
            TaskSerial_func();
        }

        /* Task1000ms – 1000 ms cycle */
        if (elapsed >= next_1000ms) {
            next_1000ms += 1000;
            Task1000ms_func();
        }

        /* Sleep 10 ms between scheduler ticks to avoid busy-waiting */
        sim_sleep_ms(10);
    }
}

void ShutdownOS(StatusType error)
{
    ShutdownHook(error);
    exit((int)error); /* Terminate the simulator process */
}

/* ------------------------------------------------------------------ */
/*  Task Management                                                    */
/* ------------------------------------------------------------------ */

StatusType ActivateTask(TaskType TaskID)
{
    if (TaskID >= MAX_TASKS) return E_OS_ID;
    task_state[TaskID] = READY;
    return E_OK;
}

StatusType TerminateTask(void)
{
    /* In the simulator tasks are plain function calls; just return. */
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
    if (TaskID == NULL) return E_OS_VALUE;
    *TaskID = 0;
    return E_OK;
}

StatusType GetTaskState(TaskType TaskID, TaskStateRefType State)
{
    if (TaskID >= MAX_TASKS) return E_OS_ID;
    if (State == NULL)       return E_OS_VALUE;
    *State = task_state[TaskID];
    return E_OK;
}

/* ------------------------------------------------------------------ */
/*  Event Management                                                   */
/* ------------------------------------------------------------------ */

StatusType SetEvent(TaskType TaskID, EventMaskType Mask)
{
    if (TaskID >= MAX_TASKS) return E_OS_ID;
    task_events[TaskID] |= Mask;
    return E_OK;
}

StatusType ClearEvent(EventMaskType Mask)
{
    task_events[0] &= ~Mask;
    return E_OK;
}

StatusType GetEvent(TaskType TaskID, EventMaskRefType Event)
{
    if (TaskID >= MAX_TASKS) return E_OS_ID;
    if (Event == NULL)       return E_OS_VALUE;
    *Event = task_events[TaskID];
    return E_OK;
}

StatusType WaitEvent(EventMaskType Mask)
{
    (void)Mask;
    /* In the simulator, WaitEvent returns immediately (non-blocking). */
    return E_OK;
}

/* ------------------------------------------------------------------ */
/*  Resource Management                                                */
/* ------------------------------------------------------------------ */

StatusType GetResource(ResourceType ResID)
{
    if (ResID >= MAX_RESOURCES) return E_OS_ID;
    if (resource_locked[ResID]) return E_OS_ACCESS;
    resource_locked[ResID] = 1;
    return E_OK;
}

StatusType ReleaseResource(ResourceType ResID)
{
    if (ResID >= MAX_RESOURCES)  return E_OS_ID;
    if (!resource_locked[ResID]) return E_OS_NOFUNC;
    resource_locked[ResID] = 0;
    return E_OK;
}

/* ------------------------------------------------------------------ */
/*  Alarm Management (stubs – alarms are handled by the scheduler)    */
/* ------------------------------------------------------------------ */

StatusType GetAlarmBase(AlarmType AlarmID, void *Info)
{
    (void)AlarmID; (void)Info;
    return E_OK;
}

StatusType GetAlarm(AlarmType AlarmID, TickRefType Tick)
{
    (void)AlarmID; (void)Tick;
    return E_OK;
}

StatusType SetRelAlarm(AlarmType AlarmID, TickType increment, TickType cycle)
{
    (void)AlarmID; (void)increment; (void)cycle;
    return E_OK;
}

StatusType SetAbsAlarm(AlarmType AlarmID, TickType start, TickType cycle)
{
    (void)AlarmID; (void)start; (void)cycle;
    return E_OK;
}

StatusType CancelAlarm(AlarmType AlarmID)
{
    (void)AlarmID;
    return E_OK;
}
