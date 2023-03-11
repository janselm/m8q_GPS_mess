#include "setup.h"
#include "mainloop.h"
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


#define I2C_ADDRESS 0x42

int main(int argc, char* argv[]) {

  ubx_nav_posllh nav_posllh;
  ubx_nav_pvt_msg nav_pvt;
  ubx_ack_msg acknak;

  // initialize GPIO library
  if (gpioInitialise() < 0) {
    printf("Error: Failed to initialize GPIO library.\n");
    return -1;
  }
  printf("Gpio library initialized\n");
  // initialize i2c bus
  int i2c_handle = i2cOpen(1, I2C_ADDRESS, 0);
  if (i2c_handle < 0) {
    printf("Error: Failed to open I2C device.\n");
    gpioTerminate();
    return -1;
  }
  printf("i2c initialized\n\n");

  disable_GNGVT(i2c_handle);
  disable_GNGSV(i2c_handle);
  disable_GNGSA(i2c_handle);
  disable_GNGLL(i2c_handle);
  disable_GNGGA(i2c_handle);
  enable_navPOSLLH(i2c_handle);
  enable_navPVT(i2c_handle);
  setRate_1hz(i2c_handle);

  main_loop();
  i2cClose(i2c_handle);
  gpioTerminate();
  return 0;
} 