/*----------------------------------- Include ----------------------------------------------*/
#include <iostream>
#include <chrono>

#include "sbus_application.h"
#include "task_management.h"

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;
using std::chrono::steady_clock;
using std::chrono::milliseconds;

/*----------------------------------- Private Definitions ----------------------------------*/

/*----------------------------------- Private Functions ------------------------------------*/
static void _sbusOnPacket(const sbus_packet_t &packet);
static void _sbusTask(void);
static void _sbusSerialInit(void);

/*----------------------------------- Private Variables ------------------------------------*/
static SBUS sbus;
static TaskIDType sbusTaskID;
static int count;

/*----------------------------------- Public Variables -------------------------------------*/


/************* Private Functions Declaration *****************/
static void _sbusOnPacket(const sbus_packet_t &packet)
{
    static auto lastPrint = steady_clock::now();
    auto now = steady_clock::now();

    if (now - lastPrint > milliseconds(500))
    {
        for (int i = 0; i < 16; ++i)
            cout << "ch" << i + 1 << ": " << packet.channels[i] << "\t";

        cout << "ch17: " << (packet.ch17 ? "true" : "false") << "\t"
             << "ch18: " << (packet.ch18 ? "true" : "false");

        if (packet.frameLost)
            cout << "\tFrame lost";

        if (packet.failsafe)
            cout << "\tFailsafe active";

        cout << endl;

        lastPrint = now;
    }
}
static void _sbusSerialInit(void){
    sbus.onPacket(_sbusOnPacket);
    sbus_err_t err = sbus.install("/dev/ttyAMA0", false);  // false for non-blocking
    if (err != SBUS_OK)
    {
        cerr << "SBUS install error: " << err << endl;
        // return err;
    }

    cout << "SBUS installed" << endl;
}
static void _sbusTask(void){
    sbus_err_t err;
    bool flag = true;
    while (flag){
        static auto lastWrite = steady_clock::now();
        auto now = steady_clock::now();

        /*
        * Receiving happens independently so we can do other things.
        * Here we send a packet every second.
        */
        if (now - lastWrite > milliseconds(1000))
        {
            lastWrite = now;

            sbus_packet_t packet = {
                .ch17 = true,
                .ch18 = false,
                .failsafe = true,
                .frameLost = false,
            };

            for (int i = 0; i < 16; i++)
            {
                packet.channels[i] = count;
            }

            // make sure to limit sending frequency, SBUS is not that fast
            sbus.write(packet);
            count++;
        }
        flag = false;
    }
    if ((err = sbus.read()) != SBUS_FAIL)
    {
        //cout << "WTF" << endl;
        // desync means a packet was misaligned and not received properly
        if (err == SBUS_ERR_DESYNC)
        {
            cerr << "SBUS desync" << endl;
        }
        
    }
    // cerr << "SBUS error: " << err << endl;
}

/************* Public Functions Declaration ******************/
void sbusSetup(void){
    _sbusSerialInit();
    taskCreate(&sbusTaskID, TASK_MODE_REPEATED, _sbusTask);
    taskStart(sbusTaskID, SBUS_TASK_PERIOD);
}