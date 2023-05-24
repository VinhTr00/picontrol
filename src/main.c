#include "main.h"

int main(void){
    wiringPiSetup();
    tasksSetup();
    buttonSetUp();
    servoSetup();

    while (1){
        tasksRun();
    }
}