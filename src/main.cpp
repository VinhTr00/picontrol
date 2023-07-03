#include "main.h"
#include <wiringPi.h>

int main(int argc, char **argv)
{
    wiringPiSetup();
    tasksSetup();
    buttonSetUp();
    servoSetup();
    speakerSetup();
    sbusSetup();

    while (1){
        tasksRun();
    }

    return 0;
}