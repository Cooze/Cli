#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <net/if.h>

#include "cmd.h"
#include "sds.h"
#include "network.h"
#include "gateway.h"
#include "system.h"
/**
 *equal return 1 else 0.
 */
int juge_cmd(const char* cmd,const char* param_cmd){
	return strcmp(cmd,param_cmd)==0;	
}

void show_usage(const char* args){
	
	printf("testes...........");	
}

void switch_by_cmd(const char* line){
	char buf[512];
	sprintf(buf,"%s",line);
    sds cmd_line = sdsnew(line);
    int count=0,is_set,is_ip;
    sds* params = sdssplitargs(cmd_line,&count);
	switch(count){
		case 1:
			if(juge_cmd(SHUTDOWN,params[0])){
				shutdown_now();
				break;
			}
			if( juge_cmd(EXIT,params[0]) )exit(0);
			break;
		case 2:
			if( juge_cmd(PING,params[0]) ){
				ping(params[1]);
				break;
			}
			if( juge_cmd(SHOW,params[0])  && juge_cmd(IP,params[1]) ){
				show_ip();
				break;		
			}
			break;
		case 5:
			is_set = juge_cmd("set",params[0]);
			is_ip  = juge_cmd("ip",params[1]);
			is_ip &= is_set;
			if( is_ip )set_network(buf);
			break;
		default:
			break;	
	}
	sdsfreesplitres(params , count);
}

/*
int main(){
	switch_by_cmd("ping 192.168.0.1");
	return 0;
}
*/
