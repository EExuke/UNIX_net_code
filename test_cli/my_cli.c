/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : my_cli.c
 *     FIRST CREATION DATE    : 2021/12/01
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/12/01
 *     FILE DESCRIPTION       :
** ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <strings.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define MAXLINE    4096

int daytime_tcp_cli(const char *ip_addr, int port)
{
	int sockfd, n;
	char recvline[MAXLINE+1] = {0};
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL) {
		perror("input ip_addr is null");
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
	}

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);  //daytime server port
	if (inet_pton(AF_INET, ip_addr, &servaddr.sin_addr) <= 0) {
		perror("inet_pton error for ip_addr");
	}

	if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		perror("connect error");
	}

	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;  //null terminate
		if (fputs(recvline, stdout) == EOF) {
			perror("fputs error");
		}
	}
	if (n < 0) {
		perror("read error");
	}

	return 0;
}

/****************************************************************************
 *  Function Name : main
 *  Description   : The Main Function.
 *  Input(s)      : argc - The numbers of input value.
 *                : argv - The pointer to input specific parameters.
 *  Output(s)     : NULL
 *  Returns       : 0
 ****************************************************************************/
int main(int argc, const char *argv[])
{
	daytime_tcp_cli("127.0.0.1", 9970);
    return 0;
}

