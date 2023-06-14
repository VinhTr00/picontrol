/*----------------------------------- Include ----------------------------------------------*/
#include <iostream>
#include <chrono>

#include "sbus_application.h"
#include "task_management.h"
#include "speaker.h"

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;
using std::chrono::steady_clock;
using std::chrono::milliseconds;

/*----------------------------------- Private Definitions ----------------------------------*/
#define MIN_THRESHOLD_CHANGE    50
#define CHANNEL_SERVO_OUT(CHANNEL)    (CHANNEL - 1)

/*----------------------------------- Private Functions ------------------------------------*/
static void _sbusOnPacket(const sbus_packet_t &packet);
static void _sbusTask(void);
static void _sbusSerialInit(void);
static void _sbusComparePulse(const uint16_t channel);

/*----------------------------------- Private Variables ------------------------------------*/
static SBUS sbus;
static TaskIDType sbusTaskID;

/*----------------------------------- Public Variables -------------------------------------*/


/************* Private Functions Declaration *****************/
static void _sbusComparePulse(const uint16_t channel)
{
    static uint16_t temp_channel;
    if (abs(channel - temp_channel) > MIN_THRESHOLD_CHANGE)
    {
        temp_channel = channel;
        if (temp_channel > SBUS_SERVO_MIN && temp_channel < SBUS_SERVO_THRESHOLD_1)
        {
            speakerChangeMode(TONE1);
        }
        else if (temp_channel > SBUS_SERVO_THRESHOLD_1 && temp_channel < SBUS_SERVO_THRESHOLD_2)
        {
            speakerChangeMode(MIC);
        }
        else if (temp_channel > SBUS_SERVO_THRESHOLD_2 && temp_channel < SBUS_SERVO_MAX)
        {
            speakerChangeMode(TONE2);
        }
        else
        {
            speakerChangeMode(ERROR_STATE);
        }
    }
}
static void _sbusOnPacket(const sbus_packet_t &packet)
{
    static auto lastPrint = steady_clock::now();
    auto now = steady_clock::now();

    if (now - lastPrint > milliseconds(500))
    {
        for (int i = 0; i < 16; ++i)
        {
            cout << "ch" << i + 1 << ": " << packet.channels[i] << "\t";
        }
        cout << endl;
        
        lastPrint = now;
    }
    _sbusComparePulse(packet.channels[CHANNEL_SERVO_OUT(12)]);
}

static void _sbusSerialInit(void){
    sbus.onPacket(_sbusOnPacket);
    sbus_err_t err = sbus.install("/dev/ttyAMA0", false);  // false for non-blocking
    if (err != SBUS_OK)
    {
        cerr << "SBUS install error: " << err << endl;
        return;
    }
    cout << "SBUS installed" << endl;
}
static void _sbusTask(void){
    sbus_err_t err;
    bool flag = true;
    static auto count = 0;;
    if ((err = sbus.read()) != SBUS_FAIL)
    {
        // desync means a packet was misaligned and not received properly
        if (err == SBUS_ERR_DESYNC)
        {
            cerr << "SBUS desync" << endl;
        }
    }
}

/************* Public Functions Declaration ******************/
void sbusSetup(void){
    _sbusSerialInit();
    taskCreate(&sbusTaskID, TASK_MODE_REPEATED, _sbusTask);
    taskStart(sbusTaskID, SBUS_TASK_PERIOD);
}