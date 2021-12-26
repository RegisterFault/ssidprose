/* Author: Travis James <tj@77.org>
 * Program: poem 
 * Purpose: Convert a text file into a poem that gets broadcasted as a series of SSIDs in beacons
 *
 *
 */

//XXX libraries. readline seems useful for this
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <sys/time.h>
#include <string.h>
#include <lorcon2/lorcon.h>
#include <lorcon2/lorcon_packasm.h>

#define STRINGS_SIZE 256
#define INTERVAL 100
#define FILESIZE 256
#define LINESIZE 29


char interface[256] = { 0 };
int channel = 11; //not that lorcon will even listen to this
lorcon_driver_t *drvlist, *driver;
lcpa_metapack_t *metapack;
lorcon_packet_t *txpack;
lorcon_t *context;
uint8_t mac[6] = {0x10, 0x01, 0x13, 0x37, 0x69, 0x00 };
uint8_t rates[] = "\x8c\x12\x24\xb0\x48\x60\x6c";
int capabilities = 0x0421;
int interval = INTERVAL;
char filename[FILESIZE] = {0};
char *strings[STRINGS_SIZE] = { NULL};

int parse_file(){
  char linebuf[LINESIZE];
  char prefix[10];
  int line_num =0;
  size_t verse_size;
  FILE * fptr;
  char *c;
  int i;
  if( (fptr = fopen(filename,"r")) == NULL){
    perror("File open failure");
    exit(1);
  }

  while( fgets(linebuf,LINESIZE, fptr) != NULL){

    //trim newline, if it exists.
    c = strchr(linebuf,'\n');
    if(c) *c = '\0';

    snprintf(prefix, 10, "1%d ", line_num);

    verse_size = strlen(prefix) + strlen(linebuf) + 1;
    strings[line_num] = malloc(verse_size);

    strncpy(strings[line_num],prefix,verse_size);
    strncat(strings[line_num],linebuf,verse_size);

    line_num++;
  }

  for(i = 0;i<line_num;i++)
	  printf("%s\n",strings[i]);

  return 0;
}



int broadcast_ssid(char * ssid){
	struct timeval time;
	uint64_t timestamp;
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

	mac[5] += 1;

}



int do_poem(){

	int i;
	if(parse_file()){
		fprintf(stderr,"File Parsing issue\n");
		return -1;
	}
	printf("Opening Wireless interface to inject poem\n");
	if( (driver = lorcon_auto_driver(interface)) == NULL){
		printf("failed to find driver for %s\n");
		return -1;
	}
	if( (context = lorcon_create(interface,driver)) == NULL){
		printf("failed to create lorcon context\n");
		return -1;
	}
	if(lorcon_open_injmon(context) < 0){
		printf("Could not create monitor mode on interface %s\n",interface);
		return -1;
	}

	printf("Monitor mode activated\n");
	lorcon_free_driver_list(driver);
	lorcon_set_channel(context,channel);
	//loop through strings
	while(1){
		mac[5] = 0;
		for(i = 0; strings[i] != NULL; i++){
			broadcast_ssid(strings[i]);
			usleep(interval*1000);
		}

	}

}



//Syntax: -i device filename
int main(int argc, char * argv[]){
	int i;
	int index;
	int c;

	//I am paranoid
	for(i=0;i<STRINGS_SIZE;i++){
		strings[i] = NULL;
	}

	//parse getopt
	while((c =getopt (argc, argv, "i:"))!=-1){
		switch(c){
		case 'i':
			strncpy(interface,optarg,256);
			break;
		case '?':
			if( optopt == 'i')
				fprintf(stderr,"option -i needs a network interface argument\n");
			else if (isprint(optopt))
				fprintf(stderr,"Unknown option: %c \n",optopt);
			else
				fprintf(stderr, "No Idea what the hell you put down, man\n");
			return -1;
		default:
			abort();
		}
	}


	//the next argument is the filename
	if(optind <argc)
		strncpy(filename, argv[optind],FILESIZE);
	else {
		fprintf(stderr,"God dammit, give me a file!\n");
		return 1;
	}


	return do_poem();


}
