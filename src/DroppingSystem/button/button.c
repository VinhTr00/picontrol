/*----------------------------------- Include ----------------------------------------------*/
#include "button.h"
#include "task_management.h"
#include "wiringPi.h"
#include "servo.h"

/*----------------------------------- Private Definitions ----------------------------------*/
#define BUTTON_PIN          29
/*----------------------------------- Private Functions ------------------------------------*/
static void _buttonTask(void);
static void _buttonInit(BUTTON_Name* Button, int Button_pin);
static uint8_t BUTTON_ReadPin(BUTTON_Name* Button);
static BUTTON_STATE BUTTON_Read(BUTTON_Name* Button);
static void BUTTON_DelayMs(uint16_t Time);

/*----------------------------------- Private Variables ------------------------------------*/
static TaskIDType buttonTaskID;
static BUTTON_Name Button1;
static BUTTON_STATE State;
/*----------------------------------- Public Variables -------------------------------------*/

/*********** Private Functions Declaration ****************/
uint8_t BUTTON_ReadPin(BUTTON_Name* Button){
    return digitalRead(Button->Pin);
}

static void BUTTON_DelayMs(uint16_t Time)
{
	delay(Time);
}

void _buttonInit(BUTTON_Name* Button, int Button_pin){
	Button->Pin = Button_pin;
    pinMode(Button->Pin, INPUT);
}

BUTTON_STATE BUTTON_Read(BUTTON_Name* Button)
{
	Button->State = NO_CLICK;
	while(BUTTON_ReadPin(Button) == 0)
	{
		Button->timePress++;
		Button->isPress = 1;
		BUTTON_DelayMs(1);
	}
	if(Button->isPress)
	{
		while(BUTTON_ReadPin(Button) == 1)
		{
			Button->timeDouble++;
			BUTTON_DelayMs(1);
			if(Button->timeDouble > DOUBLE_CLICK_TIME)
			{
				if(Button->timePress > DEBOUND_TIME && Button->timePress <= SINGLE_CLICK_TIME)
				{
					Button->isPress = 0;
					Button->timePress = 0;
					Button->timeDouble = 0;
					Button->State = SINGLE_CLICK;
					return Button->State;
				}
				else if(Button->timePress  > SINGLE_CLICK_TIME)
				{
					Button->isPress = 0;
					Button->timePress = 0;
					Button->timeDouble = 0;
					Button->State = LONGCLICK_1S;
					return Button->State;
				}
			}
		}
		while(BUTTON_ReadPin(Button) == 0)
		{
			Button->State = DOUBLE_CLICK;
			Button->isPress = 0;
			Button->timePress = 0;
			Button->timeDouble = 0;
			return Button->State;
		}
	}
	return NO_CLICK;
}
void _buttonTask(void){
	State = BUTTON_Read(&Button1);
    switch (State)
    {
        case SINGLE_CLICK:
            servoChangeMode(DROP_ONE);
            break;
        case LONGCLICK_1S:
            servoChangeMode(DROP_ALL);
            break;
        default:
            break;
    }
}

/************* Public Functions Declaration ******************/
void buttonSetUp(void){
    _buttonInit(&Button1, BUTTON_PIN);
    taskCreate(&buttonTaskID, TASK_MODE_REPEATED, _buttonTask);
    taskStart(buttonTaskID, BUTTON_TASK_PERIOD);
}
