#include "setup.h"
#include "mainloop.h"
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


void main_loop() {
  while(1) {
    printf("Main Looping\n");
    usleep(1000000);
  }
}