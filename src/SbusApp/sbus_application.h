#ifndef __SBUS_APPLICATION_H
#define __SBUS_APPLICATION_H

#include "SBUS.h"

#define SBUS_TASK_PERIOD                5 //ms

#define SBUS_SERVO_THRESHOLD_MIN        800     //us
#define SBUS_SERVO_THRESHOLD_1          1300    //us
#define SBUS_SERVO_THRESHOLD_2          1700    //us
#define SBUS_SERVO_THRESHOLD_MAX        2100    //us

#define SBUS_SERVO_PULSE_MIN            1100    //us
#define SBUS_SERVO_PULSE_MAX            2000    //us

#define SERIAL_READ_SBUS_PATH           "/dev/ttyAMA0"

extern void sbusSetup(void);
extern void _sbusOnPacket(const sbus_packet_t &packet);

#endif /* __SBUS_APPLICATION_H */