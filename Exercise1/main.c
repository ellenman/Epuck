#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>
#include "sensors/proximity.h"
#include "epuck1x/uart/e_uart_char.h"
#include "stdio.h"
#include "serial_comm.h"


messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);



/*
Try if sprintf prints on the console 
*/

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();


    messagebus_init(&bus, &bus_lock, &bus_condvar);
    serial_start();
    proximity_start();
    calibrate_ir();

    /* Infinite loop. */
    while (1) {
            // Read the values of all proximity sensors
            int proxValues[8];
            for (int i = 0; i < 8; i++) {
                proxValues[i] = get_prox(i);
            }


            // Print the values of all proximity sensors
            char str[100];
            int str_length = sprintf(str, "Proximity Sensor Values: %d %d %d %d %d %d %d %d\n",
                                     proxValues[0], proxValues[1], proxValues[2], proxValues[3],
                                     proxValues[4], proxValues[5], proxValues[6], proxValues[7]);
            e_send_uart1_char(str, str_length);


            // Read the values of all calibrated proximity sensors
            int calibratedValues[8];
            for (int i = 0; i < 8; i++) {
                calibratedValues[i] = get_calibrated_prox(i);
            }


            // Print the values of all calibrated proximity sensors
            char str2[100];
            int str_length2 = sprintf(str2, "Calibrated Proximity Sensor Values: %d %d %d %d %d %d %d %d\n",
                                     calibratedValues[0], calibratedValues[1], calibratedValues[2], calibratedValues[3],
                                     calibratedValues[4], calibratedValues[5], calibratedValues[6], calibratedValues[7]);
            e_send_uart1_char(str2, str_length2);

            // Wait for a certain interval (1 second in this case)
            chThdSleepMilliseconds(1000);
        }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
