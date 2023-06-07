#include "main.h"

int main(void){

    wiringPiSetup();
    tasksSetup();
    buttonSetUp();
    servoSetup();
    sbusSetup();

    while (1){
        tasksRun();
    }
}