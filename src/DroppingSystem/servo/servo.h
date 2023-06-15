#ifndef __SERVO_H
#define __SERVO_H

#ifdef __cplusplus
#include <cstdint>
#include <cstdbool>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "pca9685.h"
#define SERVO_TASK_PERIOD       10 //ms
#define NUMBER_OF_SERVO         9

typedef enum {
    SLEEP,
    DROP_ONE,
    DROP_ALL
} ServoMode;

typedef struct {
    uint8_t channel;
    bool activated;
} ServoType;

#ifdef __cplusplus
extern "C" {
#endif

#define servoSetAngle(channel, angle)   PCA9685_SetServoAngle(channel, angle)
extern void servoSetup(void);
extern void servoChangeMode(ServoMode mode);

#ifdef __cplusplus
}
#endif

#endif /* __SERVO_H */