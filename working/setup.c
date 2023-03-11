#include "setup.h"
#include "mainloop.h"
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


/**
 *  Disable NMEA Message
*/
void disable_GNGVT(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t gngvt_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x46};
  i2cWriteDevice(i2c_handle, gngvt_off, sizeof(gngvt_off));
  printf("GNGVT off: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("GNGVT OFF Configuration Accepted\n");
      printf("Message Class and ID: 0x%02x 0x%02x\n", acknakmsg->class_id, acknakmsg->msg_id);
      break;
    }
    if(acknakmsg->class_id == 0x05 && acknakmsg->msg_id == 0x00) { 
      printf("Message Class and ID: 0x%02x, 0x%02x\n", acknakmsg->class_id, acknakmsg->msg_id);
      printf("GNGVT off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  Disable NMEA Message
*/
void disable_GNGSV(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t gngsv_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x38};
  i2cWriteDevice(i2c_handle, gngsv_off, sizeof(gngsv_off));
  printf("GNGSV off: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("GNGSV OFF Configuration Accepted\n");
      printf("Message Class and ID: 0x%02x 0x%02x\n", acknakmsg->class_id, acknakmsg->msg_id);
      printf("Checksum: 0x%02x 0x%02x\n", acknakmsg->ck_a, acknakmsg->ck_b);
      break;
    }
    if(acknakmsg->msg_id == 0x00) { 
      printf("GNGSV off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  Disable NMEA Message
*/
void disable_GNGSA(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t gngsa_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x31};
  i2cWriteDevice(i2c_handle, gngsa_off, sizeof(gngsa_off));
  printf("GNGSA off: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("GNGSA OFF Configuration Accepted\n");
      printf("Message Class and ID: 0x%02x 0x%02x\n", acknakmsg->class_id, acknakmsg->msg_id);
      break;
    }
    if(acknakmsg->msg_id == 0x00) { 
      printf("GNGSA off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  Disable NMEA Message
*/
void disable_GNGLL(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t gngll_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a};
  i2cWriteDevice(i2c_handle, gngll_off, sizeof(gngll_off));
  printf("GNGLL off: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("GNGLL OFF Configuration Accepted\n");
      printf("Message Class and ID: 0x%02x 0x%02x\n", acknakmsg->class_id, acknakmsg->msg_id);
      break;
    }
    if(acknakmsg->msg_id == 0x00) { 
      printf("GNGLL off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  Disable NMEA Message
*/
void disable_GNGGA(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t gngga_off[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x23};
  i2cWriteDevice(i2c_handle, gngga_off, sizeof(gngga_off));
  printf("GNGGA off: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("GNGGA OFF Configuration Accepted\n");
      printf("Message Class and ID: 0x%02x 0x%02x\n", acknakmsg->class_id, acknakmsg->msg_id);
      break;
    }
    if(acknakmsg->msg_id == 0x00) { 
      printf("GNGGA off configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  Enable UBX NAV PVT
*/
void enable_navPVT(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t config_navpt_on[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xe2};
  i2cWriteDevice(i2c_handle, config_navpt_on, sizeof(config_navpt_on));
  printf("UBX NAV-PVT ON: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("UX NAV-PVT ON Configuration Accepted\n");
      break;
    }
    if(acknakmsg->msg_id == 0x00) { 
      printf("NAV-PVT ON configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  Enable UBX NAV POSLLH
*/
void enable_navPOSLLH(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t config_navposllh_on[] = {0xb5, 0x62, 0x06, 0x01, 0x08, 0x00, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0xbf};
  i2cWriteDevice(i2c_handle, config_navposllh_on, sizeof(config_navposllh_on));
  printf("UBX NAV-POSLLH ON: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("UX NAV-POSLLH ON Configuration Accepted\n");
      break;
    }
    if(acknakmsg->msg_id == 0x00) { 
      printf("NAV-POSLLH ON configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  SET RATE 1HZ
*/
void setRate_1hz(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  uint8_t config_rate[] = {0xb5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xe8, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x37};
  i2cWriteDevice(i2c_handle, config_rate, sizeof(config_rate));
  printf("1HZ Rate: sent\n\twaiting for response...\n");
  while(acknakmsg->class_id != 0x05) {
    i2cReadDevice(i2c_handle, (char*)acknakmsg, sizeof(acknakmsg));
    if(acknakmsg->msg_id == 0x01) {
      printf("Module Transfer Rate 1hz Configuration Accepted\n");
      printf("Message Class and ID: 0x%02x 0x%02x\n", acknakmsg->class_id, acknakmsg->msg_id);
      break;
    }
    if(acknakmsg->msg_id == 0x00) { 
      printf("Module Transfer Rate 1hz configuration NOT Accepted\n\tEverything is probably fine, though...\n");
      break;
    }
    usleep(200000);
  }
  free(acknakmsg);
  printf("\n");
}

/**
 *  SET RATE 5HZ
*/
void setRate_5hz(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  printf("Not Implemented\n");

  free(acknakmsg);
}

/**
 *  SET RATE 10HZ
*/
void setRate_10hz(int i2c_handle) {
  ubx_ack_msg *acknakmsg = (ubx_ack_msg*)malloc(sizeof(ubx_ack_msg));
  printf("Not Implemented\n");

  free(acknakmsg);
}