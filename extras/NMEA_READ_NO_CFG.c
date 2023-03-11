/**
 * DESCRIPTION:
 * Read GPS data from ZOE-M8Q gps module on Raspberry Pi model 4 via GPIO and I2C.
 * The pigpio library is used to interface with the GPS module. 
 * This program sends no configuration message to the GPS module.
 * 
 * All GPS data is read from the module as an NMEA sentence and is printed to the terminal
 * 
 * DOCUMENTATION: (for the GPS module)
 * https://content.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_UBX-13003221.pdf
 * https://cdn.sparkfun.com/assets/1/c/f/8/a/ZOE-M8_DataSheet__UBX-16008094_.pdf
 * https://cdn.sparkfun.com/assets/a/a/d/2/2/NEO-M8P_HardwareIntegrationManual__UBX-15028081_.pdf
 * 
 * 
 * ISSUES:
 * Occasional ouput string contains unicode characters. 
 * No configuration of module.
 * Buffer size is not accurate for each message
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pigpio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


#define I2C_ADDRESS 0x42

int main(int argc, char **argv) {
  // initialize pigpio library
  if(gpioInitialise() < 0) {
    printf("Error: Failed to initialize pigpio library");
    return -1;
  }

  // program code
  // Open I2C device
  int i2c_handle = i2cOpen(1, I2C_ADDRESS, 0);


while (1) {
    // Read NMEA sentence
    char buffer[80];
    int num_bytes = i2cReadDevice(i2c_handle, buffer, 80);
    printf("%.*s\n", num_bytes, buffer);
    printf("\t-----\n");
    // Check if sentence is $GNGLL
    if (strncmp(buffer, "$GNRMC", 6) == 0) {
        // Print a message
        printf("We got some GNRMC\n");
    }

    // Wait a little bit before reading again
    usleep(200000);
}


// original loop
  // while (1) {
  //     // Read NMEA sentence
  //     char buffer[80];
  //     int num_bytes = i2cReadDevice(i2c_handle, buffer, 80);

  //     // Print NMEA sentence
  //     printf("%.*s\n", num_bytes, buffer);

  //     // Wait a little bit before reading again
  //     usleep(200000);
  // }

  // Close I2C device
  i2cClose(i2c_handle);

  // elegantly end the program
  gpioTerminate();
  return 0;
}


