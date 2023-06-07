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
    servoSetAngle(channel, 180);
    delay(300);
    servoSetAngle(channel, 0);
}
static void _servoAllDrop(void){
    for (uint8_t i = 0; i < NUMBER_OF_SERVO; i++){
        servoSetAngle(ServoArray[i].channel, 180);
    }
    delay(300);
    for (uint8_t i = 0; i < NUMBER_OF_SERVO; i++){
        servoSetAngle(ServoArray[i].channel, 0);
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
        }
    }
    printf("Channel %d: DROP\n", *channel);
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
    PCA9685_Init();
    for (uint8_t i = 0; i < NUMBER_OF_SERVO; i++){
        ServoArray[i].channel = i;
        ServoArray[i].activated = false;
        /* calib servo */
        servoSetAngle(i, 0); 
        
    }
    ServoArray[0].activated = true;
    ServoArray[7].activated = true;
    ServoArray[8].activated = true;
}

static void _servoTask(void){
    switch (__servoMode)
    {
        case SLEEP:
            /* do nothing */
            break;
        case DROP_ONE:
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
void servoSetAngle(uint8_t channel, float angle){
    PCA9685_SetServoAngle(channel, angle);
}

void servoChangeMode(ServoMode mode){
    __servoMode = mode;
}

void servoSetup(void){
    _servoInit();
    taskCreate(&servoTaskID, TASK_MODE_REPEATED, _servoTask);
    taskStart(servoTaskID, SERVO_TASK_PERIOD);
}