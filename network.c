#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/dns.h"
#include "lwip/apps/http_client.h"

char *HttpBuff;  //this is pointer to the buff we want HTTP data to arrive in.
bool HttpDone=false;  // internal flag to know when resuest is good.
int  Results;  // HTTP results code
long HttpRecv=0;  // How many bytes have we received so far 
long HttpBuffSize=0;  // How big is the buffer
long HttpResponseSize=0;  // How big is the expected response


//
//  Init_Wifi() sets up wifi hardware and connects to a wifi network, it assumes DHCP.
//  To use: pass in the Wi-Fi network name as a string, and the network password as a string.
//  Example:      bool Err = Init_Wifi("MyNetwork","MyPassword");	
//  returns true if network connected, returns false if there was an error.
//
bool Init_Wifi(const char *SSID,const char *PASSWD) {
  if (cyw43_arch_init_with_country(CYW43_COUNTRY_NEW_ZEALAND))  // Change this to relevant country.
  {
    printf("failed to initialize\n");
    return false;
  }
  cyw43_arch_enable_sta_mode();
  if (cyw43_arch_wifi_connect_blocking(SSID, PASSWD, CYW43_AUTH_WPA2_MIXED_PSK))
  {
    printf("failed to connect\n");
    return false;
  }
  const ip_addr_t *dns_server = dns_getserver(0);
  printf("Wifi connected!\n");
  return true;
}



//
// Internal function to handle results from network operations
// deals with network errors and failures. 
//
void result_fn(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
  Results = srv_res;  // Http results code I.E 200-OK
  HttpResponseSize = rx_content_len;
  if (httpc_result == HTTPC_RESULT_OK) {
    printf("Request successful, received %u bytes\n", rx_content_len);
    HttpDone=true;
  } else {
  switch (httpc_result) {
    case HTTPC_RESULT_ERR_CONNECT:
      printf("Error: Connection failed\n");
      HttpDone=true;
      break;
    case HTTPC_RESULT_ERR_HOSTNAME:
      printf("Error: Hostname resolution failed\n");
      HttpDone=true;
      break;
    case HTTPC_RESULT_ERR_CLOSED:
      printf("Error: Connection closed unexpectedly\n");
      HttpDone=true;
      break;
    default:
      printf("Error: Unknown error occurred\n");
      HttpDone=true;
      break;
    }
  }
}



//
// Internal function to deal with HTTP headers.
// This could be expended if you require access to the headers.
// Similar to "recv_fn" - you can use "pbuf_copy_partial(hdr, HeaderBuff, hdr->tot_len, 0);"
//
static err_t headers_done_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len) {
  return ERR_OK;
}



//
// Internal function that deals with the HTTP Body. - called once for ever packet.
// 1. Copies the current packet to the HttpBuf with an offset of +Http_Recv
// 2. Adds the Length of data just copied to Http_Recv.
// 3. Call to "altcp_recved" informs the TCP layer the packet has been processed. 
// 4. Call to "pbuf_free(p)" frees the pointer of p - this is missing from many examples.
//
err_t recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
  if (HttpRecv+p->tot_len <= HttpBuffSize)  // check if buff has enough space.
  {
    pbuf_copy_partial(p, HttpBuff+HttpRecv, p->tot_len, 0);
  }
  HttpRecv = HttpRecv + p->tot_len;  // add length of this packet to total length so far.
  altcp_recved(conn, p->tot_len);  // magic missing from many examples
  pbuf_free(p);  // magic missing from many examples
  return ERR_OK;
}



//
//  This is the main call Get_HTTP_Data. 
//  call with:
//  1. string of server (this can be domain or IP)
//  2. port number (typically 80) 
//  3. string of file name (full path - example "/www/images/example.gif")
//  4. pointer to the buffer you want the data in. 
//  5. pointer to size of buffer.
//  Returns:
//  int with HTTP result code (I.E 200-OK) - NOTE result of 999 means response exceeds size of buffer.
//  NOTE: Size now holds amounts of data actually received.
//
int Get_HTTP_Data(const char *server, int port, const char *file, char *Buff, long *Size) {
  HttpBuff = Buff;  // copy external pointer to local pointer - this could be cleaner?
  HttpBuffSize=*Size;
  httpc_connection_t settings = {  // setup connection infomation
     .use_proxy = 0,
     .headers_done_fn = headers_done_fn,  // setup call back function
     .result_fn = result_fn               // setup call back function
  };
  HttpRecv=0;  // Ensure we start at the begining of the buffer.
  HttpDone=false;  // Setup bool so we know when we are done.
  err_t err = httpc_get_file_dns(server, port, file, &settings, recv_fn, NULL, NULL);
  while (HttpDone == false){   // Wait for HttpDone to become true. - this could be better?
    sleep_ms(10);
  }
  if (HttpResponseSize > HttpBuffSize) {
    Results=999;  // error buffer too small
  }
  *Size=HttpRecv;  // Update Size to now be the amount of data received.
  return Results;   // Return HTTP result code.
}

