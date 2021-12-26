#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <lorcon2/lorcon.h>
#include <lorcon2/lorcon_packasm.h>



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
  gettimeofday(&time, NULL);
  timestamp = time.tv_sec * 1000000 + time.tv_usec;
  metapack = lcpa_init();
  lcpf_beacon(metapack, mac,mac, 0x00,0x00,0x00,0x00,timestamp,interval,capabilities);
  lcpf_add_ie(metapack, 0, strlen(ssid), ssid);
  lcpf_add_ie(metapack, 1,sizeof(rates) -1, rates);
  lcpf_add_ie(metapack, 3, 1, &channel);
  lcpf_add_ie(metapack, 42, 1, "\x05");
  lcpf_add_ie(metapack, 47, 1, "\x05");
  txpack = (lorcon_packet_t *) lorcon_packet_from_lcpa(context, metapack);
  if(lorcon_inject(context, txpack) < 0){
   printf("problem injecting packet\n");
   return -1;
  }

  usleep(interval * 1000);

  printf("\033[K\r");
  printf("%d frames sent", count);
  fflush(stdout);
  count++;
  lcpa_free(metapack);
 }

 lorcon_close(context);
 lorcon_free(context);

 return 0;
}





 

