#ifndef __SBUS_APPLICATION_H
#define __SBUS_APPLICATION_H

#include "SBUS.h"

#define SBUS_TASK_PERIOD                5 //ms

#define SERIAL_READ_SBUS_PATH           "/dev/ttyAMA0"

extern void sbusSetup(void);
extern void _sbusOnPacket(const sbus_packet_t &packet);

#endif /* __SBUS_APPLICATION_H */