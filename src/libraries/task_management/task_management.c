/*----------------------------------- Include ----------------------------------------------*/
#include "task_management.h"
/*----------------------------------- Private Definitions ----------------------------------*/

/*----------------------------------- Private Functions ------------------------------------*/

/*----------------------------------- Private Variables ------------------------------------*/

/*----------------------------------- Public Variables -------------------------------------*/
TaskMannageType taskMannage;





/*********** Private Functions Declaration ****************/
/**
 * @name 
 * @brief 
 * @param
 * @return   
 */


/************* Public Functions Declaration ******************/
/**
 * @name 
 * @brief 
 * @param
 * @return   
 */
void taskCreate(uint8_t * taskID, TaskModeEnum taskMode, TaskHandler taskHandler)
{
    TaskType task;
    TASK_ID_DEF(taskID);
    task.id = *taskID;
    task.mode = taskMode;
    task.handler = taskHandler;
    task.period = 0;
    task.executeFlag = false;
    taskMannage.tasksArray[*taskID] = task;
    taskMannage.numOfTasks++;
}

/**
 * @name 
 * @brief 
 * @param
 * @return
 */
void taskStart(uint8_t taskID, uint16_t taskPeriod)
{
    taskMannage.tasksArray[taskID].period = taskPeriod;
}

/**
 * @name 
 * @brief 
 * @param
 * @return   
 */
void tasksRun(void)
{
    taskMannage.time = taskGetTime();
    for(uint8_t i = 0; i < taskMannage.numOfTasks; i++)
    {
        if(taskMannage.tasksArray[i].period != 0)
        {
            if(taskMannage.time % taskMannage.tasksArray[i].period == 0
               && taskMannage.time >= taskMannage.tasksArray[i].period)
            {
                if(taskMannage.tasksArray[i].executeFlag == false)
                {
                    taskMannage.tasksArray[i].handler();
                    taskMannage.tasksArray[i].executeFlag = true;
                }
            }
            else
            {
                taskMannage.tasksArray[i].executeFlag = false;
            }
            
        }
    }

}

void taskStop(TaskIDType taskID)
{
    taskMannage.tasksArray[taskID].period = 0;
}

void tasksSetup(void)
{
    taskMannage.time = 0;
    taskMannage.numOfTasks = 0;
}