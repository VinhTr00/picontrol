#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif


#define BUTTON_TASK_PERIOD      10 //ms

#define DEBOUND_TIME 50
#define SINGLE_CLICK_TIME 1000
#define DOUBLE_CLICK_TIME 300

typedef enum
{
    NO_CLICK,
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONGCLICK_1S 
} BUTTON_STATE;

typedef struct {
	int Pin;
	BUTTON_STATE State;
	uint8_t  isPress;
	uint16_t timePress;
	uint16_t timeDouble;
} BUTTON_Name;

#ifdef __cplusplus
extern "C" {
#endif
extern void buttonSetUp(void);
#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H */