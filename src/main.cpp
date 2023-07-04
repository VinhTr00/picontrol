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
#include "network.h"

#define IP_ADDRESS      "192.168.0.99"
#define PORT            "20002"    
#define SIZE            255

static char * ip_addr = IP_ADDRESS;
static char * ip_port = PORT;

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
    char buffer[SIZE];

    network_init(&network_client, ip_addr, ip_port);

    network_connect(&network_client);

    while (1){
        network_read(&network_client, buffer, SIZE, 5);
    }
    
    return 0;
}