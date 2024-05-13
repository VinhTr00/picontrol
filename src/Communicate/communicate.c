/*----------------------------------- Include ----------------------------------------------*/
#include "communicate.h"
#include "sbus_spec.h"
#include "pca9685.h"
#include "servo.h"
#include "task_management.h"
#include "network.h"
#include "speaker.h"
#include "ardupilotmega/mavlink.h"
#include "common/mavlink.h"

#include <wiringPi.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

/*----------------------------------- Private Definitions ----------------------------------*/
#define SIZE                255
#define SYSTEM_ID           1
#define COMPONENT_ID        50

/*----------------------------------- Private Typedef ------------------------------------*/
typedef enum {
    PAYLOAD_BRIGHTNESS_LIGHT,
    PAYLOAD_SERVO_LIGHT,
    PAYLOAD_SPEAKER,
    PAYLOAD_TOTAL,
} PAYLOAD_INDEX_E;

typedef struct {
    uint16_t *rc_value[PAYLOAD_TOTAL];
    SERVO_OUT channel[PAYLOAD_TOTAL];
} PayloadChannel_t;

/*----------------------------------- Private Functions ------------------------------------*/
static void _commTask(void);
static void _commInitNetwork(void);
static void _commInitPayload(PayloadChannel_t *pHandle);

static void _commControl_servo(uint8_t pca_channel);
static void startSmoothServo(uint8_t pca_channel, uint16_t rc_val);
static void _commControl_speaker(void);
static void _commControl_lightBrightness(uint8_t pca_channel);

/*----------------------------------- Private Variables ------------------------------------*/
PayloadChannel_t heraPayload = {
    .channel[PAYLOAD_BRIGHTNESS_LIGHT] = CHANNEL_LIGHT_BRIGHTNESS,
    .channel[PAYLOAD_SERVO_LIGHT] = CHANNEL_LIGHT_SERVO,
    .channel[PAYLOAD_SPEAKER] = CHANNEL_SPEAKER_MODE,
};

mavlink_message_t rx_msg;
mavlink_status_t status;
mavlink_servo_output_raw_t servo = {0};
mavlink_rc_channels_t rc_channel = {0};

char tx_buffer[SIZE] = {0};
char rx_buffer[SIZE] = {0};

static TaskIDType commTaskID;

static NetworkManager network_client = {
    .protocol = TCP,
    .model = CLIENT,
};

/*********** Private Functions Declaration ****************/
void _commInitPayload(PayloadChannel_t *pHandle)
{
    for (uint8_t i = 0; i < PAYLOAD_TOTAL; i++)
    {
        switch (pHandle->channel[i])
        {
            case SERVO_OUT_1:
                pHandle->rc_value[i] = &servo.servo1_raw;
                break;
            case SERVO_OUT_2:
                pHandle->rc_value[i] = &servo.servo2_raw;
                break;
            case SERVO_OUT_3:
                pHandle->rc_value[i] = &servo.servo3_raw;
                break;
            case SERVO_OUT_4:
                pHandle->rc_value[i] = &servo.servo4_raw;
                break;
            case SERVO_OUT_5:
                pHandle->rc_value[i] = &servo.servo5_raw;
                break;
            case SERVO_OUT_6:
                pHandle->rc_value[i] = &servo.servo6_raw;
                break;
            case SERVO_OUT_7:
                pHandle->rc_value[i] = &servo.servo7_raw;
                break;
            case SERVO_OUT_8:
                pHandle->rc_value[i] = &servo.servo8_raw;
                break;
            case SERVO_OUT_9:
                pHandle->rc_value[i] = &servo.servo9_raw;
                break;
            case SERVO_OUT_10:
                pHandle->rc_value[i] = &servo.servo10_raw;
                break;
            case SERVO_OUT_11:
                pHandle->rc_value[i] = &servo.servo11_raw;
                break;
            case SERVO_OUT_12:
                pHandle->rc_value[i] = &servo.servo12_raw;
                break;
            case SERVO_OUT_13:
                pHandle->rc_value[i] = &servo.servo13_raw;
                break;
            case SERVO_OUT_14:
                pHandle->rc_value[i] = &servo.servo14_raw;
                break;
            case SERVO_OUT_15:
                pHandle->rc_value[i] = &servo.servo15_raw;
                break;
            case SERVO_OUT_16:
                pHandle->rc_value[i] = &servo.servo16_raw;
                break;
            default:
                break;
        }
    }
    
}

static void _commControl_speaker(void)
{
    static uint16_t last_rc_val;
    uint16_t rc_val = *heraPayload.rc_value[PAYLOAD_SPEAKER];
    if (rc_val != last_rc_val)
    {
        if (rc_val > SBUS_SERVO_THRESHOLD_MIN && rc_val < SBUS_SERVO_THRESHOLD_1)
        {
            speakerChangeMode(TONE1);
        }
        else if (rc_val > SBUS_SERVO_THRESHOLD_1 && rc_val < SBUS_SERVO_THRESHOLD_2)
        {
            speakerChangeMode(MIC);
        }
        else if (rc_val > SBUS_SERVO_THRESHOLD_1 && rc_val < SBUS_SERVO_THRESHOLD_MAX)
        {
            speakerChangeMode(TONE2);
        }
        else 
        {
            speakerChangeMode(ERROR_STATE);
        }
        last_rc_val = rc_val;
    }
}
void startSmoothServo(uint8_t pca_channel, uint16_t rc_val)
{
    // float angle = (servo_channel - SBUS_SERVO_PULSE_MIN) * MAX_ANGLE / (SBUS_SERVO_PULSE_MAX - SBUS_SERVO_PULSE_MIN);
    // if (angle < MIN_ANGLE) angle = MIN_ANGLE;
	// else if (angle > MAX_ANGLE) angle = MAX_ANGLE;
    // printf("Angle: %.2f\n", angle);
    // servoSetAngle(pca_channel, angle);
    uint16_t step = PCA9685_CalcOntimeValue(rc_val);
    printf("Step Servo: %d\n", step);
    PCA9685_SetPin(pca_channel, step, 0);
}

static void _commControl_servo(uint8_t pca_channel)
{
    static uint16_t last_rc_val;
    uint16_t rc_val = *heraPayload.rc_value[PAYLOAD_SERVO_LIGHT];
    if (rc_val != last_rc_val)
    {
        if (rc_val > last_rc_val)
        {
            for (int i = last_rc_val; i <= rc_val; i += 5)
            {
                startSmoothServo(pca_channel, i);
                delayMicroseconds(2000);
            }
        }
        else 
        {
            for (int i = last_rc_val; i >= rc_val; i -= 5)
            {
                startSmoothServo(pca_channel, i);
                delayMicroseconds(2000);
            }
        }
        startSmoothServo(pca_channel, rc_val);
        last_rc_val = rc_val;
    }       
}

void _commControl_lightBrightness(uint8_t pca_channel)
{
    static uint16_t last_rc_val;
    uint16_t rc_val = *heraPayload.rc_value[PAYLOAD_BRIGHTNESS_LIGHT];
    if (rc_val != last_rc_val)
    {
        uint16_t step = PCA9685_CalcOntimeValue(rc_val);
        printf("Step Brightness: %d\n", step);
        PCA9685_SetPin(pca_channel, step, 0);
        last_rc_val = rc_val;
    }
}


void _commInitNetwork(void)
{
    network_init(&network_client, (char *) IP, (char *) PORT);
    network_connect(&network_client);
}

void _commTask(void){
    int bytes = 0;
    // static uint16_t last_value_speaker = SBUS_SERVO_PULSE_NEUTRAL;
    // static uint16_t last_value_light = SBUS_SERVO_PULSE_MIN;
    static bool network_connected = false;
    if (network_connected == false)
    {
        if (network_init(&network_client, (char *) IP, (char *) PORT) < 0)
        {
            return;
        }
        network_connected = true;
        network_connect(&network_client);
    }
    bytes = network_read(&network_client, rx_buffer, SIZE, 5);
    if (bytes > 0)
    {
        for (int i = 0; i < bytes; i++)
        {
            if (mavlink_parse_char(MAVLINK_COMM_0, rx_buffer[i], &rx_msg, &status))
            {
                // printf("Received message with ID %d, sequence: %d from component %d of system %d\n", rx_msg.msgid, rx_msg.seq, rx_msg.compid, rx_msg.sysid);
                switch (rx_msg.msgid){
                    // case MAVLINK_MSG_ID_RC_CHANNELS:
                    //     mavlink_msg_rc_channels_decode(&rx_msg, &rc_channel);
                    //     break;
                    case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
                        mavlink_msg_servo_output_raw_decode(&rx_msg, &servo);
                        printf("servo 11 (Light): %d \t servo 12 (Speaker): %d \t servo 13 (Servo Light): %d \t\n",servo.servo11_raw, servo.servo12_raw, servo.servo13_raw);
                        break;
                    default:
                        break;
                }
            }
        }
        // if (servo.servo12_raw != last_value_speaker)
        // {
        //     last_value_speaker = servo.servo12_raw;
        //     _commControl_speaker(last_value_speaker);
        // }
        _commControl_lightBrightness(PCA_CHANNEL_LIGHT_BRIGHTNESS);
        _commControl_speaker();
        _commControl_servo(PCA_CHANNEL_LIGHT_SERVO);
    }
    else 
    {
        network_deinit(&network_client);
        network_connected = false;
    }
}

/************* Public Functions Declaration ******************/
void commSetUp(void)
{
    taskCreate(&commTaskID, TASK_MODE_REPEATED, _commTask);
    taskStart(commTaskID, COMM_TASK_PERIOD);
    _commInitPayload(&heraPayload);
}
