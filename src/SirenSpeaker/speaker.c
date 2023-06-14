/*----------------------------------- Include ----------------------------------------------*/
#include "speaker.h"
#include "task_management.h"
#include <wiringPi.h>
#include <stdio.h>

/*----------------------------------- Private Definitions ----------------------------------*/
#define GPIO_PIN_RESET  0
#define GPIO_PIN_SET    1

/*----------------------------------- Private Functions ------------------------------------*/
static void _speakerTask(void);
static void _speakerInitPin(void);

/*----------------------------------- Private Variables ------------------------------------*/
static TaskIDType speakerTaskID;
static StateSpeaker modeSpeaker;
/*----------------------------------- Public Variables -------------------------------------*/

/*********** Private Functions Declaration ****************/
static void _speakerInitPin(void){
    pinMode(TONE1_PIN, OUTPUT);
    pinMode(TONE2_PIN, OUTPUT);
    pinMode(MIC_PIN, OUTPUT);
    digitalWrite(TONE1_PIN, GPIO_PIN_RESET);
    digitalWrite(TONE2_PIN, GPIO_PIN_RESET);
    digitalWrite(MIC_PIN, GPIO_PIN_SET);
}

static void _speakerTask(void){
    switch (modeSpeaker)
    {
        case IDLE:
            /* do nothing */
            break;
        case ERROR_STATE:
            printf("ERROR\n");
            digitalWrite(TONE1_PIN, GPIO_PIN_RESET);
            digitalWrite(TONE2_PIN, GPIO_PIN_RESET);
            digitalWrite(MIC_PIN, GPIO_PIN_SET);
            modeSpeaker = IDLE;
            break;
        case TONE1:
            printf("TONE1\n");
            digitalWrite(MIC_PIN, GPIO_PIN_RESET);
            digitalWrite(TONE2_PIN, GPIO_PIN_RESET);
            digitalWrite(TONE1_PIN, GPIO_PIN_SET);
            modeSpeaker = IDLE;
            break;
        case TONE2:
            printf("TONE2\n");
            digitalWrite(TONE1_PIN, GPIO_PIN_RESET);
            digitalWrite(MIC_PIN, GPIO_PIN_RESET);
            digitalWrite(TONE2_PIN, GPIO_PIN_SET);
            modeSpeaker = IDLE;
            break;
        case MIC:
            printf("MIC\n");
            digitalWrite(TONE1_PIN, GPIO_PIN_RESET);
            digitalWrite(TONE2_PIN, GPIO_PIN_RESET);
            digitalWrite(MIC_PIN, GPIO_PIN_SET);
            modeSpeaker = IDLE;
            break;
        default:
            break;
    }
}

/************* Public Functions Declaration ******************/
void speakerSetup(void){
    _speakerInitPin();
    taskCreate(&speakerTaskID, TASK_MODE_REPEATED, _speakerTask);
    taskStart(speakerTaskID, SPEAKER_TASK_PERIOD);
}

void speakerChangeMode(StateSpeaker mode){
    modeSpeaker = mode;
}

