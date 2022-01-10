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

#include "/home/xuke/bin/dbg.h"
#include "unp.h"
#include "1_intro_TCP_IP.h"
#include "2_basic_socket.h"

int my_daytime_tcp_cli(const char *ip_addr, int port)
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
 *  Function Name : main test client
 ****************************************************************************/
int main(int argc, const char *argv[])
{
	/*my_debug_msg("my_daytime_tcp_cli");*/
	/*my_daytime_tcp_cli("127.0.0.1", SERV_PORT);*/

	/*my_debug_msg("tcp_cli01");*/
	/*tcp_cli01("127.0.0.1");*/

	/*my_debug_msg("tcp_cli04");*/
	/*tcp_cli04("127.0.0.1");*/

	/*my_debug_msg("udp_cli01");*/
	/*udp_cli01("127.0.0.1");*/

	my_debug_msg("get 9999 from localhost");
	daytimetcpcli("localhost", "9999");

    return 0;
}

