#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include "epuck1x/uart/e_uart_char.h"
#include "stdio.h"
#include "serial_comm.h"

#include "sensors/proximity.h"

#include "motors.h"
#include "sensors/VL53L0X/VL53L0X.h"


messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

// Define the MAX_SPEED constant with a value of 300
#define MAX_SPEED 300;

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();
    serial_start();
    messagebus_init(&bus, &bus_lock, &bus_condvar);
    proximity_start(0);
    calibrate_ir();
    motors_init();
    VL53L0X_start();

    bool objectOnRight = false;
    bool objectOnLeft = false;


    /* Infinite loop. */
    while (1) {

      uint16_t distance_mm = VL53L0X_get_dist_mm();
      int prox[8];
      for (int i = 0; i < 8; i++) {
          prox[i] = get_calibrated_prox(i);
      }

        // Use the sensor data for different purposes
      int front_distance = (prox[0] + prox[1]) / 2;
      int left_distance = (prox[5] + prox[6]) / 2;
      int right_distance = (prox[1] + prox[2]) / 2;
      int back_left_distance = prox[4];
      int back_right_distance = prox[3];

      int left_speed = MAX_SPEED;
      int right_speed = MAX_SPEED;

      if(distance_mm > 10 && distance_mm < 20){
        left_speed = 0;
        right_speed = 0;
        objectOnLeft = false;
        objectOnRight = false;
      }

      else if(distance_mm < 10){
        left_speed = -MAX_SPEED;
        right_speed = -MAX_SPEED;
        objectOnLeft = true;
        objectOnRight = false;
      }
      else if (left_distance > 200){
        objectOnLeft = true;
      }
      else if(right_distance > 200){
        objectOnRight = true;
      }
      else if (back_left_distance > 200){
        objectOnLeft = true;
      }
      else if(back_right_distance > 200){
        objectOnRight = true;
      }
      else if(distance_mm > 100){
        left_speed = MAX_SPEED * speed_increase_factor;
        right_speed = MAX_SPEED * speed_increase_factor;
      }

      while (objectOnLeft && prox[7] < 150  ){
        left_speed = - MAX_SPEED;
        right_speed = MAX_SPEED;
        objectOnLeft = false;
      }
      while (objectOnRight && prox[0]<150){
        left_speed = MAX_SPEED;
        right_speed = - MAX_SPEED;
        objectOnRight = false;
      }




      left_motor_set_speed(left_speed);
      right_motor_set_speed(right_speed);
    	//waits 1 second
      chThdSleepMilliseconds(100);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
