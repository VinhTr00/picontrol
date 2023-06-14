#ifndef __SBUS_APPLICATION_H
#define __SBUS_APPLICATION_H

#include "SBUS.h"

#define SBUS_TASK_PERIOD       3 //ms

#define SBUS_SERVO_MIN              800
#define SBUS_SERVO_THRESHOLD_1      1300
#define SBUS_SERVO_THRESHOLD_2      1700
#define SBUS_SERVO_MAX              2100

extern void sbusSetup(void);

#endif /* __SBUS_APPLICATION_H */