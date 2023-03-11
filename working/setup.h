#ifndef SETUP_H
#define SETUP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


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


void disable_GNGVT(int i2c_handle);

void disable_GNGSV(int i2c_handle);

void disable_GNGSA(int i2c_handle);

void disable_GNGLL(int i2c_handle);

void disable_GNGGA(int i2c_handle);

void enable_navPVT(int i2c_handle);

void enable_navPOSLLH(int i2c_handle);

void setRate_1hz(int i2c_handle);

void setRate_5hz(int i2c_handle);

void setRate_10hz(int i2c_handle);


// more disables
// some enables

// consider trying the high rate navpvt 



#endif