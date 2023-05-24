#ifndef __SERVO_H
#define __SERVO_H

#include <stdint.h>
#include <stdbool.h>

#define SERVO_TASK_PERIOD       5 //ms
#define NUMBER_OF_SERVO         9

typedef enum {
    SERVO1,
    SERVO2,
    SERVO3,
    SERVO4,
    SERVO5,
    SERVO6,
    SERVO7,
    SERVO8,
    SERVO9,
    SERVO10,
    SERVO11,
    SERVO12,
    SERVO13,
    SERVO14,
    SERVO15,
    SERVO16
} ServoChannel;

typedef enum {
    SLEEP,
    DROP_ONE,
    DROP_ALL
} ServoMode;

typedef struct {
    ServoChannel channel;
    bool activated;
} ServoType;

extern void servoSetup(void);
extern void servoChangeMode(ServoMode mode);

#endif /* __SERVO_H */