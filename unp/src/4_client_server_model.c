/**************************************************************************** **
 * Copyright (C) 2001-2020 Inhand Networks, Inc.
 **************************************************************************** **/

/* ************************************************************************** **
 *     MODULE NAME            : system
 *     LANGUAGE               : C
 *     TARGET ENVIRONMENT     : Any
 *     FILE NAME              : 4_client_server_model.c
 *     FIRST CREATION DATE    : 2021/11/29
 * --------------------------------------------------------------------------
 *     Version                : 1.0
 *     Author                 : EExuke
 *     Last Change            : 2021/11/29
 *     FILE DESCRIPTION       : functions of part 4 client/server design model
** ************************************************************************** */

#include "unp.h"
#include "4_client_server_model.h"

/**************************************************************************** **
 * tcp测试用客户程序
 **************************************************************************** **/
int test_client(const char *hostname, const char *service, int nchildren, int nloops, int nbytes)
{
	int i, j, fd;
	pid_t pid;
	ssize_t n;
	char request[MAXLINE], reply[MAXN];

	snprintf(request, sizeof(request), "%d\n", nbytes); /* newline at end */

	for (i = 0; i < nchildren; i++) {
		if ( (pid = Fork()) == 0) {		/* child */
			for (j = 0; j < nloops; j++) {
				fd = Tcp_connect(hostname, service);

				Write(fd, request, strlen(request));

				if ( (n = Readn(fd, reply, nbytes)) != nbytes) {
					err_quit("server returned %d bytes", n);
				}

				Close(fd);		/* TIME_WAIT on client, not server */
			}
			printf("child %d done\n", i);
			return 0;
		}
		/* parent loops around to fork() again */
	}

	while (wait(NULL) > 0)	/* now parent waits for all children */
		;
	if (errno != ECHILD) {
		err_sys("wait error");
	}

	return 0;
}

/**************************************************************************** **
 * tcp并发服务器程序, 每个客户一个子进程
 **************************************************************************** **/
void pr_cpu_time(void)
{
	double user, sys;
	struct rusage myusage, childusage;

	if (getrusage(RUSAGE_SELF, &myusage) < 0) {
		err_sys("getrusage error");
	}
	if (getrusage(RUSAGE_CHILDREN, &childusage) < 0) {
		err_sys("getrusage error");
	}

	user = (double) myusage.ru_utime.tv_sec + myusage.ru_utime.tv_usec/1000000.0;
	user += (double) childusage.ru_utime.tv_sec + childusage.ru_utime.tv_usec/1000000.0;
	sys = (double) myusage.ru_stime.tv_sec + myusage.ru_stime.tv_usec/1000000.0;
	sys += (double) childusage.ru_stime.tv_sec + childusage.ru_stime.tv_usec/1000000.0;

	printf("\nuser time = %g, sys time = %g\n", user, sys);
}

void sig_int(int signo)
{
	void pr_cpu_time(void);

	pr_cpu_time();
	exit(0);
}

void web_child(int sockfd)
{
	int ntowrite;
	ssize_t nread;
	char line[MAXLINE], result[MAXN];

	for ( ; ; ) {
		if ( (nread = Readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

		/* 4line from client specifies #bytes to write back */
		ntowrite = atol(line);
		if ((ntowrite <= 0) || (ntowrite > MAXN)) {
			err_quit("client request for %d bytes", ntowrite);
		}

		Writen(sockfd, result, ntowrite);
	}
}

int model_serv01(char *hostname, char *service)
{
	int listenfd, connfd;
	pid_t childpid;
	void sig_chld(int), sig_int(int), web_child(int);
	socklen_t clilen, addrlen;
	struct sockaddr *cliaddr;


	listenfd = Tcp_listen(hostname, service, &addrlen);

	cliaddr = Malloc(addrlen);

	/*Signal(SIGCHLD, sig_chld);*/
	Signal(SIGINT, sig_int);

	for ( ; ; ) {
		clilen = addrlen;
		if ( (connfd = accept(listenfd, cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				err_sys("accept error");
		}

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			printf("client init\n");
			web_child(connfd);	/* process request */
			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
	}
}

/**************************************************************************** **
 * tcp预先派生子进程服务程序，accept无上锁保护
 **************************************************************************** **/

/**************************************************************************** **
 * tcp预先派生子进程服务程序，accept使用文件上锁保护
 **************************************************************************** **/

/**************************************************************************** **
 * tcp预先派生子进程服务程序，accept使用线程上锁保护
 **************************************************************************** **/

/**************************************************************************** **
 * tcp预先派生子进程服务程序，传递描述符
 **************************************************************************** **/

/**************************************************************************** **
 * tcp并发服务器程序，每个客户一个线程
 **************************************************************************** **/

/**************************************************************************** **
 * tcp预先创建线程服务器程序，每个线程各自accept
 **************************************************************************** **/

/**************************************************************************** **
 * tcp预先创建线程服务器程序，主线程统一accept
 **************************************************************************** **/


