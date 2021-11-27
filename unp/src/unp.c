/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : unp.c
 *     FIRST CREATION DATE    : 2021/11/28
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/28
 *     FILE DESCRIPTION       :
** ************************************************************************** */

#include "/home/xuke/bin/dbg.h"
#include "unp.h"

/***************************************************************************************
 * Description   : sudo vim /etc/xinetd.d/daytime 开启服务
 *               ：sudo /etc/init.d/xinetd restart 重启服务
 ***************************************************************************************/
int main(int argc, const char *argv[])
{
	struct sockaddr_in serv = {0};
	char buff[BUFFSIZE] = {0};
	int sockfd, n;

	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		my_debug_msg("socket error.");
	}
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv.sin_port = htons(13);

	if (sendto(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&serv, sizeof(serv)) != BUFFSIZE) {
		my_debug_msg("sendto error.");
	}
	if ((n = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)NULL, (socklen_t*)NULL)) < 2) {
		my_debug_msg("recvfrom error.");
	}
	buff[n - 2] = 0;    //null terminate
	my_debug_msg("%s", buff);

	return 0;
}

