#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/ioctl.h>
#include <netinet/in.h>  
#include <net/if.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <errno.h>

#include "sds.h"
#include "network.h"
#include "hashmap.h"
#include "gateway.h"
/**
 *get current network ip address
 */
char* get_host_name(){  
    int sock;  
    struct sockaddr_in sin;  
    struct ifreq ifr;  
    sock = socket(AF_INET, SOCK_DGRAM, 0);  
    if (sock == -1)  
    {   
        perror("socket");  
        return NULL;         
    }   
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);  
    ifr.ifr_name[IFNAMSIZ - 1] = 0;  
                  
    if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)  
    {   
        perror("ioctl");  
        return NULL;  
    }   
    char* host_ip;
    host_ip=NULL;
    host_ip = malloc( sizeof( char)*15 );  
    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));  
    sprintf(host_ip,inet_ntoa(sin.sin_addr));
    return host_ip;  
}

void get_network(char* ip_addr,char* netmask,char* gateway){

    struct sockaddr_in *addr;
    struct ifreq ifr;
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
    if(ioctl(sockfd,SIOCGIFADDR,&ifr) == -1)
        return ;
    addr = (struct sockaddr_in *)&(ifr.ifr_addr);
    sprintf(ip_addr,"%s",inet_ntoa(addr->sin_addr));

    if(ioctl(sockfd,SIOCGIFNETMASK,&ifr) == -1)
        return ;
    addr = (struct sockaddr_in *)&ifr.ifr_netmask;
    sprintf(netmask,"%s",inet_ntoa(addr->sin_addr));

	char gw[20], ifName[12];
 	get_gateway(gw, ifName);
	sprintf(gateway,"%s",gw);
	close(sockfd);
}


int set_ip(const char* ip_addr, const char* mask,const char* gateway){
	
	int fd;
	struct sockaddr_in *sin;
	struct ifreq ifr;
	struct rtentry rt;

	fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd<0)return -1;

	memset(&ifr,0,sizeof(ifr));
	strcpy(ifr.ifr_name,IF_NAME);
	sin = (struct sockaddr_in*)&ifr.ifr_addr;
	sin->sin_family = AF_INET;

	if(inet_aton(ip_addr,&(sin->sin_addr))<0){
		close(fd);
		return -1;
	}
	if(ioctl(fd,SIOCSIFADDR,&ifr) < 0){
		close(fd);
		return -1;
	}

	if(inet_aton(mask,&(sin->sin_addr))<0){
		close(fd);
		return -1;
	}
	if(ioctl(fd,SIOCSIFNETMASK,&ifr) < 0){
		close(fd);
		return -1;
	}
	
	memset(&rt,0,sizeof(struct rtentry));
	memset(sin,0,sizeof(struct sockaddr_in));
	sin->sin_family = AF_INET;
	sin->sin_port	= 0;
	if(inet_aton(gateway,&(sin->sin_addr))<0){
		close(fd);
		return -1;
	}
	
	memcpy(&rt.rt_gateway,sin,sizeof(struct sockaddr_in));
	((struct sockaddr_in*)&rt.rt_dst)->sin_family = AF_INET;
	((struct sockaddr_in*)&rt.rt_genmask)->sin_family=AF_INET;
	rt.rt_flags = RTF_GATEWAY;
	if(ioctl(fd,SIOCADDRT,&rt)<0){
		printf("ip has comflict.");	
		return -1;
	}
	ifr.ifr_flags |=IFF_UP|IFF_RUNNING;
	if(ioctl(fd,SIOCSIFFLAGS,&ifr)<0){
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


/**
 *read config message from ifcfg-eht0
 **/
void read_ifcfg(char* filename, hmap_t map ){
	if( !filename ) return;
	FILE * file = fopen( filename, "r" );
	if( !file ) return;
	char buf[300];
	char k[100];
	char v[200];
	int len;
	while( fgets( buf, sizeof(buf), file ) != NULL ){
		if( buf[0] == '\0' || buf[0] == '#' ) continue;
		ifcfg* icfg = (ifcfg*)malloc(sizeof(ifcfg));
		splite(buf,"=",k,v);
		len = strlen(v);
		if(v[len-1]=='\n')v[len-1]='\0';
		sprintf(icfg->key,"%s",k);
		sprintf(icfg->value,"%s",v);
		hashmap_put(map,icfg->key,icfg);
	}
}

/**
 *write config message to ifcfg-eth0
 **/
void write_ifcfg(char* filename,hmap_t map,const char* param_str){

	if(!filename||!map){
		printf("set ip error.\n");
		return;
	}

	FILE* file = fopen(filename,"w+");
	if(!file){
		printf("set ip error.\n");
		return;
	}
	
	sds cmd_line = sdsnew(param_str);
	int count=0;
	sds* params = sdssplitargs(cmd_line,&count);
	
	write_line(file,map,DEVICE,NULL);
	write_line(file,map,HWADDR,NULL);
	write_line(file,map,TYPE,NULL);
	write_line(file,map,UUID,NULL);
	char* flag = set_onboot("on");
	write_line(file,map,ONBOOT,flag);
	write_line(file,map,NM_CONTROLLED,NULL);
	write_line(file,map,BOOTPROTO,"static");
	write_line(file,map,IPADDR,params[2]);
	write_line(file,map,NETMASK,params[3]);
	write_line(file,map,GATEWAY,params[4]);
	fflush(file);
	fclose(file);
	sdsfreesplitres( params , count );	
}
void write_line(FILE* file,hmap_t map,char* key,char* value){
	
	ifcfg* icfg = (ifcfg*)malloc(sizeof(ifcfg));
    memset(icfg->key,0,sizeof(icfg->key));
    memset(icfg->value,0,sizeof(icfg->value));
	hashmap_get(map,key,(void*) &icfg);
	if(!icfg && !value)return;
	char buf[300];
	if( icfg && icfg->value ){
		sprintf(buf,"%s=%s\n",key,icfg->value);
		fputs(buf,file);
	}else if( value ) {
		sprintf(buf,"%s=%s\n",key,value);
		fputs(buf,file);
	}
}


void splite(char* str,char* split,char* K,char* V){
	sds * tokens;
	int count;
	sds config_line = sdsnew(str);
	tokens = sdssplitlen(config_line,sdslen(config_line),split,1,&count);
	strcpy(K,tokens[0]);
	if( count >= 1 )strcpy(V,tokens[1]);
	sdsfreesplitres( tokens , count );	
}

/**
 * free mem.
 **/
int free_ifcfg(void* data,void* arg){
	ifcfg * icfg = (ifcfg*)data;
	free(icfg);
	return 0;
}


int set_param(hmap_t map,const char* param_str){
	sds cmd_line = sdsnew(param_str);
	int count=0;
	sds* params = sdssplitargs(cmd_line,&count);
	reset_value(map,BOOTPROTO,"static");
	char* flag = set_onboot("on");
	reset_value(map,ONBOOT,flag);
	reset_value(map,IPADDR,params[2]);
	reset_value(map,NETMASK,params[3]);
	reset_value(map,GATEWAY,params[4]);
	int i =set_ip(params[2],params[3],params[4]);
	sdsfreesplitres( params , count );	
	return i;
}

void reset_value(hmap_t map,char* key,char* value){
		ifcfg* icfg = (ifcfg*)malloc(sizeof(ifcfg));
        memset(icfg->key,0,sizeof(icfg->key));
        memset(icfg->value,0,sizeof(icfg->value));
        hashmap_get(map,key,(void*)&icfg);
		if(!icfg && !value)return;
		if(icfg && value){
            memset(icfg->value,0,sizeof(icfg->value));
            sprintf(icfg->value,"%s",value);
			return;
		}
}

void remove_value(hmap_t map,char* key){
		ifcfg* icfg = (ifcfg*)malloc(sizeof(ifcfg));
		memset(icfg->key,0,sizeof(icfg->key));
        memset(icfg->value,0,sizeof(icfg->value));
        hashmap_get(map,key,(void*)&icfg);
		if(!icfg)return;
        hashmap_remove(map,key,(void*)&icfg);
		free(icfg);
		icfg=NULL;
}

char* set_onboot(char* flag){
	if(strcmp("on",flag)==0){
		char* ret="yes";
		return ret;
	}
	char* ret="no";
	return ret;
}

void restart_network(){
	int status=-1;
	status = system("service network restart");
	if(status==-1){
		printf("set ip failure.\n");
	}else{
		printf("set ip success.\n");
	}
}

void set_network(const char* param_str){
	char* filename="/etc/sysconfig/network-scripts/ifcfg-eth0";
	hmap_t map = hashmap_create();
	read_ifcfg(filename,map);
	int i = set_param(map,param_str);
	if(i<0){
		return;
	}
	write_ifcfg(filename,map,param_str);
	hashmap_destroy(map,free_ifcfg,0);
	map=NULL;
}

void ping(const char* ip_addr){
	char* cmd = (char*)malloc(sizeof(char)*20);
	sprintf(cmd,"ping %s",ip_addr); 
	system(cmd);
}

void show_ip(){
	char ip[20];
	char mask[20];
	char gw[20];
	get_network(ip,mask,gw);
	printf(" ip\t\t\tnetmask\t\t\tgateway\n");
	if(strlen(ip)>7&&strlen(mask)>7&&strlen(gw))
		printf(" %s\t\t%s\t\t%s\n",ip,mask,gw);
}
/*
int main(void){
	set_network("set ip static 192.168.0.10 255.255.0.0 192.168.0.1 on");
	return 0;
}
*/
