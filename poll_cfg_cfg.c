
/**
 * DESCRIPTION:
 * Sends UBX config messages to the module to specify module output.
 * 
 * 
 * DOCUMENTATION: (for the GPS module)
 * https://content.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_UBX-13003221.pdf
 * https://cdn.sparkfun.com/assets/1/c/f/8/a/ZOE-M8_DataSheet__UBX-16008094_.pdf
 * https://cdn.sparkfun.com/assets/a/a/d/2/2/NEO-M8P_HardwareIntegrationManual__UBX-15028081_.pdf
 * 
 * 
 * ISSUES:
 * READING MODULE RESPONSES!
 * Module response messages are ACK or NAK in the format of: 
 * 0xb5 0x65 0x05 0x01(or 0x00 for NAK) <CLASS OF ACK/NAK MSG> <ID OF ACK/NAK MSG> <CKSUM A> <CKSUM B>
 * 
 * A module ACK/NAK message is supposed to be sent within 1 second of recepit. This program output alternates between
 * all even messages with an ACK or all odd messages with an ACK, but never all. Suggests a timing issue with writing
 * configuration or reading responses. 
 * 
*/


#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <string.h>

#define I2C_ADDRESS 0x42






int main(int argc, char** argv) {
  int response;

  const uint8_t poll_messages[8] = {0xb5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x8, 0x19};
  //{0xb5, 0x62, 0x06, 0x09, 0x00, 0x00, 0xf, 0x33};

  // initialize GPIO library
  if (gpioInitialise() < 0) {
    printf("Error: Failed to initialize GPIO library.\n");
    return -1;
  }

  // initialize i2c bus
  int i2c_handle = i2cOpen(1, I2C_ADDRESS, 0);
  if (i2c_handle < 0) {
    printf("Error: Failed to open I2C device.\n");
    gpioTerminate();
    return -1;
  }

  i2cWriteDevice(i2c_handle, (char *)poll_messages, sizeof(poll_messages[8]));

  // try to read some UBX-NAV-PVT messages
  int8_t nav_pvt_data[98];
  int count = 0;
  while(count != 3) {
    i2cReadDevice(i2c_handle, nav_pvt_data, sizeof(nav_pvt_data));
    printf("Data read from i2c device:\n");
    for(int i = 0; i < sizeof(nav_pvt_data); i++) {
      printf("%02x ", nav_pvt_data[i]);
    }
    printf("\n");
    usleep(500000);
    count++;
  }

  i2cClose(i2c_handle);
  gpioTerminate();

  return 0;
}
