#ifndef __COMMUNICATE_H__
#define __COMMUNICATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define COMM_TASK_PERIOD        10 //ms
#define IP                      "192.168.0.29"
#define PORT                    "5762"  

typedef enum {
    SERVO_OUT_1,
    SERVO_OUT_2,
    SERVO_OUT_3,
    SERVO_OUT_4,
    SERVO_OUT_5,
    SERVO_OUT_6,
    SERVO_OUT_7,
    SERVO_OUT_8,
    SERVO_OUT_9,
    SERVO_OUT_10,
    SERVO_OUT_11,
    SERVO_OUT_12,
    SERVO_OUT_13,
    SERVO_OUT_14,
    SERVO_OUT_15,
    SERVO_OUT_16,
    SERVO_OUT_TOTAL,
} SERVO_OUT;

#define CHANNEL_LIGHT_BRIGHTNESS        SERVO_OUT_11
#define CHANNEL_LIGHT_SERVO             SERVO_OUT_13
#define CHANNEL_SPEAKER_MODE            SERVO_OUT_12

#define PCA_CHANNEL_LIGHT_BRIGHTNESS    PCA_PWM_3
#define PCA_CHANNEL_LIGHT_SERVO         PCA_PWM_2


extern void commSetUp(void);


#ifdef __cplusplus
}
#endif

#endif /* __COMMUNICATE_H__ */
