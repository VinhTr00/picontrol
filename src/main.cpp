// #include "main.h"
// #include <wiringPi.h>

// int main(int argc, char **argv)
// {
//     wiringPiSetup();
//     tasksSetup();
//     buttonSetUp();
//     servoSetup();
//     speakerSetup();
//     sbusSetup();

//     while (1){
//         tasksRun();
//     }

//     return 0;
// }

#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include "main.h"


#define IP_ADDRESS          "192.168.0.99"
#define PORT                "5763"    
#define SIZE                255
#define SYSTEM_ID           1
#define COMPONENT_ID        50

static char * ip_addr = IP_ADDRESS;
static char * ip_port = PORT;

mavlink_message_t tx_msg, rx_msg;
mavlink_status_t status;
mavlink_data16_t data16 = {0}; 

mavlink_request_data_stream_t request = {
    .req_message_rate = 1,
    .target_system = 1,
    .target_component = 1,
    .req_stream_id = 0,
    .start_stop = 1
};

mavlink_servo_output_raw_t servo = {0};

char tx_buffer[SIZE] = {0};
char rx_buffer[SIZE] = {0};



int byte = 0;
uint8_t counter = 0;

int main(int argc, char *argv[])
{
    NetworkManager network_client = {
        .protocol = TCP,
        .model = CLIENT,
    };
    
    if(argc > 2){
        ip_addr = argv[1];
        ip_port = argv[2];
    }
    
    
    network_init(&network_client, ip_addr, ip_port);
    mavlink_msg_request_data_stream_encode(SYSTEM_ID, COMPONENT_ID, &tx_msg, &request);
    uint8_t len = mavlink_msg_to_send_buffer((uint8_t *)tx_buffer, &tx_msg);
    
    network_connect(&network_client);

    while (1)
    {
        if (byte = network_read(&network_client, rx_buffer, SIZE, 5)){
            for (int i = 0; i < byte; i++){
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
        }
        else if (byte <= 0) {
            network_connect(&network_client);
        }
       
        // network_write(&network_client, tx_buffer, len, 5);
        // delay(1000);
    }
    
    return 0;
}