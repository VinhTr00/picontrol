/*----------------------------------- Include ----------------------------------------------*/
#include "communicate.h"
#include "sbus_application.h"
#include "pca9685.h"
#include "servo.h"
#include "task_management.h"
#include "network.h"
#include "ardupilotmega/mavlink.h"
#include "common/mavlink.h"
#include <wiringPi.h>

/*----------------------------------- Private Definitions ----------------------------------*/
#define SIZE                255
#define SYSTEM_ID           1
#define COMPONENT_ID        50

/*----------------------------------- Private Functions ------------------------------------*/
static void _commTask(void);
static void _commInitNetwork(void);
static void _commControlServo(uint8_t pca_channel, uint16_t servo_channel);

/*----------------------------------- Private Variables ------------------------------------*/
mavlink_message_t rx_msg;
mavlink_status_t status;
mavlink_servo_output_raw_t servo = {0};

char tx_buffer[SIZE] = {0};
char rx_buffer[SIZE] = {0};

static TaskIDType commTaskID;

static NetworkManager network_client = {
    .protocol = TCP,
    .model = CLIENT,
};

/*********** Private Functions Declaration ****************/
static void _commControlServo(uint8_t pca_channel, uint16_t servo_channel)
{
    float angle = (servo_channel - SBUS_SERVO_PULSE_MIN) * MAX_ANGLE / (SBUS_SERVO_PULSE_MAX - SBUS_SERVO_PULSE_MIN);
    if (angle < MIN_ANGLE) angle = MIN_ANGLE;
	else if (angle > MAX_ANGLE) angle = MAX_ANGLE;
    printf("Angle: %.2f\n", angle);
    servoSetAngle(pca_channel, angle);
}

void _commInitNetwork(void)
{
    network_init(&network_client, (char *) IP, (char *) PORT);
    network_connect(&network_client);
}

void _commTask(void){
    int bytes = 0;
    static uint16_t last_value = 0;
    if (bytes = network_read(&network_client, rx_buffer, SIZE, 5))
    {
        for (int i = 0; i < bytes; i++)
        {
            if (mavlink_parse_char(MAVLINK_COMM_0, rx_buffer[i], &rx_msg, &status))
            {
                // printf("Received message with ID %d, sequence: %d from component %d of system %d\n", rx_msg.msgid, rx_msg.seq, rx_msg.compid, rx_msg.sysid);
                switch (rx_msg.msgid){
                    case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
                        mavlink_msg_servo_output_raw_decode(&rx_msg, &servo);
                        printf("servo 1: %d \t servo 2: %d \t servo 3: %d \t servo 4: %d \t servo 5: %d \t servo 6: %d \t servo 7: %d \t servo 8: %d \t servo 9: %d \t servo 10: %d \t servo 11: %d \t servo 12: %d \t servo 13: %d \t servo 14: %d \t servo 15: %d \t servo 16: %d \t\n", 
                            servo.servo1_raw, servo.servo2_raw, servo.servo3_raw, servo.servo4_raw, servo.servo5_raw, servo.servo6_raw, servo.servo7_raw, servo.servo8_raw,
                            servo.servo9_raw, servo.servo10_raw, servo.servo11_raw, servo.servo12_raw, servo.servo13_raw, servo.servo14_raw, servo.servo15_raw, servo.servo16_raw);
                        break;
                    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
                        printf("MAVLINK_MSG_ID_GLOBAL_POSITION_INT\n");
                        break;
                }
            }
        }
        if (servo.servo5_raw > last_value){
            _commControlServo(0, servo.servo5_raw);
            last_value = servo.servo5_raw;
        }
    }
    else if (bytes <= 0) {
        _commInitNetwork();
    }
}

/************* Public Functions Declaration ******************/
void commSetUp(void)
{
    _commInitNetwork();
    taskCreate(&commTaskID, TASK_MODE_REPEATED, _commTask);
    taskStart(commTaskID, COMM_TASK_PERIOD);
}
