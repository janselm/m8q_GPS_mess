#include "setup.h"
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>



#define I2C_ADDRESS 0x42

typedef struct {
    ////////////////// Header Info //////////////////
    uint8_t header1;         // First sync character 0xB5
    uint8_t header2;         // Second sync character 0x62
    uint8_t msgClass;        // Message class (0x01 for NAV)
    uint8_t id;              // Message ID (0x07 for PVT)
    uint16_t length;         // Length of payload data (92 bytes)
    ////////////////// Payload Data //////////////////
    uint32_t iTOW;           // GPS time of week in milliseconds
    uint16_t year;           // Year (UTC)
    uint8_t month;           // Month, range 1..12 (UTC)
    uint8_t day;             // Day of month, range 1..31 (UTC)
    uint8_t hour;            // Hour of day, range 0..23 (UTC)
    uint8_t min;             // Minute of hour, range 0..59 (UTC)
    uint8_t sec;             // Seconds of minute, range 0..59 (UTC)
    uint8_t valid;           // Validity flags for date/time
    uint32_t tAcc;           // Time accuracy estimate in nanoseconds
    int32_t nano;            // Fraction of second, range -1e9..1e9 (UTC)
    uint8_t fixType;         // GNSSfix Type, range 0..5
    uint8_t flags;           // Fix Status Flags
    uint8_t flags2;          // Additional Fix Status Flags
    uint8_t numSV;           // Number of SVs used in Nav Solution
    int32_t lon;             // Longitude in degrees scaled by 1e-7
    int32_t lat;             // Latitude in degrees scaled by 1e-7
    int32_t height;          // Height above ellipsoid in mm
    int32_t hMSL;            // Height above mean sea level in mm
    uint32_t hAcc;           // Horizontal accuracy estimate in mm
    uint32_t vAcc;           // Vertical accuracy estimate in mm
    int32_t velN;            // NED north velocity in mm/s
    int32_t velE;            // NED east velocity in mm/s
    int32_t velD;            // NED down velocity in mm/s
    int32_t gSpeed;          // Ground Speed (2-D) in mm/s
    int32_t headMot;         // Heading of motion in degrees scaled by 1e-5
    uint32_t sAcc;           // Speed accuracy estimate in mm/s
    uint32_t headAcc;        // Heading accuracy estimate in degrees scaled by 1e-5
    uint16_t pDOP;           // Position DOP
    uint8_t reserved1[6];    // Reserved for future use
    int32_t headVeh;         // Heading of vehicle (2-D) in degrees scaled by 1e-5
    int16_t magDec;          // Magnetic declination at current position in degrees scaled by 1e-2
    uint16_t magAcc;         // Magnetic declination accuracy in degrees scaled by 1e-2
    ////////////////// Checksum //////////////////
    uint8_t checksumA;       // Checksum byte A
    uint8_t checksumB;       // Checksum byte B
} ubx_nav_pvt_msg;


typedef struct {
    uint8_t sync_char_1;  // First sync character of the message header
    uint8_t sync_char_2;  // Second sync character of the message header
    uint8_t class_id;     // Class ID of the acknowledged message
    uint8_t msg_id;       // Message ID of the acknowledged message
    uint16_t length;      // Message length of the acknowledged message
    uint8_t cls_id;       // Class ID of the message payload
    uint8_t msg_id_pload; // Message ID of the message payload
    uint8_t ck_a;         // First checksum byte of the acknowledged message
    uint8_t ck_b;         // Second checksum byte of the acknowledged message
} ubx_ack_msg;

typedef struct {
    uint8_t sync_char_1;  // First sync character of the message header
    uint8_t sync_char_2;  // Second sync character of the message header
    uint32_t iTOW;  // GPS Millisecond Time of Week
    int32_t lon;    // Longitude (degrees * 1e-7)
    int32_t lat;    // Latitude (degrees * 1e-7)
    int32_t height; // Height above ellipsoid (millimeters)
    int32_t hMSL;   // Height above mean sea level (millimeters)
    uint32_t hAcc;  // Horizontal accuracy estimate (millimeters)
    uint32_t vAcc;  // Vertical accuracy estimate (millimeters)
    uint8_t ck_a;         // First checksum byte of the acknowledged message
    uint8_t ck_b;         // Second checksum byte of the acknowledged message
} ubx_nav_posllh;


//////////////////////////////////////////
// temp fx to print binary data for debugging
void print_binary(void* data, size_t size) {
    uint8_t* bytes = (uint8_t*) data;
    for (int i = size - 1; i >= 0; i--) {
        for (int j = 7; j >= 0; j--) {
            printf("%d", (bytes[i] >> j) & 1);
        }
    }
    fflush(stdout);
    printf("\n");
}
////////////////////////////////////////


int main(int argc, char* argv[]) {

  ubx_nav_pvt_msg nav_pvt_msg;
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
  printf("i2c initialized\n");

  // turn off gnvtg
  uint8_t gngvt_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46};
  i2cWriteDevice(i2c_handle, gngvt_off, sizeof(gngvt_off));
  printf("GNGVT off: sent\n\twaiting for response...\n");
  while(acknak.class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
    if(acknak.msg_id == 0x01) {
      printf("GNGVT OFF Configuration Accepted\n");
      printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
      break;
    } 
    if(acknak.msg_id == 0x00) { 
      printf("GNGVT off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
  }
  memset(&acknak, 0, sizeof(acknak));

  // turn off gngsv
  uint8_t gngsv_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38};
  i2cWriteDevice(i2c_handle, gngsv_off, sizeof(gngsv_off));
  printf("GNGSV off: sent\n\twaiting for response...\n");
  while(acknak.class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
    if(acknak.msg_id == 0x01) {
      printf("GNGSV OFF Configuration Accepted\n");
      printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
      break;
    }
      if(acknak.msg_id == 0x00) { 
        printf("GNGSV off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
        break;
    }
  }
  memset(&acknak, 0, sizeof(acknak));

  // turn off gngsa
  uint8_t gngsa_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31};
  i2cWriteDevice(i2c_handle, gngsa_off, sizeof(gngsa_off));
  printf("GNGSA off: sent\n\twaiting for response...\n");
  while(acknak.class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
    if(acknak.msg_id == 0x01) {
      printf("GNGSA OFF Configuration Accepted\n");
      printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
      break;
    }
    if(acknak.msg_id == 0x00) { 
      printf("GNGSA off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
  }
  memset(&acknak, 0, sizeof(acknak));

  // turn off gngll
  uint8_t gngll_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a};
  i2cWriteDevice(i2c_handle, gngll_off, sizeof(gngll_off));
  printf("GNGLL off: sent\n\twaiting for response...\n");
  while(acknak.class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
    if(acknak.msg_id == 0x01) {
      printf("GNGLL OFF Configuration Accepted\n");
      printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
      break;
    }
    if(acknak.msg_id == 0x00) { 
      printf("GNGLL off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
  }
  memset(&acknak, 0, sizeof(acknak));

// turn off gngga
  uint8_t gngga_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x23};
  i2cWriteDevice(i2c_handle, gngga_off, sizeof(gngga_off));
  printf("GNGGA off: sent\n\twaiting for response...\n");
  while(acknak.class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
    if(acknak.msg_id == 0x01) {
      printf("GNGGA OFF Configuration Accepted\n");
      printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
      break;
    }
    if(acknak.msg_id == 0x00) { 
      printf("GNGGA off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
  }
  memset(&acknak, 0, sizeof(acknak));

  // config message to turn nav-pvt messages on
  // uint8_t config_navpt_on[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xe2};
  // i2cWriteDevice(i2c_handle, config_navpt_on, sizeof(config_navpt_on));
  // printf("UBX NAV-PVT ON: sent\n\twaiting for response...\n");
  // while(acknak.class_id != 0x05) {
  //   i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
  //   if(acknak.msg_id == 0x01) {
  //     printf("NAV-PVT ON Configuration Accepted\n");
  //     printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
  //     break;
  //   }
  //   if(acknak.msg_id == 0x00) { 
  //     printf("NAV-PVT ON configuration NOT Accepted\n\tEverything is probably fine, though...\n");
  //     break;
  //   }
  // }
  // memset(&acknak, 0, sizeof(acknak));

  // config message to turn nav-posllh on
  uint8_t config_navposllh_on[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xbf};
  i2cWriteDevice(i2c_handle, config_navposllh_on, sizeof(config_navposllh_on));
  printf("UBX NAV-POSLLH ON: sent\n\twaiting for response...\n");
  while(acknak.class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
    if(acknak.msg_id == 0x01) {
      printf("UX NAV-POSLLH ON Configuration Accepted\n");
      printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
      break;
    }
    if(acknak.msg_id == 0x00) { 
      printf("NAV-POSLLH ON configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
  }
  memset(&acknak, 0, sizeof(acknak));


  printf("\n");
  // config message to set the message rate
  uint8_t config_rate[] = {0xb5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xe8, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x37};
  i2cWriteDevice(i2c_handle, config_rate, sizeof(config_rate));
  while(acknak.class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)&acknak, sizeof(acknak));
    if(acknak.class_id == 0x01) {
      printf("Module Transfer Rate 1hz Configuration Accepted\n");
      printf("Checksum: 0x%02x 0x%02x\n", acknak.ck_a, acknak.ck_b);
      break;
    }
    if(acknak.class_id == 0x00) { 
      printf("Module Transfer Rate 1hz configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
  }
  printf("\n");
  memset(&acknak, 0, sizeof(acknak));

  
  // read nav-pvt messages from the device
  while(1) {
    i2cReadDevice(i2c_handle, (char*)&nav_pvt_msg, sizeof(nav_pvt_msg));
    if(nav_pvt_msg.msgClass == 0x01 && nav_pvt_msg.id == 0x07) {
      printf("Header: 0x%x 0x%x, Class: 0x%x, ID: 0x%x, Length: %u-bytes \n", nav_pvt_msg.header1, nav_pvt_msg.header2, nav_pvt_msg.msgClass, nav_pvt_msg.id, nav_pvt_msg.length);
      printf("ITOW: %u\n", nav_pvt_msg.iTOW);
      printf("Year: %u, Month: %ld, Day: %ld, Hour: %ld, Minute: %ld, Second: %ld\n", nav_pvt_msg.year, nav_pvt_msg.month, nav_pvt_msg.day, nav_pvt_msg.hour, nav_pvt_msg.min, nav_pvt_msg.sec);
      printf("Time Valid Flags: %u\n", nav_pvt_msg.valid);
      printf("Fix Type: %u\n", nav_pvt_msg.fixType);
      printf("Flags: \n", nav_pvt_msg.flags);
      printf("Flags2: \n", nav_pvt_msg.flags2);

      // printf("Binary: \n");
      // printf("ITOW: ");
      // print_binary(&nav_pvt_msg.iTOW, sizeof(nav_pvt_msg.iTOW));
      // printf("year: ");
      // print_binary(&nav_pvt_msg.year, sizeof(nav_pvt_msg.year));
      // printf("month: ");
      // print_binary(&nav_pvt_msg.month, sizeof(nav_pvt_msg.month));
      // printf("day: ");
      // print_binary(&nav_pvt_msg.day, sizeof(nav_pvt_msg.day));
      // printf("hour: ");
      // print_binary(&nav_pvt_msg.hour, sizeof(nav_pvt_msg.hour));
      // printf("min: ");
      // print_binary(&nav_pvt_msg.min, sizeof(nav_pvt_msg.min));
      // printf("sec: ");
      // print_binary(&nav_pvt_msg.sec, sizeof(nav_pvt_msg.sec));
      // print_binary(&nav_pvt_msg.valid, sizeof(nav_pvt_msg.valid));
      // printf("\n");
      // printf("Date/Time Valid: %ld\n", nav_pvt_msg.valid);
      // printf("Fix Type: \n");
      // print_binary(&nav_pvt_msg.fixType, sizeof(nav_pvt_msg.fixType));
      // printf("Flags 1:\n");
      // print_binary(&nav_pvt_msg.flags, sizeof(nav_pvt_msg.flags));
      // printf("Flags2: \n");
      // print_binary(&nav_pvt_msg.flags2, sizeof(nav_pvt_msg.flags2));
      // printf("Num Sats:\n");
      // print_binary(&nav_pvt_msg.numSV, sizeof(nav_pvt_msg.numSV));
      printf("-----------------\n");
    }
    usleep(500000);
  }
  i2cClose(i2c_handle);
  gpioTerminate();
  return 0;
}


// Fix Type: 
// 00011010
// Flags 1:
// 00001110
// Flags2: 
// 00011001
// Num Sats:
// 11000001
