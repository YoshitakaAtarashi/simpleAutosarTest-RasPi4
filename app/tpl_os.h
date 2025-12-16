/**
 * @file tpl_os.h
 *
 * @section desc File description
 *
 * Trampoline OS main header file (stub for basic compilation)
 * In a full Trampoline build, this would be generated from the OIL file.
 *
 * This is a minimal stub to allow basic compilation without running goil.
 */

#ifndef TPL_OS_H
#define TPL_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include minimal type definitions */
#include <stdint.h>

/* Basic type definitions */
#ifndef STATUSTYPE_DEFINED
#define STATUSTYPE_DEFINED
typedef unsigned char StatusType;
#endif

/* Standard AUTOSAR/OSEK return codes */
#define E_OK                    ((StatusType)0)
#define E_OS_ACCESS             ((StatusType)1)
#define E_OS_CALLEVEL           ((StatusType)2)
#define E_OS_ID                 ((StatusType)3)
#define E_OS_LIMIT              ((StatusType)4)
#define E_OS_NOFUNC             ((StatusType)5)
#define E_OS_RESOURCE           ((StatusType)6)
#define E_OS_STATE              ((StatusType)7)
#define E_OS_VALUE              ((StatusType)8)

/* Application mode type */
typedef unsigned char AppModeType;
#define OSDEFAULTAPPMODE        ((AppModeType)0)

/* Task types */
typedef uint16_t TaskType;
typedef TaskType* TaskRefType;
typedef uint8_t TaskStateType;
typedef TaskStateType* TaskStateRefType;

/* Task states */
#define RUNNING                 ((TaskStateType)0)
#define WAITING                 ((TaskStateType)1)
#define READY                   ((TaskStateType)2)
#define SUSPENDED               ((TaskStateType)3)

/* Event types */
typedef uint32_t EventMaskType;
typedef EventMaskType* EventMaskRefType;

/* Resource type */
typedef uint8_t ResourceType;

/* Alarm types */
typedef uint16_t AlarmType;
typedef uint32_t TickType;
typedef TickType* TickRefType;

/* Counter types */
typedef uint16_t CounterType;

/* Hook prototypes - to be implemented by user */
void StartupHook(void);
void ShutdownHook(StatusType error);
void ErrorHook(StatusType error);

/* TASK and ISR macros */
#define TASK(taskname)      void taskname##_func(void)
#define ISR(isrname)        void isrname##_handler(void)
#define DeclareTask(name)   extern void name##_func(void)
#define DeclareISR(name)    extern void name##_handler(void)
#define DeclareEvent(name)  extern const EventMaskType name
#define DeclareResource(name) extern const ResourceType name

/* Application-specific objects - normally generated from OIL file */
/* Tasks */
#define TaskSerial          0
#define TaskBlink           1
#define TaskProcess         2

/* Events */
#define EvtDataReady        ((EventMaskType)0x01)

/* Resources */
#define ResUart             ((ResourceType)0)

/* Basic OS API prototypes */
void StartOS(AppModeType mode);
void ShutdownOS(StatusType error);

/* Task management */
StatusType ActivateTask(TaskType TaskID);
StatusType TerminateTask(void);
StatusType ChainTask(TaskType TaskID);
StatusType Schedule(void);
StatusType GetTaskID(TaskRefType TaskID);
StatusType GetTaskState(TaskType TaskID, TaskStateRefType State);

/* Event management */
StatusType SetEvent(TaskType TaskID, EventMaskType Mask);
StatusType ClearEvent(EventMaskType Mask);
StatusType GetEvent(TaskType TaskID, EventMaskRefType Event);
StatusType WaitEvent(EventMaskType Mask);

/* Resource management */
StatusType GetResource(ResourceType ResID);
StatusType ReleaseResource(ResourceType ResID);

/* Alarm management */
StatusType GetAlarmBase(AlarmType AlarmID, void* Info);
StatusType GetAlarm(AlarmType AlarmID, TickRefType Tick);
StatusType SetRelAlarm(AlarmType AlarmID, TickType increment, TickType cycle);
StatusType SetAbsAlarm(AlarmType AlarmID, TickType start, TickType cycle);
StatusType CancelAlarm(AlarmType AlarmID);

#ifdef __cplusplus
}
#endif

#endif /* TPL_OS_H */
