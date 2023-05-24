/*----------------------------------- Include ----------------------------------------------*/
#include "servo.h"
#include "pca9685.h"
#include "task_management.h"
/*----------------------------------- Private Definitions ----------------------------------*/

/*----------------------------------- Private Functions ------------------------------------*/
static void _servoTask(void);
static void _servoInit(void);
static void _servoAllDrop(void);
static void _servoStartPWM(ServoChannel channel);
static ServoMode _servoOneDrop(ServoChannel *channel);

/*----------------------------------- Private Variables ------------------------------------*/
static TaskIDType servoTaskID;
static ServoType ServoArray[NUMBER_OF_SERVO];
static ServoChannel __servoChannel;
static ServoMode __servoMode;

/*----------------------------------- Public Variables -------------------------------------*/


/************* Private Functions Declaration *****************/
static void _servoStartPWM(ServoChannel channel){
    PCA9685_SetServoAngle(channel, 180);
    delay(300);
    PCA9685_SetServoAngle(channel, 0);
}
static void _servoAllDrop(void){
    for (uint8_t i = 0; i < NUMBER_OF_SERVO; i++){
        PCA9685_SetServoAngle(ServoArray[i].channel, 180);
    }
    delay(300);
    for (uint8_t i = 0; i < NUMBER_OF_SERVO; i++){
        PCA9685_SetServoAngle(ServoArray[i].channel, 0);
        ServoArray[i].activated = false;
        __servoChannel = 0;
    }
}

static ServoMode _servoOneDrop(ServoChannel *channel){
    while (ServoArray[*channel].activated){
        (*channel)++;
        if (*channel >= NUMBER_OF_SERVO){
            *channel = 0;
            for (int i = 0 ; i < NUMBER_OF_SERVO; i++){
                ServoArray[i].activated = false;
            }
            return SLEEP;
        }
    }
    printf("Channel: %d DROP\n", *channel);
    _servoStartPWM(*channel);
    ServoArray[*channel].activated = true;
    (*channel)++;
    if (*channel >= NUMBER_OF_SERVO){
        *channel = 0;
        for (int i = 0 ; i < NUMBER_OF_SERVO; i++){
                ServoArray[i].activated = false;
        }
        return SLEEP;
    }
    return DROP_ONE;
}

static void _servoInit(void){
    for (uint8_t i = 0; i < NUMBER_OF_SERVO; i++){
        ServoArray[i].channel = i;
        ServoArray[i].activated = false;
        /* calib servo */
        PCA9685_SetServoAngle(i, 0); 
    }
}

static void _servoTask(void){
    switch (__servoMode)
    {
        case SLEEP:
            //temp_counter++;
            // printf("Im Sleeping: %d\n", temp_counter);
            // delay(50);
            break;
        case DROP_ONE:
            //printf("Drop One\n");
            if (_servoOneDrop(&__servoChannel) == SLEEP){
                printf("SLEEP\n");
            }
            __servoMode = SLEEP;
            break;
        case DROP_ALL:
            printf("Drop All\n");
            _servoAllDrop();
            __servoMode = SLEEP;
            break;
        default:
            break;
    }
}
/************* Public Functions Declaration ******************/
void servoChangeMode(ServoMode mode){
    __servoMode = mode;
}

void servoSetup(void){
    PCA9685_Init();
    _servoInit();
    taskCreate(&servoTaskID, TASK_MODE_REPEATED, _servoTask);
    taskStart(servoTaskID, SERVO_TASK_PERIOD);
}