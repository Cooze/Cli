#include <stdio.h>
#include "sds.h"

int main( void ){
	sds * tokens;
	int count,j;
	/*
	sds line = sdsnew("   Hello    World!");
	
	tokens = sdssplitlen(line,sdslen(line)," ",1,&count);
	for(j=0;j<count;j++)
		printf("%s\n",tokens[j]);
	sdsfreesplitres(tokens,count);
	*/
	sds line1 = sdsnew("DEVICE=eth0 #fdasfafas");
	tokens = sdssplitlen(line1,sdslen(line1)," ",1,&count);
	for(j=0;j<count;j++)
		printf("%s\n",tokens[j]);
	sdsfreesplitres(tokens,count);
	count=0;
	sds line2 = sdsnew("   Hello    World!      -oop ewqq  \"test haha jiji\" -c da   ");
	tokens = sdssplitargs(line2,&count);
	for(j=0;j<count;j++)
		printf("%s\n",tokens[j]);
	sdsfreesplitres(tokens,count);
return 0;
}
