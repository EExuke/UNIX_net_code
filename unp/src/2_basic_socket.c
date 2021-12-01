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
#include "sum.h"

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
 * Description   : serv show client ip and port
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

/***************************************************************************************
 * Description   : tcpserv01
 ***************************************************************************************/
int tcp_serv01()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in servaddr = {0};
	struct sockaddr_in cliaddr = {0};

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	while (1) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);
		if ((childpid = Fork()) == 0) { /* child process */
			Close(listenfd);  //close listen socket
			str_echo(connfd); //process the request
			exit(0);
		}
		Close(connfd);  //parent close connected socket
	}
}

/***************************************************************************************
 * Description   : tcpcli01
 ***************************************************************************************/
int tcp_cli01(char *ip_addr)
{
	int sockfd;
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL || ip_addr[0] == '\0') {
		err_quit("input ip_addr is error");
	}

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htonl(SERV_PORT);
	Inet_pton(AF_INET, ip_addr, &servaddr.sin_addr);

	Connect(sockfd, (SA*)&servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd); //do it all

	return 0;
}

/***************************************************************************************
 * Description   : sigchldwait
 ***************************************************************************************/
void sig_chld_wait(int signo)
{
	pid_t pid;
	int stat;

	pid = wait(&stat);
	printf("child %d terminated\n", pid);

	return;
}

/***************************************************************************************
 * Description   : tcpcli04
 ***************************************************************************************/
int tcp_cli04(char *ip_addr)
{
	int i, sockfd[5];
	struct sockaddr_in servaddr = {0};

	if (ip_addr == NULL || ip_addr[0] == '\0') {
		err_quit("input ip_addr is error");
	}

	for (i=0; i<5; i++) {
		sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);

		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htonl(SERV_PORT);
		Inet_pton(AF_INET, ip_addr, &servaddr.sin_addr);

		Connect(sockfd[i], (SA*)&servaddr, sizeof(servaddr));
	}

	str_cli(stdin, sockfd[0]); //do it all

	return 0;
}

/***************************************************************************************
 * Description   : sigchldwaitpid
 ***************************************************************************************/
void sig_chld_wait_pid(int signo)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		printf("child %d terminated\n", pid);
	}

	return;
}

/***************************************************************************************
 * Description   : tcpserv04
 ***************************************************************************************/
int tcp_serv04()
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in servaddr = {0};
	struct sockaddr_in cliaddr = {0};
	void sig_chld_wait_pid(int);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
	Signal(SIGCHLD, sig_chld_wait_pid);  //must call waitpid()

	while (1) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (SA*)&cliaddr, &clilen)) < 0) {
			if (errno == EINTR) {
				continue;
			} else {
				err_sys("accept error");
			}
		}
		if ((childpid = Fork()) == 0) { /* child process */
			Close(listenfd);  //close listen socket
			str_echo(connfd); //process the request
			exit(0);
		}
		Close(connfd);  //parent close connected socket
	}
}

/***************************************************************************************
 * Description   : str_cli11
 ***************************************************************************************/
void str_cli11(FILE *fp, int sockfd)
{
	char sendline[MAXLINE] = {0};
	char recvline[MAXLINE] = {0};

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, 1);
		sleep(1);
		Writen(sockfd, sendline+1, strlen(sendline)-1);

		if (Readline(sockfd, recvline, MAXLINE) == 0) {
			err_quit("str_cli: server terminated prematurely");
		}

		Fputs(recvline, stdout);
	}
}

/***************************************************************************************
 * Description   : str_echo08
 ***************************************************************************************/
void str_echo08(int sockfd)
{
	long arg1, arg2;
	ssize_t n;
	char line[MAXLINE] = {0};

	while (1) {
		if ((n = Readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

		if (sscanf(line, "%ld%ld", &arg1, &arg2) == 2) {
			snprintf(line, sizeof(line), "%ld\n", arg1 + arg2);
		} else {
			snprintf(line, sizeof(line), "input error\n");
		}

		n = strlen(line);
		Writen(sockfd, line, n);
	}
}

/***************************************************************************************
 * Description   : str_cli09
 ***************************************************************************************/
void str_cli09(FILE *fp, int sockfd)
{
	char sendline[MAXLINE];
	struct args args;
	struct result result;

	while (Fgets(sendline, MAXLINE, fp) != NULL) {
		if (sscanf(sendline, "%ld%ld", &args.arg1, &args.arg2) != 2) {
			printf("invalid input: %s", sendline);
			continue;
		}
		Writen(sockfd, &args, sizeof(args));

		if (Readn(sockfd, &result, sizeof(result)) == 0) {
			err_quit("str_cli: server terminated prematurely");
		}
		printf("%ld\n", result.sum);
	}
}

/***************************************************************************************
 * Description   : str_echo09
 ***************************************************************************************/
void str_echo09(int sockfd)
{
	ssize_t n;
	struct args args;
	struct result result;

	while (1) {
		if ( (n = Readn(sockfd, &args, sizeof(args))) == 0) {
			return;		/* connection closed by other end */
		}

		result.sum = args.arg1 + args.arg2;
		Writen(sockfd, &result, sizeof(result));
	}
}

