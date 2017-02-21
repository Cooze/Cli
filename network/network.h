#include "hashmap.h"
/**
 *ifname keys.
 * */
#define DEVICE			"DEVICE"		
#define HWADDR			"HWADDR"		
#define TYPE			"TYPE"			/* Ethernet */
#define UUID			"UUID"
#define ONBOOT			"ONBOOT"		/* yes | no  */
#define NM_CONTROLLED	"NM_CONTROLLED"	/* yes | no */
#define BOOTPROTO		"BOOTPROTO"		/* dhcp|static etc. */
#define IPADDR			"IPADDR"
#define NETMASK			"NETMASK"
#define GATEWAY			"GATEWAY"

#define BOOTPROTO_TYPE_DHCP		"dhcp"

#define IF_NAME "eth0"

typedef struct ifcfg_t{
    char key[100];
    char value[200];
}ifcfg;

/**
 *
 *set ip for eth0
 */
void read_ifcfg(char* filename,hmap_t map);
void write_ifcfg(char* filename,hmap_t map,const char* param_str);
void splite(char* str,char* split,char* K,char* V);
int free_ifcfg(void* data,void* arg);
void set_network(const char* param_str);
int set_param(hmap_t map,const char* param_str);
void reset_value(hmap_t map,char* key,char* value);
void remove_value(hmap_t map,char* key);
char* set_onboot(char* flag);
void write_line(FILE* file,hmap_t map,char* key,char* value);
void restart_network();
int set_ip(const char* ip_addr, const char* mask,const char* gateway);
void get_network(char* ip_addr,char* netmask,char* gateway);
void show_ip();
/**
 *get current ip. 
 */
char* get_host_name();

/**
 *ping cmd
 */
void ping(const char* ip_addr);


