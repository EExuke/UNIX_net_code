/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : 2_basic_socket.c
 *     FIRST CREATION DATE    : 2021/11/29
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/29
 *     FILE DESCRIPTION       : functions in part 2 basic socket coding
** ************************************************************************** */

#include "unp.h"

// functions
/***************************************************************************************
 * Description   : get host byte order
 ***************************************************************************************/
int byte_order()
{
	union {
		short s;
		char c[sizeof(short)];
	} un;  //2byte

	un.s = 0x0102;
	printf("%s: ", CPU_VENDOR_OS);
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2) {
			printf("big-endian\n");
		} else if (un.c[0] == 2 && un.c[1] == 1) {
			printf("little-endian\n");
		} else {
			printf("unknown\n");
		}
	} else {
		printf("sizeof(short) = %ld\n", sizeof(short));
	}
	return 0;
}

/***************************************************************************************
 * Description   : 
 ***************************************************************************************/
int daytime_tcp_srv1()
{
	int listenfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr = {0};
	struct sockaddr_in cliaddr = {0};
	char buff[MAXLINE];
	time_t ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9970);  //my daytime server port

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	while (1) {
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA*)&cliaddr, &len);
		printf("connection from %s, port %d\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "daytime_tcp_srv: %.24s\r\n", ctime(&ticks));
		Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}

