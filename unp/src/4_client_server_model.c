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
#include "unpthread.h"
#include "4_client_server_model.h"
#include	<sys/mman.h>

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
		if ( (pid = Fork()) == 0) {    /* child */
			for (j = 0; j < nloops; j++) {
				fd = Tcp_connect(hostname, service);

				Write(fd, request, strlen(request));

				if ( (n = Readn(fd, reply, nbytes)) != nbytes) {
					err_quit("server returned %d bytes", n);
				}

				Close(fd);    /* TIME_WAIT on client, not server */
			}
			printf("child %d done\n", i);
			return 0;
		}
		/* parent loops around to fork() again */
	}

	while (wait(NULL) > 0)    /* now parent waits for all children */
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
				continue; /* back to for() */
			else
				err_sys("accept error");
		}

		if ( (childpid = Fork()) == 0) { /* child process */
			Close(listenfd); /* close listening socket */
			printf("client init\n");
			web_child(connfd); /* process request */
			exit(0);
		}
		Close(connfd); /* parent closes connected socket */
	}
}

/**************************************************************************** **
 * tcp预先派生子进程服务程序，accept无上锁保护
 **************************************************************************** **/
static int nchildren;
static pid_t *pids;

void sig_int_2(int signo)
{
	int		i;
	void	pr_cpu_time(void);

	/* 4terminate all children */
	for (i = 0; i < nchildren; i++)
		kill(pids[i], SIGTERM);
	while (wait(NULL) > 0)		/* wait for all children */
		;
	if (errno != ECHILD)
		err_sys("wait error");

	pr_cpu_time();
	exit(0);
}

void child_main(int i, int listenfd, int addrlen)
{
	int				connfd;
	void			web_child(int);
	socklen_t		clilen;
	struct sockaddr	*cliaddr;

	cliaddr = Malloc(addrlen);

	printf("child %ld starting\n", (long) getpid());
	for ( ; ; ) {
		clilen = addrlen;
		connfd = Accept(listenfd, cliaddr, &clilen);

		web_child(connfd);		/* process the request */
		Close(connfd);
	}
}

pid_t child_make(int i, int listenfd, int addrlen)
{
	pid_t	pid;

	if ( (pid = Fork()) > 0)
		return(pid);		/* parent */

	child_main(i, listenfd, addrlen);	/* never returns */
	return pid;
}

int model_serv02(char *hostname, char *service, int children_num)
{
	int			listenfd, i;
	socklen_t	addrlen;
	void		sig_int_2(int);
	pid_t		child_make(int, int, int);

	listenfd = Tcp_listen(hostname, service, &addrlen);

	nchildren = children_num;
	pids = Calloc(nchildren, sizeof(pid_t));

	for (i = 0; i < nchildren; i++)
		pids[i] = child_make(i, listenfd, addrlen);	/* parent returns */

	Signal(SIGINT, sig_int_2);

	for ( ; ; )
		pause();	/* everything done by children */
}

/**************************************************************************** **
 * tcp预先派生子进程服务程序，accept使用锁保护
 **************************************************************************** **/
// accept使用文件上锁保护
static struct flock	lock_it, unlock_it;/*{{{*/
static int lock_fd = -1;

/* fcntl() will fail if my_lock_init() not called */
void my_lock_init(char *pathname)
{
    char	lock_file[1024];

	/* 4must copy caller's string, in case it's a constant */
    strncpy(lock_file, pathname, sizeof(lock_file));
    lock_fd = Mkstemp(lock_file);

    Unlink(lock_file);			/* but lock_fd remains open */

	lock_it.l_type = F_WRLCK;
	lock_it.l_whence = SEEK_SET;
	lock_it.l_start = 0;
	lock_it.l_len = 0;

	unlock_it.l_type = F_UNLCK;
	unlock_it.l_whence = SEEK_SET;
	unlock_it.l_start = 0;
	unlock_it.l_len = 0;
}

void my_lock_wait()
{
	int		rc;

	while ( (rc = fcntl(lock_fd, F_SETLKW, &lock_it)) < 0) {
		if (errno == EINTR)
			continue;
		else
			err_sys("fcntl error for my_lock_wait");
	}
}

void my_lock_release()
{
	if (fcntl(lock_fd, F_SETLKW, &unlock_it) < 0)
		err_sys("fcntl error for my_lock_release");
}/*}}}*/

// accept使用线程上锁保护
static pthread_mutex_t	*mptr;	/* actual mutex will be in shared memory *//*{{{*/

void my_th_lock_init(char *pathname)
{
	int		fd;
	pthread_mutexattr_t	mattr;

	fd = Open("/dev/zero", O_RDWR, 0);

	mptr = Mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
				MAP_SHARED, fd, 0);
	Close(fd);

	Pthread_mutexattr_init(&mattr);
	Pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	Pthread_mutex_init(mptr, &mattr);
}

void my_th_lock_wait()
{
	Pthread_mutex_lock(mptr);
}

void my_th_lock_release()
{
	Pthread_mutex_unlock(mptr);
}/*}}}*/

void child_main_with_lock(int i, int listenfd, int addrlen)
{
	int				connfd;
	void			web_child(int);
	socklen_t		clilen;
	struct sockaddr	*cliaddr;

	cliaddr = Malloc(addrlen);

	printf("child %ld starting\n", (long) getpid());
	for ( ; ; ) {
		clilen = addrlen;
		my_lock_wait();
		connfd = Accept(listenfd, cliaddr, &clilen);
		my_lock_release();

		web_child(connfd);		/* process the request */
		Close(connfd);
	}
}


int model_serv02_with_lock(char *hostname, char *service, int children_num)
{
	int			listenfd, i;
	socklen_t	addrlen;
	void		sig_int_2(int);
	pid_t		child_make(int, int, int);

	listenfd = Tcp_listen(hostname, service, &addrlen);

	nchildren = children_num;
	pids = Calloc(nchildren, sizeof(pid_t));

	my_lock_init("/tmp/lock.XXXXXX");    //one lock file for all children
	for (i = 0; i < nchildren; i++)
		pids[i] = child_make(i, listenfd, addrlen);	/* parent returns */

	Signal(SIGINT, sig_int_2);

	for ( ; ; )
		pause();	/* everything done by children */
}

/**************************************************************************** **
 * tcp预先派生子进程服务程序，传递描述符
 * server/child.h
 * server/child05.c
 * server/serv05.c
 **************************************************************************** **/

/**************************************************************************** **
 * tcp并发服务器程序，每个客户一个线程
 * server/serv06.c
 **************************************************************************** **/

/**************************************************************************** **
 * tcp预先创建线程服务器程序，每个线程各自accept
 * server/pthread07.h
 * server/pthread07.c
 * server/serv07.c
 **************************************************************************** **/

/**************************************************************************** **
 * tcp预先创建线程服务器程序，主线程统一accept
 * server/pthread08.h
 * server/pthread08.c
 * server/serv08.c
 **************************************************************************** **/

