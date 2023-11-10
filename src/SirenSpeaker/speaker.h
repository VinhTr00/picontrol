#ifndef __SPEAKER_H
#define __SPEAKER_H

#define SPEAKER_TASK_PERIOD         10 //ms

#define TONE1_PIN                   5//24 5
#define TONE2_PIN                   4//23 4
#define MIC_PIN                     6//25 6

typedef enum {
	IDLE,
	ERROR_STATE,
	TONE1,
	TONE2,
	MIC,
} StateSpeaker;

#ifdef __cplusplus
extern "C" {
#endif
extern void speakerSetup(void);
extern void speakerChangeMode(StateSpeaker mode);
#ifdef __cplusplus
}
#endif

#endif /* __SPEAKER_H */