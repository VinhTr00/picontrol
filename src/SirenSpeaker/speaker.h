#ifndef __SPEAKER_H
#define __SPEAKER_H

#include <wiringPi.h>
#include "task_management.h"

#define SPEAKER_TASK_PERIOD         3 //ms

#define TONE1_PIN                   27
#define TONE2_PIN                   28
#define MIC_PIN                     29

typedef enum {
	ERROR_STATE= -1,
	TONE1,
	TONE2,
	MIC,
} StateSpeaker;

extern void speakerSetup(void);
extern void speakerChangeMode(StateSpeaker mode);

#endif /* __SPEAKER_H */