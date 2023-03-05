## Raspberry Pi with GPS Module

### This project is to build a for-fun, real-time graphical location display system using:
 #### Hardware: 
 - Raspberry Pi model 4
 - Sparkfun U-Blox ZOE-M8Q GPS Module
 #### Comms Interface: 
 - I2C via GPIO
#### Required Libraries:
- pigpio  
The wiringpi library is deprecated. pigpio library is available at:  
https://abyz.me.uk/rpi/pigpio/download.html  
#### DOCUMENTATION: (for the GPS module)
 * https://content.u-blox.com/sites/default/files/products/documents/u-blox8-M8_ReceiverDescrProtSpec_UBX-13003221.pdf
 * https://cdn.sparkfun.com/assets/1/c/f/8/a/ZOE-M8_DataSheet__UBX-16008094_.pdf
 * https://cdn.sparkfun.com/assets/a/a/d/2/2/NEO-M8P_HardwareIntegrationManual__UBX-15028081_.pdf
***
#### Overview: an outline of challenges I have not yet resolved
- The ZOE-M8Q GPS module supports a few different communication protocols. The two of interest to me are NMEA and the proprietary UBX protocol designed by u-blox.   
- By default, the module outputs GNSS data via NMEA 0183 sentences. The challenge with this protocol is that message length is variable if there are no values to report. For example, the NMEA RMC output provides "speed over ground" data. For a stationary module, the "spd" field in the sentence would be empty, causing the buffer array to read into the following sentence.
- UBX messages are more defined in their binary size, but the module must be configured to output UBX. And this is about as far as I've gotten. 
- The protocol documentation describes the binry/hex format of UBX messages and the formula to calculate message checksums. U-blox also provided the U-Center graphical tool, available only on Windows, to build UBX messages. Sending messages is pretty straightforward with the pigpio library. Reading responses has been a little more challenging with some mixed results. 



