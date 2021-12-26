#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <lorcon2/lorcon.h>
#include <lorcon2/lorcon_packasm.h>


unsigned char packet[115] = {
        0x80, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // dur ffff
        0xff, 0xff, 0x00, 0x0f, 0x66, 0xe3, 0xe4, 0x03,
        0x00, 0x0f, 0x66, 0xe3, 0xe4, 0x03, 0x00, 0x00, // 0x0000 - seq no.
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // BSS timestamp
        0x64, 0x00, 0x11, 0x00, 0x00, 0x0f, 0x73, 0x6f,
        0x6d, 0x65, 0x74, 0x68, 0x69, 0x6e, 0x67, 0x63,
        0x6c, 0x65, 0x76, 0x65, 0x72, 0x01, 0x08, 0x82,
        0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 0x03,
        0x01, 0x01, 0x05, 0x04, 0x00, 0x01, 0x00, 0x00,
        0x2a, 0x01, 0x05, 0x2f, 0x01, 0x05, 0x32, 0x04,
        0x0c, 0x12, 0x18, 0x60, 0xdd, 0x05, 0x00, 0x10,
        0x18, 0x01, 0x01, 0xdd, 0x16, 0x00, 0x50, 0xf2,
        0x01, 0x01, 0x00, 0x00, 0x50, 0xf2, 0x02, 0x01,
        0x00, 0x00, 0x50, 0xf2, 0x02, 0x01, 0x00, 0x00,
        0x50, 0xf2, 0x02};



int main(){
 
 char *interface = "wlan0";
 char *ssid = "TJ_SSID_LOL";
 uint8_t channel = 11;
 lorcon_driver_t *drvlist, *driver;
 lcpa_metapack_t *metapack;
 lorcon_packet_t *txpack;
 lorcon_t *context;

 
 uint8_t mac[6] = { 0x10,0x01,0x13,0x37,0xbe,0xef};
 struct timeval time;
 uint64_t timestamp;
 uint8_t rates[] = "\x8c\x12\x24\xb0\x48\x60\x6c";
 int interval = 100;
 int capabilities = 0x0421;
 int count;
 
 write(1,packet,sizeof(packet));

 if( (driver = lorcon_auto_driver(interface)) == NULL){
   printf("Error finding driver for interface\n");
   return -1;
 }

 if((context  = lorcon_create(interface,driver)) == NULL){
   printf("failed to create context\n");
   return -1;
 }

 if(lorcon_open_injmon(context) <0 ){
   printf("could not create monitor mode on interface!\n");
   return -1;
 } else {
   printf("Monitor mode VAP: %s\n", lorcon_get_vap(context));
   lorcon_free_driver_list(driver);
 }


 lorcon_set_channel(context, channel);
 printf("using channel %d\n\n", channel);



 while(1){

  if( lorcon_send_bytes(context, sizeof(packet), packet ) < 0 ){
   printf("send_bytes failed!\n");
   return 1;
  }


  /*
  gettimeofday(&time, NULL);
  timestamp = time.tv_sec * 1000000 + time.tv_usec;
  metapack = lcpa_init();
  lcpf_probereq(metapack, mac, mac, mac, 0x00, 0x00,0x00,0x00);
  */
/*
  lcpf_beacon(metapack, mac,mac, 0x00,0x00,0x00,0x00,timestamp,interval,capabilities);
  lcpf_add_ie(metapack, 0, strlen(ssid), ssid);
  lcpf_add_ie(metapack, 1,sizeof(rates) -1, rates);
  lcpf_add_ie(metapack, 3, 1, &channel);
  lcpf_add_ie(metapack, 42, 1, "\x05");
  lcpf_add_ie(metapack, 47, 1, "\x05");
*/
  
  /*
  txpack = (lorcon_packet_t *) lorcon_packet_from_lcpa(context, metapack);
  if(lorcon_inject(context, txpack) < 0){
   printf("problem injecting packet\n");
   return -1;
  }
  */

  usleep(interval * 1000);

  printf("\033[K\r");
  printf("%d frames sent", count);
  fflush(stdout);
  count++;
//  lcpa_free(metapack);
 }

 lorcon_close(context);
 lorcon_free(context);

 return 0;
}





 

