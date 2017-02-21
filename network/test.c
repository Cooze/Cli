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

#define IF_NAME "eth0"
//success return 0 else 1
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
	// set ip address
	if(inet_aton(ip_addr,&(sin->sin_addr))<0){
		close(fd);
		return -1;
	}
	if(ioctl(fd,SIOCSIFADDR,&ifr) < 0){
		close(fd);
		return -1;
	}
	//set netmask
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

void get_network(char* ip_addr,char* netmask,char* gateway){

    struct sockaddr_in *addr;
    struct ifreq ifr;
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
    if(ioctl(sockfd,SIOCGIFADDR,&ifr) == -1)
        return;
    addr = (struct sockaddr_in *)&(ifr.ifr_addr);
    sprintf(ip_addr,"%s",inet_ntoa(addr->sin_addr));

    if(ioctl(sockfd,SIOCGIFNETMASK,&ifr) == -1)
        return ;
    addr = (struct sockaddr_in *)&ifr.ifr_netmask;
    sprintf(netmask,"%s",inet_ntoa(addr->sin_addr));

	close(sockfd);
}

int main(void){
//	int i;

//	i = set_ip("192.168.0.100","255.255.255.0","192.168.0.1");
//	printf("%d\n",i);
	char ip[20];
	char mask[20];
	char gw[20];
	get_network(ip,mask,gw);
	printf("%s\n",ip);
	printf("%s\n",mask);
	printf("%s\n",gw);
	return 0;
}


