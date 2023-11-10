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

/*----------------------------------- Private Definitions ----------------------------------*/
#define SIZE                255
#define SYSTEM_ID           1
#define COMPONENT_ID        50

/*----------------------------------- Private Functions ------------------------------------*/
static void _commTask(void);
static void _commInitNetwork(void);
static void _commControlServo(uint8_t pca_channel, uint16_t servo_channel);
static void _commControlSpeaker(uint16_t servo_channel);

/*----------------------------------- Private Variables ------------------------------------*/
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
static void _commControlSpeaker(uint16_t servo_channel)
{
    if (servo_channel > SBUS_SERVO_THRESHOLD_MIN && servo_channel < SBUS_SERVO_THRESHOLD_1)
    {
        speakerChangeMode(TONE1);
    }
    else if (servo_channel > SBUS_SERVO_THRESHOLD_1 && servo_channel < SBUS_SERVO_THRESHOLD_2)
    {
        speakerChangeMode(MIC);
    }
    else if (servo_channel > SBUS_SERVO_THRESHOLD_1 && servo_channel < SBUS_SERVO_THRESHOLD_MAX)
    {
        speakerChangeMode(TONE2);
    }
    else 
    {
        speakerChangeMode(ERROR_STATE);
    }
}

static void _commControlServo(uint8_t pca_channel, uint16_t servo_channel)
{
    // float angle = (servo_channel - SBUS_SERVO_PULSE_MIN) * MAX_ANGLE / (SBUS_SERVO_PULSE_MAX - SBUS_SERVO_PULSE_MIN);
    // if (angle < MIN_ANGLE) angle = MIN_ANGLE;
	// else if (angle > MAX_ANGLE) angle = MAX_ANGLE;
    // printf("Angle: %.2f\n", angle);
    // servoSetAngle(pca_channel, angle);

    float angle = (servo_channel/(1000000.0/PWM_FREQUENCY)) * 4095;
    printf("%f\n", angle);
    
    PCA9685_SetPin(pca_channel, (uint16_t)angle, 0);
}

void _commInitNetwork(void)
{
    network_init(&network_client, (char *) IP, (char *) PORT);
    network_connect(&network_client);
}

void _commTask(void){
    int bytes = 0;
    static uint16_t last_value_speaker = SBUS_SERVO_PULSE_NEUTRAL;
    static uint16_t last_value_light = SBUS_SERVO_PULSE_MIN;
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
                        printf("servo 1: %d \t servo 2: %d \t servo 3: %d \t servo 4: %d \t servo 5: %d \t servo 6: %d \t servo 7: %d \t servo 8: %d \t servo 9: %d \t servo 10: %d \t servo 11: %d \t servo 12: %d \t servo 13: %d \t servo 14: %d \t servo 15: %d \t servo 16: %d \t\n", 
                            servo.servo1_raw, servo.servo2_raw, servo.servo3_raw, servo.servo4_raw, servo.servo5_raw, servo.servo6_raw, servo.servo7_raw, servo.servo8_raw,
                            servo.servo9_raw, servo.servo10_raw, servo.servo11_raw, servo.servo12_raw, servo.servo13_raw, servo.servo14_raw, servo.servo15_raw, servo.servo16_raw);
                        break;
                    default:
                        break;
                }
            }
        }
        if (servo.servo12_raw != last_value_speaker)
        {
            last_value_speaker = servo.servo12_raw;
            _commControlSpeaker(last_value_speaker);
        }
        if (servo.servo5_raw != last_value_light){
            if (servo.servo5_raw > last_value_light){
                for (int i = last_value_light; i <= servo.servo5_raw; i += 5){
                    _commControlServo(2, i);
                    delay(1);
                }
            }
            else {
                for (int i = last_value_light; i >= servo.servo5_raw; i -= 5){
                    _commControlServo(2, i);
                    delay(1);
                }
            }
            // _commControlServo(0, servo.servo5_raw);
            // _commControlServo(2, servo.servo5_raw);
            last_value_light = servo.servo5_raw;
        }
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
}
