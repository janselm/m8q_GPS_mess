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
  const int protoMessageLength = 16, rateMessageLength = 14;
  int response;

    /**
     * set up configuration messages. The messages are assigned to the array config_procol so that they can be sent to the
     * module in a loop without repeating write and sleep code
    */
    const uint8_t config_protocol[][16] = {
    // disable nmea
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x23}, // GxGGA off
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a}, // GxGLL off
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31}, // GxGSA off
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38}, // GxGSV off
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x3f}, // GxRMC off
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46}, // GxVTG off
    // disable ubx
      // { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0xDC }, // NAV-PVT off
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0xb9}, // NAV-POSLLH off
      //{0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xc0}, // NAV-STATUS off
    // enable ubx
      {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xe2} // NAV-PVT on
      //{ 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x13, 0xBE }, // NAV-POSLLH on
      //{ 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x14, 0xC5 }, // NAV-STATUS on
  };
  printf("Protocol configuration array populated.\n");

  /**
   * The rate config messages are set up th same as the protocol config messages, but in a separate array
   * because the messages are shorter than the protocol config and need to be initialized into a smaller array.
  */
  const uint8_t config_rate[][14] = {
      // rate
      //{0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12}, // (10Hz)
      //{0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A}, // (5Hz)
      {0xb5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xe8, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x37} // (1Hz)
  };
  printf("Rate configuration array populated.\n");


  /**
   * calc the number of arrays in the array. This is to make the code more manageable if it is updated. 
   * if more messages are added to the code, the loop settings will not need to be changed
  */
  int numProtoRows = sizeof(config_protocol) / sizeof(config_protocol[0]);
  int numRateRows = sizeof(config_rate) / sizeof(config_rate[0]);

  printf("  Proto config messages: %d\n", numProtoRows);
  printf("  Rate config messages: %d\n", numRateRows);

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

  // write some config messages
  int acknak[numProtoRows];
  uint8_t ack_response[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  for(int r = 0; r < numProtoRows; r++) {
    acknak[r] = 0;
    i2cWriteDevice(i2c_handle, (char *)config_protocol[r], sizeof(config_protocol[r]));
    // while(ack_response[2] != 0x05) {
    //   i2cReadDevice(i2c_handle, (char *)ack_response, sizeof(ack_response));
    //   if(ack_response[3] == 0x01) {
    //     acknak[r]= 1;
    //   } else if(ack_response == 0x00) {
    //     acknak[r] = 0;
    //   }
    // }
  }



  // reads from a pseudo-boolean array to print the ack/nak resposne status of each message
  // for(int a = 0; a < numProtoRows; a++) {
  //   if(acknak[a] == 0) {
  //     printf("Message %d not acknowledged\n", a);
  //   } else if(acknak[a] == 1) {
  //     printf("Message %d acknowledged\n", a);
  //   }
  // }

  for(int r = 0; r < numRateRows; r++) {
    acknak[r] = 0;
    i2cWriteDevice(i2c_handle, (char *)config_rate[r], sizeof(config_rate[r]));
    // while(ack_response[2] != 0x05) {
    //   i2cReadDevice(i2c_handle, (char *)ack_response, sizeof(ack_response));
    //   if(ack_response[3] == 0x01) {
    //     acknak[r]= 1;
    //   } else if(ack_response == 0x00) {
    //     acknak[r] = 0;
    //   }
    // }
  }


  // reads from a pseudo-boolean array to print the ack/nak resposne status of each message
  // for(int a = 0; a < numRateRows; a++) {
  //   if(acknak[a] == 0) {
  //     printf("Message %d not acknowledged\n", a);
  //   } else if(acknak[a] == 1) {
  //     printf("Message %d acknowledged\n", a);
  //   }
  // }

  // try to read some UBX-NAV-PVT messages
  int8_t nav_pvt_data[98];
  int count = 0;
  while(count != 20) {
    i2cReadDevice(i2c_handle, nav_pvt_data, sizeof(nav_pvt_data));
    printf("Data read from i2c device:\n");
    for(int i = 0; i < sizeof(nav_pvt_data); i++) {
      printf("%02x ", nav_pvt_data[i]);
    }
    printf("\n");
    usleep(500000);
    count++;






    // int32_t longitude = *((int32_t*)&nav_pvt_data[24]); 
    // int32_t latitude = *((int32_t*)&nav_pvt_data[28]);

    // double long_degrees = (double)longitude / 10000000.0;
    // double lat_degrees = (double)latitude / 10000000.0;

    // printf("Longitude: %.6f\n", long_degrees);
    // printf("Latitude: %.6f\n", lat_degrees);
    // usleep(500000);
  }

  i2cClose(i2c_handle);
  gpioTerminate();

  return 0;
}
