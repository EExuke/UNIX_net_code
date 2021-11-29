/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : 1_intro_TCP_IP.c
 *     FIRST CREATION DATE    : 2021/11/29
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/29
 *     FILE DESCRIPTION       : functions in part 1 intro and TCP/IP
** ************************************************************************** */

#include "unp.h"

// functions
/***************************************************************************************
 * Description   :
 ***************************************************************************************/
int daytime_tcp_cli(const char *ip_addr)
{
	int sockfd, n;
	char recvline[MAXLINE+1] = {0};
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL) {
		err_ret("input ip_addr is null");
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		err_sys("socket error");
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13);  //daytime server port
	if (inet_pton(AF_INET, ip_addr, &servaddr.sin_addr) <= 0) {
		err_quit("inet_pton error for %s", ip_addr);
	}

	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
		err_sys("connect error");
	}

	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;  //null terminate
		if (fputs(recvline, stdout) == EOF) {
			err_sys("fputs error");
		}
	}
	if (n < 0) {
		err_sys("read error");
	}

	return 0;
}

/***************************************************************************************
 * Description   :
 ***************************************************************************************/
int daytime_tcp_cli6(const char *ip6_addr)
{
	int sockfd, n;
	char recvline[MAXLINE+1] = {0};
	struct sockaddr_in6 servaddr = {0};

	if (ip6_addr == NULL) {
		err_ret("input ip6_addr is null");
	}

	if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
		err_sys("socket error");
	}

	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_port = htons(13);  //daytime server port
	if (inet_pton(AF_INET6, ip6_addr, &servaddr.sin6_addr) <= 0) {
		err_quit("inet_pton error for %s", ip6_addr);
	}

	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
		err_sys("connect error");
	}

	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;  //null terminate
		if (fputs(recvline, stdout) == EOF) {
			err_sys("fputs error");
		}
	}
	if (n < 0) {
		err_sys("read error");
	}

	return 0;
}

