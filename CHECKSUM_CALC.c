/**
 * takes a UBX sentence and calculates the checksum.
 * The UBX sentence passed as arguments must be in hex format, spaced, and
 * without commas. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


int main(int argc, char *argv[]) {

  printf("argc = %d (including program name)\n", argc);
  int ck_a = 0, ck_b = 0;
  int passedArgs = argc - 1;
  uint8_t *buffer;

  buffer = malloc(sizeof(uint8_t)*passedArgs);
  if(buffer == NULL) {
    printf("Error: malloc failed");
    return 1;
  }

  for(int i = 0; i < passedArgs; i++) {
    buffer[i] = (uint8_t)strtol(argv[i+1], NULL, 16);
  }

  for(int i = 2; i < passedArgs; i++) {
    ck_a = ck_a + buffer[i];
    if(ck_a > (int)0xFF) {
      ck_a &= 0xFF;
    }
    ck_b = ck_b + ck_a;
      if(ck_b > (int)0xFF) {
        ck_b &= 0xFF;
    }
  }

  printf("Checksum: %x, %x\n", ck_a, ck_b);


  printf("Full message with checksum: ");
  for(int k = 0; k < passedArgs; k++) {
    printf("0x%02x, ", buffer[k]);
  }
  printf("0x%x, 0x%x\n", ck_a, ck_b);

  printf("Array formatted:\n");
  printf("{");
  for(int k = 0; k < passedArgs; k++) {
    printf("0x%02x, ", buffer[k]);
  }
  printf("0x%x, 0x%x}\n", ck_a, ck_b);

  free(buffer);
  return 0;
}

