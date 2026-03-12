#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "network.h"

char *Data;  // pointer to the buffer that HTTP data will end up in.

int main() {
  stdio_init_all();
  sleep_ms(100);
  Init_Wifi("FreeWifi","Welcome1");  // update with wifi network name and password
  sleep_ms(100);
  long BuffSize = 19100;
  Data = malloc(BuffSize * sizeof(char));	

  int err = Get_HTTP_Data("www.example.com",80,"/index.html",Data,&BuffSize);

  printf("err: %d\n",err);
  printf("length of Data: %d\n",BuffSize);
  printf("Data:[0..3] %02x,%02x,%02x,%02x\n",Data[0],Data[1],Data[2],Data[3]);

  while(true) 
  {
    sleep_ms(10);
  }
}



