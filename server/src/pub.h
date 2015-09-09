/*
 * pub.h
 *
 *  Created on: 2015年4月18日
 *      Author: train
 */

#ifndef PUB_H_
#define PUB_H_

void setdaemon();
int socket_create(int port);
void catch_Signal(int Sign);
int signal1(int signo, void (*func)(int));

#endif /* PUB_H_ */
