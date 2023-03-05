/**
 * takes an NMEA GNGLL sentence as an arg and returns a formatted string
 * that can be copied and pasted into an online mapping tool because I'm too lazy
 * to do it myself. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <GNGLL string>\n", argv[0]);
        return 1;
    }

    char *gngll = argv[1];
    char *lat_str = strtok(gngll, ",");
    char *lat_dir = strtok(NULL, ",");
    char *long_str = strtok(NULL, ",");
    char *long_dir = strtok(NULL, ",");
    
    double lat = atof(lat_str);
    double longit = atof(long_str);
    
    int lat_deg = (int)lat / 100;
    double lat_min = (lat - (lat_deg * 100)) / 60;
    double latitude = lat_deg + lat_min;
    char lat_dir_char = lat_dir[0];

    int long_deg = (int)longit / 100;
    double long_min = (longit - (long_deg * 100)) / 60;
    double longitude = -(long_deg + long_min);
    char long_dir_char = long_dir[0];

    printf("%f %c, %f %c\n", latitude, lat_dir_char, longitude, long_dir_char);

    return 0;
}
