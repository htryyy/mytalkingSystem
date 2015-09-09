/*
 * myQQServer.cpp
 *
 *  Created on: 2015年4月18日
 *      Author: train
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <signal.h>
#include "pub.h"
#include "work.h"
int main(int argc,char *argv[])
{
	if(argc<2)
	{
		printf("usage:./myQQServer port\n");
		return -1;
	}
	int iport = atoi(argv[1]);
	if (iport == 0)
	{
		printf("port %d is invalid\n", iport);
		return -1;
	}
	setdaemon();
	work w(iport);
	signal1(SIGINT,catch_Signal);
	signal1(SIGPIPE,catch_Signal);
	printf("server begin\n");
	w.run();
	printf("server end\n");
}


