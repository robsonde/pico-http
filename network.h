#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/dns.h"
#include "lwip/apps/http_client.h"

bool Init_Wifi(char*,char*);

int Get_HTTP_Data(char*, int, char*, char*, long*);



