#ifndef __TASK_MANAGEMENT_H__
#define __TASK_MANAGEMENT_H__

/*---------------------------------- Include ---------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "main.h"

/*----------------------------------- Public Definitions ----------------------------------*/
#define MAX_TASKS_NUM                   5
#define TASK_ID_DEF(timerID)            (*timerID = taskMannage.numOfTasks)        
typedef uint8_t                         TaskIDType;

typedef enum 
{
    TASK_MODE_REPEATED = 0,
    TASK_MODE_SINGLE = 1
}TaskModeEnum;

typedef void (*TaskHandler)(void);
typedef struct 
{
    TaskIDType id;
    TaskModeEnum mode;
    TaskHandler handler;
    uint32_t period;
    bool executeFlag;
}TaskType;

typedef struct 
{
    uint32_t time;
    uint8_t numOfTasks;
    TaskType tasksArray[MAX_TASKS_NUM];
}TaskMannageType;


#define taskGetTime()       millis()     

/*------------------------------- Public Functions Prototype ------------------------------*/
extern void taskCreate(TaskIDType * taskID, TaskModeEnum taskMode, TaskHandler taskHandler);
extern void taskStart(TaskIDType taskID, uint16_t taskPeriod);
extern void taskStop(TaskIDType taskID);
extern void tasksRun(void);
extern void tasksSetup(void);
#endif // !__TASK_MANAGEMENT_H__