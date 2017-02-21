#include <linux/rtnetlink.h>
#include <net/if.h>
struct route_info
{
 u_int dstAddr;
 u_int srcAddr;
 u_int gateWay;
 char ifName[IF_NAMESIZE];
};

int readNlSock(int sockFd, char *bufPtr, int seqNum, int pId);
int get_gateway(char *gateway, char *ifName);
void parseRoutes(struct nlmsghdr *nlHdr, struct route_info *rtInfo,char *gateway, char *ifName);
