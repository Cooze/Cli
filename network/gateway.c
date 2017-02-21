#include <arpa/inet.h>
#include <linux/rtnetlink.h> 
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include<unistd.h>
#include<sys/types.h>
#include "gateway.h"
#define BUFSIZE 8192

int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId)
{
 	struct nlmsghdr *nlHdr;
 	int readLen = 0, msgLen = 0;
 	do{

		 if((readLen = recv(sockFd, bufPtr, BUFSIZE - msgLen, 0)) < 0)
 		{
 				perror("SOCK READ:");
 				return -1;
		 }
		 nlHdr = (struct nlmsghdr *)bufPtr;

 		if((NLMSG_OK(nlHdr, readLen) == 0) || (nlHdr->nlmsg_type == NLMSG_ERROR))
		{
			perror("Error in recieved packet");
 			return -1;
		}
 		if(nlHdr->nlmsg_type == NLMSG_DONE)
		{
			break;
		}
		else
		{
    		bufPtr += readLen;
    		msgLen += readLen;
		}
		if((nlHdr->nlmsg_flags & NLM_F_MULTI) == 0)
		{
			break;
		}
 	} while((nlHdr->nlmsg_seq != seqNum) || (nlHdr->nlmsg_pid != pId));
	return msgLen;
}

void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway, char *ifName)
{
   struct rtmsg *rtMsg;
   struct rtattr *rtAttr;
   int rtLen;
   char *tempBuf = NULL;
   struct in_addr dst;
   struct in_addr gate;

   tempBuf = (char *)malloc(100);
   rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

   if((rtMsg->rtm_family != AF_INET) || (rtMsg->rtm_table != RT_TABLE_MAIN))
     return;
   rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
   rtLen = RTM_PAYLOAD(nlHdr);
   for(;RTA_OK(rtAttr,rtLen);rtAttr = RTA_NEXT(rtAttr,rtLen)){
      switch(rtAttr->rta_type) {
         case RTA_OIF:
            if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
            break;
         case RTA_GATEWAY:
            rtInfo->gateWay = *(u_int *)RTA_DATA(rtAttr);
            break;
         case RTA_PREFSRC:
            rtInfo->srcAddr = *(u_int *)RTA_DATA(rtAttr);
            break;
         case RTA_DST:
            rtInfo->dstAddr = *(u_int *)RTA_DATA(rtAttr);
            break;
      }
   }
   dst.s_addr = rtInfo->dstAddr;
   if (strstr((char *)inet_ntoa(dst),"0.0.0.0"))
   {
   sprintf(ifName,"%s", rtInfo->ifName);

   gate.s_addr = rtInfo->gateWay;
   sprintf(gateway,"%s", (char *)inet_ntoa(gate));

   gate.s_addr = rtInfo->srcAddr;
   gate.s_addr = rtInfo->dstAddr;
   }
   free(tempBuf);
   return;
}

int get_gateway(char *gateway, char *ifName)
{
   struct nlmsghdr *nlMsg;
   struct rtmsg *rtMsg;
   struct route_info *rtInfo;
   char msgBuf[BUFSIZE];

   int sock, len, msgSeq = 0;

   if((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0){
      perror("Socket Creation:");
      return -1;
   }
   memset(msgBuf, 0, BUFSIZE);


   nlMsg = (struct nlmsghdr *)msgBuf;
   rtMsg = (struct rtmsg *)NLMSG_DATA(nlMsg);


   nlMsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); 
   nlMsg->nlmsg_type = RTM_GETROUTE; 

   nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; 
   nlMsg->nlmsg_seq = msgSeq++;
   nlMsg->nlmsg_pid = getpid();


   if(send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0){
      printf("Write To Socket Failed…n");
      return -1;
   }

   if((len = readNlSock(sock, msgBuf, msgSeq, getpid())) < 0) {
      printf("Read From Socket Failed…n");
      return -1;
   }
   rtInfo = (struct route_info *)malloc(sizeof(struct route_info));
   for(;NLMSG_OK(nlMsg,len);nlMsg = NLMSG_NEXT(nlMsg,len)){
      memset(rtInfo, 0, sizeof(struct route_info));
      parseRoutes(nlMsg, rtInfo, gateway, ifName);
   }
 	free(rtInfo);
 	close(sock);
 return 0;
}
/*
int main()
{
 char buff[256], ifName[12];
 get_gateway(buff, ifName);
 printf("interface:%s\ngetway: %s\n",ifName, buff);
 return 0;
}*/
